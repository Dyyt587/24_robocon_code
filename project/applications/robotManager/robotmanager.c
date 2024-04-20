#include <rtthread.h>
#define DBG_TAG "RobotManager"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "robotManager.h"
#include "abus_topic.h"
#include "chassis_port.h"
#include "abus_topic.h"
#include "math.h"
// #include "PathFinder.h"
#include "aboard_power_switch.h"
#include "drv_visual.h"
#include "bus_sbus.h"

#define PWM_DEV_NAME "pwm4" /* PWM设备名称 */
#define PWM_DEV_CHANNEL 1   /* PWM通道 */

struct rt_device_pwm *pwm_dev; /* PWM设备句柄 */

static rt_uint32_t period = 20000000; /* 周期为20ms，单位为纳秒ns */
static rt_uint32_t pulse = 500000;    /* PWM脉冲宽度值 */

extern abus_accounter_t rbmg_error_acc;         // 接收error
extern abus_accounter_t rbmg_dir_acc;           // 发布dir
extern abus_accounter_t rbmg_special_point_acc; // 接收special point
extern abus_accounter_t rbmg_chassis_acc;       // 发布chassis ctrl

uint8_t rbmg_mode = CAB_MODE;
uint8_t chassis_dir = 0; // 车辆前进方向，以车体坐标系为主
static float line_error = 0;
static chassis_ctrl_t ctrl;

enum
{
    END = 0U,
    FORWARD,
    RIGHT,
    BACKWARD,
    LEFT,
    ROTATION,

};

enum
{
    RED,
    BLUE,
    YELLOW,
};

rt_uint8_t color_type;

/*
7 8 9
4 5 6
1 2 3
*/

uint8_t action_type;
uint8_t now_dir = 1;  // 当前方向
uint8_t next_dir = 1; // 下一个方向

uint8_t take_cnt = 1;
uint8_t red_cnt;
uint8_t blue_cnt;
uint8_t yellow_cnt;

#define HALF_CAR_WIDTH 0.09f

void setAngle(float angle)
{
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 500000 + angle * 7407);
}

void action_relative_movement_car(float _x_m, float _y_m, float _w_rad)
{
    extern chassis_t chassis_mai;
    const chassis_pos_t *nowpos = chassis_get_pos(&chassis_mai);
    // 打印电机位置
    // 发布底盘控制速度，前进
    ctrl.type = 1;
    ctrl.pos.x_m = nowpos->x_m + _x_m;
    ctrl.pos.y_m = nowpos->y_m + _y_m;
    ctrl.pos.z_rad = nowpos->z_rad + _w_rad;
    abus_public(&rbmg_chassis_acc, &ctrl);
    // LOG_D("begin relative action %f %f %f", _x_m, _y_m, _w_rad);
    // LOG_D("action begin now:%f %f %f target:%f %f %f", nowpos->x_m, nowpos->y_m, nowpos->z_rad, ctrl.pos.x_m, ctrl.pos.y_m, ctrl.pos.z_rad);
    int counter = 0;
    while (1)
    {
        // LOG_D("action begin timeout now:%f %f %f ",nowpos->x_m, nowpos->y_m, nowpos->z_rad);
        if ((fabs(nowpos->y_m - ctrl.pos.y_m) < 0.01f) && (fabs(nowpos->x_m - ctrl.pos.x_m) < 0.01f) && (fabs(nowpos->z_rad - ctrl.pos.z_rad) < 0.1f))
        {
            chassis_pos_clean(&chassis_mai);
            LOG_D("action over");
            break;
        }
        ctrl.type = 1;
        abus_public(&rbmg_chassis_acc, &ctrl);
        // LOG_D("[action]pos x:%f y:%f z:%f delta_x_m:%f delta_y_m:%f delta_w_rad:%f", nowpos->x_m, nowpos->y_m, nowpos->z_rad, _x_m, _y_m, _w_rad);
        rt_thread_mdelay(50);
        counter++;
        if (counter > 1000)
        {
            LOG_W("action error timeout now:%f %f %f target:%f %f %f", nowpos->x_m, nowpos->y_m, nowpos->z_rad, ctrl.pos.x_m, ctrl.pos.y_m, ctrl.pos.z_rad);
            break;
        }
    }
}

/**
 * @brief 巡线使用的callback,注意要支持禁用操作
 *
 * @param sub
 * @return int
 */
int rbmg_error_callback(abus_topic_t *sub)
{
// #define KK 1.1f // 1.3
#define KK 0.9f // 1.3
// #define SPEED 0.25
#define SPEED 0.25
    if (rbmg_mode == LINE_MODE)
    {
        // 巡线模式下的处理
        afifo_out_data(sub->datafifo, (uint8_t *)&line_error, sizeof(float));
        // 发布控制到底盘
        ctrl.type = 0;
        chassis_dir = 0;
        switch (chassis_dir)
        {
        case 0: // 前
            ctrl.speed.x_m_s = 0;
            ctrl.speed.y_m_s = SPEED;
            ctrl.speed.z_rad_s = line_error * KK;
            break;
        case 1: // 右边
            ctrl.speed.x_m_s = SPEED;
            ctrl.speed.y_m_s = 0;
            ctrl.speed.z_rad_s = line_error * KK;
            break;
        case 2: // 后边
            ctrl.speed.x_m_s = 0;
            ctrl.speed.y_m_s = -SPEED;
            ctrl.speed.z_rad_s = line_error * KK;
            break;
        case 3: // 左边
            ctrl.speed.x_m_s = -SPEED;
            ctrl.speed.y_m_s = 0;
            ctrl.speed.z_rad_s = line_error * KK;
            break;
        }
        //        LOG_D("line mode");
        abus_public(&rbmg_chassis_acc, &ctrl);
    }

    return 0;
}

int rbmg_dir_callback(abus_topic_t *sub)
{
    // 接收dir变换
    return 0;
}
int rbmg_special_point_callback(abus_topic_t *sub)
{
    // 接收特殊点

    /**
     * @brief 对于特殊点的处理，询问寻路器，切换然后执行动作组最后动作组交回执行权
     * 注意不在回调中执行回调仅仅切换rbmg_mode,可以发送信号量
     *
     */
    if (rbmg_mode != CAB_MODE)
    {
        rbmg_mode = ACTION_MODE;
        // LOG_D("special point! now action mode");
    }

    return 0;
}
int rbmg_chassis_ctrl_callback(abus_topic_t *sub)
{
    // 接收底盘控制数据
    return 0;
}
void action_pick(void)
{
    power_on(SWITCH_24V_1);

<<<<<<< Updated upstream
    motor_set_speed(M2006_5_CAN1, 60); // 下
	        rt_thread_mdelay(2000);
=======
    motor_set_speed(M2006_5_CAN1, 20); // 下
    rt_thread_mdelay(2000);
>>>>>>> Stashed changes

    while (1)
    {
        static float last_speed = 0;
        float speed = motor_get_speed(M2006_5_CAN1);
        LOG_D("pick speed %f lastspeed %f", speed, last_speed);
<<<<<<< Updated upstream
        if (last_speed - speed > 2 || fabs(speed)<0.001)
=======
        if (last_speed - speed > 2 || fabs(speed) < 0.001)
>>>>>>> Stashed changes
        {
            motor_set_pos(M2006_5_CAN1, motor_get_pos(M2006_5_CAN1)-360);
            return;
        }
        last_speed = speed;
        rt_thread_mdelay(20);
    }
}

void action_up(void)
{
<<<<<<< Updated upstream
    motor_set_speed(M2006_5_CAN1, -60);
		rt_thread_mdelay(2000);
=======
    motor_set_speed(M2006_5_CAN1, -30);
    rt_thread_mdelay(2000);
>>>>>>> Stashed changes

    while (1)
    {
        static float last_speed = 0;
        float speed = motor_get_speed(M2006_5_CAN1);
        LOG_D("up speed %f lastspeed %f", speed, last_speed);
<<<<<<< Updated upstream
        if (last_speed - speed < -2 || fabs(speed)<0.001)
=======
        if (last_speed - speed < -2 || fabs(speed) < 0.001)
>>>>>>> Stashed changes
        {
            motor_set_pos(M2006_5_CAN1, motor_get_pos(M2006_5_CAN1)+360);
            return;
        }
        last_speed = speed;
        rt_thread_mdelay(20);
    }
}
<<<<<<< Updated upstream
                    extern cvdat aball;

void wait1(void)
{
												static int count=0;

	while(1){
		
		#define NN 0.16
                    ctrl.type = 0;
                    ctrl.speed.x_m_s = -(aball.posX - 0.5) * 2.8f;
                    ctrl.speed.y_m_s = (aball.posY - 0.3) * 2.8f;
									
										if(ctrl.speed.x_m_s>NN)ctrl.speed.x_m_s=NN;
										if(ctrl.speed.x_m_s<-NN)ctrl.speed.x_m_s=-NN;
									
										if(ctrl.speed.y_m_s>NN)ctrl.speed.y_m_s=NN;
										if(ctrl.speed.y_m_s<-NN)ctrl.speed.y_m_s=-NN;

                    //									  ctrl.speed.x_m_s = 0.2;
                    //                    ctrl.speed.y_m_s = 0;

                    //LOG_D("ballxy carxy:%f,%f,%f,%f,", aball.posX, aball.posY, ctrl.speed.x_m_s, ctrl.speed.y_m_s);
                    ctrl.speed.z_rad_s = 0;
                    abus_public(&rbmg_chassis_acc, &ctrl);
		
	     if ((fabs(aball.posX - 0.5) < 0.07) && (fabs(aball.posY - 0.4) < 0.07))
                    {
											if(count++>10)return;
										}else{
											count=0;
										}
										rt_thread_mdelay(50);
	
	
	}
	               
									}
=======
extern cvdat aball;

void wait_for_ball_to_center(void)
{
    int count = 0;
    int cnt = 0;
#define N 50
    while (1)
    {
        if (cnt++ % 100 == 0)
            LOG_D("wait for ball to center %f %f", aball.posX, aball.posY);
        if ((fabs(aball.posX - 0.5) < 0.07) && (fabs(aball.posY - 0.4) < 0.07))
        {
            count++;
            rt_thread_mdelay(10);
        }
        else
        {
            count = 0;
        }
        if (count > N)
        {
            break;
        }
    }
}
>>>>>>> Stashed changes
void rbmg_handle(void *parameter)
{
    // rbmg_mode = ACTION_MODE;
    // action_front_car(0.107f);
    while (1)
    {
        // LOG_D("rbmg he

        // 接到处理数据的消息
        const rc_info_t *rc = dbus_get_info();
        if (rbmg_mode == CAB_MODE)
        {

            /*
                校准模式，也可以用来简单测试

            */

            //             action_relative_movement_car( 0.6f,0, 0);
            // //
            // //            action_relative_movement_car( -0.6f,0, 0);
            // motor_set_pos(M2006_5_CAN1, -100);

            // motor_set_speed(M2006_1_CAN1, -100);
            //            action_relative_movement_car( 0.6f,0, 0);
            //
            //            action_relative_movement_car( -0.6f,0, 0);
<<<<<<< Updated upstream
            motor_set_speed(M2006_5_CAN1, -50);
=======
>>>>>>> Stashed changes

            // motor_set_speed(M2006_1_CAN1, -100);

            // action_relative_movement_car( 0.f,0.1f, 0);
            //					rt_thread_mdelay(1000);
            //

            // action_relative_movement_car( 0,0.5f, 0);
            //            while(1){
            //            motor_set_pos(M2006_5_CAN1,-600);
            //						                    rt_thread_mdelay(1500);
            //
            //
            //            motor_set_pos(M2006_5_CAN1,-300);
            //						                    rt_thread_mdelay(1000);
            //							rt_thread_mdelay(1000);
            //            motor_set_pos(M2006_5_CAN1,-600);
            //														rt_thread_mdelay(1000);

            //            }
            // rbmg_mode = LINE_MODE;
            // motor_set_speed(M2006_5_CAN1, -1500);
            // rt_thread_mdelay(6000);

            LOG_D("cab mode wait lifting ready");
            action_up();
            LOG_D("lifting is ready");
            while (1)
            {

                LOG_D("cab mode");
                if (rbmg_mode != CAB_MODE)
                {

                    break;
                }

                while (1)
                {

<<<<<<< Updated upstream

                    rt_thread_mdelay(50);

									wait1();

                        // 停车
//                        ctrl.type = 1;
//                        ctrl.pos.x_m = 0;
//                        ctrl.pos.y_m = 0;
//                        abus_public(&rbmg_chassis_acc, &ctrl);
											
												action_relative_movement_car(0,0,0);
												
												setAngle(125.0f);//放下吸盘
												rt_thread_mdelay(1000);
                        action_pick();
                        rt_thread_mdelay(2000);
												
												
												
                        action_up();
											
					              // motor_set_pos(M2006_5_CAN1, 0);
												//setAngle(12.0f);//放下吸盘
												//action_relative_movement_car(0,0,3.14);
												 ctrl.type = 1;
                        ctrl.pos.x_m = 0.1;
                        ctrl.pos.y_m = 1;
                        abus_public(&rbmg_chassis_acc, &ctrl);
												
                        while(1){
                            motor_set_pos(M2006_5_CAN1, motor_get_pos(M2006_5_CAN1));
                            rt_thread_mdelay(1000);
                            LOG_D("action_over");
                        }
=======
                    ctrl.type = 0;
                    ctrl.speed.x_m_s = -(aball.posX - 0.5) * 2.f;
                    ctrl.speed.y_m_s = (aball.posY - 0.5) * 2.f;

                    if (ctrl.speed.x_m_s > 0.12)
                        ctrl.speed.x_m_s = 0.12;
                    if (ctrl.speed.x_m_s < -0.12)
                        ctrl.speed.x_m_s = -0.12;

                    if (ctrl.speed.y_m_s > 0.12)
                        ctrl.speed.y_m_s = 0.12;
                    if (ctrl.speed.y_m_s < -0.12)
                        ctrl.speed.y_m_s = -0.12;

                    //									  ctrl.speed.x_m_s = 0.2;
                    //                    ctrl.speed.y_m_s = 0;

                    LOG_D("ballxy carxy:%f,%f,%f,%f,", aball.posX, aball.posY, ctrl.speed.x_m_s, ctrl.speed.y_m_s);
                    ctrl.speed.z_rad_s = 0;
                    abus_public(&rbmg_chassis_acc, &ctrl);

                    rt_thread_mdelay(50);

                    wait_for_ball_to_center();
                    // 停车
                    //                        ctrl.type = 1;
                    //                        ctrl.pos.x_m = 0;
                    //                        ctrl.pos.y_m = 0;
                    //                        abus_public(&rbmg_chassis_acc, &ctrl);
>>>>>>> Stashed changes

                    action_relative_movement_car(0, 0, 0);

                    setAngle(125.0f); // 放下吸盘
                    rt_thread_mdelay(1000);
                    action_pick();
                    rt_thread_mdelay(2000);

                    action_up();

                    // motor_set_pos(M2006_5_CAN1, 0);
                    // setAngle(12.0f);//放下吸盘
                    // action_relative_movement_car(0,0,3.14);
                    ctrl.type = 1;
                    ctrl.pos.x_m = 0;
                    ctrl.pos.y_m = 0;
                    abus_public(&rbmg_chassis_acc, &ctrl);
                    while (1)
                    {
                        motor_set_pos(M2006_5_CAN1, motor_get_pos(M2006_5_CAN1));
                        rt_thread_mdelay(1000);
                        LOG_D("action_over");
                    }

                    //                        // 完成一次抓取,向右平移到下一个球
                    //                        action_relative_movement_car(-0.6, 0, 0);

                    // action_relative_movement_car(-0.1,0,0);
                    // rt_thread_mdelay(1000);
                

                rt_thread_mdelay(500);
            }
        }
        else if (rbmg_mode == LINE_MODE)
        {
            // 巡线都在回调中处理
            // LOG_D("line mode");
        }
        else if (rbmg_mode == ACTION_MODE)
        {
            // 动作模式下的处理
            // 完成动作后切换回巡线模式
            LOG_D("action start");
            // action_relative_movement_car(0, 0.135f, 0);
            // action_relative_movement_car(0, 0, 3.14f/2.0f);

            /* 更新寻路器*/
            now_dir = next_dir;
            //            next_dir = Path_get_next_dir(this_table);
            LOG_D("now dir %d next dir %d", now_dir, next_dir);
            // 判断当前是否寻路完成进行切换或者特殊action
            // if (now_dir == 0)
            //     while (1)
            //     {
            //         LOG_D("finder end");
            //         rt_thread_mdelay(500);
            //     }

            // 转弯或特殊action
            //   turn_actions(now_dir, next_dir);
            rbmg_mode = LINE_MODE;
            LOG_D("action completion");
        }
        rt_thread_mdelay(50);
    }
}

int rbmg_init(void)
{
    /* step 1.1、查找 PWM 设备 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return RT_ERROR;
    }

    /* step 1.2、设置 PWM 周期和脉冲宽度默认值 */
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 1900000); // 20ms，初始化0度
    /* step 1.3、使能 PWM 设备的输出通道 */
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);

    rt_thread_t tid_rbmg = RT_NULL;

    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
    tid_rbmg = rt_thread_create("robotmanger",
                                rbmg_handle, RT_NULL,
                                4096*2,
                                14, 1);

    /* 线程创建成功，则启动线程 */
    if (tid_rbmg != RT_NULL)
    {
        rt_thread_startup(tid_rbmg);
    }
    return 0;
}
INIT_APP_EXPORT(rbmg_init);