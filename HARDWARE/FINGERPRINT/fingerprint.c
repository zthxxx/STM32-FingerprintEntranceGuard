#include"fingerprint.h"
#include "delay.h"

unsigned int 	SaveNumber=0; //ÿ¼��һ��SaveNumber++
unsigned int 	IsAppointUserID = 0;

unsigned int 	DelNumber=0; //ÿ¼��һ��SaveNumber++
unsigned int    searchnum=0;
unsigned int  	SearchNumber=0;		
unsigned int 	clk0=0;
unsigned char   IsImportUser = 0;

extern void sendUart1OneByte(uint8_t byteData);
unsigned char changeflag=0,modeflag=0,clearallflag=0;  
//Ĭ��Ϊʶ��ģʽ�����Ϊ1Ϊ¼��ָ��ģʽ

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



//Verify  Password   ����֤�豸���ֿ���: ����16���ֽڵ����ݸ��������� �������ش�12���ֽ�
 unsigned char  VPWD[17]={16,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};	


//GetImage           ��̽����ָ���Ӵ������϶���ͼ��:����12���ֽڸ����������������ش�12���ֽ�
 unsigned char  GIMG[13]={12, 0xef,0x01,0xff,0xff,0xff,0xff, 0x01,0x00,0x03,0x01,0x00,0x05};

//Gen Templet1       ������ԭʼͼ������ָ������1 : ����12���ֽڸ����������������ش�12���ֽ�
 unsigned char  GENT1[14]={13,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x02,0x01,0x00,0x08};

//Gen Templet2       ������ԭʼͼ������ָ������2 : ����12���ֽڸ����������������ش�12���ֽ�
 unsigned char  GENT2[14]={13,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x02,0x02,0x00,0x09}; 	
	
//Search Finger      ����CharBufferA��CharBufferB�е������ļ����������򲿷�ָ�ƿ�
//����17���ֽڸ�������������16���ֽڣ�ָ�Ƶ÷�û��ʲô���壬���Բ���ᣬֻ�����Ǽ���ָ�ƣ����Ǵ���
//ģ���ڲ�ʲôλ�ã���Ӧ����ָ�ƣ�1000��ָ�Ƶĵ�ַ�� 0---999�� 
//999��Ӧ����λʮ������Ϊ 0x03 0xe7,0x03�Ǹ��ֽڣ�Ҳ���ǳ���256
 unsigned char  SEAT[18]={17,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xe7,0x00,0xf8};

//Merge Templet      ;��CharBufferA��CharBufferB�е������ļ��ϲ�����ģ�壬�������ModelBuffer��
//��Ƭ������12���ֽڸ�ָ��ģ�飬ָ��ģ��ش�12���ֽ�
 unsigned char  MERG[13]={12,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x03,0x05,0x00,0x09};//����12��	

 
//��ȡ��ǰ��������������
 unsigned char  ReadFeature[14]={13,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x08,0x02,0x00,0x0F};
 
 //д�뵱ǰ��������������
 unsigned char  WriteFeature[14]={13,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x09,0x02,0x00,0x10};
 
//Store Templet      ����ModelBuffer2�е��ļ����浽flashָ�ƿ���
//��Ƭ������15���ֽڸ�ָ��ģ�飬ָ��ģ��ش�12���ֽ�
//���ֻ�ǻ�������������λ����Ҫ�ܸı�ģ���鿴�洢��������������ʱ�����������¸�ֵ
 unsigned char   STOR[16]={15,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x06,0x06,0x02,0x00,0x00,0x00,0x10};
 //DEL templet      ;ɾ��ָ��
unsigned char DelChar[17]={16,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x07,0x0c,0x00,0x00,0x00,0x01,0x00,0x0f};
 
 
//DEL templet      ;���ָ�ƿ�--
//��Ƭ������12���ֽڸ�ָ��ģ�飬ָ��ģ��ش�12���ֽ�
 unsigned char  DELE_all[13]={12,0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x03,0x0d,0x00,0x11};


 
void SetUART1_NVIC_ISENABLE(uint8_t isEnable)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    if(isEnable == 1)
    {
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    }
    else
    {
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//IRQͨ��ʹ��
    }
	
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}
 

unsigned char Command(unsigned char *p,unsigned char MaxTime) //�������,��ģ�鷢��һ������
{
  	unsigned char count=0,tmpdat=0,temp=0,i=0,package=0,flag=0;
	unsigned int  checksum=0;	
	unsigned char result=0, start=0,stop=0;

    SetUART1_NVIC_ISENABLE(0);    
    
  	i=*p;             //����ĵڡ�0����Ԫ�ء��������˱�����ĳ��ȣ���������ȸ�����i��������в���
	p++;

  	for (count=0; count<i; count++)//��Ҫ����ָ��ģ�������ͨ����Ƭ�����ڷ��͸�ģ��
    {
		temp=*p;    //ȡ����������ݣ���ָ���1��ָ����һ�������ֽڡ�
		sendUart1OneByte(temp);//��Ƭ�����ڽ����ݷ��͸�ָ��ģ��	
		p++;
	}
	  
  	result = 1;    //�������,���Ϊ�� (��Ϊ1)   	
  	FifoNumber=0;
  	for (count=0; count<MAX_NUMBER; count++)//�������FIFO[]������������ݣ�д��0X00
	{
	    FIFO[count]=0x00; 
	}
		  
  	if (1 == result)   
   	{		
     	result= 0;
      	start = 0;
   		stop  = 0;

       	count = 0;
       	clk0 = 0;	//����CL0����
		
       	do /////////////////////////////do������////////////////////////////////
		{	
			restart0:
					
	         	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//�����Ƭ������ ���յ����� 
				//��ͬ�ĵ�Ƭ������ARMоƬ����ͨ����ͬ�Ĳ�ѯ��ʽ������ѯ���ڽ����жϱ�־��
				//����ݲ�ͬ�ĵ�Ƭ���ͺŸ��������ѯ�����ѯ����Щ��Ƭ����Ҫ�ֶ�����
				//�������ݻ������⣬����ط�����һ��goto��䣬��������΢���������ˣ��������
				//���goto��������������ʲô���ѣ����������⣬���Լ�����Ϊ�ж�
				//����ÿ����Ƭ���жϴ�����Ʋ�һ�������ǲ��ṩ����֧�֣����Լ��о���
		      	{ 				
			  		tmpdat = USART_ReceiveData(USART1);          //�Ȱѽ��յ������ݷŵ�tmpdat��
					USART_ClearFlag(USART1,USART_FLAG_RXNE);	 //������ڽ����ж�

	            	if ((tmpdat==0xef)&&(start==0))//�������Ϊ��һ�������������ݣ�Ҳ���ǡ�ָ��Ӧ�𡱵ĵ�һ���ֽ�
	            	{ 
						count=0;
				    	FIFO[0] = tmpdat;//�����һ��Ӧ���ֽ�(0XEF)�����ڵڡ�0����Ԫ����    
						flag=1;	
						goto 
							restart0;//�������жϷ�ʽ����			
	             	}

					if(flag==1)//��һ���ֽ��Ѿ�����������flag==1����
					{  
						if(tmpdat!=0x01)  //�������ݴ��󣬽����´ӻ�������������
						{  	
							flag=0;//����Ӧ��ʧ��
							result=0;
	      					start =0;
	   						stop=0;
	       					count=0;
							goto 
								restart0;					
						}
						//����ɹ����յ�0xef  0x01�����Կ�ʼ��������
						flag = 2;//flag=2;��ʾӦ��ɹ������Կ�ʼ����������
						count++;//����count=1;
						FIFO[count] = tmpdat;//����ڶ���Ӧ���ֽڣ�0X01�������ڵڡ�1����Ԫ����    
						start = 1;	//Ӧ��ɹ����Կ�ʼ��������
						    goto 
								restart0;	
					}  
					                
	             	if((flag==2)&&(start==1))//flag=2;��ʾӦ��ɹ������Կ�ʼ����������
	             	{	   	  					 
				   		count++; //����Ԫ���±꣫��
			            FIFO[count] = tmpdat;//��������
						if(count>=6)
						{
							checksum = FIFO[count] + checksum; //����У���
						}
						if(count==8)
						{ 
							package=FIFO[7]*256+FIFO[8];	//���������							
							stop= 1;
						}
						if(1==stop)
						{						
							if(count==package+8)
							{
								checksum = (checksum - FIFO[count-1] - FIFO[count]);
								if(checksum != FIFO[count-1]*256 + FIFO[count] )
								{
								   result=0; //У��ʧ��,�ý����־Ϊ0
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
        }/////////////////////////////do������----------------����////////////////////////////////
		while ((clk0 <= MaxTime) && (count <MAX_NUMBER) && (changeflag==0)); //�ɶ�ʱ���Լ����������������ƣ���֤�����ڴ�һֱѭ��
        USART_ClearFlag(USART1,USART_FLAG_RXNE);	 //������ڽ����ж�/;
		FifoNumber = count;	//������յ������ݸ���
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
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//���������ж�
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


unsigned char VefPSW(void)//��֤�豸���ֿ���,�ɹ�����1     
{
 	unsigned char  count=0;
	while (1)
   	{
     	if(Command(VPWD,20) && (FifoNumber==11) && (FIFO[9]==0x00))  
		{
		  return(1);
		}	
     	count++;
   	  	if (count>=3)//������ɹ�������֤һ�Σ����3�β��ɹ�������ʧ��
        {  
            return(0);   
 	    }
	}
}


unsigned char Clear_All(void) //���ָ�ƿ�   
{				
      delay_ms(200);
	  if(Command(DELE_all,50)) //���ָ�ƿ�  		
      {
          return 1;
      }
      else
      {
          return 0;
      }
}

unsigned char ImgProcess(unsigned char BUFID)  //����ȡͼ�����������ļ�������BUFID��//�������Ϊ��������  
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


unsigned int Searchfinger(void)//����ָ��(������������Լ����ݷ���ֵȷ���Ƿ����)        
{		
   	if(Command(SEAT,60) && (FifoNumber==15) && (FIFO[9]==0x00) )  
    {
		SearchNumber = FIFO[10]*256 + FIFO[11];//��������ҳ��,�Ӷ��ж��Ǽ���ָ��
	   	return 1;
	}     
    else             
    {
       	return 0;
    }  
}


unsigned int search(void)//����ָ�� 
{
 	unsigned char SearchBuf=0;
//    unsigned char i=0;
//  	while (i < 3)  //iԽ��  ���ܶ���ͼ�����Խ�࣬�ٶ�Խ����ָ�ӦԽ��
//    {
        if (ImgProcess(1)==1)//���ȶ���һ��ָ��  
        {
            SearchBuf = Searchfinger();//����ָ�Ʊȶԣ������������������������ָ�����
            if(SearchBuf==1)
            {
                return SearchNumber; 
            }
            else 
            {
                return 0xFFFF;//��ʾ��������ָ�Ʋ���ȷ
            }     
        }
//        i++;
//    }
    return 0;
}

unsigned char savefingure(unsigned int ID)//����ָ��
{
	 unsigned char i=0;
	 unsigned int sum = 0;
	
	 //���ڿ�ʼ���д洢ָ��ģ��Ĳ���
     for (i=0;i<16;i++)	//����ָ����Ϣ
	 {
		FIFO[i]=STOR[i];
	 }
	  
	 FIFO[12]=ID/256;
     FIFO[13]=ID%256; //��ָ��ģ���ŵ�PAGE_IDҲ����FLASH��λ��

	 //���ڿ�ʼ����У��� ���ڵ�һ���ֽڣ������ַ0����ŵ����������ȣ�����Ҫע��λ��
     for (i=7;i<14;i++)	
	 {
	   sum= sum + FIFO[i];
	 }
	 FIFO[14]=sum/256;	//У���
     FIFO[15]=sum%256;	//У���

     if (Command(FIFO,70)==1 && FIFO[9]==0x00)//�˴����д��ָ��ģ�������
	 {
	   return(1);
	 }
     else
	 {
	   return(0);//���ɹ�����0
	 } 
}

unsigned char delfingure(unsigned int ID)//ɾ��ָ��
{
	 unsigned char i=0;
	 unsigned int sum = 0;
	
	 //���ڿ�ʼ���д洢ָ��ģ��Ĳ���
     for (i=0;i<17;i++)	//����ָ����Ϣ
	 {
		FIFO[i]=DelChar[i];
	 }
	  
	 FIFO[11]=ID/256;
     FIFO[12]=ID%256; //��ָ��ģ���ŵ�PAGE_IDҲ����FLASH��λ��

	 //���ڿ�ʼ����У��� ���ڵ�һ���ֽڣ������ַ0����ŵ����������ȣ�����Ҫע��λ��
     for (i=7;i<15;i++)	
	 {
	   sum= sum + FIFO[i];
	 }
	 FIFO[15]=sum/256;	//У���
     FIFO[16]=sum%256;	//У���

     if (Command(FIFO,80)==1 && FIFO[9]==0x00)//�˴����д��ָ��ģ�������
	 {
	   return(1);
	 }
     else
	 {
	   return(0);//���ɹ�����0
	 } 
}


unsigned char enroll(void) //�ɼ�����ָ�ƣ�����1�� ָ��ģ��
{
 	unsigned char temp=0,count=0;
  	while(1)
  	{
  	 	temp=ImgProcess(1); //��������1    
   		if (temp==1)//���������ļ��ɹ�             
      	{  		      
        	//�ɼ���һ�������ɹ� 
		   	count=0;  
		 
			delay_ms(200);
	 
			break;
       	}
     	else                   
       	{
        	if (temp==0)//�ɼ�ָ��û�гɹ�
          	{ 
				count++;
            	if (count>=40)//����ɼ���40�Σ������ɹ���ֱ�Ӳɼ�ʧ�ܣ�ֱ���˳�enroll����������������0  
					return(0);
            }
        }
   	}

	delay_ms(2000);//��ʱ2S��ʼ�ɼ���һ������

	//��ʼ�ɼ��ڶ������� 
 	while(1)
  	{
   		temp=ImgProcess(2); //��������2    
   		if (temp==1)//���������ļ�2�ɹ�
      	{
        	if( (Command(MERG,40)==1) && (FifoNumber==11) && (FIFO[9]==0x00)  ) //�ϲ��ɹ�����1
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
        	if (temp==1)//�ɼ�ָ��û�гɹ�
        	{
				count++;
				
				if (count>=25) 
				
				return(0);
       		}
     	}
   	}
}



 
unsigned int enteringFingerprint(SendUartIDNum sendUartAddNewAppointUserIDSub,SendUartIDNum sendUartAddNewUserIDSub)   /* Ϊ¼��ָ��ģʽ */
{
    uint16_t lastAppendNewUserID = 0;
    if(SaveNumber < 1000)                                                                /*  */
    {
        if(enroll() == 1)                                                           /* �ɼ����Σ�����1��ָ��ģ��ɹ� */
        {
            if(savefingure(SaveNumber ? SaveNumber : ++SaveNumber)== 1)      /* ����Ҳ�ɹ� */
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
                SaveNumber++;                                                   /* ��һ�� */
                ReadFingerFeatureData();
            }
        }
    }
    return lastAppendNewUserID;
}
 
void searchFingerprint(uint16_t* lastAppendNewUserID, SendUartIDNum sendUartUserIDSub)         /* Ϊʶ��ģʽ */
{
    searchnum = search();
    if((searchnum == 0) || (searchnum == 0xFFFF))     /* ʶ��ָ��ʧ�� */
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
