////////////////////////头文件////////////////////////////////////
#include "AAA-include.h"
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
u8 Docking_YaoKong_Manage(void) ;
///////////////////////全局函数////////////////////////////////////
///////////////////////私有函数////////////////////////////////////	
u8 		MID_TURN			=1;
u8		BUMP_TURN_DIR		=1;
u8 		DOCK_IN_SIGHT		=0;
bool	TOP_FLAG			=false;
bool 	DOCK_PREPARE		=false;
bool 	FIND_SEAT_FAIL;
bool 	TOP_DIR				=false;		//false向右偏转,true向左偏转
u32 	top_l_length,top_r_length,find_home,seat_time;
u8 		top_piv_left,top_piv_done,top_piv_out,top_bump_turn=0,top_turn_cnt;
#define ADDANGLE				15					//qz add 20180615
#define	BACK_LENGTH				40					//qz add 20180717后退40cm

#define MY20180412  			1
#define MIDMOVE_BUMP_ACTION		1				
bool	MIDMOVE_BUMP_FLAG		=false;			//表示找到中信号，开始回充对中过程中产生的碰撞。（默认为只与充电座碰撞，没有其他物体）
u8 		WHICH_TO_MID			=0;				//表示从哪个方向找到的中信号，左或者右
#define LEFT_TO_MID				0x02
#define	RIGHT_TO_MID			0X01


//qz add 20180418
u8 		dock_fail_count			=0;		//qz modify V2.3.8
u32 	dock_fail_time;					//qz add V2.3.8
u32     first_find_result		=0;
//qz add 20180428
#define	M_IR_CHOOSE				=1;

bool findseat_skid_check_flag=false;
u8 findseat_skid_check_step;
u32 findseat_skid_check_time;
u32 findseat_skid_check_length;

u8 REYBS_TIME=0;						//qz add 20180910,小回充没找到信号，重新请求沿边次数，3次强制失败

bool dock_rap_time=false;

/******************************************************************
回充程序的初始化
******************************************************************/
void Init_Docking(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	/******初始化设置的值********************/
	mode.mode = DOCKING;
	mode.sub_mode=DOCKING;
	mode.step=DOCKMODE_STEP_START;
	mode.step_mk=0;				//qz add 20180910
	//qz add 20180418	
	mode.time = giv_sys_time;
	dock_fail_count=0;
	//qz add end
	
	clr_ram();
	
	Enable_earth();
	Disable_wall();			//qz add 20180328
	Reset_Wall_Send();		//qz add 20180328,红外墙检，对回充红外信号有干扰，在回充阶段必须关闭
	enable_hwincept();		//允许红外接收电源
	Set_Speed_Send();
											 
	Init_Action();
	WriteWorkState();
//	ReInitAd();

	Sweep_Level_Set(DOCK_SWEEP_LEVEL);
//	mode.Info_Abort=1;		//qz add:根据张云鹏要求，小回充模式下可以仅限停止指令
	mode.All_Info_Abort=0;			//qz add 20180919
	SLAM_DOCK=false;
	DOCK_SWEEP=false;		//qz add 20180803
	FIND_SEAT_FAIL=false;
#ifdef DEBUG_Enter_Mode
	TRACE("Init Little Docking Mode Complete!\r\n");
#endif
//初始化检测的条件	
	Init_Check_Status();	//qz add 20180425
	
	findseat_skid_check_flag=false;		//qz add 20180901
	findseat_skid_check_step=0;			//qz add 20180901
	
#ifdef UV
	Reset_UV();
#endif
	Send_Voice(VOICE_DOCK_START);
}




u8 Dock_read_bump(void)
{
	u8 data1=0;

	data1=Parse_BumpValue();
	switch (data1)
		{
			case BUMP_ONLY_LEFT:
				if(!mode.bump_flag)
					{
						stop_rap();
						mode.bump=BUMP_ONLY_LEFT;
						mode.bump_flag=true;
						mode.step_bp=0;
						Slam_Data.l_bump_flag=true;
					}
				return BUMP_ONLY_LEFT;
			case BUMP_ONLY_LEFTMID:
				if(!mode.bump_flag)
					{
						stop_rap();
						mode.bump=BUMP_ONLY_LEFTMID;
						mode.bump_flag=true;
						mode.step_bp=0;
						Slam_Data.l_bump_flag=true;
					}
				return BUMP_ONLY_LEFTMID;
			case BUMP_LEFT_MID:
				if(!mode.bump_flag)
					{							
						stop_rap();
						mode.bump=BUMP_LEFT_MID;
						mode.bump_flag=true;
						mode.step_bp=0;
						Slam_Data.l_bump_flag=true;
					}
				return BUMP_LEFT_MID;
			case BUMP_ONLY_RIGHT:
				if(!mode.bump_flag)
					{							
						stop_rap();
						mode.bump=BUMP_ONLY_RIGHT;
						mode.bump_flag=true;
						mode.step_bp=0;
						Slam_Data.r_bump_flag=true;
					}
				return BUMP_ONLY_RIGHT;
			case BUMP_ONLY_RIGHTMID:
				if(!mode.bump_flag)
					{							
						stop_rap();
						mode.bump=BUMP_ONLY_RIGHTMID;
						mode.bump_flag=true;
						mode.step_bp=0;
						Slam_Data.r_bump_flag=true;
					}
				return BUMP_ONLY_RIGHTMID;
			case BUMP_RIGHT_MID:
				if(!mode.bump_flag)
					{							
						stop_rap();
						mode.bump=BUMP_RIGHT_MID;
						mode.bump_flag=true;
						mode.step_bp=0;
						Slam_Data.r_bump_flag=true;
					}
				return BUMP_RIGHT_MID;
			case BUMP_MID:
				if(!mode.bump_flag)
					{							
						stop_rap();
						mode.bump=BUMP_MID;
						mode.bump_flag=true;
						mode.step_bp=0;
						Slam_Data.m_bump_flag=true;
					}
				return BUMP_MID;
			default:
				break;
		}

	
#ifdef FINDSEAT_SKID_CHECK				//qz add 20180814
	if((Find_Seat_Skid_Check())&(mode.step==DOCKMODE_STEP_FINDSEAT))
		{
			if(mode.bump==0)
		 		{
		 			stop_rap();
					mode.bump=91;
					mode.step_bp=0;
		 		}
			return 91;
		}
						
	return 0;
#endif	
	
}

void Dock_Comm_rap_My (void)
{
#ifndef FLOAT_PID
	int ec;
#else
	float f_ec;
#endif
	
	if(dock_rap_time==false)
		return;
	dock_rap_time=false;

	Check_Speed_My(&(l_ring.real_speed), &(r_ring.real_speed));
/////////////////////////////////右轮/////////////////////////// 
	if(r_rap.sign)
		{
#ifndef FLOAT_PID
			r_ring.ek[2] = r_ring.ek[1];
			r_ring.ek[1] = r_ring.ek[0];
			r_ring.ek[0] = r_rap.rap-r_ring.real_speed;
			ec = (r_ring.ek[0]-r_ring.ek[1])/KP + r_ring.ek[0]/KI +(r_ring.ek[0]+r_ring.ek[2]-2*r_ring.ek[1])/KD;
			r_rap.pwm+=ec;
#else
			r_ring.f_ek[2]=r_ring.f_ek[1];
			r_ring.f_ek[1]=r_ring.f_ek[0];
			r_ring.f_ek[0]=(float)(r_rap.rap)-(float)(r_ring.real_speed);
			f_ec = ((r_ring.f_ek[0]-r_ring.f_ek[1])*D_KP + r_ring.f_ek[0]*D_KI +(r_ring.f_ek[0]+r_ring.f_ek[2]-2*r_ring.f_ek[1])*D_KD);
			r_rap.pwm=(int)(r_rap.pwm+f_ec);
#endif
#if 0
			if (r_rap.pwm > 400 )
				{
					if( abs(ec) > 20 )	
							{
								if(ec > 0)
									ec = 20 ;
								else
									ec = -20;
							}
				}
			else
				{
					if(ec >0)
						ec = 150;
					else
						ec = -150;
				}
		
			ec = ec + r_rap.pwm;	  
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

//					printf("ec r:%d\r\n",ec);
#endif
//			r_rap.pwm+=ec;
//			r_rap.pwm=(int)(r_rap.pwm+f_ec);
			if(r_rap.pwm>PWM_RING_MAX)
				r_rap.pwm=PWM_RING_MAX;
			else if(r_rap.pwm<=0)
				r_rap.pwm=0;
				
				
			if(r_rap.ori == FRONT)
				{enable_pwm(R_FRONT,r_rap.pwm);}
			else
				{enable_pwm(R_BACK,r_rap.pwm);}

			if(r_rap.length  <=  (r_ring.length+10) )
				{
					r_rap.sign		= 0;
					r_rap.pwm			= 0;
					r_rap.rap		= 0;
					disable_pwm(R_BACK);
					disable_pwm(R_FRONT); 
					r_rap.rap_run=0;
				}
#if 0		//qz mask 20181018
			else if(r_rap.length  <=  (r_ring.length+50) )	//QZ:减速过程
			{
				if(r_rap.rap > 500)
					{	r_rap.rap -= 15;} 
			}
			else if(r_rap.length  <=  (r_ring.length+300) )
			{
				if(r_rap.rap > 800)
					{	r_rap.rap -= 30;} 
			}
			else if(r_rap.length  <=  (r_ring.length+600) )
			{
				if(r_rap.rap > 1200)
					{	r_rap.rap -= 50;} 
			}
#else
		
#endif
				
		}
////////////////////////////////////////////////////////////////
	
	
	
	
//////////////////////////////////////////////////////////////// 
/////////////左轮脉冲所走时间////////////////////
	
	if(l_rap.sign)
		{
#ifndef FLOAT_PID		
			l_ring.ek[2] = l_ring.ek[1];
			l_ring.ek[1] = l_ring.ek[0];
			l_ring.ek[0] = l_rap.rap-l_ring.real_speed; 				

					
			ec = (l_ring.ek[0]-l_ring.ek[1])/KP 
					+l_ring.ek[0]/KI
					+(l_ring.ek[0]+l_ring.ek[2]-2*l_ring.ek[1])/KD;
			l_rap.pwm+=ec;			
#else
			l_ring.f_ek[2]=l_ring.f_ek[1];
			l_ring.f_ek[1]=l_ring.f_ek[0];
			l_ring.f_ek[0]=(float)(l_rap.rap)-(float)(l_ring.real_speed);
			f_ec = ((l_ring.f_ek[0]-l_ring.f_ek[1])*D_KP + l_ring.f_ek[0]*D_KI +(l_ring.f_ek[0]+l_ring.f_ek[2]-2*l_ring.f_ek[1])*D_KD);
			
			l_rap.pwm=(int)(l_rap.pwm+f_ec);
#endif
#if 0		
			if (l_rap.pwm > 400 )
				{
					if( abs(ec) > 20 )
					{
						if(ec > 0)
							ec = 20 ;
						else
							ec = -20;
					}
				}
			else
				{
					if(ec >0)
						ec = 150;
					else
						ec = -150;
				}
				
				
			ec = ec + l_rap.pwm;
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
#endif
//						l_rap.pwm+=ec;
//							l_rap.pwm=(int)(l_rap.pwm+f_ec);
			if(l_rap.pwm>PWM_RING_MAX)
				l_rap.pwm=PWM_RING_MAX;
			else if(l_rap.pwm<=0)
				l_rap.pwm=0;
			
		
			if(l_rap.ori == FRONT)
				{enable_pwm(L_FRONT,l_rap.pwm);}
			else
				{enable_pwm(L_BACK,l_rap.pwm);}

			if(l_rap.length  <=  (l_ring.length+10) )
				{
					l_rap.sign		= 0;
					l_rap.pwm			= 0;
					l_rap.rap		= 0;
					disable_pwm(L_FRONT);
					disable_pwm(L_BACK);
					l_rap.rap_run=0;
				}
#if 0		//qz mask 20181018
			else if(l_rap.length  <=  (l_ring.length+50) )
			{
				if(l_rap.rap > 500)
					{	l_rap.rap -= 15;} 
			}
			else if(l_rap.length  <=  (l_ring.length+300) )
			{
				if(l_rap.rap > 800)
					{	l_rap.rap -= 30;} 
			}
			else if(l_rap.length  <=  (l_ring.length+600) )
			{
				if(l_rap.rap > 1200)
					{	l_rap.rap -= 50;} 
			}
#endif		
				
	}
}

u32 ReadHwSign_My(void)
{
	   uint32 s; //由uint16改为uint32	hzh
		 
		 s = 0; 
	  
	//1,李工样机有四个红外接收头，充电座有四个发送管	
	//2,虽然底盘左右红外管读到充电座信号的机会比
	//后红外要低一些，但是读还是照样读，用就不一定用
	
	/*************************************
	
	左
	
	**************************************/
	  //if(l_hw.effect == 1)//&&((l_hw.data&0x0f) == 0)) //数据如0xd0 & 0x0f即取高四位
		{
		  if(l_hw.effectLeft) 
		  {
			s |= LEFT_INF__ULTRA_LEFT;
		  }	   			
		  if(l_hw.effectMidLeft)
		  {
			s |= LEFT_INF__MIDLEFT;
		  }
		  if(l_hw.effectRight)
		  {
			s |= LEFT_INF__RIGHT;
		  }
		  if(l_hw.effectMidRight)
		  {
			s |= LEFT_INF__MIDRIGHT;//☆读到全数据认为读到顶数据
		  } 
		  //if(l_hw.effectMidRight&l_hw.effectMidLeft&l_hw.effectLeft&l_hw.effectRight)
		  if(l_hw.effectMidLeft&l_hw.effectMidRight)
		  {
			   l_hw.effectMid=1;
			   l_hw.effect_timeMid=giv_sys_time;
		   }
		  if(l_hw.effectNear)
		  	{
		  		s|=NEAR_INF_LEFTNEAR;
		  	}
		  
		}
	/*************************************
	
	中
	
	**************************************/ 			
	   //if(rm_hw.effect == 1)//&&((rm_hw.data&0x0f) == 0)) 	//	中间红外传感器输入信号
		{
		   if(rm_hw.effectLeft) 
		   {
			 s |= MID_INF__ULTRA_LEFT;
		   }			 
		   if(rm_hw.effectMidLeft)
		   {
			 s |= MID_INF__MIDLEFT;
		   }
		   if(rm_hw.effectMidRight)
		   {
			 s |= MID_INF__MIDRIGHT;
		   }
		   if(rm_hw.effectRight)
		   {
			 s |= MID_INF__RIGHT;//☆读到全数据认为读到顶数据
		   }
		   //if(rm_hw.effectMidRight&rm_hw.effectMidLeft&rm_hw.effectLeft&rm_hw.effectRight)
		   if(rm_hw.effectMidLeft&rm_hw.effectMidRight)
		   {
		   		rm_hw.effectMid=1;
				rm_hw.effect_timeMid=giv_sys_time;
		   	}
		   if(rm_hw.effectNear)
		   	{
		   		s|=NEAR_INF_MIDNEAR;
		   	}
		}

	   //if(lm_hw.effect == 1)//&&((rm_hw.data&0x0f) == 0)) 	//	中间红外传感器输入信号
		{
		   if(lm_hw.effectLeft) 
		   {
			 s |= MMID_INF__ULTRA_LEFT;
		   }			 
		   if(lm_hw.effectMidLeft)
		   {
			 s |= MMID_INF__MIDLEFT;
		   }
		   if(lm_hw.effectMidRight)
		   {
			 s |= MMID_INF__MIDRIGHT;
		   }
		   if(lm_hw.effectRight)
		   {
			 s |= MMID_INF__RIGHT;//☆读到全数据认为读到顶数据
		   }
		   //if(lm_hw.effectMidRight&lm_hw.effectMidLeft&lm_hw.effectLeft&lm_hw.effectRight)
		   if(lm_hw.effectMidLeft&lm_hw.effectMidRight)
		   {
		   		lm_hw.effectMid=1;
				lm_hw.effect_timeMid=giv_sys_time;
		   	}
		   if(lm_hw.effectNear)
		   	{
		   		s|=NEAR_INF_MMIDNEAR;
		   	}
		}
	/*************************************
	
	右
	
	**************************************/
		//if(r_hw.effect == 1)
		{
		  if(r_hw.effectLeft)
		  {
			s |= RIGHT_INF__ULTRA_LEFT;
		  }
		  if(r_hw.effectMidLeft)
		  {
			s |= RIGHT_INF__MIDLEFT;
		  } 
		  if(r_hw.effectMidRight)
		  {
			s |= RIGHT_INF__MIDRIGHT;
		  } 
		  if(r_hw.effectRight)
		  {
			s |= RIGHT_INF__RIGHT;
		  }
		  //if(r_hw.effectMidRight&r_hw.effectMidLeft&r_hw.effectRight&r_hw.effectLeft)
		  if(r_hw.effectMidLeft&r_hw.effectMidRight)
		  {
			   r_hw.effectMid=1;
			   r_hw.effect_timeMid=giv_sys_time;
		   }
		  if(r_hw.effectNear)
		   {
			   s|=NEAR_INF_RIGHTNEAR;
		   }
		}
	/*************************************
	
	后
	
	**************************************/
		  //if(b_hw.effect == 1)
		  { 	  
			if(b_hw.effectLeft)//ULTRA_LEFT
			{
			  s |= BACK_INF__ULTRA_LEFT;
			}
			if(b_hw.effectMidLeft)//CENTER
			{
			  s |= BACK_INF__MIDLEFT;
			}	
			if(b_hw.effectMidRight)//ULTRA_RIGHT
			{
			  s |= BACK_INF__MIDRIGHT;
			} 
			if(b_hw.effectRight)//
			{
			  s |= BACK_INF__RIGHT;
			}
			//if(b_hw.effectMidRight&b_hw.effectMidLeft&b_hw.effectRight&b_hw.effectLeft)
			if(b_hw.effectMidLeft&b_hw.effectMidRight)
			{
				 b_hw.effectMid=1;
				 b_hw.effect_timeMid=giv_sys_time;
			 }
		  }
		return s;		
}

void Do_Docking_My(void)
{
	u32 abnormal;
		//系统有错误
		if(giv_sys_err != 0)
		{
		  Init_Err();
		  return ;
		}

	  /////充电管理
		
      //充电失败判断
	  //小回充超过10分钟或者尝试5次上充电座都失败（包括与充电座碰撞和电极对接不稳），停止回充，回充失败
	  if((giv_sys_time-mode.time>6000000)||(dock_fail_count>=5)||(FIND_SEAT_FAIL))		
	  	{

#ifdef DOCK_DEBUG
			TRACE("dock_fail_count=%d\r\n",dock_fail_count);
			TRACE("time=%d\r\n",giv_sys_time-mode.time);
			TRACE("FIND_SEAT_FAIL=%d\r\n",FIND_SEAT_FAIL);
#endif
	  		stop_rap();
			//qz add V2.3.8
			mode.step=DOCKMODE_STEP_DOCKFAIL;		
			mode.step_mk=0;
			dock_fail_count=0;			//以下三个变量要清0,不然会一直在次循环，而不会进入MODE_STEP_DOCKFAIL步骤
			FIND_SEAT_FAIL=false;
			mode.time=giv_sys_time;
			//qz add end
	  		//return;
	  	}
		
#if 1
		/////有充电座电极信号
	  if((power.charge_seat)&&(mode.step!=DOCKMODE_STEP_CHARGE))
	  	{
	  			stop_rap();
				mode.bump=0;
				mode.step_bp=0;
				mode.step=DOCKMODE_STEP_CHARGE;
				mode.step_mk=0;				
				mode.bump_flag=false;
				seat_time=giv_sys_time;
#ifdef DOCK_DEBUG
				TRACE("enter in DOCKMODE_STEP_CHARGE!\r\n");
#endif
				
				Sweep_Level_Set(STOP_ALL);
				return;
	  	}
	  
	  ///有直插电源信号，直接进入充电模式
#ifdef DC_NOBAT_RUN
		if((power.charge_dc)&(!dc_nobat_run))
#else
		if(power.charge_dc)
#endif
	  	{
	  		stop_rap();
			//Init_Chargeing(CHARGEING);

			//qz add 20180901
			Init_Chargeing(DC_CHARGING);
	  	}
#endif

	/*************************************
	
	清除红外有效性
	
	**************************************/
 //	说明:红外接收有效时间是100ms，在此时间内没有再收到则清除有效标志
	  clr_all_hw_effect();
	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	/*************************************	
	读取红外信号	
	**************************************/ 
	find_home = ReadHwSign_My();
	/*************************************	
	驱动轮子		  
	**************************************/ 
	//YBS_Comm_Rap_My();
	Dock_Comm_rap_My();
	
	abnormal=Read_Protect();
	if(mode.abnormity)
		{
			Action_Protect_My(abnormal);
			findseat_skid_check_flag=false;
			return;
		}

	/*************************************	
	读取碰撞信息	
	**************************************/ 
	//	m = read_bump();
	  Dock_read_bump();
	/*************************************
	
	处理碰撞
	
	**************************************/ 
	//Docking_Bump_My_Action();
	Docking_Bump_My();
	if((mode.bump!=0)&&(mode.step!=DOCKMODE_STEP_DOCKFAIL))		//qz modify V2.3.8
		{
			findseat_skid_check_flag=false;
			return;
		}
	/*************************************
	
	分类处理
	
	**************************************/
		  switch(mode.step)					//第一步，自转360
				{		
			   		case DOCKMODE_STEP_START:
						//Start_Start();
						//Start_Start_II();
						//Start_Start_III();
						Start_Start_IV();
						//Start_Start_V();
					//qz add end
		   			break;

					case DOCKMODE_STEP_TOP_SPOT:
						DOCK_PREPARE=true;
						top_piv_left=1;
						top_piv_done=0;
						top_piv_out=1;
						Start_Top_Spot_My();
						break;

					//后红外接收头，只接收到TOP信号
					case DOCKMODE_STEP_TOP:
						DOCK_PREPARE=true;
						top_piv_left=1;
						top_piv_done=0;
						top_piv_out=1;
						//Start_Top();
						//Speed=1000;
						//Start_Top_Spot();
						//Start_Top_Straight();
						//Start_Top_Straight();
						Start_Top_Spot_My();
						break;

					//在充电座中间附近
					case DOCKMODE_STEP_MID:
						Start_Mid();
						BUMP_TURN_DIR=1;
						break;
					//在充电座右边附近
					case DOCKMODE_STEP_RIGHT:
						//Start_Right();
						//Start_Right_Straight();
						//Start_Right_Straight_Spiral();
						Start_Right_Deflect_Turn();
						BUMP_TURN_DIR=1;			//qz modify 20180502:原来为2,修改为1
						break;
					//在充电座左边附近
					case DOCKMODE_STEP_LEFT:
						//Start_Left();						
						//Start__Straight();
						//Start_Left_Straight_Spiral();
						Start_Left_Deflect_Turn();
						BUMP_TURN_DIR=2;			//qz modify 20180502:原来为1,修改为2
						break;
	/********************
	发现充电座
	********************/			
					case DOCKMODE_STEP_FINDSEAT:			//	寻找充电座
						Find_Seat_My_MMHW();
					break;	
	/********************
	
	********************/	
					case DOCKMODE_STEP_CHARGE:
						stop_rap();
						if(giv_sys_time-seat_time>30000)
							{
#ifdef DOCK_DEBUG
								TRACE("enter in charge mode!\r\n");
#endif
								//Init_Chargeing(CHARGEING);

								//qz add 2018091
								Init_Chargeing(SEAT_CHARGING);
								return;
							}
						if((!power.charge_seat))
							{
								mode.step=DOCKMODE_STEP_RECHARGE;
								mode.step_mk=0;								
								dock_fail_count++;
#ifdef DOCK_DEBUG
								TRACE("power seat contact fail,go to DOCKMODE_STEP_RECHARGE!\r\n");
#endif
								Set_BS_Level(60);
							}
					break;
					case DOCKMODE_STEP_RECHARGE:
						Start_Recharge();
						if((power.charge_seat)||(power.charge_dc))
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_CHARGE;
								mode.step_mk=0;
								seat_time=giv_sys_time;
#ifdef DOCK_DEBUG
								TRACE("power seat contact!,go to DOCKMODE_STEP_CHARGE!\r\n");
#endif
								return;
							}
					break;
					case DOCKMODE_STEP_DOCKFAIL:		//qz add V2.3.8,增加这个步骤的目的是为了配合异常码一直上报，但回充失败无需一直上报
						switch(mode.step_mk)		//所以保持三秒钟回充失败上报，三秒后，进入待机
							{
								case 0:
									stop_rap();
//									error_code=SEND_ERROR_DOCKFAIL;
//									dis_err_code=DIS_ERROR_DOCK_FAIL;		//qz add 20180703
									Send_Voice(VOICE_DOCK_FAIL);
									dock_fail_time=giv_sys_time;
									mode.step_mk++;
									Open_Led(0,30000,2);					//qz add 20181120:红灯快闪3s
									break;
								case 1:
									if(giv_sys_time-dock_fail_time>10000)
										{
											REYBS_TIME=0;					//qz add 20180910
											Init_Cease();	
											dis_err_code=DIS_ERROR_DOCK_FAIL;	//qz add 20180710
											error_code=SEND_ERROR_DOCKFAIL;
											return;
										}
									break;
							}
						break;
					case DOCKMODE_STEP_REYBS:
						ReYBS2Dock();
						break;
					
					default :
	//						mode.step = 0;	 
						break;		
				}
}

//qz add 20180910
void ReYBS2Dock(void)
{
	switch(mode.step_mk)
		{
			case 0:
				REYBS_TIME++;
#ifdef DOCK_DEBUG
				TRACE("REYSB_TIME=%d\r\n",REYBS_TIME);
#endif
				if(REYBS_TIME>2)
					{
						FIND_SEAT_FAIL=true;
						mode.step_mk=2;
					}
				else
					mode.step_mk++;
				break;
			case 1:
				DOCK_SWEEP=true;
//				Init_Commander();
				delay_100us(100);		//延时10ms
				Slam_Data.dipan_req_pre=DIPAN_REQ_DOCK;
				Slam_Data.dipan_req=DIPAN_REQ_DOCKYBS;
#ifdef DOCK_DEBUG
				TRACE("Dipan Request ReYBS!\r\n");
#endif
				mode.step_mk++;
				break;
		}
}
//qz add end

u8 Find_Seat_My(void)
{
	static int stop_length,start_length;
	u32 l_speed,r_speed;
	float radius;

	switch (mode.step_mk)
		{
		case 4:
#ifdef DOCK_DEBUG
			TRACE("enter in DOCKMODE_STEP_FINDSEAT\r\n");
#endif
			if(rm_hw.effectRight)
				{
					
					mode.step_mk=0x40;
					findseat_skid_check_flag=true;
				}
#ifdef LEFT_DOCK
			else if((rm_hw.effectLeft))
				{
					mode.step_mk=0x10;
					findseat_skid_check_flag=true;
				}
			else if((rm_hw.effectMid))
				{
					mode.step_mk=0x50;
					findseat_skid_check_flag=true;
				}
			else
				{
					mode.step=DOCKMODE_STEP_START;
					mode.step_mk=0;
#ifdef DOCK_DEBUG
					TRACE("lost LEFT&RIGHT!,go to DOCKMODE_STEP_START!\r\n");
#endif
					findseat_skid_check_flag=false;

				}
#endif
			return 0;
		///////////////////////////////////////////////////
		///////////中红外找到左边信号得处理过程////////////
		case 0x10:
#ifdef	MIDMOVE_BUMP_ACTION
			WHICH_TO_MID=0;
			WHICH_TO_MID|=LEFT_TO_MID;
#endif
			enable_rap_no_length(FRONT,CONST_SPEED_800,FRONT,LOW_CONST_SPEED);		//稍偏右转
			mode.step_mk++;
			return 0;
		case 0x11:
			if(!rm_hw.effectLeft)								//脱离左信号
				{
					stop_rap();
					mode.step_mk++;
				}
			return 0;
		case 0x12:
			Speed=CONST_SPEED_600;
			if(do_action(3,8*CM_PLUS))	//原来为5cm				//直行
				{
					stop_rap();
					if(rm_hw.effectLeft)							//发现左信号，重新脱离
						mode.step_mk=0x10;
					else
						{
							//qz add 20180910
							l_speed=LOW_CONST_SPEED;r_speed=CONST_SPEED_800;
							radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);
							stop_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH);
							start_length=r_ring.all_length;
							//qz add end
							mode.step_mk++; 							//直行完毕，准备去step_mk=13
							return 0;
						}
				}
			
#ifdef MY20180412
			if((rm_hw.effectRight)&&(!rm_hw.effectLeft))			//直行中，脱离左信号且找到右信号
				{
					stop_rap();
					mode.step_mk=0x20;
				}			
			return 0;
		case 0x20:
			enable_rap_no_length(FRONT,LOW_CONST_SPEED,FRONT,CONST_SPEED_800);		//向左偏
			if(!rm_hw.effectRight)							//脱离右信号
				{
					stop_rap();
					mode.step_mk=0x21;
				}
			return 0;
		case 0x21:
			Speed=CONST_SPEED_600;
			if(do_action(3,7*CM_PLUS))						//直行
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			if(rm_hw.effectLeft)								//找到左信号
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			if(rm_hw.effectMid)								//找到中信号
				{
					stop_rap();
					mode.step_mk=0x50;
				}
#endif

			return 0;
		case 0x13:
			enable_rap_no_length(FRONT,LOW_CONST_SPEED,FRONT,CONST_SPEED_800);		//稍偏左转
			mode.step_mk++;
			return 0;
		#if 1
		case 0x14:
			//qz add 20180910	当旋转一周时都没有发现信号，从头开始
			if(r_ring.all_length-start_length>stop_length)
				{
					stop_rap();
					mode.step=DOCKMODE_STEP_START;
					mode.step_mk=0;
					return 0;
				}
			//qz add end
			if(rm_hw.effectLeft==1)										//接触左信号
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			else if(rm_hw.effectRight==1)
				{
					//stop_rap();
					//mode.step_mk=0x40;
				}
			else if(rm_hw.effectMid==1)
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=1;
				}
			else if((r_hw.effectLeft)||(b_hw.effectLeft))	//转弯中接收头没有接受到红外信号，已经严重左偏了，右红外可能会接到信号
				{
					stop_rap();
					mode.step_mk++;
				}
			return 0;
		case 0x15:
			Speed=CONST_SPEED;
			if(do_action(2,360*Angle_1))
				{
					stop_rap();
					mode.step=DOCKMODE_STEP_FINDSEAT;
					mode.step_mk=4;
#ifdef DOCK_DEBUG
					TRACE("rm_hw lost LEFT in findseat left,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif

				}
			if(rm_hw.effectLeft)
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			return 0;
		#endif
		
		//////////////////////////////////////////////////////////
		//////////////中红外找到右红外的处理过程//////////////////
		case 0x40:
#ifdef MIDMOVE_BUMP_ACTION
			WHICH_TO_MID=0;
			WHICH_TO_MID|=RIGHT_TO_MID;
#endif
			enable_rap_no_length(FRONT,LOW_CONST_SPEED,FRONT,CONST_SPEED_800);		//稍偏左转，脱离右信号
			mode.step_mk++;
			return 0;
		case 0x41:
			if(!rm_hw.effectRight)
				{
					stop_rap();
					mode.step_mk++;
					if((rm_hw.effectMid==1))//&(rm_hw.data==0x53)
						{
							mode.step_mk=0x50;
							MID_TURN=2;											//标记右转
						}
				}
			return 0;
		case 0x42:
			Speed=CONST_SPEED_600;
			if(do_action(3,8*CM_PLUS))		//原来为5cm
				{
					stop_rap();
					if(rm_hw.effectRight)	//qz add 20180428:直行完毕，发现右信号，重新脱离
						mode.step_mk=0x40;
					else
						{
							mode.step_mk++;
							//qz add 20180910
							l_speed=CONST_SPEED_800;r_speed=LOW_CONST_SPEED;
							radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);
							stop_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH); 	//最多偏转一周
							start_length=l_ring.all_length;
							return 0;
							//qz add end
						}
				}
			
#ifdef MY20180412
			if(rm_hw.effectLeft&(!rm_hw.effectRight))					//丢失右信号，找到左信号
				{
					stop_rap();
					mode.step_mk=0x30;
				}
			return 0;
		case 0x30:
			enable_rap_no_length(FRONT,CONST_SPEED_800,FRONT,LOW_CONST_SPEED);			//向右偏直到左信号消失
			if(!rm_hw.effectLeft)
				{
					stop_rap();
					mode.step_mk++;
				}
			break;
		case 0x31:
			Speed=CONST_SPEED_600;
			if(do_action(3,7*CM_PLUS))						//直行
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			if(rm_hw.effectRight)								//找到左信号
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			if(rm_hw.effectMid)								//找到中信号
				{
					stop_rap();
					mode.step_mk=0x50;
				}
#endif

			return 0;
		case 0x43:
			enable_rap_no_length(FRONT,CONST_SPEED_800,FRONT,LOW_CONST_SPEED);	//稍偏右转，接触右信号
			mode.step_mk++;
			return 0;
		case 0x44:
			//qz add 20180910
			if(l_ring.all_length-start_length>stop_length)
				{
					stop_rap();
					mode.step=DOCKMODE_STEP_START;
					mode.step_mk=0;
					return 0;
				}
			if((rm_hw.effectRight==1))
				{
					stop_rap();
					mode.step_mk=0x40;
//					if((rm_hw.effectTopReal==1)&&(rm_hw.data==0x5C));
//						mode.step_mk=0x50;
					if((rm_hw.effectMid==1))//&(rm_hw.data==0x53)
					{
						mode.step_mk=0x50;
					 	MID_TURN=2;		//1										//标记右转
					}
				}
			else if(rm_hw.effectLeft)
				{
				}
			else if(l_hw.effectRight)//中接收头没有收到信号，可能机器已经严重右偏，看左接收头能否接到右信号
				{
					stop_rap();
					mode.step_mk++;
				}
			return 0;
		case 0x45:
			Speed=CONST_SPEED;
			if(do_action(1,360*Angle_1))
				{
					stop_rap();
					mode.step=DOCKMODE_STEP_FINDSEAT;
					mode.step_mk=4;
#ifdef DOCK_DEBUG
					TRACE("rm_hw lost RIGHT in findseat right,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
				}
			if(rm_hw.effectRight)
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			return 0;
		//////////////////////////////////////////////////////
		///////////中红外找到中间信号的处理过程///////////////
		case 0x50:
			Speed=MID_CONST_SPEED;
#ifdef	MIDMOVE_BUMP_ACTION
			MIDMOVE_BUMP_FLAG=true;
#endif
			if(do_action(3,5*CM_PLUS))
				{
					if((rm_hw.effectMid==1))//&(rm_hw.data==0x53)
						mode.step_mk=0x50;
					else
						mode.step_mk=0x58;
					return 0;
				}
			#if 0
			if((rm_hw.effectRight==1)&&(rm_hw.data==0x5B))//&&(rm_hw.effectTop==0)
				{
//					enable_rap_no_length(FRONT,50,FRONT,CONST_SPEED);
//					mode.step_mk=0x52;
					l_rap.rap=200;
					r_rap.rap=300;
					return 0;
				}
			else if((rm_hw.effectLeft==1)&&(rm_hw.data==0x57))//&&(rm_hw.effectTop==0)
				{
//					enable_rap_no_length(FRONT,CONST_SPEED,FRONT,50);
//					mode.step_mk=0x55;
					r_rap.rap=200;
					l_rap.rap=300;
					return 0;
				}
			#endif
			if(!rm_hw.effectRight)
			{
				l_rap.rap=400;
				r_rap.rap=200;
			}
			else if(!rm_hw.effectLeft)
			{
				l_rap.rap=200;
				r_rap.rap=400;
			}
			else if((rm_hw.effectLeft)&&(rm_hw.effectRight)&&(rm_hw.effectMid))
				{
					r_rap.rap=MID_CONST_SPEED;
					l_rap.rap=MID_CONST_SPEED;
				}
			
			return 0;
#if 1
		case 0x51:
			#if 0
			if((rm_hw.effectRight==1)&&(rm_hw.data==0x5B))//&&(rm_hw.effectTop==0)
			{
				l_rap.rap=50;			//收到右信号，左轮减速，左转
				mode.step_mk=0x52;
				return 0;
			}
			if((rm_hw.effectLeft==1)&&(rm_hw.data==0x57))//&&(rm_hw.effectTop==0)
			{
				r_rap.rap=50;			//收到左信号，右轮减速，右转
				mode.step_mk=0x55;
				return 0;
			}
			#endif

			#if 0
			if((rm_hw.effectTopReal==0)&&(rm_hw.data!=0x5C)) 
				{
					//stop_rap();
					//mode.step_mk=0x58;
					if((rm_hw.effectRight==1)&&(rm_hw.data==0x5B))//&&(rm_hw.effectTop==0)
					{
						l_rap.rap=50;			//收到右信号，左轮减速，左转
						mode.step_mk=0x52;
						return 0;
					}
					else if((rm_hw.effectLeft==1)&&(rm_hw.data==0x57))//&&(rm_hw.effectTop==0)
					{
						r_rap.rap=50;			//收到左信号，右轮减速，右转
						mode.step_mk=0x55;
						return 0;
					}
					else if(l_ring.length>)
						{
							stop_rap();
							mode.step_mk=0x58;
						}
				}
			#endif
			
			return 0;
		case 0x52:
			if(rm_hw.effectRight==0)
				{
					stop_rap();
					mode.step_mk=0X42;//++;
				}
			return 0;
		case 0x53:
			enable_rap_no_length(FRONT,CONST_SPEED,FRONT,50);
			mode.step_mk++;
			return 0;
		case 0x54:
			if((rm_hw.effectMid==1))//&(rm_hw.data==0x53)
				{
					stop_rap();
					mode.step_mk=0x50;
				}
			return 0;
			
		case 0x55:
			if(rm_hw.effectLeft==0)
				{
					stop_rap();
					//mode.step_mk++;
					mode.step_mk=0x12;
				}
			return 0;
		case 0x56:
			l_rap.rap=CONST_SPEED;
			l_rap.sign=1;
			l_rap.ori=BACK;
			l_rap.length=l_ring.length+100000;
			r_rap.rap=CONST_SPEED;
			r_rap.sign=1;
			r_rap.ori=FRONT;
			r_rap.length=r_ring.length+100000;
			mode.step_mk++;
			return 0;
		case 0x57:
			if((rm_hw.effectMid==1))//&(rm_hw.data==0x53)
				{
					stop_rap();
					mode.step_mk=0x50;
				}
			return 0;
		//////////////////////////////////////////////////////
		////////之前中红外有中间信号，后来丢失处理////////////
		case 0x58:
			Speed=MID_CONST_SPEED;
			
#if MY20180412
			if(do_action(3,5*CM_PLUS))
				{
					stop_rap();
					if(rm_hw.effectLeft&rm_hw.effectRight)
						mode.step_mk=0x58;
					else
						mode.step_mk++;
				}
			if(rm_hw.effectMid)
				{
					stop_rap();
					mode.step_mk=0x50;
				}
			return 0;
		case 0x59:
			Speed=CONST_SPEED_600;
#endif

			if(do_action(MID_TURN,380*Angle_1))
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("rm_hw lost r/l/m signal,restart!\r\n");
#endif
					mode.step=0;
					mode.step_mk=0;
					findseat_skid_check_flag=false;
					return 0;
				}
			if((rm_hw.effectRight==1)&&(rm_hw.data==0x5B))
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			if((rm_hw.effectLeft==1)&&(rm_hw.data==0x57))
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			if((rm_hw.effectMid==1))//&(rm_hw.data==0x53)
				{
					stop_rap();
					mode.step_mk=0x50;
				}
			return 0;
		////////////////////////////////////////////////////
		/////////冲上充电座触发左碰撞以后得处理过程/////////
		case 0x60:
			Speed=MID_CONST_SPEED;
			if(do_action(2,500*Angle_1))
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("left bump action done,but l_hw no signal! restart\r\n");
#endif
					mode.step=0;
					mode.step_mk=0;
					return 0;
				}
			//if((l_hw.effectLeft==1)||(rm_hw.effectRight==1))		//左红外收到左信号或者右信号
			if(((l_hw.effectLeft)||(l_hw.effectRight)||(l_hw.effectMid))&&(!rm_hw.effectLeft)&&(!rm_hw.effectRight)&&(!rm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk++;
				}
			return 0;
		case 0x61:
			Speed=MID_CONST_SPEED;
			if(do_action(3,30*CM_PLUS))							//前进40公分	
				{
					stop_rap();
						{
							mode.step=0;
							mode.step_mk=0;
						}
				}
			if((l_hw.effectRight==1))
				{
					stop_rap();
					mode.step_mk++;
				}

			//qz add 20180403
			#if 0
			if((rm_hw.effectRight==1))
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			if(rm_hw.effectMid==1)
				{
					stop_rap();
					mode.step_mk=0x50;
				}
			#endif
			//qz add end

			return 0;
		case 0x62:
			Speed=MID_CONST_SPEED;
			if(do_action(1,500*Angle_1))
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("l_hw has signal,but rm_hw can't find left or mid signal,restart\r\n");
#endif		
					mode.step=0;
					mode.step_mk=0;
					return 0;
				}

			//qz add 20180330
			if((rm_hw.effectLeft==1))
				{
					stop_rap();
					mode.step_mk=0x10;
				}
//			if((rm_hw.effectRight==1))
//				{
//					stop_rap();
//					mode.step_mk=0x40;
//				}
			//qz add end
			if((rm_hw.effectMid==1))
				{
					stop_rap();
					mode.step_mk=0x50;
				}
#if 0
			if((rm_hw.effectRight==1))
				{
					stop_rap();
					mode.step_mk=0x40;
				}
#endif
			return 0;
		//////////////////////////////////////////
		////////右边冲上充电座处理动作////////////
		case 0x70:
			Speed=MID_CONST_SPEED;
			if(do_action(1,500*Angle_1))
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("right bump action done,but r_hw no signal.restart\r\n");
#endif
					mode.step=0;
					mode.step_mk=0;
					return 0;
				}

			if((r_hw.effectRight==1)||(r_hw.effectLeft==1))
				{
					stop_rap();
					mode.step_mk++;
				}
			return 0;
		case 0x71:
			Speed=MID_CONST_SPEED;
			if(do_action(3,30*CM_PLUS))
				{
					stop_rap();
						{
							mode.step=0;
							mode.step_mk=0;
						}
				}
			if(r_hw.effectLeft==1)
				{
					stop_rap();
					mode.step_mk++;
				}
			return 0;
		case 0x72:
			Speed=MID_CONST_SPEED;
			if(do_action(2,500*Angle_1))
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("r_hw has left signal,but rm_hw can't find right or mid signal,restart\r\n");
#endif		
					mode.step=0;
					mode.step_mk=0;
					return 0;
				}
			//qz add 20180330
//			if((rm_hw.effectLeft==1))
//				{
//					stop_rap();
//					mode.step_mk=0x10;
//				}
			if((rm_hw.effectRight==1))
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			//qz add end
			if(rm_hw.effectMid==1)
				{
					stop_rap();
					mode.step_mk=0x50;
				}
			return 0;
				
#endif   
				
		}
		return 0;
}

static void Left_Bump_Action(void)
{
	switch(mode.step_bp)
	{
		case 0:
			Speed=HIGH_FAST_SPEED;
			if(do_action(4,BACK_LENGTH*CM_PLUS)) 		//原来为25cm
				{
					stop_rap();
#ifdef MIDMOVE_BUMP_ACTION		//qz add 20180422
					mode.step_bp++;
				}
		break;
		case 1:
			Speed=HIGH_CONST_SPEED;
			if(do_action(2,100*Angle_1))
				{
					stop_rap();
					mode.step_bp++;
				}
#if 1
			if((l_hw.effectLeft))
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("l_hw find LEFT in %s, go to DOCKMODE_STEP_LEFT!\r\n",__func__);
#endif
					mode.bump=0;
					mode.step_bp=0;
					mode.bump_flag=false;
					mode.step=DOCKMODE_STEP_LEFT;
					mode.step_mk=0;
					return;
				}
#endif
			break;
		case 2:
			Speed=HIGH_CONST_SPEED;
			if(do_action(3,10*CM_PLUS))
				{
					stop_rap();
					mode.step_bp++;
				}
			if((l_hw.effectMid)||(l_hw.effectMidRight))
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("l_hw find MID or MIDRIGHT in %s!\r\n",__func__);
#endif
					mode.step_bp++;
				}
			break;
		case 3:
			Speed=HIGH_CONST_SPEED;
			if(do_action(1,360*Angle_1))	//原来为90:qz modify 20180428,原来为120:qz modify 20180615
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("rm_hw can't get MID in %s\r\n",__func__);
#endif
				#if 0
					mode.bump=0;
					mode.step_bp=0;
					mode.step=DOCKMODE_STEP_FINDSEAT;
					mode.step_mk=4;
					mode.bump_flag=false;
				#endif
					mode.step_bp++;
				}
			if(lm_hw.effectMid&rm_hw.effectMid) 	//qz modify 20180428:左碰撞，退下来找MID/RIGHT信号
				{
					stop_rap();
#endif
					mode.bump=0;
					mode.step_bp=0;
					mode.step=DOCKMODE_STEP_FINDSEAT;			//qz modify 20180422:原来为0x60,修改为此值
					mode.step_mk=4;
					mode.bump_flag=false;
#ifdef DOCK_DEBUG
					TRACE("rm_hw get MID,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
				}
			//qz add 20180615
			if(r_hw.effectMidLeft|r_hw.effectLeft)
				{
					stop_rap();
					mode.step_bp++;
#ifdef DOCK_DEBUG
					TRACE("rm_hw can't get MID in %s\r\n",__func__);
#endif
				}
			break;
		case 4:
			Speed=CONST_SPEED;
			if(do_action(2,360*Angle_1))
				{
					stop_rap();
					mode.step_bp++;
				}
			if(lm_hw.effectMidLeft|lm_hw.effectMidRight)
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("rm_hw get MIDL or MIDR,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
					mode.bump=0;
					mode.step_bp=0;
					mode.step=DOCKMODE_STEP_FINDSEAT;			//qz modify 20180422:原来为0x60,修改为此值
					mode.step_mk++;
					mode.bump_flag=false;
				}
			//qz add end
			break;
		case 5:
			mode.bump=0;
			mode.step_bp=0;
			mode.step=DOCKMODE_STEP_START;			//qz modify 20180422:原来为0x60,修改为此值
			mode.step_mk=0;
			mode.bump_flag=false;
#ifdef DOCK_DEBUG
			TRACE("rm_hw can't get MIDL or MIDR,go to DOCKMODE_STEP_START!\r\n");
#endif
			break;
	}
}


static void Right_Bump_Action(void)
{
	switch(mode.step_bp)
	{
		case 0:
			Speed=HIGH_FAST_SPEED;
			if(do_action(4,BACK_LENGTH*CM_PLUS)) 		//原来为25cm
				{
					stop_rap();
#ifdef MIDMOVE_BUMP_ACTION		//qz add 20180422
					mode.step_bp++;
				}
		break;
		case 1:
			Speed=HIGH_CONST_SPEED;
			if(do_action(1,100*Angle_1))
				{
					stop_rap();
					mode.step_bp++;
				}
#if 1
			if(r_hw.effectRight)
				{
					stop_rap();
					mode.bump=0;
					mode.step_bp=0;
					mode.bump_flag=false;
					mode.step=DOCKMODE_STEP_RIGHT;
					mode.step_mk=0;
#ifdef DOCK_DEBUG
					TRACE("r_hw find RIGHT in %s, go to DOCKMODE_STEP_RIGHT!\r\n",__func__);
#endif
				}
#endif
			break;
		case 2:
			Speed=HIGH_CONST_SPEED;
			if(do_action(3,10*CM_PLUS))
				{
					stop_rap();
					mode.step_bp++;
				}
			if((r_hw.effectMid)||(r_hw.effectMidLeft))
				{
					stop_rap();
					mode.step_bp++;
#ifdef DOCK_DEBUG
					TRACE("r_hw find MID or MIDLEFT in %s!\r\n",__func__);
#endif
				}
			break;
		case 3: 						
			Speed=HIGH_CONST_SPEED;
			if(do_action(2,360*Angle_1))	//原来为90:qz modify 20180428,原来为120:qz modify 20180615
				{
					stop_rap();
					mode.step_bp++;
					#if 0
					mode.bump=0;
					mode.step_bp=0;
					mode.step=DOCKMODE_STEP_FINDSEAT;
					mode.step_mk=4;
					mode.bump_flag=false;
					#endif
#ifdef DOCK_DEBUG
					TRACE("rm_hw can't get MID in %s\r\n",__func__);
#endif
				}
			if(rm_hw.effectMid&lm_hw.effectMid)	//qz modify 20180428:右碰撞，退下来找MID/LEFT信号
				{
					stop_rap();
#endif
					mode.bump=0;
					mode.step_bp=0;
					mode.step=DOCKMODE_STEP_FINDSEAT;			//qz modify 20180422:原来为0x60,修改为此值
					mode.step_mk=4;
					mode.bump_flag=false;
#ifdef DOCK_DEBUG
					TRACE("rm_hw get MID!,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
				}

			//qz add 20180615
			if(l_hw.effectRight|l_hw.effectMidRight)		//左红外发现MID|RIGHT信号，说明中红外找信号不成功，反转
				{
					stop_rap();
					mode.step_bp++;
#ifdef DOCK_DEBUG
					TRACE("rm_hw can't get MID in %s\r\n",__func__);
#endif
				}
			break;
		case 4:
			Speed=CONST_SPEED;
			if(do_action(1,360*Angle_1))
				{
					stop_rap();
					#if 0
					mode.bump=0;
					mode.step_bp=0;
					mode.step=DOCKMODE_STEP_FINDSEAT;			//qz modify 20180422:原来为0x60,修改为此值
					mode.step_mk=4;
					mode.bump_flag=false;
					#endif
					mode.step_bp++;
				}
			if(rm_hw.effectMid|lm_hw.effectMid)
				{
					stop_rap(); 
					mode.bump=0;
					mode.step_bp=0;
					mode.step=DOCKMODE_STEP_FINDSEAT;			//qz modify 20180422:原来为0x60,修改为此值
					mode.step_mk=4;
					mode.bump_flag=false;
#ifdef DOCK_DEBUG
					TRACE("rm_hw get MIDL or MIDR,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
				}
			//qz add end
		break;
		case 5:
			mode.bump=0;
			mode.step_bp=0;
			mode.step=DOCKMODE_STEP_START;			//qz modify 20180422:原来为0x60,修改为此值
			mode.step_mk=0;
			mode.bump_flag=false;
#ifdef DOCK_DEBUG
			TRACE("rm_hw can't get MIDL or MIDR,go to DOCKMODE_STEP_START!\r\n");
#endif
			break;

	}
}

void Docking_Bump_My(void)
{
	static u32 bump_stop_length,start_length,end_length;
	u32 l_speed,r_speed,turn_ang;
	float radius=0;

//#ifndef MIDMOVE_BUMP_ACTION		//qz mask 20180422
	if(mode.step==DOCKMODE_STEP_FINDSEAT)
//#else								//qz mask 20180422
//	if(MIDMOVE_BUMP_FLAG)			//qz mask 20180422
//#endif							//qz mask 20180422
		{
		switch (mode.bump)
			{
				case 90:
					switch (mode.step_bp)
						{
							case 0:
								//qz add 20180422
#ifdef MIDMOVE_BUMP_ACTION
								if(!MIDMOVE_BUMP_FLAG)
									{
										mode.step_bp=4;
										break;
									}
#endif
								Speed=300;
								if(do_action(4,1*CM_PLUS))
									{
										stop_rap();
										mode.step_bp++;
									}
							break;
							case 1:
								Speed=400;
								if(do_action(1,20*Angle_1))
									{
										stop_rap();
										mode.step_bp++;
									}
							break;
							case 2:
								Speed=400;
								if(do_action(2,40*Angle_1))
									{
										stop_rap();
										mode.step_bp++;
									}
							break;
							case 3:
								Speed=400;
								if(do_action(1,20*Angle_1))
									{
										stop_rap();
										mode.step_bp++;
									}
							break;
							case 4:
								Speed=HIGH_FAST_SPEED;
								if(do_action(4,BACK_LENGTH*CM_PLUS))			//原来为25cm
									{
										stop_rap();										
										//qz add 20180418
										dock_fail_count++;
										//qz add end
#ifdef MIDMOVE_BUMP_ACTION
										mode.step_bp=5;
										mode.bump=90;
										
									}
							break;
							case 5:
								Speed=HIGH_CONST_SPEED;
								if(do_action(WHICH_TO_MID,90*Angle_1))
									{
										stop_rap();
										mode.step_bp=6;
									}
							break;
							case 6:
								Speed=HIGH_CONST_SPEED;
								if(do_action(3,10*CM_PLUS))			//原来为5cm
									{
										stop_rap();
										mode.step_bp=7;
									}
							break;
							case 7:
								Speed=HIGH_CONST_SPEED;
								if(do_action((~WHICH_TO_MID)&0x03,120*Angle_1))		//qz modify 100->150 20180620
									{
										stop_rap();
										MIDMOVE_BUMP_FLAG=false;
#endif
										mode.step_bp=0;
										mode.bump=0;
										mode.step=DOCKMODE_STEP_FINDSEAT;		//从头再来
										mode.step_mk=4;
										mode.bump_flag=false;
#ifdef DOCK_DEBUG
										TRACE("MidBump action complete!,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
									}
							break;
						}
				break;
				case 5:
				case BUMP_LEFT_MID:
				case BUMP_ONLY_LEFTMID:
					Left_Bump_Action();
					//Left_Bump_Action_II();
				break;
				case 6:
				case BUMP_RIGHT_MID:
				case BUMP_ONLY_RIGHTMID:
					Right_Bump_Action();
					//Right_Bump_Action_II();
				break;
#ifdef FINDSEAT_SKID_CHECK
				case 91:
					switch(mode.step_bp)
						{
							case 0:
								stop_rap();
								mode.step_bp++;
							break;
							case 1:
								Speed=HIGH_FAST_SPEED;
								if(do_action(4,BACK_LENGTH*CM_PLUS))
									{
										stop_rap();
										mode.step=DOCKMODE_STEP_START;
										mode.step_mk=0;
										mode.bump=0;
										mode.step_bp=0;
										mode.bump_flag=false;
									}
								break;
						}
					break;
#endif
			}
		}

	//else if(mode.step==DOCKMODE_STEP_TOP_SPOT)				//只有TOP信号的情况，定点螺旋行走碰撞处理过程
	else if((mode.step==DOCKMODE_STEP_TOP)|(mode.step==DOCKMODE_STEP_TOP_SPOT))	
		{
			switch (mode.bump)
				{
					case 90:
					case 5:
					case 6:
					case BUMP_ONLY_LEFTMID:
					case BUMP_LEFT_MID:
					case BUMP_ONLY_RIGHTMID:
					case BUMP_RIGHT_MID:
						#if 0
						switch (mode.step_bp)
							{
								case 0:
									Speed=CONST_SPEED;
									if(do_action(4,4*CM_PLUS))
										{
											stop_rap();
											mode.step_bp++;
										}
									break;
								case 1:
									if(do_action(2,180*Angle_1))
										{
											stop_rap();
											mode.bump=0;
											mode.step_bp=0;
											if(mode.step_mk==1)
												mode.step_mk=3;
											else if(mode.step_mk==3)
												mode.step_mk=1;
										}
									break;
							}
						#endif
						switch(mode.step_bp)
							{
						
								case 0:
									Speed=2400;
									if(do_action(4,5*CM_PLUS))
										{
						
											stop_rap();
											mode.step_bp++;
											return;
										}
									return;
								case 1:
									if(top_piv_left)
										top_bump_turn=2;
									else
										top_bump_turn=1;
									mode.step_bp++;
									return;
								case 2:
									Speed=2400;
									if(do_action(top_bump_turn,180*Angle_1))
										{
											stop_rap();
											mode.bump=0;
											mode.step_bp=0;
											mode.bump_flag=false;
											#if 0
											if(top_piv_left)
												top_piv_left=0;
											else
												top_piv_left=1;
											Speed=1000;
											#endif
											TOP_DIR=!TOP_DIR;
											top_turn_cnt++;
											if(top_turn_cnt>=6)
												top_turn_cnt=0;
											mode.step_mk=3;
										}
								
							}
				}
		}
 	else
		{
		switch (mode.step)
			{
				case DOCKMODE_STEP_MID:
					switch(mode.bump)
						{
							case 90:
							case 5:
							case 6:
							case BUMP_ONLY_LEFTMID:
							case BUMP_LEFT_MID:
							case BUMP_ONLY_RIGHTMID:
							case BUMP_RIGHT_MID:
								switch(mode.step_bp)
									{
										case 0:
											Speed=1000;
											if(do_action(4,4*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 1:
											Speed=1000;
											if(do_action(2,360*Angle_1))
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.bump_flag=false;
													mode.step=DOCKMODE_STEP_START;
													mode.step_mk=0;
													return;
												}
											if(rm_hw.effectMidLeft|rm_hw.effectMidRight|lm_hw.effectMidLeft|lm_hw.effectMidRight)
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.bump_flag=false;
													mode.step=DOCKMODE_STEP_FINDSEAT;
													mode.step_mk=4;
													return;
												}
											break;
									}
						}
					break;
				case DOCKMODE_STEP_RIGHT:
					switch(mode.bump)
						{
							case 90:
							case 5:
							case 6:
							case BUMP_ONLY_LEFTMID:
							case BUMP_LEFT_MID:
							case BUMP_ONLY_RIGHTMID:
							case BUMP_RIGHT_MID:
								
								switch (mode.step_bp)
									{
										case 0:
											Speed=1000;
											if(do_action(4,4*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 1:
											Speed=1000;
											if(do_action(2,90*Angle_1))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 2:
											Speed=1000;
											if(do_action(3,5*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 3:
											l_speed=500;
											r_speed=1500;
											radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);
											end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/4);	//准备向左偏转，停止位置为与碰撞发生时的位置处于同一水平线，即偏转90度
											start_length=r_ring.all_length;
											mode.step_bp++;
											break;
										case 4:
											l_speed=500;
											r_speed=1500;
											enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);
											if(r_ring.all_length-start_length>end_length)			//已绕过障碍
												{
													stop_rap(); 				
													mode.bump=0;
													mode.step_bp=0;
													mode.step_mk=0;
													mode.bump_flag=false;
													return;
												}
											if(rm_hw.effectMidLeft|l_hw.effectMidLeft|r_hw.effectMidLeft|lm_hw.effectMidLeft\
												|rm_hw.effectLeft|l_hw.effectLeft|r_hw.effectLeft|lm_hw.effectLeft) 	//
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.step_mk=5;
													mode.bump_flag=false;
													return;
												}
											if(rm_hw.effectMid|l_hw.effectMid|r_hw.effectMid|lm_hw.effectMid)
												{
													stop_rap();
													mode.step_bp=90;
												}
											if((!l_bump.key)|(!r_bump.key)|(!lm_bump.key)|(!rm_bump.key))
												{
		
													stop_rap();
													bump_stop_length=r_ring.all_length-start_length;
													mode.step_bp++;
												}
											break;
										case 5:
											Speed=1000;
											if(do_action(4,4*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 6:
											Speed=1000;
											turn_ang=(u32)(90*bump_stop_length/end_length);
											if(turn_ang<15)
												turn_ang=15;
											if(do_action(2,turn_ang*Angle_1))
												{
													stop_rap();
													mode.step_bp=2;
												}
											break;
											
										case 90:
											Speed=1000;
											if(do_action(2,360*Angle_1))
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.step=DOCKMODE_STEP_TOP;
													mode.step_mk=0;
													mode.bump_flag=false;
												}
											if(rm_hw.effectMid&lm_hw.effectMid)
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.step=DOCKMODE_STEP_FINDSEAT;
													mode.step_mk=0X50;
													mode.bump_flag=false;
												}
											break;
									}
						}
					break;
				case DOCKMODE_STEP_LEFT:
					switch(mode.bump)
						{
							case 90:
							case 5:
							case 6:
							case BUMP_ONLY_LEFTMID:
							case BUMP_LEFT_MID:
							case BUMP_ONLY_RIGHTMID:
							case BUMP_RIGHT_MID:
								switch (mode.step_bp)
									{
										case 0:
											Speed=1000;
											if(do_action(4,4*CM_PLUS))		//后退
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 1:
											Speed=1000;
											if(do_action(1,90*Angle_1))		//左转
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 2:
											Speed=1000;
											if(do_action(3,5*CM_PLUS))		//前进
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 3:								//准备向右偏转走
											l_speed=1500;
											r_speed=500;
											radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);
											end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/4);	//准备向右偏转，停止位置为与碰撞发生时的位置处于同一水平线，即偏转90度
											start_length=l_ring.all_length;
											mode.step_bp++;
											break;
										case 4:
											l_speed=1500;
											r_speed=500;
											enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);
											if(l_ring.all_length-start_length>end_length)	//向右偏转走完成
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.step_mk=0;
													mode.bump_flag=false;
													return;
												}
											if(rm_hw.effectRight|l_hw.effectRight|r_hw.effectRight|lm_hw.effectRight\
												|rm_hw.effectMidRight|lm_hw.effectMidRight|l_hw.effectMidRight|r_hw.effectMidRight)		
												{											//中途发现右信号或右中信号
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.step_mk=5;
													mode.bump_flag=false;
													return;
												}
											if(rm_hw.effectMid|l_hw.effectMid|r_hw.effectMid|lm_hw.effectMid)//中途发现中信号
												{
													stop_rap();
													mode.step_bp=90;
												}
											if((!l_bump.key)|(!r_bump.key)|(!lm_bump.key)|(!rm_bump.key))
												{
		
													stop_rap();
													bump_stop_length=l_ring.all_length-start_length;
													mode.step_bp++;
												}
											break;
										case 5:
											Speed=1000;
											if(do_action(4,4*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 6:
											Speed=1000;
											turn_ang=(u32)(90*bump_stop_length/end_length);
											if(turn_ang<15)
												turn_ang=15;
											if(do_action(1,turn_ang*Angle_1))
												{
													stop_rap();
													mode.step_bp=2;
												}
											break;
											
										case 90:
											Speed=1000;
											if(do_action(1,360*Angle_1))
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.step=DOCKMODE_STEP_START;
													mode.step_mk=0;
													mode.bump_flag=false;
												}
											if(rm_hw.effectMid&lm_hw.effectMid)
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.step=DOCKMODE_STEP_FINDSEAT;
													mode.step_mk=0X50;
													mode.bump_flag=false;
												}
											break;
									}
						}				
					break;
				case DOCKMODE_STEP_START:
					if((mode.step_mk==3)|(mode.step_mk==5))
						{
							switch(mode.bump)
							{
							case 90:
							case 5:						//左碰撞
							case BUMP_ONLY_LEFTMID:
							case BUMP_LEFT_MID:
								switch (mode.step_bp)
									{
										case 0:
											Speed=1000;
											if(do_action(4,5*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 1:
											Speed=1000;
											if(do_action(2,90*Angle_1))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 2:
											Speed=1000;
											if(do_action(3,5*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 3:
											l_speed=500;
											r_speed=1500;
											radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);
											end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/4);	//准备向左偏转，停止位置为与碰撞发生时的位置处于同一水平线，即偏转90度
											start_length=r_ring.all_length;
											mode.step_bp++;
											break;
										case 4:
											l_speed=500;
											r_speed=1500;
											enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);
											if(r_ring.all_length-start_length>end_length)			//已绕过障碍
												{
													stop_rap(); 				
													mode.bump=0;
													mode.step_bp=0;
													mode.bump_flag=false;
													mode.step=DOCKMODE_STEP_START;
													mode.step_mk=0;
													return;
												}
											if((!l_bump.key)|(!r_bump.key)|(!lm_bump.key)|(!rm_bump.key))
												{
													stop_rap();
													bump_stop_length=r_ring.all_length-start_length;
													mode.step_bp++;
												}
											break;
										case 5:
											Speed=1000;
											if(do_action(4,4*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 6:
											Speed=1000;
											turn_ang=(u32)(90*bump_stop_length/end_length);
											if(turn_ang<15)
												turn_ang=15;
											if(do_action(2,turn_ang*Angle_1))
												{
													stop_rap();
													mode.step_bp=2;
												}
											break;
									}
								break;
							case 6:
							case BUMP_ONLY_RIGHTMID:
							case BUMP_RIGHT_MID:
								switch (mode.step_bp)
									{
										case 0:
											Speed=1000;
											if(do_action(4,5*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 1:
											Speed=1000;
											if(do_action(1,90*Angle_1))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 2:
											Speed=1000;
											if(do_action(3,5*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 3:
											l_speed=1500;
											r_speed=500;
											radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);
											end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/4);	//准备向右偏转，停止位置为与碰撞发生时的位置处于同一水平线，即偏转90度
											start_length=l_ring.all_length;
											mode.step_bp++;
											break;
										case 4:
											l_speed=1500;
											r_speed=500;
											enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);
											if(l_ring.all_length-start_length>end_length)
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.bump_flag=false;
													mode.step=DOCKMODE_STEP_START;
													mode.step_mk=0;
													return;
												}
											if((!l_bump.key)|(!r_bump.key)|(!lm_bump.key)|(!rm_bump.key))
												{
											
													stop_rap();
													bump_stop_length=l_ring.all_length-start_length;
													mode.step_bp++;
												}
											break;
										case 5:
											Speed=1000;
											if(do_action(4,4*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 6:
											Speed=1000;
											turn_ang=(u32)(90*bump_stop_length/end_length);
											if(turn_ang<15)
												turn_ang=15;
											if(do_action(1,turn_ang*Angle_1))
												{
													stop_rap();
													mode.step_bp=2;
												}
											break;
									}
									break;
								}
						}
					else 
						{
							switch(mode.bump)
								{
								case 5:
								case 6:
								case 90:
								case BUMP_ONLY_LEFTMID:
								case BUMP_ONLY_RIGHTMID:
								case BUMP_LEFT_MID:
								case BUMP_RIGHT_MID:
									switch(mode.step_bp)
										{
											case 0:
												Speed=2500;
												if(do_action(4,4*CM_PLUS))
													{
														stop_rap();
														mode.step_bp++;
													}
												break;
											case 1:
												if((!l_bump.key)|(!r_bump.key)|(!lm_bump.key)|(!rm_bump.key))
													{
														stop_rap();
														mode.step_bp=0;
													}
												else
													{
														stop_rap();
														mode.step_bp=0;
														mode.bump=0;
														mode.bump_flag=false;
														if(mode.step_mk==1)
															mode.step_mk=0;
													}
												break;
										}
									break;
								}
						}
					break;
				case DOCKMODE_STEP_RECHARGE:
					switch(mode.bump)
						{
							case 5:
							case 6:
							case 90:
							case BUMP_ONLY_LEFTMID:
							case BUMP_ONLY_RIGHTMID:
							case BUMP_LEFT_MID:
							case BUMP_RIGHT_MID:
								switch(mode.step_bp)
									{
										case 0:
											if(do_action(4,4*CM_PLUS))
												{
													stop_rap();
													mode.step_bp++;
												}
											break;
										case 1:
											if((!l_bump.key)|(!r_bump.key)|(!lm_bump.key)|(!rm_bump.key))
												{
													stop_rap();
													mode.bump=0;
													mode.step_bp=0;
													mode.bump_flag=false;
													mode.step=DOCKMODE_STEP_TOP;
													mode.step_mk=0;
													//b_hw.flag=0;
													//rm_hw.flag=0;
												}
											else
												{
													stop_rap();
													mode.step_bp=0;
												}
											break;
									}
								break;
						}
					break;
				case DOCKMODE_STEP_CHARGE:

					switch (mode.step_bp)
						{
							case 0:

								Speed=500;
								if(do_action(4,3*CM_PLUS))
									{
										stop_rap();
										mode.bump=0;
										mode.step_bp=0;
										mode.bump_flag=false;
									}
								break;
						}
			}
	}
}

void Start_Mid(void)

{
	static u8 Start=0;
	switch (mode.step_mk)
		{
			case 0:
				Speed=HIGH_CONST_SPEED;						//qz modify 201801515:原来为300
				if(do_action(2,360*Angle_1))			
					{
						stop_rap();						//后红外旋转360度完成
						switch (Start)
							{
							case 0:
								if(TOP_FLAG)
									{
#ifdef DOCK_DEBUG
										TRACE("b_hw can't find LEFT&RIGHT signal!\r\n");
										TRACE("but TOP signal still exist!Go to top_start!\r\n");
#endif
										mode.step=DOCKMODE_STEP_TOP;
										mode.step_mk=0;
										TOP_FLAG=false;
										return;
									}
								else
									{
#ifdef DOCK_DEBUG
										TRACE("lost all signal!  Enter Cease Mode!\r\n");
#endif
										TOP_FLAG=false;
										FIND_SEAT_FAIL=true;
										return;
									}
///								break;
							case 1:
#ifdef DOCK_DEBUG	
								TRACE("b_hw only can find LEFT signal!Go to Start_Left!\r\n");
#endif
								mode.step=DOCKMODE_STEP_LEFT;
								mode.step_mk=0;
								Start=0;
								TOP_FLAG=false;
								return;
							case 2:
#ifdef DOCK_DEBUG				
								TRACE("b_hw only can find RIGHT signal!Go to Start_Right\r\n");
#endif
								mode.step=DOCKMODE_STEP_RIGHT;
								mode.step_mk=0;
								TOP_FLAG=false;
								Start=0;
								return;
							}
					}
				if((b_hw.effectLeft)||(b_hw.effectRight))
					{
						if(b_hw.effectLeft)
							Start|=0x01;
						if(b_hw.effectRight)
							Start|=0x02;						
					}
				if(find_home&ALL_TOP_ONLY)
					TOP_FLAG=true;
				if(Start==0x03)
					{
						stop_rap();
						mode.step_mk++;
						Start=0;
					}
				break;
			case 1:
				Speed=HIGH_FAST_SPEED;				//qz modify 201801515:原来为300
				if(do_action(3,20*CM_PLUS))		//qz mask 20180515:原来为15cm
					{
						stop_rap();
						mode.step_mk++;
					}
				break;
			case 2:
				Speed=HIGH_CONST_SPEED;
				if(do_action(1,360*Angle_1))
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("rm_hw lost left and right signal! goto mode_step_start\r\n");
#endif
						mode.step=0;
						mode.step_mk=0;
					}
				if((rm_hw.effectLeft)||(rm_hw.effectRight))
					{
						stop_rap();
#ifdef DOCK_DEBUG		
						TRACE("rm_hw find right or left signal!  goto mode_step_findseat!\r\n");
#endif
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;
					}
		}
}

void Start_Top_Spot_My(void)
{
	static u32 stop_l_length,last_l_length,stop_r_length,last_r_length;
	u32 l_sp_const=2000,r_sp_const=2000,data1;
	//u32 r_sp_turn[6]={500,1000,1500,2000,2500,2800};
	u32 r_sp_turn[6]={200,600,1000,1200,1500,1800};
	//u32 l_sp_turn[6]={500,1000,1500,2000,2500,2800};
	u32 l_sp_turn[6]={200,600,1000,1200,1500,1800};
	float radius;
	switch (mode.step_mk)
		{
			case 0:
				Speed=1000;
				if(do_action(2,360*Angle_1))		//qz modify 20180910:720--->360
				{
					stop_rap();
					//FIND_SEAT_FAIL=true;			//qz mask 20180910
					mode.step=DOCKMODE_STEP_START;		//qz add 20180910
					mode.step_mk=0;					//qz add 20180910
#ifdef DOCK_DEBUG
					TRACE("all hw lost TOP in %s\r\n",__func__);
					TRACE("go back to mode_step_start!\r\n");
#endif								
				}
			if(find_home)
				{
					stop_rap();
					mode.step_mk=3;
				}
			break;
			case 3:
				if(!TOP_DIR)	//向右偏转
					{
						radius=(float)(RING_RANGE*r_sp_turn[top_turn_cnt])/(l_sp_const-r_sp_turn[top_turn_cnt]);
						stop_l_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH);
						last_l_length=l_ring.all_length;
						mode.step_mk++;
#ifdef DOCK_DEBUG
						TRACE("radius=%f\r\n",radius);
						TRACE("stop_l_length=%d\r\n",stop_l_length);
#endif
						break;

					}
				else		//向左偏转
					{
						radius=(float)(RING_RANGE*l_sp_turn[top_turn_cnt])/(r_sp_const-l_sp_turn[top_turn_cnt]);
						stop_r_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH);
						last_r_length=r_ring.all_length;
						mode.step_mk++;
#ifdef DOCK_DEBUG
						TRACE("radius=%f\r\n",radius);
						TRACE("stop_r_length=%d\r\n",stop_r_length);
#endif
						break;

					}
			case 4:
				if(!TOP_DIR)	//向右偏转
					{
						enable_rap_no_length(FRONT,l_sp_const,FRONT,r_sp_turn[top_turn_cnt]);		//左轮恒定速度,向右螺旋转弯
						if(l_ring.all_length-last_l_length>stop_l_length)
						{
							stop_rap();
							top_turn_cnt++;
							if(top_turn_cnt>=6)
								top_turn_cnt=0;
							mode.step_mk=3;
						}
					}
				else
					{
						enable_rap_no_length(FRONT,l_sp_turn[top_turn_cnt],FRONT,r_sp_const);		//左轮恒定速度,向右螺旋转弯
						if(r_ring.all_length-last_r_length>stop_r_length)
						{
							stop_rap();
							top_turn_cnt++;
							if(top_turn_cnt>=6)
								top_turn_cnt=0;
							mode.step_mk=3;
						}
					}

				//data1=find_home&ALL_TOP_MASK;
				data1=find_home;
				if(data1)
					{
						if((data1&ALL_MIDLEFT_ONLY)&(data1&ALL_MIDRIGHT_ONLY))
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_MID;
								mode.step_mk=0;
#ifdef DOCK_DEBUG
								TRACE("find_home=0x%x\r\n",data1);
								TRACE("find mid area,go to step mid\r\n");
#endif
							}
						else if((data1&ALL_MIDLEFT_ONLY)|(data1&ALL_LEFT_ONLY))
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_LEFT;
								mode.step_mk=0;
#ifdef DOCK_DEBUG
								TRACE("find_home=0x%x\r\n",data1);
								TRACE("find left,go to step left\r\n");
#endif
								return;
							}
						else if((data1&ALL_RIGHT_ONLY)|(data1&ALL_MIDRIGHT_ONLY))
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_RIGHT;
								mode.step_mk=0;
#ifdef DOCK_DEBUG
								TRACE("find_home=0x%x\r\n",data1);
								TRACE("find right,go to step right\r\n");
#endif
								return;
							}
					}
				break;
		}
}
void Start_Recharge(void)
{
	switch (mode.step_mk)
		{
			case 0:
				Speed=LOW_CONST_SPEED;					//左转一下
				if(do_action(1,5*Angle_1))
					{
						stop_rap();
						mode.step_mk++;
					}
			break;
			case 1:
				Speed=LOW_CONST_SPEED;					//右转一下
				if(do_action(2,10*Angle_1))
					{
						stop_rap();
						mode.step_mk++;
					}
			break;
			case 2:
				Speed=LOW_CONST_SPEED;					//回正
				if(do_action(1,5*Angle_1))
					{
						stop_rap();
						mode.step_mk++;
					}
			break;

			case 3:
				Speed=HIGH_CONST_SPEED;						//后退
				if(do_action(4,40*CM_PLUS))
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;
					}
			break;
		}
}


s16 Cal_Angle(s16 angle_1,s16 angle_2)
{

	s16 temp_angle;
	temp_angle=angle_2-angle_1;
	if(temp_angle<0)
		temp_angle+=360;
	return (temp_angle);
}

void Start_Right_Deflect_Turn(void)
{
	static bool b_r_flag=false;//nearby=false;
	static bool r_rm_flag=false;
	static bool mm_r_flag=false;
	static int stop_length,start_length;
	u32 l_speed,r_speed;
	float radius;

	if(r_hw.effectMidRight)
		r_rm_flag=true;

	switch (mode.step_mk)
		{
			case 0:
				Speed=HIGH_CONST_SPEED;
				if(do_action(2,360*Angle_1))					//右转
					{
						stop_rap();
#ifdef	DOCK_DEBUG
						TRACE("r_hw can't catch RIGHT\r\n");
#endif
//						mode.step=DOCKMODE_STEP_TOP;
//						mode.step_mk=0;
						mode.step_mk=15;						//qz add 20180703:用于右红外和中红外都没有找到RIGHT信号的解决
						return;
					}
				if((rm_hw.effectMid)&(lm_hw.effectMid))
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;
						r_rm_flag=false;
						return;
					}
				if(r_hw.effectRight|r_hw.effectMidRight)							//右红外找到右信号						
				//if((r_hw.effectRight|r_hw.effectMidRight)&(!rm_hw.effect)&(!l_hw.effect))
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("r_hw get R or RM!\r\n");
#endif
						mode.step_mk=40;
						//mode.step_mk++;
					}

#ifdef DOCK_NEAR
				if(find_home&0xF0000)
					{
						nearby=true;
					}
#endif
				
				break;
			case 40:
				Speed=HIGH_CONST_SPEED;//CONST_SPEED_600;				//qz modify 20181210 1000-->600
				if(do_action(2,360*Angle_1))
					{
						stop_rap(); 					//qz add 20181210
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						r_rm_flag=false;
						return;
					}
				if((!r_hw.effectRight)&(!r_hw.effectMidRight))
					{
						stop_rap();
						mode.step_mk++;
					}
				mm_r_flag=false;
				break;
			case 41:
				Speed=HIGH_CONST_SPEED;//CONST_SPEED_600;				//qz modify 20181210 1000-->600
				if(do_action(2,480*Angle_1))
					{
						stop_rap(); 					//qz add 20181210
#ifdef DOCK_DEBUG
						TRACE("r_hw can't get R|RM!\r\n");
#endif
						if(mm_r_flag)
							{
#ifdef DOCK_DEBUG
								TRACE("go to step_mk 50!\r\n");
#endif
								mode.step_mk=50;
							}
						else
							{
#ifdef DOCK_DEBUG
								TRACE("goto DOCKMODE_STEP_START!\r\n");
#endif
								mode.step=DOCKMODE_STEP_START;
								mode.step_mk=0;
								r_rm_flag=false;
							}
						mm_r_flag=false;
						return;
					}
				if((r_hw.effectRight)|(r_hw.effectMidRight))
					{
						stop_rap();
						mode.step_mk=1;
						mm_r_flag=false;
					}
				if(lm_hw.effectMidRight|lm_hw.effectRight)
					{
						mm_r_flag=true;
					}
				break;
					

			/////////右红外发现RIGHT信号处理过程//////
			case 1:
				Speed=HIGH_CONST_SPEED;//CONST_SPEED_800;				//qz modify 20181210 1000-->600
				if(do_action(3,100*CM_PLUS))					//qz modify 20180910:400--->100
					{
						stop_rap();
#ifdef	DOCK_DEBUG
						TRACE("r_hw or b_hw can't find any LEFT,go to start!\r\n");
#endif
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						r_rm_flag=false;
					}
				//if(find_home&LEFTMIDLEFT_ONLY)								//发现左信号
				//if(r_hw.effectLeft|r_hw.effectMidLeft)
				if(r_hw.effectMidLeft)
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("find_home=0x%x\r\n",find_home);
						TRACE("get LM signal!\r\n");
#endif
						mode.step_mk=4;
						return;
					}
				if((!r_hw.effectRight)&(!r_hw.effectMidRight))							//右红外右信号丢失
					{											//准备向右偏转行走
						//stop_rap();
#if 0
						mode.step_mk++;
						//l_speed=HIGH_CONST_SPEED;r_speed=MID_CONST_SPEED;
						//l_speed=800;r_speed=400;
						l_speed=800;r_speed=700;
						radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);
						stop_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH);		//最多偏转一周
						start_length=l_ring.all_length;
#else
						if(r_rm_flag)
							{
								mode.step_mk=2;
								//l_speed=MID_CONST_SPEED;r_speed=HIGH_CONST_SPEED;
								l_speed=800;r_speed=400;
								//l_speed=800;r_speed=600;
								radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);
								stop_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH); 	//最多偏转一周
								start_length=l_ring.all_length;
								return;
							}
						mode.step_mk=101;
						start_length=l_ring.all_length;
					}
				//qz add 20181225
				if(r_hw.effectLeft)
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("r_hw get L signal!\r\n");
						TRACE("go to step_mk 30\r\n");
#endif
						mode.step_mk=30;			
						return;
					}
				//qz add end
				break;
			case 101:
				//l_speed=CONST_SPEED_700;r_speed=CONST_SPEED_800;
				l_speed=CONST_SPEED_800;r_speed=HIGH_CONST_SPEED;
				enable_rap_no_length(FRONT,l_speed,FRONT, r_speed);
				if(l_ring.all_length>start_length+8*CM_PLUS)
					{
#ifdef	DOCK_DEBUG
						TRACE("go forward ok!\r\n");
#endif
						mode.step_mk=2;
						//l_speed=MID_CONST_SPEED;r_speed=HIGH_CONST_SPEED;
						l_speed=800;r_speed=400;
						//l_speed=800;r_speed=600;
						radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);
						stop_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH);		//最多偏转一周
						start_length=l_ring.all_length;
					}
				if(r_hw.effectRight|r_hw.effectMidRight)
					{
						action.sign=0;
						l_ring.length=0;
						r_ring.length=0;
						mode.step_mk=1;
						return;
					}
				break;
#endif
			/////////右红外丢失RIGHT信号,向右偏转处理过程//////
			case 2:
				//l_speed=HIGH_CONST_SPEED;r_speed=MID_CONST_SPEED;
				l_speed=800;r_speed=400;
				//l_speed=800;r_speed=700;
				enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);		//向左偏转
				if(l_ring.all_length-start_length>stop_length)			//偏转一周完成，还没有找到左信号，从头再来
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("r_hw can't get signal!go back to DOCKMODE_STEP_START!\r\n");
						TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
						mode.step=DOCKMODE_STEP_START;						//qz modify DOCKMODE_STEP_TOP_SPOT
						mode.step_mk=0;
						r_rm_flag=false;
						return;
					}
				//if(r_hw.effectMidLeft|r_hw.effectLeft)								//右红外找到左信号，按正常处理
				if(r_hw.effectMidLeft)
					{
						stop_rap();
#ifdef DOCK_DEBUG		
						TRACE("r_hw get L|LM in step_mk 2!\r\n");
						TRACE("goto step_mk 4!\r\n");
#endif
						mode.step_mk=4;
						return;
					}
				if(l_hw.effectMidLeft|l_hw.effectLeft)								//左红外发现左信号，机器姿态：其实已经右偏
					{
						stop_rap();
#ifdef DOCK_DEBUG		
						TRACE("l_hw get L|LM signal in DOCKMODE_STEP_RIGHT!\r\n");
						TRACE("Goto DOCKMODE_STEP_LEFT!\r\n");
#endif
						//mode.step_mk=7;			//qz mask 20181225
						mode.step=DOCKMODE_STEP_LEFT;	//qz add 20181225
						mode.step_mk=40;
						r_rm_flag=false;
						return;
					}
				if((rm_hw.effectMid)&(lm_hw.effectMid))
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("rm_hw get M signal!go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;					
						r_rm_flag=false;
						return;
					}
				if(l_hw.effectRight|l_hw.effectMidRight)			//中左红外发现右信号，机器姿态：其实已经严重右偏
					{
						stop_rap();
#ifdef DOCK_DEBUG		
						TRACE("l_hw get R|RM in step_mk 2!\r\n");
						TRACE("goto step_mk 0!\r\n");
#endif
						mode.step_mk=0;							//重新去step_mk0开始
						return;
					}
				if(r_hw.effectRight|r_hw.effectMidRight)							//右红外发现右信号
					{
						//stop_rap();
						action.sign=0;
						l_ring.length=0;
						r_ring.length=0;
						mode.step_mk=1;
					}
				if(r_hw.effectLeft)
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("r_hw get L signal!\r\n");
						TRACE("go to step_mk 30\r\n");
#endif
						mode.step_mk=30;			//qz add 20181220
						return;
					}
				break;
			//右红外发现LEFT信号处理过程
			case 4:
				Speed=HIGH_CONST_SPEED;
				if(do_action(3,3*CM_PLUS))								//直行	//qz modify 10cm->6cm 20180522
					{
						stop_rap();
						mode.step_mk++;
#ifdef DOCK_NEAR
						if(nearby)
							{
								TRACE("It's in Near area!\r\n");
								return;
							}
#endif
					}
				break;
			case 5:
				Speed=CONST_SPEED_600;
				if(do_action(2,360*Angle_1))							//右转360
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;						
						r_rm_flag=false;
#ifdef DOCK_DEBUG
						TRACE("rm_hw can't get M siganl! l_hw can't get R\r\n");
						TRACE("go back to DOCKMODE_STEP_START!r\n");
#endif
						return;
					}
				if((rm_hw.effectMid)&(lm_hw.effectMid))
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;						
						r_rm_flag=false;
#ifdef DOCK_DEBUG
						TRACE("rm_hw get M!go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif						
						return;
					}
				//if(l_hw.effectRight|l_hw.effectMidRight)			//qz mask 20181225		//左信号发现LEFT|RIGHT信号,表示机器右偏,需要调整
				if(l_hw.effectMidLeft|l_hw.effectLeft)						//qz add 20181225
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("l_hw get R|RM in LEFT step_mk 5!\r\n");
						TRACE("go to step_mk 6,Perpare LEFT Turn!");
#endif
						mode.step_mk++;									//go to step_mk 6
						return;
					}
				break;
			case 6:
				Speed=CONST_SPEED;					
				if(do_action(1,360*Angle_1))							//低速左转,利于中红外再次寻找信号
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;						
						r_rm_flag=false;
#ifdef DOCK_DEBUG
						TRACE("rm_hw can't get M&RM siganl!R\r\n");
						TRACE("go back to DOCKMODE_STEP_START!r\n");
#endif
						return;
					}
				//if(lm_hw.effectMidRight|lm_hw.effectMidLeft)					//中红外发现LEFT|RIGHT,go to DOCKMODE_STEP_FINDSEAT
				if(lm_hw.effectMid|lm_hw.effectLeft|lm_hw.effectRight|lm_hw.effectMidLeft|lm_hw.effectMidRight)	//qz modify 20181225
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("rm_hw.effectMid=%d\r\n",rm_hw.effectMid);
						TRACE("lm_hw.effectMid=%d\r\n",lm_hw.effectMid);
						TRACE("lm_hw.effectMidRight=%d\r\n",lm_hw.effectMidRight);
						TRACE("rm_hw get LEFT|RIGHT!go to DOCKMODE_STEP_FINDSEAT!\r\n");
						TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
						
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;					
						r_rm_flag=false;
						return;
					}
				break;

		//左红外发现左信号处理过程
			case 7:
				Speed=CONST_SPEED_800;
				if(do_action(1,360*Angle_1))			//向左旋转
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						r_rm_flag=false;
						return;
					}
				if(r_hw.effectMidLeft|r_hw.effectLeft)						//如果右红外发现LEFT信号，正面中红外角度不好，没有发现
					{
						stop_rap();
						mode.step_mk++;
						return;
					}
				if((rm_hw.effectMid)&(lm_hw.effectMid))
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("rm_hw.effectMid=%d\r\n",rm_hw.effectMid);
						TRACE("lm_hw.effectMid=%d\r\n",lm_hw.effectMid);
						TRACE("lm_hw.effectMidRight=%d\r\n",lm_hw.effectMidRight);
						TRACE("rm_hw get LEFT|RIGHT!go to DOCKMODE_STEP_FINDSEAT!\r\n");
						TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;
						r_rm_flag=false;
						return;
					}
				break;
			case 8:
				Speed=CONST_SPEED;
				if(do_action(2,360*Angle_1))			//低速向右旋转
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						r_rm_flag=false;
						return;
					}
				if((lm_hw.effectMidRight)|lm_hw.effectMidLeft)					//中红外发现LEFT|RIGHT,go to DOCKMODE_STEP_FINDSEAT
					{
						stop_rap();
#ifdef DOCK_DEBUG
						TRACE("rm_hw.effectMid=%d\r\n",rm_hw.effectMid);
						TRACE("lm_hw.effectMid=%d\r\n",lm_hw.effectMid);
						TRACE("lm_hw.effectMidRight=%d\r\n",lm_hw.effectMidRight);
						TRACE("rm_hw get LEFT|RIGHT!go to DOCKMODE_STEP_FINDSEAT!\r\n");
						TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;
						r_rm_flag=false;
						return;
					}
				break;
			case 12:
				Speed=HIGH_CONST_SPEED;
				if(do_action(2,ADDANGLE*Angle_1))
					{
						stop_rap();
						mode.step_mk=3;
					}
				break;

			//qz add 20180703
			//针对后红外可以发现RIGHT信号，但是右红外无法发现的问题处理
			//不做处理的话，将会导致一直原地转圈。
			case 15:
				Speed=CONST_SPEED;
				if(do_action(2,360*Angle_1))			//旋转360度
					{
						stop_rap();
						if(b_r_flag)					//后红外发现过RIGHT
							{
								mode.step_mk=17;
								b_r_flag=false;
							}
						else							//M_HW/R_HW/B_HW都找不到右信号
							{
								mode.step_mk=20;
							}
					}
				if(rm_hw.effectRight|rm_hw.effectMidRight)					//中红外发现RIGHT
					{
						stop_rap();
						mode.step_mk++;
					}
				if(r_hw.effectRight|r_hw.effectMidRight)					//右红外发现RIGHT
					{
						stop_rap();
						mode.step_mk=1;
					}
				if(b_hw.effectRight|b_hw.effectMidRight)					//后红外发现RIGHT
					{
						b_r_flag=true;
					}
				if(l_hw.effectLeft|l_hw.effectMidLeft)						//左红外发现LEFT
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_LEFT;
						mode.step_mk=0;
						r_rm_flag=false;
#ifdef DOCK_DEBUG
						TRACE("L_HW can get left signal in DOCKMODE_STEP_LEFT!\r\n");
						TRACE("go to DOCKMODE_STEP_LEFT\r\n");
#endif
					}
				break;
			case 16:		//M_HW能找到RIGHT信号，
				Speed=HIGH_CONST_SPEED;
				if(do_action(3,20*CM_PLUS))
					{
						stop_rap();
						//mode.step_mk++;
						mode.step=DOCKMODE_STEP_START;						//qz modify DOCKMODE_STEP_TOP_SPOT
						mode.step_mk=0;
						r_rm_flag=false;
					}
				#if 0
				if(find_home)
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_TOP_SPOT;
						mode.step_mk=0;
					}
				#endif
				break;
			case 17:		//B_HW能找到RIGHT信号
				Speed=CONST_SPEED;
				if(do_action(2,360*Angle_1))
					{
						stop_rap();
						//mode.step=DOCKMODE_STEP_TOP_SPOT;
						//mode.step_mk=0;
						mode.step_mk=20;
						return;
					}
				if(b_hw.effectRight|b_hw.effectMidRight)
					{
						stop_rap();
						mode.step_mk++;
					}
				if(l_hw.effectRight|l_hw.effectMidRight)
					{
						stop_rap();
						mode.step_mk=20;
					}
				break;
			case 18:
				Speed=CONST_SPEED;
				if(do_action(2,360*Angle_1))
					{
						//qz add 20180910
						stop_rap();
						mode.step_mk=20;
						return;
						//qz add 20180910
					}
				if((!b_hw.effectRight)&(!b_hw.effectMidRight))
					{
						stop_rap();
						mode.step_mk++;
					}
				break;
			case 19:
				Speed=CONST_SPEED;
				if(do_action(2,180*Angle_1))
					{
						stop_rap();
						mode.step_mk++;
					}
				break;
			case 20:
				Speed=HIGH_CONST_SPEED;
				if(do_action(3,15*CM_PLUS))
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						b_r_flag=false;
						r_rm_flag=false;
					}
				break;
			case 30:

				Speed=CONST_SPEED_600;
				if(do_action(1,360*Angle_1))
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						r_rm_flag=false;
						return;
					}
				if(lm_hw.effectMid|lm_hw.effectLeft|lm_hw.effectRight|lm_hw.effectMidLeft|lm_hw.effectMidRight)
					{
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;
						r_rm_flag=false;
						return;
					}
				break;
			case 50:
				Speed=CONST_SPEED_800;
				if(do_action(2,360*Angle_1))

					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						r_rm_flag=false;
					}
				if(lm_hw.effectRight|lm_hw.effectMidRight)
					{
						stop_rap();
						mode.step_mk++;
					}

				break;
			case 51:
				Speed=CONST_SPEED_800;
				if(do_action(3,10*CM_PLUS))
					{
						stop_rap();
						mode.step_mk=0;
					}
				break;
			//qz add end
		}
}

void Start_Left_Deflect_Turn(void)
{
	static bool b_l_flag=false;
	static bool l_lm_flag=false;
	static bool mm_l_flag=false;
	static int stop_length,start_length;
	u32 l_speed,r_speed;
	float radius;
	u8 data1=0;

	if(l_hw.effectMidLeft)
		l_lm_flag=true;
		switch (mode.step_mk)
			{
				case 0:
					l_lm_flag=false;
					Speed=HIGH_CONST_SPEED;
					if(do_action(1,360*Angle_1))
						{
							stop_rap();
#ifdef	DOCK_DEBUG
							TRACE("l_hw can't catch LEFT\r\n");
#endif
//							mode.step=DOCKMODE_STEP_TOP;
//							mode.step_mk=0;
							mode.step_mk=15;		//qz add 20180703:用于左红外没有找到LEFT,和中红外都没有找到RIGHT信号的解决
							return;
						}
					if((rm_hw.effectMid)&(lm_hw.effectMid))
						{
							stop_rap();
#ifdef DOCK_DEBUG	
							TRACE("rm_hw get M signal!go to mode_step_findseat!\r\n");
#endif
							mode.step=DOCKMODE_STEP_FINDSEAT;
							mode.step_mk=4;
							l_lm_flag=false;
							return;
						}
					if(l_hw.effectLeft|l_hw.effectMidLeft)
						{
							stop_rap();
							mode.step_mk=40;		//抗反射干扰
							//mode.step_mk++;
						}
#ifdef DOCK_NEAR
					if(find_home&0xF0000)
						{
							nearby=true;
						}
#endif
					break;
				case 40:
					//Speed=CONST_SPEED_600;				//qz modify 20181210 1000-->600
					Speed=HIGH_CONST_SPEED;
					if(do_action(1,360*Angle_1))
						{
							stop_rap(); 					//qz add 20181210
							mode.step=DOCKMODE_STEP_START;
							mode.step_mk=0;
							l_lm_flag=false;
							return;
						}
					if((!l_hw.effectLeft)&(!l_hw.effectMidLeft))
						{
							stop_rap();
							mode.step_mk++;
						}
					mm_l_flag=false;
					break;
				case 41:
					Speed=HIGH_CONST_SPEED;
					if(do_action(1,480*Angle_1))
						{
							stop_rap(); 					//qz add 20181210
#ifdef DOCK_DEBUG
							TRACE("l_hw can't get L|LM!\r\n");
#endif
							if(mm_l_flag)
								{
#ifdef DOCK_DEBUG
									TRACE("go to step_mk 50!\r\n");
#endif
									mode.step_mk=50;
								}
							else
								{
#ifdef DOCK_DEBUG
									TRACE("goto DOCKMODE_STEP_START!\r\n");
#endif
									mode.step=DOCKMODE_STEP_START;
									mode.step_mk=0;
									l_lm_flag=false;
								}
							mm_l_flag=false;
						}
					if(l_hw.effectLeft|l_hw.effectMidLeft)
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("l_hw get L|LM!\r\n");
#endif
							mode.step_mk=1;
						}
					if(lm_hw.effectLeft|lm_hw.effectMidLeft)
						{
							mm_l_flag=true;
						}
					break;
				case 1:
					//Speed=CONST_SPEED_800;
					Speed=HIGH_CONST_SPEED;
					if(do_action(3,100*CM_PLUS))		//qz modify 20180910:400--->100
						{
							stop_rap();
#ifdef	DOCK_DEBUG
							TRACE("r_hw or b_hw can't find any LEFT,go to start!\r\n");
#endif
							mode.step=DOCKMODE_STEP_START;
							mode.step_mk=0;
							l_lm_flag=false;
						}
					//if(find_home&RIGHTMIDRIGHT_ONLY)			//发现右信号
					//if(l_hw.effectRight|l_hw.effectMidRight)
					if(l_hw.effectMidRight)
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("find_home=0x%x\r\n",find_home);
							TRACE("get RM signal!\r\n");
#endif
							mode.step_mk=4;
						}
					if((!l_hw.effectLeft)&(!l_hw.effectMidLeft))							//左红外LEFT信号丢失,准备向左偏转
						{
//							stop_rap();
#if 0	//qz mask 20181220
							mode.step_mk++;
							//l_speed=MID_CONST_SPEED;r_speed=HIGH_CONST_SPEED;
							//l_speed=400;r_speed=800;
							l_speed=700;r_speed=800;
							radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);
							stop_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH);
							start_length=r_ring.all_length;
#else
							//qz add 20181220
							if(l_lm_flag)
								{
									mode.step_mk=2;
									//l_speed=MID_CONST_SPEED;r_speed=HIGH_CONST_SPEED;
									l_speed=400;r_speed=800;
									//l_speed=600;r_speed=800;
									radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);
									stop_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH);
									start_length=r_ring.all_length;
									return;
								}
							mode.step_mk=101;
							start_length=r_ring.all_length;
						}
					//qz add 20181225
					if(l_hw.effectRight)
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("l_hw get R signal!\r\n");
							TRACE("go to step_mk 30\r\n");
#endif
							mode.step_mk=30;			//qz add 20181220
							return;
						}
					//qz add end
					break;
				case 101:
					//l_speed=CONST_SPEED_800;r_speed=CONST_SPEED_700;
					l_speed=HIGH_CONST_SPEED;r_speed=CONST_SPEED_800;
					enable_rap_no_length(FRONT,l_speed,FRONT, r_speed);
					if(r_ring.all_length>start_length+8*CM_PLUS)
						{
#ifdef	DOCK_DEBUG
							TRACE("go forward ok!\r\n");
#endif
							mode.step_mk=2;
							//l_speed=MID_CONST_SPEED;r_speed=HIGH_CONST_SPEED;
							l_speed=400;r_speed=800;
							//l_speed=600;r_speed=800;
							radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);
							stop_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH);
							start_length=r_ring.all_length;
						}
					if(l_hw.effectLeft|l_hw.effectMidLeft)
						{
							action.sign=0;
							l_ring.length=0;
							r_ring.length=0;
							mode.step_mk=1;
							return;
						}
					break;
#endif
				case 2:
					//l_speed=MID_CONST_SPEED;r_speed=HIGH_CONST_SPEED;
					l_speed=400;r_speed=800;
					//l_speed=600;r_speed=800;
					enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);		//向左偏转
					if(r_ring.all_length-start_length>stop_length)
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("l_hw can't get signal!go back to DOCKMODE_STEP_START!\r\n");
							TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
							mode.step=DOCKMODE_STEP_START;						//qz modify DOCKMODE_STEP_TOP_SPOT
							mode.step_mk=0;
							l_lm_flag=false;
							return;
						}
					//if(l_hw.effectRight|l_hw.effectMidRight)							//左红外收到右信号
					if(l_hw.effectMidRight)
						{
							stop_rap();
#ifdef DOCK_DEBUG	
							TRACE("l_hw get RM!goto step_mk 4!\r\n");
#endif
							mode.step_mk=4;
							return;
						}
					if(r_hw.effectRight|r_hw.effectMidRight) 							//右红外发现右信号，机器姿态：其实已经右偏
						{
							stop_rap();
							//mode.step_mk=7;				//qz mask 20181225
#ifdef DOCK_DEBUG
							TRACE("r_hw get R signal!Goto DOCKMODE_STEP_RIGHT!\r\n");
#endif
							mode.step=DOCKMODE_STEP_RIGHT;		//qz add 20181225
							mode.step_mk=40;
							l_lm_flag=false;
							return;
						}
					if((rm_hw.effectMid)&(lm_hw.effectMid))
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("lm_hw get MID!go to DOCKMODE_STEP_FINDSEAT!\r\n");
							TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
							mode.step=DOCKMODE_STEP_FINDSEAT;
							mode.step_mk=4;
							l_lm_flag=false;
							return;
						}
					if(r_hw.effectLeft|r_hw.effectMidLeft)				//中右红外发现LEFT信号，机器姿态：其实已经严重左偏
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("r_hw get L|LM signal,motion is right avertence!\r\n");
							TRACE("go back to step_mk 0!\r\n");
#endif
							mode.step_mk=0; 						//重新去step_mk0开始
							return;
						}
					if(l_hw.effectLeft|l_hw.effectMidLeft)								//左红外发现左信号
						{
							//stop_rap();
							action.sign=0;
							l_ring.length=0;
							r_ring.length=0;
							mode.step_mk=1;
						}
					if(l_hw.effectRight)
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("l_hw get R signal!\r\n");
							TRACE("go to step_mk 30\r\n");
#endif
							mode.step_mk=30;			//qz add 20181220
							return;
						}
					break;
				case 4:
					Speed=CONST_SPEED_800;
					data1=3;
#ifdef DOCK_NEAR
					if(nearby)
					{
						data1=6;
						Speed=HIGH_CONST_SPEED;
					}
#endif
					if(do_action(3,data1*CM_PLUS))								//直行	//qz modify 10cm->6cm 20180522
						{
							stop_rap();
							mode.step_mk++;
#ifdef DOCK_NEAR
							if(nearby)
								{
									TRACE("It's in Near area!\r\n");
									return;
								}
#endif
						}
					break;
				case 5:
					Speed=CONST_SPEED_600;
					if(do_action(1,360*Angle_1))
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("rm_hw can't get M siganl! l_hw can't get R\r\n");
							TRACE("go back to DOCKMODE_STEP_START!r\n");
#endif
							
							mode.step=DOCKMODE_STEP_START;
							mode.step_mk=0;
							l_lm_flag=false;
							return;
						}
					//右红外发现左/左中信号，表示中红外没有找到中信号，需要调整
					if(r_hw.effectMidRight|r_hw.effectRight)		//qz modify 20181225
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("r_hw get L|LM in LEFT step_mk 5!\r\n");
							TRACE("go to step_mk 6,Perpare Right Turn!");
#endif
							mode.step_mk++;
							return;
						}
					if((rm_hw.effectMid)&(lm_hw.effectMid))
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("rm_hw get M!go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
							mode.step=DOCKMODE_STEP_FINDSEAT;
							mode.step_mk=4;
							l_lm_flag=false;
							return;
						}
					break;
				//右红外找到左/左中信号的，表示机器没有对准的调整过程
				case 6:
					Speed=CONST_SPEED;
					if(do_action(2,360*Angle_1))
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("rm_hw can't get M&RM siganl!R\r\n");
							TRACE("go back to DOCKMODE_STEP_START!r\n");
#endif
							mode.step=DOCKMODE_STEP_START;
							mode.step_mk=0;
							l_lm_flag=false;
							return;
						}
					//if(lm_hw.effectMidRight|lm_hw.effectMidLeft)					//中红外发现LEFT|RIGHT,go to DOCKMODE_STEP_FINDSEAT
					if(lm_hw.effectMid|lm_hw.effectLeft|lm_hw.effectRight|lm_hw.effectMidLeft|lm_hw.effectMidRight)		//qz modify 20181225
						{
							stop_rap();
#ifdef DOCK_DEBUG
							TRACE("rm_hw.effectMid=%d\r\n",rm_hw.effectMid);
							TRACE("lm_hw.effectMid=%d\r\n",lm_hw.effectMid);
							TRACE("lm_hw.effectMidRight=%d\r\n",lm_hw.effectMidRight);
							TRACE("rm_hw get LEFT|RIGHT!go to DOCKMODE_STEP_FINDSEAT!\r\n");
							TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
							
							mode.step=DOCKMODE_STEP_FINDSEAT;
							mode.step_mk=4;
							l_lm_flag=false;
							return;
						}
					break;
			
				//右红外发现右信号处理过程
				case 7:
						Speed=CONST_SPEED_800;
						if(do_action(1,360*Angle_1))
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_START;
								mode.step_mk=0;
								l_lm_flag=false;
								return;
							}
						if(l_hw.effectRight|l_hw.effectMidRight)
							{
								stop_rap();
								mode.step_mk++;
								return;
							}
						if((rm_hw.effectMid)&(lm_hw.effectMid))
							{
								stop_rap();
#ifdef DOCK_DEBUG
								TRACE("rm_hw.effectMid=%d\r\n",rm_hw.effectMid);
								TRACE("lm_hw.effectMid=%d\r\n",lm_hw.effectMid);
								TRACE("lm_hw.effectMidRight=%d\r\n",lm_hw.effectMidRight);
								TRACE("rm_hw get LEFT|RIGHT!go to DOCKMODE_STEP_FINDSEAT!\r\n");
								TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
								mode.step=DOCKMODE_STEP_FINDSEAT;
								mode.step_mk=4;
								l_lm_flag=false;
								return;
							}
						break;
				case 8:
						Speed=CONST_SPEED;
						if(do_action(2,360*Angle_1))
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_START;
								mode.step_mk=0;
								l_lm_flag=false;
								return;
							}
						if((rm_hw.effectMid&lm_hw.effectMid)|(lm_hw.effectMidLeft|lm_hw.effectMidRight))					//中红外发现LEFT|RIGHT,go to DOCKMODE_STEP_FINDSEAT
							{
								stop_rap();
#ifdef DOCK_DEBUG
								TRACE("rm_hw.effectMid=%d\r\n",rm_hw.effectMid);
								TRACE("lm_hw.effectMid=%d\r\n",lm_hw.effectMid);
								TRACE("lm_hw.effectMidRight=%d\r\n",lm_hw.effectMidRight);
								TRACE("rm_hw get LEFT|RIGHT!go to DOCKMODE_STEP_FINDSEAT!\r\n");
								TRACE("mode.step_mk=%d\r\n",mode.step_mk);
#endif
								
								mode.step=DOCKMODE_STEP_FINDSEAT;
								mode.step_mk=4;
								l_lm_flag=false;
								return;
							}
						break;
				case 12:
					Speed=HIGH_CONST_SPEED;
					if(do_action(1,ADDANGLE*Angle_1))
						{
							stop_rap();
							mode.step_mk=3;
						}
					break;
					//qz add 20180703
					//针对后红外可以发现LEFT信号，但是右红外无法发现的问题处理
					//不做处理的话，将会导致一直原地转圈。
						case 15:
						Speed=CONST_SPEED;
						if(do_action(2,360*Angle_1))
							{
								stop_rap();
								if(b_l_flag)
									{
										mode.step_mk=17;
										b_l_flag=false;
									}
								else
									{
										mode.step_mk=20;
									}
							}
						if(lm_hw.effectLeft|lm_hw.effectMidLeft)
							{
								stop_rap();
								mode.step_mk++;
							}
						if(l_hw.effectLeft|l_hw.effectMidLeft)
							{
								stop_rap();
								mode.step_mk=1;
							}
						if(b_hw.effectLeft|b_hw.effectMidLeft)
							{
								b_l_flag=true;
							}
						if(r_hw.effectRight|r_hw.effectMidRight)
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_RIGHT;
								mode.step_mk=0;
								l_lm_flag=false;
#ifdef DOCK_DEBUG
								TRACE("R_HW can get RIGHT signal in DOCKMODE_STEP_RIGHT!\r\n");
								TRACE("go to DOCKMODE_STEP_RIGHT\r\n");
#endif
							}
						break;
					case 16:							//中红外可以找到LEFT信号，直行
						Speed=HIGH_CONST_SPEED;
						if(do_action(3,20*CM_PLUS))
							{
								stop_rap();
								//mode.step_mk++;
								mode.step=DOCKMODE_STEP_START;						//qz modify DOCKMODE_STEP_TOP_SPOT
								mode.step_mk=0;
								l_lm_flag=false;
							}
						#if 0
						if(find_home)
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_TOP_SPOT;
								mode.step_mk=0;
							}
						#endif
						break;
					case 17:							//后红外可以找到LEFT信号
						Speed=CONST_SPEED;
						if(do_action(2,360*Angle_1))
							{
								stop_rap();
								b_l_flag=false;
								//mode.step=DOCKMODE_STEP_TOP_SPOT;
								//mode.step_mk=0;
								mode.step_mk=20;
								return;
							}
						if(b_hw.effectLeft|b_hw.effectMidLeft)
							{
								stop_rap();
								mode.step_mk++;
							}
						if(r_hw.effectLeft|r_hw.effectMidLeft)
							{
								stop_rap();
								mode.step_mk=20;
							}
						break;
					case 18:
						Speed=CONST_SPEED;
						if(do_action(2,360*Angle_1))
							{
							}
						if((!b_hw.effectLeft)&(!b_hw.effectMidLeft))
							{
								stop_rap();
								mode.step_mk++;
							}
					case 19:
						Speed=CONST_SPEED;
						if(do_action(2,180*Angle_1))
							{
								stop_rap();
								mode.step_mk++;
							}
						break;
					case 20:
						Speed=HIGH_CONST_SPEED;
						if(do_action(3,15*CM_PLUS))
							{
								stop_rap();
								mode.step=DOCKMODE_STEP_START;
								mode.step_mk=0;
								b_l_flag=false;
								l_lm_flag=false;
							}
						break;
			case 30:

				Speed=CONST_SPEED_600;
				if(do_action(1,360*Angle_1))
					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						l_lm_flag=false;
						return;
					}
				if(lm_hw.effectMid|lm_hw.effectLeft|lm_hw.effectRight|lm_hw.effectMidLeft|lm_hw.effectMidRight)
					{
						mode.step=DOCKMODE_STEP_FINDSEAT;
						mode.step_mk=4;
						l_lm_flag=false;
						return;
					}
				break;
			case 50:
				Speed=CONST_SPEED_800;
				if(do_action(2,360*Angle_1))

					{
						stop_rap();
						mode.step=DOCKMODE_STEP_START;
						mode.step_mk=0;
						l_lm_flag=false;
					}
				if(lm_hw.effectRight|lm_hw.effectMidRight)
					{
						stop_rap();
						mode.step_mk++;
					}

				break;
			case 51:
				Speed=CONST_SPEED_800;
				if(do_action(3,10*CM_PLUS))
					{
						stop_rap();
						mode.step_mk=0;
					}
				break;
					//qz add end
	}
}

void YBS_Find_Seat(void)
{
	u32 data1=0;
	if(SLAM_DOCK)
		{
			find_home=ReadHwSign_My();
			if(find_home)			//有NEAR|RIGHT|LEFT|MID|M_TOP信号
			{
				{
					stop_rap();
#ifdef DOCK_DEBUG						
					TRACE("find_home=0x%x\r\n",data1);
					TRACE("goto Little Dock\r\n"); 
#endif
					Init_Docking();
					return;
				}
			}
		}
}


//qz add 20180814
//在冠唯现场发现骑上充电座，没接触电极，也没动，也没触发碰撞
//采用万向轮方法避免情况的发生。
u8 Find_Seat_Skid_Check(void)
{
	switch (findseat_skid_check_step)
		{
			case 0:
				if(findseat_skid_check_flag)
					{
						findseat_skid_check_length=w_ring.length;
						findseat_skid_check_time=giv_sys_time;
						findseat_skid_check_step++;
					}
				else
					findseat_skid_check_step=0;
				break;
			case 1:
				if(!findseat_skid_check_flag)
					findseat_skid_check_step=0;
				else
					{
						if(giv_sys_time-findseat_skid_check_time>100000)
							{
								if(w_ring.length<findseat_skid_check_length+2)
									{
										findseat_skid_check_step=0;
										return 1;
									}
								else
									findseat_skid_check_step=0;
							}
					}
				break;
		}
	return 0;
}

#ifdef SLAM_FIND_SEAT
void Slam_Find_Seat(void)
{
	static u8 check_step=0;
	u32 data1=0;
	if(SLAM_DOCK)
		{
			data1=ReadHwSign_My();
			if((data1&=0X0AAAA)&(!check_step))
				{
					mode.Info_Abort=1;
					stop_rap();
					check_step++;
				}
			switch (check_step)
				{
					case 0:
						break;
					case 1:
						Speed=1500;
						if(do_action(2,360*Angle_1))
							{
								stop_rap();
								check_step++;
							}
						if(l_hw.effectLeft|r_hw.effectRight);
				}


			clr_hw_effect(&l_hw);
			clr_hw_effect(&rm_hw);
			clr_hw_effect(&b_hw);
			clr_hw_effect(&r_hw);
		}
}
#endif

u8 Find_Seat_My_MMHW(void)
{
		static int l_speed,r_speed;
		switch (mode.step_mk)
		{
		case 4:
#ifdef DOCK_DEBUG
			TRACE("enter in DOCKMODE_STEP_FINDSEAT\r\n");
#endif
			if((lm_hw.effectMid))
				{
					mode.step_mk=0x50;
				}
			else if((lm_hw.effectRight|lm_hw.effectMidRight))
				{					
					mode.step_mk=0x40;
				}
#ifdef LEFT_DOCK
			else if(((lm_hw.effectLeft|lm_hw.effectMidLeft)))
				{
					mode.step_mk=0x10;
				}
			else
				{
					mode.step=DOCKMODE_STEP_START;
					mode.step_mk=0;
#ifdef DOCK_DEBUG
					TRACE("lost LEFT&RIGHT!,go to DOCKMODE_STEP_START!\r\n");
#endif
				}
#endif
			return 0;
		///////////////////////////////////////////////////
		///////////中红外找到左边信号得处理过程////////////
		case 0x10:
#ifdef	MIDMOVE_BUMP_ACTION
			WHICH_TO_MID=0;
			WHICH_TO_MID|=LEFT_TO_MID;
#endif
			enable_rap_no_length(FRONT,CONST_SPEED_600,FRONT,CONST_SPEED_200);//稍偏右转
			mode.step_mk++;
			return 0;
#if 1
		case 0x11:
			//if((!lm_hw.effectLeft))
			if((!lm_hw.effectLeft)&(!lm_hw.effectMidLeft))					//脱离左信号
				{
					stop_rap();
					mode.step_mk++;
				}
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=1;
				}
			return 0;
#endif
		case 0x12:
			Speed=CONST_SPEED_600;
			if(do_action(3,5*CM_PLUS))	//原来为5cm							//直行
				{
					stop_rap();
					mode.step_mk++;
					if(lm_hw.effectLeft)				//直行完毕，发现左信号，重新脱离
						mode.step_mk=0x10;
				}
			
#ifdef MY20180412
			if((lm_hw.effectMidRight))					//直行中，脱离左信号且找到右信号
				{
					stop_rap();
					mode.step_mk=0x20;
				}
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=1;
				}
			return 0;
		case 0x20:
			enable_rap_no_length(FRONT,CONST_SPEED_200,FRONT,CONST_SPEED_600);//向左偏
			//if((!lm_hw.effectRight))				//脱离右信号
			if((!lm_hw.effectRight)&(!lm_hw.effectMidRight))
				{
					stop_rap();
					mode.step_mk=0x21;
				}
			
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=2;
				}
			return 0;
		case 0x21:
			Speed=CONST_SPEED_600;
			if(do_action(3,7*CM_PLUS))						//直行
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			if((lm_hw.effectLeft))								//找到左信号
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=2;
				}
#endif
	
			return 0;
		case 0x13:
			enable_rap_no_length(FRONT,CONST_SPEED_200,FRONT,CONST_SPEED_600);		//稍偏左转
			mode.step_mk++;
			return 0;
#if 1
		case 0x14:
			if(lm_hw.effectLeft)										//接触左信号
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=1;
				}
			else if(r_hw.effectMidLeft|b_hw.effectMidLeft|r_hw.effectMidRight|b_hw.effectMidRight) //转弯中接收头没有接受到红外信号，已经严重左偏了，右红外可能会接到信号
				{
					stop_rap();
					mode.step_mk++;
				}
			return 0;
		case 0x15:
			Speed=CONST_SPEED;
			if(do_action(2,360*Angle_1))
				{
					stop_rap();
					mode.step=DOCKMODE_STEP_FINDSEAT;
					mode.step_mk=4;
#ifdef DOCK_DEBUG
					TRACE("rm_hw lost LEFT in findseat left,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
	
				}
			if(lm_hw.effectMid)
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=1;
				}
			if(lm_hw.effectLeft|lm_hw.effectMidLeft)
				{
					stop_rap();
					mode.step_mk=0x10;
				}
			return 0;
#endif
		
		//////////////////////////////////////////////////////////
		//////////////中红外找到右红外的处理过程//////////////////
		case 0x40:
#ifdef MIDMOVE_BUMP_ACTION
			WHICH_TO_MID=0;
			WHICH_TO_MID|=RIGHT_TO_MID;
#endif
			enable_rap_no_length(FRONT,CONST_SPEED_200,FRONT,CONST_SPEED_600);		//稍偏左转，脱离右信号
			mode.step_mk++;
			return 0;
		case 0x41:
			//if((!lm_hw.effectRight))
			if((!lm_hw.effectRight)&(!lm_hw.effectMidRight))
				{
					stop_rap();
					mode.step_mk++;
				}
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=2;
				}
			return 0;
		case 0x42:
			Speed=CONST_SPEED_600;
			if(do_action(3,5*CM_PLUS))		//原来为5cm
				{
					stop_rap();
					mode.step_mk++;
					if(lm_hw.effectRight)	//qz add 20180428:直行完毕，发现右信号，重新脱离
						mode.step_mk=0x40;
				}
			
#ifdef MY20180412
			if(lm_hw.effectLeft)					//丢失右信号，找到左信号
				{
					stop_rap();
					mode.step_mk=0x30;
				}
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0X50;
					MID_TURN=2;
				}
			return 0;
		case 0x30:
			enable_rap_no_length(FRONT,CONST_SPEED_600,FRONT,CONST_SPEED_200);			//向右偏直到左信号消失
			//if((!lm_hw.effectLeft))
			if((!lm_hw.effectLeft)&(!lm_hw.effectMidLeft))
				{
					stop_rap();
					mode.step_mk++;
				}
			if(lm_hw.effectMid)
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=1;
				}
			break;
		case 0x31:
			Speed=CONST_SPEED_600;
			if(do_action(3,7*CM_PLUS))						//直行
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			if(lm_hw.effectRight)								//找到右信号
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=1;
				}
#endif
	
			return 0;
		case 0x43:
			enable_rap_no_length(FRONT,CONST_SPEED_600,FRONT,CONST_SPEED_200);	//稍偏右转，接触右信号
			mode.step_mk++;
			return 0;
		case 0x44:
			if(lm_hw.effectRight)
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			else if(l_hw.effectMidLeft|b_hw.effectMidLeft|l_hw.effectMidRight|b_hw.effectMidRight) //转弯中接收头没有接受到红外信号，已经严重左偏了，右红外可能会接到信号
				{
					stop_rap();
					mode.step_mk++;
				}
			
			if((lm_hw.effectMid))
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=2; 	//1 									//标记右转
				}
			return 0;
		case 0x45:
			Speed=CONST_SPEED;
			if(do_action(1,360*Angle_1))
				{
					stop_rap();
					mode.step=DOCKMODE_STEP_FINDSEAT;
					mode.step_mk=4;
#ifdef DOCK_DEBUG
					TRACE("rm_hw lost RIGHT in findseat right,go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
				}
			if(lm_hw.effectMid)
				{
					stop_rap();
					mode.step_mk=0x50;
					MID_TURN=2;
				}
			if(lm_hw.effectRight|lm_hw.effectMidRight)
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			return 0;
		//////////////////////////////////////////////////////
		///////////中红外找到中间信号的处理过程///////////////
		case 0x50:
			mode.step_mk=0x58;
			l_speed=800;
			r_speed=800;		
			return 0;
		//////////////////////////////////////////////////////
		////////之前中红外有中间信号，后来丢失处理////////////
		case 0x58:
			
			//if(((!rm_hw.effectMidRight)|(!lm_hw.effectMidRight))&(rm_hw.effectMidLeft|lm_hw.effectMidLeft))
			if((rm_hw.effectMid)&(lm_hw.effectMid))
				{
					//stop_rap();
					//mode.step_mk=0x50;
					l_speed=800;
					r_speed=800;
				}
			//else if(!lm_hw.effectMidRight)
			else if((!lm_hw.effectMidRight)|(!rm_hw.effectMidRight))
			{
				//l_speed+=50;
				//if(l_speed>800)
					//l_speed=800;
				r_speed-=20;
				if(r_speed<200)
					r_speed=200;
				r_speed=200;
				l_speed=800;
				
			}
			//else if(((!rm_hw.effectMidLeft)|(!lm_hw.effectMidLeft))&(rm_hw.effectMidRight|lm_hw.effectMidRight))
			//else if(!lm_hw.effectMidLeft)
			else if((!lm_hw.effectMidLeft)|(rm_hw.effectMidLeft))
			{
				l_speed-=20;
				if(l_speed<200)
					l_speed=200;
				//l_speed=600;
				l_speed=200;
				r_speed=800;
			}
	
			if ((!rm_hw.effectMidRight)&(!rm_hw.effectMidLeft)&(!lm_hw.effectMidLeft)&(!lm_hw.effectMidRight))
				{
					stop_rap();
					mode.step_mk++;
#ifdef DOCK_DEBUG
					TRACE("lost all Mid Signal!\r\n");
#endif
					return 0;
				}
			enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);
			return 0;
		case 0x59:
			Speed=MID_CONST_SPEED;
			if(do_action(2,360*Angle_1))
				{
					stop_rap();
					mode.step=0;
					mode.step_mk=0;
#ifdef DOCK_DEBUG
					TRACE("rm_hw lost r/l/m signal,go to DOCKMODE_STEP_START!\r\n");
#endif
					return 0;
				}
		#if 0
			if((rm_hw.effectMidRight)|lm_hw.effectMidRight)
				{
					stop_rap();
					mode.step_mk=0x40;
				}
			if((rm_hw.effectMidLeft)|lm_hw.effectMidLeft)
				{
					stop_rap();
					mode.step_mk=0x10;
				}
		#endif
			if((rm_hw.effectMid)&(lm_hw.effectMid))
				{
					stop_rap();
#ifdef DOCK_DEBUG
					TRACE("Reget mid signal,keep going!\r\n");
#endif
					mode.step_mk=0x50;
				}
			return 0;
				
		}
		return 0;
}

void Start_Start_IV(void)
{
	static bool l_flag=false,r_flag=false,m_flag=false;
	static bool rr_flag=false,lr_flag=false,mr_flag=false,mmr_flag=false;
	static bool rl_flag=false,ll_flag=false,ml_flag=false,mml_flag=false;
	static bool llm_flag=false,lrm_flag=false,rlm_flag=false,rrm_flag=false,mlm_flag=false,mrm_flag=false,mmlm_flag=false,mmrm_flag=false;
	static u8 l_count=0,r_count=0;
	static u8 lm_count=0,rm_count=0;
//	static int l_length_start,r_length_start;
	switch(mode.step_mk)
	{
	case 0:
		l_flag=false;r_flag=false;m_flag=false;
		lr_flag=false;rr_flag=false;mr_flag=false;mmr_flag=false;
		ll_flag=false;rl_flag=false;ml_flag=false;mml_flag=false;
		llm_flag=false;lrm_flag=false;rlm_flag=false;rrm_flag=false;mlm_flag=false;mrm_flag=false;mmlm_flag=false;mmrm_flag=false;
		l_count=0;r_count=0;
		lm_count=0;rm_count=0;
		mode.step_mk++;
		break;
	case 1:
		Speed=CONST_SPEED_800;
		if(do_action(2,360*Angle_1))
			{
#ifdef DOCK_DEBUG
				TRACE("l_count=%d lm_count=%d\r\n",l_count,lm_count);
				TRACE("r_count=%d rm_count=%d\r\n",r_count,rm_count);
#endif
				stop_rap();
				if(m_flag)
					{
#ifdef DOCK_DEBUG
						TRACE("get M in %s\r\n",__func__);
						TRACE("go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
						mode.step_mk=91;
						r_flag=false;
						l_flag=false;
					}
				else
					{
//						if(l_flag|lm_flag)												//无近信号且只有左信号
						//if(ll_flag&rl_flag&ml_flag&mml_flag)
						if((l_count+lm_count)>(r_count+rm_count))
							{
#ifdef DOCK_DEBUG
								TRACE("l_flag=%d\r\n",l_flag);
								TRACE("get L or LM in %s\r\n",__func__);
								TRACE("go to DOCKMODE_STEP_LEFT!\r\n");
#endif
								if(ml_flag|mml_flag|mlm_flag|mmlm_flag)
								{	
									mode.step_mk=10;
									//mode.step=DOCKMODE_STEP_LEFT;
									//mode.step_mk=0;
									rl_flag=false;
								}
								else
									{
										mode.step=DOCKMODE_STEP_REYBS;
										mode.step_mk=0;
										rl_flag=false;
										return;
									}
								return;
							}
						//if((r_flag)|(rm_flag))												//无近信号且只有右信号
						//if(rr_flag&lr_flag&mr_flag&mmr_flag)
						if((r_count+rm_count)>(l_count+lm_count))
							{
#ifdef DOCK_DEBUG
								TRACE("r_flag=%d\r\n",r_flag);
								TRACE("get R in %s\r\n",__func__);
								TRACE("go to DOCKMODE_STEP_RIGHT!\r\n");
#endif
								if(mrm_flag|mmrm_flag|mmr_flag|mr_flag)
								{
									mode.step_mk=30;
									//mode.step=DOCKMODE_STEP_RIGHT;
									//mode.step_mk=0;
								}
								else
									{
										mode.step=DOCKMODE_STEP_REYBS;
										mode.step_mk=0;
										return;
									}
								return;
							}
						else if(((l_count+lm_count)==(r_count+rm_count))&(l_count+lm_count>0)&(r_count+rm_count>0))
							{
#ifdef DOCK_DEBUG
								TRACE("L signal cnt = R signal cnt!\r\n");
								TRACE("Prepare to ajust!\r\n");
#endif
								mode.step_mk++;
							}
#ifdef REYBS2DOCK
						else
							{
#ifdef DOCK_DEBUG
								TRACE("can't find any signal! go back YBS!\r\n");
#endif
								mode.step=DOCKMODE_STEP_REYBS;
								mode.step_mk=0;
								return;
							}
#endif
						}
			}
		//if(lm_hw.effectMidLeft|rm_hw.effectMidLeft)
			//lm_flag=true;
		//if(lm_hw.effectMidRight|rm_hw.effectMidRight)
			//rm_flag=true;
		//if(r_hw.effectMid|l_hw.effectMid|rm_hw.effectMid|lm_hw.effectMid)		//qz mask 20181215
			//m_flag=true;
		///////以下的代码作用是防反射/////
		if((l_hw.effectLeft)&(!ll_flag))
			{
				ll_flag=true;
				l_count++;
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
#ifdef DOCK_DEBUG
				TRACE("l_hw get L signal directly!");
				TRACE("go to DOCKMODE_STEP_LEFT step_mk 40!\r\n");
#endif
				return;
			}
		if((l_hw.effectMidLeft)&(!llm_flag))	//qz add 20181210
			{
				llm_flag=true;
				lm_count++;
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
#ifdef DOCK_DEBUG
				TRACE("l_hw get LM signal directly!");
				TRACE("go to DOCKMODE_STEP_LEFT step_mk 40!\r\n");
#endif
				return;
			}
		if((l_hw.effectMidRight)&(!lrm_flag))	//qz add 20181210
			{
				lrm_flag=true;
				rm_count++;
			}
		if((l_hw.effectRight)&(!lr_flag))
			{
				lr_flag=true;
				r_count++;
			}
		
		if((rm_hw.effectLeft)&(!ml_flag))
			{
				ml_flag=true;
				l_count++;
			}
		if((rm_hw.effectMidLeft)&(!mlm_flag))	//qz add 20181210
			{
				mlm_flag=true;
				lm_count++;
			}
		if((rm_hw.effectMidRight)&(!mrm_flag))	//qz add 20181210
			{
				mrm_flag=true;
				rm_count++;
			}
		if((rm_hw.effectRight)&(!mr_flag))
			{
				mr_flag=true;
				r_count++;
			}
		
		if((lm_hw.effectLeft)&(!mml_flag))
			{
				mml_flag=true;
				l_count++;
			}
		if((lm_hw.effectMidLeft)&(!mmlm_flag))	//qz add 20181210
			{
				mmlm_flag=true;
				lm_count++;
			}
		if((lm_hw.effectMidRight)&(!mmrm_flag)) //qz add 20181210
			{
				mmrm_flag=true;
				rm_count++;
			}
		if((lm_hw.effectRight)&(!mmr_flag))
			{
				mmr_flag=true;
				r_count++;
			}
		
		if((r_hw.effectLeft)&(!rl_flag))
			{
				rl_flag=true;
				l_count++;
			}
		if((r_hw.effectMidLeft)&(!rlm_flag))	//qz add 20181210
			{
				rlm_flag=true;
				lm_count++;
			}
		if((r_hw.effectMidRight)&(!rrm_flag))	//qz add 20181210
			{
				rrm_flag=true;
				rm_count++;
				stop_rap();
				mode.step=DOCKMODE_STEP_RIGHT;
				mode.step_mk=40;
#ifdef DOCK_DEBUG
				TRACE("r_hw get RM signal directly!");
				TRACE("go to DOCKMODE_STEP_RIGHT step_mk 40!\r\n");
#endif
				return;
			}
		if((r_hw.effectRight)&(!rr_flag))
			{
				rr_flag=true;
				r_count++;
				stop_rap();
				mode.step=DOCKMODE_STEP_RIGHT;
				mode.step_mk=40;
#ifdef DOCK_DEBUG
				TRACE("r_hw get RM signal directly!");
				TRACE("go to DOCKMODE_STEP_RIGHT step_mk 40!\r\n");
#endif
				return;
			}
		////////防反射///////////////
		
		if(lm_hw.effectMid&rm_hw.effectMid)
			{
				stop_rap();
#ifdef DOCK_DEBUG
				TRACE("rm_hw&lm_hw get M signal in %s!\r\n",__func__);
				TRACE("go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 2:
		Speed=CONST_SPEED_800;
		if(do_action(2,360*Angle_1))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_REYBS;
				mode.step_mk=0;
				return;
			}
		if(rm_hw.effectLeft|rm_hw.effectMidRight|rm_hw.effectRight|rm_hw.effectMidLeft/
			lm_hw.effectLeft|lm_hw.effectRight|lm_hw.effectMidLeft|lm_hw.effectMidRight)
			{
				stop_rap();
				mode.step_mk++;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 3:
		Speed=CONST_SPEED_800;
		if(do_action(3,10*CM_PLUS))
			{
				stop_rap();
				mode.step_mk=0;
			}
		break;
	case 10:
		Speed=HIGH_CONST_SPEED;
		if(do_action(1,360*Angle_1))
			{
				mode.step=DOCKMODE_STEP_REYBS;
				mode.step_mk=0;
				rl_flag=false;
				return;
			}
		if(rm_hw.effectLeft|rm_hw.effectMidLeft|lm_hw.effectLeft|lm_hw.effectMidLeft)
			{
				stop_rap();
				mode.step_mk++;
			}
		if(l_hw.effectLeft|l_hw.effectMidLeft)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
				return;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 11:
		Speed=HIGH_CONST_SPEED;
		if(do_action(3,10*CM_PLUS))
			{
				stop_rap();
				mode.step_mk++;
			}
		break;
	case 12:
		Speed=CONST_SPEED_600;
		if(do_action(2,360*Angle_1))
			{

				stop_rap();
				mode.step=DOCKMODE_STEP_START;
				mode.step_mk=0;
				return;
			}
		if(l_hw.effectLeft|l_hw.effectMidLeft)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
				return;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 30:
		Speed=HIGH_CONST_SPEED;
		if(do_action(2,360*Angle_1))
			{
				mode.step=DOCKMODE_STEP_REYBS;
				mode.step_mk=0;
				return;
			}
		if(rm_hw.effectRight|rm_hw.effectMidRight|lm_hw.effectRight|lm_hw.effectMidRight)
			{
				stop_rap();
				mode.step_mk++;
			}
		if(r_hw.effectRight|r_hw.effectMidRight)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_RIGHT;
				mode.step_mk=40;
				return;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 31:
		Speed=HIGH_CONST_SPEED;
		if(do_action(3,10*CM_PLUS))
			{
				stop_rap();
				mode.step_mk++;
			}
		break;
	case 32:
		Speed=CONST_SPEED_600;
		if(do_action(2,360*Angle_1))
			{

				stop_rap();
				mode.step=DOCKMODE_STEP_START;
				mode.step_mk=0;
				return;
			}
		if(r_hw.effectRight|r_hw.effectMidRight)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_RIGHT;
				mode.step_mk=40;
				return;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
		
	case 91:
		Speed=CONST_SPEED_600;
		if(do_action(2,360*Angle_1))
			{
#ifdef DOCK_DEBUG
				TRACE("rm_hw&lm_hw can't get M signal!\r\n");
#endif
				stop_rap();
				if(l_flag)
					{
#ifdef DOCK_DEBUG
						TRACE("can get L,go to DOCKMODE_STEP_LEFT!\r\n");
#endif
						mode.step=DOCKMODE_STEP_LEFT;
					}
				else if(r_flag)
					{
#ifdef DOCK_DEBUG
						TRACE("can get R,go to DOCKMODE_STEP_LEFT!\r\n");
#endif
						mode.step=DOCKMODE_STEP_RIGHT;
					}
				else
					{
#ifdef DOCK_DEBUG
						TRACE("can't find any signal! go back YBS!\r\n");
#endif
						mode.step=DOCKMODE_STEP_REYBS;
					}
				mode.step_mk=0;
				return;
			}

		if(find_home&LEFTMIDLEFT_ONLY)
			l_flag=true;
		if(find_home&RIGHTMIDRIGHT_ONLY)
			r_flag=true;
		
		if((rm_hw.effectMid)&(lm_hw.effectMid))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
			}
		break;
	}
}

void Start_Start_V(void)
{
	static bool l_flag=false,r_flag=false;//m_flag=false;
	static bool rr_flag=false,lr_flag=false,mr_flag=false,mmr_flag=false;
	static bool rl_flag=false,ll_flag=false,ml_flag=false,mml_flag=false;
	static bool llm_flag=false,lrm_flag=false,rlm_flag=false,rrm_flag=false,mlm_flag=false,mrm_flag=false,mmlm_flag=false,mmrm_flag=false;
	static u8 l_count=0,r_count=0;
	static u8 lm_count=0,rm_count=0;
//	static int l_length_start,r_length_start;
	switch(mode.step_mk)
	{
	case 0:
		l_flag=false;r_flag=false;
		lr_flag=false;rr_flag=false;mr_flag=false;mmr_flag=false;
		ll_flag=false;rl_flag=false;ml_flag=false;mml_flag=false;
		llm_flag=false;lrm_flag=false;rlm_flag=false;rrm_flag=false;mlm_flag=false;mrm_flag=false;mmlm_flag=false;mmrm_flag=false;
		l_count=0;r_count=0;
		lm_count=0;rm_count=0;
		mode.step_mk++;
		break;
	case 1:
		Speed=CONST_SPEED_800;
		if(do_action(2,360*Angle_1))
			{
				stop_rap();
				ll_flag=false;llm_flag=false;lrm_flag=false;lr_flag=false;
				rl_flag=false;rlm_flag=false;rrm_flag=false;rr_flag=false;
				ml_flag=false;mlm_flag=false;mrm_flag=false;mr_flag=false;
				mml_flag=false;mmlm_flag=false;mmrm_flag=false;mmr_flag=false;
				
#ifdef DOCK_DEBUG
				TRACE("l_count=%d lm_count=%d\r\n",l_count,lm_count);
				TRACE("r_count=%d rm_count=%d\r\n",r_count,rm_count);
#endif
					{
						if((l_count+lm_count)>(r_count+rm_count))
							{
								mode.step_mk=10;
								return;
							}
						if((r_count+rm_count)>(l_count+lm_count))
							{
								mode.step_mk=30;
								return;
							}
						else if(((l_count+lm_count)==(r_count+rm_count))&(l_count+lm_count>0)&(r_count+rm_count>0))
							{
#ifdef DOCK_DEBUG
								TRACE("L signal cnt = R signal cnt!\r\n");
								TRACE("Prepare to ajust!\r\n");
#endif
								mode.step_mk++;
							}
#ifdef REYBS2DOCK
						else
							{
#ifdef DOCK_DEBUG
								TRACE("can't find any signal! go back YBS!\r\n");
#endif
								mode.step=DOCKMODE_STEP_REYBS;
								mode.step_mk=0;
								return;
							}
#endif
						}
			}
		//if(lm_hw.effectMidLeft|rm_hw.effectMidLeft)
			//lm_flag=true;
		//if(lm_hw.effectMidRight|rm_hw.effectMidRight)
			//rm_flag=true;
		//if(r_hw.effectMid|l_hw.effectMid|rm_hw.effectMid|lm_hw.effectMid)		//qz mask 20181215
			//m_flag=true;
		///////以下的代码作用是防反射/////
		if((l_hw.effectLeft)&(!ll_flag))
			{
				ll_flag=true;
				l_count++;
			}
		if((l_hw.effectMidLeft)&(!llm_flag))	//qz add 20181210
			{
				llm_flag=true;
				lm_count++;
			}
		if((l_hw.effectMidRight)&(!lrm_flag))	//qz add 20181210
			{
				lrm_flag=true;
				rm_count++;
			}
		if((l_hw.effectRight)&(!lr_flag))
			{
				lr_flag=true;
				r_count++;
			}
		
		if((rm_hw.effectLeft)&(!ml_flag))
			{
				ml_flag=true;
				l_count++;
			}
		if((rm_hw.effectMidLeft)&(!mlm_flag))	//qz add 20181210
			{
				mlm_flag=true;
				lm_count++;
			}
		if((rm_hw.effectMidRight)&(!mrm_flag))	//qz add 20181210
			{
				mrm_flag=true;
				rm_count++;
			}
		if((rm_hw.effectRight)&(!mr_flag))
			{
				mr_flag=true;
				r_count++;
			}
		
		if((lm_hw.effectLeft)&(!mml_flag))
			{
				mml_flag=true;
				l_count++;
			}
		if((lm_hw.effectMidLeft)&(!mmlm_flag))	//qz add 20181210
			{
				mmlm_flag=true;
				lm_count++;
			}
		if((lm_hw.effectMidRight)&(!mmrm_flag)) //qz add 20181210
			{
				mmrm_flag=true;
				rm_count++;
			}
		if((lm_hw.effectRight)&(!mmr_flag))
			{
				mmr_flag=true;
				r_count++;
			}
		
		if((r_hw.effectLeft)&(!rl_flag))
			{
				rl_flag=true;
				l_count++;
			}
		if((r_hw.effectMidLeft)&(!rlm_flag))	//qz add 20181210
			{
				rlm_flag=true;
				lm_count++;
			}
		if((r_hw.effectMidRight)&(!rrm_flag))	//qz add 20181210
			{
				rrm_flag=true;
				rm_count++;
			}
		if((r_hw.effectRight)&(!rr_flag))
			{
				rr_flag=true;
				r_count++;
			}
		////////防反射///////////////
		
		if(lm_hw.effectMid&rm_hw.effectMid)
			{
				stop_rap();
#ifdef DOCK_DEBUG
				TRACE("rm_hw&lm_hw get M signal in %s!\r\n",__func__);
				TRACE("go to DOCKMODE_STEP_FINDSEAT!\r\n");
#endif
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 2:
		Speed=CONST_SPEED_800;
		if(do_action(2,360*Angle_1))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_REYBS;
				mode.step_mk=0;
				return;
			}
		if(rm_hw.effectLeft|rm_hw.effectMidRight|rm_hw.effectRight|rm_hw.effectMidLeft/
			lm_hw.effectLeft|lm_hw.effectRight|lm_hw.effectMidLeft|lm_hw.effectMidRight)
			{
				stop_rap();
				mode.step_mk++;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 3:
		Speed=CONST_SPEED_800;
		if(do_action(3,10*CM_PLUS))
			{
				stop_rap();
				mode.step_mk=0;
			}
		break;
	case 10:
		Speed=CONST_SPEED_800;
		if(do_action(1,360*Angle_1))
			{
				stop_rap();
				if(mmlm_flag)
					mode.step_mk++;
				else if(mml_flag)
					mode.step_mk=15;
				else
					{
						mode.step=DOCKMODE_STEP_REYBS;
						mode.step_mk=0;
					}
				return;
			}
		if(l_hw.effectLeft|l_hw.effectMidLeft)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
				return;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		if(lm_hw.effectLeft)
			mml_flag=true;
		if(lm_hw.effectMidLeft)
			mmlm_flag=true;
		break;
	case 11:
		Speed=CONST_SPEED_800;
		if(do_action(1,360*Angle_1))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_REYBS;
				mode.step_mk=0;
				return;
			}
		if((lm_hw.effectMidLeft)|(lm_hw.effectMid&rm_hw.effectMid))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		if(l_hw.effectLeft|l_hw.effectMidLeft)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
				return;
			}
		break;
	case 15:
		Speed=CONST_SPEED_800;
		if(do_action(1,360*Angle_1))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_REYBS;
				mode.step_mk=0;
				return;
			}
		if(lm_hw.effectLeft)
			{
				stop_rap();
				mode.step_mk++;
			}
		if(l_hw.effectLeft|l_hw.effectMidLeft)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
				return;
			}
		break;
	case 16:
		Speed=CONST_SPEED_800;
		if(do_action(3,10*CM_PLUS))
			{
				stop_rap();
				mode.step_mk++;
				return;
			}
		break;
	case 17:
		Speed=CONST_SPEED_800;
		if(do_action(1,360*Angle_1))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_START;
				mode.step_mk=0;
				return;
			}
		if(l_hw.effectLeft|l_hw.effectMidLeft)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
				return;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 30:
		Speed=CONST_SPEED_800;
		if(do_action(2,360*Angle_1))
			{
				stop_rap();
				if(mmrm_flag)
					mode.step_mk++;
				else if(mmr_flag)
					mode.step_mk=35;
				else
					{
						mode.step=DOCKMODE_STEP_REYBS;
						mode.step_mk=0;
					}
				return;
			}
		if(r_hw.effectRight|r_hw.effectMidRight)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_RIGHT;
				mode.step_mk=40;
				return;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		if(lm_hw.effectRight)
			mmr_flag=true;
		if(lm_hw.effectMidRight)
			mmrm_flag=true;
		break;
	case 31:
		Speed=CONST_SPEED_800;
		if(do_action(2,360*Angle_1))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_REYBS;
				mode.step_mk=0;
				return;
			}
		if((lm_hw.effectMidRight)|(lm_hw.effectMid&rm_hw.effectMid))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		if(r_hw.effectRight|r_hw.effectMidRight)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_RIGHT;
				mode.step_mk=0;
				return;
			}
		break;
	case 35:
		Speed=CONST_SPEED_800;
		if(do_action(2,360*Angle_1))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_REYBS;
				mode.step_mk=0;
				return;
			}
		if(r_hw.effectRight|r_hw.effectMidRight)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_RIGHT;
				mode.step_mk=40;
				return;
			}
		if(r_hw.effectRight)
			{
				stop_rap();
				mode.step_mk++;
			}
		break;
	case 36:
		Speed=CONST_SPEED_800;
		if(do_action(3,10*CM_PLUS))
			{
				stop_rap();
				mode.step_mk++;
			}
		break;
	case 37:
		Speed=CONST_SPEED_800;
		if(do_action(2,360*Angle_1))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_START;
				mode.step_mk=0;
				return;
			}
		if(r_hw.effectRight|r_hw.effectMidRight)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_LEFT;
				mode.step_mk=40;
				return;
			}
		if(rm_hw.effectMid&lm_hw.effectMid)
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
				return;
			}
		break;
	case 91:
		Speed=CONST_SPEED_600;
		if(do_action(2,360*Angle_1))
			{
#ifdef DOCK_DEBUG
				TRACE("rm_hw&lm_hw can't get M signal!\r\n");
#endif
				stop_rap();
				if(l_flag)
					{
#ifdef DOCK_DEBUG
						TRACE("can get L,go to DOCKMODE_STEP_LEFT!\r\n");
#endif
						mode.step=DOCKMODE_STEP_LEFT;
					}
				else if(r_flag)
					{
#ifdef DOCK_DEBUG
						TRACE("can get R,go to DOCKMODE_STEP_LEFT!\r\n");
#endif
						mode.step=DOCKMODE_STEP_RIGHT;
					}
				else
					{
#ifdef DOCK_DEBUG
						TRACE("can't find any signal! go back YBS!\r\n");
#endif
						mode.step=DOCKMODE_STEP_REYBS;
					}
				mode.step_mk=0;
				return;
			}

		if(find_home&LEFTMIDLEFT_ONLY)
			l_flag=true;
		if(find_home&RIGHTMIDRIGHT_ONLY)
			r_flag=true;
		
		if((rm_hw.effectMid)&(lm_hw.effectMid))
			{
				stop_rap();
				mode.step=DOCKMODE_STEP_FINDSEAT;
				mode.step_mk=4;
			}
		break;
	}
}

//===============================================================================================================================================================
