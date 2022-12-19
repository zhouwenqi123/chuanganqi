#ifndef __HC_H
#define __HC_H	
#include "./systick/bsp_SysTick.h"


//定时器4设置
void hcsr04_NVIC(void);

//IO口初始化 及其他初始化
void Hcsr04Init(void);

//打开定时器4
static void OpenTimerForHc(void);

//关闭定时器4
static void CloseTimerForHc(void);

//定时器4终中断
void TIM4_IRQHandler(void);

//获取定时器4计数器值
u32 GetEchoTimer(void);

//通过定时器4计数器值推算距离
float Hcsr04GetLength(void);

#define TRIG_Send  PBout(11)
#define ECHO_Reci  PBin(10)
#endif

