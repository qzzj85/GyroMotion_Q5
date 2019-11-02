#include "AAA-include.h"
/**************************************************************
程序名称：void  Init_PWM(void)
描述：初始化pwm驱动程序，将定时器3、定时器4 配置为pwm1模式，使用
      边沿对齐模式向上计数
输入：无
输出：无
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
///////////////////////下面驱动四个左右轮的控制信号//////////////
	
#ifdef CPU_FREQ_36MHz//主频为36MHZ时，PWM频率为18KHZ，
	TIM_TimeBaseStructure.TIM_Period				= period-1;   //PWM重装值 ，就是ARR参数   //1199	
	TIM_TimeBaseStructure.TIM_Prescaler 			= prescaler;  //时钟分频系数 ,就是PSC参数
#else
#ifdef CPU_FREQ_64MHz//主频为64MHZ时，PWM频率为16KHZ
	TIM_TimeBaseStructure.TIM_Period				= period-1;   //PWM重装值 ，就是ARR参数   //1199	
	TIM_TimeBaseStructure.TIM_Prescaler 			= prescaler;  //时钟分频系数 ,就是PSC参数
#endif
#endif
	TIM_TimeBaseStructure.TIM_ClockDivision 		= 0;   //时钟分频因子，在CR1寄存器中
	TIM_TimeBaseStructure.TIM_CounterMode			= TIM_CounterMode_Up; //计数方式，在CR1寄存器中 
  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	//QZ: 定时器4，36MHz, 分频为1+1，周期为999+1，得到频率为36MHz/(1+1)/(999+1)=18KHz
	
  /* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		 //通道的工作模式  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;						 //脉冲有效宽度
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //有效电平
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

	////////////////下面定义定时器3，驱动风机中扫频率36kHz///////////////	hzh 20171218 TIM3  CH4
	///////////////20180320 qz add，根据风机datasheet，风机频率修正为25KHz//////////
	  /* Time base configuration */
#ifdef CPU_FREQ_36MHz//主频为36MHZ时，PWM频率为36KHZ
	TIM_TimeBaseStructure.TIM_Period					= period-1;  //PWM重装值 ，就是ARR参数
	TIM_TimeBaseStructure.TIM_Prescaler 				= prescaler;	 //时钟分频系数 ,就是PSC参数
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
	TIM_OCInitStructure.TIM_OCMode						= TIM_OCMode_PWM1;			//通道的工作模式  
	TIM_OCInitStructure.TIM_OutputState 				= TIM_OutputState_Enable;				//比较输出使能
	TIM_OCInitStructure.TIM_Pulse						= 0;						//脉冲有效宽度
	TIM_OCInitStructure.TIM_OCPolarity					= TIM_OCPolarity_High;			//有效电平
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);						//充电PWM控制
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);

	TIM_OC2Init(TIM3,&TIM_OCInitStructure); 						//FAN PWM控制
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Disable);

	TIM_OC3Init(TIM3,&TIM_OCInitStructure); 						//边扫PWM控制
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Disable);

	TIM_OC4Init(TIM3,&TIM_OCInitStructure); 						//中扫PWM控制
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
  
	TIM_ARRPreloadConfig(TIM3, ENABLE);
  
	TIM_SetCompare1(TIM3,0);											//关闭充电
	TIM_SetCompare2(TIM3,0);											//关闭FAN
	TIM_SetCompare3(TIM3,0);											//关闭边扫
	TIM_SetCompare4(TIM3,0);											//关闭中扫
	
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
   /////////////////////使能定时器3、4的时钟////////////
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);

	#if 1
	////////////////下面定义定时器3，驱动风机中扫频率36kHz///////////////	hzh 20171218 TIM3  CH4
	///////////////20180320 qz add，根据风机datasheet，风机频率修正为25KHz//////////
	  /* Time base configuration */
	#ifdef CPU_FREQ_36MHz//主频为36MHZ时，PWM频率为36KHZ
	TIM_TimeBaseStructure.TIM_Period					= PWM_CHARGE_MAX-1;	 //PWM重装值 ，就是ARR参数
	TIM_TimeBaseStructure.TIM_Prescaler 			= PWM_SWEEP_PRESCALER;   //时钟分频系数 ,就是PSC参数
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
  TIM_OCInitStructure.TIM_OCMode 						= TIM_OCMode_PWM1;        	//通道的工作模式  
	TIM_OCInitStructure.TIM_OutputState 			= TIM_OutputState_Enable; 				//比较输出使能
  TIM_OCInitStructure.TIM_Pulse 						= 0;                       	//脉冲有效宽度
  TIM_OCInitStructure.TIM_OCPolarity 				= TIM_OCPolarity_High; 			//有效电平
    
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);							//充电PWM控制
  TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);

  TIM_OC2Init(TIM3,&TIM_OCInitStructure);							//FAN PWM控制
  TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Disable);

  TIM_OC3Init(TIM3,&TIM_OCInitStructure);							//边扫PWM控制
  TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Disable);

  TIM_OC4Init(TIM3,&TIM_OCInitStructure);							//中扫PWM控制
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  
  TIM_SetCompare1(TIM3,0);											//关闭充电
  TIM_SetCompare2(TIM3,0);											//关闭FAN
  TIM_SetCompare3(TIM3,0);											//关闭边扫
  TIM_SetCompare4(TIM3,0);											//关闭中扫
  	
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
	GPIO_InitStructure.GPIO_Pin =   SIDEBRUSH_PWM|MIDBRUSH_PWM|CHG_PWM|FAN_PWM;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
		 
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	#endif

	
	//===================================================================================
///////////////////////下面驱动四个左右轮的控制信号//////////////
	
  #ifdef CPU_FREQ_36MHz//主频为36MHZ时，PWM频率为18KHZ，
  TIM_TimeBaseStructure.TIM_Period					  = PWM_RING_MAX-1;   //PWM重装值 ，就是ARR参数   //1199	
  TIM_TimeBaseStructure.TIM_Prescaler			  = PWM_RING_PRESCALER;	 //时钟分频系数 ,就是PSC参数
  #else
  #ifdef CPU_FREQ_64MHz//主频为64MHZ时，PWM频率为16KHZ
  TIM_TimeBaseStructure.TIM_Period					  = PWM_RING_MAX-1;   //PWM重装值 ，就是ARR参数   //1199	
  TIM_TimeBaseStructure.TIM_Prescaler			  = PWM_RING_PRESCALER;	 //时钟分频系数 ,就是PSC参数
  #endif
  #endif
  TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;   //时钟分频因子，在CR1寄存器中
  TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up; //计数方式，在CR1寄存器中 
  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	//QZ: 定时器4，36MHz, 分频为1+1，周期为999+1，得到频率为36MHz/(1+1)/(999+1)=18KHz
	
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //通道的工作模式  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_Pulse = 0;                       //脉冲有效宽度
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //有效电平
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
功能：使能PWM
输入：channel 通道名称
              ：L_FRONT     左轮向前
			  ：L_BACK      左轮向后
			  ：R_FRONT     右轮向前
			  ：R_BACK      右轮向后
			  ：CHARGE_PWM  充电pwm
	  pwm     占空比，其值在0——1000之间
输出：无
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
功能：关闭PWM
输入：channel 通道名称
              ：L_FRONT     左轮向前
			  ：L_BACK      左轮向后
			  ：R_FRONT     右轮向前
			  ：R_BACK      右轮向后
输出：无
	  
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
功能：使能PWM
输入：channel			通道名称
		：L_FRONT     左轮向前
		：L_BACK      左轮向后
		：R_FRONT     右轮向前
		：R_BACK      右轮向后
		：CHARGE_PWM  充电pwm
		pwm						占空比，其值在0——1000之间	
输出：无
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

