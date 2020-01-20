
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================
#define NORMAL_LENGTH 		0x4D
#define FACTURE_LENGTH		0X41

//QZ ADD
u8 USART1_TX_BUF[USART1_TX_SIZE];
u8 USART1_RX_BUF[USART1_RX_SIZE];
u8 USART2_TX_BUF[USART2_TX_SIZE];
u8 USART2_RX_BUF[USART2_RX_SIZE];
u8 USART3_TX_BUF[USART3_TX_SIZE];
u8 USART3_RX_BUF[USART3_RX_SIZE];	//需存放小鸟数据


GYRO_DATA		Gyro_Data;
UART 			UART1;
UART 			UART2;
UART 			UART3;

short old_yaw=0;
char cnt_180=0;

void Init_Gyro_Data(void)
{
	Gyro_Data.first_rst=false;
	Gyro_Data.tick_check_step=0;
	Gyro_Data.tick_check_time=giv_sys_time;
	Gyro_Data.tick_flag=true;
	Gyro_Data.first_roll=Gyro_Data.roll;		//qz add 20180927
	Gyro_Data.count_times=0;
	Gyro_Data.g_ek[0]=0;
	Gyro_Data.g_ek[1]=0;
	Gyro_Data.g_ek[2]=0;
}

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
/***************************START*********************/
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	USART1->DR = (u8) ch;      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	return ch;
}
#endif 
/***************************END*********************/

void u3_printf(char* fmt,...)  
{  
	while(DMA1_Channel2->CNDTR!=0);	//等待通道2传输完成 
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	DMA_USART3_TX_Length(strlen((const char*)USART3_TX_BUF));
}

void u2_printf(char* fmt,...)  
{  
#ifdef   NEW_Q55_BOARD_1113   
			  while(DMA1_Channel4->CNDTR!=0); //等待通道2传输完成 
			  va_list ap;
			  va_start(ap,fmt);
			  vsprintf((char*)USART1_TX_BUF,fmt,ap);
			  va_end(ap);
			  DMA_USART1_TX_Length(strlen((const char*)USART1_TX_BUF));
  #else     
		while(DMA1_Channel7->CNDTR!=0); //等待通道2传输完成 
		va_list ap;
		va_start(ap,fmt);
		vsprintf((char*)USART2_TX_BUF,fmt,ap);
		va_end(ap);
		DMA_USART2_TX_Length(strlen((const char*)USART2_TX_BUF));
#endif
}
			
//LRC 异或校验
u8 LRC8( u8 *ptr,u32 length)
{
	u8 value=0;
	
	value = *ptr;
	ptr++;
	value ^= *ptr;
	length -= 2;
	while(length--)
			{
					ptr++;
					value ^= *ptr;
			}
	
	return value;
}



#ifdef  RTC_8M_CORR
void rtc_8m_Report(u8   reset_flag )
{
//				int temp_32;
	u16 temp_u16=0,count=0;
	u8 temp_u8=0;
	while(DMA_UART3_TX_Channel->CNDTR!=0);	
	short    temp_second = 0;

	/*-----------帧头---------*/
	USART3_TX_BUF[count++]=0xA5;	//帧头						0
	if (reset_flag	== 1)
	USART3_TX_BUF[count++]=0x08;	//Command					1
	else  
			USART3_TX_BUF[count++]=0x20;	//Command					1

	/*---------长度-----------*/
	USART3_TX_BUF[count++]=0x00;
	USART3_TX_BUF[count++]=NORMAL_LENGTH;	//data_cnt						3

	/*---------时间戳--------*/
	//	second_time = 0X2100;
	if ((second_time > 0x2000) &&(second_time <0x2E00))  //偏差太大 有异常  
	temp_second =  second_time - 0x2710 ;
	USART3_TX_BUF[count++] = (temp_second>>8);// 				4			
	USART3_TX_BUF[count++] = (temp_second);// 				5

	temp_u16=count-2;
	USART3_TX_BUF[2]=temp_u16>>8;
	USART3_TX_BUF[3]=temp_u16;

	/*------校验字节----------*/
	temp_u8=LRC8(&(USART3_TX_BUF[2]),temp_u16);
	USART3_TX_BUF[count++]=temp_u8; 						//6

	/*------帧尾--------------*/
	USART3_TX_BUF[count++]=0x5A; //7
	UART3.TxdDataNum = count;	

	DMA_USART3_TX_Length(UART3.TxdDataNum);
	//	UART3.Trans_Busy = true;
}

#endif 




#ifdef   TUYA_WIFI
#ifdef   NEW_Q55_BOARD_1113   
void Uart1_send(u8 *temp1 ,unsigned short count1)
{
   u8 i= 0;
   
   for (i=0;i<count1;i++)
   	    USART2_TX_BUF[i] = *temp1++;
   
   UART2.TxdDataNum = count1;	

   USART2_TX_DMA_Enable();
   UART2.Trans_Busy = true;
   while (UART2.Trans_Busy == 1) ;
}

#else
void Uart1_send(u8 *temp1 ,unsigned short count1)
{
   u8 i= 0;
   
   for (i=0;i<count1;i++)
   	    USART1_TX_BUF[i] = *temp1++;
   
   UART1.TxdDataNum = count1;	

   UART_TX_DMA_Enable();
   UART1.Trans_Busy = true;
   while (UART1.Trans_Busy == 1) ;
}
#endif

#endif

void Uart1_Comunication(void)
{
#if 1   // 串口一个字节就中断方式 
    wifi_uart_service();
#endif //tuya
}

void Uart3_Communication_II(void)
{
	int data1=0;	
	u16 cnt_checksum=0,rece_checksum=0,num=0;
	unsigned char COMBUF3[USART3_RX_SIZE];
//	short temp_x=0,temp_y=0;

	memcpy(COMBUF3,USART3_RX_BUF,USART3_RX_SIZE);
	
	DMA_Cmd(DMA1_Channel3,ENABLE);
#ifdef YIS055
	if((COMBUF3[0]==0x59)&&(COMBUF3[1]==0x49)&(COMBUF3[2]==0X53))
#else
	if((COMBUF3[0]==0XAA)&&(COMBUF3[1]==0XAA))
#endif

	{
#ifdef YIS055
		num=3;
#else
		num=2;
#endif

#ifdef YIS055
		while(num<USART3_RX_SIZE-2)
#else
		while(num<USART3_RX_SIZE-1)
#endif
		{
			cnt_checksum+=COMBUF3[num];
			num++;
		}
		
#ifdef YIS055
		rece_checksum=(COMBUF3[USART3_RX_SIZE-2]<<8)+COMBUF3[USART3_RX_SIZE-1];
#else
		rece_checksum=COMBUF3[USART3_RX_SIZE-1];
#endif

#ifdef YIS055
		if(cnt_checksum==rece_checksum)
#else
		if((cnt_checksum&0x00ff)==rece_checksum)
#endif
			{
			
#ifdef YIS055
				Gyro_Data.yaw=(COMBUF3[7]<<8)+COMBUF3[8];
#else
				Gyro_Data.yaw=(COMBUF3[4]<<8)+COMBUF3[3];
#endif

#ifdef GYRO_COMPENSATION
				long	yaw_temp ;
				yaw_temp = Gyro_Data.yaw -old_yaw;
				old_yaw = Gyro_Data.yaw;
			
				if (yaw_temp < -30000)	  // 正方向 超过180度时候,	old_yaw 为正 ,新的yaw为负   , 那么需要把新的变成为正的值
					cnt_180 -=1;
				if (yaw_temp >	30000)	  //反方向转超过 180度 
					cnt_180 +=1;
				if(cnt_180 <0) 
					{
						if (Gyro_Data.yaw< -18000) 
						Gyro_Data.yaw =	Gyro_Data.yaw + 36000;
					}
				if(cnt_180 >0) 
					{
						if (Gyro_Data.yaw>  18000)  
						Gyro_Data.yaw =	Gyro_Data.yaw - 36000;
					}	
				Gyro_Data.yaw=((int)(Gyro_Data.yaw))*(36000+289)/36000;
				//Gyro_Data.yaw=((int)(Gyro_Data.yaw))*1.007;
				if (Gyro_Data.yaw> 18000) 
				 Gyro_Data.yaw	=  Gyro_Data.yaw - 36000;
				if (Gyro_Data.yaw< -18000) 
				 Gyro_Data.yaw =  Gyro_Data.yaw + 36000;
				
#endif
//				Gyro_Data.yaw=((int)(Gyro_Data.yaw))*(36000-385)/36000;
				#if 1
				Gyro_Data.roll=(COMBUF3[6]<<8)+COMBUF3[5];
				Gyro_Data.pitch=(COMBUF3[8]<<8)+COMBUF3[7];
				#else
//				temp_x=(COMBUF3[6]<<8)+COMBUF3[5];
//				temp_y=(COMBUF3[8]<<8)+COMBUF3[7];
				//Gyro_Data.x_pos=(short)((float)(temp_x)*1.724138);
				//Gyro_Data.y_pos=(short)((float)(temp_y)*1.724138);
				#endif
				Gyro_Data.x_acc=(COMBUF3[10]<<8)+COMBUF3[9];
				Gyro_Data.y_acc=(COMBUF3[12]<<8)+COMBUF3[11];
				Gyro_Data.z_acc=(COMBUF3[14]<<8)+COMBUF3[13];
				Gyro_Data.tick_check_time=giv_sys_time;

				if(turn_stop_enable)
					{
						data1=abs(Gyro_Data.yaw-turn_stop_angle);
						if(data1>DEGREE_180)
							data1=DEGREE_360-data1;
						if(data1<TURN_ANGLE_BIOS)
							turn_stop_flag=true;
					}
			}
	}

	//memset(USART3_RX_BUF,0,USART3_RX_SIZE);
	memset(COMBUF3,0,USART3_RX_SIZE);
	//DMA_Cmd(DMA1_Channel3,ENABLE);
}

//============================================================================================
//初始化IO 串口1 
//bound:波特率
void URAT1_init(u32 bound)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	//mask
	
	UART1.Trans_Busy = false;	//初始化串口1发送忙标志位
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10
	USART_InitStructure.USART_BaudRate = bound;//一般设置为115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No	;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口


#ifdef   NEW_Q55_BOARD_1113   
    USART_Cmd(USART1, ENABLE);					  //使能串口 
    USART_ClearFlag(USART1, USART_FLAG_TC); 		//解决第一个字节无法发送问题	
#else//NEW_Q55_BOARD_1113   


#ifndef TUYA_WIFI
	USART_Cmd(USART1, ENABLE);					  //使能串口 
	USART_ClearFlag(USART1, USART_FLAG_TC); 		//解决第一个字节无法发送问题	
#else
	//mask
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//mask
#ifdef DMA_IRQ // DMA  + 串口中断方式 
	USART_Cmd(USART1,ENABLE);
	
	USART_ClearITPendingBit(USART1,USART_IT_IDLE);		//mask
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);		//mask
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);		//mask
	USART_ITConfig(USART1,USART_IT_TC,DISABLE); 		//mask
	USART_ClearFlag(USART1, USART_FLAG_TC); 		//解决第一个字节无法发送问题
#else // 普通中断模式 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART1,USART_FLAG_TC);
    USART_Cmd(USART1, ENABLE); 
#endif
#endif	
#endif	
}
//============================================================================================



//============================================================================================
/*DMA  UART1发送数据配置 */
//DataLength：缓存大小
void DMA_Uart1Tx_Config(u16 DataLength)
{
	DMA_InitTypeDef DMA_InitStructure;
//#ifndef TUYA_WIFI	
	NVIC_InitTypeDef NVIC_InitStructure;
//#endif
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输时钟

	/* UART Trans DMA配置*/	
  	DMA_DeInit(DMA_UART1_Tx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (u32)(&USART1->DR);  			//DMA外设地址  UART1发送寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr 			= (uint32_t)USART1_TX_BUF;  	//DMA内存地址  UART1发送缓冲区  //QZ:这里原来为USART1_RX_BUF
	DMA_InitStructure.DMA_DIR 								= DMA_DIR_PeripheralDST;  		//数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize 					= DataLength;  							//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;  	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc 					= DMA_MemoryInc_Enable;  		//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte; 		//数据宽度为8位
	DMA_InitStructure.DMA_Mode 								= DMA_Mode_Normal;  				//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority 						= DMA_Priority_Medium; 			//DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M 								= DMA_M2M_Disable;  				//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_UART1_Tx, &DMA_InitStructure);  				//根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
  	DMA_ITConfig(DMA_UART1_Tx, DMA_IT_TC, ENABLE);		  //使能发送完成中断
  	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);		  //使能USART1发送配置
 #ifdef NEW_Q55_BOARD_1113
          DMA_Cmd(DMA_UART1_Tx,ENABLE); 	 // 调试口 需要提前打开, 把datalength 置空   ,否则 调试发送就不能执行 
 #else
	DMA_Cmd(DMA_UART1_Tx,DISABLE);						  				//失效DMA发送
#endif

//#ifndef TUYA_WIFI

	NVIC_InitStructure.NVIC_IRQChannel	=	DMA1_Channel4_IRQn;		   				//DMA_UART1_Tx
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority					= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//#endif	
}
//============================================================================================





//============================================================================================
/*DMA  UART1接收数据配置 */
//DataLength：缓存大小
void DMA_Uart1Rx_Config(unsigned short DataLength)
{
	DMA_InitTypeDef DMA_InitStructure;

/* UART Rev DMA配置*/	
  	DMA_DeInit(DMA_UART1_Rx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (u32)(&USART1->DR);  			//DMA外设地址  UART1发送寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr 			= (u32)(USART1_RX_BUF);  		//DMA内存地址  UART1接收缓冲区
	DMA_InitStructure.DMA_DIR 								= DMA_DIR_PeripheralSRC;  		//数据传输方向，从外设读取发送到内存
	DMA_InitStructure.DMA_BufferSize 					= DataLength;  						//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;  	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc 					= DMA_MemoryInc_Enable;  		//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte; 		//数据宽度为8位
	DMA_InitStructure.DMA_Mode 								= DMA_Mode_Circular;  			//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority 						= DMA_Priority_Medium; 			//DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M 								= DMA_M2M_Disable;  			//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_UART1_Rx, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	
  	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);		  //使能USART1发送配置
	DMA_Cmd(DMA_UART1_Rx,ENABLE);
}
//============================================================================================ 



//============================================================================================
//开启一次DMA传输
void UART_TX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA_UART1_Tx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道 
 	DMA_SetCurrDataCounter(DMA1_Channel4,UART1.TxdDataNum);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_UART1_Tx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}	

#ifdef   NEW_Q55_BOARD_1113   
void DMA_USART1_TX_Length(u32 length)
{
	DMA_Cmd(DMA_UART1_Tx, DISABLE);		//关闭DMA,防止处理其间有数据
	DMA_UART1_Tx->CNDTR = length;			//重装填
	DMA_Cmd(DMA_UART1_Tx,ENABLE); 		//使能串口1的DMA发送  
}
#endif

//============================================================================================

void USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	//UART2.RevLength=128;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//使能GPIOA,G时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

#if 1
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
#endif

#if 0
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_Init(USART2,&USART_InitStructure);

#if 0
	USART_ClearITPendingBit(USART2,USART_IT_IDLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART2,USART_IT_TC,DISABLE);
#endif


#ifndef   NEW_Q55_BOARD_1113   
USART_Cmd(USART2,ENABLE);
USART_ClearFlag(USART2, USART_FLAG_TC); 		//解决第一个字节无法发送问题	
#else //NEW_Q55_BOARD_1113   
 
#ifdef TUYA_WIFI
	//mask
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//mask
#ifdef DMA_IRQ // DMA  + 串口中断方式 
	USART_Cmd(USART2,ENABLE);
	
	USART_ClearITPendingBit(USART2,USART_IT_IDLE);		//mask
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);		//mask
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);		//mask
	USART_ITConfig(USART2,USART_IT_TC,DISABLE); 		//mask
	USART_ClearFlag(USART2, USART_FLAG_TC); 		//解决第一个字节无法发送问题
#else // 普通中断模式 
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_Cmd(USART2, ENABLE); 
#endif
#endif	


#endif//NEW_Q55_BOARD_1113   


	
}

void USART2_DMA_TX_Init(u16 BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA_UART2_TX);	//USART2_TX DMA1 Channel7

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART2_TX_BUF;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize=BufferSize;//UART2.TxdDataNum;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA_UART2_TX,&DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	DMA_ClearFlag(DMA1_FLAG_TE7|DMA1_FLAG_GL7|DMA1_FLAG_HT7|DMA1_FLAG_TC7);
	DMA_ITConfig(DMA_UART2_TX,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA_UART2_TX,ENABLE);	
}


void USART2_DMA_RX_Init(u16 BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA_UART2_RX);	//USART2 RX DMA_Channel_6

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART2_RX_BUF;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize=BufferSize;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular; //QZ:DMA receive data in circular mode
	DMA_InitStructure.DMA_Priority=DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA_UART2_RX,&DMA_InitStructure);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	DMA_Cmd(DMA_UART2_RX,ENABLE);
}

void USART2_TX_DMA_Enable(void)
{
	DMA_Cmd(DMA_UART2_TX,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel7, UART2.TxdDataNum);
	DMA_Cmd(DMA_UART2_TX,ENABLE);
}

void DMA_USART2_TX_Length(u32 length)
{
	DMA_Cmd(DMA_UART2_TX, DISABLE);		//关闭DMA,防止处理其间有数据
	DMA_UART2_TX->CNDTR = length;			//重装填
	DMA_Cmd(DMA_UART2_TX,ENABLE); 		//使能串口1的DMA发送  
}

//============================================================================================
//USART3
void USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	//mask
	
//	UART3.RevLength=500;//452;	//QZ:LittelBird has 452 bytes in every packet

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能GPIOA,G时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);
#ifdef		RTC_8M_CORR 
	GPIO_InitStructure.GPIO_Pin=GYRO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
#endif

#if 0
	GPIO_InitStructure.GPIO_Pin=GYRO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
#endif

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_Init(USART3,&USART_InitStructure);
	
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);

	//mask
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//mask

	USART_Cmd(USART3,ENABLE);

	USART_ClearITPendingBit(USART3,USART_IT_IDLE);		//mask
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);		//mask
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);		//mask
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);			//mask
	USART_ClearFlag(USART3, USART_FLAG_TC);		    //解决第一个字节无法发送问题
}

void USART3_DMA_TX_Init(u16 BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA_UART3_TX_Channel);

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART3->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART3_TX_BUF;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize=BufferSize;//UART3.TxdDataNum;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA_UART3_TX_Channel,&DMA_InitStructure);
	
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
	DMA_ClearFlag(DMA1_FLAG_TE2|DMA1_FLAG_GL2|DMA1_FLAG_HT2|DMA1_FLAG_TC2);
	DMA_Cmd(DMA_UART3_TX_Channel,ENABLE);	
}

void USART3_DMA_RX_Init(u16 BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA_UART3_RX);

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART3->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART3_RX_BUF;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize=BufferSize;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;	//QZ:DMA receive data in circular mode
	DMA_InitStructure.DMA_Priority=DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable; 
	DMA_Init(DMA_UART3_RX,&DMA_InitStructure);

	DMA_Cmd(DMA_UART3_RX,ENABLE);
}

void DMA_USART3_TX_Length(u32 length)
{
	DMA_Cmd(DMA_UART3_TX_Channel, DISABLE);		//关闭DMA,防止处理其间有数据
	DMA_UART3_TX_Channel->CNDTR = length;			//重装填
	DMA_Cmd(DMA_UART3_TX_Channel,ENABLE); 		//使能串口1的DMA发送  
}


