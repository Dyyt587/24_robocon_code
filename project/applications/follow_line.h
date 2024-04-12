/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-02 22:00:25
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-04 20:35:22
 * @FilePath: \project\applications\follow_line.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __FOLLOW_LINE_H__
#define __FOLLOW_LINE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <drv_gpio.h>
#include "statemachine.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "abus_topic.h"

#define INFRARED7_PIN    GET_PIN(F, 10)	//Q1
#define INFRARED6_PIN    GET_PIN(A, 5)	//P1
#define INFRARED5_PIN    GET_PIN(C, 5)	//O1
#define INFRARED4_PIN    GET_PIN(C, 4)	//N1

#define INFRARED3_PIN    GET_PIN(C, 0)	//N2
#define INFRARED2_PIN    GET_PIN(C, 1)	//O2
#define INFRARED1_PIN    GET_PIN(A, 4)	//P2
#define INFRARED0_PIN    GET_PIN(I, 9)	//Q2


enum direction{
	front=0,
	left,
	right,
	rotate,
};

typedef struct infrared
{
	rt_uint8_t infrared_data[8];

	uint8_t is_spacial_point_flag;
	enum direction move_direction;
}infrared;


void follow_line(void *parameter);

void Infrared_Init(void);
void GET_Infrared_Data(infrared* infrared_package);
void Print_Infrared_Data(infrared* infrared_package);

uint8_t Is_Spacial_point(infrared* infrared_package);

#ifdef __cplusplus
}
#endif

#endif
