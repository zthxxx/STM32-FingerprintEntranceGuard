#ifndef __FINGERPRINT_H
#define __FINGERPRINT_H	 

#include "sys.h"   //与STM32相关的一些定义
#include "usart.h" //指纹模块 需要串口驱动的支持


/*********1毫秒延时程序**********/
void delay1ms(unsigned int t);
void TxdByte(unsigned int ch);//串口发送信息,通过查询方式发送一个字符
unsigned char Command(unsigned char *p,unsigned char MaxTime); //命令解析,给模块发送一个命令
unsigned char VefPSW(void);//验证设备握手口令,成功返回1     
void Clear_All(void); //清空指纹库   
unsigned char ImgProcess(unsigned char BUFID);  //发获取图像并生成特征文件，存入BUFID中//输入参数为缓冲区号  
unsigned int Searchfinger(void);//搜索指纹(发送搜索命令、以及根据返回值确定是否存在)        
unsigned int search(void);//搜索指纹 
unsigned char savefingure(unsigned int ID);//保存指纹
unsigned char enroll(void); //采集两次指纹，生成1个 指纹模板
unsigned char delfingure(unsigned int ID);//保存指纹
void numshow(unsigned int num);

extern unsigned int 	SaveNumber; //每录入一次SaveNumber++
extern unsigned int 	IsAppointUserID;
extern unsigned int 	DelNumber;
extern unsigned int    searchnum;
extern unsigned int  	SearchNumber;
extern unsigned char changeflag,modeflag,clearallflag;  

#endif

	

