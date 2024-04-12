/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 21:53:16
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-03-19 10:55:56
 * @FilePath: \project\applications\chassis\chassis_port.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef CHASSIS_PORT_H
#define CHASSIS_PORT_H
#ifdef __cplusplus
extern "C" {
#endif
#include "chassis.h"
#include "abus_topic.h"
typedef struct{
    uint8_t type;//0 speed 1 pos
    union{
        chassis_speed_t speed;
        chassis_pos_t pos;
    };
}chassis_ctrl_t;

#ifdef __cplusplus
}
#endif
#endif
