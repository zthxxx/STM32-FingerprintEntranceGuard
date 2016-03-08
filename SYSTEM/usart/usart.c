#include "sys.h"
#include "usart.h"	  

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 


#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  


extern uint8_t FingerPrintDataReadFlag;

ReceiveUSART2PacketDelegate analyzeClientUARTPacket;//���ص�ַʱ�ĵ��ú���
ReceiveUSART2PacketDelegate analyzeFingerModelPacket;//ָ�ƻ�����


//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound, ReceiveUSART2PacketDelegate analyzeFingerModelPacketFun){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
    
    analyzeFingerModelPacket = analyzeFingerModelPacketFun;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ //

    USART_Init(USART1, &USART_InitStructure);     //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}
void uart2_init(u32 bound, ReceiveUSART2PacketDelegate analyzeClientUARTPacketFun)
{
    
    //GPIO�˿�����,�ṹ������
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

    analyzeClientUARTPacket = analyzeClientUARTPacketFun;
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); 	//ʹ��USART1��GPIOAʱ��


	USART_DeInit(USART2);  //��λ����1

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;             //�ܽ�PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);                //TX��ʼ��


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;             //�ܽ�PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);                //RX��ʼ��

//Usart2 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
//��ʼ����������
	USART_InitStructure.USART_BaudRate   = bound;                  //������9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //�ֳ�8λ
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;      //1λֹͣ�ֽ�
	USART_InitStructure.USART_Parity     = USART_Parity_No;       //����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//��Rx���պ�Tx���͹���
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;


	USART_Init(USART2, &USART_InitStructure);                                         //��ʼ��
	USART_ClockInit(USART2, &USART_ClockInitStruct);                                         //��ʼ��
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//���������ж�
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);	//���������ж�
	USART_Cmd(USART2, ENABLE);                                                        //��������

}

void sendUart1OneByte(uint8_t byteData)//���ڷ�����Ϣ,ͨ����ѯ��ʽ����һ���ַ�
{
    USART_ClearFlag(USART1,USART_FLAG_TC);//�����һ�·����жϱ�־λ��������һ���ֽڶ�ʧ�����⡣
	USART_SendData(USART1, byteData);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
}

void sendUart2OneByte(uint8_t byteData)
{
	USART_ClearFlag(USART2,USART_FLAG_TC);//�����һ�·����жϱ�־λ��������һ���ֽڶ�ʧ�����⡣
	USART_SendData(USART2, byteData);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
}
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 receiveByte = 0;

	if((FingerPrintDataReadFlag == 1) && (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		receiveByte = USART_ReceiveData(USART1);//(USART1->DR);		//��ȡ���յ�������			
		analyzeFingerModelPacket(receiveByte);//�������ܵ�����
	}
} 


void USART2_IRQHandler(void)                	//�����жϷ������
{
	u8 receiveByte = 0;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		receiveByte = USART_ReceiveData(USART2);//(USART1->DR);		//��ȡ���յ�������			
		analyzeClientUARTPacket(receiveByte);//�������ܵ�����
	}
} 


#endif	

