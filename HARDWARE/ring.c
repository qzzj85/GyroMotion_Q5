
//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================


////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
RING  l_ring,r_ring,w_ring,check_ring,all_l_ring,all_r_ring;       //���Գ����ٶȵĽṹ
BRUSH_DATA Fan,SideBrush,MidBrush,LSideBrush,RSideBrush;	//���,��ɨ,��ɨ���ݽṹ	QZ ADD

RAP  l_rap,r_rap,w_rap;           //�����ٶȾ���Ŀ��ƽṹ
bool rap_time;                    //�����ٶȵ���ʱ���־
u32 Speed ;                    //���ֵ�����ٶ�  ����ʾΪ����ʱ��
//u16 jt_l_current;              //��������1n60����©��������ľ�̬������
//u16 jt_r_current;              //��������1n60����©��������ľ�̬������
ACTION action;
//volatile u8 stop_waiting_flag=0;
u8 bump_backing_flag=0;
//u8 frontwheelon;               //ǰ����Ч��־��1Ϊ��Ч��0Ϊ��Ч
///////////////////////˽�б���////////////////////////////////////
u8 Wheel_State = 0;            //����������״̬ 0Ϊ�ϸ�1Ϊ������
extern MOTO_TASK moto_task;



///////////////////////ȫ�ֺ���////////////////////////////////////
///////////////////////˽�к���////////////////////////////////////
void stop(void);
void Set_WheelState(void);

//=======================================================================
///////////////////////����ʵ��////////////////////////////////////


//uint8_t ADC_test_counter;

//=======================================================================

/************************************************************
����  ��    ÿ10ms ִ��һ�Σ��Զ����ݳ�����Ϣ���Ƴ���
���룺 ��
��� ����
************************************************************/

void  comm_rap(void)
{
u32 	t,l;
signed long ec;
static uint8_t	delay_counter;
	
	
/////////////////////////////////�������ֵ��ٶȱ�/////////
	if(rap_time == false)
		{return;}
	rap_time = false;
		
//////		ADC_test_counter++;
//////			if((ADC_test_counter& 0x01) == 0x01)
//////			{BIRD_PE14_1();}
//////		else{BIRD_PE14_0();}
		
	///////////////////////////////////����ͬ��////////////
		delay_counter ++;
		if(((l_rap.sign)&&(r_rap.sign) && ((delay_counter&0x01)== 0x01)))   //ֻ�������ֶ�תʱ�ŵ��ڣ�����ֻ���ٶ���������  //QZ:delay_counterΪ�����ν���
				{
					if(l_rap.master == 1)    //����������
							{
								ec = (r_ring.length*l_rap.rap)/r_rap.rap - l_ring.length;  //���ݱ����������ֵ��ٶȲ�
								if (ec > 0)				//�������߾���(��ʾΪ���ߵ�������)/�����ٶ�(��ʾΪÿ���������)*�����ٶ�=����ʵ�����߾���?//QZ
										{
											if(r_rap.pwm > 0)       //�����ٶȹ���
											//	r_rap.pwm--;
												r_rap.pwm --;
										} 
								else
										{
											if(ec < 0)              //�����ٶȹ���
												r_rap.pwm ++;
										}
							}
					else              
							{               //����������
								ec = (l_ring.length*r_rap.rap)/l_rap.rap - r_ring.length; //���ݱ����������ֵ��ٶȲ�
								if (ec > 0)
									{                          //�����ٶȹ���
										if(l_rap.pwm > 0)
											l_rap.pwm --;
									} 
								else
									{
										if(ec < 0)              //�����ٶȹ���
											l_rap.pwm ++;
									}
							}
				}	
		
		/////////////������������ʱ��////////////////////	   
		t = (r_ring.dq_time - r_ring.old_time );
		r_ring.old_time = r_ring.dq_time;
		////////////�������ߵ������� ///////////////////
		l = subbtime(r_rap.all_pulse,r_ring.all_length);
		r_rap.all_pulse = r_ring.all_length;
		r_rap.real_speed = (l*10000)/t;											//	���r_rap.real_speed������  ��ת���ݲ���
		
		l = subbtime(r_rap.pulse,r_ring.length);
		r_rap.pulse = r_ring.length;
	 /////////////////////////////////����/////////////////////////// 
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
								if( abs(ec) > 20 )		//	Ϊʲë������
										{
											if(ec > 0)
												ec = 50 ;	//QZ:r_rap.pwm>400ʱ��ec=-20~20,50,-50
											else
												ec = -50;
										}
							}
					else
							{
								if(ec >0)					//QZ:r_rap.pwm<=400ʱ��ec=100,-100
									ec = 100;
								else
									ec = -100;
							}
//////								if(ec >0)
//////									ec = 100;
//////								else
//////									ec = -100;
					
		////////////			dqcurrent = account_current( ADC_RRING_CURR);   	//�������ӵĵ���
		////////////			if(dqcurrent > (466 + jt_r_current))			//��������800ma
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
		/////////////������������ʱ��////////////////////
		t = (l_ring.dq_time - l_ring.old_time );
		l_ring.old_time = l_ring.dq_time;
		////////////�������ߵ������� ///////////////////
		l = subbtime(l_rap.all_pulse,l_ring.all_length);
		l_rap.all_pulse = l_ring.all_length;
		l_rap.real_speed = (l*10000)/t;										//	���l_rap.real_speed������  ��ת���ݲ���
	
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
					//	l_ring.ek[0] = l_rap.rap - l_rap.real_speed;			//	���l_rap.real_speed������
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
							{enable_pwm(L_FRONT,l_rap.pwm);}		//	��ǰ��
					else
							{enable_pwm(L_BACK,l_rap.pwm);}			//	�����
							
							
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
	*			���ӵ������ɨ�躯��
	*
	*
	*******************************************************/
void Motor_task(void)
{
	static  unsigned char last_flag_l = STOP;
	static  unsigned char last_flag_r = STOP;
	static 	unsigned int bump_start_time;
	unsigned char now_r,now_l,real_stop;
	
	
	

			
			
			
			
		
	//��ײ�ж�
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
					
			

			
////////////		//��ײ�ж�
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
			

			
	//����̧���ж�
	//if(!lidi.sign)	//QZ:MASK
	if((!l_lidi.sign)||(!r_lidi.sign))	//QZ:ADD
			{
				moto_task.pwm_l = 0;
				moto_task.pwm_r = 0;
				moto_task.mode 	= 1;
				moto_task.sign	= 1;
			}
			
			
			
	//���ж������Ƿ�������ֹͣת��
	if((r_rap.real_speed == 0) && (l_rap.real_speed == 0))
		{real_stop = 1;}
	else
		{real_stop = 0;}
		
		
	if(moto_task.sign)
			{	
				switch(moto_task.mode)
							{
								case 1:													//�ٶ���ʻģʽ
									
									//�ж����ַ���
									if(moto_task.pwm_r == 0)
										now_r = STOP;
									else
										now_r = moto_task.pwm_r > 0 ? FRONT : BACK;
									
									//�ж����ַ���
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
										)															//�ж����ӷ����Ƿ��෴
												{ 
														stop_rap();
														Speed = 0;
												}
									else
											{
														motor_run(now_l,abs(moto_task.pwm_l),now_r,abs(moto_task.pwm_r));
														last_flag_l = now_l;
														last_flag_r = now_r;
														moto_task.sign = 0;			//��־λ���
											}
									break;
											
											
											
											
											
											
								case 2:															//�����ʻģʽ
									//�ж�������ʻ����
									if(moto_task.length_l ==0)
										now_l = STOP;
									else
										now_l = moto_task.length_l > 0 ? FRONT : BACK;
									//�ж�������ʻ����
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
										)														//�ж����ӷ����Ƿ��෴			//�ж����ӷ����Ƿ��෴
											{
													stop_rap();
													Speed = 0;
											}
									else			//���ӻ��ڻ���
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
���ܣ�32λ�޷���������
���룺    little   ����
          big      ������
���أ���ֵ
************************************************************/
u32 subbtime(u32 little,u32 big)
{
 return(big-little);
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



/*********************************************************
���ܣ��������ӵ�����֮���Ƿ�����й�ͨ�����
���룺��
��������й�ͨ������·���1�����򷵻�0
*********************************************************/
u8 Test_DriveOfWheel(void) 
{
    return 0;	
////////////	
//////////////static u16  l_current, r_current;    //��ʱ��ȡ�������ֵ���
////////////u16  dqcurrent;
////////////    /********�ر�����,����������ADת����������********/
////////////    stop();
////////////	DelayMs(10);
////////////	/******������N�����ܣ�����P�������Ƿ���©����******/
////////////	GPIO_ResetBits(GPIOE, L_BACKONOFF);
////////////	GPIO_ResetBits(GPIOE, R_BACKONOFF);
////////////	GPIO_ResetBits(GPIOE,L_FRONTONOFF);
////////////	GPIO_ResetBits(GPIOE,R_FRONTONOFF);
////////////	DelayMs(50);
////////////	dqcurrent = account_current(ADC_LRING_CURR);
////////////	if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))  //����P��������©��������40ma
////////////	{	     /********�ر�����,����������ADת����������********/
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
////////////	if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))  //����P��������©��������40ma
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
////////////	/***********������P�����ܣ�����N�������Ƿ���©����********/
////////////    Set_Pwm(L_FRONT,1000);
////////////    Set_Pwm(L_BACK,1000);
////////////    Set_Pwm(R_FRONT,1000);
////////////    Set_Pwm(R_BACK,1000);
////////////	DelayMs(50);
////////////	dqcurrent = account_current(ADC_LRING_CURR);
////////////	if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))  //����n��������©��������40ma
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
////////////	if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))  //����n��������©��������40ma
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
���ܣ�������ɨ������֮���Ƿ�����й�ͨ�����
���룺��
��������й�ͨ������·���1�����򷵻�0
*********************************************************/
u8 Test_DriveOfBrush(void) 
{ 
	return 0;
	
////////////	
//////////////static u16  l_current, r_current;    //��ʱ��ȡ�������ֵ���
////////////u16  dqcurrent;
////////////    /********�ر�����,����������ADת����������********/
////////////    Reset_M_FPMOS();
////////////    Reset_M_ZPMOS();
////////////	Set_M_FNMOS();
////////////	Set_M_ZNMOS();

////////////	DelayMs(10);
////////////	/******������N�����ܣ�����P�������Ƿ���©����******/
////////////    Set_M_FPMOS();
////////////    Set_M_ZPMOS();
////////////	DelayMs(50);
////////////	dqcurrent = account_current(ADC_MB_CURR);
////////////	if((dqcurrent > 500))  //����P��������©��������40ma
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
////////////	/***********������P�����ܣ�����N�������Ƿ���©����********/
////////////	Reset_M_FNMOS();
////////////	Reset_M_ZNMOS();

////////////	DelayMs(50);
////////////	dqcurrent = account_current(ADC_MB_CURR);
////////////	if((dqcurrent  > 500))  //����n��������©��������40ma
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


