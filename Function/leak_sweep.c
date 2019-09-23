#include "AAA-include.h"

u8 Analysis_StopLeak(short tgt_yaw)
{
	s8 now_gridx,now_gridy;
	s8 temp_x,temp_y;
	s8 ydir=Read_Motion_YDir();
	now_gridx=grid.x;now_gridy=grid.y;
	//if(tgt_yaw==F_Angle)
	if(tgt_yaw==F_Angle_Const)
		{
			temp_x=now_gridx;
			if(temp_x+1>GRID_MAX)
				temp_x=GRID_MAX;
			else
				temp_x+=1;
			
			if(ydir>0)		//Y轴正方向
				{
					temp_y=now_gridy+1;
					if(temp_y>grid.y_area_max)
						temp_y=now_gridy;
				}
			else if(ydir<0)				//Y轴负方向
				{
					temp_y=now_gridy-1;
					if(temp_y<grid.y_area_min)
						temp_y=now_gridy;
				}
			else
				{
					return 0;
				}
			
			//if(Read_Coordinate_Clean(temp_x,grid.y))
			if(Read_Coordinate_Clean(temp_x,now_gridy)&(Read_Coordinate_Clean(now_gridx,temp_y)))
				{
					return 1;
				}
		}
	else
		{
			temp_x=now_gridx;
			if(temp_x-1<GRID_MIN)
				temp_x=GRID_MIN;
			else
				temp_x-=1;

			if(ydir>0)		//Y轴正方向
				{
					temp_y=now_gridy+1;
					if(temp_y>grid.y_area_max)
						temp_y=now_gridy;
				}
			else if(ydir<0)				//Y轴负方向
				{
					temp_y=now_gridy-1;
					if(temp_y<grid.y_area_min)
						temp_y=now_gridy;
				}
			else
				{
					return 0;
				}
			
			//if(Read_Coordinate_Clean(temp_x,grid.y))
			if(Read_Coordinate_Clean(temp_x,now_gridy)&(Read_Coordinate_Clean(now_gridx,temp_y)))
				{
					return 1;
				}
		}

	
	return 0;
}

u8 Analysis_NextLeakSweep(void)
{
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	if(motion1.tgt_yaw==F_Angle_Const)
		{
			if(Read_Coordinate_Clean(now_gridx+1,now_gridy))
				return 1;
		}
	else
		{
			if(Read_Coordinate_Clean(now_gridx-1,now_gridy))
				return 1;
		}
	return 0;
}

#if 0
void LeakSweep_Bump_Action(void)
{
	u8 m=0;
	static u8 turn_dir=0;
	static short tgt_angle=0;
	s8 ydir=Read_Motion_YDir();
	m=Read_Sweep_Bump(1,1);
	switch(mode.bump)
		{
			case 9:
			case BUMP_MID:
				switch(mode.step_bp)
					{
						case 0:
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
							break;
						case 1:
							if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
								return;
							if(mode.bump==90)
								mode.step_bp++;
							else
								mode.step_bp+=2;
							break;
						case 2:
							Speed=1000;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
									mode.bump_time=giv_sys_time;
								}
							break;
						case 3:
							if(giv_sys_time-mode.bump_time<100)
								return;
#ifdef LEAK_BACK_ENABLE
							if(Analysis_NeedBack(grid.y))		//判断回扫条件
								{
									mode.step_bp=10;
								}
							else
#endif
								{
									mode.step_bp++;
								}
							break;
						case 4:
							if((motion1.repeat_sweep)&(!Read_Motion_BackSweep()))
								{
									Init_Pass2LeakSweep();
									motion1.repeat_sweep=false;
									return;
								}
							if(!Read_LeftRight())		//需要准备左沿边
								{
									turn_dir=2; 		//左沿边，碰撞向右转
								}
							else						//需要准备右沿边
								{
									turn_dir=1; 		//右沿边，碰撞向左转
								}
							mode.step_bp++;
							break;
						case 5:
							Speed=1000;
							if(do_action(turn_dir,90*Angle_1))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 6:
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							if(!Read_LeftRight())		//需要准备左沿边
								{Init_Sweep_LeftYBS(1);}
							else
								{Init_Sweep_RightYBS(1);}
							break;
						case 10:						//准备回扫进入
							Set_Motion_BackSweep(1);	//设置回扫标志
							Save_Abort_Data();
							Change_LeftRight();
							mode.step_bp=4;
							//Logout_Area_Coordinate();
							//while(1);
							break;
					}
				break;
				
			case BUMP_ONLY_LEFT:
#if 1
				switch(mode.step_bp)
					{
						case 0:
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
						break;
						case 1:
							if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
								return;
							Speed=1000;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 2:
							mode.step_bp++;
							break;
						case 3:
							Speed=1000;
							if(do_action(2,20*Angle_1))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 4:
							Init_Sweep_LeftYBS(1);
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							break;
					}
				break;
#endif
			case BUMP_ONLY_LEFTMID:
			case BUMP_LEFT_MID:
			case LM_WALL_BUMP:
				switch(mode.step_bp)
					{
						case 0:
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
							break;
						case 1:
							if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
								return;
							if(mode.bump==LM_WALL_BUMP)
								mode.step_bp+=2;
							else
							mode.step_bp++;
							break;
						case 2:
							Speed=1000;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
									mode.bump_time=giv_sys_time;
								}
							break;
						case 3:
							if(giv_sys_time-mode.bump_time<100)
								return;
#ifdef LEAK_BACK_ENABLE
							if(Analysis_NeedBack(grid.y))		//判断回扫条件
								{
									mode.step_bp=10;
								}
							else
#endif
								{
									mode.step_bp++;
								}
							break;
						case 4:
							if(motion1.repeat_sweep&(!Read_Motion_BackSweep()))
								{
									Init_Pass2LeakSweep();
									motion1.repeat_sweep=false;
									return;
								}
							if(!Read_LeftRight())		//需要准备左沿边
								{
									turn_dir=2; 		//左沿边，碰撞向右转
								}
							else						//需要准备右沿边
								{
									turn_dir=1; 		//右沿边，碰撞向左转
								}
							mode.step_bp++;
							break;
						case 5:
							Speed=1000;
							if(do_action(turn_dir,40*Angle_1))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 6:
							if(!Read_LeftRight())		//需要准备左沿边
								{
									Init_Sweep_LeftYBS(1);
								}
							else					
								{
									Init_Sweep_RightYBS(1);
								}
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							break;

						case 10:
							Set_Motion_BackSweep(1);
							Save_Abort_Data();
							Change_LeftRight();
							mode.step_bp=4;
							break;
					}
				break;
			case BUMP_ONLY_RIGHT:
#if 1
				switch(mode.step_bp)
					{
						case 0:
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
						break;
						case 1:
							if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
								return;
							Speed=1000;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 2:
							Speed=1000;
							if(do_action(1,20*Angle_1))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 3:
							Init_Sweep_RightYBS(1);
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							break;
					}
				break;
#endif
			case BUMP_ONLY_RIGHTMID:
			case BUMP_RIGHT_MID:
			case RM_WALL_BUMP:
				switch(mode.step_bp)
					{
						case 0:
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
							break;
						case 1:
							if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
								return;
							if(mode.bump==LM_WALL_BUMP)
								mode.step_bp+=2;
							else
							mode.step_bp++;
							break;
						case 2:
							Speed=1000;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 3:
							if(giv_sys_time-mode.bump_time<100)
								return;
#ifdef LEAK_BACK_ENABLE
							if(Analysis_NeedBack(grid.y))		//判断回扫条件
								{
									mode.step_bp=10;
								}
							else
#endif
								{
									mode.step_bp++;
								}
							break;
						case 4:
							if(motion1.repeat_sweep&(!Read_Motion_BackSweep()))
								{
									Init_Pass2LeakSweep();
									motion1.repeat_sweep=false;
									return;
								}
							if(!Read_LeftRight())		//需要准备左沿边
								{
									turn_dir=2; //左沿边，碰撞向右转
								}
							else				//需要准备右沿边
								{
									turn_dir=1; //右沿边，碰撞向左转
								}
							mode.step_bp++;
							break;
						case 5:
							Speed=1000;
							if(do_action(turn_dir,40*Angle_1))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 6:
							if(!Read_LeftRight())		//需要准备左沿边
								{
									Init_Sweep_LeftYBS(1);
								}
							else					
								{
									Init_Sweep_RightYBS(1);
								}
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							break;
						case 10:
							Set_Motion_BackSweep(1);		//设置回扫标志
							Save_Abort_Data();
							Change_LeftRight();
							mode.step_bp=4;
							break;
					}
				break;
			case BUMP_OUTRANGE:
				switch(mode.step_bp)
					{
						case 0:
							stop_rap();
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
//							TRACE("Set clean grid.x=%d grid.y=%d",grid.x,grid.y);
//							TRACE("Read grid.x grid.y clean=%d\r\n",Read_Coordinate_Data(grid.x,grid.y));
							break;
						case 1:
							if(giv_sys_time-mode.bump_time<100)
								return;
							Speed=1000;
							if(do_action(4,10*CM_PLUS))
								{
									stop_rap();
									if(Check_OutofRange())
										{
											mode.step_bp=1;
											return;
										}
									else
										{
											mode.step_bp++;
											return;
										}
								}
							if(!Check_OutofRange())
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 2:
#ifdef LEAK_BACK_ENABLE
							if(Analysis_NeedBack(grid.y))		//判断回扫条件
								{
									mode.step_bp=10;
								}
							else
#endif
								{
									mode.step_bp++;
								}
							break;
						case 3:
							if(motion1.repeat_sweep&(!Read_Motion_BackSweep()))
								{
									Init_Pass2LeakSweep();
									motion1.repeat_sweep=false;
									return;
								}
							if(ydir>=0)			//沿Y轴正方向清扫
								{
									if(!Read_Motion_BackSweep())
										tgt_angle=R_Angle_Const;
									else
										tgt_angle=L_Angle_Const;
								}
							else
							//沿Y轴负方向清扫
								{
									if(!Read_Motion_BackSweep())
										tgt_angle=L_Angle_Const;
									else
										tgt_angle=R_Angle_Const;
								}
//							turn_dir=Get_TurnDir(tgt_angle);
							TRACE("tgt_yaw=%d in %s %d \r\n",tgt_angle,__func__,__LINE__);
							mode.step_bp++;
							break;
						case 4:
							Speed=1000;
							turn_dir=Get_TurnDir(tgt_angle);
							if(do_action(turn_dir,720*Angle_1))
								{
									stop_rap();
									//mode.step_bp++;
								}
							if(Judge_Yaw_Reach(tgt_angle,TURN_ANGLE_BIOS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 5:
							Speed=1000;
							if(do_action(3,HORIZON_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp=7;
								}
							if((m!=0)&(m<BUMP_OUTRANGE))
								{	
									stop_rap();
									//mode.step_bp++;
									//mode.step_bp=20;
									//mode.step_bp=6;
									if(!Read_LeftRight())		//需要准备左沿边
										{
											Init_Sweep_LeftYBS(1);
										}
									else					
										{
											Init_Sweep_RightYBS(1);
										}
									mode.bump=0;
									mode.step_bp=0;
									mode.bump_flag=false;
									break;
								}
							//if(grid.y!=grid.y_straight_start)
							if((grid.y!=grid.y_straight_start)&(Judge_GridYPOS_Nearby_Reach(grid.y_straight_start)))
								{
									stop_rap();
									mode.step_bp=7;
								}
							break;
						case 6:
							Speed=1000;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 7:
							if((Check_OutofRange())==BUMP_YOUTRANGE)
								{	
									mode.step_bp=6;
									return;
								}
							TRACE("Out of Range action complete!\r\n");
							TRACE("Prepare to Pass to Sweep!\r\n");
							Init_Pass2LeakSweep();
							break;
						case 10:
							Set_Motion_BackSweep(1);		//设置回扫标志
							Save_Abort_Data();
							Change_LeftRight();
							mode.step_bp=3;
							break;
					}
		}
}
#endif

void Init_LeakSweep(short tgt_yaw)
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
#if 0
	if(DOCK_SWEEP)
		Sweep_Level_Set(DOCK_SWEEP_LEVEL);
	else
		Sweep_Level_Set(sweep_suction);
#endif		 
	Init_Action();
	
	mode.mode = SWEEP;			
	mode.sub_mode = LEAK_SWEEP;
	mode.step=0;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.status=1;
//		WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init LEAK SWEEP Mode Complete!\r\n");
#endif
	//初始化检测的条件
	CHECK_STATUS_FLAG=true; 	//qz add 20180725:如果机器处于休眠时,接到控制命令会直接进入此状态,因此需要打开检测开关
//	Init_Check_Status();//qz add 20180425
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//清除接收缓存qz add 20180703
	
	if(dis_err_code==DIS_ERROR_DOCK_FAIL)		//qz add 20180710
		{
			dis_err_code=0;
			error_code=0;
		}

#ifdef UV
	if((mode.status)&(!SLAM_DOCK))		//qz add 20180902
		Set_UV();
	else
		Reset_UV();
#endif
	motion1.tgt_yaw=tgt_yaw;
	motion1.anti_tgt_yaw=Get_Reverse_Angle(tgt_yaw);
	
	if(check_point.ydir)
		Set_Motion_YDir(1);
	else
		Set_Motion_YDir(-1);
	Assign_LeftRight();
	
	motion1.leakon=true;
	grid.x_straight_start=grid.x;
	grid.y_straight_start=grid.y;
	
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgt_yaw);

	if(Analysis_StopLeak(motion1.tgt_yaw))
		{	
			TRACE("Analysis Stop leak sweep!\r\n");
			TRACE("goto mode step 0xF0,Check leak area again!\r\n");
			mode.step=0xF0;
		}

	delay_ms(500);
	
}

void Do_LeakSweep(void)
{
	ACC_DEC_Curve();

	//LeakSweep_Bump_Action();
	Sweep_Bump_Action(1,1);
	
	if(mode.bump)
		return;
	if((mode.sub_mode!=LEAK_SWEEP))		//因为在Sweep_Bump_Action()中可能会切换模式到PASS2SWEEP
		return;
	switch (mode.step)
		{
			case 0:
				mode.time=giv_sys_time;
				mode.step++;
			break;
			case 1:
				Speed=HIGH_MOVE_SPEED;
				//do_action(3,FARAWAY);
				do_action_my(3,FARAWAY,motion1.tgt_yaw);
				if((w_m.high_sign==NEAR)|(w_r.high_sign==NEAR)|(w_l.high_sign==NEAR)|(w_lm.high_sign==NEAR)|(w_rm.high_sign==NEAR))
					{
						r_rap.rap=HIGH_MOVE_SPEED;
						l_rap.rap=HIGH_MOVE_SPEED;
					}
				else if(giv_sys_time-mode.time>10000)
					{
						if((grid.x>=grid.x_area_max-1)|(grid.x<=grid.x_area_min+1))
							{
								l_rap.rap=HIGH_MOVE_SPEED;
								r_rap.rap=HIGH_MOVE_SPEED;
							}
						else
							{
								r_rap.rap=TOP_MOVE_SPEED;
								l_rap.rap=TOP_MOVE_SPEED;
							}
					}

				break;
			case 0xF0:
				Area_Check(0);
			//qz add 20190307 X坐标超出4M范围的措施
		}
}

void Init_Leak_BackSweep(short tgt_yaw)
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
#if 0
	if(DOCK_SWEEP)
		Sweep_Level_Set(DOCK_SWEEP_LEVEL);
	else
		Sweep_Level_Set(sweep_suction);
#endif		 
	Init_Action();
	
	mode.mode = SWEEP;			
	mode.sub_mode = LEAK_BACKSWEEP;
	mode.step=0;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	
	mode.status=1;
//		WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init BACK SWEEP Mode Complete!\r\n");
#endif
	//初始化检测的条件
	CHECK_STATUS_FLAG=true; 	//qz add 20180725:如果机器处于休眠时,接到控制命令会直接进入此状态,因此需要打开检测开关
//	Init_Check_Status();//qz add 20180425
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//清除接收缓存qz add 20180703
	
	if(dis_err_code==DIS_ERROR_DOCK_FAIL)		//qz add 20180710
		{
			dis_err_code=0;
			error_code=0;
		}

#ifdef UV
	if((mode.status)&(!SLAM_DOCK))		//qz add 20180902
		Set_UV();
	else
		Reset_UV();
#endif
	motion1.tgt_yaw=tgt_yaw;
	motion1.anti_tgt_yaw=Get_Reverse_Angle(tgt_yaw);
#if 1
	if(check_point.ydir)				//Y轴正方向
		{
			if(motion1.tgt_yaw==F_Angle_Const)
				Set_LeftRight(1);			//设置方向左沿边
			else
				Set_LeftRight(0);			//设置方向右沿边
			Set_Motion_YDir(-1);
		}
	else									//Y轴负方向
		{
			if(motion1.tgt_yaw==F_Angle_Const)
				Set_LeftRight(0);			//设置方向右沿边
			else							
				Set_LeftRight(1);			//设置方向左沿边
			Set_Motion_YDir(1);
		}
#else
	Assign_LeftRight(check_point.yacc_anti);
#endif
	grid.x_straight_start=grid.x;
	grid.y_straight_start=grid.y;
	grid.y_back_last=grid.y;
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgt_yaw);
	if(Analysis_StopBack(motion1.tgt_yaw))
		{
			//退出回扫
			TRACE("Analysis Stop back sweep!\r\n");
			TRACE("Quit Back Sweep in %s\r\n",__func__);
			//Init_Stop_LeakBack();
			Init_Stop_BackSweep();
		}

	delay_ms(500);
}

void Do_Leak_BackSweep(void)
{
	s8 now_gridx,now_gridy;
	u8 turn_dir;

	now_gridx=grid.x;now_gridy=grid.y;
	
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
			mode.speed_up=false;		//qz add 20181225

			return;
		}
	Sweep_Bump_Action(0,1);
	if(mode.bump)
		return;
	if((mode.sub_mode!=LEAK_BACKSWEEP))		//因为在Sweep_Bump_Action()中可能会切换模式到PASS2SWEEP
		return;
	switch (mode.step)
		{
			case 0:
				mode.time=giv_sys_time;
				mode.step++;
			break;
			case 1:
				Speed=HIGH_MOVE_SPEED;
				//do_action(3,FARAWAY);
				if(do_action_my(3,FARAWAY,motion1.tgt_yaw));
				if((w_m.high_sign==NEAR)|(w_r.high_sign==NEAR)|(w_l.high_sign==NEAR)|(w_lm.high_sign==NEAR)|(w_rm.high_sign==NEAR))
					{
						r_rap.rap=HIGH_MOVE_SPEED;
						l_rap.rap=HIGH_MOVE_SPEED;
					}
				else if(giv_sys_time-mode.time>10000)
					{
						if((grid.x>=grid.x_area_max-1)|(grid.x<=grid.x_area_min+1))
							{
								l_rap.rap=HIGH_MOVE_SPEED;
								r_rap.rap=HIGH_MOVE_SPEED;
							}
						else
							{
								r_rap.rap=TOP_MOVE_SPEED;
								l_rap.rap=TOP_MOVE_SPEED;
							}
					}

				if((motion1.tgt_yaw==F_Angle_Const))
					{
						if((Read_Coordinate_Clean(now_gridx+1,now_gridy))&(now_gridx+1<=GRID_MAX))
							{
								stop_rap();
								TRACE("Now Grid.x=%d y=%d\r\n",now_gridx,now_gridy);
								TRACE("coor[%d][%d] has been clean,goto next!!!\r\n",now_gridy,now_gridx+1);
								mode.step++;
							}
					}

				if(motion1.tgt_yaw==B_Angle_Const)
					{
						if((Read_Coordinate_Clean(now_gridx-1,now_gridy))&(now_gridx-1>=GRID_MIN))
							{
								stop_rap();
								mode.step++;
							}
					}

				#if 0
				//if((coordinate[grid.y][grid.x].clean))
				if(Read_Coordinate_Clean(grid.x,grid.y))
					{
						TRACE("BACK OFF!!!\\r\n::");
						while(1);
					}
				#endif
				//qz add end
				break;
			//qz add 20190307 X坐标超出4M范围的措施
			case 2:
				if(!Read_LeftRight())		//需要准备左沿边
					{
						turn_dir=2; //左沿边，碰撞向右转
					}
				else				//需要准备右沿边
					{
						turn_dir=1; //右沿边，碰撞向左转
					}
				Speed=TURN_SPEED;
				if(do_action(turn_dir,100*Angle_1))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 3:
				Speed=TURN_SPEED;
				if(do_action(3,HORIZON_LENGTH*CM_PLUS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 4:
				//Init_Pass2LeakSweep();
				Init_Pass2Sweep();
				break;
			//qz add end
				
			case 0xff:
				break;
					
		}
}

#if 0
void Init_Pass2LeakSweep(void)
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
	
	mode.mode = SWEEP;			
	mode.sub_mode = PASS2LEAKSWEEP;
	mode.step=0;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919

	mode.status=1;
	WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init PASS2LeakSweep Mode Complete!\r\n");
#endif
	//初始化检测的条件
	CHECK_STATUS_FLAG=true; 	//qz add 20180725:如果机器处于休眠时,接到控制命令会直接进入此状态,因此需要打开检测开关
//	Init_Check_Status();//qz add 20180425
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//清除接收缓存qz add 20180703
	
	if(dis_err_code==DIS_ERROR_DOCK_FAIL)		//qz add 20180710
		{
			dis_err_code=0;
			error_code=0;
		}

#ifdef UV
	if((mode.status)&(!SLAM_DOCK))		//qz add 20180902
		Set_UV();
	else
		Reset_UV();
#endif
}

void Do_Pass2LeakSweep(void)
{
	short turn_angle=0;
//	YBS_Comm_Rap_My();
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
			mode.speed_up=false;		//qz add 20181225

			return;
		}
	
	Pass2Sweep_Bump_Action();
	if(mode.bump)
		return;
	switch (mode.step)
		{
			case 0:
				stop_rap();
				if(giv_sys_time-mode.time>5000)
					{
						{
							if(!Read_LeftRight())		//需要准备左沿边
								{
									mode.step=10;
									return;
								}
							else						//当前右沿边，左转。tgt_yaw为B_Anlge,需要转向到F_Angle;
								{
									mode.step=20;
									return;
								}
						}
					}
				break;
			case 10:
				Speed=800;
				if(do_action(2,720*Angle_1));
				if(motion1.tgt_yaw==F_Angle_Const)
					turn_angle=B_Angle_Const;
				else
					turn_angle=F_Angle_Const;
				if(Judge_Yaw_Reach(turn_angle,TURN_ANGLE_BIOS))
					{
						stop_rap();
						TRACE("Gyro_Data.yaw=%d turn_angle=%d\r\n",Gyro_Data.yaw,turn_angle);
						TRACE("yaw Reach!\r\n");
						mode.step=0xF0;
					}
				break;
			case 20:
				Speed=800;
				if(do_action(1,720*Angle_1));
				if(motion1.tgt_yaw==F_Angle_Const)
					turn_angle=B_Angle_Const;
				else
					turn_angle=F_Angle_Const;
				if(Judge_Yaw_Reach(turn_angle,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step=0xF0;
					}
				break;
			case 0xF0:
				if(motion1.tgt_yaw==F_Angle_Const)
					{
						turn_angle=B_Angle_Const;
					}
				else
					{	
						turn_angle=F_Angle_Const;
					}
				Init_LeakSweep(turn_angle);
				break;
		}
}

void Init_Stop_LeakBack(void)
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
	Enable_Speed(); 				//允许速度发
	
	mode.mode=SWEEP;
	mode.sub_mode=STOP_LEAKBACK;
	mode.step=0;
	mode.bump=0;
	mode.step_bp=0;
	mode.abnormity=0;
	mode.step_abn=0;
	mode.bump_flag=0;
	mode.time=giv_sys_time;
	while(giv_sys_time-mode.time<5000);
	delay_ms(500);
	TRACE("Init Stop Backsweep complete!!!\r\n");
	TRACE("grid.x_abort=%d y_abort=%d\r\n",grid.x_abort,grid.y_abort);
	short xpos=Return_GridXPos_Point(grid.x_abort);
	short ypos=Return_GridYPos_Point(grid.y_abort);
	TRACE("target actually xpos=%d ypos=%d\r\n",xpos,ypos);
}

void StopLeakBack_Bump_Action(void)
{
	u8 m=0;
	static bool turn_flag=false;
	static u8 turn_dir=0;
	static u16 ypos_dif=0;
	short ypos;
	ypos=Gyro_Data.y_pos;
	m=Read_Sweep_Bump(0,0);
	if(Judge_Ypos_Reach(motion1.ypos_abort,5))
		{
			if(mode.step==3)
				{
					stop_rap();
					TRACE("Approach Y in %s\r\n",__func__);
					mode.bump=0;
					mode.bump_flag=false;
					mode.step_bp=0;
					mode.step++;
					return;
				}
		}
	if(mode.bump==0)
		return;
	switch(mode.step_bp)
		{
			case 0:
				mode.bump_time=giv_sys_time;
				mode.step_bp++;
				turn_flag=false;
			break;
			case 1:
				if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
					return;
				Speed=1000;
				if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
					{
						stop_rap();
						mode.step_bp++;
					}
				break;
			case 2:
				if((mode.step!=3)&(mode.step!=7))
					{
						mode.step=0;
						mode.bump=0;
						mode.bump_flag=false;
						mode.step_bp=0;
					}
				else if(mode.step==3)			//沿Y轴移动
					{
						mode.step_bp=3;			
					}
				else							//沿X轴移动
					{
						mode.step_bp=7;						
					}
				break;
			case 3:
#if 0
				if(motion1.tgt_yaw==L_Angle_Const)
					{
						if(xpos>motion1.xpos_abort)
							{
								turn_dir=1;
							}
						else
							{
								turn_dir=2;
							}
					}
				else
					{
						if(xpos>motion1.xpos_abort)
							{
								turn_dir=2;
							}
						else
							{
								turn_dir=1;
							}
					}
#else
				if(motion1.tgt_yaw==L_Angle_Const)
					{
						turn_dir=2;
					}
				else
					{
						turn_dir=1;
					}
#endif
				ypos_dif=abs(motion1.ypos_abort-ypos);
				mode.step_bp++;
				break;
			case 4:
				Speed=1000;
				if(do_action(turn_dir,40*Angle_1))
					{
						stop_rap();
						mode.step_bp++;
					}
				break;
			case 5:
				Speed=1000;
				if(do_action(3,5*CM_PLUS))
					{
						stop_rap();
						mode.step_bp++;
					}
				if(m)
					{
						stop_rap();
						mode.step_bp=8;
						mode.bump_time=giv_sys_time;
					}
				break;
			case 6:
				if(turn_dir==1)
					{
						enable_rap_no_length(FRONT,1000,FRONT,300);
					}
				else
					{
						enable_rap_no_length(FRONT,300,FRONT,1000);
					}
				#if 0
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.bump=0;
						mode.step_bp=0;
						mode.bump_flag=false;
						mode.step=3;
					}
				#endif
				if(m)
					{
						stop_rap();
						mode.step_bp=8;
						mode.bump_time=giv_sys_time;
					}
				break;
			case 8:
				if(giv_sys_time-mode.bump_time<1000)
					return;
				Speed=1000;
				if(do_action(4,2*CM_PLUS))
					{
						stop_rap();
						mode.step_bp=4;
						if(abs(motion1.ypos_abort-ypos)>ypos_dif+25)
							{
								if(!turn_flag)
									{
										if(turn_dir==1)
											turn_dir=2;
										else
											turn_dir=1;
										turn_flag=true;
									}
							}
					}
				break;
				
			case 7:
				m=mode.bump;
				//Init_NormalSweep(motion1.tgt_yaw);
				Init_LeakSweep(motion1.tgt_yaw);
				mode.bump=m;
				mode.step_bp=0;
				break;
		}
}

void Do_Stop_LeakBack(void)
{
	short ypos,tgt_xpos,tgt_ypos;
	static u8 turn_dir=0;
	ypos=Gyro_Data.y_pos;
	tgt_xpos=Return_GridXPos_Point(grid.x_abort);tgt_ypos=Return_GridYPos_Point(grid.y_abort);

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
			mode.speed_up=false;		//qz add 20181225

			return;
		}

	
	StopBack_Bump_Action();
	if((Judge_Pos_Reach(tgt_xpos,tgt_ypos))&(mode.step<4))
		{
			stop_rap();
			TRACE("pos reached in %s\r\n",__func__);
			TRACE("goto mode.step 4\r\n");
			mode.step=4;
			mode.bump=0;
			mode.step_bp=0;
		}
	if(mode.bump)
		return;
	
	switch(mode.step)
		{	
			case 0:
				if(ypos+3<tgt_ypos)			//当前Y坐标小于中断坐标，需要转到R_ANGLE_CONST
					{
						motion1.tgt_yaw=R_Angle_Const;
					}
				else if(ypos>tgt_ypos+3)			//当前Y坐标大于中断坐标，需要转到R_ANGLE_CONST
					{
						motion1.tgt_yaw=L_Angle_Const;
					}
				else
					{
						TRACE("now ypos=%d tgt_ypos=%d\r\n",ypos,tgt_ypos);
						TRACE("goto mode step 4!!\r\n");
						mode.step=4;
						return;
					}
				mode.step++;
				break;
			case 1:
//				turn_dir= Get_TurnDir(motion1.tgt_yaw);
				mode.step++;
				break;
			case 2:
				Speed=1000;
				turn_dir= Get_TurnDir(motion1.tgt_yaw);
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 3:
				Speed=1000;
				do_action_my(3,FARAWAY,motion1.tgt_yaw);
				if(Judge_GridYPOS_Reach(grid.y_abort, motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 4:
				Set_Motion_BackSweep(0);	//清空回扫标志
				TRACE("Stop BackSweep action complete!\r\n");
				TRACE("Prepare enter to Normal Sweep and then Load Data!\r\n");
				Load_Abort_Data();		//然后再装在中断之前的数据
				mode.step++;
				break;
			case 5:
//				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				mode.step++;
				break;
			case 6:
				Speed=1000;
				turn_dir=Get_TurnDir(motion1.tgt_yaw);
				if(do_action(turn_dir,720*Angle_1))
					{
						stop_rap();
						mode.step=6;
					}
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 7:
				Init_LeakSweep(motion1.tgt_yaw);		//先进入正常直行清扫模式
				#if 0
				Speed=2000;
				if(do_action(3,FARAWAY));
				if(Judge_Xpos_Reach(motion1.xpos_abort,5))
					{
						stop_rap();
						Init_NormalSweep(motion1.tgt_yaw);		//先进入正常直行清扫模式
					}
				#endif
				break;
		}
}
#endif

void No_Thingleak(void)
{

}

