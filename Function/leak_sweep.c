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
			if(temp_x+1>grid.x_area_max)
				temp_x=grid.x_area_max;
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
			if(temp_x-1<grid.x_area_min)
				temp_x=grid.x_area_min;
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
	static u8 turn_dir=0;
	u8 area_check=0;
	s8 now_gridx,now_gridy,ydir=0;
	ACC_DEC_Curve();
	clr_all_hw_effect();
	//LeakSweep_Bump_Action();
	Sweep_Bump_Action(1,1);

	now_gridx=grid.x;now_gridy=grid.y;
	ydir=Read_Motion_YDir();
	
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
				
				if((motion1.tgt_yaw==B_Angle_Const)&(motion1.start_seat))//&(motion1.area_num<=2))
					{
						if(Analysis_InSeatArea(now_gridx,now_gridy))
							{
								stop_rap();
								mode.step++;
								TRACE("Motion in seat area!!!\r\n");
							}
					}
				break;
			case 2:
				if(ydir>0)
					{
						if(now_gridy+1>grid.y_area_max)
							{
								area_check=Area_Check(0);
								if(area_check==4)
									{
										Init_Docking();
									}
								else
									{
										Init_Shift_Point1(0);
									}
								return;
							}
					}
				else if(ydir<0)
					{
						if(now_gridy-1<grid.y_area_min)
							{
								area_check=Area_Check(0);
								if(area_check==4)
									{
										Init_Docking();
									}
								else
									{
										Init_Shift_Point1(0);
									}
								return;
							}
					}
				if(motion1.repeat_sweep)
					{
						TRACE("call this in %d %s\r\n",__LINE__,__func__);
						Init_Pass2Sweep();
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
				mode.step++;
				break;
			case 3:
				Speed=TURN_SPEED;
				if(do_action(turn_dir,100*Angle_1))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 4:
				Speed=HIGH_MOVE_SPEED;
				if(do_action(3,HORIZON_LENGTH*CM_PLUS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 5:
				//Init_Back_Sweep();
				TRACE("call this in %d %s\r\n",__LINE__,__func__);
				Init_Pass2Sweep();
				break;
			case 0xF0:
				area_check=Area_Check(0);
				if(area_check==4)
					{
						Init_Docking();
					}
				else
					{
						Init_Shift_Point1(0);
					}
				break;
			//qz add 20190307 X坐标超出4M范围的措施
		}
}

void Init_Leak_BackSweep(short tgt_yaw)
{
	u8 data1;
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
	data1=Analysis_StopBack(motion1.tgt_yaw);
	if(data1==1)
		{
			{
				TRACE("Analysis Stop back sweep!\r\n");
				TRACE("Quit Back Sweep in %s\r\n",__func__);
				Init_Stop_BackSweep();
			}
		}
	else if(data1==2)
		{
			mode.step=2;
		}
	delay_ms(500);
}

void Do_Leak_BackSweep(void)
{
	s8 now_gridx,now_gridy,ydir;
	static u8 turn_dir;

	now_gridx=grid.x;now_gridy=grid.y;
	ydir=Read_Motion_YDir();
	
	ACC_DEC_Curve();
	clr_all_hw_effect();
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
						if((Read_Coordinate_Clean(now_gridx+1,now_gridy))&(now_gridx+1<=grid.x_area_max))
							{
								stop_rap();
								TRACE("Now Grid.x=%d y=%d\r\n",now_gridx,now_gridy);
								TRACE("coor[%d][%d] has been clean,goto next!!!\r\n",now_gridy,now_gridx+1);
								mode.step++;
							}
					}

				if(motion1.tgt_yaw==B_Angle_Const)
					{
						if((Read_Coordinate_Clean(now_gridx-1,now_gridy))&(now_gridx-1>=grid.x_area_min))
							{
								stop_rap();
								mode.step++;
							}
					}
				if((motion1.tgt_yaw==B_Angle_Const)&(motion1.start_seat))//&(motion1.area_num<=2))
					{
						if(Analysis_InSeatArea(now_gridx,now_gridy))
							{
								stop_rap();
								mode.step=20;
								TRACE("Motion in seat area!!!\r\n");
							}
					}
				break;
			case 20:
				if(motion1.repeat_sweep)
					{
						TRACE("Call this in %s %d\r\n",__func__,__LINE__);
						Init_Pass2Sweep();
					}
				else
					{
						mode.step=2;
					}
				break;
			case 2:
				if(ydir>0)
					{
						if(now_gridy+1>grid.y_area_max)
							{
								TRACE("STOP BACK in %s %d\r\n",__func__,__LINE__);
								Init_Stop_BackSweep();
								return;
							}
					}
				else if(ydir<0)
					{
						if(now_gridy-1<grid.y_area_min)
							{
								TRACE("STOP BACK in %s %d\r\n",__func__,__LINE__);
								Init_Stop_BackSweep();
								return;
							}
					}
				
				if(!Read_LeftRight())		//需要准备左沿边
					{
						turn_dir=2; //左沿边，碰撞向右转
					}
				else				//需要准备右沿边
					{
						turn_dir=1; //右沿边，碰撞向左转
					}
				mode.step++;
				break;
			case 3:
				Speed=TURN_SPEED;
				if(do_action(turn_dir,100*Angle_1))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 4:
				Speed=TURN_SPEED;
				if(do_action(3,HORIZON_LENGTH*CM_PLUS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 5:
				//Init_Pass2LeakSweep();
				Init_Pass2Sweep();
				break;
			//qz add end
				
			case 0xff:
				break;
					
		}
}


void No_Thingleak(void)
{

}

