/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-16 20:40:11
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-17 01:35:41
 * @FilePath: \24_robocon_code\project\applications\motor_planning.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __MOTOR_PLANNING_H
#define __MOTOR_PLANNING_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define MOTOR_PLANNING_USING_PERFCOUNTER


#define POW_2(x) (x*x)
typedef struct{
    float speed;
    float position;
    int32_t time_ms;//全局时间
}motor_point_t;

typedef struct{
    float max_velocity;
    float max_accelerated_speed;//最大加速度
    float max_decelerated_speed;//最大减速度
    motor_point_t* start;//开始状态
    motor_point_t* end;//结束状态

    motor_point_t* present;// 
    motor_point_t* reference;// 

    
}motor_planning;




#ifdef __cplusplus
}
#endif
#endif
