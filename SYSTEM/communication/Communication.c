#include <string.h>
#include "Communication.h"
#include "stdlib.h"



#define FIFO_MAX_NUMBER    80


uint8_t *communicatFIFO;

uint8_t Protocol_packetStratData[2] = {0xEF,0x02};
uint8_t Protocol_addressData[4] = {0xFF,0xFF,0xCC,0x01};
uint8_t Protocol_addressReadRequestData[4] = {0xFF,0xEE,0xDD,0xCC};
uint8_t Protocol_packetSignByte = 0x01;
uint8_t Protocol_packetFollowLengthData[2] = {0x00,0x06};
uint16_t Protocol_packetFollowLength;
uint8_t Protocol_responseCommandByte = 0x00;
uint8_t Protocol_checkSumDataHex[2] = {0x00,0xFF};
uint16_t Protocol_packetAllDataSumLength = 0;
uint8_t userIDPerfixByte = 0xAB;


void sendUartUserID(uint16_t UserIDNum)
{
	uint8_t *userSendData;
	uint8_t userIDLength = 3;
	uint8_t time_ASCIILength = 14;
	uint16_t userSendDataLength = userIDLength + time_ASCIILength;//ReadDS1302ClockASCII(timerASCII);
	
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));	
	
	ReadDS1302ClockASCII(userSendData);	
	userSendData[0 + time_ASCIILength] = userIDPerfixByte;
	userSendData[1 + time_ASCIILength] = (UserIDNum >> 8) & 0x00FF;
	userSendData[2 + time_ASCIILength] = UserIDNum & 0x00FF;
	
	sendOnePacket(0x01,userSendDataLength,0x05,userSendData);	
	
	free(userSendData);
}


void sendUartAddNewUserID(uint16_t UserIDNum)
{
	uint8_t *userSendData;
	uint8_t userIDLength = 3;
	uint8_t time_ASCIILength = 14;
	uint16_t userSendDataLength = userIDLength + time_ASCIILength;//ReadDS1302ClockASCII(timerASCII);
	
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));	
	
	ReadDS1302ClockASCII(userSendData);	
	userSendData[0 + time_ASCIILength] = userIDPerfixByte;
	userSendData[1 + time_ASCIILength] = (UserIDNum >> 8) & 0x00FF;
	userSendData[2 + time_ASCIILength] = UserIDNum & 0x00FF;
	
	sendOnePacket(0x02,userSendDataLength,0x05,userSendData);	
	
	free(userSendData);
}

void sendUartAddNewAppointUserID(uint16_t UserIDNum)
{
	uint8_t *userSendData;
	uint8_t userIDLength = 3;
	uint8_t time_ASCIILength = 14;
	uint16_t userSendDataLength = userIDLength + time_ASCIILength;//ReadDS1302ClockASCII(timerASCII);
	
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));	
	
	ReadDS1302ClockASCII(userSendData);	
	userSendData[0 + time_ASCIILength] = userIDPerfixByte;
	userSendData[1 + time_ASCIILength] = (UserIDNum >> 8) & 0x00FF;
	userSendData[2 + time_ASCIILength] = UserIDNum & 0x00FF;
	
	sendOnePacket(0x06,userSendDataLength,0x05,userSendData);	
	
	free(userSendData);
}

void sendUartTimeData(void)
{
	uint8_t *userSendData;
	uint8_t time_ASCIILength = 14;
	uint16_t userSendDataLength = time_ASCIILength;
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));	
	ReadDS1302ClockASCII(userSendData);	
	sendOnePacket(0x05,userSendDataLength,0x13,userSendData);		
	free(userSendData);
}


void sendUartLocatAddress(void)
{
	uint8_t *userSendData;
	uint16_t userSendDataLength = 0;
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));
	sendOnePacket(0x00,userSendDataLength,0x00,userSendData);		
	free(userSendData);
}


void sendUartOKData(void)
{
	uint8_t *userSendData;
	uint16_t userSendDataLength = 0;
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));
	sendOnePacket(0x00,userSendDataLength,0x01,userSendData);		
	free(userSendData);
}

void sendUartForResend(void)
{
	uint8_t *userSendData;
	uint16_t userSendDataLength = 0;
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));	
	sendOnePacket(0x13,userSendDataLength,0x09,userSendData);		
	free(userSendData);
}

void sendUartOKTimeCheck(void)
{
	uint8_t *userSendData;
	uint16_t userSendDataLength = 0;
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));	
	sendOnePacket(0x08,userSendDataLength,0x13,userSendData);		
	free(userSendData);
}

void sendUartOKClearAll(void)
{
	uint8_t *userSendData;
	uint16_t userSendDataLength = 0;
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));	
	sendOnePacket(0x10,userSendDataLength,0x11,userSendData);		
	free(userSendData);
}

void sendUartOKDelOneUser(void)
{
	uint8_t *userSendData;
	uint16_t userSendDataLength = 0;
	userSendData = (uint8_t *)calloc(userSendDataLength, sizeof(uint8_t));	
	sendOnePacket(0x09,userSendDataLength,0x11,userSendData);		
	free(userSendData);
}

void sendPacketFIFO(uint16_t packetAllDataSumLength)
{
	uint8_t *packetDataFIFO = communicatFIFO;
	while(packetAllDataSumLength--)
	{
		sendUart2OneByte(*packetDataFIFO);
		packetDataFIFO++;
	}	
}

void sendOnePacket(uint8_t packetSignByte,uint16_t userSendDataLength,uint8_t responseCommandByte,uint8_t *userSendData)
{
	uint16_t circleCount = 0;
	uint16_t packetAllDataSumLength = 12;
	uint16_t checkSum = 0;
	Protocol_packetSignByte = packetSignByte;
	Protocol_responseCommandByte = responseCommandByte;
	
	Protocol_packetFollowLength = userSendDataLength + 3;
	Protocol_packetFollowLengthData[0] = (Protocol_packetFollowLength>>8) & 0x00FF;
	Protocol_packetFollowLengthData[1] = Protocol_packetFollowLength  & 0x00FF;
	
	packetAllDataSumLength += userSendDataLength;
	Protocol_packetAllDataSumLength = packetAllDataSumLength;
	free(communicatFIFO);
	communicatFIFO = (uint8_t *)calloc(packetAllDataSumLength, sizeof(uint8_t));
	
	memcpy(communicatFIFO,Protocol_packetStratData,2);
	memcpy(communicatFIFO + 2,Protocol_addressData,4);
	memcpy(communicatFIFO + 6,&Protocol_packetSignByte,1);
	memcpy(communicatFIFO + 7,Protocol_packetFollowLengthData,2);
	memcpy(communicatFIFO + 9,&Protocol_responseCommandByte,1);	
	memcpy(communicatFIFO + 10,userSendData,userSendDataLength);
	
	for(circleCount = 6;circleCount < packetAllDataSumLength - 2 ; circleCount++)
	{
		checkSum += communicatFIFO[circleCount];
	}
	
	
	Protocol_checkSumDataHex[0] = (checkSum>>8) & 0x00FF;
	Protocol_checkSumDataHex[1] = checkSum & 0x00FF;
	
	memcpy(communicatFIFO + 10 + userSendDataLength,Protocol_checkSumDataHex,2);
	
	
	sendPacketFIFO(packetAllDataSumLength);
}



void RespondToPacket()
{
	if(isThePacketEnd)
	{
		if(isDisableCheckSum || checkPacketCheckSumData())
		{
			switch(packetSignDataByte)
			{
				case 0x00:
					sendUartOKData();
				break;
				case 0x01://当前用户
					modeflag = fingerprintSearchMode;//进入查找模式					
				break;
				case 0x02://新增用户
					if(packetResponseCommandData == 0x00)
						modeflag = fingerprintAddInOrderMode;//新增一个用户					
				break;
				case 0x06://新增指定用户
					if(packetUserReceiveData[0] == userIDPerfixByte)
					{		
						SaveNumber = packetUserReceiveData[1];
						SaveNumber <<= 8 ;
						SaveNumber += packetUserReceiveData[2];
						modeflag = fingerprintAddAppointMode;//新增一个用户		
						IsAppointUserID = 1;
					}
					else
					{
						sendUartForResend();
					}
				break;
				case 0x08://校正时间
					WriteDS1302ClockASCII(packetUserReceiveData);
					sendUartTimeData();
				break;
				case 0x09://指定删除
					if(packetUserReceiveData[0] == userIDPerfixByte)
					{								
						DelNumber = packetUserReceiveData[1];
						DelNumber <<= 8 ;
						DelNumber += packetUserReceiveData[2];
						modeflag = fingerprintDelAppointMode;
					}
					else
					{
						sendUartForResend();
					}					
				break;
				case 0x10://全部清空
					modeflag = fingerprintClearAllMode;//清空全部用户
				break;
				case 0x13://重新发包
					sendPacketFIFO(Protocol_packetAllDataSumLength);
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

