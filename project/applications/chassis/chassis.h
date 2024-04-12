/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 09:10:35
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-05 23:37:15
 * @FilePath: \project\applications\chassis\chassis.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef CHASSIS_H
#define CHASSIS_H
#ifdef __cpluscplus
extern "C" {
#endif
#include "chassis_cfg.h"

#define CHASSIS_LOG(fmt, ...) LOG_D(fmt, ##__VA_ARGS__)
struct chassis;
typedef enum chassis_status
{
    CHASSIS_IDLE,
    CHASSIS_SPEED,
    CHASSIS_POS,
} chassis_status;
typedef struct chassis_ops
{
    /*注意 这里负责传输的数据是由driver和module提供，即output由输出者提供，HAL不保存该过程数据，也不会干预*/
    /*module 负责将底盘的数据转换为车轮(电机)数据，并且通过output输出，将input数据转换为HAL层数据并且更新 */
    int (*module)(struct chassis *chassis, const void *output, const void *input,chassis_status require_cmd);
    /*driver 负责将input车轮(电机)的数据进行执行，并且通过output输出车轮(电机)的实时数据 */
    int (*driver)(struct chassis *chassis, const void *input, const void *output,chassis_status require_cmd);
    void* date;/* 用于保存相互传递的数据 */
} chassis_ops_t;

typedef struct chassis_speed
{
    float x_m_s;
    float y_m_s;
    float z_rad_s;

} chassis_speed_t;
typedef struct chassis_pos
{
    float x_m;
    float y_m;
    float z_rad;
} chassis_pos_t;
typedef struct chassis_data
{
    chassis_speed_t speed;
    chassis_pos_t pos;

} chassis_data_t;


typedef struct chassis
{
    chassis_data_t target;
    chassis_data_t present;
    chassis_status run_status;
    chassis_ops_t *ops;
    chassis_data_t offset;
} chassis_t;

int chassis_init(chassis_t *chassis, chassis_ops_t *ops);
int chassis_set_pos(chassis_t *chassis, chassis_pos_t *data);
int chassis_set_speed(chassis_t *chassis, chassis_speed_t *data);

const chassis_pos_t *chassis_get_pos(chassis_t *chassis);
const chassis_speed_t *chassis_get_speed(chassis_t *chassis);

const chassis_status chassis_get_runstate(chassis_t *chassis);

int chassis_handle(chassis_t *chassis, int time_ms);

void chassis_pos_clean(chassis_t *chassis);


#ifdef __cpluscplus
}
#endif
#endif
