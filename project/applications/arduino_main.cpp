/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-12-21     ChuShicheng  first version
 */

#include <Arduino.h>
//20SFFACTORY COMMUNITY ROBOT FIRMWARE

//MAINTAINER: LEOYEUNG@20SFFACTORY
//CONTACT: yeung.kl.leo@gmail.com
//FORUM: www.facebook.com/groups/robotarm

//VERSION: V0.81

//VERSION HISTORY:
//   V0.31 WITH G92, M114, LOGGER, LIMIT_CHECK FUNCTIONS
//   V0.41 WITH DUAL SHANK LENGTH SUPPORT
//   V0.51 WITH SERVO GRIPPER
//   V0.61 WITH ARDUINO UNO OPTION
//   V0.71 WITH:
//       ESP32(WEMOS D1R32) WITH PS4 JOYSTICK CONTROL OPTION
//       COMMAND TO SET CUSTOM SPEED PROFILE 'M205 S0'
//       UNO OPTION WITH RAIL SUPPORT
//   V0.81 WITH WII REMOTE, JOYSTICK ADJUSTABLE SPEED MULTIPLIER


#include <Arduino.h>
//GENERAL CONFIG SETTINGS
//#include "config.h"
extern void test_track();

void setup()
{
//1 	rt_enter_critical();
//  test_track();
//	rt_exit_critical();
}

void loop() 
{
  delay(500);
}
