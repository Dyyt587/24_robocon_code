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


rt_thread_t test1;
#define test1Task 4096
#define test1Priority 15
#define testTimeslice 1

/*
1.PID找球
2.识别到一定位置，打开气泵power_on(int id)
*/
void test1_entry(void *parameter)
{
	while(1)
	{
		
	}
}


//创建动态线程
int chassis_init(void)
{
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
