/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-12 11:59:42
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-15 01:01:34
 * @FilePath: \project\applications\drv_visual.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef DRV_VISUAL_H
#define DRV_VISUAL_H


#include <rtthread.h>
#include <rtdevice.h>


typedef struct cvDat
{
		int color;
    int id;
    float posX;
    float posY;
} cvdat;//

extern int SSize;

extern cvdat ball[10];




#endif