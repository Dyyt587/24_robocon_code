#include "Trajectory_planning.h"

void (*pCalCurve[])(CurveObjectType *curve) = {0, CalCurveSPTA, 0};

float motor_planning(CurveObjectType *curve)
{
    if (curve->targetPos > 0)
    {
        MotorVelocityCurve(curve);
    }
    else
    {
        curve->targetPos = -curve->targetPos;
        curve->currentPos = -curve->currentPos;
        MotorVelocityCurve(curve);
        curve->targetPos = -curve->targetPos;
        curve->currentPos = -curve->currentPos;
    }
    return curve->currentPos;
}

/* 电机曲线加减速操作-------------------------------------------------------- */
void MotorVelocityCurve(CurveObjectType *curve)
{
    float temp = 0;

    // if (curve->targetPos > curve->PosMax)
    // {
    //     curve->targetPos = curve->PosMax;
    // }

    // if (curve->targetPos < curve->PosMin)
    // {
    //     curve->targetPos = curve->PosMin;
    // }

    if ((fabs(curve->currentPos - curve->startPos) <= curve->stepPos) && (curve->maxTimes == 0))
    {
        /*自动计算最大时间长度*/
        // if (curve->startPos < curve->PosMin)
        // {
        //     curve->startPos = curve->PosMin;
        // }
        curve->maxTimes = (int)(((float)fabs(curve->targetPos - curve->startPos) / curve->stepPos) + 5) * curve->intervel;
        curve->aTimes = 0;
    }

    if (curve->aTimes < curve->maxTimes + 100)
    {
        /*单步计算*/
        pCalCurve[curve->curveMode](curve);
        curve->aTimes += curve->intervel;
    }
    else
    {
        curve->currentPos = curve->targetPos;
        curve->maxTimes = 0;
        curve->aTimes = 0;
    }
}

/*S型位置计算*/
static void CalCurveSPTA(CurveObjectType *spta)
{
    float power = 0.0;
    float Pos = 0.0;

    power = (2 * ((float)spta->aTimes) - ((float)spta->maxTimes)) / ((float)spta->maxTimes);
    power = (0.0f - ((float)spta->flexible)) * power;

    Pos = 1 + expf(power);
    Pos = (spta->targetPos - spta->startPos) / Pos;
    spta->currentPos = Pos + spta->startPos;

    // if (spta->currentPos > spta->PosMax)
    // {
    //     spta->currentPos = spta->PosMax;
    // }

    // if (spta->currentPos < spta->PosMin)
    // {
    //     spta->currentPos = spta->PosMin;
    // }
}