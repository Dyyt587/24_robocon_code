#ifndef _ABOARD_POWER_SWITCH_H
#define _ABOARD_POWER_SWITCH_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <drv_gpio.h>

enum{
 SWITCH_24V_1 =GET_PIN(H,5),  //PH5
 SWITCH_24V_2 =GET_PIN(H,4),	//PH4
 SWITCH_24V_3 =GET_PIN(H,2),	//Ph2
 SWITCH_24V_4 =GET_PIN(H,3),	//ph3
 SWITCH_5V_1  =GET_PIN(G,13), //PG13

};



int power_off(int id);
int power_on(int id);


#ifdef __cplusplus
}
#endif
#endif