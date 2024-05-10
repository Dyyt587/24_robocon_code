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

//extern abus_accounter_t rbmg_error_acc;         // 接收error
//extern abus_accounter_t rbmg_dir_acc;           // 发布dir
//extern abus_accounter_t rbmg_special_point_acc; // 接收special point
//extern abus_accounter_t rbmg_chassis_acc;       // 发布chassis ctrl

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
//    abus_public(&rbmg_chassis_acc, &ctrl);
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
//        abus_public(&rbmg_chassis_acc, &ctrl);
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


void rbmg_handle(void *parameter)
{
    // rbmg_mode = ACTION_MODE;
    // action_front_car(0.107f);
    while (1)
    {
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