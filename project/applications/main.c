/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-12 10:14:08
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-06-21 17:31:04
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
#include "abus.h"
#include "abus_deamon.h"
#include "chassis_port.h"

// #include "Emm_V5.h"
#include "follow_line.h"
// #include "drv_stepper_motor.h"
#include "aboard_power_switch.h"
#include "motor.h"
//#include "apid_auto_tune_Relayfeedback.h"
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


typedef struct
{
	int cnt;
} test_data_t;

abus_filter filter1;
abus_filter filter2;
abus_filter filter3;
int abus_subcribe_cb1(abus_subcriber_t *subcriber, void *data)
{
	test_data_t *data1 = (test_data_t *)data;
	printf("sub %s data cnt %d\n", subcriber->acc->name, data1->cnt);
	return 0;
}
int abus_subcribe_cb2(abus_subcriber_t *subcriber, void *data)
{
	//LOG_D("test\r\n");
	return 0;
}

test_data_t test_data1;


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
	//motor_set_pos(0, 0);

	// static CurveObjectType curve;

	// curve.targetPos = 60.f; // m

	// curve.startPos = motor_get_pos(M3508_1_CAN1); // 初始位置
	// curve.currentPos = motor_get_pos(M3508_1_CAN1);
	// // curve.targetPos = curve.targetPosm * 360.f/0.2198f; // 目标位置
	// curve.stepPos = 0.03f; // 位置变化的步长
	// // curve.PosMax = curve.max_pos;  // 最大位置限制
	// // curve.PosMin = -curve.max_pos; // 最小位置限制
	// curve.aTimes = 0;			  // 当前时间步
	// curve.maxTimes = 0;			  // 总时间步，实际使用时需要根据实际情况计算
	// curve.curveMode = CURVE_TRAP; // 使用S位置曲线
	// curve.flexible =3.f;		  // S曲线的柔性因子
	// curve.intervel = 2;

	// LOG_D("startsetpos curpos:%f,%f", curve.currentPos, motor_get_pos(M3508_1_CAN1));

	// motor_planning(&curve);
	// motor_set_pos(M3508_1_CAN1, curve.currentPos);
	// // rt_thread_mdelay(1000);
	// LOG_D("setpos curpos11:%f,%f", curve.currentPos, motor_get_pos(M3508_1_CAN1));

	// while (curve.maxTimes)
	// {
	// 	rt_thread_mdelay(2);
	// 	motor_set_pos(M3508_1_CAN1, motor_planning(&curve));
	// 	//LOG_D("setpos curpos:%f,%f", curve.currentPos, motor_get_pos(M3508_1_CAN1));
	// }

	//motor_set_pos_plan(M3508_1_CAN1, 3.1415926, 0.02f, 5.f, 0);
	//motor_set_pos_plan(M3508_4_CAN1, 3.1415926, 0.02f, 5.f, 0);
//	motor_set_pos(M3508_1_CAN1, 3.1415926);
//	motor_set_pos(M3508_2_CAN1, 3.1415926);
//	motor_set_pos(M3508_3_CAN1, 3.1415926);
//	motor_set_pos(M3508_4_CAN1, 3.1415926);


	// // 创建话题
	// abus_topic_cfg cfg = {
	// 	.hash_table_size = 16,
	// 	.topic_data_size = sizeof(test_data_t),
	// };
	// abus_topic_t *topic1 = abus_topic_create("topic1", &cfg, "just a desc");
	// abus_topic_t *topic2 = abus_topic_create("topic2", &cfg, "just a desc2");


	// // 创建一个用户
	// abus_acc_t *acc1 = abus_accounter_create("acc1", &filter1);
	// abus_acc_t *acc2 = abus_accounter_create("acc2", &filter2);
	// abus_acc_t *acc3 = abus_accounter_create("acc3", &filter3);

	// printf("sub start\n");
	// abus_subcribe_cfg_t cfg_sub = {
	// 	.fifo = NULL,
	// 	.filter = &filter1,
	// 	.cb = abus_subcribe_cb1,
	// 	.is_async = 0,
	// 	.sem = NULL,
	// };

	// // abus_subcribe("topic1", "acc1", &cfg_sub);
	// abus_subcribe("topic2", "acc2", &cfg_sub);
	// abus_subcribe("topic2", "acc3", &cfg_sub);
	// abus_subcribe("topicnull", "acc1", &cfg_sub);
	// abus_subcribe("topic1", "acc1", &cfg_sub);

	// test_data1.cnt = 10;
	// abus_publish("topic1", &test_data1);

	// printf("Hello CMake.\n");

	// ABUS_TOPIC_CREATE("topicmicro", test_data_t, 16);

	// // ABUS_SUB("acc1", "topic1", 0, abus_subcribe_cb1, NULL);

	// ABUS_SUB_FORCE("accnull", "topic1", 0, abus_subcribe_cb1, NULL, NULL);
	// printf("Hello CMake.\n");
	// ABUS_SUB("acc1", "topicmicro", 0, abus_subcribe_cb1, NULL);
	// ABUS_SUB("acc2", "topicmicro", 0, abus_subcribe_cb1, NULL);
	// abus_publish("topicmicro", &test_data1);
	// abus_publish("topicmicro", &test_data1);

	// abus_topic_show("topic1");

	// abus_sem_t sem1;
	// abus_subcribe_cfg_t cfg_sub1 = {
	// 	.fifo = NULL,
	// 	.filter = &filter1,
	// 	.cb = NULL,
	// 	.is_async = 1,
	// 	.sem = &sem1,
	// };

	// abus_subcribe("topic1", "acc1", &cfg_sub1);
	// abus_topic_show("topic1");

	// abus_publish("topic1", &test_data1);


//extern int abus_echo_test();
// abus_echo_test();




	// abus_subcribe_cfg_t cfg_sub2 = {
	// 	.fifo = NULL,
	// 	.filter = &filter1,
	// 	.cb = abus_subcribe_cb2,
	// 	.is_async = 0,
	// 	.sem = &sem1,
	// };

	// abus_subcribe("chassis_state", "acc2", &cfg_sub2);
	// abus_topic_show("chassis_state");




	chassis_ctrl_t ctrl;
	ctrl.type=0;
	ctrl.x=1;
	ctrl.y=1;
	ctrl.w=1;
	//abus_deamon_start();
	//motor_set_pos(M3508_7_CAN1, 3.1415926/2);
		motor_set_speed(M3508_5_CAN1, -400);

	//	motor_set_pos_plan(M3508_7_CAN1, 3.1415926/8, 0.02f, 5.f, 0);

//extern  float pos_x ;
//extern  float pos_y ;
//extern  float zangle ;
//	while (1)
//	{
//LOG_D("x y z %f %f %f",pos_x,pos_y,zangle);
//		rt_thread_mdelay(20);
//	}
	///////////////////////////////////////////////////
	while (1)
	{
			//motor_set_pos(M3508_7_CAN1, 3.1415926*2);

		//abus_publish("chassis_ctrl", &ctrl);
		// motor_set_pos(	M3508_1_CAN1, 100);
		rt_pin_write(LED0_PIN, PIN_HIGH);
		rt_thread_mdelay(500);
		rt_pin_write(LED0_PIN, PIN_LOW);
		rt_thread_mdelay(500);
	}
}
