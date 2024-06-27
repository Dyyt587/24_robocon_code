#include "intel_ros.h"
#include <string.h>
#include <stdio.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"
#include "chassis_module_omni4.h"
#include "chassis.h"
#include "chassis_port.h"



static rt_device_t ros_uart = 0;

static rt_sem_t sem_ros;

ros_data ros2;

upacker_inst msg_packer;//ʵÀý
extern chassis_t chassis_mai;
rt_size_t ros_size;

float line_t=0.0f;



visual_date_t tmp;
visual_date_t Line;
visual_date_t T_Line;
visual_date_t Center_Line;


xxdate da;
xxdate dd;
/***²⊔´ú«******/
uart_data Dat;
/**
  * @brief  ϻϢ½⎶»ص÷
  * @note   
  * @param  *d: 
  * @param  size: 
  * @retval None
  */
static void handle_callback(uint8_t *d, uint16_t size)
{
    sscanf((char *)d,"%s %f %f %f\r\n",tmp.id_tap,&tmp.x,&tmp.y,&tmp.k);
//    LOG_D("data:%s",d);
//    LOG_D("jiexi: %s %f %f %f",tmp.id_tap,tmp.x,tmp.y,tmp.k);
}



static void uart_send(uint8_t *d, uint16_t size)
{
//   ros2.pos_x=chassis_mai.present.pos.x_m;
//     ros2.pos_y=chassis_mai.present.pos.y_m;
//     ros2.speed_x=chassis_mai.present.speed.x_m_s;
//     ros2.speed_y=chassis_mai.present.speed.y_m_s;
//     ros2.z_rad=chassis_mai.present.speed.z_rad_s;
     rt_device_write(ros_uart,0,d,size);
}

////·¢ˍcmd1ʽ¾ݍ
//void send_cmd1(){
//    uint8_t buff[10];
//        buff[0] = 0x40;
//    buff[1] = 55;
//    upacker_pack(&msg_packer, (uint8_t *)buff, 2);
//}     

//ʍ·ŐźŁ¿
rt_err_t ros_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
        ros_size=size;
        
    rt_sem_release(sem_ros);
    return RT_EOK;
}

/**
 * @brief   initialize dbus uart device
 * @param
 * @retval
 */
int ros_uart_init(void)
{
    sem_ros = rt_sem_create("sem_ros", 0, RT_IPC_FLAG_FIFO);
    ros_uart = rt_device_find("uart7");
    if (ros_uart == RT_NULL)
    {
        LOG_E("uart7 not found");
        return -1;
    }
    if (rt_device_open(ros_uart, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
    {
        LOG_E("uart7 open failed");
        return -1;
    }
    
    if (rt_device_set_rx_indicate(ros_uart, ros_uart_rx_ind) != RT_EOK)
    {
        LOG_E("ros_uart7 set rx indicate failed");
        return -1;
    }
        
    return 0;
}

static char rx_buffer[100];
////µõ½¸ûº³凸
//void get_buff(uint8_t ch)
//{
//    if(ch!=NULL)
//    {
//        rx_buffer=
//    }
//}

uint8_t ros_ch = 0;

//ϟ³̈뿚
void ros_entry(void *parameter)
{
    uint8_t i = 0;
        ros_uart_init();
      ros_data msg;
    rt_err_t result;

    while (1)
    {
                result=rt_device_read(ros_uart, 0, &ros_ch, 1);
        if(result!= 1)
        {
            rt_sem_take(sem_ros, RT_WAITING_FOREVER);
                        result=1;
        }
//                LOG_D("data: %c",ros_ch);
                //¶ªµ½packer½⎶£¬³ɹ¦Á˵÷Ӄcallback
                upacker_unpack(&msg_packer, &ros_ch,1);
                rt_thread_mdelay(1);
                
    }
}

rt_thread_t ros;
// ´´½¨¶¯̬ϟ³̍
int ros_init(void)
{
    ros = rt_thread_create("Ros",
                             ros_entry, RT_NULL,
                             4096, 9,
                             1);
    if (ros != NULL) // ·µ»ؖµսȷ£¬ƴ¶¯¸Ï߳̍
    {
        rt_thread_startup(ros);
    }
        //init packer
        upacker_init(&msg_packer,handle_callback, uart_send);
    return 0;
}

rt_thread_t send;


//·¢ˍcmd1ʽ¾ݍ
void send_cmd1(){
        dd.id=0x40;
        dd.a++;
        dd.b++;
//        LOG_D("cmd1 data: %s", d);
    upacker_pack(&msg_packer, (uint8_t *)&dd, sizeof(dd));
}     
void send_entry(void *parameter)
{
    while (1)
    {
            send_cmd1();
            rt_thread_mdelay(100);
    }
}
// ´´½¨¶¯̬ϟ³̍
int send_init(void)
{
    send = rt_thread_create("send",
                             send_entry, RT_NULL,
                             4096, 9,
                             1);
    if (send != NULL) // ·µ»ؖµսȷ£¬ƴ¶¯¸Ï߳̍
    {
        rt_thread_startup(send);
    }
    return 0;
}

INIT_APP_EXPORT(ros_init); // ה¶¯³õʼ»¯
//INIT_APP_EXPORT(send_init); // ה¶¯³õʼ»¯


