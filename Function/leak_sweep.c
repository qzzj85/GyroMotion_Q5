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
	Init_Action();
	
	mode.mode = MODE_SWEEP;			
	mode.sub_mode = SUBMODE_SWEEP_LEAK;
	mode.step=0;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.status=1;
//		WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init LEAK SWEEP Mode Complete!\r\n");
#endif
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//清除接收缓存qz add 20180703
	
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
#ifdef DEBUG_SWEEP	
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgt_yaw);
#endif
	if(Analysis_StopLeak(motion1.tgt_yaw))
		{	
#ifdef DEBUG_SWEEP	
			TRACE("Analysis Stop leak sweep!\r\n");
			TRACE("goto mode step 0xF0,Check leak area again!\r\n");
#endif
			mode.step=0xF0;
		}

//	delay_ms(100);
	
}

void Do_LeakSweep(void)
{
	static u8 turn_dir=0;
	s8 now_gridx,now_gridy,ydir=0;
	short now_angle=Gyro_Data.yaw;
	static bool speed_on=FALSE;
	
	ACC_DEC_Curve();
	clr_all_hw_effect();
	//LeakSweep_Bump_Action();
	Sweep_Bump_Action(1,1);

	now_gridx=grid.x;now_gridy=grid.y;
	ydir=Read_Motion_YDir();
	
	if(mode.bump)
		return;
	if((mode.sub_mode!=SUBMODE_SWEEP_LEAK))		//因为在Sweep_Bump_Action()中可能会切换模式到PASS2SWEEP
		return;
	switch (mode.step)
		{
			case 0:
				mode.time=giv_sys_time;
				//if(!Is_Close_Angle(motion1.tgt_yaw,now_angle,DEGREE_10))		///////角度修正/////////
				if(!Judge_Yaw_Reach(motion1.tgt_yaw,DEGREE_10,0))
					{
						mode.step=0xE0;
						return;
					}
				mode.step++;
				speed_on=FALSE;
			break;
			case 1:
				//Speed=HIGH_MOVE_SPEED;
				Speed=FAST_MOVE_SPEED;
				//do_action(3,FARAWAY);
				do_action_my(3,FARAWAY,motion1.tgt_yaw);
				if((w_m.high_sign==NEAR)|(w_r.high_sign==NEAR)|(w_l.high_sign==NEAR)|(w_lm.high_sign==NEAR)|(w_rm.high_sign==NEAR))
					{
						r_rap.rap=HIGH_MOVE_SPEED;
						l_rap.rap=HIGH_MOVE_SPEED;
						speed_on=FALSE;
					}
				else if(giv_sys_time-mode.time>5000)
					{
						if((grid.x>=grid.x_area_max)|(grid.x<=grid.x_area_min))
							{
								l_rap.rap=FAST_MOVE_SPEED;
								r_rap.rap=FAST_MOVE_SPEED;
							}
						else if(!speed_on)
							{
								speed_on=TRUE;
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
#ifdef DEBUG_SWEEP
								TRACE("Motion in seat area!!!\r\n");
#endif
							}
					}

#if 0
				if((motion1.tgt_yaw==F_Angle_Const))
					{
						if((Read_Coordinate_Clean(now_gridx+1,now_gridy))&(now_gridx+1<=grid.x_area_max))
							{
								stop_rap();
#ifdef DEBUG_SWEEP	
								TRACE("Now Grid.x=%d y=%d\r\n",now_gridx,now_gridy);
								TRACE("coor[%d][%d] has been clean,goto next!!!\r\n",now_gridy,now_gridx+1);
#endif
								mode.step++;
							}
					}

				if(motion1.tgt_yaw==B_Angle_Const)
					{
						if((Read_Coordinate_Clean(now_gridx-1,now_gridy))&(now_gridx-1>=grid.x_area_min))
							{
								stop_rap();
#ifdef DEBUG_SWEEP	
								TRACE("Now Grid.x=%d y=%d\r\n",now_gridx,now_gridy);
								TRACE("coor[%d][%d] has been clean,goto next!!!\r\n",now_gridy,now_gridx-1);
#endif
								mode.step++;
							}
					}
#endif
				break;
			case 2:
				if(ydir>0)
					{
						if(now_gridy+1>grid.y_area_max)
							{
								Area_Check(0);
								Init_Shift_Point1(0);
								return;
							}
					}
				else if(ydir<0)
					{
						if(now_gridy-1<grid.y_area_min)
							{
								Area_Check(0);
								Init_Shift_Point1(0);
								return;
							}
					}
				if(motion1.repeat_sweep)
					{
#ifdef DEBUG_SWEEP
						TRACE("call this in %d %s\r\n",__LINE__,__func__);
#endif
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
				if(do_action(turn_dir,90*Angle_1))
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
#ifdef DEBUG_SWEEP
				TRACE("call this in %d %s\r\n",__LINE__,__func__);
#endif
				Init_Pass2Sweep();
				break;
			///////////角度修正//////////////////
			case 0xE0:
				turn_dir=Get_TurnDir(motion1.tgt_yaw);	
				Speed=MID_MOVE_SPEED;
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS,1))
					{
						stop_rap();
						mode.step=1;
					}
				break;
				
			case 0xF0:
				Area_Check(0);
				Init_Shift_Point1(0);
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
	Init_Action();
	
	mode.mode = MODE_SWEEP;			
	mode.sub_mode = SUBMODE_SWEEP_LEAKBACK;
	mode.step=0;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	
	mode.status=1;
//		WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init BACK SWEEP Mode Complete!\r\n");
#endif
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//清除接收缓存qz add 20180703
	
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
#ifdef DEBUG_SWEEP	
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgt_yaw);
#endif
	data1=Analysis_StopBack(motion1.tgt_yaw);
	if(data1==1)
		{
			{
#ifdef DEBUG_SWEEP	
				TRACE("Analysis Stop back sweep!\r\n");
				TRACE("Quit Back Sweep in %s\r\n",__func__);
#endif
				Init_Stop_BackSweep();
			}
		}
	else if(data1==2)
		{
			mode.step=2;
		}
//	delay_ms(100);
}

void Do_Leak_BackSweep(void)
{
	s8 now_gridx,now_gridy,ydir;
	static u8 turn_dir;
	short now_angle=Gyro_Data.yaw;
	static bool speed_on=FALSE;
	
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
			return;
		}
	Sweep_Bump_Action(0,1);
	if(mode.bump)
		return;
	if((mode.sub_mode!=SUBMODE_SWEEP_LEAKBACK))		//因为在Sweep_Bump_Action()中可能会切换模式到PASS2SWEEP
		return;
	switch (mode.step)
		{
			case 0:
				mode.time=giv_sys_time;
				//if(!Is_Close_Angle(motion1.tgt_yaw,now_angle,DEGREE_10))		///////角度修正/////////
				if(!Judge_Yaw_Reach(motion1.tgt_yaw,DEGREE_10,0))
					{
						mode.step=0xE0;
						return;
					}
				mode.step++;
				speed_on=FALSE;
			break;
			case 1:
				//Speed=HIGH_MOVE_SPEED;
				Speed=FAST_MOVE_SPEED;
				//do_action(3,FARAWAY);
				if(do_action_my(3,FARAWAY,motion1.tgt_yaw));
				if((w_m.high_sign==NEAR)|(w_r.high_sign==NEAR)|(w_l.high_sign==NEAR)|(w_lm.high_sign==NEAR)|(w_rm.high_sign==NEAR))
					{
						r_rap.rap=HIGH_MOVE_SPEED;
						l_rap.rap=HIGH_MOVE_SPEED;
						speed_on=FALSE;
					}
				else if(giv_sys_time-mode.time>5000)
					{
						if((grid.x>=grid.x_area_max)|(grid.x<=grid.x_area_min))
							{
								l_rap.rap=HIGH_MOVE_SPEED;
								r_rap.rap=HIGH_MOVE_SPEED;
							}
						else if(!speed_on)
							{
								speed_on=TRUE;
								r_rap.rap=TOP_MOVE_SPEED;
								l_rap.rap=TOP_MOVE_SPEED;
							}
					}

				if((motion1.tgt_yaw==F_Angle_Const))
					{
						if((Read_Coordinate_Clean(now_gridx+1,now_gridy))&(now_gridx+1<=grid.x_area_max))
							{
								stop_rap();
#ifdef DEBUG_SWEEP	
								TRACE("Now Grid.x=%d y=%d\r\n",now_gridx,now_gridy);
								TRACE("coor[%d][%d] has been clean,goto next!!!\r\n",now_gridy,now_gridx+1);
#endif
								mode.step++;
							}
					}

				if(motion1.tgt_yaw==B_Angle_Const)
					{
						if((Read_Coordinate_Clean(now_gridx-1,now_gridy))&(now_gridx-1>=grid.x_area_min))
							{
								stop_rap();
#ifdef DEBUG_SWEEP	
								TRACE("Now Grid.x=%d y=%d\r\n",now_gridx,now_gridy);
								TRACE("coor[%d][%d] has been clean,goto next!!!\r\n",now_gridy,now_gridx-1);
#endif
								mode.step++;
							}
					}
				if((motion1.tgt_yaw==B_Angle_Const)&(motion1.start_seat))//&(motion1.area_num<=2))
					{
						if(Analysis_InSeatArea(now_gridx,now_gridy))
							{
								stop_rap();
								mode.step=20;
#ifdef DEBUG_SWEEP	
								TRACE("Motion in seat area!!!\r\n");
#endif
							}
					}
				break;
			case 20:
				if(motion1.repeat_sweep)
					{
#ifdef DEBUG_SWEEP	
						TRACE("Call this in %s %d\r\n",__func__,__LINE__);
#endif
						Init_Pass2Sweep();
						motion1.repeat_sweep=false;
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
#ifdef DEBUG_SWEEP	
								TRACE("STOP BACK in %s %d\r\n",__func__,__LINE__);
#endif
								Init_Stop_BackSweep();
								return;
							}
					}
				else if(ydir<0)
					{
						if(now_gridy-1<grid.y_area_min)
							{
#ifdef DEBUG_SWEEP	
								TRACE("STOP BACK in %s %d\r\n",__func__,__LINE__);
#endif
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
				if(do_action(turn_dir,90*Angle_1))
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
			///////////角度修正//////////////////
			case 0xE0:
				turn_dir=Get_TurnDir(motion1.tgt_yaw);	
				Speed=MID_MOVE_SPEED;
				do_action(turn_dir,360*Angle_1);
				if(Judge_Yaw_Reach(motion1.tgt_yaw,TURN_ANGLE_BIOS,1))
					{
						stop_rap();
						mode.step=1;
					}
				break;
			case 0xff:
				break;
					
		}
}


void No_Thingleak(void)
{

}

