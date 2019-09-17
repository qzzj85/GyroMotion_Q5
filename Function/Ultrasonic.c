
//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================

//******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���************
//=====================ȫ�ֱ���====================================
struct	ULTRASONIC_DATA 		Ultra;

//=====================ȫ�ֺ���====================================



//******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���************
//=====================˽�б���====================================uint8����
#define ULTRA_DATA_LENGTH			20
uint16_t SPI_Send_data[ULTRA_DATA_LENGTH];


//=====================˽�к���====================================
void DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void DMA_Enable(DMA_Channel_TypeDef*DMA_CHx);

u8 SPI3_ReadWriteByte(u16 TxData);
//===============================================================================================================
//===============================================================================================================



//=========================================================================================================================
//======����ʵ��======����ʵ��======����ʵ��======����ʵ��======����ʵ��======����ʵ��======����ʵ��======����ʵ��======
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

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );									//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI3,  ENABLE );									//SPI3ʱ��ʹ�� 	
 
	
//////	GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_5;					//MOSI
	GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//��ʼ��GPIOB


	SPI_InitStructure.SPI_Direction 				= SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode 							= SPI_Mode_Master;									//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize 					= SPI_DataSize_16b;									//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL 							= SPI_CPOL_High;										//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA 							= SPI_CPHA_2Edge;										//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS 							= SPI_NSS_Soft;											//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;					//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit 					= SPI_FirstBit_MSB;									//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial 		= 7;																//CRCֵ����Ķ���ʽ
	SPI_Init(SPI3, &SPI_InitStructure);  																				//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI3, ENABLE); 								//ʹ��SPI����

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
//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI3_ReadWriteByte(u16 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
			retry++;
			if(retry>200)
				return 0;
		}	
		
	SPI_I2S_SendData(SPI3, TxData); //ͨ������SPIx����һ������
		
	return 1;		
}
//=====================================================================================================








//=====================================================================================================
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI3_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI3->CR1&=0XFFC7;
	SPI3->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPI3,ENABLE); 

} 
//=====================================================================================================







//=====================================================================================================
//////////////////////////////////////////////////////////////////////////////////
DMA_InitTypeDef     DMA_InitStructure;
u16 								DMA1_MEM_LEN;					//����DMAÿ�����ݴ��͵ĳ��� 	    
//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ����� 
void DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//ʹ��DMA����
	
  DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ

	DMA1_MEM_LEN=cndtr;
	DMA_InitStructure.DMA_PeripheralBaseAddr 			= cpar;  												//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr 					= cmar;  												//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR 										= DMA_DIR_PeripheralDST;				//���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize 							= cndtr;  											//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc 					= DMA_PeripheralInc_Disable;  	//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc 							= DMA_MemoryInc_Enable;  				//�ڴ��ַ�Ĵ�������
	
//	DMA_InitStructure.DMA_PeripheralDataSize 			= DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
//	DMA_InitStructure.DMA_MemoryDataSize 					= DMA_MemoryDataSize_Byte; 			//���ݿ��Ϊ8λ
//	DMA_InitStructure.DMA_Mode 										= DMA_Mode_Normal; 							//����������ģʽ

	
	DMA_InitStructure.DMA_PeripheralDataSize 			= DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ	
	DMA_InitStructure.DMA_MemoryDataSize 					= DMA_MemoryDataSize_HalfWord; 			//���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode 										= DMA_Mode_Circular; 				
	DMA_InitStructure.DMA_Mode 										= DMA_Mode_Normal; 	

	DMA_InitStructure.DMA_Priority 								= DMA_Priority_Medium; 					//DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M	 										= DMA_M2M_Disable;  						//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);  																			//����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
	  	
} 
//=====================================================================================================



//=====================================================================================================
//����һ��DMA����
void DMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_CHx, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
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


















