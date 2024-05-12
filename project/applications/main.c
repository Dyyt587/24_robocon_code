/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-12 10:14:08
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-10 22:10:54
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
	apid_auto_t auto_1;
	motor_t *motor = motor_get(0);
	motor_set_speed(0, 0);
	// auto_pid_init(motor->pid_speed,&auto_1, VECTOR, 200,10);
//	apid_auto_tune_ZNmode_t auto_2;
//	apid_auto_tune_ZNmode_init(motor->pid_speed,&auto_2,200);
	while (1)
	{
		motor_set_speed(0, 100);

		rt_pin_write(LED0_PIN, PIN_HIGH);
		rt_thread_mdelay(500);
		rt_pin_write(LED0_PIN, PIN_LOW);
		rt_thread_mdelay(500);
	}
}
