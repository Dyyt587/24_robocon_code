#include "drv_visual.h"
#include <string.h>
#include <stdio.h>
#include "chassis.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"
#include "chassis.h"
#include <rtthread.h>
#include <rtdevice.h>  //使用 RT-Thread 的设备需要包含此头文件

#define center_x 0.500f
#define center_y 0.500f
#define deadZone 0.0001f

#define myabs(x) ((x)<0)?(0-(x)):((x)-0)


//extern chassis_t chassis_mai;
//extern chassis_speed_t chassis_speed;

//void SetcarmoveX(cvdat *ball,float Kp,chassis_speed_t *data)
//{
//	float err=ball->posX-center_x;
//	if(myabs(err)<deadZone)
//		err=0.00f;
//	data->x_m_s=Kp*(err);
//}

//void SetcarmoveY(cvdat *ball,float Kp,chassis_speed_t *data)
//{
//	float err=ball->posY-center_y;
//	if(myabs(err)<deadZone)
//		err=0.00f;
//	data->y_m_s=Kp*(err);
//}

//void SetCarmove(cvdat *ball,float Kpx,chassis_speed_t *data,float Kpy,chassis_t *chassis)
//{
//	SetcarmoveX(ball,Kpx,data);
//	SetcarmoveY(ball,Kpy,data);
//	LOG_D("%lf\r\n",data->x_m_s);
////	LOG_D("%f,%f\r\n",ball[1].posX,ball[1].posY);
//	chassis_set_speed(chassis,data);
//}


///* step 3、如果不再使用该通道，可以关闭 PWM 通道的输出 */
//    rt_pwm_disable(pwm_dev, PWM_DEV_CHANNEL);


#define PWM_DEV_NAME            "pwm4"  /* PWM设备名称 */
#define PWM_DEV_CHANNEL         1       /* PWM通道 */
#define THREAD_PRIORITY         15      /* 线程优先级 */
#define THREAD_STACK_SIZE       512     /* 线程栈大小 */
#define THREAD_TIMESLICE        1       /* 线程时间片大小 */

static rt_thread_t tid1 = RT_NULL;      /* 线程句柄 */
struct rt_device_pwm *pwm_dev;          /* PWM设备句柄 */
//static rt_uint32_t period = 500000;     /* 周期为0.5ms，单位为纳秒ns */
static rt_uint32_t period = 20000000;     /* 周期为0.02s，单位为纳秒ns */
static rt_uint32_t pulse = 1000000;           /* PWM脉冲宽度值的增减方向 */



//设置对应角度2500000对应最大角度
//500000对应最小角度
void setAngle(float angle)
{
	 /* step 1.2、设置 PWM 周期和脉冲宽度 */
	rt_pwm_set(pwm_dev,PWM_DEV_CHANNEL,period,500000+angle*7407);
}


/* 线程 pwm_entry 的入口函数 */
static void pwm_entry(void *parameter)
{
    rt_uint32_t count = 20;
		
//		while(1)
//		{
//			setAngle(count++)
//			rt_thread_mdelay(5);
//		}
    while (1)
    {	
				rt_pwm_set(pwm_dev,PWM_DEV_CHANNEL,period,2500000);
//				setAngle(count);
				rt_thread_mdelay(5);
    }
    
}

int pwm_test()
{
    /* step 1.1、查找 PWM 设备 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return RT_ERROR;
    }

    /* step 1.2、设置 PWM 周期和脉冲宽度默认值 */
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 1000000);
    /* step 1.3、使能 PWM 设备的输出通道 */
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);

    /* 创建线程，名称是 pwm_thread ，入口是 pwm_entry*/
    tid1 = rt_thread_create("pwm_thread",
                             pwm_entry,
                             RT_NULL,
                             THREAD_STACK_SIZE,
                             THREAD_PRIORITY,
                             THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    return RT_EOK;
}
/* 导出到 msh 命令列表中 */
INIT_APP_EXPORT(pwm_test);










