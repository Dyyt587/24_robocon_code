// PID automated tuning (Ziegler-Nichols/relay method) for Arduino and compatible boards
// Copyright (c) 2016-2020 jackw01
// This code is distrubuted under the MIT License, see LICENSE for details

#include "apid_auto_tune_ZNmode.h"
#include "apid.h"
#include "ulog.h"
/**
#include <pidautotuner.h>

void setup() {

    PIDAutotuner tuner = PIDAutotuner();

    // Set the target value to tune to
    // This will depend on what you are tuning. This should be set to a value within
    // the usual range of the setpoint. For low-inertia systems, values at the lower
    // end of this range usually give better results. For anything else, start with a
    // value at the middle of the range.
    tuner.setTargetInputValue(targetInputValue);

    // Set the loop interval in microseconds
    // This must be the same as the interval the PID control loop will run at
    tuner.setLoopInterval(loopInterval);

    // Set the output range
    // These are the minimum and maximum possible output values of whatever you are
    // using to control the system (Arduino analogWrite, for example, is 0-255)
    tuner.setOutputRange(0, 255);

    // Set the Ziegler-Nichols tuning mode
    // Set it to either PIDAutotuner::ZNModeBasicPID, PIDAutotuner::ZNModeLessOvershoot,
    // or PIDAutotuner::ZNModeNoOvershoot. Defaults to ZNModeNoOvershoot as it is the
    // safest option.
    tuner.setZNMode(PIDAutotuner::ZNModeBasicPID);

    // This must be called immediately before the tuning loop
    // Must be called with the current time in microseconds
    tuner.startTuningLoop(micros());

    // Run a loop until tuner.isFinished() returns true
    long microseconds;
    while (!tuner.isFinished()) {

        // This loop must run at the same speed as the PID control loop being tuned
        long prevMicroseconds = microseconds;
        microseconds = micros();

        // Get input value here (temperature, encoder position, velocity, etc)
        double input = doSomethingToGetInput();

        // Call tunePID() with the input value and current time in microseconds
        double output = tuner.tunePID(input, microseconds);

        // Set the output - tunePid() will return values within the range configured
        // by setOutputRange(). Don't change the value or the tuning results will be
        // incorrect.
        doSomethingToSetOutput(output);

        // This loop must run at the same speed as the PID control loop being tuned
        while (micros() - microseconds < loopInterval) delayMicroseconds(1);
    }

    // Turn the output off here.
    doSomethingToSetOutput(0);

    // Get PID gains - set your PID controller's gains to these
    double kp = tuner.getKp();
    double ki = tuner.getKi();
    double kd = tuner.getKd();
}

void loop() {

    // ...
}
*/
void apid_auto_tune_ZNmode_deinit(apid_t *pid);
void __auto_pid_handle1(apid_t *pid, PID_TYPE cycle)
{
    //覆盖输出
   // APID_(pid);
    pid->parameter.out = tunePID(pid->auto_pid,APID_Get_Present(pid),cycle);
    LOG_D(":%f",pid->parameter.out);
    // if(isFinished(pid->auto_pid)){
    //     apid_auto_tune_ZNmode_deinit(pid);
    //     APID_Enable(pid);
    // }
}

void apid_auto_tune_ZNmode_init(apid_t*pid ,apid_auto_tune_ZNmode_t *tuner,int tuneCycles)
{
    pid->auto_pid = tuner;
    pid->auto_pid_handler = __auto_pid_handle1;

    tuner-> targetInputValue = 300;
    tuner-> loopInterval = 5;
    tuner-> znMode = ZNModeNoOvershoot;
    tuner-> cycles = tuneCycles;
    tuner->microseconds = 0;

    startTuningLoop(tuner);
}
void apid_auto_tune_ZNmode_deinit(apid_t *pid)
{
    pid->auto_pid=0;
    pid->auto_pid_handler=0;
}
// Set target input for tuning
void setTargetInputValue(apid_auto_tune_ZNmode_t *tuner,float target)
{
  tuner->targetInputValue = target;
}

// Set loop interval
void setLoopInterval(apid_auto_tune_ZNmode_t *tuner,long interval)
{
  tuner->loopInterval = interval;
}

// Set output range
void setOutputRange(apid_auto_tune_ZNmode_t *tuner,float min, float max)
{
  tuner->minOutput = min;
  tuner->maxOutput = max;
}

// Set Ziegler-Nichols tuning mode
void setZNMode(apid_auto_tune_ZNmode_t *tuner,ZNMode zn)
{
  tuner->znMode = zn;
}

// Set tuning cycles
void setTuningCycles(apid_auto_tune_ZNmode_t *tuner,int tuneCycles)
{
  tuner->cycles = tuneCycles;
}

// Initialize all variables before loop
void startTuningLoop(apid_auto_tune_ZNmode_t *tuner)
{
  tuner->i = 0;         // Cycle counter
  tuner->output = true; // Current output state
  tuner->outputValue = tuner->maxOutput;
  tuner->t1 = tuner->t2 = 0;                    // Times used for calculating period
  tuner->microseconds = tuner->tHigh = tuner->tLow = 0; // More time variables
  tuner->max = -1000000000000;            // Max input
  tuner->min = 1000000000000;             // Min input
  tuner->pAverage = tuner->iAverage = tuner->dAverage = 0;
}

// Run one cycle of the loop
float tunePID(apid_auto_tune_ZNmode_t *tuner,float input, unsigned long cycle)
{
  // Useful information on the algorithm used (Ziegler-Nichols method/Relay method)
  // http://www.processcontrolstuff.net/wp-content/uploads/2015/02/relay_autot-2.pdf
  // https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method
  // https://www.cds.caltech.edu/~murray/courses/cds101/fa04/caltech/am04_ch8-3nov04.pdf

  // Basic explanation of how this works:
  //  * Turn on the output of the PID controller to full power
  //  * Wait for the output of the system being tuned to reach the target input value
  //      and then turn the controller output off
  //  * Wait for the output of the system being tuned to decrease below the target input
  //      value and turn the controller output back on
  //  * Do this a lot
  //  * Calculate the ultimate gain using the amplitude of the controller output and
  //      system output
  //  * Use this and the period of oscillation to calculate PID gains using the
  //      Ziegler-Nichols method

  // Calculate time delta
  // long prevMicroseconds = microseconds;

  tuner->microseconds += cycle;
  // float deltaT = microseconds - prevMicroseconds;

  // Calculate max and min
  tuner->max = (tuner->max > input) ? tuner->max : input;
  tuner->min = (tuner->min < input) ? tuner->min : input;

  // Output is on and input signal has risen to target
  if (tuner->output && input > tuner->targetInputValue)
  {
    // Turn output off, record current time as t1, calculate tHigh, and reset maximum
    tuner->output = false;
    tuner->outputValue = tuner->minOutput;
    tuner->t1 = tuner->microseconds;
    tuner->tHigh = tuner->t1 - tuner->t2;
    tuner->max = tuner->targetInputValue;
  }

  // Output is off and input signal has dropped to target
  if (!tuner->output && input < tuner->targetInputValue)
  {
    // Turn output on, record current time as t2, calculate tLow
    tuner->output = true;
    tuner->outputValue =tuner-> maxOutput;
    tuner->t2 = tuner->microseconds;
    tuner->tLow = tuner->t2 -tuner-> t1;

    // Calculate Ku (ultimate gain)
    // Formula given is Ku = 4d / πa
    // d is the amplitude of the output signal
    // a is the amplitude of the input signal
    float ku = (4.0 * ((tuner->maxOutput - tuner->minOutput) / 2.0)) / (M_PI * (tuner->max - tuner->min) / 2.0);

    // Calculate Tu (period of output oscillations)
    float tu = tuner->tLow + tuner->tHigh;

    // How gains are calculated
    // PID control algorithm needs Kp, Ki, and Kd
    // Ziegler-Nichols tuning method gives Kp, Ti, and Td
    //
    // Kp = 0.6Ku = Kc
    // Ti = 0.5Tu = Kc/Ki
    // Td = 0.125Tu = Kd/Kc
    //
    // Solving these equations for Kp, Ki, and Kd gives this:
    //
    // Kp = 0.6Ku
    // Ki = Kp / (0.5Tu)
    // Kd = 0.125 * Kp * Tu

    // Constants
    // https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method

    float kpConstant, tiConstant, tdConstant;
    if (tuner->znMode == ZNModeBasicPID)
    {
      kpConstant = 0.6;
      tiConstant = 0.5;
      tdConstant = 0.125;
    }
    else if (tuner->znMode == ZNModeLessOvershoot)
    {
      kpConstant = 0.33;
      tiConstant = 0.5;
      tdConstant = 0.33;
    }
    else
    { // Default to No Overshoot mode as it is the safest
      kpConstant = 0.2;
      tiConstant = 0.5;
      tdConstant = 0.33;
    }

    // Calculate gains
    tuner->kp = kpConstant * ku;
    tuner->ki = (tuner->kp / (tiConstant * tu)) * tuner->loopInterval;
    tuner->kd = (tdConstant * tuner->kp * tu) / tuner->loopInterval;

    // Average all gains after the first two cycles
    if (tuner->i > 1)
    {
      tuner->pAverage += tuner->kp;
      tuner->iAverage += tuner->ki;
      tuner->dAverage += tuner->kd;
    }

    // Reset minimum
    tuner->min = tuner->targetInputValue;

    // Increment cycle count
    tuner->i++;
  }

  // If loop is done, disable output and calculate averages
  if (tuner->i >= tuner->cycles)
  {
    tuner->output = false;
    tuner->outputValue = tuner->minOutput;
    tuner->kp = tuner->pAverage / (tuner->i - 1);
    tuner->ki = tuner->iAverage / (tuner->i - 1);
    tuner->kd = tuner->dAverage / (tuner->i - 1);
  }

  return tuner->outputValue;
}

// Get PID constants after tuning
float getKp(apid_auto_tune_ZNmode_t *tuner) { return tuner->kp; };
float getKi(apid_auto_tune_ZNmode_t *tuner) { return tuner->ki; };
float getKd(apid_auto_tune_ZNmode_t *tuner) { return tuner->kd; };

// Is the tuning loop finished?
bool isFinished(apid_auto_tune_ZNmode_t *tuner)
{
  return (tuner->i >= tuner->cycles);
}

// return number of tuning cycle
int getCycle(apid_auto_tune_ZNmode_t *tuner)
{
  return tuner->i;
}