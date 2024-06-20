/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 11:19:26
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-06-20 20:07:10
 * @FilePath: \project\applications\chassis\chassis_cfg.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef CHASSIS_CFG_H
#define CHASSIS_CFG_H

#include "ulog.h"
/* 启用标准电机抽象层 */
#define CHASSIS_USING_MOTOR_HAL
/* 启用内置麦轮解算module */
#define CHASSIS_MODULE_MAI

#define CHASSIS_MODULE_OMNI3
#define CHASSIS_MODULE_OMNI4




#ifdef CHASSIS_USING_MOTOR_HAL /*USE CHASSIS_USING_MOTOR_HAL */
#include "motor.h"
#ifdef CHASSIS_MODULE_MAI
/**
 *          车轮半径r
 *
 *          x y轴定义 车轮旋转正方向 电机编号 确定
 *
 *       // 四轮麦克纳姆轮底盘    采用abab  x型
 *       //俯视车底盘      
 *                    y
 *                   ^          a轴
 *       1轮 //      |     \\   0轮
 *          //       |      \\
 *                   |       
 *                   |_ _ _ _ _ _ > x
 *
 *         \\                //
 *      2轮 \\              //   3轮
 *                              b轴
 */
/* 用户配置使用的电机id */
//#define MOTOR_MAI_ID_1 M2006_1_CAN1
//#define MOTOR_MAI_ID_2 M2006_2_CAN1
//#define MOTOR_MAI_ID_3 M2006_3_CAN1
//#define MOTOR_MAI_ID_4 M2006_4_CAN1

#define MOTOR_MAI_ID_1 0
#define MOTOR_MAI_ID_2 0
#define MOTOR_MAI_ID_3 0
#define MOTOR_MAI_ID_4 0
#endif


#ifdef CHASSIS_MODULE_OMNI3
/**
 *          y
 *          ^
 *          |
 * b -------|------- a
 *  \       |       /
 *   \      |      /
 *    \     |_____/__________>x    
 *     \        /
 *      \      /
 *       \    /       
 *        \  /
 *         c
 * 
 * 
 */
#define MOTOR_OMNI3_ID_1 0
#define MOTOR_OMNI3_ID_2 0
#define MOTOR_OMNI3_ID_3 0

#endif


#ifdef CHASSIS_MODULE_OMNI4
/**
 *          y
 *          ^
 *          |
 * b -------|------- a
 *  \       |       /
 *   \      |      /
 *    \     |_____/__________>x    
 *     \        /
 *      \      /
 *       \    /       
 *        \  /
 *         c
 * 
 * 
 */
#define MOTOR_OMNI4_ID_1 0
#define MOTOR_OMNI4_ID_2 0
#define MOTOR_OMNI4_ID_3 0
#define MOTOR_OMNI4_ID_3 0

#endif



#endif
#endif /* CHASSIS_CFG_H */