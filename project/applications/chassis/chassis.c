/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 09:10:28
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-04-28 19:58:40
 * @FilePath: \project\applications\chassis\chassis.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "chassis.h"
#include "stdint.h"
#include "stddef.h"
#include "ulog.h"
#include "apid.h"
#include "float.h"

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

void chassis_init_curve(CurveObjectType *curve)
{
    curve->targetPos = 0;
    curve->startPos = 0.0f; // 初始位置
    curve->currentPos = 0.0f;
    curve->stepPos = 0.1f; // 位置变化的步长
    //curve->max_pos = 1500.0f;
    curve->PosMax = FLT_MAX;  // 最大位置限制
    curve->PosMin = -FLT_MAX; // 最小位置限制
    curve->aTimes = 0;               // 当前时间步
    curve->maxTimes = 500;           // 总时间步，实际使用时需要根据实际情况计算
    curve->curveMode = CURVE_SPTA;   // 使用S型曲线
    curve->flexible = 10.f;          // S曲线的柔性因子
}

void chassis_planning_(CurveObjectType *curve, float *out)
{
    if (curve->maxTimes)
    {
        mine_plan(curve);
        *out = curve->currentPos;
    }
}

int chassis_set_pos_plan(chassis_t *chassis, chassis_pos_t *data)
{
    chassis->target.pos = *data;

    chassis->ops->plan_init(chassis,&chassis->present,&chassis->target);
    chassis->ops->plan(chassis,&chassis->target);
    chassis->run_status = CHASSIS_POS;

    LOG_D("target_x %f target_y %f target_w %f",chassis->target.pos.x_m,chassis->target.pos.y_m,chassis->target.pos.z_rad);
		return 0;


}
int chassis_set_speed_plan(chassis_t *chassis, chassis_speed_t *data)
{
	return 0;
}
const chassis_pos_t *chassis_get_pos(chassis_t *chassis)
{
    /* 返回当前位置 */

    return &chassis->present.pos;
}
void chassis_pos_clean(chassis_t *chassis)
{
    chassis->present.pos = chassis->target.pos;
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
    // chassis->offset.pos.z_rad = 0.5 * (chassis->target.pos.z_rad + (zangle * 3.1415926f / 180.f));

    chassis->offset.pos.x_m = 0;
    chassis->offset.pos.y_m = 0;
    chassis->offset.pos.z_rad = 0;
}


void chassis_planning_if(chassis_t *chassis, int time_ms)
{
   // chassis_check_planning(chassis);
    if (chassis->plan.is_planning)
    {
        chassis->ops->plan(chassis,&chassis->target);
        // chassis_planning_(&chassis->plan.curve_x, &chassis->target.pos.x_m);
        // chassis_planning_(&chassis->plan.curve_y, &chassis->target.pos.y_m);
        // chassis_planning_(&chassis->plan.curve_w, &chassis->target.pos.z_rad);
        // chassis->run_status = CHASSIS_POS;
    }
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
	    static int cnt=0;

    if (chassis == NULL)
    {
        CHASSIS_LOG("chassis is NULL\n");
        return -1;
    }
    chassis_updata_speed(chassis, time_ms);
    chassis_updata_pos(chassis, time_ms);

    chassis_updata_offset(chassis, time_ms);
    chassis_planning_if(chassis, time_ms);
    /* 根据状态执行相应的操作 */
    switch (chassis->run_status)
    {
    case CHASSIS_IDLE:
    if(cnt++%10==0){
        CHASSIS_LOG("chassis status IDEL\n",1);
    }
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