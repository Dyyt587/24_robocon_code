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

#define myabs(x) ((x)<0)?(0-(x)):((x)-0)

rt_thread_t test1;

extern chassis_t chassis_mai;
extern chassis_speed_t chassis_speed;

void SetcarmoveX(cvdat *ball,float Kp,chassis_speed_t *data)
{
	float err=ball->posX-center_x;
	if(myabs(err)<deadZone)
		err=0.00f;
	data->x_m_s=Kp*(err);
}

void SetcarmoveY(cvdat *ball,float Kp,chassis_speed_t *data)
{
	float err=ball->posY-center_y;
	if(myabs(err)<deadZone)
		err=0.00f;
	data->y_m_s=Kp*(err);
}

void SetCarmove(cvdat *ball,float Kpx,chassis_speed_t *data,float Kpy,chassis_t *chassis)
{
	SetcarmoveX(ball,Kpx,data);
	SetcarmoveY(ball,Kpy,data);
	LOG_D("%lf\r\n",data->x_m_s);
//	LOG_D("%f,%f\r\n",ball[1].posX,ball[1].posY);
	chassis_set_speed(chassis,data);
}



//void test1_entry(void *parameter)
//{
//	uint8_t i=0;
//	while(1)
//	{
//	if(SSize>512)LOG_W("%d",SSize);
//    while (SSize--)
//    {
//			
//        rt_device_read(uart, 0, &ch, 1);
//        cvhandle(ch, b);
//    }
//////		LOG_D("OK\r\n");
////		SetCarmove(&ball[1],20,&chassis_speed,30,&chassis_mai);
////		rt_thread_mdelay(500);
//	}
//}

////创建动态线程
//int test1_init(void)
//{
//	test1=rt_thread_create("Test",
//													test1_entry,RT_NULL,
//													4096,15,
//													1);
//	if(test1!=NULL)//返回值正确，启动该线程
//	{
//		rt_thread_startup(test1);
//	}
//	return 0;
//}

//INIT_APP_EXPORT(test1_init);//自动初始化



