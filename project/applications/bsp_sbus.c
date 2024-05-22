
#include "bus_sbus.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

// #define DBG_TAG "drv.dbus"
// #define DBG_LVL DBG_DBG
// #include <rtdbg.h>
#include "ulog.h"
rc_t rc1;
static uint8_t state = 0;
static rt_sem_t sbus_sem;
#define myabs(x) x > 0 ? x : -x
static rt_device_t uart = 0;

uint8_t ibus_buf[64];

void ibus_in(rc_t *rc, uint8_t data)
{

	if (rc->flag == 0)
	{
		if (data == 0x20)
		{
			rc->buf[rc->flag] = data;
			rc->flag = 1;
			return;
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
			rc->flag = 0;
		}
		return;
	}
	if (rc->flag >= 2)
	{
		// 一直接收数据
		rc->buf[rc->flag++] = data;
		if (rc->flag >= 32)
		{
			rc->flag = 0;
			// 开始解析一包数据并且清空flag
			uint16_t checksum_cal = 0xffff - rc->buf[0] - rc->buf[1];
			uint16_t ch[16];
			for (int i = 0; i < RC_CHENNEL_NUM; i++)
			{
				ch[i] = (uint16_t)(rc->buf[i * 2 + 3] << 8 | rc->buf[i * 2 + 2]);
				checksum_cal = checksum_cal - rc->buf[i * 2 + 3] - rc->buf[i * 2 + 2];
			}

			uint16_t checksum_ibus = rc->buf[31] << 8 | rc->buf[30];

			if (checksum_cal == checksum_ibus)
			{
				for (int j = 0; j < RC_CHENNEL_NUM; j++)
				{
					rc->chs[j] = ch[j];
				}
				//LOG_D("checked pkgs");
			}else{
				LOG_E("ibus checksum error");
			}
			rc->new_pkg = 1;
			/*一包数据解析*/
			rc->flag = 0;
			// LOG_D("new pkgs");
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

void dbus_handle(void *d)
{
	if (rt_device_set_rx_indicate(uart, dbus_uart_rx_ind) != RT_EOK)
	{
		LOG_E("uart1(sbus) set rx indicate failed");
		// return -1;
	}
	uint8_t buf = 0;
	while (1)
	{
		if (rt_device_read(uart, 0, &buf, 1) == 0)
		{
			rt_sem_take(sbus_sem, RT_WAITING_FOREVER);
			continue;
		}
		ibus_in(&rc1, buf);

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
	//		DATA_BITS_8,				/* 8 databits */
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
		4096,	
		/* txBuf size */
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

static void ibus(int argc, char **argv)
{

	LOG_RAW("rc:\n\t 1 - 2 - 3 - 4 \n\t %d %d %d %d\r\n\t%d %d %d %d\r\n\t%d %d %d %d\r\n\t%d %d %d %d\r\n",
			rc1.chs[0], rc1.chs[1], rc1.chs[2], rc1.chs[3], rc1.chs[4], rc1.chs[5], rc1.chs[6], rc1.chs[7], rc1.chs[8], rc1.chs[9], rc1.chs[10], rc1.chs[11], rc1.chs[12], rc1.chs[13], rc1.chs[14], rc1.chs[15]);
}
MSH_CMD_EXPORT(ibus, see ibus state);
