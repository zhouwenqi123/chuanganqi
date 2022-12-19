#ifndef __HC_SR04_H

#define __HC_SR04_H
#include "stm32f10x_it.h"

#define Trig GPIO_Pin_4

#define Echo GPIO_Pin_6

void HC_sr04_Init(void);
void HC_sr04_start(void);
void Timer_HC_SR04_Init(u16 arr,u16 psc);

extern float Distance;


#endif
