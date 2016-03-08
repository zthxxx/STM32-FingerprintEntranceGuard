#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stdlib.h"
#include "timer.h"
#include "fingerprint.h"
#include "Communication.h"
#include "fingerRead.h"
#include "ds1302.h"
#include "dma.h"


int main(void)
{
    uint16_t lastAppendNewUserID = 0;
    
	delay_init();                   /* 延时函数初始化 */
	NVIC_Configuration();           /* 设置NVIC中断分组2:2位抢占优先级，2位响应优先级 */
	uart_init(9600, receiveFingerModelPacket);              /* 串口初始化为9600 */
	uart2_init(115200, receiveUSART2Packet);           /* 串口2初始化为115200 */
	InitClock();                    //DS1302 clock init
    MYDMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)UART2_DMA_SendBuff,UART2_SEND_TEXT_LENTH);//DMA1通道4,外设为串口1,存储器为SendBuff,长(TEXT_LENTH+2)*100.
	TIM3_Int_Init(200, 7199);     /* 10Khz的计数频率，计数到200为20ms */


	while(VefPSW() != 1)        /* 如果传感器握手成功，蜂鸣器响一声 与模块握手通过 */
	{
		delay_ms(200);
	}

    sendUartLocatAddress();     //开机时先推送一次地址
    
	while(1)
	{
		RespondToPacket();
        switch(modeflag)
        {
            case 0:                      /* 为识别模式 */
            {
                searchFingerprint(&lastAppendNewUserID, sendUartUserID);
                break;
            }
            case 1:                     /* 为顺序录入指纹模式 */
            case 2:                     /* 为指定录入指纹模式 */
            {
                lastAppendNewUserID = enteringFingerprint(sendUartAddNewAppointUserID, sendUartAddNewUserID);
                break;
            }
            case 3:                     /* 为删除指纹模式 */
            {
                if(delfingure(DelNumber)== 1)
                {
                    sendUartOKDelOneUser();
                }
                break;
            }
            case 4:                     //清空指纹模式
            {
                if(Clear_All() == 1)
                {
                    SaveNumber = 0;
                    sendUartOKClearAll();
                }
                break;
            }
            default:
                break;
        }
        
        if(modeflag == fingerprintReadAddressMode)
        {
            sendUartLocatAddress(); //读取地址
        }
        
        modeflag = fingerprintSearchMode;

        
		if(changeflag == 1)   //清零状态转换标志
		{
			changeflag = 0;
		}
	}
}


