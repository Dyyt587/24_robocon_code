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





void cvhandle(unsigned char dat, unsigned char *Dat)
{
	static int count=0;
    static unsigned char state = 0, ballNum = 0, i = 0;
    if (dat == 'b')
    {
        state = 1; //
    }
    else if (dat == 'e')
    {
        state = 0; //
//				if(i++%100==0)
				LOG_HEX("a",10,Dat,20);
				
				count=0;
//        ballNum=0;
//				memset(Dat, 0, strlen((char *)Dat));
    }
//    if (dat == '\n')
//    {
//        ballNum++;
//    }
    if (state == 1) //
    {
			Dat[count++]=dat;
//      cvRead(dat, ballNum, Dat);
    }
}


////解析函数
//void data_read(unsigned char *Dat,int len,uint8_t pos){
//	int max = len/18;
//	for (int i = 0;i<max;i++){
//		ball[i].color= (Dat[i*18])-48;
//		ball[i].id= Dat[i*18+2]-48;
//		ball[i].posX= (Dat[i*18+6]-48)*0.1+(Dat[i*18+7]-48)*0.01+(Dat[i*18+8]-48)*0.001+(Dat[i*18+9]-48)*0.0001;
//		ball[i].posY= (Dat[i*18+13]-48)*0.1+(Dat[i*18+14]-48)*0.01+(Dat[i*18+15]-48)*0.001+(Dat[i*18+16]-48)*0.0001;
//	}	
//}

//解析函数
void data_read(unsigned char *Dat,uint8_t pos)
{
		ball[pos].color= (Dat[0])-48;
		ball[pos].id= Dat[2]-48;
		ball[pos].posX= (Dat[6]-48)*0.1f+(Dat[7]-48)*0.01f+(Dat[8]-48)*0.001f+(Dat[9]-48)*0.0001f;
		ball[pos].posY= (Dat[13]-48)*0.1f+(Dat[14]-48)*0.01f+(Dat[15]-48)*0.001f+(Dat[16]-48)*0.0001f;
}	



void cvRead(unsigned char dat, int pos, unsigned char *Dat)
{
    static int i = 0;
		static float x,y;
		static int color,id;

    if ( dat != 'b' && dat != 'e')
    {
        Dat[i] = dat;
        i++;
    } 
    if(dat == '\n')
    {
        i = 0;
				data_read(Dat,pos);
        static int cnt=0;
			if(cnt++%300==0)
			{
					LOG_HEX("a",10,Dat,20);
					LOG_D("%f,%f,%d\r\n",ball[pos].posX,ball[pos].posY,pos);
				cnt=0;
			}
       memset(Dat, 0,100);
    }
}

uint16_t SSize=0;


rt_err_t visual_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    uint8_t ch = 0;
		SSize=size;
//	if(size>512)LOG_W("%d",size);
//    while (size--)
//    {
//			
//        rt_device_read(uart, 0, &ch, 1);
//        cvhandle(ch, b);
//    }
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

uint8_t ch=0;


void test1_entry(void *parameter)
{
	uint8_t i=0;
	while(1)
	{
		while (SSize--)
		{
					if(SSize>512)LOG_W("%d",SSize);

				if(rt_device_read(uart, 0, &ch, 1)!=1)
				{
					LOG_E("error");
				}
				cvhandle(ch, b);
		}
		rt_thread_mdelay(1);
	}
}


rt_thread_t test1;
//创建动态线程
int test1_init(void)
{
	test1=rt_thread_create("Test",
													test1_entry,RT_NULL,
													4096,9,
													1);
	if(test1!=NULL)//返回值正确，启动该线程
	{
		rt_thread_startup(test1);
	}
	return 0;
}

INIT_APP_EXPORT(test1_init);//自动初始化


INIT_DEVICE_EXPORT(visual_uart_init);
