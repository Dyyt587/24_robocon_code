#ifndef APID_AUTO_TUNE_H
#define APID_AUTO_TUNE_H
#ifdef __cplusplus
extern "C"
{
#endif


#include "main.h"
#include "motor.h"
#include "apid.h"



    enum CONTRL_TYPE
    {
        SCALAR = 0, // 分级
        VECTOR = 1, // 矢量
    };
    enum state
    {
        STATE_Init = 0,
        STATE_PreHeat,
        STATE_Inertia,
        STATE_PreShake,
        STATE_Shake,
        STATE_Analyse,
        STATE_End
    };
    typedef struct Auto_PID_t
    {
        uint32_t Timing;                // ��������ʱ��
        int16_t ControlSet;             // ������Ŀ������
        int16_t SampleCheck[10];        // ����
        int16_t SamplePeakRecord[2][3]; // ������ֵ��¼[0]=ʱ�䣬[1]=�¶ȣ�
        enum state State;                    // ���ڽ׶�
        uint8_t State_History;          // ��ʷ�׶�
        uint8_t Channel;                // ����ͨ��
        int16_t ControlOut;             // ����Ŀ���ֵ
        float Pu;
        float A;
        float Ku;
        float Proportion;      // ��������Proportional Const
        float Integration;     // ���ֳ���Integral Const
        float Differentiation; // ΢�ֳ���Derivative Const
    } Auto_PID;
    typedef void (*apid_auto_tune_ctrol)(float speed_);
    typedef struct apid_auto
    {
        unsigned char control_type; // 控制模式

        apid_t* pid;

        float set_Value;
        float get_Value;
        //float cycle;

        float frequent;

        Auto_PID pid_params;
        //apid_auto_tune_ctrol ctrl;
        uint8_t cnt;

    } apid_auto_t;

    void Handle_Auto_PID(apid_auto_t*pid_auto,float set_Value, float get_Value, float cycle);
    void auto_pid_init(apid_t *apid,apid_auto_t *apid_auto, uint8_t type, uint16_t frequent,PID_TYPE target);

#ifdef __cplusplus
}
#endif
#endif
