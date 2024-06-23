#include "follow_line.h"
#include "apid.h"
#include "apid.h"
#include "abus.h"
#include "chassis_port.h"
#include "abus_port.h"
infrared infrared_package;

float error;
int asub_callback2(abus_topic_t *sub)
{
	LOG_D("sub_callback2\n");
	return 0;
}

int asub_callback(abus_topic_t *sub)
{
	LOG_D("sub_callback\n");
	return 0;
}

int follow_line_init(void)
{
	rt_thread_t tid_follow_line = RT_NULL;
#define THREAD_PRIORITY_FOLLOW_LINE 25
#define THREAD_STACK_SIZE_FOLLOW_LINE 1024
#define THREAD_TIMESLICE_FOLLOW_LINE 5
	/* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
	tid_follow_line = rt_thread_create("follow_line",
									   follow_line, RT_NULL,
									   THREAD_STACK_SIZE_FOLLOW_LINE,
									   THREAD_PRIORITY_FOLLOW_LINE, THREAD_TIMESLICE_FOLLOW_LINE);

	/* 线程创建成功，则启动线程 */
	if (tid_follow_line != RT_NULL)
	{
		rt_thread_startup(tid_follow_line);
	}

	return 0;
}
INIT_COMPONENT_EXPORT(follow_line_init);

//在其他线程中进行扫描，得到状态(根据不同的识别巡线)，这个里面需要有一个球筐的判断，进行球的放
void line_car(void)
{
	switch(infrared_package.move_direction)
	{
		case front:
		{
			
			break;
		}
		case left:
		{
			
			break;
		}
		case right:
		{
			
			break;
		}
		case rotate:
		{
			
			break;
		}
	}
}

//找球的逻辑（需要一个零点坐标系，根据激光测距传感器来获取零点坐标系）
void back_zero()
{
	uint8_t state=0;
	switch(state)//找球，抓球，然后回归零点
	{
		case 1:
		{
			
			break;
		}
		case 2:
		{
			
			break;
		}
		
		
	}
	
}


void follow_line(void *parameter)
{
	infrared_package.move_direction = front;

	chassis_ctrl_t ctrl;
	rt_thread_mdelay(1000);
	while (1)
	{

		   
		rt_thread_mdelay(15);

	}
}

