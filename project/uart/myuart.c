//#include "usart.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

#include <rtthread.h>
#define DBG_TAG "RobotManager"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include <board.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#include "apid.h"

#endif /* RT_USING_NANO */




apid_t pid_coordinatx;//坐标的PID
apid_t pid_coordinaty;//坐标的PID


rt_thread_t test1;
#define test1Task 128
#define test1Priority 1
#define testTimeslice 10

//球的坐标
typedef struct cvDat
{
    int id;
    float posX;
    float posY;
} cvdat;


cvdat ball[10];
//设置坐标,分别代表电机ID和电机坐标，传入的应该是目标的x
void Dj_Set_TXcoordinate( float coordinate)
{
	APID_Set_Target(&pid_coordinatx,coordinate);//设置坐标
}

//设置当前坐标,传入的应该是传回数据的x
void Dj_Set_PXcoordinate( float coordinate)
{
	APID_Set_Present(&pid_coordinatx,coordinate);
}

//设置坐标,分别代表电机ID和电机坐标，传入的应该是目标的y
void Dj_Set_TYcoordinate( float coordinate)
{
	APID_Set_Target(&pid_coordinaty,coordinate);//设置坐标
}

//设置当前坐标,传入的应该是传回数据的y
void Dj_Set_PYcoordinate(float coordinate)
{
	APID_Set_Present(&pid_coordinaty,coordinate);
}






void dj_motor_handler(uint16_t cycle,cvdat *dat)
{
		////////////更新当前球的X坐标/////////////
		Dj_Set_PXcoordinate( dat->posX);
		///////////X的PID处理/////////////////////
		APID_Hander(&pid_coordinatx,cycle);
		////////////更新当前球的Y坐标/////////////
		Dj_Set_PYcoordinate( dat->posY);
		///////////Y的PID处理/////////////////////
		APID_Hander(&pid_coordinaty,cycle);
}





//对电机的坐标PID进行参数设置
int dj_motor_init(void)
{
		APID_Init(&pid_coordinatx,PID_POSITION,3.0f,0.01f,0.01f);
		APID_Set_Out_Limit(&pid_coordinatx,30000);//输出限制
		APID_Set_Integral_Limit(&pid_coordinaty,2000);//积分限制
		APID_Set_Bias_Dead_Zone(&pid_coordinatx,20);//死区设置
		
		APID_Init(&pid_coordinaty,PID_POSITION,3.0f,0.01f,0.01f);
		APID_Set_Out_Limit(&pid_coordinaty,30000);//输出限制
		APID_Set_Integral_Limit(&pid_coordinaty,2000);//积分限制
		APID_Set_Bias_Dead_Zone(&pid_coordinaty,20);//死区设置

    return 0;
}




//该线程的入口函数
//在该线程内需要


/*
1.PID找球
2.识别到一定位置，打开气泵power_on(int id)
*/
void test1_entry(void *parameter)
{
	uint8_t i=0;
	while(1)
	{
		Dj_Set_TYcoordinate( ball[0].posY);
		Dj_Set_TXcoordinate( ball[0].posX);
		dj_motor_handler(5,&ball[0]);
	}
}


//创建动态线程
int test1_init(void)
{
	dj_motor_init();
	test1=rt_thread_create("Test",
													test1_entry,RT_NULL,
													test1Task,test1Priority,
													testTimeslice);
	if(test1!=NULL)//返回值正确，启动该线程
	{
		rt_thread_startup(test1);
	}
	return 0;
}


INIT_BOARD_EXPORT(test1_init);//自动初始化



