#include "apid_auto_tune_Relayfeedback.h"
#include "stdio.h"
const uint16_t FREQUENT = 200;         //????????HZ??
const uint16_t TIME = 1000 / FREQUENT; //???е????(ms)
    const float PID_AT_PreStop = 0; // 目标到达死区
    const uint16_t MAX_OUT = 5000;  // �������ֵ
apid_auto_t _auto_pid;
//(SCALAR, FREQUENT);  //??????????????????HZ??
float returned_autopid_pram[3];

void __auto_pid_handle(apid_t *pid, PID_TYPE cycle)
{
    Handle_Auto_PID(pid->auto_pid, APID_Get_Target(pid), APID_Get_Present(pid), cycle);
}
void return_auto_pid(void)
{
    returned_autopid_pram[0] = _auto_pid.pid->parameter.kp;
    returned_autopid_pram[1] = _auto_pid.pid->parameter.ki;
    returned_autopid_pram[2] = _auto_pid.pid->parameter.kd;
}
void auto_pid_init(apid_t *apid,apid_auto_t *apid_auto, uint8_t type, uint16_t frequent,PID_TYPE target)
{
    apid_auto->control_type = type;
    apid_auto->frequent = (float)frequent;
    // apid_auto->ctrl = ctrl;
    APID_Set_Target(apid, target);
    apid_auto->pid = apid;
    apid->auto_pid = apid;
    apid->auto_pid_handler = __auto_pid_handle;
    return_auto_pid();

}
void auto_pid_deinit(apid_t *apid)
{
    apid->auto_pid=0;
    apid->auto_pid_handler=0;
    APID_Enable(apid);
}

// void my_app_init()
// {
//     //_auto_pid.set_Value = 7200; // 设置目标值
//     //_auto_pid.cycle = 5;
//     // PID_Init(&_auto_pid.pid, PID_POSITION_NULL, 1.0f, 1.f, 1.f); // 随便赋值
//     // PID_Sst_Out_Limit(&_auto_pid.pid, 10000);
//     // PID_Sst_Integral_Limit(&_auto_pid.pid, 2000);
//     // PID_Sst_Bias_Dead_Zone(&_auto_pid.pid, 20);
//     // PID_Sst_Target(&_auto_pid.pid, _auto_pid.set_Value);
// }
// void my_app_run(void)
// {
// }
// void my_app_update(void)
// {
//     // 用户编写获取当前值
//     // _auto_pid.get_Value = DJ_Get_Motor_Speed(DJ_CAN1_M0, DJ_M2006) * 36;
// }
// void motor_Ctrol(float value)
// {
//     // 用户编写设置输出值
//     // dj_set_motor_Group_A(&hcan1, (int16_t)speed_, 0, 0, 0);
// }

// void run_auto_pid(void)
// {
//     static uint8_t cnt = 0;
//     cnt++;
//     static uint8_t flag_init = 1;
//     if (flag_init)
//     {
//         my_app_init();
//         flag_init = 0;
//     }
//     if (cnt > TIME)
//     { // 5ms???
//         cnt = 0;
//         my_app_update();//获取当前值
//         my_app_run();//运行自动pid计算
//     }
// }
//		extern void run_auto_pid(void);
//    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim) {
//        if (htim == (&htim1)) {
//           run_auto_pid();
//        }

// 以下用户不需要改变
/**
 * @brief pid整定函数
 *
 * @param apid_auto_tune 句柄
 * @param target 目标值
 * @param present 当前值
 * @param cycle 周期
 */
void Handle_Auto_PID(apid_auto_t *apid_auto_tune, float target, float present, float cycle)
{

    apid_auto_tune->pid_params.ControlSet = target;
    // float present = present;
    static uint16_t flag_n = 0;
    // 记录10个数据
    apid_auto_tune->pid_params.SampleCheck[9] = apid_auto_tune->pid_params.SampleCheck[8];
    apid_auto_tune->pid_params.SampleCheck[8] = apid_auto_tune->pid_params.SampleCheck[7];
    apid_auto_tune->pid_params.SampleCheck[7] = apid_auto_tune->pid_params.SampleCheck[6];
    apid_auto_tune->pid_params.SampleCheck[6] = apid_auto_tune->pid_params.SampleCheck[5];
    apid_auto_tune->pid_params.SampleCheck[5] = apid_auto_tune->pid_params.SampleCheck[4];
    apid_auto_tune->pid_params.SampleCheck[4] = apid_auto_tune->pid_params.SampleCheck[3];
    apid_auto_tune->pid_params.SampleCheck[3] = apid_auto_tune->pid_params.SampleCheck[2];
    apid_auto_tune->pid_params.SampleCheck[2] = apid_auto_tune->pid_params.SampleCheck[1];
    apid_auto_tune->pid_params.SampleCheck[1] = apid_auto_tune->pid_params.SampleCheck[0];
    apid_auto_tune->pid_params.SampleCheck[0] = present;

    // 记录上一个状态
    if (apid_auto_tune->pid_params.State != apid_auto_tune->pid_params.State_History)
    {
        apid_auto_tune->pid_params.State_History = apid_auto_tune->pid_params.State;
    }
    switch (apid_auto_tune->pid_params.State)
    {
    case STATE_Init:
        apid_auto_tune->cnt = 0;

        apid_auto_tune->pid_params.Timing = 0;
        apid_auto_tune->pid_params.ControlSet = 0;
        // 初始化缓冲区
        for (uint8_t i = 0; i < 10; i++)
        {
            apid_auto_tune->pid_params.SampleCheck[i] = 0;
        }
        // 峰值记录初始化
        for (uint8_t n = 0; n < 3; n++)
        {
            apid_auto_tune->pid_params.SamplePeakRecord[0][n] = 0;
            apid_auto_tune->pid_params.SamplePeakRecord[1][n] = 0;
        }
        apid_auto_tune->pid_params.State = STATE_Init;
        apid_auto_tune->pid_params.State_History = STATE_Init;
        apid_auto_tune->pid_params.Ku = 0;
        apid_auto_tune->pid_params.Pu = 0;
        apid_auto_tune->pid_params.A = 0; //

        apid_auto_tune->pid_params.State = STATE_PreHeat; //

        break;
    case STATE_PreHeat:

        if ((present + PID_AT_PreStop) < apid_auto_tune->pid_params.ControlSet)
        {
            // PID计算并加载输出
            // _auto_pid.pid->parameter.present = present;
            // APID_Set_Present(_auto_pid.pid,present);
            // PID_Hander(&_auto_pid.pid, cycle);
            // motor_Ctrol(_auto_pid.pid->parameter.out);
            apid_auto_tune->pid_params.ControlOut = APID_Get_Out(_auto_pid.pid);
        }
        else
        {
            apid_auto_tune->pid_params.ControlOut = 0;        // 清空输出
            apid_auto_tune->pid_params.State = STATE_Inertia; // 进入惯性状态
        }
        if (apid_auto_tune->control_type == VECTOR)
        { // 矢量控制
            apid_auto_tune->pid_params.State = STATE_Inertia;
            apid_auto_tune->pid_params.ControlOut = 0;
        }
        break;
    case STATE_Inertia:

        if (apid_auto_tune->pid_params.SampleCheck[0] < apid_auto_tune->pid_params.SampleCheck[9])
        {
            apid_auto_tune->pid_params.State = STATE_PreShake; // 获取到最大值，进入前震荡
        }

        break;
    // 准备震荡
    case STATE_PreShake:

        if (present < apid_auto_tune->pid_params.ControlSet)
        {
            apid_auto_tune->pid_params.ControlOut = MAX_OUT; // 向上继电器切换
            flag_n = 2;
        }
        else
        {
            apid_auto_tune->pid_params.ControlOut = -MAX_OUT; // 向下继电器切换
            switch (flag_n)
            {
            case 0:
                flag_n = 1;
                break;
            case 2:
            {
                flag_n = 0;
                apid_auto_tune->pid_params.Timing = 0;
                apid_auto_tune->pid_params.State = STATE_Shake; // 完成一个切换
            }
            break;
            default:
                break;
            }
        }

    //????????
    case STATE_Shake:

        apid_auto_tune->pid_params.Timing++; // 切换加一
        if (present < apid_auto_tune->pid_params.ControlSet)
        {
            apid_auto_tune->pid_params.ControlOut = MAX_OUT;
            if (flag_n == 0)
            {
                flag_n = 1;
                apid_auto_tune->pid_params.SamplePeakRecord[0][flag_n] = apid_auto_tune->pid_params.Timing;
                apid_auto_tune->pid_params.SamplePeakRecord[1][flag_n] = present;
            }
            //??????????
            if (present < apid_auto_tune->pid_params.SamplePeakRecord[1][flag_n])
            {
                apid_auto_tune->pid_params.SamplePeakRecord[0][flag_n] = apid_auto_tune->pid_params.Timing;
                apid_auto_tune->pid_params.SamplePeakRecord[1][flag_n] = present;
            }
        }
        else
        {
            apid_auto_tune->pid_params.ControlOut = -MAX_OUT; //??????????
            if (flag_n == 1)
            {
                flag_n = 2;
                apid_auto_tune->pid_params.SamplePeakRecord[0][flag_n] = apid_auto_tune->pid_params.Timing;
                apid_auto_tune->pid_params.SamplePeakRecord[1][flag_n] = present;
            }
            //????岨??
            if (present > apid_auto_tune->pid_params.SamplePeakRecord[1][flag_n])
            {
                apid_auto_tune->pid_params.SamplePeakRecord[0][flag_n] = apid_auto_tune->pid_params.Timing;
                apid_auto_tune->pid_params.SamplePeakRecord[1][flag_n] = present;
            }
            if ((flag_n == 2) && (apid_auto_tune->pid_params.SampleCheck[0] < apid_auto_tune->pid_params.SampleCheck[9]))
            {
                apid_auto_tune->pid_params.State = STATE_Analyse;
                apid_auto_tune->pid_params.ControlOut = 0;
            }
        }

        break;
    // 开始分析数据
    case STATE_Analyse:

        /**分析参数**/
        apid_auto_tune->pid_params.A = apid_auto_tune->pid_params.SamplePeakRecord[1][2] - apid_auto_tune->pid_params.SamplePeakRecord[1][1];
        apid_auto_tune->pid_params.Pu = (apid_auto_tune->pid_params.SamplePeakRecord[0][2] - apid_auto_tune->pid_params.SamplePeakRecord[0][1]) / apid_auto_tune->frequent; //??λms->s
        apid_auto_tune->pid_params.Ku = 4 * MAX_OUT / (apid_auto_tune->pid_params.A * 3.14159f);

        apid_auto_tune->pid_params.Proportion = 0.6f * apid_auto_tune->pid_params.Ku;
        apid_auto_tune->pid_params.Integration = 1.2f * apid_auto_tune->pid_params.Ku / apid_auto_tune->pid_params.A;
        apid_auto_tune->pid_params.Differentiation = 0.075f * apid_auto_tune->pid_params.Ku * apid_auto_tune->pid_params.Pu;
        apid_auto_tune->pid_params.State = STATE_End;

        break;
    case STATE_End: // 整定结束，加载参数
    {
        // if (apid_auto_tune->cnt == 0) /*only load once*/
        // {
            apid_auto_tune->cnt++;
            const float param[3] = {apid_auto_tune->pid_params.Proportion, apid_auto_tune->pid_params.Integration, apid_auto_tune->pid_params.Differentiation};

            apid_auto_tune->pid->parameter.kp = param[0];
            apid_auto_tune->pid->parameter.ki = param[1];
            apid_auto_tune->pid->parameter.kd = param[2];
            apid_auto_tune ->pid =  NULL;
            
            auto_pid_deinit(apid_auto_tune->pid);
       // }
    }
    break;
    default:
        break;
    }
    if (apid_auto_tune->pid_params.State != STATE_End)
    {
        APID_Pause(apid_auto_tune->pid);
        //覆盖输出
        apid_auto_tune->pid->parameter.out = apid_auto_tune->pid_params.ControlOut;
    }
    else
    {
        APID_Enable(apid_auto_tune->pid);

        // _auto_pid.pid->parameter.present = present;
        // //PID_Hander(&_auto_pid.pid, cycle);
        // motor_Ctrol(_auto_pid.pid->parameter.out); //????PID?????
        return_auto_pid();
    }
}

// end of the file !!!
