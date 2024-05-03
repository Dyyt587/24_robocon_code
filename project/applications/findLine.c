#include "drv_visual.h"

/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-04-12 16:11:45
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-15 01:22:57
 * @FilePath: \VS_WORKSPACE\test\test.c
 * @Description: ???????????,??????`customMade`, ????koroFileHeader?????? ????????: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <string.h>
#include <stdio.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"

static rt_device_t uart = 0;

static rt_sem_t sem_visual;

typedef struct line_control
{
    int kind;
    int x_pos;
    int y_pos;
    int angle;
} line;

line line_ctr;
// Ball realball[3]; // 三个id
static rt_sem_t sem_visual;

unsigned char buffer[200] = {0};
char line_data[100];

void lineparse_data(void)
{
		strcpy((char *)line_data,(char *)buffer);
		LOG_D("%s\r\n", line_data);
		if (sscanf(line_data, "%d,%d,%d,%d", &line_ctr.kind, &line_ctr.x_pos, &line_ctr.y_pos, &line_ctr.angle) != 4)
    {
        LOG_E("format error");
        return;
    }
		LOG_D("%d %d %d %d\r\n", line_ctr.kind, line_ctr.x_pos, line_ctr.y_pos, line_ctr.angle);
}


//一个一个字节的传
void linehandle(uint8_t dat)
{
    static int count = 0;
    static unsigned char state = 0, ballNum = 0, i = 0;
		if (dat == 0x0a)
		{
				state=1;
		}
		else if (dat == 0x0b)
		{
				state=2;
		}
		if(state==1&&dat!=0x0a)
		{
			buffer[count++] = dat;//接收数据
		}
		else if(state==2)//接收完一包
		{
			// 接收完一包数据，打印数据
       static int cnt = 0;
       // if(cnt++%10==0)
       // LOG_HEX("a",16,b,count+2);
       volatile int _count = count;
       count = 0;
			lineparse_data();
			count=0;
		}
}

rt_err_t visual_uart1_rx_ind(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(sem_visual);
    return RT_EOK;
}

//初始化不用改
/**
 * @brief   initialize dbus uart device
 * @param
 * @retval
 */
int visual_uart1_init(void)
{
    sem_visual = rt_sem_create("sem_visual", 0, RT_IPC_FLAG_FIFO);
//		aball.posY = 0.001;
//		aball.posX = 0.5;

    uart = rt_device_find("uart3");
    if (uart == RT_NULL)
    {
        LOG_E("line_uart not found");
        return -1;
    }
    if (rt_device_open(uart, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
    {
        LOG_E("line_uart open failed");
        return -1;
    }
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    // 在打开串口设备之后对其参数进行修改
    config.baud_rate = BAUD_RATE_921600;
    config.rx_bufsz = BSP_UART3_RX_BUFSIZE;
    config.tx_bufsz = BSP_UART3_TX_BUFSIZE;
    if (RT_EOK != rt_device_control(uart, RT_DEVICE_CTRL_CONFIG, &config))
    {
        rt_kprintf("change %s failed!\n", uart->parent.name);
    }
    if (rt_device_set_rx_indicate(uart, visual_uart1_rx_ind) != RT_EOK)
    {
        LOG_E("line_uart set rx indicate failed");
        return -1;
    }
    return 0;
}


uint8_t ch1 = 0;

//线程入口
void test2_entry(void *parameter)
{
    uint8_t i = 0;
    visual_uart1_init();//在线程里面初始化
    while (1)
    {
        while (rt_device_read(uart, 0, &ch1, 1) != 1)
        {
            //					if(SSize>512)LOG_W("%d",SSize);

            //				if(!=1)
            //				{
            //					//LOG_E("error");
            //				}
            // rt_thread_mdelay(100);

            rt_sem_take(sem_visual, RT_WAITING_FOREVER);//信号量
        }
        //cvhandle(ch);//线程里面解析
				linehandle(ch1);
        // rt_thread_mdelay(1);
    }
}

rt_thread_t test2;
// 创建动态线程
int test2_init(void)
{
    test2 = rt_thread_create("Test2",
                             test2_entry, RT_NULL,
                             4096, 9,
                             1);
    if (test2 != NULL) // 返回值正确，启动该线程
    {
        rt_thread_startup(test2);
    }
    return 0;
}

INIT_APP_EXPORT(test2_init); // 自动初始化

// INIT_DEVICE_EXPORT(visual_uart_init);
