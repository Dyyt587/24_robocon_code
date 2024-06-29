#ifndef INTEL_ROS_H
#define INTEL_ROS_H

#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"
#include "chassis_module_omni4.h"
#include "chassis.h"
#include "upacker.h"
#include <stdint.h>

typedef struct ros_data
{
    float pos_x;
    float pos_y;
    float speed_x;
    float speed_y;
    float z_rad;
}ros_data;

typedef struct uart_data
{
    uint8_t dat1[20];
    uint8_t dat2[20];
    uint8_t dat3[20];
    uint8_t dat4[20];
    uint8_t dat5[20];
}uart_data;

typedef struct{
    uint8_t id;
    float hu;
    float a;
    int b;
}xxdate;



typedef struct{
    char new_pkg;
    char id_tap[20];
    float x;
    float y;
    float k;
}visual_date_t;
#endif