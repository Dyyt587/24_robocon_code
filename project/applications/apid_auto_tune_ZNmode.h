/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-23 08:10:27
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-23 10:34:31
 * @FilePath: \arduino-pid-autotuner\apid_auto_tune_ZNmode.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
// PID automated tuning (Ziegler-Nichols/relay method) for Arduino and compatible boards
// Copyright (c) 2016-2020 jackw01
// This code is distrubuted under the MIT License, see LICENSE for details

#ifndef APID_AUTO_TUNE_ZNMODE_H
#define APID_AUTO_TUNE_ZNMODE_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
typedef enum
{
    ZNModeBasicPID,
    ZNModeLessOvershoot,
    ZNModeNoOvershoot
} ZNMode;

typedef struct apid_auto_tune_ZNmode
{
 


    float targetInputValue ;
    float loopInterval ;
    float minOutput, maxOutput;
    ZNMode znMode ;
    int cycles ;

    // See startTuningLoop()
    int i;
    bool output;
    float outputValue;
    long microseconds, t1, t2, tHigh, tLow;
    float max, min;
    float pAverage, iAverage, dAverage;

    float kp, ki, kd;
} apid_auto_tune_ZNmode_t;







   // Constants for Ziegler-Nichols tuning mode

void apid_auto_tune_ZNmode_init(apid_auto_tune_ZNmode_t *tuner,int tuneCycles);

    // Configure parameters for PID tuning
    // See README for more details - https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
    // targetInputValue: the target value to tune to
    // loopInterval: PID loop interval in microseconds - must match whatever the PID loop being tuned runs at
    // outputRange: min and max values of the output that can be used to control the system (0, 255 for analogWrite)
    // znMode: Ziegler-Nichols tuning mode (znModeBasicPID, znModeLessOvershoot, znModeNoOvershoot)
    // tuningCycles: number of cycles that the tuning runs for (optional, default is 10)
    void setTargetInputValue(apid_auto_tune_ZNmode_t *tuner,float target);
    void setLoopInterval(apid_auto_tune_ZNmode_t *tuner,long interval);
    void setOutputRange(apid_auto_tune_ZNmode_t *tuner,float min, float max);
    void setZNMode(apid_auto_tune_ZNmode_t *tuner,ZNMode zn);
    void setTuningCycles(apid_auto_tune_ZNmode_t *tuner,int tuneCycles);

    // Must be called immediately before the tuning loop starts
    void startTuningLoop(apid_auto_tune_ZNmode_t *tuner);

    // Automatically tune PID
    // This function must be run in a loop at the same speed as the PID loop being tuned
    // See README for more details - https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
    float tunePID(apid_auto_tune_ZNmode_t *tuner,float input, unsigned long us);

    // Get results of most recent tuning
    float getKp(apid_auto_tune_ZNmode_t *tuner);
    float getKi(apid_auto_tune_ZNmode_t *tuner);
    float getKd(apid_auto_tune_ZNmode_t *tuner);

    bool isFinished(apid_auto_tune_ZNmode_t *tuner); // Is the tuning finished?

    int getCycle(apid_auto_tune_ZNmode_t *tuner); // return tuning cycle











#ifdef __cplusplus
}
#endif
#endif
