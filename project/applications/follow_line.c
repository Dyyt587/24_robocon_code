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
//int line_dir_sub_callback(abus_topic_t *sub)
//{
//	//读取数据
//	afifo_out_data(sub->fifo, &infrared_package.move_direction, sizeof(infrared_package.move_direction));
//	LOG_D("line_dir_sub_callback:%d\n", infrared_package.move_direction);
//	return 0;
//}
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

