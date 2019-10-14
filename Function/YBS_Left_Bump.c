	
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

//******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义************
//=====================全局变量====================================


//=====================全局函数====================================



//******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义************
//=====================私有变量====================================
//static u8 leftbumpcnt=0;
//static u8 YBS_LeftRight_Time=0;
//static u32 leftbumptime=0;			
			
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------		
void LYBS_Cliff_Action(u8 bump_temp)
{
	switch (mode.bump)
	{
		case BUMP_LEFT_CLIFF:
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
					if(do_action(7,180*Angle_1))			//左轮不动向右转
						{
							stop_rap();
							mode.step_bp++;
						}
					if(e_l.sign==NEAR)			//qz modify 20180929 e_l--->e_r
						{
							mode.step_bp=0xf0;
						}
					break;
				case 3:
					Speed=TURN_SPEED; 								//左转10度
					if(do_action(2,10*Angle_1))
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
					if(do_action(1,10*Angle_1))
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
					if(e_l.sign==NEAR)
						{
							stop_rap();
							mode.step_bp=0XF0;					//OK去F0
						}
					else
						{
							error_code=SEND_ERROR_DANGER;
							dis_err_code=DIS_ERROR_DANGER;
							Send_Voice(VOICE_ERROR_DANGER);
							Init_Err();
						}
					break;
				case 0XF0:
					Speed=TURN_SPEED;
					if(do_action(2,60*Angle_1))
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
								error_code=SEND_ERROR_DANGER;
								dis_err_code=DIS_ERROR_DANGER;
								Send_Voice(VOICE_ERROR_DANGER);
								Init_Err();
							}
						break;
					case 0XF0:
						Speed=TURN_SPEED;
						if(do_action(2,100*Angle_1))
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
						if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))		//后退
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 1:
						stop_rap();
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
						if(do_action(8,180*Angle_1))			//右轮不动向左转
							{
								stop_rap();
								mode.step_bp++;
							}
						if(e_r.sign==NEAR)
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
					case 10: 										//检查左地检
						if(e_r.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								error_code=SEND_ERROR_DANGER;
								dis_err_code=DIS_ERROR_DANGER;
								Send_Voice(VOICE_ERROR_DANGER);
								Init_Err();
							}
						break;
					case 0XF0:
						Speed=TURN_SPEED;
						if(do_action(2,120*Angle_1))
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
	}
}


void YBS_Left_Bump(u8 out_enable)
{ 
#ifdef YBS_AVOID_SEAT
	u32 l_speed,r_speed,temp_data1;
	float radius;
	static u32 start_length,end_length;
	static bool no_enter=false;
#endif
	u32 m;
	static u8 turn_dir=0,turn_angle=0;
	static short tgt_angle=0;
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	s8 ydir=Read_Motion_YDir();
	m = YBS_read_bump1(out_enable);
	switch(mode.bump)	   //处理   //bump碰撞的标志   
  	{	    
		default :
	    	mode.bump=0;
			mode.step_bp=0;
			mode.bump_flag=0;
	        break;
		case BUMP_MID_CLIFF:
		case BUMP_LEFT_CLIFF:
		case BUMP_RIGHT_CLIFF:
			LYBS_Cliff_Action(m);
		break;
/*************************************************************************
//      
//                      地检
//                      地检      
//                      地检
//      
*************************************************************************/ 


/*************************************************************************
//      
//                      碰撞
//                      碰撞    
//                      碰撞
//      
*************************************************************************/ 
/******************************************
//      
//           双碰撞即前碰撞
//
******************************************/              
		case BUMP_MID://LR_BUMP;  双碰撞   //
		switch(mode.step_bp)
			{
				case 0:
					mode.bump_time=giv_sys_time;
					mode.step_bp=1;
					break;
				case 1:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed = BUMP_BACK_SPEED;
					if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))		
						{
							stop_rap();
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
							return ; 
						}	
					break; 
				case 2:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					mode.step_bp++;
					return;
				case 3:
					Speed = BUMP_BACK_SPEED;
					//if(do_action(1,	(uint32_t)(80 * Angle_1))		)// 左转 //QZ MASK
					if(do_action(2,(u32)(70*Angle_1)))	//QZ ADD
						{
							stop_rap();
							mode.step_bp = 0;
							mode.bump=0;
							mode.bump_flag=false;
							mode.step = 0x40;
#ifdef YBS_BLACK		
							YBS_DISTANCE=YBS_DISTANCE_CONST;
#endif
							mode.bump_time=giv_sys_time;
							return ; 
						}	
					break;
				default :
					mode.bump=0;
					mode.step_bp=0;
					mode.bump_flag=false;
					mode.step=0x40;
					mode.bump_time=giv_sys_time;
					break;
			}
		break; 
/******************************************
//      
//                 右碰撞
//
******************************************/           
			//右侧碰撞		//右侧碰撞			//右侧碰撞				//右侧碰撞				//右侧碰撞				//右侧碰撞					//右侧碰撞		
			//右侧碰撞		//右侧碰撞			//右侧碰撞				//右侧碰撞				//右侧碰撞				//右侧碰撞					//右侧碰撞			
	    case BUMP_ONLY_RIGHT://R_BUMP:
	    case BUMP_ONLY_RIGHTMID:
		case BUMP_RIGHT_MID:
        switch(mode.step_bp)
			{
#ifdef NARROW_PASS
				case 0:
					mode.bump_time=giv_sys_time;
					mode.step_bp=1;
				break;
				case 1:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed = BUMP_BACK_SPEED;
					if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))	//QZ:后退 
						{
							stop_rap();
							mode.step_bp++;
							mode.bump_time=giv_sys_time;
							return ; 
						}	
				break;
				case 2:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					mode.step_bp++;
				break;
				case 3:
					Speed = BUMP_BACK_SPEED;
					mode.step_bp=0x60;				//左转
					YBS_LeftRight_Time++;			
					if(YBS_LeftRight_Time>2)		//左转尝试最多3次，超过三次，则直接右转
						{
							mode.step_bp=0x50;
							YBS_LeftRight_Time=0;
						}
					
					if(mode.step>=0x88) 			//qz add 20180803:如果是第一步，则直接右转
						mode.step_bp=0x50;
				break;
				case 0x50:
					Speed=TURN_SPEED;
					if(do_action(2, (uint32_t)(80 * Angle_1)))		//右转qz modify 20180801 60-->80
					{
						stop_rap();
						mode.step_bp = 0;
						mode.bump=0;
						mode.bump_flag=false;
						mode.step = 0x40;//0x88; QZ:原来为0x00s
#ifdef YBS_BLACK
						YBS_DISTANCE=YBS_DISTANCE_CONST;
#endif
						mode.bump_time=giv_sys_time;
						return ; 
					}	
				break;
				case 0x60:
					Speed=TURN_SPEED;
					if(do_action(1,(u32)(25*Angle_1)))			//qz:左转
						{
							stop_rap();
							mode.step_bp=0;
							mode.bump=0;
							mode.bump_flag=false;
							mode.step=0x40;
#ifdef YBS_BLACK
							YBS_DISTANCE=YBS_DISTANCE_CONST;
#endif
							mode.bump_time=giv_sys_time;
							return;
						}
					break;
				default :
					mode.bump=0;
					mode.step_bp=0;
					mode.bump_flag=false;
					break;
#else
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
					Speed=TURN_SPEED;
					if(do_action(2,90*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 3:
					mode.bump=0;
					mode.step_bp=0;
					mode.bump_flag=false;
					mode.step=0x40;
					mode.bump_time=giv_sys_time;
					break;
			#endif
			}
		break;	
/******************************************
//      
//                左碰撞
//
******************************************/ 
          
	    case BUMP_ONLY_LEFT://L_BUMP:
	   	case BUMP_ONLY_LEFTMID:
		case BUMP_LEFT_MID:
        switch(mode.step_bp)
			{
				case 0:
					mode.bump_time=giv_sys_time;
					mode.step_bp++;
				break;
				case 1:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed = TURN_SPEED;
					if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))		//ZDK屏蔽 	//QZ:后退
						{
							stop_rap();
							mode.step_bp++;
							mode.bump_time=giv_sys_time;
							return ; 
						}	
				break; 
				case 2:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					mode.step_bp++;
					return;
				case 3:
					Speed = TURN_SPEED;
					//if(do_action(2,(u32)(45*Angle_1)))		//QZ ADD
					if(do_action(2, (uint32_t)(30* Angle_1))		)				//	左转		//QZ MASK	qz:原来为45
					{
						stop_rap();
						mode.step_bp = 0;
						mode.bump=0;
						mode.bump_flag=false;
						mode.step = 0x40;//0x88; 原来为0x88
						mode.bump_time=giv_sys_time;

#ifdef YBS_BLACK
						#if 0
						if(leftbumpcnt==0)
							{
								leftbumptime=giv_sys_time;
								
							}
						leftbumpcnt++;
						if((leftbumpcnt>=3))
							{
								if(giv_sys_time-leftbumptime<100000)		//10s内右碰撞三次
									YBS_DISTANCE=YBS_DISTANCE_LONG;
								leftbumptime=0;
								leftbumpcnt=0;
							}
						#endif
						YBS_DISTANCE+=10;
#endif
						return ; 
					}	
					//read_sideearth(); 	//zdkshield
					break;

				
				default :
					mode.bump=0;
					mode.step_bp=0;
					mode.bump_flag=false;
					mode.step=0x40;
					mode.bump_time=giv_sys_time;
					break;
			}

		return;
		//------------------------------------------------------------------
			
		//------------------------------------------------------------------	
////	    case 10://up_hw			//	红外	 	
////	    case 11://l_hw			//	红外	 	 	
////	    case 12://rm_hw			//	红外	 	
////	    case 13://r_hw			//	红外	 		 	
			//------------------------------------------------------------------	
/*************************************************************************
//      
//                      墙检
//                      墙检      
//                      墙检
//      
*************************************************************************/
#if 1    
/******************************************
//      
//               左墙检
//
******************************************/       
  
/*****************************************
//      
//         墙检      左中             
//      
******************************************/ 
//hzh   已测试，没问题
	//QZ:qz add,原来没有
	    case 8:									 
			switch (mode.step_bp)
				{
					case 0:
						stop_rap();
								mode.bump_time=giv_sys_time;
						mode.step_bp++;
					break;
					case 1:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed = TURN_SPEED;
						//if(do_action(2, (uint32_t)(15 * Angle_1))		)			//右转,原来为40	//qz modify 20-->15 20180710
						//if(do_action(2,20*Angle_1))
						do_action(2,360*Angle_1);
						if(m!=BUMP_WALL_LEFT_MID)
						{
							stop_rap(); //qz add
							mode.step_bp++;
						}	
						//read_sideearth();
											//	return 0;	
						break;
					case 2:
						if(m)
							{
								mode.bump=m;
								mode.step_bp=0;
								return;
							}
						mode.step_bp = 0;
						mode.bump=0;
						mode.step = 0x40;//0x88;  //QZ:原来为0x88
#ifdef YBS_BLACK
						YBS_DISTANCE=YBS_DISTANCE_CONST;
#endif
						mode.bump_time=giv_sys_time;
						break;
					default :
						mode.bump=0;
				}
		break;
#endif	
/*****************************************
//      
//         墙检      中             
//      
******************************************/
	    case 9:								     
			switch(mode.step_bp)
			{
				case 0:
					stop_rap();
					mode.bump_time=giv_sys_time;
					mode.step_bp++;
				break;
				case 1:
					if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
						return;
					Speed = TURN_SPEED;
					if(do_action(2,70*Angle_1))	//向右转90度
					{
						//qz add 20180203
						if(m!=0)
							{
								stop_rap();
								mode.bump=m;
								mode.step_bp=0;
#ifdef YBS_BLACK					//qz add 20180902
								YBS_DISTANCE=YBS_DISTANCE_CONST;
								rightbumpcnt=0;
#endif
								return;
							}
						//qz add end
						stop_rap();
						mode.step_bp++;
					}	
				break;	
				case 2:
					stop_rap();
					mode.step_bp = 0x40;
					mode.bump=0;
					mode.step = 0x40;//0x88;  //QZ:原来为0x88
#ifdef YBS_BLACK
					YBS_DISTANCE=YBS_DISTANCE_CONST;
#endif
					mode.bump_time=giv_sys_time;
					break;
			}
		break;
			
#ifdef YBS_AVOID_SEAT
		case BUMP_SEAT:
			Set_Coordinate_Seat(now_gridx,now_gridy);
			switch(mode.step_bp)
				{
					case 0:
						Speed=TURN_SPEED;
						if(do_action(2,80*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 1:
						l_speed=550;
						r_speed=800;
						radius=(float)(RING_RANGE*l_speed)/(r_speed-l_speed);	//半径约为20mm
						end_length=(u32)((radius+RING_RANGE)*2*3.141592/PULSE_LENGTH/2);	//准备右偏转180度
						start_length=r_ring.all_length;
						mode.step_bp++;
						no_enter=false;
						break;
					case 2:
						l_speed=550;
						r_speed=800;
						enable_rap_no_length(FRONT,l_speed,FRONT,r_speed);
						if(r_ring.all_length-start_length>end_length)
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
									no_enter=true;
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

#if 0
			if((mode.mode==SHIFT)|(mode.mode==EXIT))
				{
					if((mode.bump==BUMP_YMAX_OUT)|(mode.bump==BUMP_YMIN_OUT))
						{
							if(now_gridx!=mode.out_grid[mode.out_grid_cnt])
								{
									mode.out_grid[++mode.out_grid_cnt]=now_gridx;
								}
						}
					else if((mode.bump==BUMP_XMAX_OUT)|(mode.bump==BUMP_XMIN_OUT))
						{
							if(now_gridy!=mode.out_grid[mode.out_grid_cnt])
								{
									mode.out_grid[++mode.out_grid_cnt]=now_gridy;
								}
						}
				}
#endif
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
										case BUMP_LF_OUT:
											tgt_angle=R_Angle_Const;
											break;
										case BUMP_XMIN_OUT:
										case BUMP_RB_OUT:
											tgt_angle=L_Angle_Const;
											break;
										case BUMP_YMAX_OUT:
										case BUMP_RF_OUT:
											tgt_angle=B_Angle_Const;
											break;
										case BUMP_YMIN_OUT:
										case BUMP_LB_OUT:
											tgt_angle=F_Angle_Const;
											break;
									}
//								turn_dir=Get_TurnDir(tgt_angle);
							}
						break;
					case 2:
						Speed=TURN_SPEED;
						turn_dir=Get_TurnDir(tgt_angle);
						do_action(turn_dir,360*Angle_1);
						if(Judge_Yaw_Reach(tgt_angle,TURN_ANGLE_BIOS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 3:
						Speed=HIGH_MOVE_SPEED;
						//if(do_action(3,FARAWAY*CM_PLUS))
						if(do_action_my(3,FARAWAY*CM_PLUS,tgt_angle))
							{
								stop_rap();
								mode.last_outbump=mode.bump;
								mode.bump=0;
								mode.step_bp=0;
								mode.step=0;
								mode.bump_flag=false;
								return;
							}
						if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
							{
								stop_rap();
								mode.step_bp++;
							}
						if((m!=mode.bump)&(m>=BUMP_OUTRANGE))
							{
								stop_rap();
								mode.last_outbump=mode.bump;
								mode.bump=m;
								mode.step_bp=0;
							}
						break;
					case 4:
						Set_Coordinate_Wall(grid.x,grid.y);
						switch(m)
							{
								case BUMP_ONLY_LEFT:
									turn_angle=30;
									break;
#if 0
								case BUMP_ONLY_LEFTMID:
								case BUMP_LEFT_MID:
									turn_angle=60;
									break;
								case BUMP_MID:
									turn_angle=90;
									break;
								case BUMP_ONLY_RIGHTMID:
								case BUMP_RIGHT_MID:
									turn_angle=120;
									break;
								case BUMP_ONLY_RIGHT:
									turn_angle=150;
									break;
#else
								case BUMP_MID:
								case BUMP_ONLY_RIGHT:
									turn_angle=60;
#endif
								default:
									turn_angle=30;
									break;
							}
						mode.step_bp++;
						mode.bump_time=giv_sys_time;
						break;
					case 5:
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
					case 6:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed=TURN_SPEED;
						if(do_action(2,turn_angle*Angle_1))
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
								mode.step_bp++;
							}
						if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
							{
								stop_rap();
								mode.step_bp=4;
							}
						break;
					case 8:
						enable_rap_no_length(FRONT,REVOLUTION_SPEED_LOW,FRONT,REVOLUTION_SPEED_HIGH);
						if((m>=BUMP_ONLY_LEFT)&(m<=BUMP_MID))
							{
								stop_rap();
								mode.step_bp=4;
								return;
							}
						if((m!=mode.bump)&(m>=BUMP_OUTRANGE))
							{
								stop_rap();
								mode.last_outbump=mode.bump;
								mode.bump=m;
								mode.step_bp=0;
								return;
							}
						if(m==0)
							{
								mode.step=0x40;
								mode.last_outbump=mode.bump;
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
						Set_Motion_BackSweep(1);	//设置回扫标志
						Save_Abort_Data();
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
//						turn_dir=Get_TurnDir(tgt_angle);
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
								//mode.bump=m;
								//mode.step_bp=0;
								mode.step_bp++;
								return;
							}
						break;
					case 14:
						Set_Coordinate_Wall(grid.x,grid.y);
						switch(m)
							{
								case BUMP_ONLY_LEFT:
									turn_angle=30;
									break;
#if 0
								case BUMP_ONLY_LEFTMID:
								case BUMP_LEFT_MID:
									turn_angle=60;
									break;
								case BUMP_MID:
									turn_angle=90;
									break;
								case BUMP_ONLY_RIGHTMID:
								case BUMP_RIGHT_MID:
									turn_angle=120;
									break;
								case BUMP_ONLY_RIGHT:
									turn_angle=150;
									break;
#else
								case BUMP_MID:
								case BUMP_ONLY_RIGHT:
									turn_angle=60;
#endif
								default:
									turn_angle=30;
							}
						mode.step_bp++;
						mode.bump_time=giv_sys_time;
						break;
					case 15:
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
					case 16:
						if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
							return;
						Speed=TURN_SPEED;
						if(do_action(2,turn_angle*Angle_1))
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
						enable_rap_no_length(FRONT,REVOLUTION_SPEED_LOW,FRONT,REVOLUTION_SPEED_HIGH);
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
		break;
	}
}

void No_thing1(void)
{
	
}
