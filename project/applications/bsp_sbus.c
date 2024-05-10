
#include "bus_sbus.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

// #define DBG_TAG "drv.dbus"
// #define DBG_LVL DBG_DBG
// #include <rtdbg.h>
#include "ulog.h"
uint8_t dbus_buf[DBUS_MAX_LEN];
static rc_info_t rc;
static void sw_judge(rc_info_t *rc);
static int flag = 0;
static uint8_t state = 0;
static rt_sem_t sbus_sem;
#define myabs(x) x > 0 ? x : -x
static rt_device_t uart = 0;
/**
 * @brief       handle received rc data
 * @param[out]  rc:   structure to save handled rc data
 * @param[in]   buff: the buff which saved raw rc data
 * @retval
 */
void rc_callback_handler(rc_info_t *rc, uint8_t *buff)
{
	int min = 15;

#if DJI
	rc->ch1 = (buff[0] | buff[1] << 8) & 0x07FF;
	rc->ch1 -= 1024;
	rc->ch2 = (buff[1] >> 3 | buff[2] << 5) & 0x07FF;
	rc->ch2 -= 1024;
	rc->ch3 = (buff[2] >> 6 | buff[3] << 2 | buff[4] << 10) & 0x07FF;
	rc->ch3 -= 1024;
	rc->ch4 = (buff[4] >> 1 | buff[5] << 7) & 0x07FF;
	rc->ch4 -= 1024;

	rc->ch5 = ((buff[5] >> 4) & 0x000C) >> 2;
	rc->ch6 = (buff[5] >> 4) & 0x0003;

#else
	// if(buff[24]==0){
	rc->ch1 = ((uint16_t)buff[1]) | ((uint16_t)((buff[2] & 0x07) << 8));
	rc->ch2 = ((uint16_t)((buff[2] & 0xf8) >> 3)) | (((uint16_t)(buff[3] & 0x3f)) << 5);
	rc->ch3 = ((uint16_t)((buff[3] & 0xc0) >> 6)) | ((((uint16_t)buff[4]) << 2)) | (((uint16_t)(buff[5] & 0x01)) << 10);
	rc->ch4 = ((uint16_t)((buff[5] & 0xfe) >> 1)) | (((uint16_t)(buff[6] & 0x0f)) << 7);
	rc->ch5 = ((uint16_t)((buff[6] & 0xf0) >> 4)) | (((uint16_t)(buff[7] & 0x7f)) << 4);
	rc->ch6 = ((uint16_t)((buff[7] & 0x80) >> 7)) | (((uint16_t)buff[8]) << 1) | (((uint16_t)(buff[9] & 0x03)) << 9);
	rc->ch7 = ((uint16_t)((buff[9] & 0xfc) >> 2)) | (((uint16_t)(buff[10] & 0x1f)) << 6);
	rc->ch8 = ((uint16_t)((buff[10] & 0xe0) >> 5)) | (((uint16_t)(buff[11])) << 3);
	rc->ch9 = ((uint16_t)buff[12]) | (((uint16_t)(buff[13] & 0x07)) << 8);
	rc->ch10 = ((uint16_t)((buff[13] & 0xf8) >> 3)) | (((uint16_t)(buff[14] & 0x3f)) << 5);
	rc->ch11 = ((uint16_t)((buff[14] & 0xc0) >> 6)) | (((uint16_t)buff[15]) << 2) | (((uint16_t)(buff[16] & 0x01)) << 10);
	rc->ch12 = ((uint16_t)((buff[16] & 0xfe) >> 1)) | (((uint16_t)(buff[17] & 0x0f)) << 7);
	rc->ch13 = ((uint16_t)((buff[17] & 0xf0) >> 4)) | (((uint16_t)(buff[18] & 0x7f)) << 4);
	rc->ch14 = ((uint16_t)((buff[18] & 0x80) >> 7)) | (((uint16_t)buff[19]) << 1) | (((uint16_t)(buff[20] & 0x03)) << 9);
	rc->ch15 = ((uint16_t)((buff[20] & 0xfc) >> 2)) | (((uint16_t)(buff[21] & 0x1f)) << 6);
	rc->ch16 = ((uint16_t)((buff[21] & 0xe0) >> 5)) | (((uint16_t)buff[22]) << 3);

	rc->ch1 -= 1024;
	rc->ch2 -= 1024;
	rc->ch3 -= 1024;
	rc->ch4 -= 1024;
	rc->ch13 -= 1024;
	rc->ch14 -= 1024;
	rc->ch15 -= 1024;
	rc->ch16 -= 1024;

	sw_judge(rc);
//}else{

// LOG_E("sbus receiver error");
// }
#endif
	if (rc->ch1 < min && rc->ch1 > -min)
		rc->ch1 = 0;
	if (rc->ch2 < min && rc->ch2 > -min)
		rc->ch2 = 0;
	if (rc->ch3 < min && rc->ch3 > -min)
		rc->ch3 = 0;
	if (rc->ch4 < min && rc->ch4 > -min)
		rc->ch4 = 0;
	//	LOG_D("\nrc: %d %d %d %d\r\n%d %d %d %d\r\n%d %d %d %d\r\n%d %d %d %d",
	//	rc->ch1, rc->ch2, rc->ch3, rc->ch4, rc->ch5, rc->ch6, rc->ch7, rc->ch8, rc->ch9, rc->ch10, rc->ch11, rc->ch12, rc->ch13, rc->ch14, rc->ch15, rc->ch16);
	//	//LOG_D("bug[25] %d",dbus_buf[24]);
}



uint8_t ibus_buf[32];

void ibus_in(rc_t *rc, uint8_t data)
{

    if (rc->flag == 0)
    {
        if (data == 0x20)
        {
            rc->buf[rc->flag] = data;
            rc->flag = 1;
        }
    }
    else if (rc->flag == 1)
    {
        if (data == 0x40)
        {
            rc->buf[rc->flag] = data;
            rc->flag = 2;
        }
        else
        {
            flag = 0;
        }
    }
    if (rc->flag >= 2)
    {
        // 一直接收数据
        rc->buf[flag++] = data;
        if (rc->flag == 32)
        {
            // 开始解析一包数据并且清空flag
            uint16_t checksum_cal = 0xffff - rc->buf[0] - rc->buf[1];

            for (int i = 0; i < RC_CHENNEL_NUM; i++)
            {
                rc->ch[i] = (uint16_t)(rc->buf[i * 2 + 3] << 8 | rc->buf[i * 2 + 2]);
                checksum_cal = checksum_cal - rc->buf[i * 2 + 3] - rc->buf[i * 2 + 2];
            }

            checksum_ibus = rc->buf[31] << 8 | rc->buf[30];

            if (checksum_cal == checksum_ibus)
            {
                for (int j = 0; j < RC_CHENNEL_NUM; j++)
                {
                    rc->chs[j] = rc->ch[j];
                }
            }
            rc->new_pkg = 1;
            /*一包数据解析*/
            rc->flag = 0;
        }
    }
}
/**
 * @brief
 *
 * @param dev
 * @param size
 * @return rt_err_t
 */
rt_err_t dbus_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(sbus_sem);
	return 0;
}

void sw_judge(rc_info_t *rc)
{
	if (rc->ch5 > 1600)
		rc->ch5 = 1;
	else if (rc->ch5 < 400)
		rc->ch5 = 3;
	else
		rc->ch5 = 2;
	if (rc->ch6 > 1600)
		rc->ch6 = 1;
	else if (rc->ch6 < 400)
		rc->ch6 = 3;
	else
		rc->ch6 = 2;
	if (rc->ch7 > 1600)
		rc->ch7 = 1;
	else if (rc->ch7 < 400)
		rc->ch7 = 3;
	else
		rc->ch7 = 2;
	if (rc->ch8 > 1600)
		rc->ch8 = 1;
	else if (rc->ch8 < 400)
		rc->ch8 = 3;
	else
		rc->ch8 = 2;
	if (rc->ch9 > 1600)
		rc->ch9 = 1;
	else if (rc->ch9 < 400)
		rc->ch9 = 3;
	else
		rc->ch9 = 2;
	if (rc->ch10 > 1600)
		rc->ch10 = 1;
	else if (rc->ch10 < 400)
		rc->ch10 = 3;
	else
		rc->ch10 = 2;
	if (rc->ch11 > 1600)
		rc->ch11 = 1;
	else if (rc->ch11 < 400)
		rc->ch11 = 3;
	else
		rc->ch11 = 2;
	if (rc->ch12 > 1600)
		rc->ch12 = 1;
	else if (rc->ch12 < 400)
		rc->ch12 = 3;
	else
		rc->ch12 = 2;
};

const rc_info_t *dbus_get_info(void)
{
	return &rc;
}
rc_t rc;
void dbus_handle(void *d)
{
	if (rt_device_set_rx_indicate(uart, dbus_uart_rx_ind) != RT_EOK)
	{
		LOG_E("uart1(sbus) set rx indicate failed");
		// return -1;
	}
	uint8_t buf=0;
	while (1)
	{
		if (rt_device_read(uart, 0, &buf,1)==0)
		{
			rt_sem_take(sbus_sem, RT_WAITING_FOREVER);
			continue;
		}
		ibus_in(&rc, buf);
		// LOG_D("buf 0x%x",buf);
		// if(buf == 0x20){
		// 	LOG_D("ibus start");
		// }	
	}
}
/**
 * @brief   initialize dbus uart device
 * @param
 * @retval
 */
int dbus_uart_init(void)
{
	uart = rt_device_find("uart8");
	if (uart == RT_NULL)
	{
		LOG_E("uart8(ibus) not found");
		return -1;
	}
	if (rt_device_open(uart, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_RX_NON_BLOCKING) != RT_EOK)
	{
		LOG_E("uart8(ibus) open failed");
		return -1;
	}
	//			DATA_BITS_8,				/* 8 databits */
	//		STOP_BITS_2,				/* 1 stopbit */
	//		PARITY_EVEN,				/* No parity  */
	struct serial_configure config = {
		115200,						/* 115200 bits/s */
		DATA_BITS_8,				/* 8 databits */
		STOP_BITS_1,				/* 1 stopbit */
		PARITY_NONE,				/* No parity  */
		BIT_ORDER_LSB,				/* LSB first sent */
		NRZ_NORMAL,					/* Normal mode */
		4096,						/* rxBuf size */
		4096,						/* txBuf size */
		RT_SERIAL_FLOWCONTROL_NONE, /* Off flowcontrol */
		0};
	if (RT_EOK != rt_device_control(uart, RT_DEVICE_CTRL_CONFIG, &config))
	{
		rt_kprintf("change %s failed!\n", uart->parent.name);
	}

	rt_thread_t tid_chassis = RT_NULL;

	/* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
	tid_chassis = rt_thread_create("ibus",
								   dbus_handle, RT_NULL,
								   4096,
								   8, 1);
	sbus_sem = rt_sem_create("dbus_sem", 0, RT_IPC_FLAG_FIFO);

	/* 线程创建成功，则启动线程 */
	if (tid_chassis != RT_NULL)
	{
		rt_thread_startup(tid_chassis);
	}
	return 0;

	return 0;
}
INIT_COMPONENT_EXPORT(dbus_uart_init);

static void dbus(int argc, char **argv)
{

	LOG_RAW("rc:\n\t 1 - 2 - 3 - 4 \n\t %d %d %d %d\r\n\t%d %d %d %d\r\n\t%d %d %d %d\r\n\t%d %d %d %d\r\n",
			rc.ch1, rc.ch2, rc.ch3, rc.ch4, rc.ch5, rc.ch6, rc.ch7, rc.ch8, rc.ch9, rc.ch10, rc.ch11, rc.ch12, rc.ch13, rc.ch14, rc.ch15, rc.ch16);
}
MSH_CMD_EXPORT(dbus, see dbus state);
