#include "./hc_rc05/hc_rc05.h"

static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置NVIC为优先级组1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  /* 配置抢占优先级 【0.1】*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级 【0..7】*/
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
 /* 配置中断源：按键1  PA0*/
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  /* 使能中断通道 */
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void HC_RC05_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/*开启按键GPIO口的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
												
	/* 配置 NVIC 中断*/
	NVIC_Configuration();
	
	/* 选择按键用到的GPIO */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  /* 配置为浮空输入 */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* 选择EXTI的信号源 */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC , GPIO_PinSource7); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	
	/* EXTI为中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* 上升沿中断 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* 使能中断 */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}


