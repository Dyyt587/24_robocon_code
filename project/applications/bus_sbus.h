/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-03-22 21:49:31
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-03-31 01:00:24
 * @FilePath: \construction_robot\project\applications\bus_sbus.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __BUS_SBUS_H
#define __BUS_SBUS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define UART_RX_DMA_SIZE (1024)
#define DBUS_MAX_LEN     (25)
#define DJI 0

#if DJI
#define DBUS_BUFLEN      (18)
#else
#define DBUS_BUFLEN      (25)
#endif



#define DBUS_HUART       huart1 /* for dji remote controler reciever */

typedef struct
{
    /* rocker channel information */
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    int16_t ch4;
    int16_t ch5;
    int16_t ch6;
    int16_t ch7;
    int16_t ch8;
    int16_t ch9;
    int16_t ch10;
    int16_t ch11;
    int16_t ch12;
    int16_t ch13;
    int16_t ch14;
    int16_t ch15;
    int16_t ch16;
    /* left and right lever information */
    uint8_t sw1;
    uint8_t sw2;
} rc_info_t;



//void dbus_uart_init(void);
void sw_judge(rc_info_t *rc);
const rc_info_t *get_remote_control_point(void);

#ifdef __cplusplus
}
#endif
#endif