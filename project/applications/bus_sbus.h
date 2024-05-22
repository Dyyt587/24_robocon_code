/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-03-22 21:49:31
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-10 17:20:13
 * @FilePath: \construction_robot\project\applications\bus_sbus.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __BUS_SBUS_H
#define __BUS_SBUS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define RC_BUF_SIZE 32
#define RC_CHENNEL_NUM 14
typedef struct
{
    uint8_t new_pkg;
    uint8_t buf[RC_BUF_SIZE];
    uint16_t chs[RC_CHENNEL_NUM];
    uint8_t flag;
} rc_t;

#ifdef __cplusplus
}
#endif
#endif