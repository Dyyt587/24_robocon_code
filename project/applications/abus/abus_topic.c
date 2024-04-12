/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-11 00:05:09
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-04-03 11:05:51
 * @FilePath: \abus_v2.0.1\abus_v2.0.1\abus_topic.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "abus_topic.h"
#include "ulog.h"
static abus_accounter_t* accs[ABUS_ACCS_NUM]={0};
static uint32_t accs_index=0;

static abus_topic_t* topics[ABUS_TOPICS_NUM]={0};
static uint32_t topics_index=0;

int abus_topic_init(abus_topic_t *topic, abus_topic_init_t *init)
{
    topic->name = init->name;
    topic->datafifo = afifo_init(init->buf, init->buf_size, 0);
    INIT_LIST_HEAD(&topic->sync_accounter);
    INIT_LIST_HEAD(&topic->async_accounter);
    topic->msg_size = init->msg_size;

    ABUS_ASSERT(topics_index<ABUS_TOPICS_NUM);
    /* 添加到topics */
    topics[topics_index++]=topic;
    return 0;
}
int abus_acc_init(abus_accounter_t *acc, abus_acc_init_t *init)
{
    acc->name = init->name;
    acc->datafifo = init->datafifo;
    acc->callback = init->callback;

    ABUS_ASSSERT(accs_index<ABUS_ACCS_NUM);
    /* 添加到accs */
    accs[accs_index++]=acc;
    return 0;
}

abus_accounter_t *abus_accounter_find(const char *name)
{
    for (uint32_t i = 0; i < accs_index; i++)
    {
        if (accs[i]->name == name)
        {
            return accs[i];
        }
    }
    return 0;
}
abus_topic_t *abus_topic_find(const char *name)
{
    for (uint32_t i = 0; i < topics_index; i++)
    {
        if (topics[i]->name == name)
        {
            return topics[i];
        }
    }
    return 0;
}   
int abus_topic_subscribe_name(const char *topic, const char  *acc, abus_sub_flag flag)
{
    abus_accounter_t *acc_t = abus_accounter_find(acc);
    abus_topic_t *topic_t = abus_topic_find(topic);
    if (acc_t && topic_t)
    {
        return abus_topic_subscribe(topic_t, acc_t, flag);
    }
    return -1;
}
int abus_topic_unsubscribe_name(const char *topic, const char  *acc)
{
    abus_accounter_t *acc_t = abus_accounter_find(acc);
    abus_topic_t *topic_t = abus_topic_find(topic);
    if (acc_t && topic_t)
    {
        return abus_topic_unsubscribe(topic_t, acc_t);
    }
    return -1;
}
int abus_public_name(const char *acc, void *msg)
{
    abus_accounter_t *acc_t = abus_accounter_find(acc);
    if (acc_t)
    {
        return abus_public(acc_t, msg);
    }
    return -1;
}

int abus_topic_subscribe(abus_topic_t *topic, abus_accounter_t *acc, abus_sub_flag flag)
{
    acc->flag = flag;
    acc->topic = topic;
    if (!acc->flag.is_async)
    {
        // sync
        list_add(&acc->head, &topic->sync_accounter);
    }
    else
    {
        LOG_W("abus_topic_subscribe  async\n");
        /* async 将调用一个线程去完成传输*/
        list_add(&acc->head, &topic->async_accounter);
    }
    return 0;
}

int abus_topic_unsubscribe(abus_topic_t *topic, abus_accounter_t *acc)
{
    acc->topic = 0;
    list_del(&acc->head);

    return 0;
}

int abus_public(abus_accounter_t *acc, void *msg)
{

    // 将数据放入topic
    uint32_t acc_in = afifo_in(acc->topic->datafifo, msg, acc->topic->msg_size);
    if (acc_in != acc->topic->msg_size)
    {
        // TODO:
        LOG_D("abus_public  data in error acc in %d will in %d\n", acc_in, acc->topic->msg_size);
        while (1)
            ;
    }
    // 遍历sync链表
    alist_head_t *pos = 0;
    list_for_each(pos, &acc->topic->sync_accounter)
    {
        abus_accounter_t *sync = list_entry(pos, abus_accounter_t, head);
        if (!sync->flag.is_async)
        {
            // sync
            if (sync->datafifo)
            {
                // 将数据复制到里面
                uint32_t sync_in = afifo_in(sync->datafifo, msg, acc->topic->msg_size);
                if (sync_in != sync->topic->msg_size)
                {
                    LOG_D("abus_public data in error sync in %d will in %d\n", sync_in, acc->topic->msg_size);
                    while (1)
                        ;
                }
            }
            if (sync->callback)
                sync->callback(acc->topic);
            //LOG_D("[abus]topic:%s\tpublicer:%s\tsub:%s", acc->topic->name, acc->name, sync->name);
        }
    }

    //    //sync
    //    if (sync->datafifo) {
    //        //将数据复制到里面

    //    }
    //    if (sync->callback)acc->callback(acc->topic);

    //}
    afifo_add_out(acc->topic->datafifo, acc->topic->msg_size);

    return 0;
}
