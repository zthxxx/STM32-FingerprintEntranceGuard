#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "sys.h"   //与STM32相关的一些定义
#include "usart.h" //指纹模块 需要串口驱动的支持
#include "fingerprint.h"
#include "ds1302.h"



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
#endif
