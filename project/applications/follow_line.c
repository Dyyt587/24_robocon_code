#include "follow_line.h"
#include "apid.h"
#include "apid.h"
#include "abus_topic.h"
#include "chassis_port.h"
#include "abus_port.h"
infrared infrared_package;

float error;
int asub_callback2(abus_topic_t *sub)
{
	LOG_D("sub_callback2\n");
	return 0;
}
int line_dir_sub_callback(abus_topic_t *sub)
{
	//读取数据
	afifo_out_data(sub->datafifo, &infrared_package.move_direction, sizeof(infrared_package.move_direction));
	LOG_D("line_dir_sub_callback:%d\n", infrared_package.move_direction);
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

void follow_line(void *parameter)
{
	Infrared_Init();
	infrared_package.move_direction = front;

	chassis_ctrl_t ctrl;
	ctrl.type = 1;
	ctrl.pos.z_rad = 400;
	rt_thread_mdelay(1000);
	while (1)
	{
#define factor3 0.1
#define factor2 0.2
#define factor1 0.3
#define factor0 0.4
		/* 线程处理 */
		GET_Infrared_Data(&infrared_package);
		/* 线程运行，打印计数 */
		    //Print_Infrared_Data(&infrared_package);
		//rt_kprintf("is_spacial_point_flag:%d\n",Is_Spacial_point(&infrared_package));
		error =
			(-infrared_package.infrared_data[0] * factor0 +
			infrared_package.infrared_data[7] * factor0

			- infrared_package.infrared_data[1] * factor1 +
			infrared_package.infrared_data[6] * factor1

			- infrared_package.infrared_data[2] * factor2 +
			infrared_package.infrared_data[5] * factor2

			- infrared_package.infrared_data[3] * factor3 +
			infrared_package.infrared_data[4] * factor3);

		if(Is_Spacial_point(&infrared_package)){
			uint8_t bool_=1;
		abus_public(&line_spacial_point_acc, &bool_);

		}
				abus_public(&line_error_acc, &error);

		rt_thread_mdelay(15);

	}
}

// 以上帝视角为准,引入 拿/放（反转视角）时,小车行进状态:直行、左行、右行
uint8_t Is_Spacial_point(infrared *infrared_package)
{
	switch (infrared_package->move_direction)
	{
	case front:
		if (( (infrared_package->infrared_data[0]&&infrared_package->infrared_data[1]) || (infrared_package->infrared_data[7]&&infrared_package->infrared_data[6])) && (infrared_package->infrared_data[3] || infrared_package->infrared_data[4]))
			{
			infrared_package->is_spacial_point_flag = 1;
			// Print_Infrared_Data(infrared_package);
			}
		else
			infrared_package->is_spacial_point_flag = 0;
		break;

	// case left:
	// 	if (infrared_package->infrared_data[left_left0_infrared] || infrared_package->infrared_data[left_right1_infrared] && (infrared_package->infrared_data[left_middle0_infrared] || infrared_package->infrared_data[left_middle1_infrared]))
	// 		infrared_package->is_spacial_point_flag = 1;
	// 	else
	// 		infrared_package->is_spacial_point_flag = 0;
	// 	break;

	// case right:
	// 	if (infrared_package->infrared_data[right_left0_infrared] || infrared_package->infrared_data[right_right1_infrared] && (infrared_package->infrared_data[right_middle0_infrared] || infrared_package->infrared_data[right_middle1_infrared]))
	// 		infrared_package->is_spacial_point_flag = 1;
	// 	else
	// 		infrared_package->is_spacial_point_flag = 0;
	// 	break;

	case rotate:
		infrared_package->is_spacial_point_flag = 0;
		break;

	default:
		infrared_package->is_spacial_point_flag = 0;
		break;
	}
	return infrared_package->is_spacial_point_flag;
}

void Infrared_Init(void)
{
	rt_pin_mode(INFRARED0_PIN, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(INFRARED1_PIN, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(INFRARED2_PIN, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(INFRARED3_PIN, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(INFRARED4_PIN, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(INFRARED5_PIN, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(INFRARED6_PIN, PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(INFRARED7_PIN, PIN_MODE_INPUT_PULLDOWN);
}

void GET_Infrared_Data(infrared *infrared_package)
{
	#define TT(x) ((x)?0:1)
	infrared_package->infrared_data[0] = TT(rt_pin_read(INFRARED0_PIN));
	infrared_package->infrared_data[1] = TT(rt_pin_read(INFRARED1_PIN));
	infrared_package->infrared_data[2] = TT(rt_pin_read(INFRARED2_PIN));
	infrared_package->infrared_data[3] = TT(rt_pin_read(INFRARED3_PIN));
	infrared_package->infrared_data[4] = TT(rt_pin_read(INFRARED4_PIN));
	infrared_package->infrared_data[5] = TT(rt_pin_read(INFRARED5_PIN));
	infrared_package->infrared_data[6] = TT(rt_pin_read(INFRARED6_PIN));
	infrared_package->infrared_data[7] = TT(rt_pin_read(INFRARED7_PIN));
}

void Print_Infrared_Data(infrared *infrared_package)
{
	LOG_D("data %d %d %d %d %d %d %d %d ",infrared_package->infrared_data[0],
	infrared_package->infrared_data[1],
	infrared_package->infrared_data[2],
	infrared_package->infrared_data[3],
	infrared_package->infrared_data[4],
	infrared_package->infrared_data[5],
	infrared_package->infrared_data[6],
	infrared_package->infrared_data[7]);
}

// aStateMachine* sm = (aStateMachine*)malloc(sizeof(aStateMachine));
// aStateMachine_Init(sm, "sm");
// aState* s1 = (aState*)malloc(sizeof(aState));
// aState_Init(s1, "s1", aState_excution, aState_entry, aState_exit);
// aState* s2 = (aState*)malloc(sizeof(aState));
// aState_Init(s2, "s2", aState_excution, aState_entry, aState_exit);
// aState* s3 = (aState*)malloc(sizeof(aState));
// aState_Init(s3, "s3", aState_excution, aState_entry, aState_exit);
// aState* s4 = (aState*)malloc(sizeof(aState));
// aState_Init(s4, "s4", aState_excution, aState_entry, aState_exit);
// aStateMachine_addState(sm, s1);
// aStateMachine_addState(sm, s2);
// aStateMachine_addState(sm, s3);
// aStateMachine_addState(sm, s4);

// aStateMachine_start(sm, s1);
// aStateMachine_addTransition1(sm, s1, s2, condition); //[](aStateMachine* sm) {printf("%s condition s1 -> s2", aState_getName(sm->current)); return false; }
// aStateMachine_addTransition1(sm, s1, s3, condition);//[](aStateMachine* sm) {printf("%s condition s1 -> s3", aState_getName(sm->current)); return true; }
// aStateMachine_addTransition1(sm, s2, s3, condition);//[](aStateMachine* sm) {return true; }
// aStateMachine_addTransition1(sm, s3, s4, condition);
// aStateMachine_addTransition1(sm, s4, 0, condition);
// aStateMachine_showStates(sm);

// //update 会更新状态机状态，会执行excution 然后判断condtion
// aStateMachine_update(sm);
// aStateMachine_update(sm);
// aStateMachine_update(sm);
// aStateMachine_update(sm);
// aStateMachine_update(sm);
