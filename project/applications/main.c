/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-12 10:14:08
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-14 15:46:53
 * @FilePath: \project\applications\main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2023-12-03     Meco Man     support nano version
 */

#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */

#include "ulog.h"
// #include "Emm_V5.h"
#include "follow_line.h"
// #include "drv_stepper_motor.h"
#include "aboard_power_switch.h"
#include "motor.h"
#include "apid_auto_tune_Relayfeedback.h"
#include "apid_auto_tune_ZNmode.h"
#include "Trajectory_planning.h"

/* defined the LED_G pin: PF14 */
#define LED0_PIN GET_PIN(F, 14)

#define ADC_DEV_NAME "adc1"	   /* ADC 设备名称 */
#define ADC_DEV_CHANNEL 18	   /* ADC 通道 */
#define REFER_VOLTAGE 330	   /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS (1 << 12) /* 转换位数为12位 */

#define THREAD_PRIORITY_FOLLOW_LINE 25
#define THREAD_STACK_SIZE_FOLLOW_LINE 1024
#define THREAD_TIMESLICE_FOLLOW_LINE 5

#define THREAD_PRIORITY_CHASSIS 25
#define THREAD_STACK_SIZE_CHASSIS 1024
#define THREAD_TIMESLICE_CHASSIS 5
void motor_plan_start(CurveObjectType *curve, float target, float start, float stepPos, float flexible, uint32_t max_time)
{
}


void motor_plan_init(motor_t*motor,CurveObjectType *curve, float target, float start, float stepPos, float flexible, uint32_t max_time)
{
}
int main(void)
{
	// //////////////////////////////////////////////////////////////创建红外循迹线程
	// 	  rt_thread_t tid_follow_line = RT_NULL;

	//     /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
	//   tid_follow_line = rt_thread_create("follow_line",
	//               follow_line, RT_NULL,
	//               THREAD_STACK_SIZE_FOLLOW_LINE,
	//               THREAD_PRIORITY_FOLLOW_LINE, THREAD_TIMESLICE_FOLLOW_LINE);

	//   /* 线程创建成功，则启动线程 */
	//   if (tid_follow_line != RT_NULL)
	//   {
	//     rt_thread_startup(tid_follow_line);
	//   }
	//////////////////////////////////////////////////////////////创建步进电机线程

	//////////////////////////////////////////////////////////////创建底盘运动线程

	//////////////////////////////////////////////////////////////
	/* set LED0 pin mode to output */
	rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

	power_on(SWITCH_24V_1);
	power_on(SWITCH_24V_2);
	power_off(SWITCH_24V_3);
	power_on(SWITCH_24V_4);
	power_on(SWITCH_5V_1);

	power_off(SWITCH_24V_4);

	// 	rt_adc_device_t adc_dev;
	// 	rt_uint32_t value;
	// float 	vol;
	// 	rt_err_t ret = RT_EOK;
	// 	rt_uint8_t count =0;
	// rt_thread_mdelay(2000);
	// apid_auto_t auto_1;
	// motor_t *motor = motor_get(0);
	// motor_set_speed(0, 100);
	// apid_auto_tune_ZNmode_t auto_2;
	// apid_auto_tune_ZNmode_init(motor->pid_speed, &auto_2,ZNModeLessOvershoot,300,-20000,20000, 20);
	// motor_set_speed(0, 0);

	// motor_t *motor = motor_get(0);
	// apid_auto_tune_ZNmode_t auto_2;
	// apid_auto_tune_ZNmode_init(motor->pid_pos, &auto_2,ZNModeLessOvershoot,300,-300*19,300*19, 20);
	// motor_set_speed(0, 0);

	/////////////////////////////////////////////////////////
	// 使用示例
	motor_set_pos(0, 0);

	static CurveObjectType curve;

	curve.targetPos = 60.f; // m

	curve.startPos = motor_get_pos(M3508_1_CAN1); // 初始位置
	curve.currentPos = motor_get_pos(M3508_1_CAN1);
	// curve.targetPos = curve.targetPosm * 360.f/0.2198f; // 目标位置
	curve.stepPos = 0.050f; // 位置变化的步长
	curve.max_pos = 1000.0f;
	// curve.PosMax = curve.max_pos;  // 最大位置限制
	// curve.PosMin = -curve.max_pos; // 最小位置限制
	curve.aTimes = 0;			  // 当前时间步
	curve.maxTimes = 0;			  // 总时间步，实际使用时需要根据实际情况计算
	curve.curveMode = CURVE_TRAP; // 使用S位置曲线
	curve.flexible = 4.5f;		  // S曲线的柔性因子
	curve.intervel = 10;

	LOG_D("startsetpos curpos:%f,%f", curve.currentPos, motor_get_pos(M3508_1_CAN1));

	motor_planning(&curve);
	motor_set_pos(M3508_1_CAN1, curve.currentPos);
	// rt_thread_mdelay(1000);
	LOG_D("setpos curpos11:%f,%f", curve.currentPos, motor_get_pos(M3508_1_CAN1));

	while (curve.maxTimes)
	{
		rt_thread_mdelay(10);
		motor_set_pos(M3508_1_CAN1, motor_planning(&curve));
		LOG_D("setpos curpos:%f,%f", curve.currentPos, motor_get_pos(M3508_1_CAN1));
	}
	///////////////////////////////////////////////////
	while (1)
	{
		// motor_set_pos(	M3508_1_CAN1, 100);
		rt_pin_write(LED0_PIN, PIN_HIGH);
		rt_thread_mdelay(500);
		rt_pin_write(LED0_PIN, PIN_LOW);
		rt_thread_mdelay(500);
	}
}
