#ifndef CHASSIS_MODULE_OMINI4_H
#define CHASSIS_MODULE_OMINI4_H
#include "chassis.h"

#ifdef __cplusplus
extern "C" {
#endif

extern chassis_ops_t ops_omni4;
typedef struct chassis_omni4_data
{
    chassis_status type;
    float motora;
    float motorb;
    float motorc;
    float motord;

} chassis_omni4_data_t;
int module_omni4(struct chassis *chassis, const void *output, const void *input,chassis_status require_cmd);

#ifdef __cplusplus
}
#endif
#endif
