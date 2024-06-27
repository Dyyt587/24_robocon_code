/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 21:52:49
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-11 11:47:21
 * @FilePath: \project\applications\chassis\chassis_port.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "chassis_port.h"
#include "chassis_module_mai.h"
//#include "abus_topic.h"

#include <rtthread.h>
#include <rtdbg.h>
#define DBG_TAG "Chassis.port"
#define DBG_LVL DBG_LOG

chassis_t chassis_mai;

chassis_speed_t chassis_speed;
chassis_pos_t chassis_pos;

static CurveObjectType plan_mai;
float sinx = 0;
float cosx = 1;
int chassis_port_plan_init(struct chassis *chassis, chassis_data_t *present, chassis_data_t *target)
{
    LOG_D("chassi planning init");

    float delta_x = target->pos.x_m - present->pos.x_m;
    float delta_y = target->pos.y_m - present->pos.y_m;

    float pos_delta = sqrtf(delta_x * delta_x + delta_y * delta_y);
    cosx = delta_x / pos_delta;
    sinx = delta_y / pos_delta;


    plan_mai.targetPos = pos_delta;
    plan_mai.startPos = sqrtf(present->pos.x_m*present->pos.x_m + present->pos.y_m*present->pos.y_m); // 初始位置
    plan_mai.currentPos = plan_mai.startPos;
    plan_mai.stepPos = 0.002f;       // 位置变化的步长，单位m
//    plan_mai.PosMax = FLT_MAX;       // 最大位置限制
//    plan_mai.PosMin = -FLT_MAX;      // 最小位置限制
    plan_mai.aTimes = 0;             // 当前时间
    plan_mai.maxTimes = 0;           // 总时间，设置为0，自动计算,时间单位为ms
    plan_mai.curveMode = CURVE_SPTA; // 使用S型曲线
    plan_mai.flexible = 10.f;        // S曲线的柔性因子
    plan_mai.intervel = 20;          // 调用间隔，单位为ms
    motor_planning(&plan_mai);     

    return 0;
}

int chassis_port_plan(struct chassis *chassis, chassis_data_t *outdata)
{
    // LOG_D("chassis planning");
    if (plan_mai.maxTimes) /*确保规划还在继续*/
    {
        chassis->plan.is_planning = 1;
        motor_planning(&plan_mai);
        outdata->pos.x_m = plan_mai.currentPos * cosx;
        outdata->pos.y_m = plan_mai.currentPos * sinx;
        LOG_D(":%f,%f", outdata->pos.x_m, outdata->pos.y_m);
    }
    else
    {
        chassis->plan.is_planning = 0;
    }
}

chassis_pos_t chassis_pos1;
// 0.55 正中间 400 90度
void chassis_port_handle(void *parameter)
{
	static int time=0;
	rt_thread_mdelay(2000);
    // int chassis_set_speed(chassis_t *chassis, chassis_speed_t *data);
    // int chassis_set_pos(chassis_t *chassis, chassis_pos_t *data);
    chassis_speed.x_m_s = 0.0f;
    chassis_speed.y_m_s = 0.0f;
    chassis_speed.z_rad_s = 0.0;

    chassis_pos.x_m = 0.0f;
    chassis_pos.y_m = 0.0f;
    chassis_pos.z_rad = 3.1415926*1.0f;
	chassis_pos1.x_m=chassis_get_pos(&chassis_mai)->x_m;
	chassis_pos1.y_m=chassis_get_pos(&chassis_mai)->y_m;
	chassis_pos1.z_rad=chassis_get_pos(&chassis_mai)->z_rad;
//        chassis_set_speed(&chassis_mai, &chassis_speed);

//    chassis_set_pos_plan(&chassis_mai, &chassis_pos);
		chassis_set_pos(&chassis_mai, &chassis_pos);
    while (1)
    {
#if defined(CHASSIS_MODULE_MAI) && defined(CHASSIS_MODULE_MAI)

			
        chassis_handle(&chassis_mai, 0);
				chassis_pos1.x_m=chassis_get_pos(&chassis_mai)->x_m;
	chassis_pos1.y_m=chassis_get_pos(&chassis_mai)->y_m;
	chassis_pos1.z_rad=chassis_get_pos(&chassis_mai)->z_rad;
			if(time++%2==0)LOG_D("pos:%f,%f,%f",chassis_pos1.x_m,chassis_pos1.y_m,chassis_pos1.z_rad);
#endif

        rt_thread_mdelay(20);
    }
}


int chassis_port_init(void)
{
#if defined(CHASSIS_MODULE_MAI) && defined(CHASSIS_MODULE_MAI)
    ops_mai.plan_init = chassis_port_plan_init;
    ops_mai.plan = chassis_port_plan;
    chassis_init(&chassis_mai, &ops_mai);
#endif

    rt_thread_t tid_chassis = RT_NULL;

    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
    tid_chassis = rt_thread_create("chassis_mai",
                                   chassis_port_handle, RT_NULL,
                                   4096,
                                   22, 1);

    /* 线程创建成功，则启动线程 */
    if (tid_chassis != RT_NULL)
    {
        rt_thread_startup(tid_chassis);
    }
    return 0;
}
INIT_ENV_EXPORT(chassis_port_init);
