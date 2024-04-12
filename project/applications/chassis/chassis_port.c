/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 21:52:49
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-03-24 16:34:07
 * @FilePath: \project\applications\chassis\chassis_port.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "chassis_port.h"
#include "chassis_module_mai.h"
#include "abus_topic.h"

#include <rtthread.h>
#include <rtdbg.h>
#define DBG_TAG    "Chassis.port"
#define DBG_LVL               DBG_LOG

chassis_t chassis_mai;

chassis_speed_t chassis_speed;
chassis_pos_t chassis_pos;


// 0.55 正中间 400 90度
void chassis_port_handle(void *parameter)
{
    // int chassis_set_speed(chassis_t *chassis, chassis_speed_t *data);
    // int chassis_set_pos(chassis_t *chassis, chassis_pos_t *data);
    // chassis_speed.x_m_s = 10;
    chassis_speed.y_m_s = 0;
    //chassis_speed.z_rad_s = 10;

    // chassis_pos.x_m = 10;
    // chassis_pos.y_m = 0.55;
    chassis_pos.z_rad = 0;
    // chassis_set_speed(&chassis_mai, &chassis_speed);


    chassis_set_pos(&chassis_mai, &chassis_pos);
    while (1)
    {
#if defined(CHASSIS_MODULE_MAI) && defined(CHASSIS_MODULE_MAI)

        chassis_handle(&chassis_mai, 0);
#endif

        rt_thread_mdelay(50);
    }
}
int chassis_sub_callback(abus_topic_t *sub)
{
    chassis_ctrl_t ctrl;
    uint16_t size;
    size = afifo_out_data(sub->datafifo, (uint8_t*)&ctrl, sizeof(chassis_ctrl_t));
    if (size!=sizeof(chassis_ctrl_t))
    {
        LOG_E("abus_topic_subscribe  afifo_out_data error\n");
        return -1;
    }
    if (ctrl.type == 0)
    {
        //LOG_D("speed x:%f y:%f w:%f",ctrl.speed.x_m_s,ctrl.speed.y_m_s,ctrl.speed.z_rad_s);
        chassis_set_speed(&chassis_mai, &ctrl.speed);
    }
    else
    {
        //LOG_D("pos x:%f y:%f w:%f",ctrl.pos.x_m,ctrl.pos.y_m,ctrl.pos.z_rad);
        chassis_set_pos(&chassis_mai, &ctrl.pos);
    }
    return 0;
}

int chassis_port_init(void)
{
#if defined(CHASSIS_MODULE_MAI) && defined(CHASSIS_MODULE_MAI)
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
