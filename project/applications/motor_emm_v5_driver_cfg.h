/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-06-19 18:21:34
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-06-19 19:53:50
 * @FilePath: \project\applications\motor_emm_v5_driver_cfg.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef MOTOR_EMM_V5_DRIVER_CFG_H
#define MOTOR_EMM_V5_DRIVER_CFG_H
#if defined(__cplusplus)
extern "C" {
#endif
#include "motor_emm_v5_driver.h"
   /**
     * @brief 电机抽象层电机操作函数定义
     */

#ifdef MOTOR_EMMV5_ID1_SERIAL1
#define MOTOR_EMMV5_ID1_SERIAL1_OPS            \
    {                                               \
        .curve =NULL,                               \
        .driver = motor_emm_v5_driver,              \
        .control = motor_emm_v5_ctr,                \
        .user_data = &dj_motors[SP_M_SERIAL1_ID1],  \
    }
#endif
	
	
#if defined(__cplusplus)
}
#endif
#endif
