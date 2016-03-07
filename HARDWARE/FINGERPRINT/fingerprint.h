#ifndef __FINGERPRINT_H
#define __FINGERPRINT_H	 

#include "stdlib.h"
#include "sys.h"   //��STM32��ص�һЩ����


/*********1������ʱ����**********/
typedef void (*SendUartIDNum)(uint16_t);

void delay1ms(unsigned int t);
void TxdByte(unsigned int ch);//���ڷ�����Ϣ,ͨ����ѯ��ʽ����һ���ַ�
unsigned char Command(unsigned char *p,unsigned char MaxTime); //�������,��ģ�鷢��һ������
unsigned char VefPSW(void);//��֤�豸���ֿ���,�ɹ�����1     
unsigned char Clear_All(void); //���ָ�ƿ�   
unsigned char ImgProcess(unsigned char BUFID);  //����ȡͼ�����������ļ�������BUFID��//�������Ϊ��������  
unsigned int Searchfinger(void);//����ָ��(������������Լ����ݷ���ֵȷ���Ƿ����)        
unsigned int search(void);//����ָ�� 
unsigned char savefingure(unsigned int ID);//����ָ��
unsigned char enroll(void); //�ɼ�����ָ�ƣ�����1�� ָ��ģ��
unsigned char delfingure(unsigned int ID);//����ָ��
unsigned int enteringFingerprint(SendUartIDNum, SendUartIDNum);
void setReadAddressMode(void);
void searchFingerprint(uint16_t* lastAppendNewUserID, SendUartIDNum);


extern unsigned int 	SaveNumber; //ÿ¼��һ��SaveNumber++
extern unsigned int 	IsAppointUserID;
extern unsigned int 	DelNumber;
extern unsigned int     searchnum;
extern unsigned int  	SearchNumber;
extern unsigned char    changeflag,modeflag,clearallflag;  

extern unsigned char fingerprintSearchMode;//0
extern unsigned char fingerprintAddInOrderMode;//1
extern unsigned char fingerprintAddAppointMode;//2
extern unsigned char fingerprintDelAppointMode;
extern unsigned char fingerprintClearAllMode;
extern unsigned char fingerprintReadAddressMode;
#endif

	

