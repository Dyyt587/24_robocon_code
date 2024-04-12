
#define DBG_TAG "Chassis.mai_ops"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>
#include "chassis_module_mai_ops.h"
#include "math.h"

#ifdef CHASSIS_MODULE_MAI
int module_mai_ops(struct chassis *chassis, const void *output, const void *input, chassis_status require_cmd);
#ifdef CHASSIS_USING_MOTOR_HAL
static int driver_mai_ops(struct chassis *chassis, const void *output, const void *input, chassis_status require_cmd);
#endif

chassis_mai_data_t mai_data1;

chassis_ops_t ops_mai_ops = {
    .module = module_mai_ops,
#ifdef CHASSIS_USING_MOTOR_HAL
    .driver = driver_mai_ops,
#else
    .driver = NULL,
#endif
    .date = &mai_data1};

// #define CHASSIS_MAI_WHELL_R_M (0.036804f) // 3个都差1mm 导致地盘旋转180会有5.6度偏差，，，测量误差！！！！！没想过这些会影响这么大，，学会使用工具和是适当的替换距离
#define CHASSIS_MAI_WHELL_R_M (0.034704f) // 3个都差1mm 导致地盘旋转180会有5.6度偏差，，，测量误差！！！！！没想过这些会影响这么大，，学会使用工具和是适当的替换距离
//#define CHASSIS_MAI_WHELL_R_M (0.034904f) // 3个都差1mm 导致地盘旋转180会有5.6度偏差，，，测量误差！！！！！没想过这些会影响这么大，，学会使用工具和是适当的替换距离
#define CHSSIS_MAI_A_M (0.129f)
#define CHSSIS_MAI_B_M (0.17906f)
#define PI (3.14159265359f)
#define CHASSIS_HALF_A_B (0.5 * (CHSSIS_MAI_A_M + CHSSIS_MAI_B_M))
#define CHASSIS_A_B ((CHSSIS_MAI_A_M + CHSSIS_MAI_B_M))
#define COS45 (0.70710678118)
#define SIN45 (0.70710678118)
#define CHASSIS_2PIR (6.28318530718 * CHASSIS_MAI_WHELL_R_M)
#define CHASSIS_R (sqrt((CHSSIS_MAI_A_M / 2.f) * (CHSSIS_MAI_A_M / 2.f) + (CHSSIS_MAI_B_M / 2.f) * (CHSSIS_MAI_B_M / 2.f)))

#define conversion (180.f / CHASSIS_MAI_WHELL_R_M / PI)

extern float pos_x;
extern float pos_y;
extern float zangle;
extern float xangle;
extern float yangle;
extern float w_z;


apid_t *pid_ops_x = APID_CREATE_STATIC_ANONYMOUS_POSITION(1.0,1.0,1.0);
apid_t *pid_ops_y = APID_CREATE_STATIC_ANONYMOUS_POSITION(1.0,1.0,1.0);
apid_t *pid_ops_w = APID_CREATE_STATIC_ANONYMOUS_POSITION(1.0,1.0,1.0);
int module_mai_ops(struct chassis *chassis, const void *output, const void *input, chassis_status require_cmd)
{
    if (output != NULL)
    {
        chassis_mai_data_t *data = (chassis_mai_data_t *)output;
        data->type = require_cmd;
        switch (require_cmd)
        {
        case CHASSIS_SPEED:
            // 速度控制
            data->motor1 = -(((chassis->target.speed.x_m_s + chassis->offset.speed.x_m_s) - (chassis->target.speed.y_m_s + chassis->offset.speed.y_m_s)) - ((chassis->target.speed.z_rad_s + chassis->offset.speed.z_rad_s) * CHASSIS_HALF_A_B)) / CHASSIS_2PIR * 60.f;
            data->motor2 = -(((chassis->target.speed.x_m_s + chassis->offset.speed.x_m_s) + (chassis->target.speed.y_m_s + chassis->offset.speed.y_m_s)) - ((chassis->target.speed.z_rad_s + chassis->offset.speed.z_rad_s) * CHASSIS_HALF_A_B)) / CHASSIS_2PIR * 60.f;
            data->motor3 = -(((chassis->target.speed.x_m_s + chassis->offset.speed.x_m_s) + (chassis->target.speed.y_m_s + chassis->offset.speed.y_m_s)) - ((chassis->target.speed.z_rad_s + chassis->offset.speed.z_rad_s) * CHASSIS_HALF_A_B)) / CHASSIS_2PIR * 60.f;
            data->motor4 = -(((chassis->target.speed.x_m_s + chassis->offset.speed.x_m_s) - (chassis->target.speed.y_m_s + chassis->offset.speed.y_m_s)) - ((chassis->target.speed.z_rad_s + chassis->offset.speed.z_rad_s) * CHASSIS_HALF_A_B)) / CHASSIS_2PIR * 60.f;
            break;
        case CHASSIS_POS:
            // 位置控制,使用ops当前值作为输入，速度为输出

            


            data->motor1 = (((chassis->target.pos.x_m + chassis->offset.pos.x_m) + (chassis->target.pos.y_m + chassis->offset.pos.y_m)) - ((chassis->target.pos.z_rad + chassis->offset.pos.z_rad) * CHASSIS_HALF_A_B)) * conversion;  // 430  chassis->target.pos.z_rad  CHASSIS_R
            data->motor2 = (((chassis->target.pos.x_m + chassis->offset.pos.x_m) - (chassis->target.pos.y_m + chassis->offset.pos.y_m)) - ((chassis->target.pos.z_rad + chassis->offset.pos.z_rad) * CHASSIS_HALF_A_B)) * conversion;  //-430
            data->motor3 = ((-(chassis->target.pos.x_m + chassis->offset.pos.x_m) - (chassis->target.pos.y_m + chassis->offset.pos.y_m)) - ((chassis->target.pos.z_rad + chassis->offset.pos.z_rad) * CHASSIS_HALF_A_B)) * conversion; //-430          1433
            data->motor4 = ((-(chassis->target.pos.x_m + chassis->offset.pos.x_m) + (chassis->target.pos.y_m + chassis->offset.pos.y_m)) - ((chassis->target.pos.z_rad + chassis->offset.pos.z_rad) * CHASSIS_HALF_A_B)) * conversion; // 430
            break;
        default:
            break;
        }
    }
    if (input != NULL)
    {
        chassis_mai_data_t *data = (chassis_mai_data_t *)input;
        switch (require_cmd)
        {
        case CHASSIS_SPEED:
            // 速度控制
            chassis->present.speed.x_m_s = chassis->offset.speed.x_m_s + ((data->motor1 - data->motor2) / CHASSIS_2PIR * 60.f) / 2.f;
            chassis->present.speed.y_m_s = chassis->offset.speed.y_m_s + ((-data->motor2 + data->motor4) / CHASSIS_2PIR * 60.f) / 2.f;
            chassis->present.speed.z_rad_s = chassis->offset.speed.z_rad_s + (-(data->motor3 + data->motor2) / CHASSIS_2PIR * 60.f) / (2.f * (CHASSIS_HALF_A_B));
            break;
        case CHASSIS_POS:

            // 位置控制
            // chassis->present.pos.x_m = ((data->motor1 + data->motor2) /  ( conversion)) / 2.f;
            // chassis->present.pos.y_m = ((-data->motor2 + data->motor4 ) /( conversion)) / 2.f;
            // chassis->present.pos.z_rad = (-(-data->motor3 + data->motor2) /( conversion)) / (2.f * CHASSIS_HALF_A_B);
            chassis->present.pos.x_m = pos_x;
            chassis->present.pos.y_m = pos_y;
            chassis->present.pos.z_rad = w_z;
            LOG_D("xm:%f,ym:%f,zrad:%f", chassis->present.pos.x_m, chassis->present.pos.y_m, chassis->present.pos.z_rad);

            break;
        default:

            break;
        }
        return 0;
    }
    return 0;
}
#ifdef CHASSIS_USING_MOTOR_HAL
static int driver_mai_ops(struct chassis *chassis, const void *output, const void *input, chassis_status require_cmd)
{
    if (input != NULL)
    {
        // 读取电机数据输出
        chassis_mai_data_t *data = (chassis_mai_data_t *)input;
        switch (require_cmd)
        {
        case CHASSIS_SPEED:
            // 速度控制
            // LOG_D("speed get motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            data->motor1 = motor_get_speed(MOTOR_MAI_ID_1);
            data->motor2 = motor_get_speed(MOTOR_MAI_ID_2);
            data->motor3 = motor_get_speed(MOTOR_MAI_ID_3);
            data->motor4 = motor_get_speed(MOTOR_MAI_ID_4);
            break;
        case CHASSIS_POS:
            // 位置控制,不需要driver提供
            // LOG_D("pos get motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            data->motor1 = 0.f;
            data->motor2 = 0.f;
            data->motor3 = 0.f;
            data->motor4 = 0.f;
            break;
        default:
            break;
        }
    }
    if (output != NULL)
    {
        // 写入电机数据
        chassis_mai_data_t *data = (chassis_mai_data_t *)output;
        switch (require_cmd)
        {
        case CHASSIS_SPEED:
            // 速度控制
            // LOG_D("speed set motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            motor_set_speed(MOTOR_MAI_ID_1, data->motor1);
            motor_set_speed(MOTOR_MAI_ID_2, data->motor2);
            motor_set_speed(MOTOR_MAI_ID_3, data->motor3);
            motor_set_speed(MOTOR_MAI_ID_4, data->motor4);
            break;
        case CHASSIS_POS:
            // 位置控制
            // LOG_D("pos set motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            motor_set_speed(MOTOR_MAI_ID_1, data->motor1);
            motor_set_speed(MOTOR_MAI_ID_2, data->motor2);
            motor_set_speed(MOTOR_MAI_ID_3, data->motor3);
            motor_set_speed(MOTOR_MAI_ID_4, data->motor4);
            break;
        default:
            break;
        }
    }
    return 0;
}
#endif
#endif
