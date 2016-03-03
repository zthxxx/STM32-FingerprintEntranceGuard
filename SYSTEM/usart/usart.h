#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//#include "fingerprint.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.csom
//修改日期:2011/6/14
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
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
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

extern void RespondToPacket(void);
extern uint8_t Protocol_packetStratData[2];
extern uint8_t Protocol_addressData[4];
extern uint8_t Protocol_addressReadRequestData[4];
extern uint8_t receiveCountSign;//接受到第几位
extern uint8_t isThePacketStart;//是否有包
extern uint8_t isThePacketEnd;//是否包结束
extern uint8_t packetSignDataByte;//包标识
extern uint16_t packetFollowLengthData;//后续长度
extern uint16_t packetUserSendDataLength;//用户数据长度 = 后续长度 - 3
extern uint8_t packetResponseCommandData;//响应指令
extern uint8_t packetUserReceiveData[50];//用户发送有效数据
extern uint16_t packetCheckSumData;//校验和
extern uint8_t isDisableCheckSum;//1为关闭校验和	
typedef void (*RequestLocalAddress)(void);



//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void uart2_init(u32 bound, RequestLocalAddress);
void sendUart2OneByte(uint8_t byteData);
uint8_t checkPacketCheckSumData(void);
void receiveUSART2Packet(uint8_t receiveByte);
#endif


