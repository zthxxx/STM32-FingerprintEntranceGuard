#include <string.h>
#include "Communication.h"
#include "stdlib.h"



#define FIFO_MAX_NUMBER    80


uint8_t UART2_DMA_SendBuff[UART2_SEND_TEXT_LENTH];
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



uint8_t receiveCountSign=0;//接受到第几位
uint8_t isThePacketStart = 0;//是否有包
uint8_t isThePacketEnd = 0;//是否包结束
uint8_t packetSignDataByte = 0;//包标识
uint16_t packetFollowLengthData = 0;//后续长度
uint16_t packetUserSendDataLength = 0;//用户数据长度 = 后续长度 - 3
uint8_t packetResponseCommandData = 0;//响应指令
uint8_t packetUserReceiveData[500] = {0};//用户发送有效数据
uint16_t packetCheckSumData = 0;//校验和
uint8_t isDisableCheckSum = 1;//1为关闭校验和	
uint8_t readRequestFlag = 0;//读取地址标记


void retransmissionFingerPrintData(uint8_t* userSendData,uint16_t userSendDataLength,uint8_t ResponseCommandData)
{
    sendOnePacket(0x04,userSendDataLength,ResponseCommandData,userSendData);
}

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
    memcpy(UART2_DMA_SendBuff,communicatFIFO,packetAllDataSumLength);
    USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE); //串口向dma发出请求
    UART2_TXD_DMA_Enable(packetAllDataSumLength);
    
//    uint8_t *packetDataFIFO = communicatFIFO;
//	while(packetAllDataSumLength--)
//	{
//		sendUart2OneByte(*packetDataFIFO);
//		packetDataFIFO++;
//	}
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
                    {
                        modeflag = fingerprintAddInOrderMode;//新增一个用户
                    }
				break;
                case 0x04://导入指定用户
                    if(packetUserReceiveData[0] == userIDPerfixByte)
					{		
						SaveNumber = packetUserReceiveData[1];
						SaveNumber <<= 8 ;
						SaveNumber += packetUserReceiveData[2];
						IsImportUser = 1;
                        WriteFingerFeatureInstruct();
					}
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
                case 0x07://添加指定用户指纹
                    WriteFingerFeatureData(packetUserReceiveData,packetUserSendDataLength);
                    if(packetResponseCommandData == 0x05)
                    {
                        if(savefingure(SaveNumber ? SaveNumber : ++SaveNumber)== 1)      /* 保存也成功 */
                        {
                            sendUartAddNewAppointUserID(SaveNumber);
                        }
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


void receiveUSART2Packet(uint8_t receiveByte)
{
    RespondToPacket();//先判断上一包是否响应
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
		if((receiveCountSign == 0)&&(receiveByte == Protocol_packetStratData[0]))//0xEF  
		{
			receiveCountSign = 1;
		}
		else if((receiveCountSign == 1)&&(receiveByte == Protocol_packetStratData[1]))//0x02
		{
			receiveCountSign = 2;
		}
		else if((receiveCountSign >= 2 && receiveCountSign <= 5)&&(receiveByte == Protocol_addressData[receiveCountSign-2]) && (readRequestFlag == 0))//0xFF 0xFF 0xFF 0xFF 
		{
			receiveCountSign++;
			if(receiveCountSign >= 6) 
				isThePacketStart = 1;
		}
		else if((receiveCountSign >= 2 && receiveCountSign <= 5)&&(receiveByte == Protocol_addressReadRequestData[receiveCountSign-2]))
		{
			receiveCountSign++;
			readRequestFlag = 1;
			if(receiveCountSign >= 6) 
			{
				readRequestFlag = 0;
				receiveCountSign = 0;
                modeflag = fingerprintReadAddressMode;
			}
		}		
		else 
		{
			isThePacketEnd=0;
			receiveCountSign = 0;
		}
	}
}
