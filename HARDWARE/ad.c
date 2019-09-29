#include  "ad.h"

//qz add
#define W_LM_ADPIN		GPIO_Pin_4	//PC4		ADC1_CHN_14		//左墙检检测
#define W_L_ADPIN		GPIO_Pin_7	//PA7		ADC1_CHN_7		//左中墙检检测
#define W_M_ADPIN		GPIO_Pin_5	//PC5		ADC1_CHN_15		//中墙检检测
#define W_RM_ADPIN		GPIO_Pin_0	//PB0		ADC1_CHN_8		//右中墙检检测
#define W_R_ADPIN		GPIO_Pin_1	//PB1		ADC1_CHN_9		//右墙检检测
#define E_L_ADPIN		GPIO_Pin_4	//PA4		ADC1_CHN_4		//左地检检测
#define E_M_ADPIN		GPIO_Pin_5	//PA5		ADC1_CHN_5		//中地检检测
#define E_R_ADPIN		GPIO_Pin_6	//PA6		ADC1_CHN_6		//右地检检测
#define	L_SB_ADPIN		GPIO_Pin_3	//PA3		ADC1_CHN_3		//左边刷电流检测
#define R_SB_ADPIN		GPIO_Pin_3	//PA3		ADC1_CHN_3		//右边刷电流检测
#define MB_ADPIN		GPIO_Pin_0	//PC0		ADC1_CHN_10		//中扫电流检测
#define L_RING_ADPIN 	GPIO_Pin_2	//PC2		ADC1_CHN_12		//左轮电流检测
#define R_RING_ADPIN	GPIO_Pin_3	//PC3		ADC1_CHN_13		//右轮电流检测
#define CHR_CUR_ADPIN	GPIO_Pin_2	//PA2		ADC1_CHN_2		//充电电流检测
#define BAT_VOL_ADPIN	GPIO_Pin_0	//PA0		ADC1_CHN_0		//电池电压检测
#define	BAT_TEMP_ADPIN	GPIO_Pin_1	//PA1		ADC1_CHN_1		//电池温度检测
#define FAN_CUR_ADPIN	GPIO_Pin_1	//PC1		ADC1_CHN_11		//风扇电流检测

u16  ADC_ConvertedValue[256];//ad转换结果存入的数组
bool ADC_ready;                //ad转换完成一次。
/************************************************************
功能：在复位时执行初始化AD转换。
************************************************************/
void init_ad(void)
{

	GPIO_InitTypeDef GPIO_InitStructure; //io结构
/* Enable peripheral clocks --------------------------------------------------*/
  /* Enable DMA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
                           | RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);

#ifdef CPU_FREQ_36MHz
  RCC->CFGR&=0XFFFF3FFF;
  RCC->CFGR|=0X00004000;	//QZ ADD,当主频为36MHz时, ADC预分频4，ADC最大频率不超过14MHz,ADC频率为9MHz
#endif

#ifdef CPU_FREQ_64MHz
  RCC->CFGR&=0XFFFF3FFF;
  RCC->CFGR|=0X0000C000;	//QZ ADD,当主频为64MHz时, ADC预分频8，ADC最大频率不超过14MHz,ADC频率为8MHz
#endif
/*初始化ad的16个io口*/
//PA0——PA7

 //PA0——PA7
  GPIO_InitStructure.GPIO_Pin = BAT_VOL_ADPIN|CHR_CUR_ADPIN | E_L_ADPIN
                              | E_M_ADPIN | E_R_ADPIN | W_L_ADPIN|BAT_TEMP_ADPIN|R_SB_ADPIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//PB0  PB1
  GPIO_InitStructure.GPIO_Pin = W_RM_ADPIN | W_R_ADPIN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 //PC0——PC5 
  GPIO_InitStructure.GPIO_Pin = MB_ADPIN| FAN_CUR_ADPIN|L_RING_ADPIN|R_RING_ADPIN|W_LM_ADPIN | W_M_ADPIN;

  GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  ReInitAd();
#ifdef DEBUG_INIT
	TRACE("AD init OK!\r\n");
#endif
}
/***************************************************
重新初始化AD的寄存器
***************************************************/
void ReInitAd(void)
{
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
 /*初始化DMA*/
  DMA_DeInit(DMA1_Channel1);	
  ADC_DeInit(ADC1);										  //ADC基地址       ADC_DR的偏移地址
  DMA_InitStructure.DMA_PeripheralBaseAddr 	=0x4001244c;	  //(0x40012400  +  0x4c )
  DMA_InitStructure.DMA_MemoryBaseAddr 			= (u32)ADC_ConvertedValue;
  DMA_InitStructure.DMA_DIR 								= DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize 					= 256;
  DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc 					= DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode 								= DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority 						= DMA_Priority_High;
  DMA_InitStructure.DMA_M2M 								= DMA_M2M_Disable;

  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_ClearFlag(DMA1_FLAG_TC1);
  DMA_ITConfig(DMA1_Channel1,DMA1_IT_TC1,ENABLE);
  
  /* Enable DMA channel1 */
  DMA_Cmd(DMA1_Channel1, DISABLE);

  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
 /*初始化AD*/ 
  ADC_InitStructure.ADC_Mode	 							= ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode 				= ENABLE;
	
  ADC_InitStructure.ADC_ContinuousConvMode 	= ENABLE;
	
  ADC_InitStructure.ADC_ExternalTrigConv 		= ADC_ExternalTrigConv_None;

  ADC_InitStructure.ADC_DataAlign 					= ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel 				= 16;
  ADC_Init(ADC1, &ADC_InitStructure);

////  /* ADC1 regular channel14 configuration */ 

	//ADC转换频率:当主频是64Mhz时,F_ADC=8MHz,当主频是36MHz时,F_ADC=9Mhz
	//16个通道转换时间是：16*(12.5+1.5)/8M=28us。

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 	1, 		ADC_SampleTime_1Cycles5);				//电池电压检测
	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 	2, 		ADC_SampleTime_1Cycles5);				//	电池温度
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 	2, 		ADC_SampleTime_1Cycles5);
	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 	3, 		ADC_SampleTime_1Cycles5);				//充电电流检测
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 	4, 		ADC_SampleTime_1Cycles5);				//边扫电流检测
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 	5, 		ADC_SampleTime_1Cycles5);				//左地检检测
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 	6, 		ADC_SampleTime_1Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 	6, 		ADC_SampleTime_1Cycles5);				//中地检检测
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 	7, 		ADC_SampleTime_1Cycles5);				//右地检检测
  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 	8, 		ADC_SampleTime_1Cycles5);				//左墙检检测			
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 	9, 		ADC_SampleTime_1Cycles5);				//右中墙检检测
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 	10, 	ADC_SampleTime_1Cycles5);				//右墙检检测
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 	11, 	ADC_SampleTime_1Cycles5);				//中扫电流检测
	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 	12, 	ADC_SampleTime_1Cycles5);//pc2			//风扇电流检测
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 	13, 	ADC_SampleTime_1Cycles5);//pc3			//左右轮电机电流采样
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 	12, 	ADC_SampleTime_1Cycles5);			
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 	13, 	ADC_SampleTime_1Cycles5);		
	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 	14, 	ADC_SampleTime_1Cycles5);				//左右轮电机电流采样
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 	15, 	ADC_SampleTime_1Cycles5);				//左中墙检检测
  ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 	16, 	ADC_SampleTime_1Cycles5);				//中墙检检测


//#define ADC_Channel_TempSensor                      ((uint8_t)ADC_Channel_16)
//#define ADC_Channel_Vrefint                         ((uint8_t)ADC_Channel_17)
	
	
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
 // ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);  

  DMA_Cmd(DMA1_Channel1,ENABLE);
  
  
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  ADC_ready = false;	  
}
//--------------------------------------------------------

 




/***************************************************
功能:计算单一通道的平均值
输入：通道值
输出：电压平均值
***************************************************/
u16 account_current (u8 channel)
{
 u16 l;
 u8  x;
 l = 0;
   for(x=0; x<16; x++)
   {
      l += ADC_ConvertedValue[channel + x*16] & 0XFFF;
   }
   l = l/16;
   return l;
   
}




/***************************************************
功能:计算单一通道的平均值
输入：通道值
输出：电压平均值
***************************************************/
uint16 Grand_Sensor_AD (uint8 channel)
{
 uint16 l;
 uint8  x;
 l = 0;
	
   for(x=0; x<16; x++)
		 {
				l += ADC_ConvertedValue[channel + x*16] & 0XFFF;
		 }
   l = l/16;
   return l;
   
}



/***************************************************
功能:计算单一通道的平均值
输入：通道值
输出：电压平均值
***************************************************/
uint16 Wall_account_current (u8 channel)
{
 u16 l;
 u8  x;
 l = 0;
   for(x=0; x<8; x++)
		 {
				l += ADC_ConvertedValue[channel + x*16] & 0XFFF;
		 }
   l = l/8;
   return l;
 
}




