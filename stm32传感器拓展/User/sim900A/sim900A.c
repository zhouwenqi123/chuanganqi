/**
	************************************************************
	************************************************************
	************************************************************

	*
	*	说明： 		SIM900A的简单驱动
	*
	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "./sim900A/sim900A.h"
#include"./HC_RC05/hc_rc05.h"
#include "./adc/bsp_adc.h"
//硬件驱动
#include "./dwt_delay/core_delay.h"
#include "./usart/bsp_usart.h"

//C库
#include <string.h>
#include <stdio.h>


#define SIM900A_SENDM	"AT+CMGF=1\r\n"

#define SIM900A_SEND_MAREA		"AT+CSCA=\"+8619895633139\"\r\n"

char SEND_TO[][24] ={ "AT+CMGS=\"13506273308\"\r\n" , "AT+CMGS=\"13506273308\"\r\n" };

unsigned char SIM900A_buf[128];
unsigned short SIM900A_cnt = 0, SIM900A_cntPre = 0;


//==========================================================
//	函数名称：	SIM900A_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void SIM900A_Clear(void)
{

	memset(SIM900A_buf, 0, sizeof(SIM900A_buf));
	SIM900A_cnt = 0;

}

//==========================================================
//	函数名称：	SIM900A_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool SIM900A_WaitRecive(void)
{
	if(SIM900A_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(SIM900A_cnt == SIM900A_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		SIM900A_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	SIM900A_cntPre = SIM900A_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	SIM900A_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool SIM900A_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 10;//200;

	Usart_SendString(SIM900A_USART, (unsigned char *)cmd, strlen((const char *)cmd));
	//Usart_SendString(SIM900A_USART, (unsigned char *)cmd);
	while(timeOut--)
	{
		//printf("[%d]%s \n",SIM900A_cnt,SIM900A_buf);
		if(SIM900A_WaitRecive() == REV_OK)							//如果收到数据
		{
			//printf("Usart_GetReceive: %s  \n",SIM900A_buf);
			if(strstr((const char *)SIM900A_buf, res) != NULL)		//如果检索到关键词
			{
				printf("GOT: %s  \n",SIM900A_buf);
				SIM900A_Clear();									//清空缓存
				return 0;
			}
		}
		
		Delay(0x0FFFFF);
	}
	
	return 1;

}



//==========================================================
//	函数名称：	SIM900A_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如SIM900A的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *SIM900A_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(SIM900A_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)SIM900A_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		Delay(0x0FFFFF);												//延时等待
	} while(timeOut--);
	
	return NULL;														//超时还未找到，返回空指针

}

/**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = SIM900A_USART_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}
 /**
  * @brief  USART2 GPIO 配置,工作参数配置 PA2(TX) -----SIM900A RX ;PA3(RX) -----SIM900A TX ;
  * @param  无
  * @retval 无
  */
void SIM900A_USART_Config(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	SIM900A_USART_GPIO_APBxClkCmd(SIM900A_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	SIM900A_USART_APBxClkCmd(SIM900A_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = SIM900A_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SIM900A_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = SIM900A_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SIM900A_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = SIM900A_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(SIM900A_USART, &USART_InitStructure);

	// 使能串口
	USART_Cmd(SIM900A_USART, ENABLE);	 

	// 串口中断优先级配置
	NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig(SIM900A_USART, USART_IT_RXNE, ENABLE);	
	
	// 使能串口
	USART_Cmd(SIM900A_USART, ENABLE);		

	//清除发送完成标志
	USART_ClearFlag(SIM900A_USART, USART_FLAG_TC); 
}

//==========================================================
//	函数名称：	SIM900A_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void SIM900A_SendData(unsigned char *data)
//void SIM900A_SendData( char *data)
{
char Buff[200];
char cmdBuf[200];
	SIM900A_Clear();			//清空接收缓存SIM900A_buf
	sprintf(cmdBuf, "AT+CMGS=\"%s\"\r\n","13506273308");//发送的对象
	while(SIM900A_SendCmd(cmdBuf  , ">"))  //
	{
		printf("#### resend\r\n");
		Delay(0x0FFFFF);
	}
	printf("111111111111111111\r\n");
 //已经进入到>发送内容编辑模式 ， 16进制的1A表示退出发送内容编辑模式
	sprintf(Buff, "%s\x1a\r\n" , data);  // “from atm32\x1a\r\n”
	printf("短信内容:%s\n", Buff);
	Usart_SendString(SIM900A_USART,(char*)Buff, strlen((const char *)Buff));	

}
//==========================================================
//	函数名称：	SIM900A_Init
//
//	函数功能：	初始化SIM900A
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void SIM900A_send_message(void)
{
  unsigned char dispBuff[200];
	SIM900A_Clear();


	printf("1. AT\n"); //pc
	while(SIM900A_SendCmd("AT\r\n", "OK")) // SIM900A
	{
		Delay(0x0FFFFF);
		//printf( " --AT \n");
	}		
	
	printf(  "2. %s",SIM900A_SENDM);
	while(SIM900A_SendCmd(SIM900A_SENDM, "OK"))
		Delay(0x0FFFFF);
	
	printf(  "3. %s",SIM900A_SEND_MAREA);
	while(SIM900A_SendCmd(SIM900A_SEND_MAREA, "OK"))  //
		Delay(0x0FFFFF);
//	
	printf( "4. sendto %s", SEND_TO[1]);
	
	
	Delay(0x0FFFFF);
	
	
//	if(hc_rc05_flag){
//		sprintf((char *)dispBuff,"Temperature:%d.%d centigrade\r\nHumidity:%d.%dRH\r\nIs there anyone:No\r\nBrightness:%f \r\nDistance:%.2f cm \r\n\x1A\r\n",
//		DHT11_Data.temp_int,DHT11_Data.temp_deci,DHT11_Data.humi_int,DHT11_Data.humi_deci,ADC_ConvertedValueLocal,Distance);
//	}else{
//		sprintf((char *)dispBuff,"Temperature:%d.%d centigrade\r\nHumidity:%d.%dRH\r\nIs there anyone:Yes\r\nBrightness:%f \r\nDistance:%.2f cm \r\n\x1A\r\n",
//		DHT11_Data.temp_int,DHT11_Data.temp_deci,DHT11_Data.humi_int,DHT11_Data.humi_deci,ADC_ConvertedValueLocal,Distance);
//	}
//	
	
	SIM900A_SendData("message test!!!"); // 
	printf("from stm32 \x1A\r\n");
	
	printf(  "5. SIM900A Init OK\r\n");

}
void SIM900A_phone(void){
	SIM900A_Clear();
	while(SIM900A_SendCmd("ATD19901541203;\r\n", "OK")) // SIM900A
	{
		Delay(0x0FFFFF);
		//printf( " --AT \n");
	}
}

