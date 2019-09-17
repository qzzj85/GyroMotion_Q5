#include "AAA-Include.h"


bool RTC_SEND_FLAG=false;  //定义从UART1发送RTC时间标志
bool ADC_SEND_FLAG=false;
bool LB_FLAG=false;
u8 UART1_TX_BUF[100];


UPDATA UpData;


void RTC_Send(void)
{
	u32 t;
	u8 length=0;
	
	UpData.RTC_UpData.RTC_Min=0;
	if(RTC_SEND_FLAG)
		{
			UART1_TX_BUF[0]='$';
			UART1_TX_BUF[1]='R';
			UART1_TX_BUF[2]='O';
			UART1_TX_BUF[3]='B';
			UART1_TX_BUF[4]='O';
			UART1_TX_BUF[5]='T';
			t=RTC_GetCounter();
			if(t>= 604800)		 //一周=7*24*60*60=604800(s)
				t = t % 604800;
			t=t/7;

			UART1_TX_BUF[6]='T';  //功能码，0x01为RTC时间发送
			UART1_TX_BUF[8]=t%3600;
			t=t/3600;
			UART1_TX_BUF[9]=t%60;
			t=t/60;
			UART1_TX_BUF[10]=t;

			UART1_TX_BUF[11]=LRC8((unsigned char*)&UART1_TX_BUF[7],3);

			UART1_TX_BUF[7]=3;	//发送数据长度

			UART1_TX_BUF[12]='#';
			UART1_TX_BUF[13]='#';

//			UART1.TxdDataNum=14;

//			UART_TX_DMA_Enable();
//			UART1.Trans_Busy = true;
			length=14;
			for(int i=0;i<length;i++)
				USART1_Putc(UART1_TX_BUF[i]);

			RTC_SEND_FLAG=false;
		}
}

void RTC_DMA_Send(void)
{
	u32 t;
	if(RTC_SEND_FLAG&&(UART1.Trans_Busy==false))
	{
		UART1.ComTxBuf[0]='$';
		UART1.ComTxBuf[1]='R';
		UART1.ComTxBuf[2]='O';
		UART1.ComTxBuf[3]='B';
		UART1.ComTxBuf[4]='O';
		UART1.ComTxBuf[5]='T';
		t=RTC_GetCounter();
		if(t>= 86400)		 //一天，24*3600=86400
			t = t % 86400;
		//t=t/7;

		//printf("t=%d\r\n",t);
		
		UART1.ComTxBuf[6]='T';  //功能码，0x01为RTC时间发送
		UART1.ComTxBuf[8]=t/3600;	//hour
		t=t%3600;
		UART1.ComTxBuf[9]=t/60;		//min
		t=t%60;
		UART1.ComTxBuf[10]=t;		//sec

		UART1.ComTxBuf[11]=LRC8((unsigned char*)&UART1.ComTxBuf[7],3);

		UART1.ComTxBuf[7]=3;	//发送数据长度

		UART1.ComTxBuf[12]='#';
		UART1.ComTxBuf[13]='#';

		UART1.TxdDataNum=14;

		UART_TX_DMA_Enable();
		UART1.Trans_Busy = true;

		RTC_SEND_FLAG=false;
	}	
}

void ADC_Send(void)
{
	u16 temp[6]; 
	if(ADC_SEND_FLAG)
		{
			UART1_TX_BUF[0]='$';
			UART1_TX_BUF[1]='R';
			UART1_TX_BUF[2]='O';
			UART1_TX_BUF[3]='B';
			UART1_TX_BUF[4]='O';
			UART1_TX_BUF[5]='T';

			temp[0]=Wall_account_current(W_L);
			temp[1]=Wall_account_current(W_LM);
			temp[2]=Wall_account_current(W_M);
			temp[3]=Wall_account_current(W_RM);
			temp[4]=Wall_account_current(W_R);
			temp[5]=account_current(BATTERY_VOLTAGE);

//			for(int i=7,k=0;i<17;)
//				{
//					UART1_TX_BUF[i++]=temp[k]>>8;
//					UART1_TX_BUF[i++]=temp[k++]0x00ff;					
//				}

			UART1_TX_BUF[6]='A';		//功能码，0x02为ADC数据发送
			UART1_TX_BUF[8]=temp[0]>>8;			//左墙检
			UART1_TX_BUF[9]=temp[0]&0x00ff;		//左墙检
			UART1_TX_BUF[10]=temp[1]>>8;		//左中墙检
			UART1_TX_BUF[11]=temp[1]&0x00ff;	//左中墙检
			UART1_TX_BUF[12]=temp[2]>>8;		//中墙检
			UART1_TX_BUF[13]=temp[2]&0x00ff;	//中墙检
			UART1_TX_BUF[14]=temp[3]>>8;		//右中墙检
			UART1_TX_BUF[15]=temp[3]&0x00ff;	//右中墙检
			UART1_TX_BUF[16]=temp[4]>>8;		//右墙检
			UART1_TX_BUF[17]=temp[4]&0x00ff;	//右墙检
			UART1_TX_BUF[18]=temp[5]>>8;		//电池电压
			UART1_TX_BUF[19]=temp[5]&0x00ff;	//电池电压
			
			UART1_TX_BUF[20]=LRC8((unsigned char*)UART1_TX_BUF[7],10);
			UART1_TX_BUF[21]='#';
			UART1_TX_BUF[22]='#';
					
			UART1_TX_BUF[7]=12;	//发送数据长度
//			UART1.TxdDataNum=21;

//			UART_TX_DMA_Enable();
//			UART1.Trans_Busy = true;
			int length=23;
			for(int i=0;i<length;i++)
				USART1_Putc(UART1_TX_BUF[i]);

			ADC_SEND_FLAG=false;
			
		}
}

void ADC_DMA_Send(void)
{
		u16 temp[6];
		if(ADC_SEND_FLAG&&(UART1.Trans_Busy==false))
			{
				UART1.ComTxBuf[0]='$';
				UART1.ComTxBuf[1]='R';
				UART1.ComTxBuf[2]='O';
				UART1.ComTxBuf[3]='B';
				UART1.ComTxBuf[4]='O';
				UART1.ComTxBuf[5]='T';
	
				temp[0]=Wall_account_current(W_L);
				temp[1]=Wall_account_current(W_LM);
				temp[2]=Wall_account_current(W_M);
				temp[3]=Wall_account_current(W_RM);
				temp[4]=Wall_account_current(W_R);
				temp[5]=account_current(BATTERY_VOLTAGE);
				
	
	//			for(int i=7,k=0;i<17;)
	//				{
	//					UART1.ComTxBuf[i++]=temp[k]>>8;
	//					UART1.ComTxBuf[i++]=temp[k++]0x00ff;					
	//				}
	
				UART1.ComTxBuf[6]='A';		//功能码，0x02为ADC数据发送
				UART1.ComTxBuf[8]=temp[0]>>8;
				UART1.ComTxBuf[9]=temp[0]&0x00ff;
				UART1.ComTxBuf[10]=temp[1]>>8;
				UART1.ComTxBuf[11]=temp[1]&0x00ff;
				UART1.ComTxBuf[12]=temp[2]>>8;
				UART1.ComTxBuf[13]=temp[2]&0x00ff;
				UART1.ComTxBuf[14]=temp[3]>>8;
				UART1.ComTxBuf[15]=temp[3]&0x00ff;
				UART1.ComTxBuf[16]=temp[4]>>8;
				UART1.ComTxBuf[17]=temp[4]&0x00ff;
				UART1.ComTxBuf[18]=temp[5]>>8;
				UART1.ComTxBuf[19]=temp[5]&0x00ff;

				
				UART1.ComTxBuf[20]=LRC8((unsigned char*)UART1_TX_BUF[7],10);
				UART1.ComTxBuf[21]='#';
				UART1.ComTxBuf[22]='#';
						
				UART1.ComTxBuf[7]=12; //发送数据长度
				UART1.TxdDataNum=23;
	
				UART_TX_DMA_Enable();
				UART1.Trans_Busy = true;
	
				ADC_SEND_FLAG=false;
				
			}
}


//QZ:LittleBird Control Message Send
void LB_Ctrl(void)
{
	u8 temp;
	if((LB_FLAG)&&(UART3.Rece_Busy==false))
	{		
//		USART3_Putc(0xA5);
//		USART3_Putc(0X07);
//		USART3_Putc(0X01);
//		USART3_Putc(0X36);
//		USART3_Putc(0X00);
//		USART3_Putc(0X00);
//		USART3_Putc(0XE3);
		LB_FLAG=false;
//		#ifdef USART3_DMA		
//		DMA_ClearITPendingBit(DMA1_IT_GL3);	//This code is aviod received wrong data in first data packet when power reset
//		DMA_Cmd(DMA1_Channel3,ENABLE);
//		#else
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE);//This code is aviod received wrong data in first data packet when power reset
//		USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
//		#endif
	}
}


u16 Find_LBFrame(void)
{
	for(u16 i=0;i<452;i++)
		{

			if((UART3.ComRxBuf[i]==0xA5)&&(UART3.ComRxBuf[i+1]==0x01))
				{
					if(i>=448)
						i=500;
					return(i);
				}
		}
}

u16 Check_LBData(void)
{
	u16 result=0;
	u16 i;
	i=Find_LBFrame();
	if(i==500)
		return 0;
	else
		{
			i+=2;
			for(i;i<450;i++)
				result+=UART3.ComRxBuf[i];
			return result;
		}
		
	
}

void debug_report(void)
{
	
	printf("w_l.difference:%d\r\n",w_l.difference);
	printf("w_l.sign:");
	if(w_l.sign==0xFD)
		printf("FRAN\r\n");
	else
		printf("NEAR\r\n");
	
	printf("w_lm.difference:%d\r\n",w_lm.difference);
	printf("w_lm.sign:");
	if(w_lm.sign==0xFD)
		printf("FRAN\r\n");
	else
		printf("NEAR\r\n");
	
	printf("w_m.difference:%d\r\n",w_m.difference);
	printf("w_m.sign:");
	if(w_m.sign==0xFD)
		printf("FRAN\r\n");
	else
		printf("NEAR\r\n");
	
	printf("w_rm.difference:%d\r\n",w_rm.difference);
	printf("w_rm.sign:");
	if(w_rm.sign==0xFD)
		printf("FRAN\r\n");
	else
		printf("NEAR\r\n");
	
	printf("w_r.difference:%d\r\n",w_r.difference);
	printf("w_r.sign:");
	if(w_r.sign==0xFD)
		printf("FRAN\r\n");
	else
		printf("NEAR\r\n");
}

