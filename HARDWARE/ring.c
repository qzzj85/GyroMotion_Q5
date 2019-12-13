
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================


////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
RING  l_ring,r_ring,w_ring,check_ring,all_l_ring,all_r_ring;       //测试车轮速度的结构
BRUSH_DATA Fan,SideBrush,MidBrush,LSideBrush,RSideBrush;	//风机,边扫,中扫数据结构	QZ ADD

RAP  l_rap,r_rap,w_rap;           //两轮速度距离的控制结构
bool rap_time;                    //两轮速度调整时间标志
u32 Speed ;                    //两轮的最大速度  ，表示为脉冲时间
//u16 jt_l_current;              //左轮由于1n60反向漏电流引起的静态电流。
//u16 jt_r_current;              //右轮由于1n60反向漏电流引起的静态电流。
ACTION action;
//volatile u8 stop_waiting_flag=0;
u8 bump_backing_flag=0;
//u8 frontwheelon;               //前轮有效标志，1为有效，0为无效
///////////////////////私有变量////////////////////////////////////
u8 Wheel_State = 0;            //左右轮驱动状态 0为合格，1为驱动坏
extern MOTO_TASK moto_task;



///////////////////////全局函数////////////////////////////////////
///////////////////////私有函数////////////////////////////////////
void stop(void);
void Set_WheelState(void);

//=======================================================================
///////////////////////函数实体////////////////////////////////////


//uint8_t ADC_test_counter;

//=======================================================================

/************************************************************
功能  ：    每10ms 执行一次，自动根据车轮信息控制车轮
输入： 无
输出 ：无
************************************************************/

void  comm_rap(void)
{
u32 	t,l;
signed long ec;
static uint8_t	delay_counter;
	
	
/////////////////////////////////控制两轮的速度比/////////
	if(rap_time == false)
		{return;}
	rap_time = false;
		
//////		ADC_test_counter++;
//////			if((ADC_test_counter& 0x01) == 0x01)
//////			{BIRD_PE14_1();}
//////		else{BIRD_PE14_0();}
		
	///////////////////////////////////两轮同步////////////
		delay_counter ++;
		if(((l_rap.sign)&&(r_rap.sign) && ((delay_counter&0x01)== 0x01)))   //只有在两轮都转时才调节，并且只调速度慢的轮子  //QZ:delay_counter为奇数次进入
				{
					if(l_rap.master == 1)    //左轮是主轮
							{
								ec = (r_ring.length*l_rap.rap)/r_rap.rap - l_ring.length;  //根据比例计算两轮的速度差
								if (ec > 0)				//右轮行走距离(表示为所走的脉冲数)/右轮速度(表示为每秒的脉冲数)*左轮速度=左轮实际行走距离?//QZ
										{
											if(r_rap.pwm > 0)       //右轮速度过快
											//	r_rap.pwm--;
												r_rap.pwm --;
										} 
								else
										{
											if(ec < 0)              //右轮速度过慢
												r_rap.pwm ++;
										}
							}
					else              
							{               //右轮是主轮
								ec = (l_ring.length*r_rap.rap)/l_rap.rap - r_ring.length; //根据比例计算两轮的速度差
								if (ec > 0)
									{                          //左轮速度过快
										if(l_rap.pwm > 0)
											l_rap.pwm --;
									} 
								else
									{
										if(ec < 0)              //左轮速度过慢
											l_rap.pwm ++;
									}
							}
				}	
		
		/////////////右轮脉冲所走时间////////////////////	   
		t = (r_ring.dq_time - r_ring.old_time );
		r_ring.old_time = r_ring.dq_time;
		////////////右轮所走的脉冲数 ///////////////////
		l = subbtime(r_rap.all_pulse,r_ring.all_length);
		r_rap.all_pulse = r_ring.all_length;
		r_rap.real_speed = (l*10000)/t;											//	这个r_rap.real_speed有问题  反转数据不对
		
		l = subbtime(r_rap.pulse,r_ring.length);
		r_rap.pulse = r_ring.length;
	 /////////////////////////////////右轮/////////////////////////// 
		if(r_rap.sign)
				{	  
					if(l == 0)
							{
								r_ring.odds++;
								if(r_ring.odds > 200)
									r_ring.state = BAD;
							}
					else
							{
								r_ring.odds = 0;
								r_ring.state = GOOD;
							}
							
					r_ring.ek[2] = r_ring.ek[1];
					r_ring.ek[1] = r_ring.ek[0];
					if(t)
						r_ring.ek[0] = r_rap.rap - (l*10000)/t;
					//	r_ring.ek[0] = r_rap.rap - r_rap.real_speed;
					else
						r_ring.ek[0] = r_rap.rap;
					
					ec = (r_ring.ek[0]-r_ring.ek[1])/KP + r_ring.ek[0]/KI +(r_ring.ek[0]+r_ring.ek[2]-2*r_ring.ek[1])/KD;
		
					if (r_rap.pwm > 400 )
							{
								if( abs(ec) > 20 )		//	为什毛？？？
										{
											if(ec > 0)
												ec = 50 ;	//QZ:r_rap.pwm>400时，ec=-20~20,50,-50
											else
												ec = -50;
										}
							}
					else
							{
								if(ec >0)					//QZ:r_rap.pwm<=400时，ec=100,-100
									ec = 100;
								else
									ec = -100;
							}
//////								if(ec >0)
//////									ec = 100;
//////								else
//////									ec = -100;
					
		////////////			dqcurrent = account_current( ADC_RRING_CURR);   	//采样轮子的电流
		////////////			if(dqcurrent > (466 + jt_r_current))			//电流大于800ma
		////////////			{
		////////////				if(ec > 0)
		////////////					ec = 0;
		////////////				ec -= 10;
		////////////			}
							
////								if(ec >0)
////									ec = 150;
////								else
////									ec = -150;
							
					ec = ec + r_rap.pwm;	//QZ:key command 

					if(ec<0  ||  r_rap.rap == 0)
							{
								r_rap.pwm = 0;
							}
					else
							{ 
								if(ec > 1000)
									ec = 1000;
								r_rap.pwm = (u16)ec; 
							}
							
							
					if(r_rap.ori == FRONT)
							{enable_pwm(R_FRONT,r_rap.pwm);}
					else
							{enable_pwm(R_BACK,r_rap.pwm);}
							
							
//////					if(r_rap.length  <=  (r_ring.length+100) )
//////				//	if(r_rap.length  <=  (r_ring.length+10) )
//////							{
//////								if(r_rap.length > r_ring.length+10)
//////									 r_rap.rap = (r_rap.length - r_ring.length) * 5;
//////							 else
//////									{
//////											if((r_rap.master))
//////													{
//////														if((--r_rap.sign) == 0)
//////																{stop();}		  			 
//////													}
//////										}
//////							}
							
							
						if(r_rap.length  <=  (r_ring.length+5) )
							{
									r_rap.sign		= 0;
									r_rap.pwm			= 0;
									r_rap.rap 		= 0;
									disable_pwm(R_BACK);
									disable_pwm(R_FRONT); 
							}	
						else if(r_rap.length  <=  (r_ring.length+50) )
							{
								if(r_rap.rap > 500)
									{	r_rap.rap -=10;}  
							}
				}
		////////////////////////////////////////////////////////////////
				
				
				
				
		//////////////////////////////////////////////////////////////// 
		/////////////左轮脉冲所走时间////////////////////
		t = (l_ring.dq_time - l_ring.old_time );
		l_ring.old_time = l_ring.dq_time;
		////////////左轮所走的脉冲数 ///////////////////
		l = subbtime(l_rap.all_pulse,l_ring.all_length);
		l_rap.all_pulse = l_ring.all_length;
		l_rap.real_speed = (l*10000)/t;										//	这个l_rap.real_speed有问题  反转数据不对
	
		l = subbtime(l_rap.pulse,l_ring.length);
		l_rap.pulse = l_ring.length;
				
				
		if(l_rap.sign)
				{	
					if(l == 0)
							{
									l_ring.odds++;
									if(l_ring.odds > 200)
											l_ring.state = BAD;
							}
					else
							{
									l_ring.odds = 0;
									l_ring.state = GOOD;
							}
							
							
					l_ring.ek[2] = l_ring.ek[1];
					l_ring.ek[1] = l_ring.ek[0];
							
					if(t)
							l_ring.ek[0] = l_rap.rap - (l*10000)/t;
					//	l_ring.ek[0] = l_rap.rap - l_rap.real_speed;			//	这个l_rap.real_speed有问题
					else
							l_ring.ek[0] = l_rap.rap;
					
					ec = (l_ring.ek[0]-l_ring.ek[1])/KP 
								+l_ring.ek[0]/KI
								+(l_ring.ek[0]+l_ring.ek[2]-2*l_ring.ek[1])/KD;
					
								
					if (l_rap.pwm > 400 )
							{
								if( abs(ec) > 20 )
								{
									if(ec > 0)
										ec = 50 ;
									else
										ec = -50;
								}
							}
					else
							{
								if(ec >0)
									ec = 100;
								else
									ec = -100;
							}


							
		////////////			dqcurrent = account_current( ADC_LRING_CURR);
		////////////			if(dqcurrent > (466 + jt_l_current))
		////////////			{
		////////////				if(ec > 0)
		////////////					ec = 0;
		////////////				ec -= 10;
		////////////			}
							
							
////								if(ec >0)
////									ec = 150;
////								else
////									ec = -150;
								
					ec = ec + l_rap.pwm;	//QZ:key command
					
					if(ec<0||l_rap.rap == 0)
							{
								l_rap.pwm = 0;
							}
					else
							{ 
								if(ec > 1000)
									ec = 1000;
								l_rap.pwm = (u16)ec; 
							}
							
							
					if(l_rap.ori == FRONT)
							{enable_pwm(L_FRONT,l_rap.pwm);}		//	向前走
					else
							{enable_pwm(L_BACK,l_rap.pwm);}			//	向后走
							
							
//////					if(l_rap.length<=l_ring.length+100)
//////				//	if(l_rap.length<=l_ring.length+10)
//////							{
//////								if(l_rap.length > l_ring.length+5)
//////									l_rap.rap = (l_rap.length - l_ring.length) * 5;
//////							 else
//////									{
//////										if((l_rap.master))
//////												{
//////													if((--l_rap.sign) == 0)
//////															{stop();}		  	
//////												}
//////									}
//////							}
							
						if(l_rap.length  <=  (l_ring.length+5) )
							{
									l_rap.sign		= 0;
									l_rap.pwm			= 0;
									l_rap.rap 		= 0;
									disable_pwm(L_FRONT);
									disable_pwm(L_BACK);
							}	
						else if(l_rap.length  <=  (l_ring.length+50) )
							{
								if(l_rap.rap > 500)
									{	l_rap.rap -=10;} 
							}
				}
				
				
}
//=======================================================================
//=======================================================================
void Change_Ring_Speed(int lspeed,int rspeed)
{
	l_rap.rap=lspeed;
	r_rap.rap=rspeed;
}

void enable_rap_no_length(u32 ori_l,u32 speed_l,u32 ori_r,u32 speed_r)
{
	Enable_RingPWMCtrl();
	Open_Ring_Cnt();
	if(giv_sys_err != 0)
	{
		return;
	}
						   
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
	if(giv_sys_err != 0)
	{
		return;
	}
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
	if(giv_sys_err != 0)
	{
		return;
	}
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








//==============================================================================================
//==============================================================================================
void motor_run(u32 ori_l, u32 pwm_l,u32 ori_r,u32 pwm_r)
{
	u32 master_pulse = 1000000;
	if(ori_l == STOP && ori_r == STOP)
		{
			stop_rap();
			Speed = 0;
		}
		
		
	if(pwm_l >= pwm_r)
		{
//////			Speed = pwm_l / 0.2167;
			Speed = pwm_l / 0.1083;
			enable_rap(ori_l,	((double)pwm_l/(double)pwm_r)*master_pulse,				ori_r,			master_pulse);
		}
	else
		{
////////////			Speed = pwm_r / 0.2167;
			Speed = pwm_r / 0.1083;
			enable_rap(ori_l,	master_pulse,			ori_r,	((double)pwm_r/(double)pwm_l)*master_pulse);
		}
}
//====================================================================









//====================================================================
/********************************************************
	*			void Motor_task(void)
	*			轮子电机任务扫描函数
	*
	*
	*******************************************************/
void Motor_task(void)
{
	static  unsigned char last_flag_l = STOP;
	static  unsigned char last_flag_r = STOP;
	static 	unsigned int bump_start_time;
	unsigned char now_r,now_l,real_stop;
	
	
	

			
			
			
			
		
	//碰撞判断
	//if((l_bump.sign)&&(lidi.sign))
		//if((l_bump.sign||r_bump.sign)&&(lidi.sign))	//QZ:MASK
		if((l_bump.sign||r_bump.sign)&&((l_lidi.sign)&&(r_lidi.sign)))
			{
				if(bump_backing_flag == 0)
						{
							bump_start_time = giv_sys_time;
						}
				bump_backing_flag = 1;
				if((giv_sys_time - bump_start_time) > 6000)
						{
							stop_rap();
							bump_backing_flag = 0;
							l_bump.sign = 0;	
							r_bump.sign = 0;
						}
				else
						{
							moto_task.pwm_l = -100;
							moto_task.pwm_r	= -100;
							moto_task.mode 	= 1;
							moto_task.sign	= 1 ;
						}
			}
					
			

			
////////////		//碰撞判断
////////////	if((l_bump.sign||r_bump.sign)&&(lidi.sign))
////////////			{
////////////				if(bump_backing_flag == 0)
////////////						{
////////////							bump_start_time = giv_sys_time;
////////////						}
////////////				bump_backing_flag = 1;
////////////				if((giv_sys_time - bump_start_time) > 6000)
////////////						{
////////////							bump_backing_flag = 0;
////////////							l_bump.sign = 0;
////////////							r_bump.sign = 0;		
////////////							stop_rap();
////////////						}
////////////				else
////////////						{
////////////							moto_task.pwm_l = -100;
////////////							moto_task.pwm_r	= -100;
////////////							moto_task.mode 	= 1;
////////////							moto_task.sign	= 1 ;
////////////						}
////////////			}		
			

			
	//轮子抬起判断
	//if(!lidi.sign)	//QZ:MASK
	if((!l_lidi.sign)||(!r_lidi.sign))	//QZ:ADD
			{
				moto_task.pwm_l = 0;
				moto_task.pwm_r = 0;
				moto_task.mode 	= 1;
				moto_task.sign	= 1;
			}
			
			
			
	//先判断轮子是否真正的停止转动
	if((r_rap.real_speed == 0) && (l_rap.real_speed == 0))
		{real_stop = 1;}
	else
		{real_stop = 0;}
		
		
	if(moto_task.sign)
			{	
				switch(moto_task.mode)
							{
								case 1:													//速度行驶模式
									
									//判断右轮方向
									if(moto_task.pwm_r == 0)
										now_r = STOP;
									else
										now_r = moto_task.pwm_r > 0 ? FRONT : BACK;
									
									//判断左轮方向
									if(moto_task.pwm_l == 0)
										now_l = STOP;
									else
										now_l = moto_task.pwm_l > 0 ? FRONT : BACK;	

									
									if(
											(
													((now_l == BACK) && (last_flag_l == FRONT)) || 
													((now_l == FRONT) && (last_flag_l == BACK)) ||
													(last_flag_l == STOP)	|| (last_flag_r == STOP) ||
													((now_r == BACK) && (last_flag_r == FRONT)) || 
													((now_r == FRONT) && (last_flag_r == BACK))
											) 
											&& (real_stop ==0) 
										)															//判断轮子方向是否相反
												{ 
														stop_rap();
														Speed = 0;
												}
									else
											{
														motor_run(now_l,abs(moto_task.pwm_l),now_r,abs(moto_task.pwm_r));
														last_flag_l = now_l;
														last_flag_r = now_r;
														moto_task.sign = 0;			//标志位清除
											}
									break;
											
											
											
											
											
											
								case 2:															//里程行驶模式
									//判断左轮行驶方向
									if(moto_task.length_l ==0)
										now_l = STOP;
									else
										now_l = moto_task.length_l > 0 ? FRONT : BACK;
									//判断右轮行驶方向
									if(moto_task.length_r ==0)
										now_r = STOP;
									else
										now_r = moto_task.length_r > 0 ? FRONT : BACK;

									if(
											(
													((now_l == BACK) && (last_flag_l == FRONT)) || 
													((now_l == FRONT) && (last_flag_l == BACK)) ||
													(last_flag_l == STOP)	|| (last_flag_r == STOP) ||
													((now_r == BACK) && (last_flag_r == FRONT)) || 
													((now_r == FRONT) && (last_flag_r == BACK))
											) 
											&& (real_stop ==0) 
										)														//判断轮子方向是否相反			//判断轮子方向是否相反
											{
													stop_rap();
													Speed = 0;
											}
									else			//轮子还在滑行
											{		
														Speed = moto_task.speed;
														//motor_run(now_l,abs(moto_task.pwm_l),now_r,abs(moto_task.pwm_r));
														enable_rap(now_l,moto_task.length_l,now_r,moto_task.length_r);
														last_flag_l = now_l;
														last_flag_r = now_r;
														moto_task.sign = 0;
											}
									break;
											
											
											
											
											
											
											
											
							}
			}//
}
//===========================================================








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

/************************************************************
功能：32位无符号数减法
输入：    little   减数
          big      被减数
返回：差值
************************************************************/
u32 subbtime(u32 little,u32 big)
{
 return(big-little);
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



/*********************************************************
功能：测试轮子的驱动之间是否存在有贯通的情况
输入：无
输出：在有贯通的情况下返回1，否则返回0
*********************************************************/
u8 Test_DriveOfWheel(void) 
{
    return 0;	
////////////	
//////////////static u16  l_current, r_current;    //临时读取的左右轮电流
////////////u16  dqcurrent;
////////////    /********关闭驱动,测量左右轮AD转换的误差电流********/
////////////    stop();
////////////	DelayMs(10);
////////////	/******打开下面N沟道管，测量P沟道管是否有漏电流******/
////////////	GPIO_ResetBits(GPIOE, L_BACKONOFF);
////////////	GPIO_ResetBits(GPIOE, R_BACKONOFF);
////////////	GPIO_ResetBits(GPIOE,L_FRONTONOFF);
////////////	GPIO_ResetBits(GPIOE,R_FRONTONOFF);
////////////	DelayMs(50);
////////////	dqcurrent = account_current(ADC_LRING_CURR);
////////////	if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))  //左轮P沟道管有漏电流大于40ma
////////////	{	     /********关闭驱动,测量左右轮AD转换的误差电流********/
////////////        stop();
////////////	    DelayMs(20);
////////////	    GPIO_ResetBits(GPIOE, L_BACKONOFF);
////////////	    GPIO_ResetBits(GPIOE, R_BACKONOFF);
////////////	    GPIO_ResetBits(GPIOE,L_FRONTONOFF);
////////////	    GPIO_ResetBits(GPIOE,R_FRONTONOFF);
////////////	    DelayMs(200);
////////////	    dqcurrent = account_current(ADC_LRING_CURR);
////////////		if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))
////////////		{
////////////		    giv_sys_err = 27;
////////////			stop();
////////////		    return 1;
////////////		}
////////////	}
////////////	dqcurrent = account_current(ADC_RRING_CURR);
////////////	if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))  //右轮P沟道管有漏电流大于40ma
////////////	{			
////////////        stop();
////////////	    DelayMs(20);
////////////	    GPIO_ResetBits(GPIOE, L_BACKONOFF);
////////////	    GPIO_ResetBits(GPIOE, R_BACKONOFF);
////////////	    GPIO_ResetBits(GPIOE,L_FRONTONOFF);
////////////	    GPIO_ResetBits(GPIOE,R_FRONTONOFF);
////////////	    DelayMs(200);
////////////		dqcurrent = account_current(ADC_RRING_CURR);
////////////		if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))
////////////		{
////////////		    giv_sys_err = 28;
////////////			stop();
////////////		    return 1;
////////////		}
////////////	}
////////////    stop();
////////////	DelayMs(10);
////////////	/***********打开上面P沟道管，测量N沟道管是否有漏电流********/
////////////    Set_Pwm(L_FRONT,1000);
////////////    Set_Pwm(L_BACK,1000);
////////////    Set_Pwm(R_FRONT,1000);
////////////    Set_Pwm(R_BACK,1000);
////////////	DelayMs(50);
////////////	dqcurrent = account_current(ADC_LRING_CURR);
////////////	if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))  //左轮n沟道管有漏电流大于40ma
////////////	{			
////////////        stop();
////////////	    DelayMs(20);
////////////        Set_Pwm(L_FRONT,1000);
////////////        Set_Pwm(L_BACK,1000);
////////////        Set_Pwm(R_FRONT,1000);
////////////        Set_Pwm(R_BACK,1000);
////////////	    DelayMs(200);
////////////	    dqcurrent = account_current(ADC_LRING_CURR);
////////////		if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))
////////////		{
////////////		    giv_sys_err = 29;
////////////			stop();
////////////		    return 1;
////////////		}
////////////	}
////////////	dqcurrent = account_current(ADC_RRING_CURR);
////////////	if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))  //右轮n沟道管有漏电流大于40ma
////////////	{			  
////////////        stop();
////////////	    DelayMs(20);
////////////        Set_Pwm(L_FRONT,1000);
////////////        Set_Pwm(L_BACK,1000);
////////////        Set_Pwm(R_FRONT,1000);
////////////        Set_Pwm(R_BACK,1000);
////////////	    DelayMs(200);
////////////		dqcurrent = account_current(ADC_RRING_CURR);
////////////		if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))
////////////		{
////////////		    giv_sys_err = 30;
////////////			stop();
////////////		    return 1;
////////////		}
////////////	}
////////////    stop();
////////////	DelayMs(10);
////////////    return 0;	
}
 
/*********************************************************
功能：测试中扫的驱动之间是否存在有贯通的情况
输入：无
输出：在有贯通的情况下返回1，否则返回0
*********************************************************/
u8 Test_DriveOfBrush(void) 
{ 
	return 0;
	
////////////	
//////////////static u16  l_current, r_current;    //临时读取的左右轮电流
////////////u16  dqcurrent;
////////////    /********关闭驱动,测量左右轮AD转换的误差电流********/
////////////    Reset_M_FPMOS();
////////////    Reset_M_ZPMOS();
////////////	Set_M_FNMOS();
////////////	Set_M_ZNMOS();

////////////	DelayMs(10);
////////////	/******打开下面N沟道管，测量P沟道管是否有漏电流******/
////////////    Set_M_FPMOS();
////////////    Set_M_ZPMOS();
////////////	DelayMs(50);
////////////	dqcurrent = account_current(ADC_MB_CURR);
////////////	if((dqcurrent > 500))  //左轮P沟道管有漏电流大于40ma
////////////	{
////////////	    DelayMs(200);
////////////	    dqcurrent = account_current(ADC_MB_CURR);
////////////		if((dqcurrent  > 500))
////////////		{
////////////            Reset_M_FPMOS();
////////////            Reset_M_ZPMOS();
////////////	        Set_M_FNMOS();
////////////	        Set_M_ZNMOS();
////////////		    return 1;
////////////		}
////////////	}
////////////    Reset_M_FPMOS();
////////////    Reset_M_ZPMOS();
////////////	Set_M_FNMOS();
////////////	Set_M_ZNMOS();
////////////	DelayMs(10);
////////////	/***********打开上面P沟道管，测量N沟道管是否有漏电流********/
////////////	Reset_M_FNMOS();
////////////	Reset_M_ZNMOS();

////////////	DelayMs(50);
////////////	dqcurrent = account_current(ADC_MB_CURR);
////////////	if((dqcurrent  > 500))  //左轮n沟道管有漏电流大于40ma
////////////	{
////////////	    DelayMs(200);
////////////	    dqcurrent = account_current(ADC_MB_CURR);
////////////		if((dqcurrent  > 500))
////////////		{
////////////		    giv_sys_err = 29;
////////////			stop();
////////////		    return 1;
////////////		}
////////////	}
////////////    Reset_M_FPMOS();
////////////    Reset_M_ZPMOS();
////////////	Set_M_FNMOS();
////////////	Set_M_ZNMOS();
////////////	DelayMs(10);
////////////    return 0;	
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


