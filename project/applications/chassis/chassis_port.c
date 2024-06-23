/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 21:52:49
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-22 22:37:07
 * @FilePath: \project\applications\chassis\chassis_port.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "chassis_port.h"
#include "chassis_module_mai.h"
#include "chassis_module_omni4.h"
#include "abus.h"
#include "abus_echo.h"

#include <rtthread.h>
#include <rtdbg.h>
#define DBG_TAG "Chassis.port"
#define DBG_LVL DBG_LOG

chassis_t chassis_mai;

chassis_speed_t chassis_speed;
chassis_pos_t chassis_pos;

int abus_subcribe_cb22(abus_subcriber_t *subcriber, void *data)
{
    chassis_ctrl_t *ctrl = (chassis_ctrl_t *)data;
    if (ctrl->type == 0) // speed
    {
        chassis_speed_t speed;
        speed.x_m_s = ctrl->x;
        speed.y_m_s = ctrl->y;
        speed.z_rad_s = ctrl->w;
//        LOG_D("sub speed x y w:%f,%f,%f", speed.x_m_s, speed.y_m_s, speed.z_rad_s);

        chassis_set_speed(&chassis_mai, &speed);
    }
    if (ctrl->type == 1) // pos
    {
        chassis_pos_t pos;
        pos.x_m = ctrl->x;
        pos.y_m = ctrl->y;
        pos.z_rad = ctrl->w;
//        LOG_D("sub pos x y w:%f,%f,%f", pos.x_m, pos.y_m, pos.z_rad);

        chassis_set_pos(&chassis_mai, &pos);
    }
    return 0;
}
chassis_state_t chassis_state;

// 0.55 正中间 400 90度
void chassis_port_handle(void *parameter)
{
    // int chassis_set_speed(chassis_t *chassis, chassis_speed_t *data);
    // int chassis_set_pos(chassis_t *chassis, chassis_pos_t *data);
    chassis_speed.x_m_s = 0.5;
    chassis_speed.y_m_s = 0;
    // chassis_speed.z_rad_s = 10;

    chassis_pos.x_m = 0.2;
    chassis_pos.y_m = 0;
    chassis_pos.z_rad = 0;
     chassis_set_speed(&chassis_mai, &chassis_speed);

    //chassis_set_pos(&chassis_mai, &chassis_pos);

    while (1)
    {
#if defined(CHASSIS_MODULE_MAI) && defined(CHASSIS_MODULE_MAI)

         chassis_handle(&chassis_mai, 0);
#endif
        chassis_state.pos_w = chassis_mai.present.pos.z_rad;
        chassis_state.pos_x = chassis_mai.present.pos.x_m;
        chassis_state.pos_y = chassis_mai.present.pos.y_m;

        chassis_state.speed_w = chassis_mai.present.speed.z_rad_s;
        chassis_state.speed_x = chassis_mai.present.speed.x_m_s;
        chassis_state.speed_y = chassis_mai.present.speed.y_m_s;

        //abus_publish("chassis_state", &chassis_state);
				//abus_topic_show("chassis_state");
	LOG_D("chassis_pos x y z:%f %f %f",chassis_mai.present.pos.x_m,chassis_mai.present.pos.y_m,chassis_mai.present.pos.z_rad);

        rt_thread_mdelay(50);
    }
}

int chassis_port_init(void)
{
//#if defined(CHASSIS_MODULE_MAI) && defined(CHASSIS_MODULE_MAI)
//    chassis_init(&chassis_mai, &ops_mai);
//#endif
    chassis_init(&chassis_mai, &ops_omni4);

    // 创建话题
    abus_topic_cfg cfg = {
        .hash_table_size = 4,
        .topic_data_size = sizeof(chassis_ctrl_t),
    };
    abus_topic_t *topic1 = abus_topic_create("chassis_ctrl", &cfg, "chassis ctrl");

    abus_topic_cfg cfg11 = {
        .hash_table_size = 4,
        .topic_data_size = sizeof(chassis_state_t),
    };
    abus_topic_t *topic111 = abus_topic_create("chassis_state", &cfg, "chassis state");

    abus_subcribe_cfg_t cfg_subx = {
        .fifo = NULL,
        .filter = NULL,
        .cb = abus_subcribe_cb22,
        .is_async = 0,
        .sem = NULL,
    };
    abus_acc_t *accx = abus_accounter_create("accx", NULL);

    // abus_subcribe("topic1", "acc1", &cfg_sub);
    abus_subcribe("chassis_ctrl", "accx", &cfg_subx);

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
