/**
 * @file map_ctrl.c
 * @author Dyyt587 (67887002+Dyyt587@users.noreply.github.com)
 * @brief 用于在地图坐标系下控制车辆
 * @version 0.1
 * @date 2024-05-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "map_ctrl.h"
#include "chassis.h"
typedef struct{
    float x;//平移x
    float y;//平移y
    float rota;//旋转角
}map_ctrl_pose_t;
typedef struct{
    chassis_t *chassis;//被控底盘
    map_ctrl_pose_t pose_trans;//指示位姿转换
}map_ctrl_t;
void map_ctrl_set_init_pose(map_ctrl_t* map_ctrl,float x,float y,float rota)
{
    if(map_ctrl==0){
        while(1);
    }
    map_ctrl->pose_trans.x = x;
    map_ctrl->pose_trans.y = y;
    map_ctrl->pose_trans.rota = rota;

}
void map_ctrl_init(map_ctrl_t* map_ctrl,chassis_t* chassis) {

    map_ctrl->chassis = chassis;
    map_ctrl->pose_trans.x = 0;
    map_ctrl->pose_trans.y = 0;
    map_ctrl->pose_trans.rota = 0;
    if(chassis==0){
        LOG_E("chassis is null");
    }
}
