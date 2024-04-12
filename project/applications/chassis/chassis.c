/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 09:10:28
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-04 19:48:48
 * @FilePath: \project\applications\chassis\chassis.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "chassis.h"
#include "stdint.h"
#include "stddef.h"
#include "ulog.h"
#include "apid.h"

apid_t pid_offset;
int chassis_init(chassis_t *chassis, chassis_ops_t *ops)
{
    chassis->ops = ops;
    chassis->run_status = CHASSIS_IDLE;
    return 0;
}

int chassis_set_pos(chassis_t *chassis, chassis_pos_t *data)
{
    /* 将数据更新到target 改变status */
    chassis->target.pos = *data;
    chassis->run_status = CHASSIS_POS;
    return 0;
}

int chassis_set_speed(chassis_t *chassis, chassis_speed_t *data)
{
    /* 将数据更新到target 改变status */
    chassis->target.speed = *data;
    chassis->run_status = CHASSIS_SPEED;
    return 0;
}

const chassis_pos_t *chassis_get_pos(chassis_t *chassis)
{
    /* 返回当前位置 */

    return &chassis->present.pos;
}
void chassis_pos_clean(chassis_t *chassis)
{
    chassis->present.pos=chassis->target.pos;
}
const chassis_speed_t *chassis_get_speed(chassis_t *chassis)
{
    /* 返回当前速度 */
    return &chassis->present.speed;
}

void chassis_updata_speed(chassis_t *chassis, int time_ms)
{
    /* 更新当前数据 */
    const void *output = chassis->ops->date;
    chassis->ops->driver(chassis, NULL, output, CHASSIS_SPEED);
    chassis->ops->module(chassis, NULL, output, CHASSIS_SPEED);
}

void chassis_updata_pos(chassis_t *chassis, int time_ms)
{
    /* 更新当前数据 */
    const void *output = chassis->ops->date;
    chassis->ops->driver(chassis, NULL, output, CHASSIS_POS);
    chassis->ops->module(chassis, NULL, output, CHASSIS_POS);
}

void chassis_drv_set_speed(chassis_t *chassis)
{
    /* 设置数据 */
    const void *output = chassis->ops->date;
    chassis->ops->module(chassis, output, NULL, CHASSIS_SPEED);
    chassis->ops->driver(chassis, output, NULL, CHASSIS_SPEED);
}

void chassis_drv_set_pos(chassis_t *chassis)
{
    /* 设置数据 */
    const void *output = chassis->ops->date;
    chassis->ops->module(chassis, output, NULL, CHASSIS_POS);
    chassis->ops->driver(chassis, output, NULL, CHASSIS_POS);
}

/**
 * @brief 更新底盘误差值
 *
 * @param chassis
 * @param time_ms
 */
rt_weak void chassis_updata_offset(chassis_t *chassis, int time_ms)
{
    // 设置底盘偏移量
    extern float pos_x;
    extern float pos_y;
    extern float zangle;
    extern float xangle;
    extern float yangle;
    extern float w_z;
    // chassis->offset.pos.x_m = 0.8 * (chassis->target.pos.x_m + (pos_x / 1000.f));
    // chassis->offset.pos.y_m = 0.8 * (chassis->target.pos.y_m - (pos_y / 1000.f));
    //chassis->offset.pos.z_rad = 0.5 * (chassis->target.pos.z_rad + (zangle * 3.1415926f / 180.f));

chassis->offset.pos.x_m = 0;
chassis->offset.pos.y_m = 0;
chassis->offset.pos.z_rad =0;

}

/**
 * @brief 底盘抽象层的处理函数
 *
 * @param chassis 底盘对象
 * @param time_ms 全局时间，单位ms
 * @return int
 */
int chassis_handle(chassis_t *chassis, int time_ms)
{
    if (chassis == NULL)
    {
        CHASSIS_LOG("chassis is NULL\n");
        return -1;
    }
    chassis_updata_speed(chassis, time_ms);
    chassis_updata_pos(chassis, time_ms);

    chassis_updata_offset(chassis, time_ms);
    /* 根据状态执行相应的操作 */
    switch (chassis->run_status)
    {
    case CHASSIS_IDLE:
        break;
    case CHASSIS_SPEED:
        chassis_drv_set_speed(chassis);
        break;
    case CHASSIS_POS:
        chassis_drv_set_pos(chassis);
        break;
    default:
        CHASSIS_LOG("chassis status error(%d)\n", chassis->run_status);
        break;
    }
    return 0;
}

const chassis_status chassis_get_runstate(chassis_t *chassis)
{
    return chassis->run_status;
}