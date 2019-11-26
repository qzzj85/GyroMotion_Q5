#include "AAA-include.h"
#define SHIFTMODE_STEP_REACHPOINT1		100
#define SHIFTMODE_STEP_REACHPOINT2		120
#define SHIFTMODE_STEP_TURNGRID			130
#define SHIFTMODE_STEP_NO_REACH			150
#define SHIFTMODE_STEP_PATHPOINT		170
#define	SHIFT_STRAIGHT_SPD				1500

bool abort_shiftybs_flag=false;
static u8 next_action=0;

u8 Analysis_Reach_YAbort(void)
{
	if(Read_CheckPoint_NextAction()==CHECK_BACK2NORMAL)
		{
			if(Judge_GridYPOS_Reach(grid.y_abort,0))
				{
					stop_rap();
					Init_Stop_BackSweep();
					return 1;
				}
		}
	return 0;
}

u8 Analysis_Reach_ExitArea(void)
{
	u8 temp_areano;
	temp_areano=Read_Coordinate_AreaNo(grid.x,grid.y);
	if((temp_areano!=0)&(temp_areano==motion1.exit_area_num))
		return 1;
	else
		return 0;
}

u8 Read_Shift_Bump(void)
{
	u32 data1=0;
	
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
					 Slam_Data.l_bump_flag=true;
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
					 Slam_Data.l_bump_flag=true;
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
					 Slam_Data.l_bump_flag=true;
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
					 Slam_Data.r_bump_flag=true;
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
					 Slam_Data.r_bump_flag=true;
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
					 Slam_Data.r_bump_flag=true;
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
					 Slam_Data.m_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_MID;
	}

#if 0
	if((w_m.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0)|(mode.bump==BUMP_SEAT))		//中墙检靠近墙
				{
					stop_rap();
					mode.bump=9;//W_M;
					mode.step_bp=0;
					mode.bump_time=giv_sys_time;
				}
			return 9;//W_M;
		}
#endif
	 
#ifdef FREE_SKID_CHECK
	if(Check_Free_Sikd())
		{
			Slam_Data.skid_flag=1;
#ifdef SKID_REPORT_TIME
			Slam_Data.skid_report_time=giv_sys_time;
#endif
#ifdef FREE_SKID_ACTION
			stop_rap();
			mode.bump=0xff;
			mode.step_bp=0;
			mode.Info_Abort=1;
#endif
		}
#endif

#ifdef COMMANDER_SEAT_REPORT						//qz add 20180814
	if(r_hw.effectNear|r_hw.effectTopReal)
		{
			Slam_Data.ir_flag=true;
		}
#endif

#ifdef SLAM_FIND_SEAT
	if(SLAM_DOCK)
		{
			data1=ReadHwSign_My();
			if(data1&0xD1111)						//only top signal
				{
					if(mode.bump==0)
						{
							stop_rap();
							mode.bump=0xfe;
							mode.step_bp=0;
							mode.Info_Abort=1;
						}
				}
		}
#endif

#if 0
	if(Check_OutofRange())
		{
			//if(mode.bump<BUMP_OUTRANGE)
			if(mode.bump==0)
				{
					stop_rap();
					mode.bump=BUMP_OUTRANGE;
					mode.step_bp=0;
				}
			return BUMP_OUTRANGE;
		}
#endif
						
	return 0;		
}

void Shift_BumpAction(void)
{
	u8 static turn_dir,turn_angle,bump_time=0,cliff_time=0;
	u8 m,temp_data=0,nextaction=0;
	s8 now_gridx,now_gridy,tgt_gridx1,tgt_gridy1,tgt_gridx2,tgt_gridy2;
	now_gridx=grid.x;now_gridy=grid.y;
	tgt_gridx1=check_point.new_x1;tgt_gridy1=check_point.new_y1;
	tgt_gridx2=check_point.new_x2;tgt_gridy2=check_point.new_y2;
	
	m=Read_Shift_Bump();
	nextaction=Read_CheckPoint_NextAction();

	if(nextaction==CHECK_GOEXIT)
		{
			if(Analysis_Reach_ExitArea())
				{
					stop_rap();
					TRACE("now grid is in the exit area num!!!\r\n");
					TRACE("Abort in %s!!!\r\n",__func__);
					stop_rap();
					mode.step=0;
					mode.bump=0;
					mode.step_bp=0;
					mode.bump_flag=false;
					Delete_All_PathPoint();
					Do_ExitAtion();
					return;
				}
		}
	
	if(Analysis_Stop_StartArea())
		{
			stop_rap();
			TRACE("motion is in start area!!!\r\n");
			Delete_All_PathPoint();
			Init_Cease();
		}

	
	switch(mode.bump)
		{
			case BUMP_LEFT_CLIFF:
			case BUMP_RIGHT_CLIFF:
			case BUMP_MID_CLIFF:
				switch(mode.step_bp)
					{
						case 0:
							Set_Coordinate_Wall(now_gridx,now_gridy);
							mode.step_bp++;
							cliff_time=0;
							break;
						case 1:
							Speed=TOP_MOVE_SPEED;
							if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))
								{
									if(!Read_Cliff())
										{
											mode.step_bp++;
											return;
										}
									else
										{
											Set_Coordinate_Wall(now_gridx,now_gridy);
											cliff_time++;
										}
									if(cliff_time>3)
										{
											error_code=ERROR_LIFT;
											Send_Voice(VOICE_ERROR_DANGER);
											mode.err_code|=WIFI_ERR_EARTH;
											Init_Err();
										}
								}
							break;
						case 2:
							//if(mode.step>=SHIFTMODE_STEP_REACHPOINT1)

							switch(nextaction)
								{
									case CHECK_NEWAREA:
									case CHECK_GOEXIT:
										TRACE("now is CHECK_NEWAREA|EXIT!!\r\n");
										TRACE("Goto Right Shift YBS!!\r\n");
										Init_Shift_RightYBS(2);
										return;
									case CHECK_DOCK:
										TRACE("now is CHECK_DOCK!!!\r\n");
										TRACE("Goto Rigth Dock YBS");
										Init_Dock_RightYBS(0);
										return;
									default:
										break;
								}
							
							if(mode.sub_mode==SHIFTPOINT1)
								{
									temp_data=Judge_YBS_Dir();
									if(temp_data==1)
										{
											Init_Shift_LeftYBS(0);
											return;
										}
									else if(temp_data==2)
										{
											Init_Shift_RightYBS(0);
											return;
										}
									else
										{
											if(check_point.ybs_dir==LEFT)
												{
													Init_Shift_LeftYBS(0);
													return;
												}
											else if(check_point.ybs_dir==RIGHT)
												{
													Init_Shift_RightYBS(0);
													return;
												}
										}
								}
							
							if(mode.sub_mode==SHIFTPOINT2)
								{
									if(nextaction<=CHECK_LEAKSWEEP)
										{
											if(check_point.ybs_dir==LEFT)
												{
													Init_Shift_LeftYBS(0);
													return;
												}
											else if(check_point.ybs_dir==RIGHT)
												{
													Init_Shift_RightYBS(0);
													return;
												}
											
											if(check_point.new_y1<check_point.new_y2)
												{
													if(check_point.next_tgtyaw==F_Angle_Const)
														{
															Init_Shift_RightYBS(0);
															return;
														}
													else
														{
															Init_Shift_LeftYBS(0);
															return;
														}
												}
											else
												{
													if(check_point.next_tgtyaw==F_Angle_Const)
														{
															Init_Shift_LeftYBS(0);
															return;
														}
													else
														{
															Init_Shift_RightYBS(0);
															return;
														}
												}
										}
								}
							if((mode.bump==BUMP_LEFT_CLIFF))
								{
									Init_Shift_LeftYBS(0);
								}
							else
								{
									Init_Shift_RightYBS(0);
								}
							return;
					}
				break;
			case BUMP_ONLY_LEFTMID:
			case BUMP_ONLY_LEFT:
			case BUMP_LEFT_MID:
			case BUMP_ONLY_RIGHTMID:
			case BUMP_ONLY_RIGHT:
			case BUMP_RIGHT_MID:
			case BUMP_MID:
				//if((now_gridx==check_point.new_x2)&(now_gridy==check_point.new_y2)&(mode.step_bp<10))
#if 0
				if((now_gridx==tgt_gridx2)&(Judge_GridYPOS_Reach(tgt_gridy2,0))&(mode.step_bp<10))
					{
						mode.step_bp=10;
					}
#endif
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
							Set_Coordinate_Wall(now_gridx,now_gridy);
							mode.step_bp++;
							bump_time++;
							break;
						case 2:
							Speed=BUMP_BACK_SPEED;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
									mode.bump_time=giv_sys_time;
									
									if(bump_time>2)
										{
											bump_time=0;
											//if(mode.step>=SHIFTMODE_STEP_REACHPOINT1)
											switch(nextaction)
												{
													case CHECK_NEWAREA:
													case CHECK_GOEXIT:
														TRACE("now is CHECK_NEWAREA|EXIT!!\r\n");
														TRACE("Goto Right Shift YBS!!\r\n");
														Init_Shift_RightYBS(2);
														return;
													case CHECK_DOCK:
														TRACE("now is CHECK_DOCK!!!\r\n");
														TRACE("Goto Rigth Dock YBS");
														Init_Dock_RightYBS(0);
														return;
													default:
														break;
												}
#if 0											
											if(nextaction==CHECK_GOEXIT)
												{
													if((mode.bump>=BUMP_ONLY_LEFT)&(mode.bump<=BUMP_LEFT_MID))
														{
															Init_ShiftExit_LeftYBS(0);
															return;
														}
													else
														{
															Init_ShiftExit_RightYBS(0);
															return;
														}
												}
											else if(nextaction==CHECK_DOCK)
												{
													Init_Dock_RightYBS(0);
													return;
												}
#endif
									//////////以下为nextaction小于CHECK_NEWAREA////////////////
											if(mode.sub_mode==SHIFTPOINT1)
												{
													temp_data=Judge_YBS_Dir();
													if(temp_data==1)
														{
															Init_Shift_LeftYBS(0);
															return;
														}
													else if(temp_data==2)
														{
															Init_Shift_RightYBS(0);
															return;
														}
													
													else
														{
															if(check_point.ybs_dir==LEFT)
																{
																	Init_Shift_LeftYBS(0);
																	return;
																}
															else if(check_point.ybs_dir==RIGHT)
																{
																	Init_Shift_RightYBS(0);
																	return;
																}
														}
												}
											
											if(mode.sub_mode==SHIFTPOINT2)
												{
													if(nextaction<=CHECK_LEAKSWEEP)
														{
															if(check_point.ybs_dir==LEFT)
																{
																	Init_Shift_LeftYBS(0);
																	return;
																}
															else if(check_point.ybs_dir==RIGHT)
																{
																	Init_Shift_RightYBS(0);
																	return;
																}
															
															if(check_point.new_y1<check_point.new_y2)
																{
																	if(check_point.next_tgtyaw==F_Angle_Const)
																		{
																			Init_Shift_RightYBS(0);
																			return;
																		}
																	else
																		{
																			Init_Shift_LeftYBS(0);
																			return;
																		}
																}
															else
																{
																	if(check_point.next_tgtyaw==F_Angle_Const)
																		{
																			Init_Shift_LeftYBS(0);
																			return;
																		}
																	else
																		{
																			Init_Shift_RightYBS(0);
																			return;
																		}
																}
														}
												}
											if((mode.bump>=BUMP_ONLY_LEFT)&(mode.bump<=BUMP_LEFT_MID))
												{
													Init_Shift_LeftYBS(0);
												}
											else
												{
													Init_Shift_RightYBS(0);
												}
											return;
										}
									else
										{
											if(((now_gridx==tgt_gridx1)&(now_gridy==tgt_gridy1))|((now_gridx==tgt_gridx2)&(now_gridy==tgt_gridy2)))
												{
													if(nextaction<=CHECK_NEWAREA)
														{
															mode.step_bp=6;
															return;
														}
												}
										}
									switch(mode.bump)
										{
											case BUMP_ONLY_LEFTMID:
												turn_angle=60;
												turn_dir=2;
												break;
											case BUMP_ONLY_LEFT:
												turn_angle=20;
												turn_dir=2;
												break;
											case BUMP_LEFT_MID:
												turn_angle=40;
												turn_dir=2;
												break;
											case BUMP_ONLY_RIGHTMID:
												turn_angle=60;
												turn_dir=1;
												break;
											case BUMP_ONLY_RIGHT:
												turn_angle=20;
												turn_dir=1;
												break;
											case BUMP_RIGHT_MID:
												turn_angle=40;
												turn_dir=1;
												break;
											case BUMP_MID:
												turn_angle=90;
												turn_dir=2;
												break;
											default:
												turn_angle=30;
												break;
										}
								}
							break;
						case 3:
							if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
								return;
							Speed=TURN_SPEED;
							if(do_action(turn_dir,turn_angle*Angle_1))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 4:
							Speed=MID_MOVE_SPEED;
							if(do_action(3,4*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
#if 0
							if(grid.y==tgt_gridy2)
								{
									stop_rap();
									mode.bump=0;
									mode.step_bp=0;
									mode.bump_flag=false;
									mode.step=SHIFTMODE_STEP_REACHPOINT1;
									return;
								}
#endif
							if(m)
								{
									stop_rap();
									mode.bump=m;
									mode.step_bp=0;
								}
							break;
						case 5:
							if(turn_dir==2)
								enable_rap_no_length(FRONT,REVOLUTION_SPEED_LOW,FRONT,REVOLUTION_SPEED_HIGH);
							else
								enable_rap_no_length(FRONT,REVOLUTION_SPEED_HIGH,FRONT,REVOLUTION_SPEED_LOW);
							if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
								{
									stop_rap();
									//if(mode.step>=SHIFTMODE_STEP_REACHPOINT1)
									if((mode.sub_mode==SHIFTPOINT2))
										{
											mode.step_bp++;
											return;
										}
									mode.bump=0;
									mode.step_bp=0;
									mode.bump_flag=false;
									mode.step=5;		//从头开始
									bump_time=0;
									return;
								}
							if(m)
								{
									stop_rap();
									mode.bump=m;
									mode.step_bp=0;
								}
							break;
						case 6:
							if(nextaction>CHECK_NEWAREA)
								{
									mode.bump=0;
									mode.step_bp=0;
									mode.bump_flag=false;
									bump_time=0;
									mode.step=0;

									Area_Check(0);
									Init_Shift_Point1(0);
								}
							else if(nextaction==CHECK_BACKSWEEP)
								{
									TRACE("Abort for CHECK_BACKSWEEP in %s!!!\r\n",__func__);
									mode.bump=0;
									mode.step_bp=0;
									mode.bump_flag=false;
									bump_time=0;
									mode.step=0;
									
									if(Analysis_Back_Leak()==0)
										Init_Stop_BackSweep();
									else if((tgt_gridx1!=check_point.new_x1)&(tgt_gridy1!=check_point.new_y1))
										Init_Shift_Point1(1);
								}
							else if(nextaction==CHECK_BACK2NORMAL)
								{
									mode.bump=0;
									mode.step_bp=0;
									mode.bump_flag=false;
									bump_time=0;
									mode.step=0;
									return;
								}
							else
								{
									Area_Check(1);
									Init_Shift_Point1(1);
								}
							break;
						case 10:
							Speed=MID_MOVE_SPEED;
							if(do_action(4,2*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 11:
							bump_time=0;
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							mode.step=SHIFTMODE_STEP_REACHPOINT2;
							Init_Shift_Point2();
							return;
					}
				break;
		}
}

//function:从当前点移动到check_point的第一个点
//input:
//0:无需避开沿边，直接分析路径并移动
//1:需要先避开沿边，然后再分析路径并移动
//2:先到达转移点TURN_GRID，然后再分析路径并移动
void Init_Shift_Point1(u8 pre_action)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
		
	/******初始化设置的值********************/
	clr_ram();
//	ReInitAd();
	Enable_earth();
	Enable_wall();
	enable_hwincept();				//允许红外接收电源
	Enable_Speed(); 				//允许速度发送
	Init_Action();
	
	mode.mode = SHIFT;			
	mode.sub_mode = SHIFTPOINT1;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.status=1;
	switch(pre_action)
		{
			case 0:					//无需避开沿边
				mode.step=5;
				break;
			case 1:					//需要避开沿边
				mode.step=0;
				break;
			case 2:					//需要先到TURN_GRID
				mode.step=SHIFTMODE_STEP_TURNGRID;
				break;
		}
//		WriteWorkState();

//	Set_Motion_BackSweep(0);

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init SHIFTPOINT1 Mode Complete!\r\n");
#endif
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//清除接收缓存qz add 20180703
	
	if(dis_err_code==DIS_ERROR_DOCK_FAIL)		//qz add 20180710
		{
			dis_err_code=0;
			error_code=0;
		}
	
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgt_yaw);
	Logout_CheckPoint();
	Delete_All_PathPoint();
	delay_ms(500);
}


void Do_Shift_Point1(void)
{
	static u32 length;
	static u8 turn_dir,path_num,temp_nextaction,turn_angle;
	static s8 temp_tgty,temp_tgtx;
//	static short tgt_angle;
	s8 tgt_gridy1,tgt_gridx1,tgt_gridy2,tgt_gridx2,temp_gridy,temp_gridx,now_gridx,now_gridy;
	u8 temp_result,i;
	tgt_gridy1=check_point.new_y1;tgt_gridx1=check_point.new_x1;
	tgt_gridy2=check_point.new_y2;tgt_gridx2=check_point.new_x2;
	now_gridx=grid.x;now_gridy=grid.y;
	static struct PATH_POINT *temp_point=NULL;

	temp_nextaction=Read_CheckPoint_NextAction();
	
	ACC_DEC_Curve();

	u8 abnormal=Read_Protect();
	if(mode.abnormity)
		{
			Action_Protect_My(abnormal);
#ifdef FREE_SKID_INDEP_CHECK
			Free_Skid_Indep.check_flag=false;
#endif

#ifdef YBS_DIS_RESTORE
			Disable_Rotate_Angle();
#endif
			//mode.speed_up=false;		//qz add 20181225

			return;
		}
	
	if(Analysis_Check_Dock())
		return;

	if(Analysis_Reach_YAbort())
		return;
	
	Shift_BumpAction();
	if(mode.bump)
		return;
	if(mode.sub_mode!=SHIFTPOINT1)
		return;
	switch(mode.step)
		{
			case 0:
				mode.time=giv_sys_time;
				mode.step++;
				break;
			case 1:
				if(giv_sys_time-mode.time<1000)
					return;
				if(mode.last_sub_mode==YBS_SUB_LEFT)
					turn_dir=2;
				else
					turn_dir=1;
				mode.step++;
				break;
			case 2:

				Speed=TURN_SPEED;
				if(do_action(turn_dir,70*Angle_1))				//先旋转到远离刚才沿边的角度
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 3:
				Speed=MID_MOVE_SPEED;
				if(do_action(3,10*CM_PLUS))						//脱离沿边位置20cm
					{
						stop_rap();
						if(!Read_Coordinate_Wall(now_gridx,now_gridy))
							mode.step+=2;
						else
							mode.step++;
						mode.time=giv_sys_time;
					}
				break;
			case 4:
				Speed=MID_MOVE_SPEED;
				do_action(3,5*CM_PLUS);
				if(Read_Coordinate_Clean(now_gridx,now_gridy))
					mode.step++;
				break;
			case 5:
				if(giv_sys_time-mode.time<1000)
					return;
				if((now_gridx==tgt_gridx1)&(now_gridy==tgt_gridy1))		//当前点刚好处于目标点
					{
						TRACE("now grid is match the check_point new_1!!\r\n");
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
					}
				else if((now_gridx==tgt_gridx2)&(now_gridy==tgt_gridy2))
					{
						TRACE("now grid is match the check_point new_2!!\r\n");
						Init_Shift_Point2();
					}
#if 1
				else
					{
						mode.step++;
					}
#else
				else if(grid.y==tgt_gridy1)						//当前点与目标点横坐标不一致，纵坐标重叠
					{
						TRACE("now grid Y is match the check_point new!!\r\n");
						TRACE("but grid X is not match!!!\r\n");
						mode.step=80;
					}
				else if(grid.x==tgt_gridx1)
					{
						TRACE("now grid X is match the check_point new!!\r\n");
						TRACE("but grid Y is not match!!!\r\n");
						mode.step=100;
					}
				else
					{
						TRACE("now grid XY is not match the check_point new!!\r\n");
						mode.step++;							//当前点与目标点横纵坐标都不一致。
					}
#endif
				break;
			case 6:
				temp_result=Find_Directly_Way(check_point.new_x1,check_point.new_y1);		//寻找是否能直接到的路径
				TRACE("analysis direct way reuslt=%d\r\n",temp_result);
				switch(temp_result)
					{
						case 0:
							TRACE("motion can't directly move!!\r\n");
//							TRACE("prepare indirect way!!\r\n");
//							TRACE("Goto mode step 60\r\n");
//							mode.step=60;
							mode.step++;
							//if(Read_CurrNode_AreaNO()>1)
								{
									if(Read_CheckPoint_NextAction()==CHECK_DOCK)
										mode.step=10;
								}
							break;
						case 1:
							TRACE("motion can directly move with Y-axis first,X-axis Second!!\r\n");
							mode.step=20;
							break;
						case 2:
							TRACE("motion can directly move with X-axis first,Y-axis Second!!\r\n");
							mode.step=40;
							break;
						default:
							TRACE("motion can't directly move!!\r\n");
							TRACE("prepare indirect way!!\r\n");
							mode.step=60;
							break;
					}
				break;
			case 7:
#if 0
				switch(temp_nextaction)
					{
						case CHECK_NEWAREA:
						case CHECK_GOEXIT:
							TRACE("now is CHECK_NEWAREA|EXIT!!\r\n");
							TRACE("Goto Right Shift YBS!!\r\n");
							Init_Shift_RightYBS(2);
							return;
						case CHECK_DOCK:
							TRACE("now is CHECK_DOCK!!!\r\n");
							TRACE("Goto Rigth Dock YBS");
							Init_Dock_RightYBS(0);
							return;
						default:
							break;
					}
#endif
				//temp_result=Find_PathPoint_Way(check_point.new_x1,check_point.new_y1);
				temp_result=Find_PathPoint_NoWall_Way(check_point.new_x1,check_point.new_y1);
				if(temp_result)
					{
						TRACE("motion can use pathpoint move!!!\r\n");
						TRACE("go to PATHPOINT step!!!\r\n");
						mode.step=SHIFTMODE_STEP_PATHPOINT;
					}
				else if(temp_nextaction>=CHECK_NEWAREA)
					{
#if 0
						switch(temp_nextaction)
							{
								case CHECK_NEWAREA:
								case CHECK_GOEXIT:
									TRACE("now is CHECK_NEWAREA|EXIT!!\r\n");
									TRACE("Goto Right Shift YBS!!\r\n");
									Init_Shift_RightYBS(2);
									return;
								case CHECK_DOCK:
									TRACE("now is CHECK_DOCK!!!\r\n");
									TRACE("Goto Rigth Dock YBS");
									Init_Dock_RightYBS(0);
									return;
								default:
									break;
							}
#else
						if(temp_nextaction==CHECK_DOCK)
							{
								TRACE("now is CHECK_DOCK!!!\r\n");
								TRACE("Goto Rigth Dock YBS");
								Init_Dock_RightYBS(0);
								return;
							}
						
						temp_result=Judge_YBS_Dir();
						if(temp_result==1)
							{
								TRACE("now is CHECK_NEWAREA|EXIT!!\r\n");
								TRACE("Goto Left Shift YBS!!\r\n");
								Init_Shift_LeftYBS(0);
								return;
							}
						else
							{
								TRACE("now is CHECK_NEWAREA|EXIT!!\r\n");
								TRACE("Goto Right Shift YBS!!\r\n");
								Init_Shift_RightYBS(2);
								return;
							}
#endif
					}
				else
					{
						TRACE("motion can't use pathpoint move!!!\r\n");
						TRACE("prepare indirect way!!\r\n");
						TRACE("Goto mode step 60\r\n");
						mode.step=60;
					}
				break;
			case 10:
				
				//TRACE("motion need dock and area num=%d\r\n",Read_CurrNode_AreaNO());
				temp_result=Find_PathPoint_WayAll(check_point.new_x1,check_point.new_y1);
				if(temp_result)
					{
						TRACE("motion can use pathpoint move!!!\r\n");
						TRACE("go to PATHPOINT step!!!\r\n");
						mode.step=SHIFTMODE_STEP_PATHPOINT;
					}
				else
					{
						TRACE("Call this in %s %d\r\n",__func__,__LINE__);
						Init_Docking();
					}
				break;
				
			//可以先从Y方向，再从X方向直接到目标点
			case 20:
				if(now_gridy>tgt_gridy1)
					{
						motion1.tgt_yaw=L_Angle_Const;
					}
				else if(now_gridy<tgt_gridy1)
					{
						motion1.tgt_yaw=R_Angle_Const;
					}
				else
					{
						mode.step=23;
						return;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				mode.step++;
				break;
			case 21:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
						length=abs(now_gridy-tgt_gridy1)*20;
						length+=20;
					}
				break;
			case 22:
				Speed=FAST_MOVE_SPEED;
				//Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=20;
					}
				if(Judge_GridYPOS_Reach(tgt_gridy1, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 23:
				if(now_gridx>tgt_gridx1)
					{
						motion1.tgt_yaw=B_Angle_Const;
					}
				else if(now_gridx<tgt_gridx1)
					{
						motion1.tgt_yaw=F_Angle_Const;
					}
				else
					{
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
						return;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				mode.step++;
				break;
			case 24:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
						length=abs(now_gridx-tgt_gridx1)*20;length+=20;
					}
				break;
			case 25:
				//Speed=HIGH_MOVE_SPEED;
				Speed=FAST_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=20;
					}
				if(Judge_GridXPOS_Reach(tgt_gridx1, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
					}
				break;

			//可以先从X方向，再从Y方向直接到目标点
			case 40:
				if(now_gridx>tgt_gridx1)
					{
						motion1.tgt_yaw=B_Angle_Const;
					}
				else if(now_gridx<tgt_gridx1)
					{
						motion1.tgt_yaw=F_Angle_Const;
					}
				else
					{
						mode.step=43;
						return;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				mode.step++;
				break;
			case 41:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
						length=abs(now_gridx-tgt_gridx1)*20;length+=20;
					}
				break;
			case 42:
				//Speed=HIGH_MOVE_SPEED;
				Speed=FAST_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=40;
					}
				if(Judge_GridXPOS_Reach(tgt_gridx1, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 43:
				if(now_gridy>tgt_gridy1)
					{
						motion1.tgt_yaw=L_Angle_Const;
					}
				else if(now_gridy<tgt_gridy1)
					{
						motion1.tgt_yaw=R_Angle_Const;
					}
				else
					{
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
						return;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				mode.step++;
				break;
			case 44:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
						length=abs(now_gridy-tgt_gridy1)*20;length+=20;
					}
				break;
			case 45:
				//Speed=HIGH_MOVE_SPEED;
				Speed=FAST_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=43;
					}
				if(Judge_GridYPOS_Reach(tgt_gridy1, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
					}
				break;

			//不能直接到达，需要间接的找出路
			case 60:
				temp_result=Find_Indirectly_Way(tgt_gridx1,tgt_gridy1);
				TRACE("analysis indirect way reuslt=%d\r\n",temp_result);
				if(temp_result==1)
					{
						TRACE("motion can indirectly move to tgt!!!\r\n");
						mode.step=70;
					}
				else if(temp_result==2)
					{
						TRACE("motion can indirectly move to tgt,but dir opposite!!!\r\n");
						mode.step++;
					}
				else
					{
						Logout_CheckPoint();
						//Logout_Area_Coordinate();
						TRACE("motion can't indirectly move to tgt!!\r\n");
						mode.step=SHIFTMODE_STEP_NO_REACH;
						//while(1);
					}
				break;

			//可以间接到达的处理过程,但是首先需要反方向寻找
			case 61:
				if((now_gridx==tgt_gridx1)&(now_gridy==tgt_gridy1))		//当前点刚好处于目标点
					{
						TRACE("now grid is match the check_point new_1!!\r\n");
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
						return;
					}
				else if(now_gridy==tgt_gridy1) 					//当前点与目标点横坐标不一致，纵坐标重叠
					{
						TRACE("now grid Y is match the check_point new!!\r\n");
						TRACE("but grid X is not match!!!\r\n");
						mode.step=80;
						return;
					}
				else
					{
						temp_gridx=now_gridx;
						temp_gridy=now_gridy;
						mode.step++;
					}
				break;
			case 62:
				if(temp_gridy<tgt_gridy1)
					temp_tgtx=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy-1);
				else
					temp_tgtx=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy+1);

				if(temp_tgtx>temp_gridx)
					motion1.tgt_yaw=F_Angle_Const;
				else
					motion1.tgt_yaw=B_Angle_Const;
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				length=abs(temp_gridx-temp_tgtx)*20;length+=20;
				mode.step++;
				break;
			case 63:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 64:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 65:
				if(temp_gridy<tgt_gridy1)
					{
						motion1.tgt_yaw=L_Angle_Const;
					}
				else
					{
						motion1.tgt_yaw=R_Angle_Const;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				break;
			case 66:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 67:
				Speed=HIGH_MOVE_SPEED;
				do_action_my(3,20*CM_PLUS,motion1.tgt_yaw);
				if(Judge_GridYPOS_Reach(tgt_gridy1, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=60;
					}
				break;
				
				
			//可以间接到达的处理过程
			case 70:				
				if((now_gridx==tgt_gridx1)&(now_gridy==tgt_gridy1))		//当前点刚好处于目标点
					{
						TRACE("now grid is match the check_point new_1!!\r\n");
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
						return;
					}
				else if(now_gridy==tgt_gridy1) 						//当前点与目标点Y坐标一致，X坐标不一致
					{
						TRACE("now grid Y is match the check_point new!!\r\n");
						TRACE("but grid X is not match!!!\r\n");
						mode.step=80;
						return;
					}
				temp_tgty=Find_MaxApproch_Y(tgt_gridy1);			//目标Y坐标能够达到无碰撞区域的最大Y坐标
				if(temp_tgty>now_gridy)
					motion1.tgt_yaw=R_Angle_Const;
				else if(temp_tgty<now_gridy)
					motion1.tgt_yaw=L_Angle_Const;
				else
					{
						if(now_gridx<tgt_gridx1)
							{
								motion1.tgt_yaw=F_Angle_Const;
								temp_tgtx=now_gridx+1;
							}
						else
							{
								motion1.tgt_yaw=B_Angle_Const;
								temp_tgtx=now_gridx-1;
							}
//						turn_dir=Get_TurnDir(motion1.tgt_yaw);
						length=30;
						mode.step=74;						//直行一个X坐标
						return;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				length=abs(now_gridy-temp_tgty)*20;length+=20;
				mode.step++;
				break;
			case 71:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 72:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				if(Judge_GridYPOS_Reach(temp_tgty, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 73:
				if((now_gridx==tgt_gridx2)&(now_gridy==tgt_gridy2))
					{
						TRACE("now grid is match the check_point new_2!!\r\n");
						mode.step=SHIFTMODE_STEP_REACHPOINT2;
						Init_Shift_Point2();
						return;
					}
				if((now_gridx==tgt_gridx1)&(now_gridy==tgt_gridy1))		//当前点刚好处于目标点
					{
						TRACE("now grid is match the check_point new_1!!\r\n");
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
						return;
					}
				else if(now_gridy==tgt_gridy1)						//当前点与目标点横坐标不一致，纵坐标重叠
					{
						TRACE("now grid Y is match the check_point new!!\r\n");
						TRACE("but grid X is not match!!!\r\n");
						mode.step=80;
						return;
					}
#if 0
				temp_tgtx=Find_MaxApproch_X(tgt_gridx1);
#else
				if(now_gridy<tgt_gridy1)
					//temp_tgtx=Find_Adjoin_Gap(grid.x,grid.y,grid.y+1);
					temp_tgtx=Find_Adjoin_Gap(tgt_gridx1,now_gridy,now_gridy+1);		//寻找距离目标点X坐标最近的空隙
				else
					//temp_tgtx=Find_Adjoin_Gap(grid.x,grid.y,grid.y-1);
					temp_tgtx=Find_Adjoin_Gap(tgt_gridx1,now_gridy,now_gridy-1);
#endif
				if(temp_tgtx==0x7f)
					{
						//TRACE("hang at %s %d\r\n",__func__,__LINE__);
						//while(1);
						mode.step=SHIFTMODE_STEP_NO_REACH;
						return;
					}
				if(temp_tgtx>now_gridx)
					motion1.tgt_yaw=F_Angle_Const;
				else if(temp_tgtx<now_gridx)
					motion1.tgt_yaw=B_Angle_Const;
				else										//空隙区域就是当前X坐标
					{
						if(now_gridy<tgt_gridy1)
							{
								motion1.tgt_yaw=R_Angle_Const;
								temp_tgty=now_gridy+1;
							}
						else
							{
								motion1.tgt_yaw=L_Angle_Const;
								temp_tgty=now_gridy-1;
							}
//						turn_dir=Get_TurnDir(motion1.tgt_yaw);
						length=30;
						mode.step=71;						//直行一个Y坐标
						return;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				length=abs(now_gridx-temp_tgtx)*20;length+=20;
				mode.step++;
				break;
			case 74:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 75:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=70;
					}
				if(Judge_GridXPOS_Reach(temp_tgtx, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=70;
					}
				break;
			case 76:
				temp_tgtx=Find_MaxApproch_X(tgt_gridx1);
				if(temp_tgtx!=tgt_gridx1)
					{
						if(temp_tgtx>tgt_gridx1)
							motion1.tgt_yaw=B_Angle_Const;
						else
							motion1.tgt_yaw=F_Angle_Const;
//						turn_dir=Get_TurnDir(motion1.tgt_yaw);
						length=abs(temp_tgtx-tgt_gridx1)*20;length+=20;
						mode.step=74;
					}
				else
					{
						
					}
			case 80:
				if((now_gridx==tgt_gridx1)&(now_gridy==tgt_gridy1))		//当前点刚好处于目标点
					{
						TRACE("now grid is match the check_point new_1!!\r\n");
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
						return;
					}
				if(now_gridx<tgt_gridx1)
					{
						motion1.tgt_yaw=F_Angle_Const;
					}
				else
					{
						motion1.tgt_yaw=B_Angle_Const;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				length=abs(now_gridx-tgt_gridx1)*20;length+=20;
				mode.step++;
				break;
			case 81:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 82:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						if(now_gridx!=tgt_gridy1)
							{
								if(now_gridy!=tgt_gridy1)
									{
										mode.time=giv_sys_time;
										mode.step=5;
									}
								else
									{
										mode.step=80;
									}
							}
					}
				if(Judge_GridXPOS_Reach(tgt_gridx1, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
					}
				break;

			//到达目标点1后的动作处理,准备前往目标点2
			case SHIFTMODE_STEP_REACHPOINT1:
				stop_rap();
				if((now_gridx!=tgt_gridx1)|(now_gridy!=tgt_gridy1))
					mode.step=5;
				else
					Init_Shift_Point2();
				break;
			//需要先到达TURN_GRID的情况下的处理过程
			//TURN_GRID是沿边时的路径分析结果
			case SHIFTMODE_STEP_TURNGRID:
				if((now_gridy==turn_grid.gridy)&(now_gridx==turn_grid.gridx))
					{
						stop_rap();
						mode.step=5;
						if(check_point.use_pathpoint)
							mode.step=SHIFTMODE_STEP_PATHPOINT;
						return;
					}
				else
					{
						if(now_gridx==turn_grid.gridx)
							mode.step=131;
						//else if(grid.y==turn_grid.gridy)
						else
							mode.step=135;
					}
				break;
			case 131:
				if(now_gridy>turn_grid.gridy)
					{
						motion1.tgt_yaw=L_Angle_Const;
					}
				else
					{
						motion1.tgt_yaw=R_Angle_Const;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				length=30;
				mode.step++;
				break;
			case 132:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 133:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
						return;
					}
				if(Judge_GridYPOS_Reach(turn_grid.gridy, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 134:
				mode.step=SHIFTMODE_STEP_TURNGRID;
				break;
			case 135:
				if(now_gridx>turn_grid.gridx)
					{
						motion1.tgt_yaw=B_Angle_Const;
					}
				else
					{
						motion1.tgt_yaw=F_Angle_Const;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				length=30;
				mode.step++;
				break;
			case 136:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 137:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
						return;
					}
				if(Judge_GridXPOS_Reach(turn_grid.gridx, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 138:
				mode.step=SHIFTMODE_STEP_TURNGRID;
				break;

			///////////////使用PATHPOINT路径行走////////////////////
			///////////////使用PATHPOINT路径行走////////////////////
			case SHIFTMODE_STEP_PATHPOINT:
				stop_rap();
				TRACE("begin ShiftMode Step Pathpoint in %s!!!\r\n",__func__);
				temp_point=NULL;
				path_num=1;
				check_point.use_pathpoint=false;
				mode.step++;				
				break;
			case 171:
				temp_gridx=now_gridx;
				i=0;
				while(path_num+i<=path_length)
					{
						temp_point=Get_PathPoint(path_num+i);
						if(temp_point==NULL)
							{
								TRACE("temp_point is NULL in %s!!!\r\n",__func__);
								mode.step=5;
								return;
							}
						if(temp_gridx==temp_point->path_grid.gridx)
							{
								i++;
							}
						else
							{
								break;
							}
					}
				if(i)
					{
						path_num=path_num+i-1;
						temp_point=Get_PathPoint(path_num);
						if(temp_point==NULL)
							{
								TRACE("temp_point is NULL in %s!!!\r\n",__func__);
								mode.step=5;
								return;
							}
						mode.step=175;
					}
				else
					{
						mode.step++;
					}
				TRACE("next path point gridx=%d gridy=%d\r\n",temp_point->path_grid.gridx,temp_point->path_grid.gridy);
				TRACE("next m.step=%d in m.step 171\r\n",mode.step);
				break;
			//当前X轴坐标与下一个PATH点（即公共点）的X轴坐标不相等处理
			case 172:
				temp_gridx=temp_point->path_grid.gridx;
				temp_gridy=temp_point->path_grid.gridy;
				if(temp_gridx>now_gridx)
					{
						motion1.tgt_yaw=F_Angle_Const;
					}
				else if(temp_gridx<now_gridx)
					{
						motion1.tgt_yaw=B_Angle_Const;
					}
				else									//当前点X轴坐标与下一个PATH点（即公共点）的X轴坐标一致
					{									//判断Y轴坐标
						if(temp_gridy==now_gridy)		//当前点Y轴坐标与下一个PATH点（即公共点）的Y轴坐标一致
							{
								mode.step=178;			//准备下下个点
								return;
							}
						else							//不一致
							{	
								mode.step=175;			//前往Y轴坐标处理
								return;
							}
					}
				length=abs(temp_gridx-now_gridx)*20+20;
				mode.step++;
			case 173:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
						mode.time=giv_sys_time;
					}
				break;
			case 174:
				temp_gridy=temp_point->path_grid.gridy;
				temp_gridx=temp_point->path_grid.gridx;
				Speed=HIGH_MOVE_SPEED;
				if((w_m.high_sign==NEAR)|(w_r.high_sign==NEAR)|(w_l.high_sign==NEAR)|(w_lm.high_sign==NEAR)|(w_rm.high_sign==NEAR))
					{l_rap.rap=MID_MOVE_SPEED;r_rap.rap=MID_MOVE_SPEED;}
				else if((length>40)&(giv_sys_time-mode.time<10000))
					{l_rap.rap=FAST_MOVE_SPEED;r_rap.rap=FAST_MOVE_SPEED;}
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{	
						stop_rap();
						mode.step=SHIFTMODE_STEP_PATHPOINT;
					}
				if(Judge_GridXPOS_Reach(temp_gridx, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=172;
					}
				break;
			//Y轴坐标与下一PATH点Y轴坐标（即公共点坐标）处理
			case 175:
				temp_gridy=temp_point->path_grid.gridy;
				temp_gridx=temp_point->path_grid.gridx;
				if(temp_gridy>now_gridy)
					{
						motion1.tgt_yaw=R_Angle_Const;
					}
				else if(temp_gridy<now_gridy)
					{
						motion1.tgt_yaw=L_Angle_Const;
					}
				else
					{
						if(now_gridx==temp_gridx)
							{
								mode.step=178;
								return;
							}
						else
							{
								mode.step=172;
								return;
							}
					}
				length=abs(temp_gridy-now_gridy)*20+20;
				mode.step++;
				break;
			case 176:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
						mode.time=giv_sys_time;
					}
				break;
			case 177:
				Speed=HIGH_MOVE_SPEED;
				temp_gridy=temp_point->path_grid.gridy;
				if((w_m.high_sign==NEAR)|(w_r.high_sign==NEAR)|(w_l.high_sign==NEAR)|(w_lm.high_sign==NEAR)|(w_rm.high_sign==NEAR))
					{l_rap.rap=MID_MOVE_SPEED;r_rap.rap=MID_MOVE_SPEED;}
				else if((length>40)&(giv_sys_time-mode.time<10000))
					{l_rap.rap=FAST_MOVE_SPEED;r_rap.rap=FAST_MOVE_SPEED;}
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
					}
				if(Judge_GridYPOS_Reach(temp_gridy, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=175;
					}
				break;
			case 178:
				if((now_gridx==check_point.new_x1)&(now_gridy==check_point.new_y1))
					{
						stop_rap();
						TRACE("now grid is match new1,out path point way!!!\r\n");
						mode.step=SHIFTMODE_STEP_REACHPOINT1;
						if(Delete_All_PathPoint())
							motion1.pathpoint_ok=false;
						return;
					}
				path_num++;
				mode.step=SHIFTMODE_STEP_PATHPOINT+1;
				if(path_num>path_length)
					{
						TRACE("path num has out of range!!!\r\n");
						if(Delete_All_PathPoint())
							motion1.pathpoint_ok=false;
						mode.step=5;
					}
				break;
			

			
			//间接也无法到达new_1点的漏扫，的处理过程
			case SHIFTMODE_STEP_NO_REACH:
				TRACE("Enter mode step can't reach point1");
				TRACE("Need YBS!!\r\n");

				temp_result=Judge_YBS_Dir();
				if(temp_result==1)
					{
						Init_Shift_LeftYBS(0);
						return;
					}
				else if(temp_result==2)
					{
						Init_Shift_RightYBS(0);
						return;
					}

				if(Read_CheckPoint_NextAction()==CHECK_GOEXIT)
					{
						if(mode.last_sub_mode==YBS_SUB_LEFT)
							Init_ShiftExit_LeftYBS(1);
						else if(mode.last_sub_mode==YBS_SUB_RIGHT)
							Init_ShiftExit_RightYBS(1);
						else
							Init_ShiftExit_RightYBS(1);
					}
				else
					{
						if(mode.last_sub_mode==YBS_SUB_LEFT)
							Init_Shift_LeftYBS(1);
						else if(mode.last_sub_mode==YBS_SUB_RIGHT)
							Init_Shift_RightYBS(1);
						else
							Init_Shift_RightYBS(1);
					}
				break;
			case 151:
				break;
		}
}

void Init_Shift_Point2(void)
{
	if((grid.y!=check_point.new_y1)|(grid.x!=check_point.new_x1))						//如果当前坐标不等于new_1坐标
		{
			if((grid.y!=check_point.new_y2)|(grid.x!=check_point.new_x2))				//如果当前坐标也不等于new_2坐标
				{
					TRACE("grid is not macth check_point new1|new2 in %s\r\n",__func__);
					TRACE("Go back shiftpoint1 again!!\r\n");
					Init_Shift_Point1(0);
					return;
				}
		}
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
		
	/******初始化设置的值********************/
	clr_ram();
//	ReInitAd();
	Enable_earth();
	Enable_wall();
	enable_hwincept();				//允许红外接收电源
	Enable_Speed(); 				//允许速度发送
	Init_Action();
	
	mode.mode = SHIFT;			
	mode.sub_mode = SHIFTPOINT2;
	mode.time=giv_sys_time;
	mode.step=0;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.status=1;
//		WriteWorkState();

//	Set_Motion_BackSweep(0);

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init SHIFTPOINT2 Mode Complete!\r\n");
#endif
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//清除接收缓存qz add 20180703
	
	if(dis_err_code==DIS_ERROR_DOCK_FAIL)		//qz add 20180710
		{
			dis_err_code=0;
			error_code=0;
		}
	
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgt_yaw);
	Logout_CheckPoint();
	Delete_All_PathPoint();
	delay_ms(500);
}

void Do_Shift_Point2(void)
{
	static u32 length;
	static u8 turn_dir;
	s8 tgt_gridx2,tgt_gridy2,now_gridx,now_gridy;
	u8 temp_nextaction=0;
	tgt_gridx2=check_point.new_x2;tgt_gridy2=check_point.new_y2;
	now_gridx=grid.x;now_gridy=grid.y;
	temp_nextaction=Read_CheckPoint_NextAction();
	
	ACC_DEC_Curve();
	
	u8 abnormal=Read_Protect();
	if(mode.abnormity)
		{
			Action_Protect_My(abnormal);
#ifdef FREE_SKID_INDEP_CHECK
			Free_Skid_Indep.check_flag=false;
#endif

#ifdef YBS_DIS_RESTORE
			Disable_Rotate_Angle();
#endif
			//mode.speed_up=false;		//qz add 20181225

			return;
		}
	
	if(Analysis_Check_Dock())
		return;

	if(Analysis_Reach_YAbort())
		return;
	
	Shift_BumpAction();
	if(mode.bump)
		return;
	if(mode.sub_mode!=SHIFTPOINT2)
		return;
	switch(mode.step)
		{
			case 0:
				if((now_gridx==tgt_gridx2)&(now_gridy==tgt_gridy2))
					{
						mode.step=100;
					}			
				else if(now_gridx==tgt_gridx2) 		//如果X坐标一致，Y坐标不一致
					{
						mode.step=1;
					}
				else if(now_gridy==tgt_gridy2) 		//如果Y坐标一致，X坐标不一致
					{
						mode.step=4;
					}
				else
					{
						TRACE("grid.x!=tgt_x2 grid.y!=tgt_y2!!!\r\n");
						mode.step=1;
						//TRACE("hang at %s %d\r\n",__func__,__LINE__);
						//while(1);
					}
				break;

			//X坐标一致，Y坐标不一致处理过程
			case 1:
				if(tgt_gridy2>now_gridy)
					motion1.tgt_yaw=R_Angle_Const;
				else
					motion1.tgt_yaw=L_Angle_Const;
				length=abs(tgt_gridy2-now_gridy)*20;length+=20;
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				mode.step++;
				break;
			case 2:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 3:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=0;
					}
				if(Judge_GridYPOS_Reach(tgt_gridy2, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=0;
					}
				break;

			//Y坐标一致，X坐标不一致处理过程
			case 4:
				if(now_gridx>tgt_gridx2)
					{
						motion1.tgt_yaw=B_Angle_Const;
					}
				else
					{
						motion1.tgt_yaw=F_Angle_Const;
					}
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				length=abs(now_gridx-tgt_gridx2)*20;length+=20;
				TRACE("length=%d in %s %d\r\n",length,__func__,__LINE__);
				mode.step++;
				break;
			case 5:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 6:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,length*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=0;
					}
				if(Judge_GridXPOS_Reach(tgt_gridx2, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=0;
					}
				break;
			case 100:

				TRACE("tgt_gridx2=%d tgt_gridy2=%d\r\n",tgt_gridx2,tgt_gridy2);
				TRACE("grid.x =%d grid.y=%d\r\n",grid.x,grid.y);
//				if(check_point.go_exit)
				if(temp_nextaction==CHECK_GOEXIT)
					{
						Do_ExitAtion();
						return;
					}
//				if(check_point.next_area)
				else if(temp_nextaction==CHECK_DOCK)
					{
						TRACE("Call this in %s %d\r\n",__func__,__LINE__);
						Init_Docking();
						return;
					}
				else if(temp_nextaction==CHECK_NEWAREA)
					{
						TRACE("Prepare to next area sweep!!!\r\n");
						TRACE("Next tgt_yaw=%d\r\n",check_point.next_tgtyaw);
						TRACE("Next y_acc=%d\r\n",check_point.next_yacc);
						TRACE("Next x_acc=%d\r\n",check_point.next_xacc);
						Init_Pass2Init(check_point.next_tgtyaw,check_point.next_yacc,check_point.next_xacc);
//						check_point.next_area=false;
						Reset_CheckPoint_NextAction();
						return;
					}
				else
					{
						mode.step++;
					}
				break;
			case 101:
				motion1.tgt_yaw=check_point.next_tgtyaw;
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				mode.step++;
			break;
			case 102:
				Speed=TURN_SPEED;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						//if(check_point.leak_sweep)
						if(temp_nextaction==CHECK_LEAKSWEEP)
							{
								Init_LeakSweep(motion1.tgt_yaw);
								motion1.repeat_sweep=true;
							}
						else if(temp_nextaction==CHECK_NORMALSWEEP)
							{
								Init_NormalSweep(motion1.tgt_yaw);
								motion1.repeat_sweep=true;
							}
						else if(temp_nextaction==CHECK_BACKSWEEP)
							{
								Init_Back_Sweep(motion1.tgt_yaw);
								motion1.repeat_sweep=true;
							}
						else if(temp_nextaction==CHECK_BACK2NORMAL)
							Init_Stop_BackSweep();
						Set_AreaWorkTime(20);
					}
				break;
		}
}

//输入参数pre_action：
//0:直接沿边；1：先以当前角度直行再沿边；2:CHECK_NEWAREA/CHECK_GOEXIT专用动作再直行
void Init_Shift_RightYBS(u8 pre_action)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
	
	clr_ram();
	Enable_earth(); 					//打开地检
	Enable_wall();						//打开墙检
	enable_hwincept();					//允许红外接收电源
	Enable_Speed(); 				//打开速度检测
	Init_Action();
	//	ReInitAd();
	//clr_all_hw_struct();				//清零回充信号标志	//qz modify 20181210 effect-->struct	//qz mask 20181215
	WriteWorkState();					//功能：保存工作状态
	

	mode.mode = SHIFT;			
	mode.sub_mode = YBS_SUB_RIGHT;		
	mode.step_bp = 0;
	mode.bump = 0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	
	mode.status=1;
	if(pre_action==0)
		{
			mode.step = 0;//QZ:原来为0x88;
		}
	else if(pre_action==1)
		{
			mode.step = 0x88;
		}
	else
		{
			mode.step =0xa0;
		}
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();			//打开万向轮检测 
#endif
#ifdef ROTATE_SKID_CHECK	
	//Enable_Rotate_Skid_Check(0);
#endif

	YBS_DISTANCE=YBS_DISTANCE_CONST;


	while(giv_sys_time-mode.time<1000);
	motion1.xpos_ybs_start=Gyro_Data.x_pos;
	motion1.ypos_ybs_start=Gyro_Data.y_pos;
	motion1.continue_checkstep=0;			//qz add 20190328
	grid.x_ybs_start=grid.x;
	grid.y_ybs_start=grid.y;
	mode.time=giv_sys_time;
	next_action=Read_CheckPoint_NextAction();
	abort_shiftybs_flag=false;
#ifdef DEBUG_Enter_Mode
	TRACE("Init Shift Right YBS Mode Complete!\r\n");
#endif
	TRACE("motion1.ybs_start_xpos=%d ypos=%d\r\n",motion1.xpos_ybs_start,motion1.ypos_ybs_start);
	Delete_All_PathPoint();
}

//输入参数pre_action：
//0:直接沿边；1：先以当前角度直行再沿边；2:CHECK_NEWAREA/CHECK_GOEXIT专用动作再直行
void Init_Shift_LeftYBS(u8 pre_action)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
	
	clr_ram();
	Enable_earth(); 					//打开地检
	Enable_wall();						//打开墙检
	enable_hwincept();					//允许红外接收电源
	Enable_Speed(); 				//打开速度检测
	Init_Action();
	//	ReInitAd();
	clr_all_hw_effect();				//清零回充信号标志
	WriteWorkState();					//功能：保存工作状态
	

	mode.mode = SHIFT;			
	mode.sub_mode = YBS_SUB_LEFT;		
	mode.step_bp = 0;
	mode.bump = 0;
	mode.bump_flag=false;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.time=giv_sys_time;

	mode.status=1;
	if(pre_action==0)
		mode.step = 0x40;//QZ:原来为0x88;
	else
		mode.step=0x88;
#ifdef FREE_SKID_CHECK
		Enable_Free_Skid_Check();			//打开万向轮检测 
#endif

#ifdef ROTATE_SKID_CHECK	
	//Enable_Rotate_Skid_Check(0);
#endif

	YBS_DISTANCE=YBS_DISTANCE_CONST;


	while(giv_sys_time-mode.time<1000);
	motion1.xpos_ybs_start=Gyro_Data.x_pos;
	motion1.ypos_ybs_start=Gyro_Data.y_pos;
	motion1.continue_checkstep=0;			//qz add 20190328
	grid.x_ybs_start=grid.x;
	grid.y_ybs_start=grid.y;
	next_action=Read_CheckPoint_NextAction();
	abort_shiftybs_flag=false;
#ifdef DEBUG_Enter_Mode
	TRACE("Init Shift Left YBS Mode Complete!\r\n");
#endif
	TRACE("motion1.ybs_start_xpos=%d ypos=%d\r\n",motion1.xpos_ybs_start,motion1.ypos_ybs_start);
	Delete_All_PathPoint();
}

u8 CheckOutRange_Clean_inShift(u16 data)
{
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	u8 result=0;
	switch (data)
		{
			case BUMP_XMAX_OUT:					//Xmax Out
				if((!Read_Coordinate_AreaNo(grid.x_area_max+1,now_gridy-1))&(!Read_Coordinate_AreaNo(grid.x_area_max+1,now_gridy))&(!Read_Coordinate_AreaNo(grid.x_area_max+1,now_gridy+1)))
					{
						stop_rap();
						check_point.new_x1=grid.x_area_max;
						check_point.new_y1=now_gridy;
						check_point.new_x2=grid.x_area_max+1;
						check_point.new_y2=now_gridy;
						check_point.next_xacc=1;
						check_point.next_yacc=2;
						Set_CheckPoint_NextAction(CHECK_NEWAREA);
						check_point.next_tgtyaw=F_Angle_Const;
						result=1;
					}
				break;
			case BUMP_XMIN_OUT:					//Xmin Out
				if((!Read_Coordinate_AreaNo(grid.x_area_min-1,now_gridy-1))&(!Read_Coordinate_AreaNo(grid.x_area_min-1,now_gridy))&(!Read_Coordinate_AreaNo(grid.x_area_min-1,now_gridy+1)))
					{
						stop_rap();
						check_point.new_x1=grid.x_area_min;
						check_point.new_y1=now_gridy;
						check_point.new_x2=grid.x_area_min-1;
						check_point.new_y2=now_gridy;
						check_point.next_xacc=0;
						check_point.next_yacc=2;
						Set_CheckPoint_NextAction(CHECK_NEWAREA);
						check_point.next_tgtyaw=B_Angle_Const;
						result=1;
					}
				break;
			case BUMP_YMAX_OUT:					//Ymax Out
				if((!Read_Coordinate_AreaNo(now_gridx-1,grid.y_area_max+1))&(!Read_Coordinate_AreaNo(now_gridx,grid.y_area_max+1))&(!Read_Coordinate_AreaNo(now_gridx+1,grid.y_area_max+1)))
					{
						stop_rap();
						check_point.new_x1=now_gridx;
						check_point.new_y1=grid.y_area_max;
						check_point.new_x2=now_gridx;
						check_point.new_y2=grid.y_area_max+1;
						check_point.next_xacc=2;
						check_point.next_yacc=1;
						Set_CheckPoint_NextAction(CHECK_NEWAREA);
						check_point.next_tgtyaw=F_Angle_Const;
						result=1;
					}
				break;
			case BUMP_YMIN_OUT:					//Ymin Out
				if((!Read_Coordinate_AreaNo(now_gridx-1,grid.y_area_min-1))&(!Read_Coordinate_AreaNo(now_gridx,grid.y_area_min=1))&(!Read_Coordinate_AreaNo(now_gridx+1,grid.y_area_min-1)))
					{
						stop_rap();
						check_point.new_x1=now_gridx;
						check_point.new_y1=grid.y_area_max;
						check_point.new_x2=now_gridx;
						check_point.new_y2=grid.y_area_min-1;
						check_point.next_xacc=2;
						check_point.next_yacc=0;
						Set_CheckPoint_NextAction(CHECK_NEWAREA);
						check_point.next_tgtyaw=F_Angle_Const;
						result=1;
					}
				break;
			default:				
				result=0;
				break;
		}
	return result;
}


u8 Abort_ShiftYBS(void)
{
	u8 temp_nextaction=0,area_check=0;
	s8 now_gridx;//now_gridy;
	s8 tgt_gridx1,tgt_gridy1,tgt_gridx2,tgt_gridy2;
	now_gridx=grid.x;//now_gridy=grid.y;
	tgt_gridx1=check_point.new_x1;tgt_gridy1=check_point.new_y1;
	tgt_gridx2=check_point.new_x2;tgt_gridy2=check_point.new_y2;
	temp_nextaction=Read_CheckPoint_NextAction();
	
	if((mode.bump>=BUMP_OUTRANGE)&(temp_nextaction>=CHECK_NEWAREA))	//(check_point.next_area))
		{
			if(Check_OutRange_Clean_YBS(mode.bump))
				{
					TRACE("Prepare to next area sweep in ShiftYBS!!!\r\n");
					TRACE("check_point.new_x1=%d\r\n",check_point.new_x1);
					TRACE("check_point.new_y1=%d\r\n",check_point.new_y1);
					TRACE("check_point.new_x2=%d\r\n",check_point.new_x2);
					TRACE("check_point.new_y2=%d\r\n",check_point.new_y2);
					TRACE("Next tgt_yaw=%d\r\n",check_point.next_tgtyaw);
					TRACE("Next y_acc=%d\r\n",check_point.next_yacc);
					TRACE("Next x_acc=%d\r\n",check_point.next_xacc);
//					Init_Pass2Init(check_point.next_tgtyaw,check_point.next_yacc,check_point.next_xacc);
					Init_Shift_Point1(0);
					Reset_CheckPoint_NextAction();
					return 1;
				}
		}

	if(mode.last_sub_mode==SHIFTPOINT1)
		{
			//if((now_gridy==tgt_gridy1)&(now_gridx==tgt_gridx1))
			if((Judge_GridYPOS_Reach(tgt_gridy1,0))&(now_gridx==tgt_gridx1))
				{
					stop_rap();
					TRACE("Grid is match the checkpoint new1 in %s!!\r\n",__func__);
					TRACE("Prepare to SHIFT POINT 2!!\r\n");
					Init_Shift_Point2();
					return 1;
				}
			//if((now_gridy==tgt_gridy2)&(now_gridx==tgt_gridx2))
			if((Judge_GridYPOS_Reach(tgt_gridy2,0))&(now_gridx==tgt_gridx2))
				{
					stop_rap();
					TRACE("Grid is match the checkpoint new2 in %s!!\r\n",__func__);
					TRACE("Prepare to SHIFT POINT 2!!\r\n");
					Init_Shift_Point2();
					return 1;
				}
		}
	
	if(mode.last_sub_mode==SHIFTPOINT2)
		{
			//if((now_gridy==tgt_gridy2)&(now_gridx==tgt_gridx2))
			if((Judge_GridYPOS_Reach(tgt_gridy2,0))&(now_gridx==tgt_gridx2))
				{
					stop_rap();
					TRACE("Grid is match the checkpoint new2 in %s!!\r\n",__func__);
					TRACE("Prepare to SHIFT POINT 2!!\r\n");
					Init_Shift_Point2();
					return 1;
				}
		}

	//Abort2Sweep(tgt_gridx2, tgt_gridy2);
	
	//if(abort_shiftybs_flag)
	if((abort_shiftybs_flag)&(temp_nextaction!=CHECK_BACK2NORMAL))
		{
			abort_shiftybs_flag=false;
			if(temp_nextaction<=CHECK_LEAKSWEEP)
				{
//					s8 tgt_gridx1,tgt_gridx2,tgt_gridy1,tgt_gridy2;
//					tgt_gridx1=check_point.new_x1;tgt_gridy1=check_point.new_y1;
//					tgt_gridx2=check_point.new_x2;tgt_gridy2=check_point.new_y2;
					if(abs(Gyro_Data.y_pos-motion1.ypos_ybs_start)<=20)
						return 0;

					if(temp_nextaction==CHECK_BACKSWEEP)
						{
							TRACE("Abort for CHECK_BACKSWEEP in %s!!!\r\n",__func__);
							if(Analysis_Back_Leak()==0)
								Init_Stop_BackSweep();
							else if((tgt_gridx1!=check_point.new_x1)&(tgt_gridy1!=check_point.new_y1))
								Init_Shift_Point1(1);
							return 1;
							
						}

					area_check=Area_Check(1);
					if(area_check==1)
						{
							if((tgt_gridx1==check_point.new_x1)&(tgt_gridx2==check_point.new_x2)&(tgt_gridy1==check_point.new_y1)&(tgt_gridy2==check_point.new_y2))
								{
									if(mode.last_sub_mode==SHIFTPOINT1)
										{
											if(Find_DirectlyWay_YBS(check_point.new_x1,check_point.new_y1))
												{
													stop_rap();
													TRACE("Find directly Way in %s\r\n",__func__);
													TRACE("turn_grid x=%d y=%d can reach point1!!!\r\n",turn_grid.gridx,turn_grid.gridy);
													TRACE("Prepare to Shift Point1 TurnGird!!!\r\n");
													//check_point.new_x2=turn_grid.gridx;check_point.new_y2=turn_grid.gridy;
													//Init_Shift_Point2();
													Init_Shift_Point1(2);
													return 1;
												}
											#if 0
											if(Find_PathPoint_YBS(check_point.new_x1,check_point.new_y1))
												{
													stop_rap();
													TRACE("Find indirectly nowallway in %s\r\n",__func__);
													TRACE("turn_grid x=%d y=%d can reach point1!!!\r\n",turn_grid.gridx,turn_grid.gridy);
													TRACE("Prepare to Shift Point1 TurnGird!!!\r\n");
													check_point.use_pathpoint=true;
													Init_Shift_Point1(2);
													return 1;													
												}
											#endif
										}
								}
							else
								{
									stop_rap();
									Init_Shift_Point1(1);
									return 1;
								}
						}
					else if(area_check==4)
						{
							stop_rap();
							TRACE("Call this in %s %d\r\n",__func__,__LINE__);
							Init_Docking();
							return 1;
						}
					else
						{
							if(area_check==3)
							Set_AreaWorkTime(10);
							stop_rap();
							Init_Shift_Point1(1);
//							Set_AreaWorkTime(5);
							return 1;
						}
				}
			else
				{
					if(temp_nextaction==CHECK_NEWAREA)
						{
							if(!Can_Entry_NewArea(&check_point))
								{
									stop_rap();
									TRACE("The New Area Entry has Closed!!\r\n");
									TRACE("Find Next New Area!!\r\n");
									Area_Check(1);
									Init_Shift_Point1(1);
									return 1;
								}
						}
					
					if(mode.last_sub_mode==SHIFTPOINT1)
						{
							if(Find_DirectlyWay_YBS(check_point.new_x1,check_point.new_y1))
								{
									stop_rap();
									TRACE("Find directly Way in %s\r\n",__func__);
									TRACE("turn_grid x=%d y=%d can reach point1!!!\r\n",turn_grid.gridx,turn_grid.gridy);
									TRACE("Prepare to Shift Point1 TurnGird!!!\r\n");
									//check_point.new_x2=turn_grid.gridx;check_point.new_y2=turn_grid.gridy;
									//Init_Shift_Point2();
									Init_Shift_Point1(2);
									return 1;
								}
#if 0
							else if(Find_Indirectly_Way(check_point.new_x1,check_point.new_y1))
								{
									stop_rap();
									TRACE("Find indirectly Way in %s\r\n",__func__);
									Init_Shift_Point1(1);
									return 1;
								}
#endif
							else
								return 0;
						}
					else if(mode.last_sub_mode==SHIFTPOINT2)
						{
							if(abs(Gyro_Data.y_pos-motion1.ypos_ybs_start)>20)
								{
									stop_rap();
									Area_Check(1);
									Init_Shift_Point1(1);
									return 1;
								}							
						}
				}
		}

	if(giv_sys_time-mode.time>600000)				//原来是15s
		{
			if((grid.x==grid.x_ybs_start)&(grid.y==grid.y_ybs_start))
				{
					if(temp_nextaction==next_action)
						{
							TRACE("motion1 ybs has been complete one circle!!!\r\n:");
							switch (temp_nextaction)
								{
									case CHECK_NORMALSWEEP:
									case CHECK_LEAKSWEEP:
										stop_rap();
										TRACE("next action is NORMALSWEEP or LEAKSWEEP!!!\r\n");
										TRACE("abort now action,prepare to next action!!!\r\n");
										motion1.area_ok=true;
#ifdef USE_AREA_TREE
										Set_Curr_AreaTree_LeakInfo(motion1.area_ok);
#else
										Set_CurrNode_LeakInfo(motion1.area_ok);
#endif
										
										Area_Check(1);
										Init_Shift_Point1(1);
										return 1;
									case CHECK_NEWAREA:
										TRACE("next action is NEWAREA!!!\r\n");
										TRACE("Area check again!!!\r\n");
#ifdef USE_AREA_TREE
										Set_Curr_AreaTree_AllNewOK();
#else
										Set_Curr_AllNewAreaOK();
#endif
										
										Area_Check(1);
										Init_Shift_Point1(1);
										return 1;
									case CHECK_GOEXIT:
									case CHECK_DOCK:
										TRACE("now action is GOEXIT!!!\r\n");
										TRACE("That means can't go back!!!\r\n");
										Init_Docking();
										break;											
								}
						}
				}
		}

	if(Analysis_Stop_StartArea())
		{
			stop_rap();
			TRACE("motion is in start area!!!\r\n");
			Init_Cease();
		}

	return 0;
}

void Do_ShiftYBS(void)
{
//	static u8 piv_out;	//机器是否向外展开，1为向外展开，0为向里缩小
//	static u8 piv_left; //机器是否向左转，1为向左转，0为向右转
	u8 temp_data1=0;
	u8 abnormal,turn_dir=0;
	u32 uin32;
	static short turn_angle=0;

#ifdef DC_NOBAT_RUN
	if((power.charge_dc)&(!dc_nobat_run))
#else
	if(power.charge_dc)
#endif
		{
			stop_rap();
			uin32 = giv_sys_time;
			while(giv_sys_time - uin32 < 20000)
				{
					AutoReadKey();
					judge_charge();
				}
				 
#ifdef DC_NOBAT_RUN
			if((power.charge_dc)&(!dc_nobat_run))
#else
			if(power.charge_dc)
#endif
				{
					 Init_Chargeing(DC_CHARGING);
					 return;
				}					
		}


//	YBS_comm_rap(); 
//	if(Gyro_Data.straight)
		ACC_DEC_Curve();
//	else
//		YBS_Comm_Rap_My();

	clr_all_hw_effect();			//qz add 20181210

	if(mode.sub_mode==YBS_SUB_RIGHT)
		YBS_Check_corner();
	else
		YBS_Left_Check_corner();
	
//	YBS_Wall_Count();		//qz add 20180902

	abnormal=Read_Protect();
	if(mode.abnormity)
		{
			Action_Protect_My(abnormal);
#ifdef FREE_SKID_INDEP_CHECK
			Free_Skid_Indep.check_flag=false;
#endif

#ifdef YBS_DIS_RESTORE
			Disable_Rotate_Angle();
#endif
			//mode.speed_up=false;		//qz add 20181225

			return;
		}

	if(Abort_ShiftYBS())
		return;
	
	if(Abort2Sweep())
		return;

	if(Analysis_Reach_YAbort())
		return;

	if(Analysis_Check_Dock())
		return;
	
	if((mode.sub_mode==YBS_SUB_RIGHT))				//	RIGHT
		{
			YBS_Right_Bump(1);
		}		
	else if((mode.sub_mode == YBS_SUB_LEFT))		//	LEFT
		{
			YBS_Left_Bump(1);
		}
	else
		return;
			
	if(mode.bump != 0)		//	有碰撞需要处理，返回d
			{
				//Wall_lost_counter = 0;
				if(mode.step>=0x88)
					{
						if(mode.sub_mode==YBS_SUB_RIGHT)
						{
							//mode.step=0x00;			//qz mask 20180910
						}
						else
							mode.step=0x40;
					}


#ifdef YBS_DIS_RESTORE
			Disable_Rotate_Angle();
#endif

#ifdef FREE_SKID_INDEP_CHECK
			Free_Skid_Indep.check_flag=false;
#endif
			return;
		}
	
	//qz add 20180228
#ifdef	FREE_SKID_CHECK
	if(Check_Free_Sikd())
		{
			Slam_Data.skid_flag=1;
#ifdef SKID_REPORT_TIME
			Slam_Data.skid_report_time=giv_sys_time;
#endif
#ifdef FREE_SKID_ACTION
			stop_rap();
			mode.step=0xB0;
#endif
		}
#endif
	//qz add end
	
	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	
	switch (mode.step)	//step路径执行的步骤
		{
			case 0x88:
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
#ifndef YBS_START_RIGHT_ANGLE
				enable_rap_no_length(FRONT, 3500, FRONT, 3000); //qz modify 20180703:走斜线
#else
				Speed=FAST_MOVE_SPEED;//2000							
				if(do_action(3,100*CM_PLUS))		//直行1m
					{
						stop_rap();
						mode.step++;
					}
				break;
			//qz add 20180801
			case 0x89:
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=false;
#endif
				Speed=FAST_MOVE_SPEED;
				if(mode.sub_mode==YBS_SUB_RIGHT)
					temp_data1=2;
				else
					temp_data1=1;
				if(do_action(temp_data1,90*Angle_1))			//右转90度
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 0x8A:
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
				Speed=FAST_MOVE_SPEED;//2000
				if(do_action(3,100*CM_PLUS))		//直行5m
					{
						stop_rap();
						mode.step=0x88;
					}
#endif					
				break;
					
			case 0:
				if(mode.sub_mode==YBS_SUB_LEFT)
					{
						mode.step=0x40;
						return;
					}
				if(giv_sys_time-mode.bump_time<200)
					return;
				Speed=HIGH_MOVE_SPEED;		//800//2000
				forward(0xFF812345);
				mode.step = 1;
				//qz add 20180316

#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif				
				break;
			
			case 1:
				YBS_Check_corner(); 	//QZ:利用w_r的difference，计算distance，每次得到一个新的YBS_Wall_Distance
				//YBS_Distance_Check(0);
				Wall_Comm_Rap();
				if(YBS_Wall_Distance < 80)		//80	//	第一次找到墙
				//if(YBS_Wall_Distance<CONST_DIS+YBS_DISTANCE)
					{
						mode.step = 2;
#ifdef	YBS_Straight_FAST
						YBS_Straight_Time=giv_sys_time;
#endif

#ifdef YBS_DIS_RESTORE			//准备检查里程计算出的机器角度
						Enable_Rotate_Angle();
#endif
						return;
					}
				//QZ ADD
				//if(YBS_Wall_Distance>140) //140
				else
					{
						mode.step=3;
					}
				
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
				break;
							
			case 2:
				YBS_Check_corner(); 					
				//YBS_Distance_Check(0);
				Wall_Comm_Rap();
				if(YBS_Wall_Distance > 80)				//	彻底丢失墙壁	  有可能出现拐角//80  //140
				//if(YBS_Wall_Distance>=CONST_DIS+YBS_DISTANCE)
					{
						mode.step = 3;

#ifdef YBS_DIS_RESTORE		//出现空旷区域，停止检测里程计角度
						Disable_Rotate_Angle();
#endif
					}

#ifdef YBS_DIS_RESTORE		//检查里程计计算的角度，如果角度大于8度，恢复YBS_DISTANCE_CONST
				Check_Rotate_Angle();
				if((rotate_angle.rot_angle>(12.0))&(YBS_DISTANCE>YBS_DISTANCE_CONST))	//qz modify 20180902:8.0-->6.0-->12.0
					{
						YBS_DISTANCE=YBS_DISTANCE_CONST;
					}
#endif


#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
				break;
			
			case 3:
//					Wall_lost_Start_Pos = r_rap.pulse;							//	向前走一点 	//qz mask 20180203
				Wall_lost_Start_Pos=l_ring.all_length;
				r_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				l_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;

				mode.step = 4;	 
				break;
			case 4: 
				r_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				l_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				if((l_ring.all_length-Wall_lost_Start_Pos)>=3*CM_PLUS)
				{
					//QZ:ADD
					stop_rap();

					//qz add
					if(YBS_Wall_Distance<CONST_DIS+YBS_DISTANCE)
						{
							mode.step=0x00;
							return;
						}
					//qz add end
					mode.step = 0x10;
				}

				break;
			case 0x10:		
				Wall_lost_Start_Pos = l_ring.all_length;							//	旋转 
				r_rap.rap =YBS_LOST_CORNER_TURN_SPD_R;	//QZ:原来为300,350		
				l_rap.rap = YBS_LOST_CORNER_TURN_SPD ;
				l_rap.sign=1;
				r_rap.sign=1;
				l_rap.ori=FRONT;
				r_rap.ori=FRONT;
				mode.step = 0x11;
				YBS_DISTANCE=YBS_DISTANCE_CONST;		//qz add 20810803
				lost_turn_time=giv_sys_time;
				//qz add 20180316

#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=false;
#endif
				break;
			case 0x11:
				YBS_Check_corner();

				//QZ ADD
				r_rap.rap =YBS_LOST_CORNER_TURN_SPD_R;	//QZ:原来为300,350		
				l_rap.rap = YBS_LOST_CORNER_TURN_SPD ;
					
				l_rap.ori=FRONT;
				l_rap.sign=1;
				r_rap.ori=FRONT;
				r_rap.sign=1;

				if(giv_sys_time-lost_turn_time>10000)			//qz add 20180902 1.5s qz modify 20181201 1.5s--->1.0s
					{
						YBS_DISTANCE=YBS_DISTANCE_CONST;
					}
				if((YBS_Wall_Distance <= (YBS_DISTANCE + 20))|(w_m.high_sign==NEAR)|(w_rm.high_sign==NEAR))//||(M_WALL_DIS<100)||(RM_WALL_DIS<100)) 			//	第二次找到墙	//qz modify 20181225
					{
						//QZ:ADD
						stop_rap();
					//	mode.step = 0x12;
						//qz add
						r_rap.rap=HIGH_MOVE_SPEED;
						l_rap.rap=HIGH_MOVE_SPEED;
						l_rap.ori=FRONT;
						l_rap.sign=1;
						r_rap.ori=FRONT;
						r_rap.sign=1;
						mode.step =0x01;
					}
				break;
			case 0x12:								//	停止旋转
				mode.step = 0x00;
				break;
			//--------------------------------------------------------
							

			
			//--------------------------------------------------------
			//--------------------------------------------------------
			case 0x40:	
				if(giv_sys_time-mode.bump_time<200)
					return;
				Speed=HIGH_MOVE_SPEED;	//2000
				forward(0xFF812345);
				mode.step = 0x41;				
				//qz add 20180316
			
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
								
				break;
			case 0x41:	
				YBS_Left_Check_corner();	//QZ:利用w_r的difference，计算distance，每次得到一个新的YBS_Wall_Distance
				//YBS_Left_Distance_Check();
				Wall_Comm_Rap();
				if(YBS_Wall_Distance < 80)		//80	//	第一次找到墙
					{
						mode.step = 0x42;
#ifdef	YBS_Straight_FAST
						YBS_Straight_Time=giv_sys_time;
#endif
			
#ifdef YBS_DIS_RESTORE		//准备检查里程计算出的机器角度
						Enable_Rotate_Angle();
#endif
					}
					//QZ ADD
				if(YBS_Wall_Distance>140)	//140
					{
						mode.step=0x43;
					}
								
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
								
								
				break;
			case 0x42:
				YBS_Left_Check_corner();					
				//YBS_Left_Distance_Check();
				Wall_Comm_Rap();
				if(YBS_Wall_Distance > 140) 			//	彻底丢失墙壁	  有可能出现拐角//80
						{
							mode.step = 0x43;

#ifdef YBS_DIS_RESTORE		//出现空旷区域，停止检测里程计角度
							Disable_Rotate_Angle();
#endif

						}

#ifdef YBS_DIS_RESTORE		//检查里程计计算的角度，如果角度大于8度，恢复YBS_DISTANCE_CONST
				Check_Rotate_Angle();
				if((rotate_angle.rot_angle<(-8.0))&(YBS_DISTANCE>YBS_DISTANCE_CONST))
					{
						YBS_DISTANCE=YBS_DISTANCE_CONST;
					}
#endif


#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
				break;
			//--------------------------------------------------------
			//--------------------------------------------------------
			case 0x43:
				Wall_lost_Start_Pos=r_ring.all_length;
				r_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				l_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;

				mode.step = 0x44;	 
				break;
			case 0x44:
				r_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				l_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				if((r_ring.all_length-Wall_lost_Start_Pos)>=3*CM_PLUS)
				{
					//QZ:ADD
					stop_rap();
					mode.step = 0x50;
				}

				//qz add
				if(YBS_Wall_Distance<80)
					{
						mode.step=0x40;
					}
				//qz add end

				break;
			case 0x50:
				Wall_lost_Start_Pos = r_ring.all_length;							//	旋转 
				r_rap.rap	=YBS_LOST_CORNER_TURN_SPD;	//QZ:原来为300,350		
				l_rap.rap	=YBS_LOST_CORNER_TURN_SPD_R ;
				l_rap.sign	=1;
				r_rap.sign	=1;
				l_rap.ori	=FRONT;
				r_rap.ori	=FRONT;
				mode.step	= 0x51;
				YBS_DISTANCE=YBS_DISTANCE_CONST;		//qz add 20810803

#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=false;
#endif
				break;
			case 0x51:
				YBS_Left_Check_corner();

				//QZ ADD
				r_rap.rap =YBS_LOST_CORNER_TURN_SPD;	//QZ:原来为300,350		
				l_rap.rap = YBS_LOST_CORNER_TURN_SPD_R ;
				
				l_rap.ori=FRONT;
				l_rap.sign=1;
				r_rap.ori=FRONT;
				r_rap.sign=1;

#if 0			//取消底盘孤岛判断
				if(l_ring.all_length-Wall_lost_Start_Pos>(angle720+angle360))
					{
						stop_rap();
						mode.step=0x88;//0x12
#ifdef FREE_SKID_CHECK
						Enable_Free_Skid_Check();
#endif
#ifdef YBS_BLACK
						YBS_DISTANCE=30;
#endif
						return;
					}
									//QZ ADD END
#endif 
				if((YBS_Wall_Distance < (YBS_DISTANCE + 20))||(w_rm.sign==NEAR)||(w_m.high_sign==NEAR)|(w_lm.high_sign==NEAR))//||(M_WALL_DIS<100)||(RM_WALL_DIS<100))				//	第二次找到墙
						{
							//QZ:ADD
							stop_rap();
					//		mode.step = 0x12;
							//qz add
							r_rap.rap=HIGH_MOVE_SPEED;
							l_rap.rap=HIGH_MOVE_SPEED;
							l_rap.ori=FRONT;
							l_rap.sign=1;
							r_rap.ori=FRONT;
							r_rap.sign=1;
							mode.step =0x41;
						}
				break;

			case 0xa0:
				switch(check_point.new_area_dir)
					{
						case DIR_YMAX:
							turn_angle=B_Angle_Const;
							break;
						case DIR_XMAX:
							turn_angle=R_Angle_Const;
							break;
						case DIR_YMIN:
							turn_angle=F_Angle_Const;
							break;
						case DIR_XMIN:
							turn_angle=L_Angle_Const;
							break;
						default:
							mode.step=0;
							return;
					}
				mode.step++;
				break;
			case 0xa1:
				turn_dir=Get_TurnDir(turn_angle);
				Speed=TURN_SPEED;
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(turn_angle,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 0xa2:
				Speed=FAST_MOVE_SPEED;
				do_action_my(3,FARAWAY*CM_PLUS,turn_angle);
				break;
			
			//绕过障碍后的处理过程
			case 0xD0:
				if(giv_sys_time-mode.time<5000)
					return;
				mode.step++;
				break;
			case 0xD1:
				//Area_Check(1);
				break;
				//	不停继续转圈

			default:
				mode.step=0x88;
				break;
		}	//	end of		switch (mode.step)	//step路径执行的步骤
}

//function:此函数被调用时，已经退出之前清扫的区域，返回到开始进入之前区域的上一个区域，然后直行一些处理动作
void Do_ExitAtion(void)
{		
	TRACE("Enter in %s..\r\n");
#ifdef USE_AREA_TREE
	Back_To_ParentTree();
	if(Read_Curr_AreaTree_NO()==0)
		{
			TRACE("Call this in %s %d\r\n",__func__,__LINE__);
			Init_Docking();
			return;
		}
	Get_Curr_AreaTree_Info();
#else
	Del_AreaNode_End();							//删除上一清扫区域的节点
	if(Read_CurrNode_AreaNO()==0)
		{
			TRACE("Call this in %s %d\r\n",__func__,__LINE__);
			Init_Docking();
			return;
		}
	Get_CurrNode_Info();						//获取当前区域的节点信息
#endif
	Cal_PosArea_Max();							//获取当前区域的坐标信息
	if(motion1.force_dock)
		{
			TRACE("Force Dock!!!\r\n");
			Force_Dock();
			return;
		}
	Area_Check(0);
	Init_Shift_Point1(0);
	TRACE("set worktime_area 10!!\r\n");
	Set_AreaWorkTime(10);

}

u8 Abort2Sweep(void)
{
	s8 now_gridy,now_gridx,tgt_gridy2;//,tgt_gridx1,tgt_gridx2,tgt_gridy1
	u8 nextaction=0,check_result=0;

	static s8 last_gridx=0x7f,last_gridy=0x7f;
	
	now_gridx=grid.x;now_gridy=grid.y;
//	tgt_gridx1=check_point.new_x1;tgt_gridy1=check_point.new_y1;
	tgt_gridy2=check_point.new_y2;//tgt_gridx2=check_point.new_x2;
	nextaction=Read_CheckPoint_NextAction();

	s8 temp_gridx1,temp_gridy1,temp_gridx2,temp_gridy2,temp_gridx3,temp_gridx4;	
	short now_angle=Gyro_Data.yaw;

	if(motion1.force_dock)
		return 0;
	
	switch(nextaction)
		{

			case CHECK_NORMALSWEEP:
			case CHECK_BACKSWEEP:
			case CHECK_LEAKSWEEP:
				if(now_gridy!=tgt_gridy2)
					return 0;
				if(mode.sub_mode==YBS_SUB_LEFT)														//左沿边
					{
						if(check_point.ydir>0)
							{
								if(Judge_Yaw_Reach(L_Angle_Const,DEGREE_30))
									{
										temp_gridx1=now_gridx+1;temp_gridy1=now_gridy;
										temp_gridx2=now_gridx+1;temp_gridy2=now_gridy+1;
									}
								else if(Judge_Yaw_Reach(R_Angle_Const,DEGREE_30))
									{
										temp_gridx1=now_gridx-1;temp_gridy1=now_gridy;
										temp_gridx2=now_gridx-1;temp_gridy2=now_gridy+1;
									}
								else
									{
										return 0;
									}
							}
						else
							{
								if(Judge_Yaw_Reach(L_Angle_Const,DEGREE_30))
									{
										temp_gridx1=now_gridx+1;temp_gridy1=now_gridy;
										temp_gridx2=now_gridx+1;temp_gridy2=now_gridy-1;
									}
								else if(Judge_Yaw_Reach(R_Angle_Const,DEGREE_30))
									{
										temp_gridx1=now_gridx-1;temp_gridy1=now_gridy;
										temp_gridx2=now_gridx-1;temp_gridy2=now_gridy-1;
									}
								else
									{
										return 0;
									}
							}
					}		
				else																				//右沿边
					{
						if(check_point.ydir>0)
							{
								if(Judge_Yaw_Reach(L_Angle_Const,DEGREE_30))
									{
										temp_gridx1=now_gridx-1;temp_gridy1=now_gridy;
										temp_gridx2=now_gridx-1;temp_gridy2=now_gridy+1;
									}
								else if(Judge_Yaw_Reach(R_Angle_Const,DEGREE_30))
									{
										temp_gridx1=now_gridx+1;temp_gridy1=now_gridy;
										temp_gridx2=now_gridx+1;temp_gridy2=now_gridy+1;
									}
								else
									{
										return 0;
									}
							}
						else
							{
								if(Judge_Yaw_Reach(L_Angle_Const,DEGREE_30))
									{
										temp_gridx1=now_gridx-1;temp_gridy1=now_gridy;
										temp_gridx2=now_gridx-1;temp_gridy2=now_gridy-1;
									}
								else if(Judge_Yaw_Reach(R_Angle_Const,DEGREE_30))
									{
										temp_gridx1=now_gridx+1;temp_gridy1=now_gridy;
										temp_gridx2=now_gridx+1;temp_gridy2=now_gridy-1;
									}
								else
									{
										return 0;
									}
							}
					}


				if(!Read_Coordinate_Clean(temp_gridx1,temp_gridy1))
					{
						if(!Read_Coordinate_Clean(temp_gridx2,temp_gridy2))
							{

								if((last_gridx==now_gridx)&(last_gridy==now_gridx))
									{
										return 0;
									}
								last_gridx=now_gridx;last_gridy=now_gridy;

								stop_rap();
								TRACE("girdx=%d gridy=%d not clean\r\n",temp_gridx1,temp_gridy1);
								TRACE("girdx=%d gridy=%d not clean\r\n",temp_gridx2,temp_gridy2);
								TRACE("Abort in %s %d!!!\r\n",__func__,__LINE__);
								check_point.new_x1=now_gridx;check_point.new_y1=now_gridy;
								check_point.new_x2=now_gridx;check_point.new_y2=now_gridy;
								//Set_CheckPoint_NextAction(CHECK_LEAKSWEEP);
								if(temp_gridx1>now_gridx)
									check_point.next_tgtyaw=F_Angle_Const;
								else
									check_point.next_tgtyaw=B_Angle_Const;
								check_point.ydir=check_point.ydir;
								Init_Shift_Point1(0);
								return 1;
							}
					}
				break;
#if 1
			case CHECK_NEWAREA:
			case CHECK_GOEXIT:
				
				if((now_gridy>=grid.y_area_max)|(now_gridy<=grid.y_area_min))
					return 0;
				if((now_gridx>=grid.x_area_max)|(now_gridx<=grid.x_area_min))
					return 0;

				if(mode.sub_mode==YBS_SUB_LEFT)														//左沿边
					{
						if(Judge_Yaw_Reach(L_Angle_Const,DEGREE_30))
							{
								temp_gridx1=now_gridx+1;temp_gridy1=now_gridy;
								temp_gridx2=now_gridx+1;temp_gridy2=now_gridy-1;
							}
						else if(Judge_Yaw_Reach(R_Angle_Const,DEGREE_30))
							{
								temp_gridx1=now_gridx-1;temp_gridy1=now_gridy;
								temp_gridx2=now_gridx-1;temp_gridy2=now_gridy+1;
							}
						else
							{
								return 0;
							}
					}		
				else																				//右沿边
					{
						if(Judge_Yaw_Reach(L_Angle_Const,DEGREE_30))
							{
								temp_gridx1=now_gridx-1;temp_gridy1=now_gridy;
								temp_gridx2=now_gridx-1;temp_gridy2=now_gridy-1;
							}
						else if(Judge_Yaw_Reach(R_Angle_Const,DEGREE_30))
							{
								temp_gridx1=now_gridx+1;temp_gridy1=now_gridy;
								temp_gridx2=now_gridx+1;temp_gridy2=now_gridy+1;
							}
						else
							{
								return 0;
							}
					}

				check_result=0;
				temp_gridx3=temp_gridx1;temp_gridx4=temp_gridx2;
				if(temp_gridx3>now_gridx)
					{
						while(temp_gridx3<grid.x_area_max)
							{
								if((!Read_Coordinate_Clean(temp_gridx3,temp_gridy1))&(!Read_Coordinate_Clean(temp_gridx4,temp_gridy2)))
									{
										temp_gridx3++;
										temp_gridx4++;
										check_result++;
									}
								else
									{
										return 0;
										//break;
									}
							}
						if(check_result<2)
							return 0;
					}
				else
					{
						while(temp_gridx3>grid.x_area_min)
							{
								if((!Read_Coordinate_Clean(temp_gridx3,temp_gridy1))&(!Read_Coordinate_Clean(temp_gridx4,temp_gridy2)))
									{
										temp_gridx3--;
										temp_gridx4--;
										check_result++;
									}
								else
									{	
										return 0;
										//break;
									}
							}
						if(check_result<2)
							return 0;
					}


				if(motion1.sweep_time<SWEEP_AREANUM_MAX)
					{
						if(temp_gridx2>now_gridx)				//向grid.x_area_max方向
							{
								if(temp_gridy2>now_gridy)		//向grid.y_area_max方向
									{
										if(motion1.ymax_ok)
											{
												motion1.ymax_ok=false;
#ifdef USE_AREA_TREE
												Set_Curr_AreaTree_NewInfo(motion1.ymax_ok,DIR_YMAX);
#else
												Set_CurrNode_NewAreaInfo(motion1.ymax_ok, DIR_YMAX);
#endif
											}
									}
								else							//向grid.y_area_min方向
									{
										if(motion1.ymin_ok)
											{
												motion1.ymin_ok=false;
#ifdef USE_AREA_TREE
												Set_Curr_AreaTree_NewInfo(motion1.ymin_ok,DIR_YMIN);
#else
												Set_CurrNode_NewAreaInfo(motion1.ymin_ok, DIR_YMIN);
#endif
											}
									}
								
								if(motion1.xmax_ok)
									{
										motion1.xmax_ok=false;
#ifdef USE_AREA_TREE
										Set_Curr_AreaTree_NewInfo(motion1.xmax_ok,DIR_XMAX);
#else
										Set_CurrNode_NewAreaInfo(motion1.xmax_ok, DIR_XMAX);
#endif
									}
							}
						else									//向grid.x_area_min方向
							{
								if(temp_gridy2>now_gridy)		//向grid.y_area_max方向
									{
										if(motion1.ymax_ok)
											{
												motion1.ymax_ok=false;
#ifdef USE_AREA_TREE
												Set_Curr_AreaTree_NewInfo(motion1.ymax_ok,DIR_YMAX);
#else
												Set_CurrNode_NewAreaInfo(motion1.ymax_ok, DIR_YMAX);
#endif
											}
									}
								else							//向grid.y_area_min方向
									{
										if(motion1.ymin_ok)
											{
												motion1.ymin_ok=false;
#ifdef USE_AREA_TREE
												Set_Curr_AreaTree_NewInfo(motion1.ymin_ok,DIR_YMIN);
#else
												Set_CurrNode_NewAreaInfo(motion1.ymin_ok, DIR_YMIN);
#endif
											}
									}

								if(motion1.xmin_ok)
									{
										motion1.xmin_ok=false;
#ifdef USE_AREA_TREE
										Set_Curr_AreaTree_NewInfo(motion1.xmin_ok,DIR_XMIN);
#else
										Set_CurrNode_NewAreaInfo(motion1.xmin_ok, DIR_XMIN);
#endif
									}
							}
					}

					//if((last_gridx==now_gridx)&(last_gridy==now_gridy))
					if((abs(last_gridx-now_gridx)<2)&(abs(last_gridy-now_gridy)<2))
						{
							return 0;
						}
					last_gridx=now_gridx;last_gridy=now_gridy;
					
				//if(!Read_Coordinate_Clean(temp_gridx1,temp_gridy1))
					{
						//if(!Read_Coordinate_Clean(temp_gridx2,temp_gridy2))
							{
								stop_rap();
								TRACE("girdx1=%d gridy1=%d not clean\r\n",temp_gridx1,temp_gridy1);
								TRACE("girdx2=%d gridy2=%d not clean\r\n",temp_gridx2,temp_gridy2);
								TRACE("Abort in %s %d!!!\r\n",__func__,__LINE__);
								check_point.new_x1=now_gridx;check_point.new_y1=now_gridy;
								check_point.new_x2=now_gridx;check_point.new_y2=now_gridy;
								Set_CheckPoint_NextAction(CHECK_LEAKSWEEP);
								if(temp_gridx1>now_gridx)
									check_point.next_tgtyaw=F_Angle_Const;
								else
									check_point.next_tgtyaw=B_Angle_Const;
								if(temp_gridy1<temp_gridy2)
									check_point.ydir=1;
								else
									check_point.ydir=0;
								Init_Shift_Point1(0);
								return 1;
							}
					}
				break;
			default:
				break;
#endif
		}
	return 0;	
}

void Init_ShiftExit_RightYBS(u8 pre_action)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
	
	clr_ram();
	Enable_earth(); 					//打开地检
	Enable_wall();						//打开墙检
	enable_hwincept();					//允许红外接收电源
	Enable_Speed(); 				//打开速度检测
	Init_Action();
	//	ReInitAd();
	//clr_all_hw_struct();				//清零回充信号标志	//qz modify 20181210 effect-->struct	//qz mask 20181215
	WriteWorkState();					//功能：保存工作状态
	

	mode.mode = EXIT;			
	mode.sub_mode = YBS_SUB_RIGHT;		
	mode.step_bp = 0;
	mode.bump = 0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	
	mode.status=1;
	if(pre_action==0)
		{
			mode.step = 0;//QZ:原来为0x88;
		}
	else
		{
			mode.step = 0x88;
		}
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();			//打开万向轮检测 
#endif
#ifdef ROTATE_SKID_CHECK	
	//Enable_Rotate_Skid_Check(0);
#endif

	YBS_DISTANCE=YBS_DISTANCE_CONST;


	while(giv_sys_time-mode.time<1000);
	motion1.xpos_ybs_start=Gyro_Data.x_pos;
	motion1.ypos_ybs_start=Gyro_Data.y_pos;
	motion1.continue_checkstep=0;			//qz add 20190328
	grid.x_ybs_start=grid.x;
	grid.y_ybs_start=grid.y;
	mode.time=giv_sys_time;
	next_action=Read_CheckPoint_NextAction();
	abort_shiftybs_flag=false;
#ifdef DEBUG_Enter_Mode
	TRACE("Init Shift Right YBS Mode Complete!\r\n");
#endif
	TRACE("motion1.ybs_start_xpos=%d ypos=%d\r\n",motion1.xpos_ybs_start,motion1.ypos_ybs_start);
}

void Init_ShiftExit_LeftYBS(u8 pre_action)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
	
	clr_ram();
	Enable_earth(); 					//打开地检
	Enable_wall();						//打开墙检
	enable_hwincept();					//允许红外接收电源
	Enable_Speed(); 				//打开速度检测
	 
	Init_Action();
	//	ReInitAd();
	clr_all_hw_effect();				//清零回充信号标志
	WriteWorkState();					//功能：保存工作状态
	

	mode.mode = EXIT;			
	mode.sub_mode = YBS_SUB_LEFT;		
	mode.step_bp = 0;
	mode.bump = 0;
	mode.bump_flag=false;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.time=giv_sys_time;

	mode.status=1;
	if(pre_action==0)
		mode.step = 0x40;//QZ:原来为0x88;
	else
		mode.step=0x88;
	Enable_Free_Skid_Check();			//打开万向轮检测 

#ifdef ROTATE_SKID_CHECK	
	//Enable_Rotate_Skid_Check(0);
#endif

	YBS_DISTANCE=YBS_DISTANCE_CONST;


	while(giv_sys_time-mode.time<1000);
	motion1.xpos_ybs_start=Gyro_Data.x_pos;
	motion1.ypos_ybs_start=Gyro_Data.y_pos;
	motion1.continue_checkstep=0;			//qz add 20190328
	grid.x_ybs_start=grid.x;
	grid.y_ybs_start=grid.y;
	next_action=Read_CheckPoint_NextAction();
	abort_shiftybs_flag=false;
#ifdef DEBUG_Enter_Mode
	TRACE("Init Shift Left YBS Mode Complete!\r\n");
#endif
	TRACE("motion1.ybs_start_xpos=%d ypos=%d\r\n",motion1.xpos_ybs_start,motion1.ypos_ybs_start);
}

u8 Abort_ShiftExit_YBS(void)
{
	s8 now_gridx,now_gridy,temp_gridx1,temp_gridy1;
	u8 temp_areano=0;
	now_gridx=grid.x;now_gridy=grid.y;

	if(Analysis_Stop_StartArea())
		{
			stop_rap();
			TRACE("motion is in start area!!!\r\n");
			Init_Cease();
			return 1;
		}

	if(((now_gridx==motion1.exit_gridx1)&(now_gridy==motion1.exit_gridy1))\
		|((now_gridx==motion1.exit_gridx2)&(now_gridy==motion1.exit_gridy2)))
		{
			stop_rap();
			Area_Check(0);
			Init_Shift_Point1(0);
			return 1;
		}
	
	if(Analysis_Reach_ExitArea())
		{
			TRACE("now grid is in the exit area num!!!\r\n");
			TRACE("now grid is in the exit area num!!!\r\n");
			TRACE("Abort and recheck!!!\r\n");
			stop_rap();
			Do_ExitAtion();
			return 1;
		}

	if(mode.bump>BUMP_OUTRANGE)
		{
			switch(mode.bump)
				{
					case BUMP_XMAX_OUT:
						temp_gridx1=now_gridx+1;
						temp_gridy1=now_gridy;
						temp_areano=Read_Coordinate_AreaNo(temp_gridx1,temp_gridy1);
						break;
					case BUMP_XMIN_OUT:
						temp_gridx1=now_gridx-1;
						temp_gridy1=now_gridy;
						temp_areano=Read_Coordinate_AreaNo(temp_gridx1,temp_gridy1);
						break;
					case BUMP_YMAX_OUT:
						temp_gridx1=now_gridx;
						temp_gridy1=now_gridy+1;
						temp_areano=Read_Coordinate_AreaNo(temp_gridx1,temp_gridy1);
						break;
					case BUMP_YMIN_OUT:
						temp_gridx1=now_gridx;
						temp_gridy1=now_gridy-1;
						temp_areano=Read_Coordinate_AreaNo(temp_gridx1,temp_gridy1);
						break;
					default:
						temp_areano=0;
						break;
				}
		}
	
	if((temp_areano!=0)&(temp_areano==motion1.exit_area_num))
		{
			stop_rap();
			TRACE("Abort and recheck!!!\r\n");
			TRACE("change check point exit!!!\r\n");
			check_point.new_x1=now_gridx;check_point.new_y1=now_gridy;
			check_point.new_x2=temp_gridx1;check_point.new_y2=temp_gridy1;
			Init_Shift_Point1(0);
			return 1;
		}

	if((now_gridx==check_point.new_x1)&(now_gridy==check_point.new_y1))
		{
			TRACE("now grid is match checkpoint new1!!\r\n");
			stop_rap();
			Init_Shift_Point1(0);
			return 1;
		}

	if(Find_DirectlyWay_YBS(check_point.new_x1,check_point.new_y1))
		{
			stop_rap();
			TRACE("Find directly Way in %s\r\n",__func__);
			TRACE("turn_grid x=%d y=%d can reach point1!!!\r\n",turn_grid.gridx,turn_grid.gridy);
			TRACE("Prepare to Shift Point1 TurnGird!!!\r\n");
			//check_point.new_x2=turn_grid.gridx;check_point.new_y2=turn_grid.gridy;
			//Init_Shift_Point2();
			Init_Shift_Point1(2);
			return 1;
		}
	return 0;
}

void Do_ShiftExit_YBS(void)
{
//	static u8 piv_out;	//机器是否向外展开，1为向外展开，0为向里缩小
//	static u8 piv_left; //机器是否向左转，1为向左转，0为向右转
	u8 temp_data1=0;
	u8 abnormal;
	u32 uin32;

#ifdef DC_NOBAT_RUN
	if((power.charge_dc)&(!dc_nobat_run))
#else
	if(power.charge_dc)
#endif
		{
			stop_rap();
			uin32 = giv_sys_time;
			while(giv_sys_time - uin32 < 20000)
				{
					AutoReadKey();
					judge_charge();
				}
				 
#ifdef DC_NOBAT_RUN
			if((power.charge_dc)&(!dc_nobat_run))
#else
			if(power.charge_dc)
#endif
				{
					 Init_Chargeing(DC_CHARGING);
					 return;
				}					
		}


//	YBS_comm_rap(); 
//	if(Gyro_Data.straight)
		ACC_DEC_Curve();
//	else
//		YBS_Comm_Rap_My();

	clr_all_hw_effect();			//qz add 20181210

	if(mode.sub_mode==YBS_SUB_RIGHT)
		YBS_Check_corner();
	else
		YBS_Left_Check_corner();
	
//	YBS_Wall_Count();		//qz add 20180902

	abnormal=Read_Protect();
	if(mode.abnormity)
		{
			Action_Protect_My(abnormal);
#ifdef FREE_SKID_INDEP_CHECK
			Free_Skid_Indep.check_flag=false;
#endif

#ifdef YBS_DIS_RESTORE
			Disable_Rotate_Angle();
#endif
			//mode.speed_up=false;		//qz add 20181225

			return;
		}

	if(Abort2Sweep())
		return;

	if(Abort_ShiftExit_YBS())
		return;
	
	if(Analysis_Check_Dock())
		return;
	
	if((mode.sub_mode==YBS_SUB_RIGHT))				//	RIGHT
		{
			YBS_Right_Bump(1);
		}		
	else if((mode.sub_mode == YBS_SUB_LEFT))		//	LEFT
		{
			YBS_Left_Bump(1);
		}
	else
		return;
			
	if(mode.bump != 0)		//	有碰撞需要处理，返回d
			{
				//Wall_lost_counter = 0;
				if(mode.step>=0x88)
					{
						if(mode.sub_mode==YBS_SUB_RIGHT)
						{
							//mode.step=0x00;			//qz mask 20180910
						}
						else
							mode.step=0x40;
					}

#ifdef YBS_DIS_RESTORE
			Disable_Rotate_Angle();
#endif

#ifdef FREE_SKID_INDEP_CHECK
			Free_Skid_Indep.check_flag=false;
#endif
			return;
		}
	
	//qz add 20180228
#ifdef	FREE_SKID_CHECK
	if(Check_Free_Sikd())
		{
			Slam_Data.skid_flag=1;
#ifdef SKID_REPORT_TIME
			Slam_Data.skid_report_time=giv_sys_time;
#endif
#ifdef FREE_SKID_ACTION
			stop_rap();
			mode.step=0xB0;
#endif
		}
#endif
	
	//----------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------
	
	switch (mode.step)	//step路径执行的步骤
		{
			case 0x88:
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
#ifndef YBS_START_RIGHT_ANGLE
				enable_rap_no_length(FRONT, 3500, FRONT, 3000); //qz modify 20180703:走斜线
#else
				Speed=FAST_MOVE_SPEED;//2000							
				if(do_action(3,100*CM_PLUS))		//直行1m
					{
						stop_rap();
						mode.step++;
					}
				break;
			//qz add 20180801
			case 0x89:
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=false;
#endif
				Speed=FAST_MOVE_SPEED;
				if(mode.sub_mode==YBS_SUB_RIGHT)
					temp_data1=2;
				else
					temp_data1=1;
				if(do_action(temp_data1,90*Angle_1))			//右转90度
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 0x8A:
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
				Speed=FAST_MOVE_SPEED;//2000
				if(do_action(3,100*CM_PLUS))		//直行5m
					{
						stop_rap();
						mode.step=0x88;
					}
#endif					
				break;
					
			case 0:
				if(mode.sub_mode==YBS_SUB_LEFT)
					{
						mode.step=0x40;
						return;
					}
				if(giv_sys_time-mode.bump_time<200)
					return;
				Speed=HIGH_MOVE_SPEED;		//800//2000
				forward(0xFF812345);
				mode.step = 1;
				//qz add 20180316

#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif				
				break;
			
			case 1:
				YBS_Check_corner(); 	//QZ:利用w_r的difference，计算distance，每次得到一个新的YBS_Wall_Distance
				//YBS_Distance_Check(0);
				Wall_Comm_Rap();
				if(YBS_Wall_Distance < 80)		//80	//	第一次找到墙
				//if(YBS_Wall_Distance<CONST_DIS+YBS_DISTANCE)
					{
						mode.step = 2;
#ifdef	YBS_Straight_FAST
						YBS_Straight_Time=giv_sys_time;
#endif

#ifdef YBS_DIS_RESTORE			//准备检查里程计算出的机器角度
						Enable_Rotate_Angle();
#endif
						return;
					}
				//QZ ADD
				//if(YBS_Wall_Distance>140) //140
				else
					{
						mode.step=3;
					}
				
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
				break;
							
			case 2:
				YBS_Check_corner(); 					
				//YBS_Distance_Check(0);
				Wall_Comm_Rap();
				if(YBS_Wall_Distance > 80)				//	彻底丢失墙壁	  有可能出现拐角//80  //140
				//if(YBS_Wall_Distance>=CONST_DIS+YBS_DISTANCE)
					{
						mode.step = 3;

#ifdef YBS_DIS_RESTORE		//出现空旷区域，停止检测里程计角度
						Disable_Rotate_Angle();
#endif
					}

#ifdef YBS_DIS_RESTORE		//检查里程计计算的角度，如果角度大于8度，恢复YBS_DISTANCE_CONST
				Check_Rotate_Angle();
				if((rotate_angle.rot_angle>(12.0))&(YBS_DISTANCE>YBS_DISTANCE_CONST))	//qz modify 20180902:8.0-->6.0-->12.0
					{
						YBS_DISTANCE=YBS_DISTANCE_CONST;
					}
#endif


#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
				break;
			
			case 3:
//					Wall_lost_Start_Pos = r_rap.pulse;							//	向前走一点 	//qz mask 20180203
				Wall_lost_Start_Pos=l_ring.all_length;
				r_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				l_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;

				mode.step = 4;	 
				break;
			case 4: 
				r_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				l_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				if((l_ring.all_length-Wall_lost_Start_Pos)>=3*CM_PLUS)
				{
					//QZ:ADD
					stop_rap();

					//qz add
					if(YBS_Wall_Distance<CONST_DIS+YBS_DISTANCE)
						{
							mode.step=0x00;
							return;
						}
					//qz add end
					mode.step = 0x10;
				}

				break;
			case 0x10:		
				Wall_lost_Start_Pos = l_ring.all_length;							//	旋转 
				r_rap.rap =YBS_LOST_CORNER_TURN_SPD_R;	//QZ:原来为300,350		
				l_rap.rap = YBS_LOST_CORNER_TURN_SPD ;
				l_rap.sign=1;
				r_rap.sign=1;
				l_rap.ori=FRONT;
				r_rap.ori=FRONT;
				mode.step = 0x11;
				YBS_DISTANCE=YBS_DISTANCE_CONST;		//qz add 20810803
				lost_turn_time=giv_sys_time;
				//qz add 20180316
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=false;
#endif
				break;
			case 0x11:
				YBS_Check_corner();

				//QZ ADD
				r_rap.rap =YBS_LOST_CORNER_TURN_SPD_R;	//QZ:原来为300,350		
				l_rap.rap = YBS_LOST_CORNER_TURN_SPD ;
					
				l_rap.ori=FRONT;
				l_rap.sign=1;
				r_rap.ori=FRONT;
				r_rap.sign=1;

				if(giv_sys_time-lost_turn_time>10000)			//qz add 20180902 1.5s qz modify 20181201 1.5s--->1.0s
					{
						YBS_DISTANCE=YBS_DISTANCE_CONST;
					}
				if((YBS_Wall_Distance <= (YBS_DISTANCE + 20))|(w_m.high_sign==NEAR)|(w_rm.high_sign==NEAR))//||(M_WALL_DIS<100)||(RM_WALL_DIS<100)) 			//	第二次找到墙	//qz modify 20181225
					{
						//QZ:ADD
						stop_rap();
					//	mode.step = 0x12;
						//qz add
						r_rap.rap=HIGH_MOVE_SPEED;
						l_rap.rap=HIGH_MOVE_SPEED;
						l_rap.ori=FRONT;
						l_rap.sign=1;
						r_rap.ori=FRONT;
						r_rap.sign=1;
						mode.step =0x01;
					}
				break;
			case 0x12:								//	停止旋转
				mode.step = 0x00;
				break;
			//--------------------------------------------------------
							

			
			//--------------------------------------------------------
			//--------------------------------------------------------
			case 0x40:	
				if(giv_sys_time-mode.bump_time<200)
					return;
				Speed=HIGH_MOVE_SPEED;	//2000
				forward(0xFF812345);
				mode.step = 0x41;				
				//qz add 20180316
			
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
								
				break;
			case 0x41:	
				YBS_Left_Check_corner();	//QZ:利用w_r的difference，计算distance，每次得到一个新的YBS_Wall_Distance
				//YBS_Left_Distance_Check();
				Wall_Comm_Rap();
				if(YBS_Wall_Distance < 80)		//80	//	第一次找到墙
					{
						mode.step = 0x42;
#ifdef	YBS_Straight_FAST
						YBS_Straight_Time=giv_sys_time;
#endif
			
#ifdef YBS_DIS_RESTORE		//准备检查里程计算出的机器角度
						Enable_Rotate_Angle();
#endif
					}
					//QZ ADD
				if(YBS_Wall_Distance>140)	//140
					{
						mode.step=0x43;
					}
								
#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
								
								
				break;
			case 0x42:
				YBS_Left_Check_corner();					
				//YBS_Left_Distance_Check();
				Wall_Comm_Rap();
				if(YBS_Wall_Distance > 140) 			//	彻底丢失墙壁	  有可能出现拐角//80
						{
							mode.step = 0x43;

#ifdef YBS_DIS_RESTORE		//出现空旷区域，停止检测里程计角度
							Disable_Rotate_Angle();
#endif

						}

#ifdef YBS_DIS_RESTORE		//检查里程计计算的角度，如果角度大于8度，恢复YBS_DISTANCE_CONST
				Check_Rotate_Angle();
				if((rotate_angle.rot_angle<(-8.0))&(YBS_DISTANCE>YBS_DISTANCE_CONST))
					{
						YBS_DISTANCE=YBS_DISTANCE_CONST;
					}
#endif


#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=true;
#endif
				break;
			//--------------------------------------------------------
			//--------------------------------------------------------
			case 0x43:
				Wall_lost_Start_Pos=r_ring.all_length;
				r_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				l_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;

				mode.step = 0x44;	 
				break;
			case 0x44:
				r_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				l_rap.rap = YBS_LOST_CORNER_FORWARD_SPEED;
				if((r_ring.all_length-Wall_lost_Start_Pos)>=3*CM_PLUS)
				{
					//QZ:ADD
					stop_rap();
					mode.step = 0x50;
				}

				//qz add
				if(YBS_Wall_Distance<80)
					{
						mode.step=0x40;
					}
				//qz add end

				break;
			case 0x50:
				Wall_lost_Start_Pos = r_ring.all_length;							//	旋转 
				r_rap.rap	=YBS_LOST_CORNER_TURN_SPD;	//QZ:原来为300,350		
				l_rap.rap	=YBS_LOST_CORNER_TURN_SPD_R ;
				l_rap.sign	=1;
				r_rap.sign	=1;
				l_rap.ori	=FRONT;
				r_rap.ori	=FRONT;
				mode.step	= 0x51;
				YBS_DISTANCE=YBS_DISTANCE_CONST;		//qz add 20810803

#ifdef FREE_SKID_INDEP_CHECK
				Free_Skid_Indep.check_flag=false;
#endif
				break;
			case 0x51:
				YBS_Left_Check_corner();

				//QZ ADD
				r_rap.rap =YBS_LOST_CORNER_TURN_SPD;	//QZ:原来为300,350		
				l_rap.rap = YBS_LOST_CORNER_TURN_SPD_R ;
				
				l_rap.ori=FRONT;
				l_rap.sign=1;
				r_rap.ori=FRONT;
				r_rap.sign=1;

#if 0			//取消底盘孤岛判断
				if(l_ring.all_length-Wall_lost_Start_Pos>(angle720+angle360))
					{
						stop_rap();
						mode.step=0x88;//0x12
#ifdef FREE_SKID_CHECK
						Enable_Free_Skid_Check();
#endif
#ifdef YBS_BLACK
						YBS_DISTANCE=30;
#endif
						return;
					}
									//QZ ADD END
#endif 
				if((YBS_Wall_Distance < (YBS_DISTANCE + 20))||(w_rm.sign==NEAR)||(w_m.high_sign==NEAR)|(w_lm.high_sign==NEAR))//||(M_WALL_DIS<100)||(RM_WALL_DIS<100))				//	第二次找到墙
						{
							//QZ:ADD
							stop_rap();
					//		mode.step = 0x12;
							//qz add
							r_rap.rap=HIGH_MOVE_SPEED;
							l_rap.rap=HIGH_MOVE_SPEED;
							l_rap.ori=FRONT;
							l_rap.sign=1;
							r_rap.ori=FRONT;
							r_rap.sign=1;
							mode.step =0x41;
						}
				break;
			default:
				mode.step=0x88;
				break;
		}	//	end of		switch (mode.step)	//step路径执行的步骤
}

void Nothing(void)
{
}

