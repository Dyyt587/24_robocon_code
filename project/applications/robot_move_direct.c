/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-04 09:55:33
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-04 16:44:19
 * @FilePath: \construction_robot\project\applications\robot_move_direct.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "robot_move_direct.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"
#include "chassis.h"

#define DBG_TAG "robot_move_direct"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>


extern float pos_x  ;
extern float pos_y  ;
extern float zangle  ;
extern float xangle  ;
extern float yangle  ;
extern float w_z  ;
extern chassis_t chassis_mai;
void Robot_Move_Direct_Process(void *parameter)
{
    while(1)
    {
				//LOG_D("pos_x=%f pos_y=%f zangle=%f w_z=%f",pos_x,pos_y,zangle,w_z);
				//LOG_D("offset_pos_x=%f offset_pos_y=%f offset_z=%f",chassis_mai.offset.pos.x_m,chassis_mai.offset.pos.y_m,chassis_mai.offset.pos.z_rad);
        rt_thread_delay(10);
    }
}

int Robot_Move_Direct_Init(void)
{
	rt_thread_t fsus_thread = RT_NULL;
	// // 创建信号量
	
    
	// 创建线程
	fsus_thread = rt_thread_create("drv_fsus",
								   Robot_Move_Direct_Process, NULL,
								   1024,
								   25, 1);

	if (fsus_thread != RT_NULL)
	{
		rt_thread_startup(fsus_thread);
	}

	return 0;
}
INIT_APP_EXPORT(Robot_Move_Direct_Init);