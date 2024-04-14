#include "drv_visual.h"

/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-04-12 16:11:45
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-12 13:58:02
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

typedef struct cvDat
{
    int id;
    float posX;
    float posY;
} cvdat;//



typedef struct ball
{
	uint8_t count;//球的个数
	cvdat Dat[10];//球的数据
}Ball;//这个是不同的id的球的

cvdat ball[10];
Ball realball[3];//三个id

unsigned char b[200] = {0};
void cvRead(unsigned char dat, int pos, unsigned char *Dat);
void cvhandle(unsigned char dat, unsigned char *Dat);



//存球，对应的球存入对应id的结构体
void Storeball(void)
{
	uint8_t i=0,j=0;
	for(i=0;i<10;i++)
	{
		realball[ball[i].id].Dat[j]=ball[i];
		j=realball[ball[i].id].count++;
	}
}



void cvhandle(unsigned char dat, unsigned char *Dat)
{
    static unsigned char state = 0, ballNum = 0, i = 0;
    if (dat == 'b')
    {
        state = 1; //
    }
    else if (dat == 'e')
    {
        state = 0; //
        ballNum=0;
    }
    if (dat == '\n')
    {
        ballNum++;
    }
    if (state == 1) //
    {
        cvRead(dat, ballNum, Dat);
    }
}
void cvRead(unsigned char dat, int pos, unsigned char *Dat)
{
    static int i = 0;
    if (dat != '\n' && dat != 'b' && dat != 'e')
    {
        Dat[i] = dat;
        i++;
    } 
    else if (dat == '\n')
    {
        i = 0;
        static int cnt=0;
        if(cnt++%100==0){
        LOG_D("b:%s\r\n", Dat);
        }
        sscanf((char *)Dat, "%d,%.4f,%.4f", &ball[pos].id, &ball[pos].posX, &ball[pos].posY);
        memset(Dat, 0, strlen((char *)Dat));
    }
		Storeball();
}

rt_err_t visual_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    uint8_t ch = 0;
	if(size>512)LOG_W("%d",size);
    while (size--)
    {
        rt_device_read(uart, 0, &ch, 1);
			//LOG_D("%d,%c",ch,ch);
        cvhandle(ch, b);
    }
		return RT_EOK;
}




/**
 * @brief   initialize dbus uart device
 * @param
 * @retval
 */
int visual_uart_init(void)
{
    uart = rt_device_find("uart3");
    if (uart == RT_NULL)
    {
        LOG_E("uart1 not found");
        return -1;
    }
    if (rt_device_open(uart, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_RX_NON_BLOCKING) != RT_EOK)
    {
        LOG_E("uart1 open failed");
        return -1;
    }
		struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
		
//在打开串口设备之后对其参数进行修改
config.baud_rate  =  BAUD_RATE_921600;
config.rx_bufsz=BSP_UART3_RX_BUFSIZE;
config.tx_bufsz=BSP_UART3_TX_BUFSIZE;
if(RT_EOK != rt_device_control(uart, RT_DEVICE_CTRL_CONFIG, &config))
{
	 rt_kprintf("change %s failed!\n", uart->parent.name);
}
    if (rt_device_set_rx_indicate(uart, visual_uart_rx_ind) != RT_EOK)
    {
        LOG_E("uart1 set rx indicate failed");
        return -1;
    }
    return 0;
}
INIT_DEVICE_EXPORT(visual_uart_init);
