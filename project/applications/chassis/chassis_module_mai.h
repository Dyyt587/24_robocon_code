/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 11:08:39
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-16 21:48:11
 * @FilePath: \project\applications\chassis\chassis_module_mai.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef CHASSIS_MODULE_MAI_H
#define CHASSIS_MODULE_MAI_H
#ifdef __cplusplus
extern "C" {
#endif
	#include "chassis.h"

extern chassis_ops_t ops_mai;
typedef struct chassis_mai_data
{
    chassis_status type;
    float motor1;
    float motor2;
    float motor3;
    float motor4;

} chassis_mai_data_t;
int module_mai(struct chassis *chassis, const void *output, const void *input,chassis_status require_cmd);
#ifdef __cplusplus
}
#endif
#endif
