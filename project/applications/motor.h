/**
 * @file motor.h
 * @author Dyyt587 (805207319@qq.com)
 * @brief 电机通用驱动
 * @version 0.1
 * @date 2024-02-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef MOTOR_H
#define MOTOR_H
#if defined(__cplusplus)
extern "C"
{
#endif
#include "apid.h"
#include "stdbool.h"
#include "motor_cfg.h"
#include <stdint.h>
#include <stdlib.h>
#include "motor_dj_rm_driver.h"
#include "motor_planning.h"

/* Compiler Related Definitions */
#include "rtcompiler.h"

#define MOTOR_USING_AUTO_INIT   /* 自动初始化 */
#define MOTOR_DEBUGING_AUTO_INIT  

    typedef enum
    {
        MOTOR_MODE_IDEL = 0U,   /* 空闲 */
        MOTOR_MODE_TORQUE = 1U, /* 力矩 */
        MOTOR_MODE_SPEED = 2U,  /* 速度 rpm 转每分钟 */
        MOTOR_MODE_POS = 3U,    /* 位置 rad */
        MOTOR_MODE_VOLTAGE,     /* 电压 mv 毫伏 */
        MOTOR_MODE_TEMP,        /* 温度 °C */

        MOTOR_MODE_SAFETY_STOP,  /* 紧急关闭 */
        MOTOR_MODE_SAFETY_START, /* 安全启动 */
        MOTOR_MODE_MAX,
    } MOTOR_VALUE_TYPE;

    enum
    {
        MOTOR_CONTROL_SUPPORT_NONE = 0, // 硬件支持
        MOTOR_CONTROL_SUPPORT_TORQUE,   // 硬件支持
        MOTOR_CONTROL_SUPPORT_SPEED,    // 速度支持
        MOTOR_CONTROL_SUPPORT_POS,      // 位置支持
    };
    typedef struct motor motor_t;
    typedef int (*motor_driver)(int id, uint16_t mode, float *value, void *user_data);
    typedef int (*motor_ctr)(int id, uint16_t mode, float *data);
    typedef int (*motor_behiver)(int id, uint16_t mode, void *data, void *user_data);
    typedef void (*motor_shakedown)(int id, motor_t *motor);
    typedef struct
    {
        motor_driver driver;
        motor_ctr control;
        void *user_data;
    } motor_ops_t;
    struct motor
    {
        motor_ops_t *ops;
        int id;
        const char *name;
        
        long long time;
        uint8_t ratio_pos:3;
        uint8_t ratio_speed:3;
        uint8_t torque_tick:2;
        apid_t *pid_speed;
        apid_t *pid_pos;
        apid_t *pid_torque;

        float tar_speed;
        float tar_pos;
        float tar_torque;

        float cur_speed;
        float cur_pos;
        float cur_torque;

        uint8_t flag_run_mode : 2;         // 记录当前运行模式
        uint8_t flag_out_mode : 2;         // 记录当前输出模式
        uint8_t flag_accept_level : 2;     // 记录允许的模式 0 - 3
        uint8_t flag_passive_feedback : 1; // 用户自行反馈当前值
        uint8_t flag_is_stop : 1;          // 记录支持的模式 0 - 3

        motor_behiver behaver;
        float acc_out;

        uint8_t timeout_cnt;

        motor_planning* plan;

        float ratio;/*减速比*/
    };

    motor_t *motor_get(int id);
    int motor_get_id(const char *name);

    int motor_handle(int id, float cycle);
    void motor_init(void);

    int motor_read_feedback(int id, int cycle);

    void motor_set_passive_feedback(int id, bool is_true);

    void motor_set_ratio(int id, float ratio);


    int motor_feedback_torque(int id, float value);
    int motor_feedback_speed(int id, float value);
    int motor_feedback_pos(int id, float value);

    int motor_stop(int id);
    int motor_start(int id);

    int motor_set_speed(int id, float value);

    int motor_set_pos(int id, float value);

    int motor_set_torque(int id, float value);

    float motor_get_speed(int id);

    float motor_get_pos(int id);

    float motor_get_torque(int id);

    apid_t *motor_get_pid_torque(int id);
    apid_t *motor_get_pid_speed(int id);
    apid_t *motor_get_pid_pos(int id);

    void motor_shakdown(int id);

    int motor_control(int id, MOTOR_VALUE_TYPE mode, void *data);

    void motor_set_pid_speed_ratio(int id,uint8_t ratio_speed,uint8_t ratio_pos);

#if defined(__cplusplus)
}
#endif
#endif