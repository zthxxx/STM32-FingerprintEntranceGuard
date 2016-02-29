#include "sys.h"
#include "usart.h"	  
#include "Communication.h"
#include "fingerprint.h"
#include "ds1302.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

uint8_t receiveCountSign=0;//接受到第几位
uint8_t isThePacketStart = 0;//是否有包
uint8_t isThePacketEnd = 0;//是否包结束
uint8_t packetSignDataByte = 0;//包标识
uint16_t packetFollowLengthData = 0;//后续长度
uint16_t packetUserSendDataLength = 0;//用户数据长度 = 后续长度 - 3
uint8_t packetResponseCommandData = 0;//响应指令
uint8_t packetUserReceiveData[50] = {0};//用户发送有效数据
uint16_t packetCheckSumData = 0;//校验和
uint8_t isDisableCheckSum = 1;//1为关闭校验和	
	
//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

   //Usart1 NVIC 配置

//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式 //

    USART_Init(USART1, &USART_InitStructure);     //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}
void uart2_init(u32 bound)
{
    //GPIO端口设置,结构体申明
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); 	//使能USART1，GPIOA时钟


	USART_DeInit(USART2);  //复位串口1

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;             //管脚PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);                //TX初始化


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;             //管脚PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);                //RX初始化

//Usart2 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
//初始化参数设置
	USART_InitStructure.USART_BaudRate   = bound;                  //波特率9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //字长8位
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;      //1位停止字节
	USART_InitStructure.USART_Parity     = USART_Parity_No;       //无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//打开Rx接收和Tx发送功能
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;


	USART_Init(USART2, &USART_InitStructure);                                         //初始化
	USART_ClockInit(USART2, &USART_ClockInitStruct);                                         //初始化
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启接收中断
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);	//开启发送中断
	USART_Cmd(USART2, ENABLE);                                                        //启动串口

}

void sendUart2OneByte(uint8_t byteData)
{
	USART_ClearFlag(USART2,USART_FLAG_TC);//先清除一下发送中断标志位，会解决第一个字节丢失的问题。
	USART_SendData(USART2, byteData);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
}
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	;
} 


void receiveUSART2Packet(uint8_t receiveByte)
{
	if(isThePacketStart)
		{
			if(receiveCountSign == 6)//&& receiveByte != 0xFF
			{
				packetSignDataByte = receiveByte;
				receiveCountSign = 7;
			}
			else if(receiveCountSign == 7)
			{
				packetFollowLengthData = receiveByte & 0x00FF;
				packetFollowLengthData <<= 8;
				receiveCountSign = 8;
			}
			else if(receiveCountSign == 8)
			{
				packetFollowLengthData |= receiveByte & 0x00FF;
				packetUserSendDataLength = packetFollowLengthData - 3;
				receiveCountSign = 9;
			}
			else if(receiveCountSign == 9)
			{
				packetResponseCommandData = receiveByte;
				receiveCountSign = 10;
			}
			else if((receiveCountSign >= 10) && (receiveCountSign < 10 + packetUserSendDataLength))
			{
				receiveCountSign++;			
				packetUserReceiveData[receiveCountSign-11] = receiveByte;				
			}
			else if(receiveCountSign == 10 + packetUserSendDataLength)
			{
				packetCheckSumData = receiveByte & 0x00FF;
				packetCheckSumData <<= 8;
				receiveCountSign++;
			}
			else if(receiveCountSign == 11 + packetUserSendDataLength)
			{
				packetCheckSumData |= receiveByte & 0x00FF;
				receiveCountSign++;
				isThePacketEnd = 1;
				isThePacketStart = 0;
				receiveCountSign = 0;
			}
		}
		else
		{
			if((receiveCountSign == 0)&&(receiveByte == 0xEF))
			{
				receiveCountSign = 1;
			}
			else if((receiveCountSign == 1)&&(receiveByte == 0x02))
			{
				receiveCountSign = 2;
			}
			else if((receiveCountSign >= 2 && receiveCountSign <= 5)&&(receiveByte == 0xFF))
			{
				receiveCountSign++;
				if(receiveCountSign >= 6) 
					isThePacketStart = 1;
			}
			else 
			{
				isThePacketEnd=0;
				receiveCountSign = 0;
			}
		}
}

void RespondToPacket()
{
	if(isThePacketEnd)
	{
		if(isDisableCheckSum || checkPacketCheckSumData())
		{
			switch(packetSignDataByte)
			{
				case 0x01:
					//sendUart2OneByte('O');sendUart2OneByte('K');//DEBUG
					sendUartOKData();
					modeflag = 0;//进入查找模式					
				break;
				case 0x02:
					//sendUart2OneByte('O');sendUart2OneByte('K');//DEBUG
					sendUartOKData();
					modeflag = 1;//新增一个用户					
				break;
				case 0x08:
					sendUartOKData();
					WriteDS1302ClockASCII(packetUserReceiveData);
					sendUartTimeData();
				break;
				case 0x10:
					clearallflag = 1;//清空全部用户
				break;
			}
		}
		else
		{
			sendUartForResend();
		}
		isThePacketEnd=0;
	}	
}

uint8_t checkPacketCheckSumData()
{
	uint16_t thePacketCheckSum=0;
	uint16_t count=0;
	uint8_t isPacketSumRigth = 0;
	thePacketCheckSum = packetSignDataByte + packetFollowLengthData + packetResponseCommandData;
	for(count=0;count<packetUserSendDataLength;count++)
	{
		thePacketCheckSum += packetUserReceiveData[count];
	}
	
	if(thePacketCheckSum == packetCheckSumData)
	{
		isPacketSumRigth = 1;
	}
	
	return isPacketSumRigth;
}

void USART2_IRQHandler(void)                	//串口中断服务程序
{
	uint8_t receiveByte=0;//当前字节

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		RespondToPacket();//先判断上一包是否响应
		receiveByte = USART_ReceiveData(USART2);//(USART1->DR);		//读取接收到的数据
			
		receiveUSART2Packet(receiveByte);//解析接受的数据		
	}

	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
} 
#endif	

