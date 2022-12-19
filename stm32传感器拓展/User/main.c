#include "stm32f10x.h"
#include "bsp_led.h"  
#include "./ldr/bsp_ldr.h" 
#include "./beep/bsp_beep.h"
#include "./systick/bsp_SysTick.h"
#include "./dht11/bsp_dht11.h"
#include "./usart/bsp_usart.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./dwt_delay/core_delay.h"
#include "./sr04/hc.h"
#include "./key/bsp_key.h" 
#include "./font/pic.h"
#include "./sim900A/sim900A.h"
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */ 
void Delay(__IO u32 nCount);
extern uint16_t lcdid;
void LCD_Test(void)
{
	LCD_SetFont(&Font8x16);
	//LCD_SetColors(RED,BLACK);
	LCD_Showimage(0, 0, 240, 320, gImage_pic);
	//ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	ILI9341_DisplayStringEx(24,0*48+20,48,48,(uint8_t *)"Welcome",0);
	ILI9341_DisplayStringEx(0,1*48+20,24,24,(uint8_t *)"小组成员如下",0);
	ILI9341_DisplayStringEx(0,80+20,16,16,(uint8_t *)"王俊辉",0);
	ILI9341_DisplayStringEx(120,80+20,16,16,(uint8_t *)"徐飞飞",0);
	ILI9341_DisplayStringEx(0,100+20,16,16,(uint8_t *)"32陈乾",0);
	ILI9341_DisplayStringEx(120,100+20,16,16,(uint8_t *)"周崔晨",0);
	//ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	Delay(0xFFFFFF);
	Delay(0xFFFFFF);
	Delay(0xFFFFFF);
	Delay(0xFFFFFF);
}
int main(void)
{	
	char dispBuff[100];
	DHT11_Data_TypeDef DHT11_Data;
	float length;
	SysTick_Init();
	BEEP_GPIO_Config();	
	ILI9341_Init (); 
	ILI9341_GramScan ( 6 );
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	LDR_GPIO_Config();
	/* LED端口初始化 */
	LED_GPIO_Config();
	SIM900A_USART_Config();
  LED3_OFF;
	LED1_OFF;
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	Hcsr04Init();
	printf("串口测试\r\n");   
  Hcsr04Init();    
  printf("超声波初始化成功!\n");
	/*开机画面*/
	LCD_Showimage(0, 0, 240, 320, gImage_p);
	Delay(0x1FFFFFF);
	LCD_Test();
	Delay(0x1FFFFFF);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	LCD_Showimage(0, 0, 240, 320, gImage_pic);
  while(1) 
	{
			ILI9341_DisplayStringEx(100,0,18,16,(uint8_t *)"Welcome",0);
		if(DHT11_Read_TempAndHumidity (&DHT11_Data) == SUCCESS)
		{
				char BUFF1[100];
        sprintf(BUFF1,"°☆☆☆温度: %d.%d°C ☆☆",DHT11_Data.temp_int, DHT11_Data.temp_deci);
				printf("%d.%d\r\n",DHT11_Data.temp_int, DHT11_Data.temp_deci);
				ILI9341_DisplayStringEx(0,30,18,18,(uint8_t *)BUFF1,0);
				if (DHT11_Data.temp_int>=26)
			{
				LED1_ON;   
				BEEP( Open); 			  // 响
				Delay(0x0FFFFF);	
				BEEP( Close );		  // 不响
				Delay(0x0FFFFF);
				ILI9341_DisplayStringEx(0,90,18,18,(uint8_t *)"警告，温度过高",0);
			}
			else{
				
				ILI9341_DisplayStringEx(0,90,18,18,(uint8_t *)"提示:温度正常！",0);
			}
			
        sprintf(BUFF1,"°☆☆☆湿度: %d.%d%%  ☆☆",DHT11_Data.humi_int, DHT11_Data.humi_deci);
        ILI9341_DisplayStringEx(0,60,18,18,(uint8_t *)BUFF1,0);
			
			}
			else
			{
				ILI9341_DisplayStringEx(0,30,18,18,(uint8_t *)"读取失败",0);
			}
			
		if (LDR_Test(LDR_GPIO_PORT,LDR_GPIO_PIN) == LDR_ON)
		{
				LED3_OFF;
				ILI9341_DisplayStringEx(0,120,18,18,(uint8_t *)"光线充足，不亮",0);
		}
		else if(LDR_Test(LDR_GPIO_PORT,LDR_GPIO_PIN) == LDR_OFF)
		{
				LED3_ON;   
				BEEP( Open); 			  // 响
				Delay(0x0FFFFF);	
				BEEP( Close );		  // 不响
				Delay(0x0FFFFF);
				ILI9341_DisplayStringEx(0,120,18,18,(uint8_t *)"光线不足，灯亮",0);
		}
		length = Hcsr04GetLength();
		printf("距离为:%.3fcm\n",length);
		sprintf(dispBuff,"/****距离为:%.3fcm ****/",length);
    ILI9341_DisplayStringEx(0,150,18,18,(uint8_t *)"*****提示：监测到距离*****",0);
		ILI9341_DisplayStringEx(0,180,18,18,(uint8_t *)dispBuff,0);
		if(length<=20) 
		{
			  LED1_ON;   
				BEEP(Open); 			  // 响
				Delay(0x0FFFFF);	
				BEEP(Close);		  // 不响
				Delay(0x0FFFFF);
			  sprintf(dispBuff,"/*距离过近,距离为:%.3fcm */",length);
			  ILI9341_DisplayStringEx(0,210,18,18,(uint8_t *)"/*距离异常*/",0);
		}
		else{
			LED1_OFF;
			ILI9341_DisplayStringEx(0,210,18,18,(uint8_t *)"/*距离正常*/",0);
		}
		
		
		
       /*红外线感应模块*/
				if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
			{
				printf("附近没人\r\n");
        ILI9341_DisplayStringEx(0,240,18,18,(uint8_t *)"无人靠近，一切正常！",0);
			}
			else
			{
				printf("附近有人\r\n");

        ILI9341_DisplayStringEx(0,240,18,18,(uint8_t *)"有人靠近，警报响起！",0);
				BEEP(Open); 			// 响
		    Delay(0x0FFFFF);	
		    BEEP(Close);		  // 不响
		    Delay(0x0FFFFF);
				SIM900A_phone();
				Delay(0x0FFFFF);
			
			}
			Delay(0x0FFFFF);
	}				
	
}
 
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
} 