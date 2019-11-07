////////////////////////ͷ�ļ�////////////////////////////////////
#include "AAA-include.h"

////////////////////////˽�ж���//////////////////////////////////
#define	OUTSIDE_SPEED	1000
#define	SPD_CNT_MAX		7
////////////////////////ȫ�ֱ���//////////////////////////////////
s8 spot_gyrobios_cnt=0;	//������ת�࣬������ת��
///////////////////////˽�б���////////////////////////////////////
static bool piv_out;	//�����Ƿ�����չ����1Ϊ����չ����0Ϊ������С
static bool piv_left; //�����Ƿ�����ת��1Ϊ����ת��0Ϊ����ת
static bool piv_done;   //qz add��������ɶ���Ϊ1,δ���Ϊ0��20180125
static s8 spd_cnt=0;
static u8 startfrom=0;
///////////////////////ȫ�ֺ���////////////////////////////////////
///////////////////////˽�к���////////////////////////////////////
///////////////////////����ʵ��////////////////////////////////////
/*****************************************************************
���ܣ���ʼ��������ɨ�ĳ���
*****************************************************************/
void Init_Spot(u8 start_from)
{					 
    /******��ʼ����ʾ***********/
	/******��ʼ�����õ�ֵ********************/	
	if(mode.low_power)
		{
			Send_Voice(VOICE_POWER_LOW);
			Init_Cease();
			return;
		}

	
	mode.mode = SPOT;	 //����ģʽΪ��ɨ
	mode.sub_mode = SUBMODE_SPOT;
	clr_ram();
	
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(sweep_level);
	Enable_earth();
	Enable_wall();
	Enable_Speed();
	enable_hwincept();
	Speed=OUTSIDE_SPEED;

	
	
	Init_Action();
//	ReInitAd();

	WriteWorkState();

	piv_left=1;
	piv_done=0;
	piv_out=1;
	startfrom=start_from;
	if((startfrom==SPOT_FROM_CHARGE)|(startfrom==SPOT_FROM_CEASE))
		{
			motion1.clean_size=0;
			motion1.worktime=1;
		}
	CHECK_STATUS_FLAG=true;			//ʹ���쳣���
	Init_Check_Status();
	Send_Voice(VOICE_SWEEP_START);
}

void Init_Sweep_Spot(u8 start_from)
{					 
    /******��ʼ����ʾ***********/
	/******��ʼ�����õ�ֵ********************/
	
	mode.mode = SWEEP;	 //����ģʽΪ��ɨ
	mode.sub_mode = SUBMODE_SPOT;
	clr_ram();
	
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(sweep_level);
	Enable_earth();
	Enable_wall();
	Enable_Speed();
	enable_hwincept();
	Speed=OUTSIDE_SPEED;

	
	
	Init_Action();
//	ReInitAd();

	WriteWorkState();

	piv_left=1;
	piv_done=0;
	piv_out=1;
	startfrom=start_from;
	if((startfrom==SPOT_FROM_CHARGE)|(startfrom==SPOT_FROM_CEASE))
		{
			motion1.clean_size=0;
			motion1.worktime=1;
		}
	CHECK_STATUS_FLAG=true;			//ʹ���쳣���
	Init_Check_Status();
}


u8 Read_Spot_Bump(u8 ir_enable)
{
	u8 data1=0;
#ifdef CLIFF_ENABLE			//ZDK����
	data1=Read_Cliff();
	if(data1)
		{
			if((mode.bump>BUMP_ALL_CLIFF)|(mode.bump==0))
				{
					stop_rap();
					mode.bump=data1;
					mode.step_bp=0;
					
#ifdef EARTH_IN_TIM2
					enable_pwm(L_BACK,1200);
					enable_pwm(R_BACK,1200);
					l_rap.ori=BACK;
					r_rap.ori=BACK;
#endif
				}
			return data1;
		}
#endif	 

	data1=Parse_BumpValue();
	switch (data1)
	{
		case BUMP_ONLY_LEFT:						 //����ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.l_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_LEFT;
		 case BUMP_ONLY_LEFTMID:					 //������ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.l_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_LEFTMID;
		 case BUMP_LEFT_MID:						 //��������ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_LEFT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.l_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_LEFT_MID;
		 case BUMP_ONLY_RIGHT:						 //����ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.r_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_RIGHT;
		 case BUMP_ONLY_RIGHTMID:					 //������ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.r_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_RIGHTMID;
		 case BUMP_RIGHT_MID:						 //��������ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_RIGHT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.r_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_RIGHT_MID;
		 case BUMP_MID: 							 //����ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.m_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_MID;
	}

	if((w_m.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0)|(mode.bump==BUMP_SEAT)) 	//��ǽ�쿿��ǽ
				{
					stop_rap();
					mode.bump=BUMP_WALL_MID;//W_M;
					mode.step_bp=0;
					mode.bump_time=giv_sys_time;
				}
			return BUMP_WALL_MID;//W_M;
		}
	
	if((w_lm.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0))		 //����ǽ�쿿��ǽ
				{
					stop_rap();
					mode.bump=LM_WALL_BUMP;//W_LM;
					mode.step_bp=0;
				}
			return LM_WALL_BUMP;//W_LM;
		}

	if((w_rm.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0))		 //����ǽ�쿿��ǽ
				{
					stop_rap();
					mode.bump=RM_WALL_BUMP;//W_LM;
					mode.step_bp=0;
				}
			return RM_WALL_BUMP;//W_LM;
		}
	return 0;
}

void Spot_Bump_Action(u8 ir_enable)
{
	u32 m=0;
	static u8 bump_turn;
	m=Read_Spot_Bump(ir_enable);

	if(mode.bump)
		{
	  #if 0
			  switch(mode.bump)    //bump��ײ�ı�־
			  {
				  case 1://E_L			 
					  switch  (mode.step_bp)   //step_bp��ײʱ�Ĳ����־
					  {
						  case	0:
							  if(do_action(7,angle360))
							  {  
								  giv_sys_err = mode.bump;
								  Init_Err();
								  return ; 
							  }
							  if((m==1))
							  {
							   return ;
							  } 		 
							  stop_rap();
							  mode.step_bp=1;
						  case	1:
							  if(do_action(7,angle30))
							  {
								  mode.step_bp++;
							  }
							  return ;	   
						  case	2:
							  if(do_action(8,angle15))
							  {
								  mode.step_bp++;
							  } 	
							  read_earth();
							  return ;
						  case	3:		  
							  if(do_action(2,angle30))
							  {
								  mode.bump = 0;
							  }
							  read_earth();   
							  return;
						  default :  
							  stop_rap();
							  mode.step_bp = 0;
					  }
					  return  ;
				  case 4://E_R: 
					  switch  (mode.step_bp)
					  {
						  case	0:
							  if(do_action(8,angle360))
							  {  
								  giv_sys_err = mode.bump;
								  Init_Err();
								  return ; 
							  }
							  if((m==4))
							  {
								   return ;
							  } 		 
							  stop_rap();
							  mode.step_bp=1;
						  case	1:
							  if(do_action(8,angle30))
							  {
								  mode.step_bp++;
							  }
							  return ;	   
						  case	2:
							  if(do_action(7,angle15))
							  {
								  mode.step_bp++;
							  }   
							  read_earth();
							  return ;
						  case	3:		  
							  if(do_action(1,angle30))
							  {
								  mode.bump = 0;
							  }
							  read_earth();   
							  return;
						  default :  
							  stop_rap();
							  mode.step_bp = 0;
					  }
					  return  ;    
						  
					  case 2://E_LM 
				  case 3://E_RM:   
				  case 5://L_BUMP:
				  case 6://R_BUMP:	  
				  case 7://W_L:
				  case 8://W_LM:  
				  case 9://W_M: 				  
				  case 10://up_hw			  
				  case 11://l_hw	  
				  case 12://rm_hw
				  case 13://r_hw  
				  case 14://W_R:  
				  case 15://W_RM: 
					  switch(mode.step_bp)
										  {
						  case 0:
													  if(do_action(4,angle360))
														  {
															  giv_sys_err = mode.bump;
															  Init_Err();
															  return ; 
														  } 	  
													  read_sideearth();
													  if(m == 0)
														  {
															  stop_rap();
															  mode.step_bp = 1;
														  }
												  return ;
									  case 1:
										  if(do_action(1,30*Angle_1))
										  //if(do_action(1,angle30))
											  {
													  mode.bump = 0;
											  } 	   
										  read_earth();   
										  return;
											  
						  default :
							  mode.bump = 0;
									  }
					  return ;
				  default :
					  mode.bump = 0;
					  break;
			  } //����  switch(mode.bump)   
	  #endif
			
#if 1 	  
			  switch(mode.step_bp)
				  {	
					  case 0:
						  Speed=BUMP_BACK_SPEED;
						  if(do_action(4,5*CM_PLUS))
							  {

								  stop_rap();
								  mode.step_bp++;
								  return;
							  }
						  return;
					  case 1:
						  if(piv_left)
							  bump_turn=2;
						  else
							  bump_turn=1;
						  mode.step_bp++;
						  return;
					  case 2:
						  Speed=TURN_SPEED;
						  if(do_action(bump_turn,180*Angle_1))
							  {
								  stop_rap();
								  if(m==0)
									  {
										  mode.bump=0;
										  mode.step_bp=0;
										  if(piv_left)
											  piv_left=0;
										  else
											  piv_left=1;
										  Speed=4000;
									  }
								  
							  }
					  
				  }
#endif
		}
}

/*****************************************************************
���ܣ�������ɨ����
*****************************************************************/
void Do_Spot(void)
{
//  static u8 piv_out;	//�����Ƿ�����չ����1Ϊ����չ����0Ϊ������С
//  static u8 piv_left; //�����Ƿ�����ת��1Ϊ����ת��0Ϊ����ת
//  static u8 piv_done;   //qz add��������ɶ���Ϊ1,δ���Ϊ0��20180125

  s32 l_length,r_length;	//qz add 20180125
 
  //comm_rap();
  ACC_DEC_Curve();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&rm_hw);
  clr_hw_effect(&r_hw);	
  ////������һЩ��Ҫ�������ж�////////////

  Spot_Bump_Action(0);
  
  if(mode.sub_mode!=SUBMODE_SPOT)
	  return;
  if(mode.bump)
	  return;

	//ϵͳ�д���
	if(giv_sys_err != 0)
	{
	    Init_Err();
		return ;
	}

	
	
  switch (mode.step)	//step·��ִ�еĲ���
		{
			case 0:
				#if 0	//qz mask 20180125
				left(360*Angle_1);
//				left(angle360);//ԭ��תȦ1Ȧ
				mode.step++;
				piv_out = 1;
				piv_left = 1;
				break;
				#endif
			
				#if 1
				if(!piv_done)
					{
						left(360*Angle_1);
						mode.step++;
						piv_out=1;
						piv_left=1;
					}
				else
					{
						if((!r_rap.sign)&&(!l_rap.sign))
							{
								if(do_action(1,(360*Angle_1)))
									{
										piv_done=0;					//qz add ׼����һ�ζ���
										stop_rap();
//										Init_Cease();
//										Init_Commander();
										return;
									}
								return;
							}
					}
					break;
				#endif
			
			case 1:
			case 2:
			case 3:
			case 4:
				if((!l_rap.sign)&&(!r_rap.sign))
						{   
								action.step++;
//								if(piv_out == 0)
//										{
//												Init_Cease();
//												return;
//										}
								if(piv_left != 0)
										{
											//qz add
											l_length=(Angle_1*180)-mode.step*(Angle_1*45);
											if(l_length<0)
												l_length=0;
											r_length=(Angle_1*180)+mode.step*(Angle_1*45);
											if(r_length<0)
												r_length=0;
											//qz add end
												
//											enable_rap(BACK,(angle180-mode.step*angle45), FRONT,(angle180+mode.step*angle45));		//qz mask
											enable_rap(BACK,l_length, FRONT,r_length);												//qz add
										}
								else
										{
											//qz add
											l_length=(Angle_1*180)+mode.step*(Angle_1*45);
											if(l_length<0)
												l_length=0;
											r_length=(Angle_1*180)-mode.step*(Angle_1*45);
											if(r_length<0)
												r_length=0;
											//qz add end
											
//											enable_rap(FRONT,(angle180+mode.step*angle45),BACK,(angle180-mode.step*angle45) ); 
											enable_rap(BACK,l_length, FRONT,r_length);
										}
								
										//qz add
								if(piv_out!=0)
									mode.step++;
								else
									{
										mode.step--;
										piv_done=1;
									}
								
						}
				break;
						
						
				case   5:
				case   6:
				case   7:
				case   8:
				case   9:
				case   10:
				case   11:
				case   12:
				case   13:
				case   14:
				case   15:
				case   16:
				case   17:
					if((!l_rap.sign)&&(!r_rap.sign))
							{
								action.step++;
								if(piv_left != 0)
										{
												enable_rap(FRONT, (mode.step-4)*(45*Angle_1), FRONT, (mode.step-4)*(45*Angle_1)+360*(Angle_1));		//qz add									
//												enable_rap(FRONT, (mode.step-4)*angle45, FRONT, (mode.step-4)*angle45+angle360);					//qz mask 20180124
										}
								else
										{
												enable_rap(FRONT, (mode.step-4)*(45*Angle_1)+360*(Angle_1), FRONT, (mode.step-4)*(45*Angle_1));					//qz add
//												enable_rap(FRONT, (mode.step-4)*angle45+angle360, FRONT, (mode.step-4)*angle45);					//qz mask
										}
								if(mode.step == 17)
										{
												piv_out = 0;
										}
								if(piv_out != 0)
										{
												mode.step++;
										}
								else 
										{
												mode.step--;
										}
							}
				break; 
		}
}
/******************************************
��ȡ��ײ��Ϣ
*******************************************/
u32  read_bump1(void)
{
if((e_l.sign == FARN))
   {
	  if((mode.bump > 4) || (mode.bump == 0))	    //��ؼ�����
       { 
	      mode.bump = 1;// E_L;
	      mode.step_bp=0;
	      stop_rap();
	    }
	 return 1;//E_L;
   }   
//if((e_lm.sign==FARN))
//   {
//	  if((mode.bump > 4) || (mode.bump == 0))	    
//     {
//      mode.bump= 2;//E_LM;
//	  mode.step_bp=0;
//	  stop_rap();
//	 }
//	  return 2;//E_LM;
//    }
//if((e_rm.sign==FARN))
//  {
//	  if((mode.bump > 4) || (mode.bump == 0))	    
//     { 
//      mode.bump=3;//E_RM;
//	  mode.step_bp=0;
//	  stop_rap();
//	 }
//	  return 3;//E_RM;
//  }	 						
  if((e_r.sign==FARN))
   {
	  if((mode.bump > 4) || (mode.bump == 0))	   
     {	
      mode.bump=4;//E_R;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 4;//E_R;
   }
  if (0 == l_bump.key)		//qz modify:��Ļ���Ϊ�½��� 20180125
    {
	 if((mode.bump == 0))	 //��ǰײ
     {
      mode.bump=5;//L_BUMP;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 5;//L_BUMP;
 	}
  if (0 == r_bump.key)		//qz modify:��Ļ���Ϊ�½��� 20180125
    {
	if((mode.bump == 0))	 //��ǰײ
     {
      mode.bump=6;//R_BUMP;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 6;//R_BUMP;
	}      
  if((w_lm.sign==NEAR))
  {
   if((mode.bump == 0))		 //����ǽ�쿿��ǽ
     {
      mode.bump=8;//W_LM;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  
	  return 8;//W_LM;
   }
  if((w_m.sign==NEAR))
   {
   if((mode.bump == 0))	   //��ǽ�쿿��ǽ
     {
      mode.bump=9;//W_M;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 9;//W_M;
   }	   
  if((w_rm.sign==NEAR))
  {
   if((mode.bump == 0))		//����ǽ�쿿��ǽ
     {
      mode.bump=15;//W_RM;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 15;//W_RM;
   }

return 0;
}

void Spot_Bump_Action_My(u8 ir_enable)
{
	u32 m=0;
	static u8 bump_turn;
	m=Read_Spot_Bump(ir_enable);

	if(!mode.bump)
		return;

	switch(mode.step_bp)
		{
		case 0:
			Speed=BUMP_BACK_SPEED;
			if(do_action(4,5*CM_PLUS))
				{
	
					stop_rap();
					mode.step_bp++;
					return;
				}
			return;
		case 1:
			if(piv_left)
				bump_turn=2;
			else
				bump_turn=1;
			mode.step_bp++;
			return;
		case 2:
			Speed=TURN_SPEED;
			if(do_action(bump_turn,180*Angle_1))
				{
					stop_rap();
					if(m==0)
						{
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							if(piv_left)
								piv_left=0;
							else
								piv_left=1;
							if(piv_out)
								mode.step=1;
							else
								mode.step=5;
						}					
				}
		
	}
}

void Do_Spot_My(void)
{	
	static short tgt_yaw;
	static u32 end_length,l_start_length,r_start_length;
	float radius=0;
	int r_sp_turn[SPD_CNT_MAX]={0,100,200,300,400,500,600};
	int l_sp_turn[SPD_CNT_MAX]={0,100,200,300,400,500,600};
	u32 l_speed,r_speed;
	u16 abnormal=0;
	
	ACC_DEC_Curve();
	clr_hw_effect(&l_hw);
	clr_hw_effect(&rm_hw);
	clr_hw_effect(&r_hw); 

	abnormal=Read_Protect();
	if(mode.abnormity)
		{
			Action_Protect_My(abnormal);
			return;
		}

	Spot_Bump_Action_My(0);

	if(mode.sub_mode!=SUBMODE_SPOT)
		return;
	if(mode.bump)
		return;

	switch(mode.step)
		{
			case 0:
				//mode.step++;
				//break;
				Speed=TURN_SPEED;
				if(do_action(1,360*Angle_1))
					{
						stop_rap();
						piv_left=true;
						piv_out=true;
						mode.step++;
						mode.time=giv_sys_time;
						spd_cnt=0;
						spot_gyrobios_cnt=0;
					}
				break;
			case 1:
				if(giv_sys_time-mode.time>1000)
					{
						tgt_yaw=Gyro_Data.yaw;
						mode.step++;
						mode.time=giv_sys_time;
					}
				break;
			case 2:
				if(piv_left)								//���̶ֹ��ٶȣ�����ƫת
					{
						l_speed=abs(l_sp_turn[spd_cnt]);
						r_speed=OUTSIDE_SPEED;	
						r_start_length=r_ring.all_length;
						radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);	//�뾶ԼΪ20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//׼����ƫת180��
					}
				else										//���̶ֹ��ٶȣ�����ƫת
					{
						l_speed=OUTSIDE_SPEED;
						r_speed=abs(r_sp_turn[spd_cnt]);
						l_start_length=l_ring.all_length;
						radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);	//�뾶ԼΪ20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//׼����ƫת180��
					}
				
				enable_rap_no_length(FRONT, l_speed, FRONT, r_speed);
				mode.step++;
				break;
			case 3:
				if(piv_left)
					{
						if(r_ring.all_length-r_start_length>end_length)
							{
								stop_rap();
								mode.step++;
							}
					}
				else
					{
						if(l_ring.all_length-l_start_length>end_length)
							{
								stop_rap();
								mode.step++;
							}
					}
				break;
			case 4:
				mode.time=giv_sys_time;
				tgt_yaw=Gyro_Data.yaw;
				spd_cnt++;
				if(spd_cnt>SPD_CNT_MAX-1)
					{
						spd_cnt=SPD_CNT_MAX-1;
						mode.step=20;
						piv_out=false;
						tgt_yaw=Get_Reverse_Angle(Gyro_Data.yaw);
					}
				else
					{
						mode.step=1;
					}
				break;
			case 20:
				Speed=TURN_SPEED;
				do_action(2,360*Angle_1);
				if(Judge_Yaw_Reach(tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						if(!piv_left)
							piv_left=1;
						else
							piv_left=0;
						mode.step=5;
					}
				break;
			case 5:
				if(giv_sys_time-mode.time>1000)
					{
						tgt_yaw=Gyro_Data.yaw;
						mode.step++;
						mode.time=giv_sys_time;
					}
				break;
			case 6:
				if(piv_left)								//���̶ֹ��ٶȣ�����ƫת
					{
						l_speed=abs(l_sp_turn[spd_cnt]);
						r_speed=OUTSIDE_SPEED;	
						r_start_length=r_ring.all_length;
						radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);	//�뾶ԼΪ20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//׼����ƫת180��
					}
				else
					{
						l_speed=OUTSIDE_SPEED;
						r_speed=abs(r_sp_turn[spd_cnt]);
						l_start_length=l_ring.all_length;
						radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);	//�뾶ԼΪ20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//׼����ƫת180��
					}
				
				enable_rap_no_length(FRONT, l_speed, FRONT, r_speed);
				mode.step++;
				break;
			case 7:
				if(piv_left)
					{
						if(r_ring.all_length-r_start_length>end_length)
							{
								stop_rap();
								mode.step++;
							}
					}
				else
					{
						if(l_ring.all_length-l_start_length>end_length)
							{
								stop_rap();
								mode.step++;
							}
					}
				break;
			case 8:
				mode.time=giv_sys_time;
				tgt_yaw=Gyro_Data.yaw;
				spd_cnt--;
				if(spd_cnt<0)
					{
						spd_cnt=0;
						mode.step++;
					}
				else
					{
						mode.step=5;
					}
				break;
			case 9:
				stop_rap();
				switch(startfrom)
					{
						case SPOT_FROM_CEASE:
							Init_Cease();
							break;
						case SPOT_FROM_CHARGE:
							Init_Docking();
							break;
						case SPOT_FROM_SWEEP:
							mode.step++;
							TRACE("spot_gyrobios_cnt=%d\r\n",spot_gyrobios_cnt);
							break;
					}
				break;
			case 10:
				Speed=TURN_SPEED;
				if(spot_gyrobios_cnt<0)		//��ת�࣬����ת
					{
						if(do_action(2,abs(spot_gyrobios_cnt)*360*Angle_1))
							{
								stop_rap();
								mode.step++;
								spot_gyrobios_cnt=0;
							}
					}
				else if(spot_gyrobios_cnt>0)
					{
						if(do_action(1,spot_gyrobios_cnt*360*Angle_1))
							{
								stop_rap();
								mode.step++;
								spot_gyrobios_cnt=0;
							}
					}
				else
					{
						mode.step++;
					}
				break;
			case 11:
				Area_Check(0);
				Init_Shift_Point1(0);
				break;

		}
}
