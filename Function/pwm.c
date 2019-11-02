#include "AAA-include.h"
/**************************************************************
�������ƣ�void  Init_PWM(void)
��������ʼ��pwm�������򣬽���ʱ��3����ʱ��4 ����Ϊpwm1ģʽ��ʹ��
      ���ض���ģʽ���ϼ���
���룺��
�������
**************************************************************/
void Init_Ring_Pwm(u16 period,u16 prescaler)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);	
	TIM_Cmd(TIM4, DISABLE);
	//===================================================================================
///////////////////////���������ĸ������ֵĿ����ź�//////////////
	
#ifdef CPU_FREQ_36MHz//��ƵΪ36MHZʱ��PWMƵ��Ϊ18KHZ��
	TIM_TimeBaseStructure.TIM_Period				= period-1;   //PWM��װֵ ������ARR����   //1199	
	TIM_TimeBaseStructure.TIM_Prescaler 			= prescaler;  //ʱ�ӷ�Ƶϵ�� ,����PSC����
#else
#ifdef CPU_FREQ_64MHz//��ƵΪ64MHZʱ��PWMƵ��Ϊ16KHZ
	TIM_TimeBaseStructure.TIM_Period				= period-1;   //PWM��װֵ ������ARR����   //1199	
	TIM_TimeBaseStructure.TIM_Prescaler 			= prescaler;  //ʱ�ӷ�Ƶϵ�� ,����PSC����
#endif
#endif
	TIM_TimeBaseStructure.TIM_ClockDivision 		= 0;   //ʱ�ӷ�Ƶ���ӣ���CR1�Ĵ�����
	TIM_TimeBaseStructure.TIM_CounterMode			= TIM_CounterMode_Up; //������ʽ����CR1�Ĵ����� 
  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	//QZ: ��ʱ��4��36MHz, ��ƵΪ1+1������Ϊ999+1���õ�Ƶ��Ϊ36MHz/(1+1)/(999+1)=18KHz
	
  /* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		 //ͨ���Ĺ���ģʽ  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;						 //������Ч���
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //��Ч��ƽ
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);	//qz add
  //TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);	//qz mask
  
  /* PWM1 Mode configuration: Channel2 */
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); 
  
//	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);	//qz mask
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);	//qz add

  /* PWM1 Mode configuration: Channel3 */
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  
//	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);	//qz mask
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Disable);	//qz add
  
  /* PWM1 Mode configuration: Channel4 */
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Disable);	//qz add
//	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);	//qz mask



	TIM_ARRPreloadConfig(TIM4, ENABLE);
  /* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DR_RIN | DR_FIN |DL_FIN | DL_RIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#ifdef DEBUG_INIT
	TRACE("Ring PWM init OK!\r\n");
#endif
}

void Init_Sweep_Pwm(u16 period,u16 prescaler)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO , ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_Cmd(TIM3, DISABLE);

	////////////////���涨�嶨ʱ��3�����������ɨƵ��36kHz///////////////	hzh 20171218 TIM3  CH4
	///////////////20180320 qz add�����ݷ��datasheet�����Ƶ������Ϊ25KHz//////////
	  /* Time base configuration */
#ifdef CPU_FREQ_36MHz//��ƵΪ36MHZʱ��PWMƵ��Ϊ36KHZ
	TIM_TimeBaseStructure.TIM_Period					= period-1;  //PWM��װֵ ������ARR����
	TIM_TimeBaseStructure.TIM_Prescaler 				= prescaler;	 //ʱ�ӷ�Ƶϵ�� ,����PSC����
#endif
#ifdef CPU_FREQ_64MHz//???64MHZ?,PWM???32KHZ
	TIM_TimeBaseStructure.TIM_Period					= period-1; 		//PWM??? ,??ARR??
	TIM_TimeBaseStructure.TIM_Prescaler 				= prescaler;		//?????? ,??PSC??
#endif
#ifdef CPU_FREQ_72MHz//???64MHZ?,PWM???36KHZ
	TIM_TimeBaseStructure.TIM_Period					= period-1; 		//PWM??? ,??ARR??
	TIM_TimeBaseStructure.TIM_Prescaler 				= prescaler;		//?????? ,??PSC??
#endif
	TIM_TimeBaseStructure.TIM_ClockDivision 			= 0;						//??????,?CR1????
	TIM_TimeBaseStructure.TIM_CounterMode				= TIM_CounterMode_Up;		//????,?CR1???? 
  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
   /* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode						= TIM_OCMode_PWM1;			//ͨ���Ĺ���ģʽ  
	TIM_OCInitStructure.TIM_OutputState 				= TIM_OutputState_Enable;				//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse						= 0;						//������Ч���
	TIM_OCInitStructure.TIM_OCPolarity					= TIM_OCPolarity_High;			//��Ч��ƽ
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);						//���PWM����
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);

	TIM_OC2Init(TIM3,&TIM_OCInitStructure); 						//FAN PWM����
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Disable);

	TIM_OC3Init(TIM3,&TIM_OCInitStructure); 						//��ɨPWM����
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Disable);

	TIM_OC4Init(TIM3,&TIM_OCInitStructure); 						//��ɨPWM����
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
  
	TIM_ARRPreloadConfig(TIM3, ENABLE);
  
	TIM_SetCompare1(TIM3,0);											//�رճ��
	TIM_SetCompare2(TIM3,0);											//�ر�FAN
	TIM_SetCompare3(TIM3,0);											//�رձ�ɨ
	TIM_SetCompare4(TIM3,0);											//�ر���ɨ
	
  /* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
	GPIO_InitStructure.GPIO_Pin =	SIDEBRUSH_PWM|CHG_PWM|FAN_PWM|MIDBRUSH_PWM;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);		 
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
#ifdef DEBUG_INIT
		TRACE("Sweep PWM init OK!\r\n");
#endif
}

void  Init_PWM(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC 
											 | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO , ENABLE);
   /////////////////////ʹ�ܶ�ʱ��3��4��ʱ��////////////
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);

	#if 1
	////////////////���涨�嶨ʱ��3�����������ɨƵ��36kHz///////////////	hzh 20171218 TIM3  CH4
	///////////////20180320 qz add�����ݷ��datasheet�����Ƶ������Ϊ25KHz//////////
	  /* Time base configuration */
	#ifdef CPU_FREQ_36MHz//��ƵΪ36MHZʱ��PWMƵ��Ϊ36KHZ
	TIM_TimeBaseStructure.TIM_Period					= PWM_CHARGE_MAX-1;	 //PWM��װֵ ������ARR����
	TIM_TimeBaseStructure.TIM_Prescaler 			= PWM_SWEEP_PRESCALER;   //ʱ�ӷ�Ƶϵ�� ,����PSC����
#endif
#ifdef CPU_FREQ_64MHz//???64MHZ?,PWM???32KHZ
	TIM_TimeBaseStructure.TIM_Period					= PWM_CHARGE_MAX-1;	 		//PWM??? ,??ARR??
	TIM_TimeBaseStructure.TIM_Prescaler 				= PWM_SWEEP_PRESCALER;   	//?????? ,??PSC??
#endif
#ifdef CPU_FREQ_72MHz//???64MHZ?,PWM???36KHZ
	TIM_TimeBaseStructure.TIM_Period					= PWM_CHARGE_MAX-1;  		//PWM??? ,??ARR??
	TIM_TimeBaseStructure.TIM_Prescaler 				= PWM_SWEEP_PRESCALER;	 	//?????? ,??PSC??
#endif
  	TIM_TimeBaseStructure.TIM_ClockDivision 			= 0;   						//??????,?CR1????
  	TIM_TimeBaseStructure.TIM_CounterMode 				= TIM_CounterMode_Up; 		//????,?CR1???? 
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
   /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode 						= TIM_OCMode_PWM1;        	//ͨ���Ĺ���ģʽ  
	TIM_OCInitStructure.TIM_OutputState 			= TIM_OutputState_Enable; 				//�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_Pulse 						= 0;                       	//������Ч���
  TIM_OCInitStructure.TIM_OCPolarity 				= TIM_OCPolarity_High; 			//��Ч��ƽ
    
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);							//���PWM����
  TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);

  TIM_OC2Init(TIM3,&TIM_OCInitStructure);							//FAN PWM����
  TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Disable);

  TIM_OC3Init(TIM3,&TIM_OCInitStructure);							//��ɨPWM����
  TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Disable);

  TIM_OC4Init(TIM3,&TIM_OCInitStructure);							//��ɨPWM����
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  
  TIM_SetCompare1(TIM3,0);											//�رճ��
  TIM_SetCompare2(TIM3,0);											//�ر�FAN
  TIM_SetCompare3(TIM3,0);											//�رձ�ɨ
  TIM_SetCompare4(TIM3,0);											//�ر���ɨ
  	
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
	GPIO_InitStructure.GPIO_Pin =   SIDEBRUSH_PWM|MIDBRUSH_PWM|CHG_PWM|FAN_PWM;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
		 
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	#endif

	
	//===================================================================================
///////////////////////���������ĸ������ֵĿ����ź�//////////////
	
  #ifdef CPU_FREQ_36MHz//��ƵΪ36MHZʱ��PWMƵ��Ϊ18KHZ��
  TIM_TimeBaseStructure.TIM_Period					  = PWM_RING_MAX-1;   //PWM��װֵ ������ARR����   //1199	
  TIM_TimeBaseStructure.TIM_Prescaler			  = PWM_RING_PRESCALER;	 //ʱ�ӷ�Ƶϵ�� ,����PSC����
  #else
  #ifdef CPU_FREQ_64MHz//��ƵΪ64MHZʱ��PWMƵ��Ϊ16KHZ
  TIM_TimeBaseStructure.TIM_Period					  = PWM_RING_MAX-1;   //PWM��װֵ ������ARR����   //1199	
  TIM_TimeBaseStructure.TIM_Prescaler			  = PWM_RING_PRESCALER;	 //ʱ�ӷ�Ƶϵ�� ,����PSC����
  #endif
  #endif
  TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;   //ʱ�ӷ�Ƶ���ӣ���CR1�Ĵ�����
  TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up; //������ʽ����CR1�Ĵ����� 
  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	//QZ: ��ʱ��4��36MHz, ��ƵΪ1+1������Ϊ999+1���õ�Ƶ��Ϊ36MHz/(1+1)/(999+1)=18KHz
	
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //ͨ���Ĺ���ģʽ  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_Pulse = 0;                       //������Ч���
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //��Ч��ƽ
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);	//qz add
  //TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);	//qz mask
  
  /* PWM1 Mode configuration: Channel2 */
  TIM_OC2Init(TIM4, &TIM_OCInitStructure); 
  
//  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);	//qz mask
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);	//qz add

  /* PWM1 Mode configuration: Channel3 */
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  
//  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);	//qz mask
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Disable);	//qz add
  
  /* PWM1 Mode configuration: Channel4 */
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Disable);	//qz add
//  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);	//qz mask



  TIM_ARRPreloadConfig(TIM4, ENABLE);
  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);
	GPIO_InitStructure.GPIO_Pin = DR_RIN | DR_FIN |DL_FIN | DL_RIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#ifdef DEBUG_INIT
	TRACE("PWM init OK!\r\n");
#endif
}
/*********************************************
���ܣ�ʹ��PWM
���룺channel ͨ������
              ��L_FRONT     ������ǰ
			  ��L_BACK      �������
			  ��R_FRONT     ������ǰ
			  ��R_BACK      �������
			  ��CHARGE_PWM  ���pwm
	  pwm     ռ�ձȣ���ֵ��0����1000֮��
�������
*********************************************/
void  enable_pwm(u16 channel,u16  pwm)
{
	switch (channel)
	{
		case	L_FRONT:
			 if(pwm>PWM_RING_MAX)
			 	pwm=PWM_RING_MAX;
			 TIM_SetCompare4(TIM4,pwm);
             TIM_SetCompare3(TIM4,0);
             //TIM_SetCompare4(TIM4,pwm);
             //TIM_SetCompare3(TIM4,0);
		
			break;
		case	L_BACK:
			if(pwm>PWM_RING_MAX)
			   pwm=PWM_RING_MAX;
			TIM_SetCompare3(TIM4,pwm);
            TIM_SetCompare4(TIM4,0);
            //TIM_SetCompare3(TIM4,pwm);
            //TIM_SetCompare4(TIM4,0);
		
			break;
		case	R_BACK:
			if(pwm>PWM_RING_MAX)
			   pwm=PWM_RING_MAX;
			TIM_SetCompare1(TIM4,pwm);
            TIM_SetCompare2(TIM4,0);
            //TIM_SetCompare1(TIM4,pwm);
            //TIM_SetCompare2(TIM4,0);
		
			break;
		case	R_FRONT:
			if(pwm>PWM_RING_MAX)
			   pwm=PWM_RING_MAX;
			TIM_SetCompare2(TIM4,pwm);
            TIM_SetCompare1(TIM4,0);
            //TIM_SetCompare2(TIM4,pwm);
            //TIM_SetCompare1(TIM4,0);
		
			break;

		//qz add 20180515
		case 	CHARGE_PWM:
			if(pwm>PWM_RING_MAX)
			   pwm=PWM_RING_MAX;
			TIM_SetCompare1(TIM3,pwm);
			break;
			
	}	 
}
/*********************************************
���ܣ��ر�PWM
���룺channel ͨ������
              ��L_FRONT     ������ǰ
			  ��L_BACK      �������
			  ��R_FRONT     ������ǰ
			  ��R_BACK      �������
�������
	  
*********************************************/
void  disable_pwm(u16 channel)
{
	switch (channel)
	{

case	L_FRONT:
	TIM_SetCompare3(TIM4,0);
	//TIM_SetCompare4(TIM4,0);
	break;
case	L_BACK:
	TIM_SetCompare4(TIM4,0);
	//TIM_SetCompare3(TIM4,0);
	break;
case	R_BACK:
	TIM_SetCompare2(TIM4,0);
	//TIM_SetCompare1(TIM4,0);
	break;
case	R_FRONT:
	TIM_SetCompare1(TIM4,0);
	//TIM_SetCompare2(TIM4,0);
	break;

//qz add 20180515
case	CHARGE_PWM:
	TIM_SetCompare1(TIM3,0);
	break;

	
		
	}
}
/*********************************************
���ܣ�ʹ��PWM
���룺channel			ͨ������
		��L_FRONT     ������ǰ
		��L_BACK      �������
		��R_FRONT     ������ǰ
		��R_BACK      �������
		��CHARGE_PWM  ���pwm
		pwm						ռ�ձȣ���ֵ��0����1000֮��	
�������
**********************************************/
void  Set_Pwm(u16 channel,u16  pwm)
{
	switch (channel)
	{

case	L_FRONT:
	TIM_SetCompare4(TIM4,pwm);
	break;
case	L_BACK:
	TIM_SetCompare3(TIM4,pwm);
	break;
case	R_BACK:
	TIM_SetCompare1(TIM4,pwm);
	break;
case	R_FRONT:
	TIM_SetCompare2(TIM4,pwm);
	break;

//qz add 20180515
case 	CHARGE_PWM:
	TIM_SetCompare1(TIM3,pwm);
	break;
	
	}	 
}







void  Break_pwm(u16 channel)
{
	switch (channel)
	{

//zdkshield
#if 0
		case	L_FRONT:
			TIM_SetCompare4(TIM4,1000);
			break;
		case	L_BACK:
			TIM_SetCompare3(TIM4,1000);
			break;
		case	R_BACK:
			TIM_SetCompare1(TIM4,1000);
			break;
		case	R_FRONT:
			TIM_SetCompare2(TIM4,1000);
			break;
#endif


	case	L_FRONT:
		TIM_SetCompare3(TIM4,0);
		//TIM_SetCompare4(TIM4,0);
		break;
	case	L_BACK:
		TIM_SetCompare4(TIM4,0);
		//TIM_SetCompare3(TIM4,0);
		break;
	case	R_BACK:
		TIM_SetCompare2(TIM4,0);
		//TIM_SetCompare1(TIM4,0);
		break;
	case	R_FRONT:
		TIM_SetCompare1(TIM4,0);
		//TIM_SetCompare2(TIM4,0);
		break;

//qz add 20180515
	case 	CHARGE_PWM:
		TIM_SetCompare1(TIM3,0);
		break;



	}
}

void Set_SideBrush_Pwm(u16 pwm)
{
	TIM_SetCompare3(TIM3,pwm);
	if(pwm)
	{LSB_PWR_1}
	else
	{LSB_PWR_0}
}

void Set_MidBrush_Pwm(u16 pwm)
{
	TIM_SetCompare4(TIM3,pwm);
}

void Set_Fan_Pwm(u16 pwm)
{
	TIM_SetCompare2(TIM3,pwm);
}

void Set_Charge_Pwm(u16 pwm)
{
	TIM_SetCompare1(TIM3,pwm);
}

