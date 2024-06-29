#include "steer.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"


struct rt_device_pwm *pwm_steer_dev; /* PWM设 备 句 柄 */
//struct rt_device_pwm *pwm_steer_dev_right; /* PWM设 备 句 柄 */
//struct rt_device_pwm *pwm_steer_dev_last; /* PWM设 备 句 柄 */

int steer_init(void)
{
	/* 查 找 设 备 */
	pwm_steer_dev = (struct rt_device_pwm *)rt_device_find("pwm4");
	if (pwm_steer_dev == RT_NULL)
	{
			LOG_E("pwm sample run failed! can't find pwm4 device!\n");
			return RT_ERROR;
	}
	rt_pwm_set(pwm_steer_dev, 1,20000000, 500000);//周期20ms，控制0.5ms到2.5ms
	/* 使 能 设 备 */
  rt_pwm_enable(pwm_steer_dev,1);
	rt_pwm_enable(pwm_steer_dev,2);
	rt_pwm_enable(pwm_steer_dev,3);
//	rt_pwm_enable(pwm_steer_dev,4);
	return 1;
}

//id0：左，id1：右，id2：另
void steer_plate(float angle,uint8_t id)
{
	if(id==0)
		rt_pwm_set(pwm_steer_dev, 1,20000000, 500000+angle*11111);//周期20ms，控制0.5ms到2.5ms
	else if(id==1)
		rt_pwm_set(pwm_steer_dev, 2,20000000, 500000+angle*(2000000/180));//周期20ms，控制0.5ms到2.5ms
	else if(id==2)
		rt_pwm_set(pwm_steer_dev, 3,20000000, 500000+angle*11111);//周期20ms，控制0.5ms到2.5ms
}

INIT_APP_EXPORT(steer_init); //