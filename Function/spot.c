////////////////////////头文件////////////////////////////////////
#include "AAA-include.h"

////////////////////////私有定义//////////////////////////////////
#define	OUTSIDE_SPEED	1000
#define	SPD_CNT_MAX		7
////////////////////////全局变量//////////////////////////////////
s8 spot_gyrobios_cnt=0;	//正则右转多，负则左转多
///////////////////////私有变量////////////////////////////////////
static bool piv_out;	//机器是否向外展开，1为向外展开，0为向里缩小
static bool piv_left; //机器是否向左转，1为向左转，0为向右转
static s8 spd_cnt=0;
static u8 startfrom=0;
///////////////////////全局函数////////////////////////////////////
///////////////////////私有函数////////////////////////////////////
///////////////////////函数实体////////////////////////////////////
/*****************************************************************
功能：初始化定点清扫的程序
*****************************************************************/
void Init_Spot(u8 start_from)
{					 
    /******初始化显示***********/
	/******初始化设置的值********************/	
	if(mode.low_power)
		{
			Send_Voice(VOICE_POWER_LOW);
			Init_Cease();
			return;
		}

	
	mode.mode = MODE_SPOT;	 //工作模式为清扫
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
	piv_out=1;
	startfrom=start_from;
	if((startfrom==SPOT_FROM_CHARGE)|(startfrom==SPOT_FROM_CEASE))
		{
			motion1.clean_size=0;
			motion1.worktime=1;
		}
	CHECK_STATUS_FLAG=true;			//使能异常检测
	Init_Check_Status();
	Send_Voice(VOICE_SWEEP_START);
}

void Init_Sweep_Spot(u8 start_from)
{					 
    /******初始化显示***********/
	/******初始化设置的值********************/
	
	mode.mode = MODE_SWEEP;	 //工作模式为清扫
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
	piv_out=1;
	startfrom=start_from;
	if((startfrom==SPOT_FROM_CHARGE)|(startfrom==SPOT_FROM_CEASE))
		{
			motion1.clean_size=0;
			motion1.worktime=1;
		}
	CHECK_STATUS_FLAG=true;			//使能异常检测
	Init_Check_Status();
	Set_AreaWorkTime(10);
}


u8 Read_Spot_Bump(u8 ir_enable)
{
	u8 data1=0;
#ifdef CLIFF_ENABLE			//ZDK屏蔽
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
		case BUMP_ONLY_LEFT:						 //左碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_LEFT;
		 case BUMP_ONLY_LEFTMID:					 //左中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_LEFTMID;
		 case BUMP_LEFT_MID:						 //左左中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_LEFT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_LEFT_MID;
		 case BUMP_ONLY_RIGHT:						 //右碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_RIGHT;
		 case BUMP_ONLY_RIGHTMID:					 //右中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_RIGHTMID;
		 case BUMP_RIGHT_MID:						 //右右中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_RIGHT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_RIGHT_MID;
		 case BUMP_MID: 							 //中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_MID;
	}

	if((w_m.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0)|(mode.bump==BUMP_SEAT)) 	//中墙检靠近墙
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
			if((mode.bump == 0))		 //左中墙检靠近墙
				{
					stop_rap();
					mode.bump=LM_WALL_BUMP;//W_LM;
					mode.step_bp=0;
				}
			return LM_WALL_BUMP;//W_LM;
		}

	if((w_rm.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0))		 //左中墙检靠近墙
				{
					stop_rap();
					mode.bump=RM_WALL_BUMP;//W_LM;
					mode.step_bp=0;
				}
			return RM_WALL_BUMP;//W_LM;
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
				if(piv_left)								//右轮固定速度，向左偏转
					{
						l_speed=abs(l_sp_turn[spd_cnt]);
						r_speed=OUTSIDE_SPEED;	
						r_start_length=r_ring.all_length;
						radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);	//半径约为20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//准备右偏转180度
					}
				else										//左轮固定速度，向右偏转
					{
						l_speed=OUTSIDE_SPEED;
						r_speed=abs(r_sp_turn[spd_cnt]);
						l_start_length=l_ring.all_length;
						radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);	//半径约为20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//准备右偏转180度
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
				if(Judge_Yaw_Reach(tgt_yaw,TURN_ANGLE_BIOS,1))
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
				if(piv_left)								//右轮固定速度，向左偏转
					{
						l_speed=abs(l_sp_turn[spd_cnt]);
						r_speed=OUTSIDE_SPEED;	
						r_start_length=r_ring.all_length;
						radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);	//半径约为20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//准备右偏转180度
					}
				else
					{
						l_speed=OUTSIDE_SPEED;
						r_speed=abs(r_sp_turn[spd_cnt]);
						l_start_length=l_ring.all_length;
						radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);	//半径约为20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//准备右偏转180度
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
#ifdef DEBUG_SPOT
							TRACE("spot_gyrobios_cnt=%d\r\n",spot_gyrobios_cnt);
#endif
							break;
					}
				break;
			case 10:
				Speed=TURN_SPEED;
				if(spot_gyrobios_cnt<0)		//左转多，需右转
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
