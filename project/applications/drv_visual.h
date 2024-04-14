#ifndef DRV_VISUAL_H
#define DRV_VISUAL_H


#include <rtthread.h>
#include <rtdevice.h>


typedef struct cvDat
{
		int color;
    int id;
    double posX;
    double posY;
} cvdat;//

extern uint16_t SSize;

extern cvdat ball[10];




#endif