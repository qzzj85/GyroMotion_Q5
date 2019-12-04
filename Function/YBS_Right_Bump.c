	
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

//=====================全局变量====================================

//=====================全局函数====================================

//=====================私有变量====================================

#define EXIT_LENGTH 	50
static u8 RIGHT_TURN_ANGLE;	//30
//static u8 LEFT_TURN_ANGLE;	//25
#define RM_HW_TURN_ANGLE 15;	//15

u8 YBS_TurnRight_Cnt=0;
u32 YBS_TurnRight_Time=0;
u32 rightbumptime;
u8 rightbumpcnt=0;	
//static u32 delay_time=0;

void RYBS_Cliff_Action(u8 bump_temp)
{
	switch (mode.bump)
	{
		case BUMP_LEFT_CLIFF:
			switch (mode.step_bp)
				{
					case 0:
						Speed=FAST_MOVE_SPEED;
						if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))		//后退
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 1:
						stop_rap();
						if((e_l.sign==NEAR))					//左地检接地
							{
								mode.step_bp=0xF0;
							}
						else									//左地检悬崖
							{
								mode.step_bp++;
							}
						break;
					case 2:
						Speed=HIGH_MOVE_SPEED;								
						if(do_action(7,180*Angle_1))			//右轮不动向左转
							{
								stop_rap();
								mode.step_bp++;
							}
						if(e_l.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0xF0;
							}
						break;
					case 3:
						Speed=TURN_SPEED;									//左转10度
						if(do_action(1,10*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 4:
						Speed=MID_MOVE_SPEED;									//前进1cm
						if(do_action(3,1*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 5:
						Speed=FAST_MOVE_SPEED;									//后退10cm
						if(do_action(4,10*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 6:											//检查左地检
						if(e_l.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								mode.step_bp++;
							}
						break;
					case 7:
						Speed=TURN_SPEED; 								//右转10度
						if(do_action(2,10*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 8:
						Speed=MID_MOVE_SPEED; 								//前进1cm
						if(do_action(3,1*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 9:
						Speed=FAST_MOVE_SPEED; 								//后退10cm
						if(do_action(4,10*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 10: 										//检查左地检
						if(e_l.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								error_code=ERROR_DANGER;
								Send_Voice(VOICE_ERROR_DANGER);
								mode.wifi_err_code|=WIFI_ERR_EARTH;
								Init_Err();
							}
						break;
					case 0XF0:
						Speed=TURN_SPEED;
						if(do_action(1,120*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						if(bump_temp)
							{
								stop_rap();
								mode.bump=bump_temp;
								mode.step_bp=0;
								
							}
						break;
					case 0xF1:
						mode.bump=0;
						mode.step_bp=0;
						mode.step=0;
						mode.bump_time=giv_sys_time;
						return;
				}
			break;
			
		case BUMP_MID_CLIFF:
			switch (mode.step_bp)
				{
					case 0:
						Speed=FAST_MOVE_SPEED;
						if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))		//后退
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 1:
						if(e_m.sign==NEAR)
							{
								mode.step_bp=0xF0;
							}
						else
							{
								mode.step_bp++;
							}
						break;
					case 2:
						mode.step_bp++;
						break;
					case 3:
						Speed=TURN_SPEED; 								//左转10度
						if(do_action(1,10*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 4:
						Speed=MID_MOVE_SPEED; 								//前进1cm
						if(do_action(3,1*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 5:
						Speed=FAST_MOVE_SPEED; 								//后退10cm
						if(do_action(4,10*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 6: 										//检查左地检
						if(e_m.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								mode.step_bp++;
							}
						break;
					case 7:
						Speed=TURN_SPEED; 								//右转10度
						if(do_action(2,10*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 8:
						Speed=2000; 								//前进1cm
						if(do_action(3,1*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 9:
						Speed=FAST_MOVE_SPEED; 								//后退10cm
						if(do_action(4,10*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 10:										//检查左地检
						if(e_m.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								error_code=ERROR_DANGER;
								Send_Voice(VOICE_ERROR_DANGER);
								mode.wifi_err_code|=WIFI_ERR_EARTH;
								Init_Err();
							}
						break;
					case 0XF0:
						Speed=TURN_SPEED;
						if(do_action(1,100*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						if(bump_temp)
							{
								stop_rap();
								mode.bump=bump_temp;
								mode.step_bp=0;
							}
						break;
					case 0xF1:
						mode.bump=0;
						mode.step_bp=0;
						mode.step=0;
						mode.bump_time=giv_sys_time;
						return;
				}
			break;

		case BUMP_RIGHT_CLIFF:
			switch (mode.step_bp)
				{
				case 0:
					Speed=FAST_MOVE_SPEED;
					if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS)) 		//后退
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 1:
					if((e_r.sign==NEAR))					//左地检接地
						{
							mode.step_bp=0xF0;
						}
					else									//左地检悬崖
						{
							mode.step_bp++;
						}
					break;
				case 2:
					Speed=HIGH_MOVE_SPEED; 							
					if(do_action(8,180*Angle_1))			//左轮不动向右转
						{
							stop_rap();
							mode.step_bp++;
						}
					if(e_r.sign==NEAR)			//qz modify 20180929 e_l--->e_r
						{
							mode.step_bp=0xf0;
						}
					break;
				case 3:
					Speed=TURN_SPEED; 								//左转10度
					if(do_action(1,10*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 4:
					Speed=MID_MOVE_SPEED; 								//前进1cm
					if(do_action(3,1*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 5:
					Speed=FAST_MOVE_SPEED; 								//后退10cm
					if(do_action(4,10*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 6: 										//检查左地检
					if(e_r.sign==NEAR)
						{
							stop_rap();
							mode.step_bp=0XF0;					//OK去F0
						}
					else
						{
							mode.step_bp++;
						}
					break;
				case 7:
					Speed=TURN_SPEED; 								//右转10度
					if(do_action(2,10*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 8:
					Speed=MID_MOVE_SPEED; 								//前进1cm
					if(do_action(3,1*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 9:
					Speed=FAST_MOVE_SPEED; 								//后退10cm
					if(do_action(4,10*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 10:										//检查左地检
					if(e_r.sign==NEAR)
						{
							stop_rap();
							mode.step_bp=0XF0;					//OK去F0
						}
					else
						{
							error_code=ERROR_DANGER;
							Send_Voice(VOICE_ERROR_DANGER);
							mode.wifi_err_code|=WIFI_ERR_EARTH;
							Init_Err();
						}
					break;
				case 0XF0:
					Speed=TURN_SPEED;
					if(do_action(1,60*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					if(bump_temp)
						{
							stop_rap();
							mode.bump=bump_temp;
							mode.step_bp=0;
						}
					break;
				case 0xF1:
					Speed=HIGH_MOVE_SPEED;
					if(do_action(3,10*CM_PLUS))
						{
							stop_rap();
							mode.bump=0;
							mode.step_bp=0;
							mode.step=0;
							mode.bump_time=giv_sys_time;
						}
					if(bump_temp)
						{
							stop_rap();
							mode.bump=bump_temp;
							mode.step_bp=0;
						}
					break;
				}
			break;
	}
}

#ifdef NARROW_PASS		//qz add 20180919
static void Left_Bump_Action(u8 m)
{
	u8 turn_angle=0;
	if((mode.bump==BUMP_ONLY_LEFT)|(mode.bump==BUMP_LEFT_MID)|(mode.bump==BUMP_ONLY_LEFTMID))	//qz modify 20181201
		{
	switch (mode.step_bp)
		{
			case 0:
#ifdef YBS_BLACK
				YBS_DISTANCE=YBS_DISTANCE_CONST;
				rightbumpcnt=0; 		//qz add 20180902
#endif
				
				mode.bump_time=giv_sys_time;
				mode.step_bp++;
			return;
			case 1:
				if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)//BUMP_TIME_DELAY)	//
					return;
				Speed = BUMP_BACK_SPEED;
				if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))//300))	//QZ:后退 
				{
					stop_rap();
					mode.step_bp++;
					mode.bump_time=giv_sys_time;
					return ; 
				}	
			return;
			case 2:
				if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)//BUMP_TIME_DELAY)	200ms
					return;
				mode.step_bp++;
				return;
			case 3:
				if(YBS_TurnRight_Cnt==0)
					YBS_TurnRight_Time=giv_sys_time;
				YBS_TurnRight_Cnt++;
				if(mode.step>=0x88) 										//如果是第一步，则直接左转
					mode.step_bp=0x50;
				else														//不是第一步
					{
						if(YBS_TurnRight_Cnt>3)								//右转已超过3次						//qz modify 20181210  3-->4
							{
								if(giv_sys_time-YBS_TurnRight_Time<80000)	//距离第一次右转时间小于6s					//qz modify 20181210 6s-->8s
									{
										mode.step_bp=0x50;						//直接左转
									}
								else										//距离第一次右转时间超过6s
									{
										mode.step_bp++;						//还是右转	
									}
								YBS_TurnRight_Cnt=0;
							}
						else												//右转次数小于3次
							{
								mode.step_bp++;
							}
					}
			break;
			case 4:			
				Speed=TURN_SPEED;
				if(do_action(2,12*Angle_1))		//右转12度
					{
						stop_rap();
						mode.step_bp++;
						mode.bump_time=giv_sys_time;
					}
				break;
			case 5:
				if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
					return;
				mode.step_bp++;
				break;
			case 6:
				Speed=MID_MOVE_SPEED;
				if(do_action(3,10*CM_PLUS))		//直行10公分
					{
						stop_rap();
						mode.step_bp=0xF0;
					}
				if((m!=0)&(m!=15))				//有除开有中红外的其他碰撞动作
					{
						stop_rap();
						mode.bump=m;
						mode.bump_flag=false;
						mode.step_bp=0;
						//if((m==5)&(YBS_TurnRight_Cnt>2))			//如果左转次数超过3次，则直接右转
							//mode.step_bp=0x50;
						mode.last_bump=LEFT;	//qz add 20181020
					}
				break;
			case 0x50:
				if(do_action(1, (uint32_t)(120 * Angle_1)))		//qz modify 20180801 60-->80  qz modify 20190219 80--->120
					{
						stop_rap();
						mode.step_bp=0xF0;
						YBS_TurnRight_Cnt=0;
					}	
				//read_sideearth(); 	//zdkshield
				break;
			case 0x60:
				if(do_action(2,(u32)(LEFT_TURN_ANGLE*Angle_1))) 		//qz:右转
					{
						stop_rap();
						mode.step_bp=0xF0;
					}
				break;
			case 0xF0:
				stop_rap();
				mode.step_bp=0;
				mode.bump=0;
				mode.bump_flag=false;
				mode.step=0x00;
				mode.last_bump=LEFT;	//qz add 20181020
				mode.bump_time=giv_sys_time;
				break;
	
							
			default :
					mode.bump = 0;
		}
		}


	else
		{
			switch (mode.step_bp)
				{
					case 0:
						mode.bump_time=giv_sys_time;
						mode.step_bp++;
						break;
					case 1:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed=BUMP_BACK_SPEED;
						if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
								mode.bump_time=giv_sys_time;
							}
						break;
					case 2:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						if(mode.bump==BUMP_ONLY_LEFTMID)
							{
								turn_angle=90;
							}
						else
							{
								turn_angle=120;
							}
						Speed=TURN_SPEED;
						if(do_action(1,turn_angle*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
								mode.bump_time=giv_sys_time;
							}
						break;
					case 3:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						mode.bump=0;
						mode.step_bp=0;
						mode.bump_flag=false;
						mode.step=0;
						mode.bump_time=giv_sys_time;
						break;
				}
		}
}
#endif

#if 0
static void Right_Bump_Action(u8 m)
{
		switch(mode.step_bp)
		{
				case 0:
					mode.bump_time=giv_sys_time;
					mode.step_bp=1;
				return;
				case 1:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed = BUMP_BACK_SPEED;
					if(do_action(4,400))		//ZDK屏蔽 	//QZ:后退
					{
						stop_rap();
						//qz add 20180127
						if((!l_bump.key)|(!r_bump.key)|(!lm_bump.key)|(!rm_bump.key))		//还有碰撞信号
							mode.step_bp=0x00;
						else
							{
								mode.step_bp = 0x30;
								mode.bump_time=giv_sys_time;
							}
						return ; 
					}	
				return; 

				case 0x30:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;					
					mode.step_bp=0x40;
					break;
				case 0x40:
					Speed=TURN_SPEED;
					if(do_action(1,10*Angle_1))			//左转10度
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 0x41:
					Speed=MID_MOVE_SPEED;
					if(do_action(3,10*CM_PLUS))			//前进10公分
						{
							stop_rap();
							mode.step_bp=0xF0;
							return;
						}
					if((m!=0)&(m!=6)&(m!=15))			//有除了右中红外和右碰撞的其他碰撞
						{
							stop_rap();
							mode.bump=m;
							mode.step_bp=0;
							return;
						}
					else if(m==6)						//再次右碰撞
						{
							stop_rap();
							mode.step_bp=0x50;
							return;
						}
					break;
				case 0x50:
					Speed = BUMP_BACK_SPEED;
					if(do_action(1, 20* Angle_1))				//	左转		//QZ MASK	qz:原来为45
					{
						stop_rap();
						mode.step_bp=0xF0;
#ifdef YBS_BLACK
						YBS_DISTANCE+=10;
#endif
						return ; 
					}	
					break;
				case 0xF0:
					stop_rap();
					mode.step_bp = 0;
					mode.bump = 0;
					mode.step = 0x00;//0x88; 原来为0x88
					mode.bump_time=giv_sys_time;
					break;
				default :
					mode.bump = 0;
	}
}
#endif

void YBS_Right_Bump(u8 out_enable)
{
#ifdef YBS_AVOID_SEAT
	u32 l_speed,r_speed;
	float radius;
	static u32 start_length,end_length;
#endif
	static u8 turn_dir=0;
	static short tgt_angle=0;
	static u8 turn_angle=0;
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	s8 ydir=Read_Motion_YDir();
	u32 m;
  	m = YBS_read_bump1(out_enable);
	
	switch(mode.bump)	 //bump碰撞的标志
    {	    
		default :
		    mode.bump = 0;
	    break;

		case BUMP_MID_CLIFF:
		case BUMP_LEFT_CLIFF:
		case BUMP_RIGHT_CLIFF:
			RYBS_Cliff_Action(m);
		break;
							
		//LR_BUMP;  双碰撞//LR_BUMP;  双碰撞//LR_BUMP;  双碰撞//LR_BUMP;  双碰撞//LR_BUMP;  双碰撞//LR_BUMP;  双碰撞//LR_BUMP;  双碰撞				

		case BUMP_MID://LR_BUMP;  双碰撞
		 	switch(mode.step_bp)
			{
				case 0:
					mode.bump_time=giv_sys_time;
					mode.step_bp++;
					Set_Coordinate_Wall(now_gridx,now_gridy);
					return;
				case 1:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed = BUMP_BACK_SPEED;
					if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))//400))		
					{
						stop_rap();
						mode.step_bp++;
						mode.bump_time=giv_sys_time;
						return ; 
					}	
					return;	
				case 2:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					mode.step_bp++;
					return;
				case 3:
					Speed = TURN_SPEED;
					if(do_action(1,(u32)(70*Angle_1)))	//QZ ADD
					{
						stop_rap();
						mode.step_bp = 0;
						mode.bump = 0;
						mode.bump_flag=false;
						mode.step = 0;
#ifdef YBS_BLACK		
						YBS_DISTANCE=YBS_DISTANCE_CONST;
						rightbumpcnt=0;			//qz add 20180902
#endif
						mode.last_bump=RIGHT;	//qz add 20181020
						mode.bump_time=giv_sys_time;
						return ; 
					}	
					break;
				default :
					mode.bump = 0;
					mode.bump_flag=false;
			}
		break;	
				
		//右侧碰撞		//右侧碰撞			//右侧碰撞				//右侧碰撞				//右侧碰撞				//右侧碰撞					//右侧碰撞		
		//右侧碰撞		//右侧碰撞			//右侧碰撞				//右侧碰撞				//右侧碰撞				//右侧碰撞					//右侧碰撞			
	    case BUMP_ONLY_RIGHT://R_BUMP:
	    //case BUMP_ONLY_RIGHTMID:	//qz add 20181020
		//case BUMP_RIGHT_MID:		//qz add 20181020
#if 0
		   		Right_Bump_Action(m);
#else
        	switch(mode.step_bp)
				{
					case 0:
						mode.bump_time=giv_sys_time;
						mode.step_bp++;
						Set_Coordinate_Wall(now_gridx,now_gridy);
					return;
					case 1:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed = BUMP_BACK_SPEED;
						if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))//400))		
						{
							stop_rap();
							mode.step_bp ++;
							mode.bump_time=giv_sys_time;
							return ; 
						}	
					return;	
					case 2:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						//qz add 20180919
						if((w_r.high_sign==NEAR)&(w_l.high_sign==NEAR))		//qz mask 20181210
						//if(mode.last_bump==LEFT)
							{
								RIGHT_TURN_ANGLE=10;
							}
						else
							{
								RIGHT_TURN_ANGLE=30;//20;
							}
						
						mode.step_bp++;
						return;
					case 3:
						Speed = TURN_SPEED;								
						if(do_action(1,	(uint32_t)(RIGHT_TURN_ANGLE* Angle_1))		)				//	左转		//QZ MASK	qz:原来为45
						{
							stop_rap();
							mode.step_bp = 0;
							mode.bump = 0;
							mode.bump_flag=false;
							mode.step = 0x00;//0x88; 原来为0x88
							mode.last_bump=RIGHT;	//qz add 20181020

#ifdef YBS_BLACK
							if(rightbumpcnt==0)
								{
									rightbumptime=giv_sys_time;
//									Enable_Wall_Count();			//qz add 20180902
								}
							rightbumpcnt++;
							YBS_DISTANCE+=10;
							if(YBS_DISTANCE>YBS_DISTANCE_LONG)		//qz add 20180919
								YBS_DISTANCE=YBS_DISTANCE_LONG;		//qz add 20180919
#endif
							mode.bump_time=giv_sys_time;
							return ; 
						}	
						break;
					default :
						mode.bump = 0;
						mode.bump_flag=false;
				}
#endif
		break;	
			//------------------------------------------------------------------	

			//右侧碰撞		//右侧碰撞			//右侧碰撞				//右侧碰撞				//右侧碰撞				//右侧碰撞					//右侧碰撞	
			//右侧碰撞		//右侧碰撞			//右侧碰撞				//右侧碰撞				//右侧碰撞				//右侧碰撞					//右侧碰撞								
		case BUMP_ONLY_RIGHTMID:
		case BUMP_RIGHT_MID:
			switch(mode.step_bp)
				{
					case 0:
						mode.bump_time=giv_sys_time;
						mode.step_bp++;
						Set_Coordinate_Wall(now_gridx,now_gridy);
						break;
					case 1:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed=BUMP_BACK_SPEED;
						if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))//400))		
							{
								stop_rap();
								mode.step_bp++;
								mode.bump_time=giv_sys_time;
							}
						break;
					case 2:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed=TURN_SPEED;
						turn_angle=30;
						if(do_action(1,turn_angle*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
								mode.bump_time=giv_sys_time;
							}
						break;
					case 3:
						mode.bump=0;
						mode.step_bp=0;
						mode.step=0;
						mode.bump_flag=false;
						mode.last_bump=RIGHT;	//qz add 20181020
						//qz add 20181210
						YBS_DISTANCE+=10;
						if(YBS_DISTANCE>YBS_DISTANCE_LONG)		//qz add 20180919
							YBS_DISTANCE=YBS_DISTANCE_LONG; 	//qz add 20180919
						//qz add end	
						mode.bump_time=giv_sys_time;
						break;
				}
			break;						
														
			//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM
			//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM	
			//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM//E_RM		
			//------------------------------------------------------------------
			//------------------------------------------------------------------



			//------------------------------------------------------------------		
	    case BUMP_ONLY_LEFT://L_BUMP:
	    case BUMP_ONLY_LEFTMID:
		case BUMP_LEFT_MID:
#ifdef NARROW_PASS
			Left_Bump_Action(m);
#else
			switch(mode.step_bp)
				{
					case 0:
						mode.bump_time=giv_sys_time;
						mode.step_bp=1;
						Set_Coordinate_Wall(now_gridx,now_gridy);
					return;
					case 1:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed = BUMP_BACK_SPEED;
						if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))    //QZ:后退 
						{
							stop_rap();
							mode.step_bp++;
							mode.bump_time=giv_sys_time;
							return ; 
						}   
					return;
					case 2:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						mode.step_bp++;
						return;
					case 3:
						Speed=TURN_SPEED;
						if(do_action(1,90*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 4:
						mode.bump=0;
						mode.step_bp=0;
						mode.bump_flag=false;
						mode.step=0;
						mode.bump_time=giv_sys_time;
						break;
					default :
						mode.bump = 0;
		 }
#endif
	 		return;
		//------------------------------------------------------------------
			

							
					


						
		//------------------------------------------------------------------	
	    case 10://rm_hw			//	红外	 	
	    case 11://l_hw			//	红外	 	 	
	    case 12://lm_hw			//	红外	 	
	    case 13://r_hw			//	红外	 		 	
		//------------------------------------------------------------------
		case 7://W_L:										//	左侧             
			switch(mode.step_bp)
				{
					case 0:
						mode.bump_time=giv_sys_time;
						mode.step_bp++;
						Set_Coordinate_Wall(now_gridx,now_gridy);
						break;
					case 1:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed = TURN_SPEED;
						if(do_action(1,	(uint32_t)(125 * Angle_1)))			//	左转
							{
								stop_rap();
								mode.step_bp++;
								mode.bump_time=giv_sys_time;
							}
						break;			 
					case 2:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						stop_rap();
						mode.step_bp = 0;
						mode.bump = 0;
						mode.step = 0x00;		//zdkshield
						mode.last_bump=RIGHT;	//qz add 20181020
						mode.bump_time=giv_sys_time;
						break;
					default :
						mode.bump = 0;
			}
		return;
		
	    case 8://W_LM:									//	左侧中墙
			switch(mode.step_bp)
				{
					case 0:
						stop_rap();
						mode.bump_time=giv_sys_time;
						mode.step_bp++;
						Set_Coordinate_Wall(now_gridx,now_gridy);
						break;
					case 1:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed = TURN_SPEED;
						if(do_action(1,	(uint32_t)(65 * Angle_1))		)			//	左转	//QZ MASK
							{
								stop_rap();
								mode.step_bp++;
								mode.bump_time=giv_sys_time;
							}
						break;			 								
					case 2:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						stop_rap();
						mode.step_bp = 0;
						mode.bump = 0;
						mode.step = 0x00;		//QZ MASK
						mode.last_bump=RIGHT;	//qz add 20181020
						mode.bump_time=giv_sys_time;
						break;
					default :
						mode.bump = 0;
			}
		return;

#ifdef W_MM_ENABLE
		case 99:
#endif
		case 9://W_M:										//	中墙
            switch(mode.step_bp)
			{
				case 0:
					stop_rap();
					mode.bump_time=giv_sys_time;
					mode.step_bp++;
					Set_Coordinate_Wall(now_gridx,now_gridy);
					break;
				case 1:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed = TURN_SPEED;				
					if(do_action(0x01,70*Angle_1))	//向右转90度
					{
						if(m!=0)
							{
								stop_rap();
								mode.bump=m;
								mode.step_bp=0;
#ifdef YBS_BLACK											//qz add 20180902
								YBS_DISTANCE=YBS_DISTANCE_CONST;
								rightbumpcnt=0;
#endif
								mode.last_bump=RIGHT;	//qz add 20181020
								return;
							}
						stop_rap();
						mode.step_bp++;
						mode.bump_time=giv_sys_time;
					}	
					break;	
				case 2:
					//if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						//return;
					stop_rap();
					mode.step_bp = 0;
					mode.bump = 0;
					mode.step = 0x00;//0x88;  //QZ:原来为0x88
#ifdef YBS_BLACK
					YBS_DISTANCE=YBS_DISTANCE_CONST;
					rightbumpcnt=0; 		//qz add 20180902
#endif
					mode.last_bump=RIGHT;	//qz add 20181020
					mode.bump_time=giv_sys_time;
					break;
				default :
						mode.bump = 0;
						
			}
		return;	

	case 15:
		switch (mode.step_bp)
			{
				case 0:
					stop_rap();
					mode.bump_time=giv_sys_time;
					mode.step_bp++;
					Set_Coordinate_Wall(now_gridx,now_gridy);
				break;
				case 1:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed = TURN_SPEED;
					if(do_action(1, (uint32_t)(30 * Angle_1)))			//右转,原来为40	//qz modify 20-->15 20180710
					//do_action(1,360*Angle_1);
					//if(m!=BUMP_WALL_RIGHT_MID)
						{
							stop_rap();	//qz add
							mode.step_bp = 0;
							mode.bump = 0;
							mode.step = 0x00;//0x88;  //QZ:原来为0x88
#ifdef YBS_BLACK
							//YBS_DISTANCE=YBS_DISTANCE_CONST;
							//rightbumpcnt=0;		//qz add 20180902
#endif
							mode.last_bump=RIGHT;	//qz add 20181020
						}	
										//	return 0;	
					break;
				case 0xF0:
					//if(giv_sys_time-mode.bump_time<500)
						//return;
					stop_rap();
					mode.step_bp = 0;
					mode.bump = 0;
					mode.step = 0x00;//0x88;  //QZ:原来为0x88
#ifdef YBS_BLACK
					//YBS_DISTANCE=YBS_DISTANCE_CONST;
					//rightbumpcnt=0; 		//qz add 20180902
#endif
					mode.last_bump=RIGHT;	//qz add 20181020
					mode.bump_time=giv_sys_time;
					break;
				default :
					mode.bump = 0;
			}
		break;								
			//qz add 20180625
			//过障处理动作
#ifdef OBSTACLE_CURRENT_CHECK
	case 80:
		switch(mode.step_bp)
			{
			#if 1
				case 0:
					Speed=TOP_MOVE_SPEED;
					if(do_action(4,10*CM_PLUS))
						{
							stop_rap();
							mode.step_bp=10;
							delay_time=giv_sys_time;
						}
					break;
				case 10:
					if(giv_sys_time-delay_time>5000)
						{
							mode.step_bp=1;
						}
					break;
				case 1:
					Speed=BUMP_BACK_SPEED;
					if(do_action(1,25*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 2:
					Speed=TOP_MOVE_SPEED;
					if(do_action(3,25*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 3:
					Speed=BUMP_BACK_SPEED;
					if(do_action(2,30*Angle_1))
						{
							stop_rap();
							mode.step_bp=0;
							mode.bump=0;
							mode.step=0;
							clr_curr_buf(l_curr_buf);
							clr_curr_buf(r_curr_buf);
						}
					break;
			#endif
			#if 0
				case 0:
					start_length=l_ring.all_length;
					mode.step_bp++;
				case 1:
					enable_rap_no_length(FRONT,4000,BACK,500);
					if(l_ring.all_length-start_length>120*Angle_1)
						{
							stop_rap();
							mode.step_bp++;
							delay_time=giv_sys_time;
						}
					break;
				case 2:
					if(giv_sys_time-delay_time>5000)
						{
							mode.step_bp++;
							start_length=r_ring.all_length;
						}
					break;
				case 3:
					enable_rap_no_length(BACK,500,FRONT,4000);
					if(r_ring.all_length-start_length>120*Angle_1)
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 4:
					Speed=4000;
					if(do_action(3,10*CM_PLUS))
						{
							stop_rap();
							mode.step_bp=0;
							mode.bump=0;
							mode.step=0;
						}
					break;
				#endif
			}
		return;
#endif

#ifdef YBS_AVOID_SEAT
	case BUMP_SEAT:
		Set_Coordinate_Seat(now_gridx,now_gridy);
		switch(mode.step_bp)
			{
				case 0:
					Speed=TURN_SPEED;
					if(do_action(1,80*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 1:
					l_speed=800;
					r_speed=550;
					radius=(float)(RING_RANGE*r_speed)/(l_speed-r_speed);	//半径约为20mm
					end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//准备右偏转180度
					start_length=l_ring.all_length;
					mode.step_bp++;
					break;
				case 2:
					l_speed=800;
					r_speed=550;
					enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);
					if(l_ring.all_length-start_length>end_length)
						{
							stop_rap();
							mode.bump=0;
							mode.step_bp=0;
							mode.step=0;
						}
					if((m!=mode.bump)&(m>0))
						{
							stop_rap();
							mode.bump=m;
							mode.step_bp=0;
							return;
						}
					if(mode.mode==DOCKING)
						{
							if(l_ring.all_length-start_length>(end_length/2))
							{
								stop_rap();
								enable_hwincept();
								Init_Docking();
							}
						}
					break;
			}
		break;
#endif	
	case BUMP_XOUTRANGE:
	case BUMP_YOUTRANGE:
	case BUMP_XMAX_OUT:
	case BUMP_XMIN_OUT:
	case BUMP_YMAX_OUT:
	case BUMP_YMIN_OUT:
	case BUMP_RF_OUT:
	case BUMP_LF_OUT:
	case BUMP_RB_OUT:
	case BUMP_LB_OUT:
		switch(mode.step_bp)
			{
				case 0:
					stop_rap();
					mode.step_bp++;
					//mode.step_bp=50;
					mode.bump_time=giv_sys_time;
				break;
				case 50:
					Speed=BUMP_BACK_SPEED;
					do_action(4,4*CM_PLUS);
					if(!Check_OutofRange_YBS(out_enable))
						{
							stop_rap();
							mode.step_bp=1;
						}
					break;
				case 1:
					if(giv_sys_time-mode.bump_time<1000)
						return;
					#if 0
					if(Read_Motion_BackSweep())
						{
							mode.step_bp=11;
							return;
						}
					#endif
#if 1
					if(Analysis_NeedBack_YBS(grid.y))
						{
							mode.step_bp=10;
						}
					else
#endif
						{
							mode.step_bp++;
							switch(mode.bump)
								{
									case BUMP_XMAX_OUT:
									case BUMP_RF_OUT:
										tgt_angle=L_Angle_Const;
										break;
									case BUMP_XMIN_OUT:
									case BUMP_LB_OUT:
										tgt_angle=R_Angle_Const;
										break;
									case BUMP_YMAX_OUT:
									case BUMP_RB_OUT:
										tgt_angle=F_Angle_Const;
										break;
									case BUMP_YMIN_OUT:
									case BUMP_LF_OUT:
										tgt_angle=B_Angle_Const;
										break;
								}
//							turn_dir=Get_TurnDir(tgt_angle);
						}
					break;
				case 2:
					Speed=TURN_SPEED;
					turn_dir=Get_TurnDir(tgt_angle);
					do_action(turn_dir,360*Angle_1);
					if(Judge_Yaw_Reach(tgt_angle,TURN_ANGLE_BIOS))
						{
							stop_rap();
							mode.step_bp=100;
							mode.bump_time=giv_sys_time;
						}
					break;
				case 100:				//增加这个的目的，是按照当前角度走5cm，如果出现LF/LB/RF/RB等双重出界，可以暂时摆脱该区域
					Speed=HIGH_MOVE_SPEED;
					if(do_action(3,5*CM_PLUS))
						{
							stop_rap();
							mode.step_bp=3;
						}
					if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
						{
							stop_rap();
							mode.step_bp=4;
							return;
						}
					break;
				case 3:
					Speed=FAST_MOVE_SPEED;
					//if(do_action(3,FARAWAY*CM_PLUS))
					if(do_action_my(3,FARAWAY*CM_PLUS,tgt_angle))
						{
							stop_rap();
							mode.bump=0;
							mode.step_bp=0;
							mode.step=0;
							mode.bump_flag=false;
							mode.bump_time=giv_sys_time;
							return;
						}
					if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
						{
							stop_rap();
							mode.step_bp++;
							return;
						}
					if((m!=mode.bump)&(m>=BUMP_OUTRANGE))
						{
							stop_rap();
							mode.bump=m;
							mode.step_bp=0;
						}
					break;
				case 4:
					Set_Coordinate_Wall(grid.x,grid.y);
					switch(m)
						{
#if 0
							case BUMP_ONLY_LEFT:
								turn_angle=150;
								break;
							case BUMP_ONLY_LEFTMID:
							case BUMP_LEFT_MID:
								turn_angle=120;
								break;
							case BUMP_MID:
								turn_angle=90;
								break;
							case BUMP_ONLY_RIGHTMID:
							case BUMP_RIGHT_MID:
#else
							case BUMP_ONLY_LEFT:
							case BUMP_MID:
#endif
								turn_angle=60;
								break;
							case BUMP_ONLY_RIGHT:
								turn_angle=30;
								break;
							default:
								turn_angle=30;
								break;
						}
					mode.step_bp++;
					break;
				case 5:
					Speed=BUMP_BACK_SPEED;
					if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
							mode.bump_time=giv_sys_time;
						}
					break;
				case 6:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed=TURN_SPEED;
					if(do_action(1,turn_angle*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 7:
					Speed=TURN_SPEED;
					if(do_action(3,8*CM_PLUS))
						{
							stop_rap();
#if 0
							mode.step=0;
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=0;
#endif
							mode.step_bp++;
						}
					if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
						{
							stop_rap();
							//mode.bump=m;
							//mode.step_bp=0;
							//return;
							mode.step_bp=4;
						}
					break;
				case 8:
					enable_rap_no_length(FRONT,REVOLUTION_SPEED_HIGH,FRONT,REVOLUTION_SPEED_LOW);
					if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
						{
							stop_rap();
							//mode.bump=m;
							//mode.step_bp=0;
							//return;
							mode.step_bp=4;
							return;
						}
					if((m!=mode.bump)&(m>=BUMP_OUTRANGE))
						{
							stop_rap();
							mode.bump=m;
							mode.step_bp=0;
							return;
						}
					if(m==0)
						{
							mode.step=0;
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=0;	
							mode.bump_time=giv_sys_time;
							return;
						}
					if(Judge_Yaw_Reach(tgt_angle,TURN_ANGLE_BIOS))
						{
							stop_rap();
							mode.step_bp=3;
						}
					break;

				case 10:
					Save_Abort_Data();
					Set_Motion_BackSweep(1);	//设置回扫标志
					Change_LeftRight();
					mode.step_bp++;
					break;
				case 11:
					if(ydir>=0)		//沿Y轴正方向清扫
						{
							tgt_angle=L_Angle_Const;
						}
					else							//沿Y轴负方向清扫
						{
							tgt_angle=R_Angle_Const;
						}
//					turn_dir=Get_TurnDir(tgt_angle);
					mode.step_bp++;
					break;
				case 12:
					Speed=TURN_SPEED;
					turn_dir=Get_TurnDir(tgt_angle);
					if(do_action(turn_dir,720*Angle_1));
					if(Judge_Yaw_Reach(tgt_angle,TURN_ANGLE_BIOS))
						{
							stop_rap();
							mode.step_bp++;
				 		}
					break;
				case 13:
					Speed=MID_MOVE_SPEED;
					//if(do_action(3,20*CM_PLUS))
					if(do_action_my(3,20*CM_PLUS,tgt_angle))
						{
							stop_rap();
						}
					if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
						{
							stop_rap();
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
						}
					break;
				case 14:
					switch(m)
						{
#if 0
							case BUMP_ONLY_LEFT:
								turn_angle=150;
								break;
							case BUMP_ONLY_LEFTMID:
							case BUMP_LEFT_MID:
								turn_angle=120;
								break;
							case BUMP_MID:
								turn_angle=90;
								break;
							case BUMP_ONLY_RIGHTMID:
							case BUMP_RIGHT_MID:
#else
							case BUMP_ONLY_LEFT:
							case BUMP_MID:
#endif
								turn_angle=60;
								break;
							case BUMP_ONLY_RIGHT:
								turn_angle=30;
								break;
							default:
								turn_angle=30;
								break;
						}
					Set_Coordinate_Wall(grid.x,grid.y);
					mode.step_bp++;
					break;
				case 15:
					Speed=BUMP_BACK_SPEED;
					if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
							mode.bump_time=giv_sys_time;
						}
					break;
				case 16:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed=TURN_SPEED;
					if(do_action(1,turn_angle*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 17:
					Speed=TURN_SPEED;
					if(do_action(3,8*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
						{
							stop_rap();
							mode.bump_time=giv_sys_time;
							mode.step_bp=14;
						}
					break;
				case 18:
					enable_rap_no_length(FRONT,REVOLUTION_SPEED_HIGH,FRONT,REVOLUTION_SPEED_LOW);
					if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
						{
							stop_rap();
							mode.bump_time=giv_sys_time;
							mode.step_bp=14;
						}
					if(Judge_Yaw_Reach(tgt_angle,TURN_ANGLE_BIOS))
						{
							stop_rap();
							mode.step_bp=13;
						}
					break;
			
			}
					
	} //结束  switch(mode.bump)	
		
	
	
}
//=====================================================================================

void No_thing(void)
{
}


