#include "bus_sbus.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
uint8_t dbus_buf[DBUS_MAX_LEN];
static rc_info_t rc;
static void sw_judge(rc_info_t *rc);
#include "bus_sbus.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

#define DBG_TAG "drv.dbus"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

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

#endif
	if (rc->ch1 < min && rc->ch1 > -min)
		rc->ch1 = 0;
	if (rc->ch2 < min && rc->ch2 > -min)
		rc->ch2 = 0;
	if (rc->ch3 < min && rc->ch3 > -min)
		rc->ch3 = 0;
	if (rc->ch4 < min && rc->ch4 > -min)
		rc->ch4 = 0;
	LOG_D("\nrc: %d %d %d %d\r\n%d %d %d %d\r\n%d %d %d %d\r\n%d %d %d %d",
	rc->ch1, rc->ch2, rc->ch3, rc->ch4, rc->ch5, rc->ch6, rc->ch7, rc->ch8, rc->ch9, rc->ch10, rc->ch11, rc->ch12, rc->ch13, rc->ch14, rc->ch15, rc->ch16);
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
	static uint8_t state = 0;
	if (state == 0)
	{
		rt_device_read(uart, 0, dbus_buf, 1);
		if (dbus_buf[0] == 0x0f)
		{
			state = 1;
		}
	}
	else if (state == 1) /* 接收到包头*/
	{
		if (size >= 24)
		{
			rt_device_read(uart, 0, dbus_buf + 1, 24);
			state = 0;
			rc_callback_handler(&rc, dbus_buf);
		}
	}
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

const rc_info_t*dbus_get_info(void)
{
	return &rc;
}
/**
 * @brief   initialize dbus uart device
 * @param
 * @retval
 */
int dbus_uart_init(void)
{
	uart = rt_device_find("uart1");
	if (uart == RT_NULL)
	{
		LOG_E("uart1(sbus) not found");
		return -1;
	}
	if (rt_device_open(uart, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_RX_NON_BLOCKING) != RT_EOK)
	{
		LOG_E("uart1(sbus) open failed");
		return -1;
	}
	struct serial_configure config = {
		100000,						/* 115200 bits/s */
		DATA_BITS_9,				/* 8 databits */
		STOP_BITS_1,				/* 1 stopbit */
		PARITY_EVEN,				/* No parity  */
		BIT_ORDER_LSB,				/* LSB first sent */
		NRZ_NORMAL,					/* Normal mode */
		4096,		/* rxBuf size */
		4096,		/* txBuf size */
		RT_SERIAL_FLOWCONTROL_NONE, /* Off flowcontrol */
		0};
	if (RT_EOK != rt_device_control(uart, RT_DEVICE_CTRL_CONFIG, &config))
	{
		rt_kprintf("change %s failed!\n", uart->parent.name);
	}

	if (rt_device_set_rx_indicate(uart, dbus_uart_rx_ind) != RT_EOK)
	{
		LOG_E("uart1(sbus) set rx indicate failed");
		return -1;
	}
	return 0;
}
INIT_COMPONENT_EXPORT(dbus_uart_init);

static void dbus(int argc, char**argv)
{

    LOG_RAW("rc:\n\t 1 - 2 - 3 - 4 \n\t %d %d %d %d\r\n\t%d %d %d %d\r\n\t%d %d %d %d\r\n\t%d %d %d %d\r\n",
		rc.ch1, rc.ch2, rc.ch3, rc.ch4, rc.ch5, rc.ch6, rc.ch7, rc.ch8, rc.ch9, rc.ch10, rc.ch11, rc.ch12, rc.ch13, rc.ch14, rc.ch15, rc.ch16);


}
MSH_CMD_EXPORT(dbus, see dbus state);
