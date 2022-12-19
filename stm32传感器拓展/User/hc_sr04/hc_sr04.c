 
#include "stm32f10x.h"
#include "delay.h"
#include "bsp_usart.h"
#include "hc_sr04.h"





/**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ����NVICΪ���ȼ���1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  /* ������ռ���ȼ� ��0.1��*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ� ��0..7��*/
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	
 /* �����ж�Դ�� PA6*/
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  /* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

 /**
  * @brief  ���� IOΪEXTI�жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void HC_sr04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/*��������GPIO�ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC,ENABLE);
												
	/* ���� NVIC �ж�*/
	NVIC_Configuration();
	
/*--------------------------TRIG PA4 ����-----------------------------*/	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;   //�������ģʽ
	GPIO_InitStructure.GPIO_Pin=Trig;                //��PA4 ��Trig����
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
/*--------------------------ECHO����-----------------------------*/
  /* ѡ�񰴼��õ���ECHO GPIO */	
  GPIO_InitStructure.GPIO_Pin = Echo;
  /* ����Ϊ�������� */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);


	/* ѡ��EXTI���ź�Դ */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC , GPIO_PinSource6); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	
	/* EXTIΪ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* �������ж� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  /* ʹ���ж� */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
 }


void HC_sr04_start(void)
{
	GPIO_SetBits(GPIOC,Trig);   //��Trig����Ϊ�ߵ�ƽ
	Delay_us(20);               //��������10us����������������ģ�鹤��
	GPIO_ResetBits(GPIOC,Trig); 
	
}
