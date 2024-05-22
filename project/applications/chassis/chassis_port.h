/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 21:53:16
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-22 22:20:19
 * @FilePath: \project\applications\chassis\chassis_port.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef CHASSIS_PORT_H
#define CHASSIS_PORT_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "chassis.h"
#include "abus.h"
#include "abus_echo.h"
#pragma pack(1)

    ABUS_DATA_TYPE_CREATE(
        int type; // 0speed 1pos
        float x;
        float y;
        float w;
        , chassis_ctrl_t);

    ABUS_DATA_TYPE_CREATE(
        float pos_x;
        float pos_y;
        float pos_w;

        float speed_x;
        float speed_y;
        float speed_w;
        , chassis_state_t);

#pragma pack()
#ifdef __cplusplus
}
#endif
#endif
