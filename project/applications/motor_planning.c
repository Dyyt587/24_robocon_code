#include "motor_planning.h"
#include "motor.h"

#include <rtthread.h>
#include <float.h>

rt_weak int32_t motor_planning_get_time_ms(void)
{
#ifdef MOTOR_PLANNING_USING_PERFCOUNTER
    extern int32_t get_system_ms(void);
    return get_system_ms();
#else
    return rt_tick_get() * 1000 / RT_TICK_PER_SECOND;
#endif
}

float motor_cal_next_point(motor_planning *planning, int32_t interval_ms)
{
    // T型速度规划
    motor_point_t *present = planning->present;
    float delta_pos = planning->end->position - planning->start->position;
    if (delta_pos == 0)
        return planning->max_velocity;

    // 假设有匀速段
    float max_v = planning->max_velocity;
    float present_v = present->speed;
    float end_v = planning->end->speed;
    float acc_pos = (POW_2(max_v) - POW_2(present_v)) / (2.f * planning->max_accelerated_speed);
    float dec_pos = (POW_2(end_v) - POW_2(max_v)) / (2.f * planning->max_accelerated_speed);
    float max_acc = planning->max_accelerated_speed;//不带符号
    float max_dec = planning->max_decelerated_speed;

    if (fabs(acc_pos + dec_pos) > fabs(delta_pos))
    { /*只有加减速或纯加速或纯减速*/
        if (fabs(acc_pos) > fabs(delta_pos) || fabs(dec_pos) > fabs(delta_pos))
        {
            /*纯加纯减*/
        }
        else
        {
            /*加速 减速*/
            float vx = sqrt(((-2) * max_acc * (-max_dec) * delta_pos + max_acc * POW_2(end_v) - POW_2(present_v)) / (max_acc + max_dec));
            float acc_time = (-present_v + vx) / max_acc;
            float dec_time = (vx-present_v ) / max_dec;
            if (acc_time > interval_ms)
            {
                // 下一次来还会加速
                float vel_max = present_v + max_acc * interval_ms;
                return vel_max;
            }
            else
            {
                // 直接给最大限幅
                return planning->max_velocity;
            }
        }
    }
    else
    {//梯形 加速 减速 匀速
        float vel_max = present_v+max_acc*interval_ms;//加速
        if(vel_max>planning->max_velocity)vel_max=planning->max_velocity;//匀速
        return vel_max;
    }
}
/**
 * @brief 计算当前规划区间的速度限幅值
 *
 * @param planning 规划句柄
 * @param interval_ms 当前规划区间(未来下次调用距离这次的时间间隔)
 * @return float 限幅速度
 */
float motor_cal_speed(motor_t *motor, int32_t interval_ms)
{
    int32_t time = motor_planning_get_time_ms();

    if (motor->plan->start->time_ms > time)
        return FLT_MAX; // 还没有开始要规划

    // 更新电机当前参数
    motor->plan->present->position = motor->cur_pos;
    motor->plan->present->speed = motor->cur_speed;
    motor->plan->present->time_ms = time;
}
