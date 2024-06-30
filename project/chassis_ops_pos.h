#ifndef CHASSIS_OPS_POS_H
#define CHASSIS_OPS_POS_H
#include "chassis.h"
void chassis_ops_handle(int cycle);
void chassis_ops_set_pos(chassis_pos_t* pos);

void chassis_ops_relative_move(float x,float y,float z);
#endif
