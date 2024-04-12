/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-03-03 16:29:11
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-03-03 22:31:37
 * @FilePath: \project\applications\aboard_power_switch.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */

#include "ulog.h"
#define LED0_PIN    GET_PIN(F, 14)

#include "aboard_power_switch.h"
int power_on(int id)
{
		rt_pin_write(id, PIN_HIGH);

		return 0;
}

int power_off(int id)
{
		rt_pin_write(id, PIN_LOW);

		return 0;
}

//TODO:实现命令行开关对应switch 比如 power_switch 1 就打开 SWITCH_24V_1 
// power_switch 就打印帮助信息，显示不同switch对应 id
//poweer on all
static long power_switch(int argc, char**argv)
{

    return 0;
}
MSH_CMD_EXPORT(power_switch, open or close a board power interface);


int power_init(void)
{
    rt_pin_mode(SWITCH_24V_1, PIN_MODE_OUTPUT);
    rt_pin_mode(SWITCH_24V_2, PIN_MODE_OUTPUT);
    rt_pin_mode(SWITCH_24V_3, PIN_MODE_OUTPUT);
    rt_pin_mode(SWITCH_24V_4, PIN_MODE_OUTPUT);
    rt_pin_mode(SWITCH_5V_1, PIN_MODE_OUTPUT);
	
	
	power_off(SWITCH_24V_1);
	power_off(SWITCH_24V_2);
	power_off(SWITCH_24V_3);
	power_off(SWITCH_24V_4);
	power_off(SWITCH_5V_1 );
	
	return 0;
}
INIT_BOARD_EXPORT(power_init);

