/**
	************************************************************
	************************************************************
	************************************************************

	*
	*	˵���� 		SIM900A�ļ�����
	*
	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "./sim900A/sim900A.h"
#include"./HC_RC05/hc_rc05.h"
#include "./adc/bsp_adc.h"
//Ӳ������
#include "./dwt_delay/core_delay.h"
#include "./usart/bsp_usart.h"

//C��
#include <string.h>
#include <stdio.h>


#define SIM900A_SENDM	"AT+CMGF=1\r\n"

#define SIM900A_SEND_MAREA		"AT+CSCA=\"+8619895633139\"\r\n"

char SEND_TO[][24] ={ "AT+CMGS=\"13506273308\"\r\n" , "AT+CMGS=\"13506273308\"\r\n" };

unsigned char SIM900A_buf[128];
unsigned short SIM900A_cnt = 0, SIM900A_cntPre = 0;


//==========================================================
//	�������ƣ�	SIM900A_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void SIM900A_Clear(void)
{

	memset(SIM900A_buf, 0, sizeof(SIM900A_buf));
	SIM900A_cnt = 0;

}

//==========================================================
//	�������ƣ�	SIM900A_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool SIM900A_WaitRecive(void)
{
	if(SIM900A_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(SIM900A_cnt == SIM900A_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		SIM900A_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	SIM900A_cntPre = SIM900A_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	SIM900A_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool SIM900A_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 10;//200;

	Usart_SendString(SIM900A_USART, (unsigned char *)cmd, strlen((const char *)cmd));
	//Usart_SendString(SIM900A_USART, (unsigned char *)cmd);
	while(timeOut--)
	{
		//printf("[%d]%s \n",SIM900A_cnt,SIM900A_buf);
		if(SIM900A_WaitRecive() == REV_OK)							//����յ�����
		{
			//printf("Usart_GetReceive: %s  \n",SIM900A_buf);
			if(strstr((const char *)SIM900A_buf, res) != NULL)		//����������ؼ���
			{
				printf("GOT: %s  \n",SIM900A_buf);
				SIM900A_Clear();									//��ջ���
				return 0;
			}
		}
		
		Delay(0x0FFFFF);
	}
	
	return 1;

}



//==========================================================
//	�������ƣ�	SIM900A_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��SIM900A�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *SIM900A_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(SIM900A_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)SIM900A_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		Delay(0x0FFFFF);												//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

/**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = SIM900A_USART_IRQ;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
}
 /**
  * @brief  USART2 GPIO ����,������������ PA2(TX) -----SIM900A RX ;PA3(RX) -----SIM900A TX ;
  * @param  ��
  * @retval ��
  */
void SIM900A_USART_Config(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// �򿪴���GPIO��ʱ��
	SIM900A_USART_GPIO_APBxClkCmd(SIM900A_USART_GPIO_CLK, ENABLE);
	
	// �򿪴��������ʱ��
	SIM900A_USART_APBxClkCmd(SIM900A_USART_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = SIM900A_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SIM900A_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = SIM900A_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SIM900A_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// ���ô��ڵĹ�������
	// ���ò�����
	USART_InitStructure.USART_BaudRate = SIM900A_USART_BAUDRATE;
	// ���� �������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// ����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(SIM900A_USART, &USART_InitStructure);

	// ʹ�ܴ���
	USART_Cmd(SIM900A_USART, ENABLE);	 

	// �����ж����ȼ�����
	NVIC_Configuration();
	
	// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(SIM900A_USART, USART_IT_RXNE, ENABLE);	
	
	// ʹ�ܴ���
	USART_Cmd(SIM900A_USART, ENABLE);		

	//���������ɱ�־
	USART_ClearFlag(SIM900A_USART, USART_FLAG_TC); 
}

//==========================================================
//	�������ƣ�	SIM900A_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void SIM900A_SendData(unsigned char *data)
//void SIM900A_SendData( char *data)
{
char Buff[200];
char cmdBuf[200];
	SIM900A_Clear();			//��ս��ջ���SIM900A_buf
	sprintf(cmdBuf, "AT+CMGS=\"%s\"\r\n","13506273308");//���͵Ķ���
	while(SIM900A_SendCmd(cmdBuf  , ">"))  //
	{
		printf("#### resend\r\n");
		Delay(0x0FFFFF);
	}
	printf("111111111111111111\r\n");
 //�Ѿ����뵽>�������ݱ༭ģʽ �� 16���Ƶ�1A��ʾ�˳��������ݱ༭ģʽ
	sprintf(Buff, "%s\x1a\r\n" , data);  // ��from atm32\x1a\r\n��
	printf("��������:%s\n", Buff);
	Usart_SendString(SIM900A_USART,(char*)Buff, strlen((const char *)Buff));	

}
//==========================================================
//	�������ƣ�	SIM900A_Init
//
//	�������ܣ�	��ʼ��SIM900A
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
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

