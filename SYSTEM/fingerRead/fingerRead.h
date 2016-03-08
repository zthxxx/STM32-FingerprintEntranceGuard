#ifndef __FINGERREAD_H
#define __FINGERREAD_H	 

#include "stdlib.h"
#include "sys.h"   //与STM32相关的一些定义
#include <string.h>

void receiveFingerModelPacket(uint8_t receiveByte);
void RespondToFingerModelPacket(void);









#endif
