/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-14 21:58:17
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-15 01:10:17
 * @FilePath: \project\applications\carMove.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "drv_visual.h"
#include <string.h>
#include <stdio.h>
#include "chassis.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"
#include "chassis.h"

#define center_x 0.500f
#define center_y 0.500f
#define deadZone 0.0001f

#define myabs(x) ((x) < 0) ? (0 - (x)) : ((x) - 0)


extern chassis_t chassis_mai;
static  chassis_speed_t chassis_speed_move;

void SetcarmoveX(cvdat *ball, float Kp, chassis_speed_t *data)
{
	float err = ball->posX - center_x;
	if (myabs(err) < deadZone)
		err = 0.00f;
	data->x_m_s = Kp * (err);
}

void SetcarmoveY(cvdat *ball, float Kp, chassis_speed_t *data)
{
	float err = ball->posY - center_y;
	if (myabs(err) < deadZone)
		err = 0.00f;
	data->y_m_s = Kp * (err);
}

void SetCarmove(cvdat *ball, float Kpx, chassis_speed_t *data, float Kpy, chassis_t *chassis)
{
	SetcarmoveX(ball, Kpx, data);
	SetcarmoveY(ball, Kpy, data);
	LOG_D("%lf\r\n", data->x_m_s);
	//	LOG_D("%f,%f\r\n",ball[1].posX,ball[1].posY);
	data->z_rad_s=0;
	chassis_set_speed(chassis, data);
}

void carMove_entry(void *parameter)
{
	uint8_t i = 0;
	while (1)
	{
//		chassis_speed_move.x_m_s=0.3;
//		chassis_speed_move.y_m_s=0.0;
//		chassis_speed_move.z_rad_s=0;
//		
//		chassis_set_speed(&chassis_mai, &chassis_speed_move);
		rt_thread_mdelay(100);
	}
}

int carMove_init(void)
{
	 rt_thread_t test1 = rt_thread_create("car_move",
							 carMove_entry, RT_NULL,
							 4096, 15,
							 1);
	if (test1 != NULL) //
	{
		rt_thread_startup(test1);
	}
	return 0;
}

INIT_APP_EXPORT(carMove_init); //
