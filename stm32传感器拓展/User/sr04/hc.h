#ifndef __HC_H
#define __HC_H	
#include "./systick/bsp_SysTick.h"


//��ʱ��4����
void hcsr04_NVIC(void);

//IO�ڳ�ʼ�� ��������ʼ��
void Hcsr04Init(void);

//�򿪶�ʱ��4
static void OpenTimerForHc(void);

//�رն�ʱ��4
static void CloseTimerForHc(void);

//��ʱ��4���ж�
void TIM4_IRQHandler(void);

//��ȡ��ʱ��4������ֵ
u32 GetEchoTimer(void);

//ͨ����ʱ��4������ֵ�������
float Hcsr04GetLength(void);

#define TRIG_Send  PBout(11)
#define ECHO_Reci  PBin(10)
#endif

