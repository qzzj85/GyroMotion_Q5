
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================


////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
RING  l_ring,r_ring,w_ring;       //测试车轮速度的结构
BRUSH_DATA Fan,SideBrush,MidBrush,LSideBrush,RSideBrush;	//风机,边扫,中扫数据结构	QZ ADD

RAP  l_rap,r_rap,w_rap;           //两轮速度距离的控制结构
bool rap_time;                    //两轮速度调整时间标志
u32 Speed ;                    //两轮的最大速度  ，表示为脉冲时间
ACTION action;
u8 bump_backing_flag=0;
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////
///////////////////////私有函数////////////////////////////////////

//=======================================================================
///////////////////////函数实体////////////////////////////////////


void Change_Ring_Speed(int lspeed,int rspeed)
{
	l_rap.rap=lspeed;
	r_rap.rap=rspeed;
}

void enable_rap_no_length(u32 ori_l,u32 speed_l,u32 ori_r,u32 speed_r)
{
	Enable_RingPWMCtrl();
	Open_Ring_Cnt();
						   
	l_rap.pwm	= l_rap.pwm;
	r_rap.pwm	= r_rap.pwm;
			
	l_rap.ori			=	ori_l;
	l_rap.length	=	FARAWAY;

	l_ring.length	= 0; 
	l_ring.odds 	= 0;
	l_ring.ek[0]	= 0;
	l_ring.ek[1]	= 0;
	l_ring.ek[2]	= 0;

	//qz add
	for(int i=0;i<3;i++)
		{
			l_ring.f_ek[i]=0.0;
			r_ring.f_ek[i]=0.0;
		}
	//qz add

	w_ring.dq_time = giv_sys_time;

	r_rap.ori			= ori_r;
	r_rap.length	= FARAWAY;
	
	r_ring.length	= 0; 
	r_ring.odds 	= 0;
	r_ring.ek[0]	= 0;
	r_ring.ek[1]	= 0;
	r_ring.ek[2]	= 0;

								
	r_rap.AccDec	= 50;
	r_rap.rap		= speed_r;
	r_rap.sign		= 1;
	l_rap.AccDec	= 50;
	l_rap.rap		= speed_l;
	l_rap.sign		= 1;
	r_rap.Max_Speed = Speed;
	l_rap.Max_Speed = Speed;
		
	r_rap.Desired_Speed = 0;	
	l_rap.Desired_Speed = 0;	
		
	r_rap.ACC_Position = r_rap.length / 2;	
	l_rap.ACC_Position = l_rap.length / 2;
		
	r_rap.State = 0x70;
	l_rap.State = 0x70;

	//QZ ADD
	r_rap.pulse=0;
	l_rap.pulse=0;
	//QZ ADD END	
	Gyro_Data.straight=false;

	spd_acc_flag=true;
}

//=======================================================================
//=======================================================================
/****************使能车轮做一定的动作**************************
输入
					place          	车轮左右   0右轮   1左轮
					ori            	车轮动作的方向
					length    			车轮动作的距离
					rap           	车轮动作的速度
					sign          	车轮动作模式
*/
void  enable_rap(u32 ori_l, u32 length_l,u32 ori_r,u32 length_r)
{
	if(Speed>MAX_SPEED)
		Speed=MAX_SPEED;

	Gyro_Data.straight=0;
	Gyro_Data.g_ek[0]=0;
	Gyro_Data.g_ek[1]=0;
	Gyro_Data.g_ek[2]=0;
	
	//	l_rap.real_ori = ori_l;
	//	r_rap.real_ori = ori_r;
	               
	l_rap.pwm	= l_rap.pwm;
	r_rap.pwm	= r_rap.pwm;
			
	l_rap.ori			=	ori_l;
	l_rap.length	=	length_l;

	r_rap.ori			=	ori_r;
	r_rap.length	=	length_r;
			    
	l_ring.length	= 0; 
	l_ring.odds		= 0;
	l_ring.ek[0]	= 0;
	l_ring.ek[1]	= 0;
	l_ring.ek[2]	= 0;

	//qz add
	for(int i=0;i<3;i++)
		{
			l_ring.f_ek[i]=0.0;
			r_ring.f_ek[i]=0.0;
		}
	//qz add

	w_ring.dq_time = giv_sys_time;

	r_rap.ori			= ori_r;
	r_rap.length	= length_r;
    
	r_ring.length	= 0; 
	r_ring.odds		= 0;
	r_ring.ek[0]	= 0;
	r_ring.ek[1]	= 0;
	r_ring.ek[2]	= 0;

								
	r_rap.AccDec 	= 50;
	r_rap.rap 		= 200;
	l_rap.AccDec 	= 50;
	l_rap.rap 		= 200;
	r_rap.Max_Speed = Speed;
	l_rap.Max_Speed = Speed;
		
	r_rap.Desired_Speed = 0;	
	l_rap.Desired_Speed = 0;	
		
	r_rap.ACC_Position = length_r / 2;	
	l_rap.ACC_Position = length_l / 2;
		
	r_rap.State = 0x10;

	//QZ ADD
	r_rap.pulse=0;
	l_rap.pulse=0;

	r_rap.rap_run=0;
	l_rap.rap_run=0;
	//QZ ADD END	
	
	if(	length_l == length_r)
	{
			l_rap.rap 		= Speed;
			r_rap.rap 		= Speed;
			l_rap.sign   	=	1;   
			r_rap.sign		=	1;
			l_rap.master	=	1;
			r_rap.master	=	0;

			r_rap.State=0x70;
			l_rap.State=0x70;

			
//			l_rap.master=0;
//			r_rap.master=1;
	}	 
	else if((length_l > length_r))			//如果左轮需要行走的距离大于右轮，QZ
	{
			l_rap.rap			= Speed;	//左轮速度(表示为脉冲时间),QZ
			l_rap.sign		= 1;			//左轮标志置1，车轮启动，方式为到点刹车，QZ
			l_rap.master	= 1;			//左轮为主动轮
		
			r_rap.master	= 0;
			if(length_r>0)					//如果右轮需要行走的距离大于0，QZ
					{ 
						r_rap.rap  = (Speed*r_rap.length)/l_rap.length;	//右轮速度(每秒脉冲数)=(速度(此处位左轮速度)*右轮需要行走距离)/左轮需要行走距离,QZ
																		//右轮速度=右轮需要行走距离/(左轮需要行走距离/左轮速度)QZ
						r_rap.sign = 1;		//右轮标志置1，车轮启动，方式为到点刹车，QZ
					}
			else							//如果右轮需要行走的距离小于等于0，QZ
					{r_rap.sign=0;}			//右轮标志置0，车轮停止，QZ
	}
	else if((length_r > length_l))			//与上面类似,右轮行走距离大于左轮，所以右轮为主动轮，QZ  
	{
			r_rap.rap			= Speed;
			r_rap.sign		= 1;
			r_rap.master	= 1;
		
			l_rap.master	= 0;
			if(length_l>0)		
			{		
				l_rap.rap  = (l_rap.length*Speed)/r_rap.length ;	////左轮速度=左轮需要行走距离/(右轮需要行走距离/右轮速度)QZ
				l_rap.sign = 1;
			}
			else
				{l_rap.sign=0;}
	} 
			
////////////	{
////////////		w_rap.sign = 1;
////////////	}
	spd_acc_flag=true;
}
//==============================================================================================
//==============================================================================================


void  enable_rap_yaw(u32 ori_l, u32 length_l,u32 ori_r,u32 length_r,short tgt_yaw)
{
	if(Speed>MAX_SPEED)
		Speed=MAX_SPEED;

	Gyro_Data.straight=false;
	Gyro_Data.g_ek[0]=0;
	Gyro_Data.g_ek[1]=0;
	Gyro_Data.g_ek[2]=0;
	
	//	l_rap.real_ori = ori_l;
	//	r_rap.real_ori = ori_r;
	               
	l_rap.pwm	= l_rap.pwm;
	r_rap.pwm	= r_rap.pwm;
			
	l_rap.ori			=	ori_l;
	l_rap.length	=	length_l;

	r_rap.ori			=	ori_r;
	r_rap.length	=	length_r;
			    
	l_ring.length	= 0; 
	l_ring.odds		= 0;
	l_ring.ek[0]	= 0;
	l_ring.ek[1]	= 0;
	l_ring.ek[2]	= 0;

	//qz add
	for(int i=0;i<3;i++)
		{
			l_ring.f_ek[i]=0.0;
			r_ring.f_ek[i]=0.0;
		}
	//qz add

	w_ring.dq_time = giv_sys_time;

	r_rap.ori			= ori_r;
	r_rap.length	= length_r;
    
	r_ring.length	= 0; 
	r_ring.odds		= 0;
	r_ring.ek[0]	= 0;
	r_ring.ek[1]	= 0;
	r_ring.ek[2]	= 0;

								
	r_rap.AccDec 	= 50;
	r_rap.rap 		= 200;
	l_rap.AccDec 	= 50;
	l_rap.rap 		= 200;
	r_rap.Max_Speed = Speed;
	l_rap.Max_Speed = Speed;
		
	r_rap.Desired_Speed = 0;	
	l_rap.Desired_Speed = 0;	
		
	r_rap.ACC_Position = length_r / 2;	
	l_rap.ACC_Position = length_l / 2;
		
	r_rap.State = 0x10;

	//QZ ADD
	r_rap.pulse=0;
	l_rap.pulse=0;

	r_rap.rap_run=0;
	l_rap.rap_run=0;
	//QZ ADD END	

	Gyro_Data.tgt_yaw=tgt_yaw;
	
	if(	length_l == length_r)
	{
			l_rap.rap 		= Speed;
			r_rap.rap 		= Speed;
			l_rap.sign   	=	1;   
			r_rap.sign		=	1;
			l_rap.master	=	1;
			r_rap.master	=	0;

			r_rap.State=0x70;
			l_rap.State=0x70;

			if((l_rap.ori==FRONT)&(r_rap.ori==FRONT))
				Gyro_Data.straight=1;
			
//			l_rap.master=0;
//			r_rap.master=1;
		}
	else if((length_l > length_r))			//如果左轮需要行走的距离大于右轮，QZ
	{
			l_rap.rap			= Speed;	//左轮速度(表示为脉冲时间),QZ
			l_rap.sign		= 1;			//左轮标志置1，车轮启动，方式为到点刹车，QZ
			l_rap.master	= 1;			//左轮为主动轮
		
			r_rap.master	= 0;
			if(length_r>0)					//如果右轮需要行走的距离大于0，QZ
					{ 
						r_rap.rap  = (Speed*r_rap.length)/l_rap.length;	//右轮速度(每秒脉冲数)=(速度(此处位左轮速度)*右轮需要行走距离)/左轮需要行走距离,QZ
																		//右轮速度=右轮需要行走距离/(左轮需要行走距离/左轮速度)QZ
						r_rap.sign = 1;		//右轮标志置1，车轮启动，方式为到点刹车，QZ
					}
			else							//如果右轮需要行走的距离小于等于0，QZ
					{r_rap.sign=0;}			//右轮标志置0，车轮停止，QZ
	}
	else if((length_r > length_l))			//与上面类似,右轮行走距离大于左轮，所以右轮为主动轮，QZ  
	{
			r_rap.rap			= Speed;
			r_rap.sign		= 1;
			r_rap.master	= 1;
		
			l_rap.master	= 0;
			if(length_l>0)		
			{		
				l_rap.rap  = (l_rap.length*Speed)/r_rap.length ;	////左轮速度=左轮需要行走距离/(右轮需要行走距离/右轮速度)QZ
				l_rap.sign = 1;
			}
			else
				{l_rap.sign=0;}
	} 
			
////////////	{
////////////		w_rap.sign = 1;
////////////	}
	spd_acc_flag=true;
}

#ifdef   MOTOR_SELF_TEST
void  motor_self_test(void)
{
    static  u16   curr;
	Set_BS_Level(1);   
	delay_100us(3);
	Set_BS_Level(0);  
	u16 l;
	u8  x;
	l = 0;
	for(x=0; x<16; x++)
		{
			l += ADC_ConvertedValue[ADC_SB_CURRENT + x*16] & 0XFFF;
		}
	curr  = l/16;   
              //     Set_BS_Level(0);       		
}
#endif

/******************************************************
停止两个轮子的转动
******************************************************/
void stop_rap(void)
{
#ifdef MILE_COMPENSATION
    stop();
#else
	Disable_RingPWMCtrl();
	stop();
#endif
//	l_rap.pulse		=	0;     
	l_ring.length	=	0;
//	r_rap.pulse		=	0;     
	r_ring.length	=	0;
	action.sign		=	0;	
	mode.action = 0; 
	Gyro_Data.straight=false;
#ifdef GYRO_CAL
	if(mode.status)
		{
			if(((e_l.sign==NEAR)|(e_m.sign==NEAR)|(e_r.sign==NEAR))&(!bump_value))
				{
					if(Gyro_Data.cal_flag)
						{
							Gyro_Data.cal_flag=false;
#if 0
							if(motion1.first_gyrocal)
								{
									motion1.first_gyrocal=false;
									return;
								}
#endif
#ifdef MILE_COMPENSATION
							delay_ms(300);
#ifdef		 RTC_8M_CORR 
							rtc_8m_Report(0);
							delay_ms(10);
							rtc_8m_Report(0);	 
							delay_ms(10);	 
#endif

#else
							delay_ms(700);
#endif
							GYRO_CAL_PIN_0;
							TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
							delay_ms(1000);
							//delay_ms(1000);
							TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
							GYRO_CAL_PIN_1;
						}
				}
		}
#endif
}

/***********************************************************
功能：停止两轮转动
***********************************************************/
void stop(void)
{
	disable_pwm(L_FRONT);
	disable_pwm(L_BACK);
	disable_pwm(R_BACK);
	disable_pwm(R_FRONT) ;

#ifdef MILE_COMPENSATION
	if((!bump_value)&((r_rap.sign)|(l_rap.sign)))
		{
		    Close_Ring_Cnt();
	        back_speed();
	        Open_Ring_Cnt();
		}
#endif
	l_rap.sign=0;
	r_rap.sign=0;
	r_rap.pwm=0;
	l_rap.pwm=0;	
	//w_rap.sign = 0;//取消前轮计速
	r_ring.real_speed=0;		//qz add
	l_ring.real_speed=0;		//qz add

	l_rap.ori=0;
	r_rap.ori=0;
		
	//qz add
	r_rap.rap=0;
	l_rap.rap=0;
	r_rap.Desired_Speed=0;
	r_rap.Max_Speed=0;
	l_rap.Desired_Speed=0;
	l_rap.Max_Speed=0;
	memset(l_ring.ek,0,sizeof(l_ring.ek)/sizeof(l_ring.ek[0]));		//qz add 20180203
	memset(r_ring.ek,0,sizeof(r_ring.ek)/sizeof(r_ring.ek[0]));		//qz add 20180203

	//qz add
	for(int i=0;i<3;i++)
		{
			l_ring.f_ek[i]=0.0;
			r_ring.f_ek[i]=0.0;
		}
	//qz add
	
	r_ring.odds=0;
	l_ring.odds=0;
	//qz add end

	r_rap.rap_run=0;
	l_rap.rap_run=0;
}

void Init_Ring(void)
{
#ifdef SPEED_EXTI
	EXTI_InitTypeDef EXTI_InitStruce;
	NVIC_InitTypeDef NVIC_InitStruce;

	//使能R_SPEED中断,L_SPEED中断,已经在红外中断中使能
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource1);		//右轮
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource0);		//左轮
	EXTI_InitStruce.EXTI_Line=EXTI_Line1|EXTI_Line0;
	EXTI_InitStruce.EXTI_LineCmd=ENABLE;
	EXTI_InitStruce.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruce.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruce);

	NVIC_InitStruce.NVIC_IRQChannel=EXTI1_IRQn;
	NVIC_InitStruce.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruce.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruce.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruce);

	NVIC_InitStruce.NVIC_IRQChannel=EXTI0_IRQn;
	NVIC_InitStruce.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruce.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruce.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruce);
#endif
	//giv_sys_time=0;
	//	Reset_Speed_Send();
	 ////////////////////初始化变量的值////////////////////
	l_ring.all_length			= 0;		//qz add 20180803
	l_ring.speed_length 		= 0;		//qz add 20180803
	l_ring.last_speed_length	= 0;		//qz add 20180803
	l_ring.length				= 0;	//左轮的脉冲为0
	l_ring.dq_time				= giv_sys_time;  //复位脉冲调整时间
	l_ring.old_time 			= giv_sys_time;
	l_ring.state				= GOOD;
	l_ring.odds 				= 0;
	l_rap.sign					= 0;
	l_ring.real_speed			= 0;		//qz add
		
	r_bump.sign = 0; //碰撞传感器初始化
	l_bump.sign = 0;

	r_ring.all_length	=0; 	//qz add 20180127
	r_ring.speed_length =0; 
	r_ring.last_speed_length	=0;
	r_ring.length		= 0;	
	r_ring.dq_time		= giv_sys_time;
	r_ring.old_time 	= giv_sys_time ;
	r_ring.state			= GOOD;
	r_ring.odds 			= 0;
	r_rap.sign				= 0;
	r_ring.real_speed=0x00; 	//qz add
//		delay(500000);
//		jt_r_current = account_current( ADC_RRING_CURR);
//		jt_l_current = account_current( ADC_LRING_CURR);
//	delay(10000);
	Speed				= 0;//1500;   // 1/1000秒
	//qz add
	for(int i=0;i<3;i++)
		{
			l_ring.f_ek[i]=0.0;
			r_ring.f_ek[i]=0.0;
		}
#ifdef DEBUG_INIT
	TRACE("Ring init OK!\r\n");
#endif
}
//=======================================================================

/*********************************************************
功能：使能测速以及前撞
*********************************************************/
void Enable_Speed(void)
{
//    Reset_Speed_Send();
	Set_Speed_Send();
}
/*********************************************************
功能：关闭测速以及前撞
*********************************************************/
void Disable_Speed(void)
{
//    Set_Speed_Send();  //关闭红外灯光发送 
	Reset_Speed_Send();		//qz modify 20180201
}


