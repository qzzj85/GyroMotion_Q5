
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

//******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义************
//=====================全局变量====================================
struct	ULTRASONIC_DATA 		Ultra;

//=====================全局函数====================================



//******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义************
//=====================私有变量====================================uint8——
#define ULTRA_DATA_LENGTH			20
uint16_t SPI_Send_data[ULTRA_DATA_LENGTH];


//=====================私有函数====================================
void DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void DMA_Enable(DMA_Channel_TypeDef*DMA_CHx);

u8 SPI3_ReadWriteByte(u16 TxData);
//===============================================================================================================
//===============================================================================================================



//=========================================================================================================================
//======函数实体======函数实体======函数实体======函数实体======函数实体======函数实体======函数实体======函数实体======
//=========================================================================================================================




struct GYRO_MODE Imu;	



//=====================================================================================================
//=====================================================================================================

//=====================================================================================================
//=====================================================================================================
void Ultrasonic(void)
{
//		if	(	
//							(mode.mode == DOCKING) 	|| (mode.mode == CHARGEING)
//					|| 	(mode.mode == SPOT)
//					|| 	(mode.mode == ERR) 			|| (mode.mode == SLEEP)
//				)
//		{return;}	


						if(Ultra.Repeat_time == true)		//20mS
								{	
										Ultra.Repeat_time = false;

										
//////										#define Ultra_Door_Near		0x0A00
//////										#define Ultra_Door				0x0A00
//////									
//////										#define Ultra_XiShu				0.023611*0.8
																		
//////									if( (Ultra. UD_Time[0] > 0x0C50) && (Ultra. UD_Time[0] < 0x0E00))
//////											{
//////												Ultra.Distance[0] = 50;
//////											}
//////									else if( (Ultra. UD_Time[1] > 0x0C50) && (Ultra. UD_Time[1] < 0x0F00))
//////											{
//////												Ultra.Distance[0] = 50;
//////											}
//////									else if( (Ultra. UD_Time[2] > 0x0C50) && (Ultra. UD_Time[2] < 0x1000))
//////											{
//////												Ultra.Distance[0] = 50;
//////											}
//////									else if( (Ultra. UD_Time[3] > 0x1B50) && (Ultra. UD_Time[3] < 0x2300))
//////											{
//////												Ultra.Distance[0] = 50;
//////											}
//////									else 
//////											{
//////												Ultra.Distance[0] = 0;
//////											}
//////									
									if( (Ultra. UD_Time[2] > 0x0050) && (Ultra. UD_Time[2] < 0x0100))
											{
							//					Ultra.Distance[0] = 50;
											}
							//		else if( (Ultra. UD_Time[3] > 0x0800) && (Ultra. UD_Time[3] < 0x0F00))
									else if( (Ultra. UD_Time[3] > 0x0500) && (Ultra. UD_Time[3] < 0x0800))
											{
//												Ultra.Distance[0] = 50;
											}
									else 
											{
												Ultra.Distance[0] = 0;
											}
									
											
										//	IO_PE0_1;
												for(Ultra. UD_Time_Counter=0;	Ultra. UD_Time_Counter<8;	Ultra. UD_Time_Counter++)
														{Ultra. UD_Time[Ultra. UD_Time_Counter] = 0;}
												Ultra. UD_Time_Counter = 0;
												Ultra.sign = 0;
														
										//		TIM6->ARR = 3500;
												TIM6->ARR = 3700;				//	LD001
												TIM6->CNT = 0;
												DMA_Enable(DMA2_Channel2);
										//	IO_PE0_0;	
								}

								
								
								
								
								
								
								if((Ultra.sign == 1) | (Ultra.Distance[0] == 50))
									{
										Ultra.Distance[0] = 50;	
						//				Enable_Beep_number(1);
									}
									else 
											{
												Ultra.Distance[0] = 0;
											}







											
Ultra.Distance[0] = 0;	








											
}
//=====================================================================================================
//=====================================================================================================









//=====================================================================================================
//=====================================================================================================
void SPI3_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );									//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI3,  ENABLE );									//SPI3时钟使能 	
 
	
//////	GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_5;					//MOSI
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化GPIOB


	SPI_InitStructure.SPI_Direction 				= SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode 							= SPI_Mode_Master;									//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize 					= SPI_DataSize_16b;									//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL 							= SPI_CPOL_High;										//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA 							= SPI_CPHA_2Edge;										//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS 							= SPI_NSS_Soft;											//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;					//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit 					= SPI_FirstBit_MSB;									//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial 		= 7;																//CRC值计算的多项式
	SPI_Init(SPI3, &SPI_InitStructure);  																				//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI3, ENABLE); 								//使能SPI外设

	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);


	SPI_Send_data[0] =  0x01FC;
	SPI_Send_data[1] =  0x07F0;
	SPI_Send_data[2] =  0x1FC0;
	SPI_Send_data[3] =  0x7F01;
	SPI_Send_data[4] =  0xFC07;
	SPI_Send_data[5] =  0xF01F;
	SPI_Send_data[6] =  0xC07F;
	
	SPI_Send_data[7] 	=  0x01FC;
	SPI_Send_data[8] 	=  0x07F0;
	SPI_Send_data[9] 	=  0x1FC0;
	SPI_Send_data[10] =  0x7F01;
	SPI_Send_data[11] =  0xFC07;
	SPI_Send_data[12] =  0xF000;
	
	
//		DMA_Config(DMA2_Channel2,	   	(u32)(&SPI3->DR),		(u32)(SPI_Send_data),			2);				//	LD001
//		DMA_Config(DMA2_Channel2,	   	(u32)(&SPI3->DR),		(u32)(SPI_Send_data),			4);
//		DMA_Config(DMA2_Channel2,	   	(u32)(&SPI3->DR),		(u32)(SPI_Send_data),			5);
		DMA_Config(DMA2_Channel2,	   	(u32)(&SPI3->DR),		(u32)(SPI_Send_data),			7);
//		DMA_Config(DMA2_Channel2,	   	(u32)(&SPI3->DR),		(u32)(SPI_Send_data),			10);

		DMA_Enable(DMA2_Channel2);
} 
//=====================================================================================================
//=====================================================================================================



//=====================================================================================================
//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI3_ReadWriteByte(u16 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
			retry++;
			if(retry>200)
				return 0;
		}	
		
	SPI_I2S_SendData(SPI3, TxData); //通过外设SPIx发送一个数据
		
	return 1;		
}
//=====================================================================================================








//=====================================================================================================
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
void SPI3_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI3->CR1&=0XFFC7;
	SPI3->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
	SPI_Cmd(SPI3,ENABLE); 

} 
//=====================================================================================================







//=====================================================================================================
//////////////////////////////////////////////////////////////////////////////////
DMA_InitTypeDef     DMA_InitStructure;
u16 								DMA1_MEM_LEN;					//保存DMA每次数据传送的长度 	    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量 
void DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//使能DMA传输
	
  DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值

	DMA1_MEM_LEN=cndtr;
	DMA_InitStructure.DMA_PeripheralBaseAddr 			= cpar;  												//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr 					= cmar;  												//DMA内存基地址
	DMA_InitStructure.DMA_DIR 										= DMA_DIR_PeripheralDST;				//数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize 							= cndtr;  											//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc 					= DMA_PeripheralInc_Disable;  	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc 							= DMA_MemoryInc_Enable;  				//内存地址寄存器递增
	
//	DMA_InitStructure.DMA_PeripheralDataSize 			= DMA_PeripheralDataSize_Byte;  //数据宽度为8位
//	DMA_InitStructure.DMA_MemoryDataSize 					= DMA_MemoryDataSize_Byte; 			//数据宽度为8位
//	DMA_InitStructure.DMA_Mode 										= DMA_Mode_Normal; 							//工作在正常模式

	
	DMA_InitStructure.DMA_PeripheralDataSize 			= DMA_PeripheralDataSize_HalfWord;  //数据宽度为16位	
	DMA_InitStructure.DMA_MemoryDataSize 					= DMA_MemoryDataSize_HalfWord; 			//数据宽度为16位
	DMA_InitStructure.DMA_Mode 										= DMA_Mode_Circular; 				
	DMA_InitStructure.DMA_Mode 										= DMA_Mode_Normal; 	

	DMA_InitStructure.DMA_Priority 								= DMA_Priority_Medium; 					//DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M	 										= DMA_M2M_Disable;  						//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);  																			//根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	  	
} 
//=====================================================================================================



//=====================================================================================================
//开启一次DMA传输
void DMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}	
//=====================================================================================================  

 




//72/1200 / 2/2 = 30K

void init_time_6(void)
{

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
//	TIM_TimeBaseStructure.TIM_Period 					= 65535;       //	9.1 mS    7.2 K 
	
	
	TIM_TimeBaseStructure.TIM_Period 					= 2000;
	TIM_TimeBaseStructure.TIM_Prescaler 			= 6;       
	TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;    
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM6, TIM_IT_Update );
	TIM_Cmd(TIM6, ENABLE);

	/* TIM IT enable */
	TIM_ITConfig(TIM6, TIM_IT_Update , ENABLE);
	

	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 14;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	


	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
 
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_Init(&NVIC_InitStructure);
	
	
	
	EXTI->FTSR 	|= 0x1000;
	EXTI->RTSR 	|= 0x1000; 
	EXTI->IMR 	|= 0x1000; 
	EXTI->EMR 	|= 0x1000; 
	
}



//void init_time_6(void)
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

//	NVIC_InitTypeDef NVIC_InitStructure;
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
//	
//	TIM_TimeBaseStructure.TIM_Period 					= 27600 + 6000; 
//	///TIM_TimeBaseStructure.TIM_Period 					= 18000+8000;          
//	TIM_TimeBaseStructure.TIM_Prescaler 			= 0;       
//	TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;    
//	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up; 
//	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

//	TIM_ClearITPendingBit(TIM6, TIM_IT_Update );
//	TIM_Cmd(TIM6, ENABLE);

//	/* TIM IT enable */
//	TIM_ITConfig(TIM6, TIM_IT_Update , ENABLE);

//	NVIC_InitStructure.NVIC_IRQChannel 										= TIM6_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 7;	
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;  
//	NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}






//void init_Ultrasonic_Input_Line(void)
//{
//NVIC_InitTypeDef NVIC_InitStructure;
//	
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
//  EXTI->FTSR |= 0xe;
//  EXTI->RTSR |= 0x1800;
//  
//  
//  NVIC_InitStructure.NVIC_IRQChannel 										= EXTI15_10_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 8;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

//}


















