#ifndef CHASSIS_MODULE_OMINI3_H
#define CHASSIS_MODULE_OMINI3_H
#include "chassis.h"

#ifdef __cplusplus
extern "C" {
#endif

extern chassis_ops_t ops_omni3;
typedef struct chassis_omni3_data
{
    chassis_status type;
    float motora;
    float motorb;
    float motorc;

} chassis_omni3_data_t;
int module_omni3(struct chassis *chassis, const void *output, const void *input,chassis_status require_cmd);

#ifdef __cplusplus
}
#endif
#endif
