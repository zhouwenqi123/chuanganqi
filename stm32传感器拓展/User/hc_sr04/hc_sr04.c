 
#include "stm32f10x.h"
#include "delay.h"
#include "bsp_usart.h"
#include "hc_sr04.h"





/**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置NVIC为优先级组1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  /* 配置抢占优先级 【0.1】*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级 【0..7】*/
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	
 /* 配置中断源： PA6*/
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  /* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

 /**
  * @brief  配置 IO为EXTI中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void HC_sr04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/*开启按键GPIO口的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC,ENABLE);
												
	/* 配置 NVIC 中断*/
	NVIC_Configuration();
	
/*--------------------------TRIG PA4 配置-----------------------------*/	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;   //推挽输出模式
	GPIO_InitStructure.GPIO_Pin=Trig;                //将PA4 于Trig相连
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
/*--------------------------ECHO配置-----------------------------*/
  /* 选择按键用到的ECHO GPIO */	
  GPIO_InitStructure.GPIO_Pin = Echo;
  /* 配置为浮空输入 */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);


	/* 选择EXTI的信号源 */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC , GPIO_PinSource6); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	
	/* EXTI为中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* 上升沿中断 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  /* 使能中断 */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
 }


void HC_sr04_start(void)
{
	GPIO_SetBits(GPIOC,Trig);   //将Trig设置为高电平
	Delay_us(20);               //持续大于10us触发，触发超声波模块工作
	GPIO_ResetBits(GPIOC,Trig); 
	
}
