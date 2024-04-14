#include "drv_visual.h"

/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-04-12 16:11:45
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-15 01:22:57
 * @FilePath: \VS_WORKSPACE\test\test.c
 * @Description: ����Ĭ������,������`customMade`, ����koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <string.h>
#include <stdio.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"

static rt_device_t uart = 0;

// typedef struct ball
// {
//     uint8_t count; // 球的个数
//     cvdat Dat[10]; // 球的数据
// } Ball;            // 这个是不同的id的球的

cvdat ball[10];
cvdat aball;

// Ball realball[3]; // 三个id
static rt_sem_t sem_visual;

unsigned char b[200] = {0};
void cvRead(unsigned char dat, int pos, unsigned char *Dat);
void cvhandle(unsigned char dat);

void cvparse_data(int count)
{
    b[count] = 0;
    char *raw_str = (char *)b;

    if (sscanf(raw_str, "%d,%d,%f,%f.", &aball.color, &aball.id, &aball.posX, &aball.posY) != 4)
    {
        LOG_E("format error");
    }

    // LOG_D("%d,%d,%f,%f", ball.color, ball.id, ball.posX, ball.posY);
    memcpy(&ball[aball.id], &aball, sizeof(aball));
    //LOG_D("%d,%d,%f,%f", ball[aball.id].color, ball[aball.id].id, ball[aball.id].posX, ball[aball.id].posY);
}
void cvhandle(unsigned char dat)
{
    static int count = 0;
    static unsigned char state = 0, ballNum = 0, i = 0;
    if (dat == 'b')
    {
        state = 1; // 开始接收球数据
        return;
    }
    else if (dat == 'e')
    {
        state = 2; // 接收完毕
    }

    if (state == 1)
    {
        b[count++] = dat;
    }
    if (state == 2)
    {
        // 接收完一包数据，打印数据
        static int cnt = 0;
        // if(cnt++%10==0)
        // LOG_HEX("a",16,b,count+2);
        volatile int _count = count;
        count = 0;
        cvparse_data(_count);
    }
}


rt_err_t visual_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(sem_visual);
    return RT_EOK;
}

/**
 * @brief   initialize dbus uart device
 * @param
 * @retval
 */
int visual_uart_init(void)
{
    sem_visual = rt_sem_create("sem_visual", 0, RT_IPC_FLAG_FIFO);

    uart = rt_device_find("uart3");
    if (uart == RT_NULL)
    {
        LOG_E("uart3 not found");
        return -1;
    }
    if (rt_device_open(uart, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
    {
        LOG_E("uart3 open failed");
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
    if (rt_device_set_rx_indicate(uart, visual_uart_rx_ind) != RT_EOK)
    {
        LOG_E("uart3 set rx indicate failed");
        return -1;
    }
    return 0;
}

uint8_t ch = 0;

void test1_entry(void *parameter)
{
    uint8_t i = 0;
    visual_uart_init();
    while (1)
    {
        while (rt_device_read(uart, 0, &ch, 1) != 1)
        {
            //					if(SSize>512)LOG_W("%d",SSize);

            //				if(!=1)
            //				{
            //					//LOG_E("error");
            //				}
            // rt_thread_mdelay(100);

            rt_sem_take(sem_visual, RT_WAITING_FOREVER);
        }
        cvhandle(ch);

        // rt_thread_mdelay(1);
    }
}

rt_thread_t test1;
// 创建动态线程
int test1_init(void)
{
    test1 = rt_thread_create("Test",
                             test1_entry, RT_NULL,
                             4096, 9,
                             1);
    if (test1 != NULL) // 返回值正确，启动该线程
    {
        rt_thread_startup(test1);
    }
    return 0;
}

INIT_APP_EXPORT(test1_init); // 自动初始化

// INIT_DEVICE_EXPORT(visual_uart_init);
