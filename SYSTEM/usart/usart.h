#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2011/6/14
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
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
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

extern uint8_t receiveCountSign;//���ܵ��ڼ�λ
extern uint8_t isThePacketStart;//�Ƿ��а�
extern uint8_t isThePacketEnd;//�Ƿ������
extern uint8_t packetSignDataByte;//����ʶ
extern uint16_t packetFollowLengthData;//��������
extern uint16_t packetUserSendDataLength;//�û����ݳ��� = �������� - 3
extern uint8_t packetResponseCommandData;//��Ӧָ��
extern uint8_t packetUserReceiveData[50];//�û�������Ч����
extern uint16_t packetCheckSumData;//У���
extern uint8_t isDisableCheckSum;//1Ϊ�ر�У���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void uart2_init(u32 bound);
void sendUart2OneByte(uint8_t byteData);

uint8_t checkPacketCheckSumData(void);
void receiveUSART2Packet(uint8_t receiveByte);
#endif


