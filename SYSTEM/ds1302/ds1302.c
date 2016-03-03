#include "ds1302.h"



void DS1302_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* PE4,5,6��� */
	GPIO_ResetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//50Mʱ���ٶ�
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


static void DelayNOP(u32 count)
{
	while(count--) __ASM ("nop");
}

static void DS1302_OUT(void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void DS1302_IN(void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DS1302SendByte(u8 byte)
{
	u8	i;

	for(i=0x01;i;i<<=1)
	{
		if(byte&i)	DS1302_OUT_H();
		else	DS1302_OUT_L();
		DS1302_CLK_H();
		DelayNOP(50);		//����ʱ
		DS1302_CLK_L();
		__ASM ("nop");__ASM ("nop");		//����ʱ
	}
}
u8 DS1302ReceiveByte(void)
{
	u8	i,byte=0;

	for(i=0x01;i;i<<=1)
	{
		if(DS1302_IN_X)	byte |= i;
		DS1302_CLK_L();
		DelayNOP(50);		//����ʱ
		DS1302_CLK_H();
		__ASM ("nop");__ASM ("nop");		//����ʱ
	}
	return(byte>>1);
}

void timerBCDtoASCII(u8 *time_BCD,u8 *time_ASCII)
{
	u8 numBCD2ASCII = 48;
	u8 time_BCDLength = 6;
	u8 count = 0;
	
	*(time_ASCII++) = '2';
	*(time_ASCII++) = '0';
	for(count = 0; count < time_BCDLength; count++)
	{
		*(time_ASCII++) = (((*time_BCD)>>4) & 0x0F) + numBCD2ASCII;
		*(time_ASCII++) = ((*(time_BCD++)) & 0x0F) + numBCD2ASCII;
	}
}


void timerASCIItoBCD(u8 *time_BCD,u8 *time_ASCII)
{
	u8 numBCD2ASCII = 48;
	u8 time_BCDLength = 6;
	u8 count = 0;
	
	time_ASCII+=2;//���˵�ǧ�����  ��ʮ�꿪ʼ
	for(count = 0; count < time_BCDLength; count++)
	{		
		*(time_BCD) = (*(time_ASCII++) - numBCD2ASCII) << 4;
		*(time_BCD++) |= (*(time_ASCII++) - numBCD2ASCII) & 0x0F;
	}
}




void Write1302(u8 addr,u8 data)
{
    DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(addr);
	DS1302SendByte(data);
	DelayNOP(100);
	DS1302_RST_L();
}

u8 Read1302(u8 addr)
{
    u8 data=0;

    DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(addr|0x01);
	DS1302_IN();
	data = DS1302ReceiveByte();
	DelayNOP(100);
	DS1302_RST_L();
	return(data);
}

//��ȡʱ�亯��
void DS1302_GetTime(u8 *time)
{

	
	time[0] = Read1302(DS1302_YEAR);
	time[1] = Read1302(DS1302_WEEK);
	time[2] = Read1302(DS1302_MONTH);
	time[3] = Read1302(DS1302_DAY);
	time[4] = Read1302(DS1302_HOUR);
	time[5] = Read1302(DS1302_MINUTE);
	time[6] = Read1302(DS1302_SECOND);	
}

/*
��ȡDS1302�е�RAM
addr:��ַ,��0��30,��31���ֽڵĿռ�
����Ϊ����ȡ������
*/
u8 ReadDS1302Ram(u8 addr)
{
	u8	tmp,res;

	tmp = (addr<<1)|0xc0;
	res = Read1302(tmp);
	return(res);
}

/*
дDS1302�е�RAM
addr:��ַ,��0��30,��31���ֽڵĿռ�
data:Ҫд������
*/
void WriteDS1302Ram(u8 addr,u8 data)
{
	u8	tmp;

	Write1302(DS1302_WRITE,0x00);		//�ر�д����
	tmp = (addr<<1)|0xc0;
	Write1302(tmp,data);
	Write1302(DS1302_WRITE,0x80);		//��д����
}

void ReadDSRam(u8 *p,u8 add,u8 cnt)
{
	u8 i;
	
	if(cnt>30) return;
	for(i=0;i<cnt;i++)
	{
		*p = ReadDS1302Ram(add+1+i);
		p++;
	}
}

void WriteDSRam(u8 *p,u8 add,u8 cnt)
{
	u8 i;
	
	if(cnt>30) return;
	for(i=0;i<cnt;i++)
	{
		WriteDS1302Ram(add+1+i,*p++);
	}
}
  
/*
��ʱ�亯��,˳��Ϊ:��������ʱ����
*/
void ReadDS1302Clock(u8 *p)
{
	DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(0xbf);			//ͻ��ģʽ
	DS1302_IN();
	p[5] = DS1302ReceiveByte();		//��
	p[4] = DS1302ReceiveByte();		//��
	p[3] = DS1302ReceiveByte();		//ʱ
	p[2] = DS1302ReceiveByte();		//��
	p[1] = DS1302ReceiveByte();		//��
	DS1302ReceiveByte();			//��
	p[0] = DS1302ReceiveByte();		//��
	DS1302ReceiveByte();			//������־�ֽ�
	DelayNOP(100);
	DS1302_RST_L();
}

/*
дʱ�亯��,˳��Ϊ:��������ʱ����
*/
void WriteDS1302Clock(u8 *p)
{
	Write1302(DS1302_WRITE,0x00);		//�ر�д����
	DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(0xbe);				//ͻ��ģʽ
	DS1302SendByte(p[5]);				//��
	DS1302SendByte(p[4]);				//��
	DS1302SendByte(p[3]);				//ʱ
	DS1302SendByte(p[2]);				//��
	DS1302SendByte(p[1]);				//��
	DS1302SendByte(0x01);				//�ܣ����ó���һ��û��ʹ��
	DS1302SendByte(p[0]);				//��
	DS1302SendByte(0x80);				//������־�ֽ�
	DelayNOP(100);
	DS1302_RST_L();
}

void ReadDS1302ClockASCII(u8 *timerASCII)
{
	u8 timerBCD[6] = {0};
	ReadDS1302Clock(timerBCD);
	timerBCDtoASCII(timerBCD,timerASCII);	
}

void WriteDS1302ClockASCII(u8 *timerASCII)
{
	u8 timerBCD[6] = {0};
	timerASCIItoBCD(timerBCD,timerASCII);
	WriteDS1302Clock(timerBCD);		
}


void InitClock(void)
{
	u8	tmp = 0;

	DS1302_Configuration();
	tmp = ReadDS1302Ram(0);
	
	if(tmp != 0x25)
	{
		WriteDS1302Ram(0,0x25);
		Write1302(DS1302_WRITE,0x00);		//�ر�д����
		Write1302(0x90,0x03);				//��ֹ������
		Write1302(DS1302_HOUR,0x00);		//���ó�24Сʱ��
		Write1302(DS1302_SECOND,0x00);		//ʹ��ʱ������
		WriteDS1302ClockASCII("20160115023500");
		Write1302(DS1302_WRITE,0x80);		//��д����
	}
	//
}

void TestDS1302(void)
{
	u8 i,tt[7],dd1[30],dd2[30];
	
	DS1302_Configuration();
	InitClock();
	tt[0] = 0x13;
	tt[1] = 0x05;
	tt[2] = 0x23;
	tt[3] = 0x09;
	tt[4] = 0x25;
	tt[5] = 0x00;
	WriteDS1302Clock(tt);
	for(i=0;i<30;i++)
	{
		dd1[i] = i;
		dd2[i] = 0;
	}
	WriteDSRam(dd1,0,30);
	ReadDSRam(dd2,0,30);
	while(1) 
	{
		ReadDS1302Clock(tt);
	}
}

//end

