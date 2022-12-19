#ifndef _SIM900A_H_
#define _SIM900A_H_

#include "./usart/bsp_usart.h"
#include "./dht11/bsp_dht11.h"
#include "./hc_sr04/hc_sr04.h"
#if 1
// 串口2-USART2
#define  SIM900A_USART                   	USART2
#define  SIM900A_USART_CLK                RCC_APB1Periph_USART2
#define  SIM900A_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  SIM900A_USART_BAUDRATE           19200

// USART GPIO 引脚宏定义
#define  SIM900A_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  SIM900A_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  SIM900A_USART_TX_GPIO_PORT         GPIOA   
#define  SIM900A_USART_TX_GPIO_PIN          GPIO_Pin_2
#define  SIM900A_USART_RX_GPIO_PORT       GPIOA
#define  SIM900A_USART_RX_GPIO_PIN        GPIO_Pin_3

#define  SIM900A_USART_IRQ                USART2_IRQn
#define  SIM900A_USART_IRQHandler         USART2_IRQHandler
#else
// 串口4-UART4
#define  SIM900A_USART                    UART4
#define  SIM900A_USART_CLK                RCC_APB1Periph_UART4
#define  SIM900A_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  SIM900A_USART_BAUDRATE           19200

// USART GPIO 引脚宏定义
#define  SIM900A_USART_GPIO_CLK           (RCC_APB2Periph_GPIOC)
#define  SIM900A_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  SIM900A_USART_TX_GPIO_PORT         GPIOC   
#define  SIM900A_USART_TX_GPIO_PIN          GPIO_Pin_10
#define  SIM900A_USART_RX_GPIO_PORT       	GPIOC
#define  SIM900A_USART_RX_GPIO_PIN        	GPIO_Pin_11

#define  SIM900A_USART_IRQ                UART4_IRQn
#define  SIM900A_USART_IRQHandler         UART4_IRQHandler


#endif


#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


void SIM900A_send_message(void);
void SIM900A_Clear(void);
void SIM900A_SendData(unsigned char *data);

//void SIM900A_SendData( char *data);
unsigned char *SIM900A_GetIPD(unsigned short timeOut);
void SIM900A_USART_Config(void);
void SIM900A_phone(void);
extern unsigned char SIM900A_buf[128];
extern unsigned short SIM900A_cnt, SIM900A_cntPre;

#endif
