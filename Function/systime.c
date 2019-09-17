/**
  ******************************************************************************
  * @file    main.c
  * @author  Robotleo
  * @version V1.0.0
  * @date    11-August-2016
  * @brief   
  *
  ******************************************************************************
	*/
#include   "systime.h"
#include "AAA-include.h"
volatile  unsigned int  giv_sys_time;     //����Ϊ����ϵͳ��ϵͳʱ��
volatile  unsigned int  time_tmp[10];
unsigned int time_stemp;	//QZ ADD

bool app_bat_min;

volatile bool Sec;												//���־
volatile bool Sec_Two;		    						//����ı�־
bool Half_Min;											//30s��־
bool gbv_minute;   												//���ӱ�־�����ڶ�ʱ������صĵ�ѹ��
bool Five_Sec;
bool Ten_Sec;
bool Half_Sec;
bool Deci_Sec;		//100ms
bool MiliSec_50;	//50ms
vu16 CCR1_Val = 1200;//��ʱ���ļ������塣
bool time_speed=false;
bool gyro_check_time=false;

bool coordinate_show=false;


void Init_Time_1(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef		TIM_ICInitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;
	GPIO_InitTypeDef		GPIO_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=RM_HW;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);

#ifdef CPU_FREQ_36MHz
	TIM_TimeBaseStructure.TIM_Period = 5000; 	   //��Ƶ�޸ĵ�36MHz,36MHZ/(1200+1)*(2+1)=10KHz        
	TIM_TimeBaseStructure.TIM_Prescaler = 35;
	
#endif
#ifdef CPU_FREQ_64MHz
	TIM_TimeBaseStructure.TIM_Period = 5000;	   //5ms
	TIM_TimeBaseStructure.TIM_Prescaler = 63;
#endif
#ifdef CPU_FREQ_72MHz
	TIM_TimeBaseStructure.TIM_Period = 5000;	   //5ms
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
#endif
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	///////////���벶��/////////////////////////
  	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;  
  	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	
  	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  	TIM_ICInitStructure.TIM_ICFilter = 0x00;
  	TIM_ICInit(TIM1, &TIM_ICInitStructure);

	NVIC_InitStructure.NVIC_IRQChannel						= TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 1;	//1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

#if 0
	NVIC_InitStructure.NVIC_IRQChannel						= TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;	//1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	TIM_ClearFlag(TIM1,TIM_IT_CC2);

	TIM_ITConfig(TIM1,TIM_IT_CC2,ENABLE);
	TIM_Cmd(TIM1, ENABLE);
	
}


/*************************************************
��������void init_time_2(void)
���ܣ���ʼ����ʱ��2��ʹ����Ϊϵͳʱ�ӡ�
���룺�ޣ�
*************************************************/
void init_time_2(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//qz add
#ifdef CPU_FREQ_36MHz
	TIM_TimeBaseStructure.TIM_Period = 1199; 	   //��Ƶ�޸ĵ�36MHz,36MHZ/(1200+1)*(2+1)=10KHz        
	TIM_TimeBaseStructure.TIM_Prescaler = 2;
#endif
#ifdef CPU_FREQ_64MHz
	TIM_TimeBaseStructure.TIM_Period = 1599; 	//��Ƶ�޸ĵ�64MHz,64MHZ/(1599+1)*(3+1)=10KHz       
	TIM_TimeBaseStructure.TIM_Prescaler = 3;
#endif
#ifdef CPU_FREQ_72MHz
	TIM_TimeBaseStructure.TIM_Period = 1799; 	//��Ƶ�޸ĵ�64MHz,64MHZ/(1599+1)*(3+1)=10KHz       
	TIM_TimeBaseStructure.TIM_Prescaler = 3;
#endif
	//qz addd end
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM2, ENABLE);

	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_Update , ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel 										= TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/****************************************************
���ܣ���ʱ����
���룺������
�������	
****************************************************/
void DelayMs(unsigned int Ms)
{
unsigned int stime;  //��ʼʱ��
	stime = giv_sys_time;
	while (((giv_sys_time - stime) < Ms*10))
	{
	    IWDG_ReloadCounter();
	}
}
