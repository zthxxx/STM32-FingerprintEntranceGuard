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
    
	delay_init();                   /* ��ʱ������ʼ�� */
	NVIC_Configuration();           /* ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� */
	uart_init(9600, receiveFingerModelPacket);              /* ���ڳ�ʼ��Ϊ9600 */
	uart2_init(115200, receiveUSART2Packet);           /* ����2��ʼ��Ϊ115200 */
	InitClock();                    //DS1302 clock init
    MYDMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)UART2_DMA_SendBuff,UART2_SEND_TEXT_LENTH);//DMA1ͨ��4,����Ϊ����1,�洢��ΪSendBuff,��(TEXT_LENTH+2)*100.
	TIM3_Int_Init(200, 7199);     /* 10Khz�ļ���Ƶ�ʣ�������200Ϊ20ms */


	while(VefPSW() != 1)        /* ������������ֳɹ�����������һ�� ��ģ������ͨ�� */
	{
		delay_ms(200);
	}

    sendUartLocatAddress();     //����ʱ������һ�ε�ַ
    
	while(1)
	{
		RespondToPacket();
        switch(modeflag)
        {
            case 0:                      /* Ϊʶ��ģʽ */
            {
                searchFingerprint(&lastAppendNewUserID, sendUartUserID);
                break;
            }
            case 1:                     /* Ϊ˳��¼��ָ��ģʽ */
            case 2:                     /* Ϊָ��¼��ָ��ģʽ */
            {
                lastAppendNewUserID = enteringFingerprint(sendUartAddNewAppointUserID, sendUartAddNewUserID);
                break;
            }
            case 3:                     /* Ϊɾ��ָ��ģʽ */
            {
                if(delfingure(DelNumber)== 1)
                {
                    sendUartOKDelOneUser();
                }
                break;
            }
            case 4:                     //���ָ��ģʽ
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
            sendUartLocatAddress(); //��ȡ��ַ
        }
        
        modeflag = fingerprintSearchMode;

        
		if(changeflag == 1)   //����״̬ת����־
		{
			changeflag = 0;
		}
	}
}


