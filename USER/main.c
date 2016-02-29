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
 * ALIENTEK Mini STM32�����巶������11
 * TFTLCD��ʾʵ��
 * ����֧�֣�www.openedv.com
 * ������������ӿƼ����޹�˾
 */
int main( void )
{
	delay_init();                   /* ��ʱ������ʼ�� */
	NVIC_Configuration();           /* ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� */
	uart_init( 9600 );              /* ���ڳ�ʼ��Ϊ9600 */
	uart2_init( 115200 );           /* ����2��ʼ��Ϊ115200 */

	InitClock();

	TIM3_Int_Init( 200, 7199 );     /* 10Khz�ļ���Ƶ�ʣ�������200Ϊ20ms */


	while ( VefPSW() != 1 )         /* ������������ֳɹ�����������һ�� ��ģ������ͨ�� */
	{
		delay_ms( 200 );
	}
	
	sendUartStartSuccessfull();
	
	while ( 1 )
	{
		RespondToPacket();
		if ( modeflag == 1 )                                                                            /* Ϊ¼��ָ��ģʽ */
		{
			if ( SaveNumber < 162 )                                                                 /*  */
			{
				if ( enroll() == 1 )                                                            /* �ɼ����Σ�����1��ָ��ģ��ɹ� */
				{
					if ( savefingure( SaveNumber ? SaveNumber : ++SaveNumber ) == 1 )       /* ����Ҳ�ɹ� */
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
						SaveNumber++;                                                   /* ��һ�� */
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


		if ( modeflag == 0 )                    /* Ϊʶ��ģʽ */
		{
			searchnum = search();

			if ( searchnum >= 1 && searchnum <= 1000 )
			{
				numshow( searchnum );   /* ��ʾ��������ָ�� */
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
			if ( searchnum == 0xFFFF )      /* ʶ��ָ��ʧ�� */
			{
				/* ��������3�� */
				numshow( 0 );
			}
		}
		if ( modeflag == 3 )                    /* Ϊɾ��ָ��ģʽ */
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
			/* ����������һ�Σ���ʾ�������ָ�ƽ��� */
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


