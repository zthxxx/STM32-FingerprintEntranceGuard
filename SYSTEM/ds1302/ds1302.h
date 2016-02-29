#ifndef DS1302_H
#define DS1302_H
#include "stm32f10x.h"

//
#define NOP() __NOP

#define DS1302_CLK_H()	(GPIOA->BSRR=GPIO_Pin_5)
#define DS1302_CLK_L()	(GPIOA->BRR=GPIO_Pin_5)

#define DS1302_RST_H()	(GPIOA->BSRR=GPIO_Pin_7)
#define DS1302_RST_L()	(GPIOA->BRR=GPIO_Pin_7)

#define DS1302_OUT_H()	(GPIOA->BSRR=GPIO_Pin_6)
#define DS1302_OUT_L()	(GPIOA->BRR=GPIO_Pin_6)
											
#define DS1302_IN_X		(GPIOA->IDR&GPIO_Pin_6)

#define Time_24_Hour	0x00	//24时制控制	
#define Time_Start		0x00	//开始走时
 
#define DS1302_SECOND	0x80	//DS1302各寄存器操作命令定义
#define DS1302_MINUTE	0x82
#define DS1302_HOUR		0x84
#define DS1302_DAY		0x86
#define DS1302_MONTH	0x88
#define DS1302_WEEK		0x8A
#define DS1302_YEAR		0x8C
#define DS1302_WRITE	0x8E
#define DS1302_POWER	0x90

static GPIO_InitTypeDef GPIO_InitStructure;

 void InitClock(void);
 void ReadDS1302Clock(u8 *p);
 void WriteDS1302Clock(u8 *p);
 void ReadDSRam(u8 *p,u8 add,u8 cnt);
 void WriteDSRam(u8 *p,u8 add,u8 cnt);
 u8 Read1302(u8 addr);
 void timerBCDtoASCII(u8 *time_BCD,u8 *time_ASCII);
 void timerASCIItoBCD(u8 *time_BCD,u8 *time_ASCII);
 void ReadDS1302ClockASCII(u8 *timerASCII);
 void WriteDS1302ClockASCII(u8 *timerASCII);
#endif

