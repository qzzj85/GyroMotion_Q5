
//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================


////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
RING  l_ring,r_ring,w_ring;       //���Գ����ٶȵĽṹ
BRUSH_DATA Fan,SideBrush,MidBrush,LSideBrush,RSideBrush;	//���,��ɨ,��ɨ���ݽṹ	QZ ADD

RAP  l_rap,r_rap,w_rap;           //�����ٶȾ���Ŀ��ƽṹ
bool rap_time;                    //�����ٶȵ���ʱ���־
u32 Speed ;                    //���ֵ�����ٶ�  ����ʾΪ����ʱ��
ACTION action;
u8 bump_backing_flag=0;
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////
///////////////////////˽�к���////////////////////////////////////

//=======================================================================
///////////////////////����ʵ��////////////////////////////////////


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
/****************ʹ�ܳ�����һ���Ķ���**************************
����
					place          	��������   0����   1����
					ori            	���ֶ����ķ���
					length    			���ֶ����ľ���
					rap           	���ֶ������ٶ�
					sign          	���ֶ���ģʽ
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
	else if((length_l > length_r))			//���������Ҫ���ߵľ���������֣�QZ
	{
			l_rap.rap			= Speed;	//�����ٶ�(��ʾΪ����ʱ��),QZ
			l_rap.sign		= 1;			//���ֱ�־��1��������������ʽΪ����ɲ����QZ
			l_rap.master	= 1;			//����Ϊ������
		
			r_rap.master	= 0;
			if(length_r>0)					//���������Ҫ���ߵľ������0��QZ
					{ 
						r_rap.rap  = (Speed*r_rap.length)/l_rap.length;	//�����ٶ�(ÿ��������)=(�ٶ�(�˴�λ�����ٶ�)*������Ҫ���߾���)/������Ҫ���߾���,QZ
																		//�����ٶ�=������Ҫ���߾���/(������Ҫ���߾���/�����ٶ�)QZ
						r_rap.sign = 1;		//���ֱ�־��1��������������ʽΪ����ɲ����QZ
					}
			else							//���������Ҫ���ߵľ���С�ڵ���0��QZ
					{r_rap.sign=0;}			//���ֱ�־��0������ֹͣ��QZ
	}
	else if((length_r > length_l))			//����������,�������߾���������֣���������Ϊ�����֣�QZ  
	{
			r_rap.rap			= Speed;
			r_rap.sign		= 1;
			r_rap.master	= 1;
		
			l_rap.master	= 0;
			if(length_l>0)		
			{		
				l_rap.rap  = (l_rap.length*Speed)/r_rap.length ;	////�����ٶ�=������Ҫ���߾���/(������Ҫ���߾���/�����ٶ�)QZ
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
	else if((length_l > length_r))			//���������Ҫ���ߵľ���������֣�QZ
	{
			l_rap.rap			= Speed;	//�����ٶ�(��ʾΪ����ʱ��),QZ
			l_rap.sign		= 1;			//���ֱ�־��1��������������ʽΪ����ɲ����QZ
			l_rap.master	= 1;			//����Ϊ������
		
			r_rap.master	= 0;
			if(length_r>0)					//���������Ҫ���ߵľ������0��QZ
					{ 
						r_rap.rap  = (Speed*r_rap.length)/l_rap.length;	//�����ٶ�(ÿ��������)=(�ٶ�(�˴�λ�����ٶ�)*������Ҫ���߾���)/������Ҫ���߾���,QZ
																		//�����ٶ�=������Ҫ���߾���/(������Ҫ���߾���/�����ٶ�)QZ
						r_rap.sign = 1;		//���ֱ�־��1��������������ʽΪ����ɲ����QZ
					}
			else							//���������Ҫ���ߵľ���С�ڵ���0��QZ
					{r_rap.sign=0;}			//���ֱ�־��0������ֹͣ��QZ
	}
	else if((length_r > length_l))			//����������,�������߾���������֣���������Ϊ�����֣�QZ  
	{
			r_rap.rap			= Speed;
			r_rap.sign		= 1;
			r_rap.master	= 1;
		
			l_rap.master	= 0;
			if(length_l>0)		
			{		
				l_rap.rap  = (l_rap.length*Speed)/r_rap.length ;	////�����ٶ�=������Ҫ���߾���/(������Ҫ���߾���/�����ٶ�)QZ
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
ֹͣ�������ӵ�ת��
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
���ܣ�ֹͣ����ת��
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
	//w_rap.sign = 0;//ȡ��ǰ�ּ���
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

	//ʹ��R_SPEED�ж�,L_SPEED�ж�,�Ѿ��ں����ж���ʹ��
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource1);		//����
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource0);		//����
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
	 ////////////////////��ʼ��������ֵ////////////////////
	l_ring.all_length			= 0;		//qz add 20180803
	l_ring.speed_length 		= 0;		//qz add 20180803
	l_ring.last_speed_length	= 0;		//qz add 20180803
	l_ring.length				= 0;	//���ֵ�����Ϊ0
	l_ring.dq_time				= giv_sys_time;  //��λ�������ʱ��
	l_ring.old_time 			= giv_sys_time;
	l_ring.state				= GOOD;
	l_ring.odds 				= 0;
	l_rap.sign					= 0;
	l_ring.real_speed			= 0;		//qz add
		
	r_bump.sign = 0; //��ײ��������ʼ��
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
	Speed				= 0;//1500;   // 1/1000��
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
���ܣ�ʹ�ܲ����Լ�ǰײ
*********************************************************/
void Enable_Speed(void)
{
//    Reset_Speed_Send();
	Set_Speed_Send();
}
/*********************************************************
���ܣ��رղ����Լ�ǰײ
*********************************************************/
void Disable_Speed(void)
{
//    Set_Speed_Send();  //�رպ���ƹⷢ�� 
	Reset_Speed_Send();		//qz modify 20180201
}


