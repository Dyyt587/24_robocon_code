#include "abus_topic.h"
#include <rtthread.h>
#include <ulog.h>

#include "chassis_port.h"
#include "robotmanager.h"

abus_topic_t line_error_topic;	 /* 由巡线传感器发出-->，指示线路偏差 (float) */
abus_topic_t line_dir_topic;	 /* 由巡线传感器接收<--，用于更换传感器方向 (uint8_t) */
abus_topic_t line_special_topic; /* 由巡线传感器发出-->,用于提示发现特殊点 (uint8_t) */

abus_topic_t chassis_ctrl_topic; /* 由地盘模块接收<--,地盘控制 (chassis_ctrl_t) */

abus_accounter_t line_error_acc;		 /* 巡线传感器error值接收用户，由传感器模块使用 */
abus_accounter_t line_dir_acc;			 /* 巡线传感器方向接收用户，由传感器模块使用 */
abus_accounter_t line_spacial_point_acc; /* 巡线传感器special point 发送用户，有传感器模块使用 */

abus_accounter_t chassis_acc; /*地盘控制接收用户，由底盘模块使用 */

//机器人manager的acc
/**
 * @brief robot manager的acc
 * robot manager 是机器人的核心逻辑模块，负责整个机器人的状态管理，路径规划，任务分配等等
 * 
 */
 abus_accounter_t rbmg_error_acc;//接收error
 abus_accounter_t rbmg_dir_acc;//发布dir
 abus_accounter_t rbmg_special_point_acc;//接收special point
 abus_accounter_t rbmg_chassis_acc;//发布chassis ctrl


// static abus_accounter_t line_chassis_ctrl_acc;

int abus_all_init(void)
{
	extern int line_dir_sub_callback(abus_topic_t * sub);
	extern int chassis_sub_callback(abus_topic_t * sub);

	abus_topic_init_t init;
		abus_acc_init_t acc_init;


	init.buf = (uint8_t *)malloc(1024);
	init.buf_size = 1024;
	init.msg_size = sizeof(float);
	init.name = "line_error_topic";
	abus_topic_init(&line_error_topic, &init);

	init.buf = (uint8_t *)malloc(1024);
	init.buf_size = 1024;
	init.msg_size = sizeof(uint8_t);
	init.name = "line_special_topic";
	abus_topic_init(&line_special_topic, &init);

	init.buf = (uint8_t *)malloc(1024);
	init.buf_size = 1024;
	init.msg_size = sizeof(uint8_t);
	init.name = "line_dir_topic";
	abus_topic_init(&line_dir_topic, &init);


	// 初始化topic
	abus_topic_init_t topic_init;
	topic_init.name = "chassis_ctrl";
	topic_init.buf = rt_malloc(1024);
	topic_init.buf_size = 1024;
	topic_init.msg_size = sizeof(chassis_ctrl_t);
	abus_topic_init(&chassis_ctrl_topic, &topic_init);



	acc_init.name = "line_error_acc";
	acc_init.callback = NULL;
	acc_init.datafifo = NULL;
	abus_acc_init(&line_error_acc, &acc_init);
	abus_topic_subscribe(&line_error_topic, &line_error_acc,  (abus_sub_flag){0, 0});

	line_dir_acc.name = "line_dir_acc";
	line_dir_acc.callback = line_dir_sub_callback;
	line_dir_acc.datafifo = NULL;
	line_dir_acc.flag.is_async = 0;
	abus_acc_init(&line_dir_acc, &acc_init);
	abus_topic_subscribe(&line_dir_topic, &line_dir_acc, (abus_sub_flag){0, 0});

	acc_init.name = "line_special_acc";
	acc_init.callback = NULL;
	acc_init.datafifo = NULL;
	abus_acc_init(&line_spacial_point_acc, &acc_init);
	abus_topic_subscribe(&line_special_topic, &line_spacial_point_acc,  (abus_sub_flag){0, 0});


	// 初始化chassis 模块acc
	acc_init.name = "chassis_acc";
	acc_init.datafifo = NULL;
	acc_init.callback = chassis_sub_callback;
	abus_acc_init(&chassis_acc, &acc_init);
	abus_topic_subscribe(&chassis_ctrl_topic, &chassis_acc, (abus_sub_flag){0, 0});

//初始化robotManager 模块acc

	acc_init.name = "rbmg_error_acc";
	acc_init.datafifo = NULL;
	acc_init.callback = rbmg_error_callback;
	abus_acc_init(&rbmg_error_acc, &acc_init);
	abus_topic_subscribe(&line_error_topic, &rbmg_error_acc, (abus_sub_flag){0, 0});

	acc_init.name = "rbmg_dir_acc";
	acc_init.datafifo = NULL;
	acc_init.callback = rbmg_dir_callback;
	abus_acc_init(&rbmg_dir_acc, &acc_init);
	abus_topic_subscribe(&line_dir_topic, &rbmg_dir_acc, (abus_sub_flag){0, 0});

	acc_init.name = "rbmg_special_point_acc";
	acc_init.datafifo = NULL;
	acc_init.callback = rbmg_special_point_callback;
	abus_acc_init(&rbmg_special_point_acc, &acc_init);
	abus_topic_subscribe(&line_special_topic, &rbmg_special_point_acc, (abus_sub_flag){0, 0});

	acc_init.name = "rbmg_chassis_acc";
	acc_init.datafifo = NULL;
	acc_init.callback = rbmg_chassis_ctrl_callback;
	abus_acc_init(&rbmg_chassis_acc, &acc_init);
	abus_topic_subscribe(&chassis_ctrl_topic, &rbmg_chassis_acc, (abus_sub_flag){0, 0});
	return 0;
}
INIT_BOARD_EXPORT(abus_all_init);
