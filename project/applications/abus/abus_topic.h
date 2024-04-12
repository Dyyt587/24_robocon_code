/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-07 19:35:50
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-03-19 10:15:25
 * @FilePath: \abus_v2\abus_topic.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef ABUS_TOPIC_H
#define ABUS_TOPIC_H
#ifdef __cpluscplus
extern "C"
{
#endif
#include "cvector.h"
#include "alist.h"
#include "afifo.h"

#define ABUS_ACCS_NUM (128)
#define ABUS_TOPICS_NUM (32)

#define ABUS_TOPIC_STATIC(name, _struct_)

#define ABUS_ASSSERT(x) \
	if (!(x))          \
		while (1)


	typedef struct abus_accounter abus_accounter_t;
	typedef struct abus_topic abus_topic_t;
	typedef int (*sub_callback)(abus_topic_t *sub);
	typedef struct
	{
		uint8_t is_async;
		uint8_t is_self_fifo;

	} abus_sub_flag;
	 struct abus_accounter
	{
		const char *name;
		alist_head_t head;
		afifo_t *datafifo;
		sub_callback callback;
		abus_sub_flag flag;
		abus_topic_t *topic;
	} ;
	typedef struct
	{
		const char *name;
		afifo_t *datafifo;
		sub_callback callback;
	} abus_acc_init_t;
	 struct abus_topic
	{
		const char *name;
		alist_head_t sync_accounter;  // 同步订阅，在发布者支持同步条件下会同步调用
		alist_head_t async_accounter; // 由topic线程转发的
		afifo_t *datafifo;			  // 用于给发布者缓存数据，这个fifo支持覆盖
		uint32_t msg_size;

	} ;
	typedef struct
	{
		const char *name;
		uint8_t *buf;
		uint32_t buf_size;
		uint32_t msg_size;
	} abus_topic_init_t;

	// typedef struct
	// {
	// 	const char*name;
	// 	abus_accounter_t *acc;
	// } abus_t;
#define ABUS_ASSERT(x) \
	if (!(x))          \
		while (1)

#define ABUS_ACC_ALLOC(_name,_datafifo,_callback)do{\
	abus_acc_init_t init={\
		.name=_name,\
		.datafifo=_datafifo,\
		.callback=_callback,\
	};\
	abus_accounter_t*acc = ABUS_MALLOC(sizeof(abus_accounter_t));\
	ABUS_ASSERT(acc);\
	abus_acc_init(acc,&init);\
}while(0)
	
	int abus_topic_init(abus_topic_t *topic, abus_topic_init_t *init);
	int abus_acc_init(abus_accounter_t *acc, abus_acc_init_t *init);

	int abus_topic_subscribe(abus_topic_t *topic, abus_accounter_t *acc, abus_sub_flag flag);
	int abus_topic_unsubscribe(abus_topic_t *topic, abus_accounter_t *acc);
	int abus_public(abus_accounter_t *acc, void *msg);

	int abus_topic_subscribe_name(const char *topic, const char *acc, abus_sub_flag flag);
	int abus_topic_unsubscribe_name(const char *topic, const char *acc);
	int abus_public_name(const char *acc, void *msg);
#ifdef __cpluscplus
}
#endif
#endif