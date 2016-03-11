#include"fingerprint.h"
#include "delay.h"

unsigned int 	SaveNumber=0; //每录入一次SaveNumber++
unsigned int 	IsAppointUserID = 0;

unsigned int 	DelNumber=0; //每录入一次SaveNumber++
unsigned int    searchnum=0;
unsigned int  	SearchNumber=0;		
unsigned int 	clk0=0;
unsigned char   IsImportUser = 0;

extern void sendUart1OneByte(uint8_t byteData);
unsigned char changeflag=0,modeflag=0,clearallflag=0;  
//默认为识别模式，如果为1为录入指纹模式

unsigned char fingerprintSearchMode = 0;
unsigned char fingerprintAddInOrderMode = 1;
unsigned char fingerprintAddAppointMode = 2;
unsigned char fingerprintDelAppointMode = 3;
unsigned char fingerprintClearAllMode = 4;
unsigned char fingerprintReadAddressMode = 5;

uint8_t FingerPrintDataReadFlag = 0;

#define MAX_NUMBER    80 

unsigned char 	 FifoNumber=0; 
unsigned char    FIFO[MAX_NUMBER]={0};



//Verify  Password   ：验证设备握手口令: 发送16个字节的数据给传感器， 传感器回传12个字节
 unsigned char  VPWD[17]={16,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};	


//GetImage           ：探测手指并从传感器上读入图像:发送12个字节给传感器，传感器回传12个字节
 unsigned char  GIMG[13]={12, 0xef,0x01,0xff,0xff,0xff,0xff, 0x01,0x00,0x03,0x01,0x00,0x05};

//Gen Templet1       ：根据原始图像生成指纹特征1 : 发送12个字节给传感器，传感器回传12个字节
 unsigned char  GENT1[14]={13,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x02,0x01,0x00,0x08};

//Gen Templet2       ：根据原始图像生成指纹特征2 : 发送12个字节给传感器，传感器回传12个字节
 unsigned char  GENT2[14]={13,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x02,0x02,0x00,0x09}; 	
	
//Search Finger      ：以CharBufferA或CharBufferB中的特征文件搜索整个或部分指纹库
//发送17个字节给传感器，回送16个字节，指纹得分没有什么意义，可以不理会，只关心是几号指纹，就是存在
//模块内部什么位置，对应几号指纹（1000个指纹的地址是 0---999） 
//999对应的两位十六进制为 0x03 0xe7,0x03是高字节，也就是乘以256
 unsigned char  SEAT[18]={17,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xe7,0x00,0xf8};

//Merge Templet      ;将CharBufferA与CharBufferB中的特征文件合并生成模板，结果存于ModelBuffer。
//单片机发送12个字节给指纹模块，指纹模块回传12个字节
 unsigned char  MERG[13]={12,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x03,0x05,0x00,0x09};//回送12个	

 
//读取当前缓冲区特征数据
 unsigned char  ReadFeature[14]={13,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x08,0x02,0x00,0x0F};
 
 //写入当前缓冲区特征数据
 unsigned char  WriteFeature[14]={13,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x09,0x02,0x00,0x10};
 
//Store Templet      ：将ModelBuffer2中的文件储存到flash指纹库中
//单片机发送15个字节给指纹模块，指纹模块回传12个字节
//这个只是基本的命令，里面的位置是要能改变的，请查看存储函数，里面有临时变量，会重新赋值
 unsigned char   STOR[16]={15,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x06,0x06,0x02,0x00,0x00,0x00,0x10};
 //DEL templet      ;删除指定
unsigned char DelChar[17]={16,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x07,0x0c,0x00,0x00,0x00,0x01,0x00,0x0f};
 
 
//DEL templet      ;清空指纹库--
//单片机发送12个字节给指纹模块，指纹模块回传12个字节
 unsigned char  DELE_all[13]={12,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x03,0x0d,0x00,0x11};


 
void SetUART1_NVIC_ISENABLE(uint8_t isEnable)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    if(isEnable == 1)
    {
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    }
    else
    {
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//IRQ通道使能
    }
	
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}
 

unsigned char Command(unsigned char *p,unsigned char MaxTime) //命令解析,给模块发送一个命令
{
  	unsigned char count=0,tmpdat=0,temp=0,i=0,package=0,flag=0;
	unsigned int  checksum=0;	
	unsigned char result=0, start=0,stop=0;

    SetUART1_NVIC_ISENABLE(0);    
    
  	i=*p;             //数组的第“0”个元素、里面存放了本数组的长度，把这个长度给变量i，方便进行操作
	p++;

  	for (count=0; count<i; count++)//把要发给指纹模块的命令通过单片机串口发送给模块
    {
		temp=*p;    //取得里面的内容，再指针加1，指向下一个命令字节。
		sendUart1OneByte(temp);//单片机串口将数据发送给指纹模块	
		p++;
	}
	  
  	result = 1;    //发送完成,结果为真 (真为1)   	
  	FifoNumber=0;
  	for (count=0; count<MAX_NUMBER; count++)//清空所有FIFO[]数组里面的内容，写入0X00
	{
	    FIFO[count]=0x00; 
	}
		  
  	if (1 == result)   
   	{		
     	result= 0;
      	start = 0;
   		stop  = 0;

       	count = 0;
       	clk0 = 0;	//清零CL0计数
		
       	do /////////////////////////////do的内容////////////////////////////////
		{	
			restart0:
					
	         	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//如果单片机串口 接收到数据 
				//不同的单片机或者ARM芯片，是通过不同的查询方式，来查询串口接收中断标志的
				//请根据不同的单片机型号更改这个查询命令，查询后，有些单片机需要手动清零
				//否则数据会有问题，这个地方用了一个goto语句，我相信稍微有能力的人，不会觉得
				//这个goto语句理解起来会有什么困难，如果不能理解，请自己更改为中断
				//至于每个单片机中断处理机制不一样，我们不提供技术支持，请自己研究！
		      	{ 				
			  		tmpdat = USART_ReceiveData(USART1);          //先把接收到的数据放到tmpdat中
					USART_ClearFlag(USART1,USART_FLAG_RXNE);	 //清除串口接收中断

	            	if ((tmpdat==0xef)&&(start==0))//这个数据为第一个传回来的数据，也就是“指令应答”的第一个字节
	            	{ 
						count=0;
				    	FIFO[0] = tmpdat;//读入第一个应答字节(0XEF)，存在第“0”个元素中    
						flag=1;	
						goto 
							restart0;//可以用中断方式进行			
	             	}

					if(flag==1)//第一个字节已经回来，所以flag==1成立
					{  
						if(tmpdat!=0x01)  //接收数据错误，将重新从缓冲区接收数据
						{  	
							flag=0;//接收应答失败
							result=0;
	      					start =0;
	   						stop=0;
	       					count=0;
							goto 
								restart0;					
						}
						//如果成功接收到0xef  0x01，可以开始接收数据
						flag = 2;//flag=2;表示应答成功，可以开始接收数据了
						count++;//现在count=1;
						FIFO[count] = tmpdat;//读入第二个应答字节（0X01），存在第“1”个元素中    
						start = 1;	//应答成功可以开始接收数据
						    goto 
								restart0;	
					}  
					                
	             	if((flag==2)&&(start==1))//flag=2;表示应答成功，可以开始接收数据了
	             	{	   	  					 
				   		count++; //数据元素下标＋＋
			            FIFO[count] = tmpdat;//存入数据
						if(count>=6)
						{
							checksum = FIFO[count] + checksum; //计算校验和
						}
						if(count==8)
						{ 
							package=FIFO[7]*256+FIFO[8];	//计算包长度							
							stop= 1;
						}
						if(1==stop)
						{						
							if(count==package+8)
							{
								checksum = (checksum - FIFO[count-1] - FIFO[count]);
								if(checksum != FIFO[count-1]*256 + FIFO[count] )
								{
								   result=0; //校验失败,置结果标志为0
								}								
								else 
								{
								   result=1;
								}	
								flag = 0;
								break;
							} 
						}
	             	}
	 	       	}
        }/////////////////////////////do的内容----------------结束////////////////////////////////
		while ((clk0 <= MaxTime) && (count <MAX_NUMBER) && (changeflag==0)); //由定时器以及最大接收数据来控制，保证不会在此一直循环
        USART_ClearFlag(USART1,USART_FLAG_RXNE);	 //清除串口接收中断/;
		FifoNumber = count;	//保存接收到的数据个数
        FingerPrintDataReadFlag = 0;
	 }
   return (result);
}

void WriteFingerModelCommand(uint8_t* CommandInstruct)
{
    uint8_t sendLength = 0;
    uint8_t sendCount = 0;
    sendLength = CommandInstruct[0];
    while(sendCount < sendLength)
    {
        sendUart1OneByte(CommandInstruct[++sendCount]);
    }
}

void ReadFingerFeatureData()
{
    SetUART1_NVIC_ISENABLE(1);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启接收中断
    FingerPrintDataReadFlag = 1;
    WriteFingerModelCommand(ReadFeature);
    clk0 = 0;
    while(FingerPrintDataReadFlag == 1 && clk0 <= 200)
    {
        RespondToFingerModelPacket();
    }
}

void WriteFingerFeatureInstruct()
{
    WriteFingerModelCommand(WriteFeature);
    delay_ms(200);
}


void WriteFingerFeatureData(uint8_t* packetUserReceiveData,uint16_t packetUserSendDataLength)
{
    uint8_t *packetDataFIFO = packetUserReceiveData;
	while(packetUserSendDataLength--)
	{
		sendUart1OneByte(*packetDataFIFO);
		packetDataFIFO++;
	}
}


unsigned char VefPSW(void)//验证设备握手口令,成功返回1     
{
 	unsigned char  count=0;
	while (1)
   	{
     	if(Command(VPWD,20) && (FifoNumber==11) && (FIFO[9]==0x00))  
		{
		  return(1);
		}	
     	count++;
   	  	if (count>=3)//如果不成功，再验证一次，如果3次不成功，返回失败
        {  
            return(0);   
 	    }
	}
}


unsigned char Clear_All(void) //清空指纹库   
{				
      delay_ms(200);
	  if(Command(DELE_all,50)) //清空指纹库  		
      {
          return 1;
      }
      else
      {
          return 0;
      }
}

unsigned char ImgProcess(unsigned char BUFID)  //发获取图像并生成特征文件，存入BUFID中//输入参数为缓冲区号  
{	
    if(Command(GIMG,89) && (FifoNumber==11) && (FIFO[9]==0x00))  
    {
		if(BUFID==1)      
		{	   			
      		if(Command(GENT1,60) && (FifoNumber==11) && (FIFO[9]==0x00))  
        	{							
				return 1;
			}  
        	else
        	{
  				return 0;
         	}
		 }
		else if(BUFID==2)
		{
		  	if(Command(GENT2,60) && (FifoNumber==11) && (FIFO[9]==0x00))  
        	{				
				return 1;
			}  
        	else
        	{
  				return 0;
         	}			
		}
    }
    else
    {
      return 0;       
    } 
	return 0;            
}


unsigned int Searchfinger(void)//搜索指纹(发送搜索命令、以及根据返回值确定是否存在)        
{		
   	if(Command(SEAT,60) && (FifoNumber==15) && (FIFO[9]==0x00) )  
    {
		SearchNumber = FIFO[10]*256 + FIFO[11];//搜索到的页码,从而判断是几号指纹
	   	return 1;
	}     
    else             
    {
       	return 0;
    }  
}


unsigned int search(void)//搜索指纹 
{
 	unsigned char SearchBuf=0;
//    unsigned char i=0;
//  	while (i < 3)  //i越大  可能读入图像次数越多，速度越慢，指令反应越慢
//    {
        if (ImgProcess(1)==1)//首先读入一次指纹  
        {
            SearchBuf = Searchfinger();//进行指纹比对，如果搜索到，返回搜索到的指纹序号
            if(SearchBuf==1)
            {
                return SearchNumber; 
            }
            else 
            {
                return 0xFFFF;//表示搜索到的指纹不正确
            }     
        }
//        i++;
//    }
    return 0;
}

unsigned char savefingure(unsigned int ID)//保存指纹
{
	 unsigned char i=0;
	 unsigned int sum = 0;
	
	 //现在开始进行存储指纹模板的操作
     for (i=0;i<16;i++)	//保存指纹信息
	 {
		FIFO[i]=STOR[i];
	 }
	  
	 FIFO[12]=ID/256;
     FIFO[13]=ID%256; //把指纹模板存放的PAGE_ID也就是FLASH的位置

	 //现在开始计算校验和 由于第一个字节（数组地址0）存放的是这个命令长度，所以要注意位置
     for (i=7;i<14;i++)	
	 {
	   sum= sum + FIFO[i];
	 }
	 FIFO[14]=sum/256;	//校验和
     FIFO[15]=sum%256;	//校验和

     if (Command(FIFO,70)==1 && FIFO[9]==0x00)//此处进行存放指纹模板的命令
	 {
	   return(1);
	 }
     else
	 {
	   return(0);//不成功返回0
	 } 
}

unsigned char delfingure(unsigned int ID)//删除指纹
{
	 unsigned char i=0;
	 unsigned int sum = 0;
	
	 //现在开始进行存储指纹模板的操作
     for (i=0;i<17;i++)	//保存指纹信息
	 {
		FIFO[i]=DelChar[i];
	 }
	  
	 FIFO[11]=ID/256;
     FIFO[12]=ID%256; //把指纹模板存放的PAGE_ID也就是FLASH的位置

	 //现在开始计算校验和 由于第一个字节（数组地址0）存放的是这个命令长度，所以要注意位置
     for (i=7;i<15;i++)	
	 {
	   sum= sum + FIFO[i];
	 }
	 FIFO[15]=sum/256;	//校验和
     FIFO[16]=sum%256;	//校验和

     if (Command(FIFO,80)==1 && FIFO[9]==0x00)//此处进行存放指纹模板的命令
	 {
	   return(1);
	 }
     else
	 {
	   return(0);//不成功返回0
	 } 
}


unsigned char enroll(void) //采集两次指纹，生成1个 指纹模板
{
 	unsigned char temp=0,count=0;
  	while(1)
  	{
  	 	temp=ImgProcess(1); //生成特征1    
   		if (temp==1)//生成特征文件成功             
      	{  		      
        	//采集第一个特征成功 
		   	count=0;  
		 
			delay_ms(200);
	 
			break;
       	}
     	else                   
       	{
        	if (temp==0)//采集指纹没有成功
          	{ 
				count++;
            	if (count>=40)//如果采集了40次，还不成功，直接采集失败，直接退出enroll函数－－－－返回0  
					return(0);
            }
        }
   	}

	delay_ms(2000);//延时2S开始采集下一个特征

	//开始采集第二个特征 
 	while(1)
  	{
   		temp=ImgProcess(2); //生成特征2    
   		if (temp==1)//生成特征文件2成功
      	{
        	if( (Command(MERG,40)==1) && (FifoNumber==11) && (FIFO[9]==0x00)  ) //合并成功返回1
			{
				 return(1);
			}
			else
			{
			   return(0);
			}		
        }
      	else      
       	{	
        	if (temp==1)//采集指纹没有成功
        	{
				count++;
				
				if (count>=25) 
				
				return(0);
       		}
     	}
   	}
}



 
unsigned int enteringFingerprint(SendUartIDNum sendUartAddNewAppointUserIDSub,SendUartIDNum sendUartAddNewUserIDSub)   /* 为录入指纹模式 */
{
    uint16_t lastAppendNewUserID = 0;
    if(SaveNumber < 1000)                                                                /*  */
    {
        if(enroll() == 1)                                                           /* 采集两次，生成1个指纹模板成功 */
        {
            if(savefingure(SaveNumber ? SaveNumber : ++SaveNumber)== 1)      /* 保存也成功 */
            {
                delay_ms(100);              
                if(IsAppointUserID == 1)
                {
                    IsAppointUserID = 0;
                    sendUartAddNewAppointUserIDSub(SaveNumber);
                }
                else
                {
                    sendUartAddNewUserIDSub(SaveNumber);
                }
                lastAppendNewUserID = SaveNumber;
                SaveNumber++;                                                   /* 加一次 */
                ReadFingerFeatureData();
            }
        }
    }
    return lastAppendNewUserID;
}
 
void searchFingerprint(uint16_t* lastAppendNewUserID, SendUartIDNum sendUartUserIDSub)         /* 为识别模式 */
{
    searchnum = search();
    if((searchnum == 0) || (searchnum == 0xFFFF))     /* 识别指纹失败 */
    {
        return;
    }
    if(searchnum >= 1 && searchnum <= 1000)
    {
        if(searchnum == *lastAppendNewUserID)
        {
            *lastAppendNewUserID = 0;
        }
        else  
        {
            sendUartUserIDSub(searchnum);
        }
    }
}



void setReadAddressMode(void)
{
    modeflag = fingerprintReadAddressMode;
}
