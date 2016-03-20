#ifndef __FINGERREAD_H
#define __FINGERREAD_H	 

#include "stdlib.h"
#include "sys.h"   //��STM32��ص�һЩ����
#include <string.h>
#include "usart.h"	  

void ReceiveFingerModelPacket(uint8_t receiveByte);
void RespondToFingerModelPacket(void);
void ReadAndProcessFingerPacketFIFO(void);








#endif
