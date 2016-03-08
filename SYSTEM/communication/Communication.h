#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "sys.h"   //与STM32相关的一些定义
#include "usart.h" //指纹模块 需要串口驱动的支持
#include "fingerprint.h"
#include "ds1302.h"
#include "dma.h"

#define UART2_SEND_TEXT_LENTH 500

extern uint8_t UART2_DMA_SendBuff[UART2_SEND_TEXT_LENTH];
extern uint8_t *communicatFIFO;

void sendUartUserID(uint16_t UserIDNum);
void sendUartAddNewUserID(uint16_t UserIDNum);
void sendUartAddNewAppointUserID(uint16_t UserIDNum);
void sendUartTimeData(void);
void sendOnePacket(uint8_t packetSignByte,uint16_t userSendDataLength,uint8_t responseCommandByte,uint8_t *userSendData);
void sendUartForResend(void);
void sendUartOKData(void);
void sendUartLocatAddress(void);
void sendPacketFIFO(uint16_t packetAllDataSumLength);
void sendUartOKTimeCheck(void);
void sendUartOKClearAll(void);
void RespondToPacket(void);
void sendUartOKDelOneUser(void);
uint8_t checkPacketCheckSumData(void);
void receiveUSART2Packet(uint8_t receiveByte);
typedef void (*RequestLocalAddress)(void);
void retransmissionFingerPrintData(uint8_t* userSendData,uint16_t userSendDataLength);

#endif
