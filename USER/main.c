#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"

#include "stdlib.h"


#include "timer.h"
#include "fingerprint.h"
#include "Communication.h"
#include "ds1302.h"


/*
 * ALIENTEK Mini STM32开发板范例代码11
 * TFTLCD显示实验
 * 技术支持：www.openedv.com
 * 广州市星翼电子科技有限公司
 */
int main( void )
{
	delay_init();                   /* 延时函数初始化 */
	NVIC_Configuration();           /* 设置NVIC中断分组2:2位抢占优先级，2位响应优先级 */
	uart_init( 9600 );              /* 串口初始化为9600 */
	uart2_init( 115200 );           /* 串口2初始化为115200 */

	InitClock();

	TIM3_Int_Init( 200, 7199 );     /* 10Khz的计数频率，计数到200为20ms */


	while ( VefPSW() != 1 )         /* 如果传感器握手成功，蜂鸣器响一声 与模块握手通过 */
	{
		delay_ms( 200 );
	}
	
	sendUartStartSuccessfull();
	
	while ( 1 )
	{
		RespondToPacket();
		if ( modeflag == 1 )                                                                            /* 为录入指纹模式 */
		{
			if ( SaveNumber < 162 )                                                                 /*  */
			{
				if ( enroll() == 1 )                                                            /* 采集两次，生成1个指纹模板成功 */
				{
					if ( savefingure( SaveNumber ? SaveNumber : ++SaveNumber ) == 1 )       /* 保存也成功 */
					{
						delay_ms( 100 );
						numshow( SaveNumber );
						addNewUserID = SaveNumber;
						if(IsAppointUserID == 1)
						{
							sendUartAddNewAppointUserID( SaveNumber );
							IsAppointUserID = 0;
						}
						else
						{
							sendUartAddNewUserID( SaveNumber );
						}
						SaveNumber++;                                                   /* 加一次 */
						modeflag = 0;
					}
				}
				else  
				{
					delay_ms( 100 );
					numshow( 0 );
				}
			}
		}


		if ( modeflag == 0 )                    /* 为识别模式 */
		{
			searchnum = search();

			if ( searchnum >= 1 && searchnum <= 1000 )
			{
				numshow( searchnum );   /* 显示搜索到的指纹 */
				if ( searchnum == addNewUserID )
				{
					addNewUserID = 0;
				}
				else  
				{
					sendUartUserID( searchnum );
				}
				/* delay_ms(100); */
			}
			if ( searchnum == 0xFFFF )      /* 识别指纹失败 */
			{
				/* 蜂鸣器响3声 */
				numshow( 0 );
			}
		}
		if ( modeflag == 3 )                    /* 为删除指纹模式 */
		{
			if ( delfingure( DelNumber ) == 1 )
			{
				sendUartOKDelOneUser();
			}
			modeflag = 0;
		}

		if ( clearallflag == 1 )
		{
			clearallflag = 0;
			Clear_All();
			/* 蜂鸣器长响一次，表示清除所有指纹结束 */
			modeflag = 0;
			sendUartOKClearAll();
			SaveNumber = 0;
			numshow( 0 );
		}

		if ( changeflag == 1 )
		{
			changeflag = 0;
		}
	}
}


