#ifndef __FINGERPRINT_H
#define __FINGERPRINT_H	 

#include "stdlib.h"
#include "sys.h"   //与STM32相关的一些定义


/*********1毫秒延时程序**********/
typedef void (*SendUartIDNum)(uint16_t);

void delay1ms(unsigned int t);
void TxdByte(unsigned int ch);//串口发送信息,通过查询方式发送一个字符
unsigned char Command(unsigned char *p,unsigned char MaxTime); //命令解析,给模块发送一个命令
unsigned char VefPSW(void);//验证设备握手口令,成功返回1     
unsigned char Clear_All(void); //清空指纹库   
unsigned char ImgProcess(unsigned char BUFID);  //发获取图像并生成特征文件，存入BUFID中//输入参数为缓冲区号  
unsigned int Searchfinger(void);//搜索指纹(发送搜索命令、以及根据返回值确定是否存在)        
unsigned int search(void);//搜索指纹 
unsigned char savefingure(unsigned int ID);//保存指纹
unsigned char enroll(void); //采集两次指纹，生成1个 指纹模板
unsigned char delfingure(unsigned int ID);//保存指纹
unsigned int enteringFingerprint(SendUartIDNum, SendUartIDNum);
void setReadAddressMode(void);
void searchFingerprint(uint16_t* lastAppendNewUserID, SendUartIDNum);


extern unsigned int 	SaveNumber; //每录入一次SaveNumber++
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

	

