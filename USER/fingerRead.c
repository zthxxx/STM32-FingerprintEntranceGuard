#include "fingerRead.h"

uint8_t Finger_Model_Protocol_packetStratData[2] = {0xEF,0x01};
uint8_t Finger_Model_Protocol_addressData[4] = {0xFF,0xFF,0xFF,0xFF};
uint8_t Finger_Model_Protocol_packetSignByte = 0x01;
uint8_t Finger_Model_Protocol_packetFollowLengthData[2] = {0x00,0x06};
uint16_t Finger_Model_Protocol_packetFollowLength;
uint8_t Finger_Model_Protocol_responseCommandByte = 0x00;
uint8_t Finger_Model_Protocol_checkSumDataHex[2] = {0x00,0xFF};
uint16_t Finger_Model_Protocol_packetAllDataSumLength = 0;
uint8_t Finger_Model_Protocol_isDisableCheckSum = 1;//1为关闭校验和


uint8_t Finger_Model_receiveCountSign=0;//接受到第几位
uint8_t Finger_Model_isThePacketStart = 0;//是否有包开始 即地址正确
uint8_t Finger_Model_isThePacketEnd = 0;//是否包结束
uint8_t Finger_Model_packetSignDataByte = 0;//包标识
uint8_t Finger_Model_packetFollowLengthData[2] = {0};
uint16_t Finger_Model_packetFollowDataLength = 0;//后续长度
uint16_t Finger_Model_packetUserSendDataLength = 0;//用户数据长度 = 后续长度 - 2
uint16_t Finger_Model_packetDataLength = 0;//总包长 = 后续长度 + 9
uint8_t Finger_Model_packetResponseCommandData = 0;//响应指令
uint8_t Finger_Model_packetUserReceiveData[500] = {0};//用户发送有效数据
uint16_t Finger_Model_packetCheckSum = 0;
uint8_t Finger_Model_packetCheckSumData[2] = {0};//校验和
uint8_t FingerModelDataPacket[500];

uint8_t countReacive = 0;
extern uint8_t FingerPrintDataReadFlag;
extern void retransmissionFingerPrintData(uint8_t* userSendData,uint16_t userSendDataLength);


void combinationFingerModelDataPacket()
{
    
    //FingerModelDataPacket = (uint8_t *)calloc(Finger_Model_packetDataLength, sizeof(uint8_t));
    
    memcpy(FingerModelDataPacket,Finger_Model_Protocol_packetStratData,2);
	memcpy(FingerModelDataPacket + 2,Finger_Model_Protocol_addressData,4);
	memcpy(FingerModelDataPacket + 6,&Finger_Model_packetSignDataByte,1);
	memcpy(FingerModelDataPacket + 7, Finger_Model_packetFollowLengthData,2);
	memcpy(FingerModelDataPacket + 9,Finger_Model_packetUserReceiveData,Finger_Model_packetUserSendDataLength);
    memcpy(FingerModelDataPacket + 9 + Finger_Model_packetUserSendDataLength,Finger_Model_packetCheckSumData,2);

}


uint8_t checkFingerModelPacketCheckSum()
{
	uint16_t thePacketCheckSum=0;
	uint16_t count=0;
	uint8_t isPacketSumRigth = 0;
	thePacketCheckSum = Finger_Model_packetSignDataByte + Finger_Model_packetFollowDataLength;
	for(count=0;count<Finger_Model_packetUserSendDataLength;count++)
	{
		thePacketCheckSum += Finger_Model_packetUserReceiveData[count];
	}
	
	if(thePacketCheckSum == Finger_Model_packetCheckSum)
	{
		isPacketSumRigth = 1;
	}
	
	return isPacketSumRigth;
}



void RespondToFingerModelPacket()
{
	if(Finger_Model_isThePacketEnd)
	{
        Finger_Model_isThePacketEnd=0;
		if(Finger_Model_Protocol_isDisableCheckSum || checkFingerModelPacketCheckSum())
		{
            
			switch(Finger_Model_packetSignDataByte)
			{
				case 0x02:
                    countReacive++;
                    combinationFingerModelDataPacket();
                    retransmissionFingerPrintData(FingerModelDataPacket, Finger_Model_packetDataLength);
                    break;
                case 0x08:
                    combinationFingerModelDataPacket();
                    retransmissionFingerPrintData(FingerModelDataPacket, Finger_Model_packetDataLength);
                    FingerPrintDataReadFlag = 0;
                    break;
                default:
                    break;
			}
		}
		
        
	}	
}


void receiveFingerModelPacket(uint8_t receiveByte)
{
    RespondToFingerModelPacket();
	if(Finger_Model_isThePacketStart)
	{
		if(Finger_Model_receiveCountSign == 6)//&& receiveByte != 0xFF
		{
			Finger_Model_packetSignDataByte = receiveByte;
			Finger_Model_receiveCountSign = 7;
		}
		else if(Finger_Model_receiveCountSign == 7)
		{
            Finger_Model_packetFollowLengthData[0] = receiveByte;
			Finger_Model_packetFollowDataLength = receiveByte & 0x00FF;
			Finger_Model_packetFollowDataLength <<= 8;
			Finger_Model_receiveCountSign = 8;
		}
		else if(Finger_Model_receiveCountSign == 8)
		{
            Finger_Model_packetFollowLengthData[1] = receiveByte;
			Finger_Model_packetFollowDataLength |= receiveByte & 0x00FF;
			Finger_Model_packetUserSendDataLength = Finger_Model_packetFollowDataLength - 2;
            Finger_Model_packetDataLength = Finger_Model_packetFollowDataLength + 9;
			Finger_Model_receiveCountSign = 9;
		}
		else if((Finger_Model_receiveCountSign >= 9) && (Finger_Model_receiveCountSign < 9 + Finger_Model_packetUserSendDataLength))
		{
			Finger_Model_receiveCountSign++;			
			Finger_Model_packetUserReceiveData[Finger_Model_receiveCountSign-10] = receiveByte;				
		}
		else if(Finger_Model_receiveCountSign == 9 + Finger_Model_packetUserSendDataLength)
		{
            Finger_Model_packetCheckSumData[0] = receiveByte;
			Finger_Model_packetCheckSum = receiveByte & 0x00FF;
			Finger_Model_packetCheckSum <<= 8;
			Finger_Model_receiveCountSign++;
		}
		else if(Finger_Model_receiveCountSign == 10 + Finger_Model_packetUserSendDataLength)
		{
            Finger_Model_packetCheckSumData[1] = receiveByte;
			Finger_Model_packetCheckSum |= receiveByte & 0x00FF;
			Finger_Model_isThePacketEnd = 1;
			Finger_Model_isThePacketStart = 0;
			Finger_Model_receiveCountSign = 0;
		}
	}
	else
	{
		if((Finger_Model_receiveCountSign == 0)&&(receiveByte == Finger_Model_Protocol_packetStratData[0]))//0xEF  
		{
			Finger_Model_receiveCountSign = 1;
		}
		else if((Finger_Model_receiveCountSign == 1)&&(receiveByte == Finger_Model_Protocol_packetStratData[1]))//0x01
		{
			Finger_Model_receiveCountSign = 2;
		}
		else if((Finger_Model_receiveCountSign >= 2 && Finger_Model_receiveCountSign <= 5)&&(receiveByte == Finger_Model_Protocol_addressData[Finger_Model_receiveCountSign-2]))//0xFF 0xFF 0xFF 0xFF 
		{
			Finger_Model_receiveCountSign++;
			if(Finger_Model_receiveCountSign >= 6) 
				Finger_Model_isThePacketStart = 1;
		}
		else 
		{
			Finger_Model_isThePacketEnd=0;
			Finger_Model_receiveCountSign = 0;
		}
	}
}
