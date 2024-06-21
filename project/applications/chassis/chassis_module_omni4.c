#include "chassis_module_omni4.h"
#include "math.h"

#define DBG_TAG "Chassis.omni4"
#define DBG_LVL DBG_DBG
#include <ulog.h>

#ifdef CHASSIS_MODULE_MAI
int module_omni4(struct chassis *chassis, const void *output, const void *input, chassis_status require_cmd);
#ifdef CHASSIS_USING_MOTOR_HAL
static int driver_omni4(struct chassis *chassis, const void *output, const void *input, chassis_status require_cmd);
#endif

chassis_omni4_data_t omni4_data;

chassis_ops_t ops_omni4 = {
    .module = module_omni4,
#ifdef CHASSIS_USING_MOTOR_HAL
    .driver = driver_omni4,
#else
    .driver = NULL,
#endif
    .date = &omni4_data};

#define GEN3 (1.73205)
#undef M_PI
#define M_PI 3.141592653589793238462643383279502884
#define L 1.71  //车外接圆半径
#define R 0.5 //车轮半径
#define M2W_SPEED(motor_speed)    (2*M_PI*R*(motor_speed)/60)             //电机速度到底盘速度的转换
#define M2W_POS(motor_pos)    M2W_SPEED(motor_pos)            //电机速度到底盘速度的转换

#define W2M_SPEED(wheel_speed)    (((wheel_speed)*60)/(2*M_PI*R))             //电机速度到底盘速度的转换
#define W2M_POS(wheel_pos)    M2W_SPEED(wheel_pos)            //电机速度到底盘速度的转换


int module_omni4(struct chassis *chassis, const void *output, const void *input, chassis_status require_cmd)
{
    if (output != NULL)
    {
        chassis_omni4_data_t *data = (chassis_omni4_data_t *)output;
        data->type = require_cmd;
        switch (require_cmd)
        {
        case CHASSIS_SPEED:
            // 速度控制
            data->motora = W2M_SPEED(-(chassis->target.speed.x_m_s + chassis->offset.speed.x_m_s) +(chassis->target.speed.y_m_s + chassis->offset.speed.y_m_s)+ L*(chassis->target.speed.z_rad_s+chassis->offset.speed.z_rad_s));
            data->motorb = W2M_SPEED((chassis->target.speed.x_m_s + chassis->offset.speed.x_m_s) +(chassis->target.speed.y_m_s + chassis->offset.speed.y_m_s)- L*(chassis->target.speed.z_rad_s+chassis->offset.speed.z_rad_s));
            data->motorc = W2M_SPEED(-(chassis->target.speed.x_m_s + chassis->offset.speed.x_m_s) +(chassis->target.speed.y_m_s + chassis->offset.speed.y_m_s)- L*(chassis->target.speed.z_rad_s+chassis->offset.speed.z_rad_s));
            data->motord = W2M_SPEED((chassis->target.speed.x_m_s + chassis->offset.speed.x_m_s) +(chassis->target.speed.y_m_s + chassis->offset.speed.y_m_s)+ L*(chassis->target.speed.z_rad_s+chassis->offset.speed.z_rad_s));

				break;
        case CHASSIS_POS:
            // 位置控制

            data->motora = W2M_SPEED(-(chassis->target.pos.x_m + chassis->offset.pos.x_m) +(chassis->target.pos.y_m + chassis->offset.pos.y_m)+ L*(chassis->target.pos.z_rad+chassis->offset.pos.z_rad));
            data->motorb = W2M_SPEED((chassis->target.pos.x_m + chassis->offset.pos.x_m) +(chassis->target.pos.y_m + chassis->offset.pos.y_m)- L*(chassis->target.pos.z_rad+chassis->offset.pos.z_rad));
            data->motorc = W2M_SPEED(-(chassis->target.pos.x_m + chassis->offset.pos.x_m) +(chassis->target.pos.y_m + chassis->offset.pos.y_m)- L*(chassis->target.pos.z_rad+chassis->offset.pos.z_rad));
            data->motord = W2M_SPEED((chassis->target.pos.x_m + chassis->offset.pos.x_m) +(chassis->target.pos.y_m + chassis->offset.pos.y_m)+ L*(chassis->target.pos.z_rad+chassis->offset.pos.z_rad));

            break;
        default:
            break;
        }
    }
    if (input != NULL)
    {
        chassis_omni4_data_t *data = (chassis_omni4_data_t *)input;
        switch (require_cmd)
        {
        case CHASSIS_SPEED:
            // 速度控制
            chassis->present.speed.x_m_s = chassis->offset.speed.x_m_s + (GEN3*M2W_SPEED((data->motora - data->motorb))) / 3 ;
            chassis->present.speed.y_m_s = chassis->offset.speed.y_m_s + M2W_SPEED((data->motora + data->motorb - 2*data->motorc)) / 3;
            chassis->present.speed.z_rad_s = chassis->offset.speed.z_rad_s + M2W_SPEED((data->motora + data->motorb + data->motorc)) / (3*L);
            break;
        case CHASSIS_POS:
            // 位置控制
            chassis->present.pos.x_m = chassis->offset.speed.x_m_s + (GEN3*M2W_POS((data->motora - data->motorb))) / 3 ;
            chassis->present.pos.y_m = chassis->offset.speed.y_m_s + M2W_POS((data->motora + data->motorb - 2*data->motorc)) / 3;
            chassis->present.pos.z_rad = chassis->offset.speed.z_rad_s + M2W_POS((data->motora + data->motorb + data->motorc)) / (3*L);
            //LOG_D("xm:%f,ym:%f,zrad:%f", chassis->present.pos.x_m, chassis->present.pos.y_m, chassis->present.pos.z_rad);

            break;
        default:

            break;
        }
        return 0;
    }
    return 0;
}
#ifdef CHASSIS_USING_MOTOR_HAL
static int driver_omni4(struct chassis *chassis, const void *output, const void *input, chassis_status require_cmd)
{
    if (input != NULL)
    {
        // 读取电机数据输出
        chassis_omni4_data_t *data = (chassis_omni4_data_t *)input;
        switch (require_cmd)
        {
        case CHASSIS_SPEED:
            // 速度控制
            // LOG_D("speed get motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            data->motora = motor_get_speed(MOTOR_OMNI4_ID_1);
            data->motorb = motor_get_speed(MOTOR_OMNI4_ID_2);
            data->motorc = motor_get_speed(MOTOR_OMNI4_ID_3);
            data->motord = motor_get_speed(MOTOR_OMNI4_ID_4);
            break;
        case CHASSIS_POS:
            // 位置控制
            // LOG_D("pos get motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
             data->motora = motor_get_pos(MOTOR_OMNI4_ID_1);
             data->motorb = motor_get_pos(MOTOR_OMNI4_ID_2);
             data->motorc = motor_get_pos(MOTOR_OMNI4_ID_3);
             data->motord = motor_get_pos(MOTOR_OMNI4_ID_4);
            break;
        default:
            break;
        }
    }
    if (output != NULL)
    {
        // 写入电机数据
        chassis_omni4_data_t *data = (chassis_omni4_data_t *)output;
        switch (require_cmd)
        {
        case CHASSIS_SPEED:
            // 速度控制
            //LOG_D("speed set motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            motor_set_speed(MOTOR_MAI_ID_1, data->motora);
            motor_set_speed(MOTOR_MAI_ID_2, data->motorb);
            motor_set_speed(MOTOR_MAI_ID_3, data->motorc);
            motor_set_speed(MOTOR_MAI_ID_4, data->motord);
            break;
        case CHASSIS_POS:
            // 位置控制
            // LOG_D("pos set motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            motor_set_pos(MOTOR_MAI_ID_1, data->motora);
            motor_set_pos(MOTOR_MAI_ID_2, data->motorb);
            motor_set_pos(MOTOR_MAI_ID_3, data->motorc);
            motor_set_pos(MOTOR_MAI_ID_4, data->motord);
            break;
        default:
            break;
        }
    }
    return 0;
}
#endif
#endif
