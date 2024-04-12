#include "drv_action.h"
#include <rtdbg.h>
#define DBG_TAG "drv.action.ops"
#define DBG_LVL DBG_DBG
 float pos_x = 0;
 float pos_y = 0;
 float zangle = 0;
 float xangle = 0;
 float yangle = 0;
 float w_z = 0;
static rt_device_t uart = 0;

typedef struct
{
    float x_m;
    float y_m;
} action_pos_t;

typedef struct
{
    float zangle;
    float xangle;
    float yangle;
} action_angle_t;

/**
 * @brief 数据解析函数  如更换MCU平台或更换软件库，只需将串口接收到的值传入该函数即可解析
 * @param  rec 串口接收到的字节数据
 */
void Data_Analyse(uint8_t rec)
{
    static uint8_t ch;
    static union
    {
        uint8_t date[24];
        float ActVal[6];
    } posture;
    static uint8_t count = 0;
    static uint8_t i = 0;

    ch = rec;
    switch (count)
    {
    case 0:
        if (ch == 0x0d)
            count++;
        else
            count = 0;
        break;
    case 1:
        if (ch == 0x0a)
        {
            i = 0;
            count++;
        }
        else if (ch == 0x0d)
            ;
        else
            count = 0;
        break;
    case 2:
        posture.date[i] = ch;
        i++;
        if (i >= 24)
        {
            i = 0;
            count++;
        }
        break;
    case 3:
        if (ch == 0x0a)
            count++;
        else
            count = 0;
        break;
    case 4:
        if (ch == 0x0d)
        {
            zangle = posture.ActVal[0];
            xangle = posture.ActVal[1];
            yangle = posture.ActVal[2];
            pos_x = posture.ActVal[3];
            pos_y = posture.ActVal[4];
            w_z = posture.ActVal[5];
        }
        count = 0;
        break;
    default:
        count = 0;
        break;
    }
}



void Action_set_zero(void)
{
    rt_device_write(uart, 0, "ACTO", sizeof("ACTO"));
}



static rt_err_t dbus_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    uint8_t data = 0;
    while (size--)
    {
        if (rt_device_read(dev, 0, &data, 1) != 1)
        {
            return 0;
        }
        Data_Analyse(data);
    }
		return 0;
}
int Action_Init(void)
{
    uart = rt_device_find("uart7");
    if (uart == RT_NULL)
    {
        LOG_E("uart1 not found");
        return -1;
    }
    if (rt_device_open(uart, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        LOG_E("uart1 open failed");
        return -1;
    }

    if (rt_device_set_rx_indicate(uart, dbus_uart_rx_ind) != RT_EOK)
    {
        LOG_E("uart1 set rx indicate failed");
        return -1;
    }
    return 0;
}
INIT_COMPONENT_EXPORT(Action_Init);
