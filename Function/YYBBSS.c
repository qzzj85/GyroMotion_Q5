
//=====================头文件====================================
#include "AAA-Include.h"

//=====================私有定义====================================
#define	REPEAT_ANGLE_BIOS	500		//5度
//=====================全局变量====================================

//=====================全局函数====================================

//=====================私有变量====================================


					
//=====================私有函数====================================
u8 YBS_DISTANCE=YBS_DISTANCE_CONST;
//===============================================================================================================
//===============================================================================================================
 
u16 YBS_Wall_Distance;
////////////////////////????//////////////////////////////////
bool  YBS_Check_Flag;

///////////////////////????////////////////////////////////////
static int16_t  err_Distance[3];       //?????????


uint8_t Wall_lost_flag;
int Wall_lost_Start_Pos;
u32 	lost_turn_time=0;					//qz add 20180902 机器右边无障碍开始向右转弯的时间

uint8_t Wall_lost_counter;
//static 		int16_t corner[16];
u32 		YBS_Straight_Time;	
bool 	YBS_rap_time=false;
bool 	YBS2leftright=0;	//0变换为左沿边，1变换为右沿边
bool    YBS_check_base=false;
uint8_t last_Bump_Num;
uint8_t Same_Bump_Counter;

//===============================================================================================
//===============================================================================================
/*****************************************************************
功能：定点清扫程序
*****************************************************************/


void YBS_YBS(void)
{
//	static u8 piv_out;	//机器是否向外展开，1为向外展开，0为向里缩小
//	static u8 piv_left; //机器是否向左转，1为向左转，0为向右转
	u8 temp_data1=0;
	u8 abnormal;
	u32 uin32;
	static short tgt_yaw=0;

#if 1		
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
#endif


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

	if(giv_sys_time-mode.time>25*10000)
		{
			if((grid.x==grid.x_ybs_start)&(grid.y==grid.y_ybs_start))
				{
					Send_Voice(VOICE_SWEEP_DONE);
					Init_Docking();
				}
		}

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
	
	if((mode.sub_mode==YBS_SUB_RIGHT))				//	RIGHT
		{
			YBS_Right_Bump(0);
		}		
	else if((mode.sub_mode == YBS_SUB_LEFT))		//	LEFT
		{
			YBS_Left_Bump(0);
		}
			
	if(mode.bump != 0)		//	有碰撞需要处理，返回d
		{
			//Wall_lost_counter = 0;
			if((mode.step==0x90)|(mode.step==0x91))
				{
					grid.x_ybs_start=grid.x;
					grid.y_ybs_start=grid.y;
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
				Speed=HIGH_MOVE_SPEED;//2000
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
//					Speed = 1200;
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
								
			//	扩大搜索范围//	扩大搜索范围//	扩大搜索范围//	扩大搜索范围//	扩大搜索范围
			case 0xA0:		
				if((l_rap.sign == 0)&&(r_rap.sign == 0))
					{
						Speed = 1200;
						enable_rap(FRONT,(uint32_t)(1.5 * METER_PLUS) , FRONT,(uint32_t)(1 * METER_PLUS));
						
						mode.step = 0xA1;
					}
				break;					
			case  0xA1:
				if((l_rap.sign == 0)&&(r_rap.sign == 0))
					{
							mode.step = 0xA5;		//	可能受困
					}
				break;							
			case  0xA5://	可能受困
				Speed = 2600;
				if(do_action(3, (uint32_t)(0.5 * METER_PLUS)) == 1) 	
					{mode.step = 0xA6;} 
				break;
			case  0xA6://	可能受困
				Speed = 2600;
				if(do_action(4, (uint32_t)(0.5 * METER_PLUS)) == 1) 	
					{mode.step = 0xA7;} 
				break;
			case  0xA7://	可能受困
				Speed = 2200;
				if(do_action(2, (uint32_t)(180 * Angle_1))) 	
					{mode.step = 0x88;} 
				break;
			//	扩大搜索范围//	扩大搜索范围//	扩大搜索范围//	扩大搜索范围//	扩大搜索范围
			//qz add
			case 0xB0:
				Speed=2400;
				if(do_action(4,(u32)(10*CM_PLUS)))
					{
						stop_rap();
						mode.step=0xB1;
					}
				break;
			case 0xB1:
				Speed=1600;
				if(do_action(1,angle90))
					{
						stop_rap();
						mode.step=0xB2;
					}
				break;
			case 0xB2:
				if(do_action(3,(u32)(15*CM_PLUS)))
					{
						stop_rap();
						mode.step=0XB3;
					}
				break;
			case 0xB3:
				if(do_action(6,angle90_D))
					{
						stop_rap();
						mode.step=0x00;
					}
				break;

			//y坐标回溯超过20cm时，开启左沿边
			case 0x90:
				tgt_yaw=Gyro_Data.yaw;
				mode.step++;
				break;
			case 0x91:
				Speed=FAST_MOVE_SPEED;
				do_action_my(3,FARAWAY*CM_PLUS,tgt_yaw);
				break;
		}	//	end of		switch (mode.step)	//step路径执行的步骤
}
//===============================================================================================
//===============================================================================================
//===============================================================================================
//===============================================================================================
/******************************************
读取碰撞信息
*******************************************/
u8 Check_Xmax_OutYBS(void)
{
	//if(mode.mode==EXIT)
		{
			if((grid.x>grid.x_area_max)|(Gyro_Data.x_pos>motion1.xpos_start+RANGE_MAX))
				return 1;
			else
				return 0;
		}
#if 0	
	if((Gyro_Data.x_pos>motion1.xpos_max_area)|(Gyro_Data.x_pos>motion1.xpos_start+RANGE_MAX))
		return 1;
	else
		return 0;
#endif
}

u8 Check_Xmin_OutYBS(void)
{
	//if(mode.mode==EXIT)
		{
			if((grid.x<grid.x_area_min)|(Gyro_Data.x_pos<motion1.xpos_start-RANGE_MAX))
				return 1;
			else
				return 0;
		}
#if 0	
	if((Gyro_Data.x_pos<motion1.xpos_min_area)|(Gyro_Data.x_pos<motion1.xpos_start-RANGE_MAX))
		return 1;
	else
		return 0;
#endif
}

u8 Check_Ymax_OutYBS(void)
{
	//if(mode.mode==EXIT)
		{
			if((grid.y>grid.y_area_max)|(Gyro_Data.y_pos>motion1.ypos_start+RANGE_MAX))
				return 1;
			else
				return 0;
		}
#if 0
	if((Gyro_Data.y_pos>motion1.ypos_max_area)|(Gyro_Data.y_pos>motion1.ypos_start+RANGE_MAX))
		return 1;
	else
		return 0;
#endif
}

u8 Check_Ymin_OutYBS(void)
{
	//if(mode.mode==EXIT)

		{
			if((grid.y<grid.y_area_min)|(Gyro_Data.y_pos<motion1.ypos_start-RANGE_MAX))
				return 1;
			else
				return 0;
		}
#if 0
	if((Gyro_Data.y_pos<motion1.ypos_min_area)|(Gyro_Data.y_pos<motion1.ypos_start-RANGE_MAX))
		return 1;
	else
		return 0;
#endif
}

u8 Check_OutRange_Clean_YBS(u8 data)
{
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	u8 result=0;
	switch (data)
		{
			case 8:					//Xmax Out
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
			case 4:					//Xmin Out
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
			case 2:					//Ymax Out
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
			case 1:					//Ymin Out
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


u32 Check_OutofRange_YBS(u8 out_enable)
{
	u8 xmax_out=0,xmin_out=0,ymax_out=0,ymin_out=0,data=0;
	u32 result=0;

	if(!out_enable)
		return 0;
#if 1
	if(((mode.mode==SHIFT)|(mode.mode==EXIT))&(mode.bump==0))
		{
			if((mode.step==0x10)|(mode.step==0x11)|(mode.step==0x50)|(mode.step==0x51))
				{
					//TRACE("outgrid!!\r\n");
					return 0;
				}
			if((mode.step==0x4)|(mode.step==0x44))
				{
					//TRACE("outgrid!!\r\n");
					return 0;
				}
		}
#endif	
	xmax_out=Check_Xmax_OutYBS();
	xmin_out=Check_Xmin_OutYBS();
	ymax_out=Check_Ymax_OutYBS();
	ymin_out=Check_Ymin_OutYBS();
	data=(xmax_out<<3)+(xmin_out<<2)+(ymax_out<<1)+ymin_out;

	switch(data)
		{
			case 10:
				result=BUMP_RF_OUT;
				break;
			case 9:
				result=BUMP_LF_OUT;
				break;
			case 6:
				result=BUMP_RB_OUT;
				break;
			case 5:
				result=BUMP_LB_OUT;
				break;
			case 8:
				result=BUMP_XMAX_OUT;
				break;
			case 4:
				result=BUMP_XMIN_OUT;
				break;
			case 2:
				result=BUMP_YMAX_OUT;
				break;
			case 1:
				result=BUMP_YMIN_OUT;
				break;
			default:
				result=0;
				break;
		}
#if 0
	if((data)&(Read_CheckPoint_NextAction()==CHECK_NEWAREA))	//(check_point.next_area))
		{
			if(Check_OutRange_Clean_YBS(data))
				{
					TRACE("Prepare to next area sweep in ShiftYBS!!!\r\n");
					TRACE("check_point.new_x1=%d\r\n",check_point.new_x1);
					TRACE("check_point.new_y1=%d\r\n",check_point.new_y1);
					TRACE("check_point.new_x2=%d\r\n",check_point.new_x2);
					TRACE("check_point.new_y2=%d\r\n",check_point.new_y2);
					TRACE("Next tgt_yaw=%d\r\n",check_point.next_tgtyaw);
					TRACE("Next y_acc=%d\r\n",check_point.next_yacc);
					TRACE("Next x_acc=%d\r\n",check_point.next_xacc);
					Init_Pass2Init(check_point.next_tgtyaw,check_point.next_yacc,check_point.next_xacc);
		//			check_point.next_area=false;
					Reset_CheckPoint_NextAction();
					return 0;
				}
		}
#endif
	return result;
}

u32  YBS_read_bump1(u8 out_enable)
{
	u32 data1=0;
//	static u32 find_seat_time=0;
	static bool find_seat_first=false;
//-------------------------------------------------------------	 
//-------------------------------------------------------------	 
#ifdef CLIFF_ENABLE
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
		case BUMP_ONLY_LEFT:						//左碰撞
			if((!mode.bump_flag)&(mode.bump==0))
				{
					stop_rap();
					mode.bump=BUMP_ONLY_LEFT;
					mode.bump_flag=true;
					mode.step_bp=0;
					Slam_Data.l_bump_flag=true;
				}
			return BUMP_ONLY_LEFT;
		case BUMP_ONLY_LEFTMID:						//左中碰撞
			if((!mode.bump_flag)&(mode.bump==0))
				{
					stop_rap();
					mode.bump=BUMP_ONLY_LEFTMID;
					mode.bump_flag=true;
					mode.step_bp=0;
					Slam_Data.l_bump_flag=true;
				}
			return BUMP_ONLY_LEFTMID;
		case BUMP_LEFT_MID:							//左左中碰撞
			if((!mode.bump_flag)&(mode.bump==0))
				{							
					stop_rap();
					mode.bump=BUMP_LEFT_MID;
					mode.bump_flag=true;
					mode.step_bp=0;
					Slam_Data.l_bump_flag=true;
				}
			return BUMP_LEFT_MID;
		case BUMP_ONLY_RIGHT:						//右碰撞
			if((!mode.bump_flag)&(mode.bump==0))
				{							
					stop_rap();
					mode.bump=BUMP_ONLY_RIGHT;
					mode.bump_flag=true;
					mode.step_bp=0;
					Slam_Data.r_bump_flag=true;
				}
			return BUMP_ONLY_RIGHT;
		case BUMP_ONLY_RIGHTMID:					//右中碰撞
			if((!mode.bump_flag)&(mode.bump==0))
				{							
					stop_rap();
					mode.bump=BUMP_ONLY_RIGHTMID;
					mode.bump_flag=true;
					mode.step_bp=0;
					Slam_Data.r_bump_flag=true;
				}
			return BUMP_ONLY_RIGHTMID;
		case BUMP_RIGHT_MID:						//右右中碰撞
			if((!mode.bump_flag)&(mode.bump==0))
				{							
					stop_rap();
					mode.bump=BUMP_RIGHT_MID;
					mode.bump_flag=true;
					mode.step_bp=0;
					Slam_Data.r_bump_flag=true;
				}
			return BUMP_RIGHT_MID;
		case BUMP_MID:								//中碰撞
			if((!mode.bump_flag)&(mode.bump==0))
				{							
					stop_rap();
					mode.bump=BUMP_MID;
					mode.bump_flag=true;
					mode.step_bp=0;
					Slam_Data.m_bump_flag=true;
				}
			return BUMP_MID;
	}


//zdkshield
#if 1	//QZ:原来位0

	if((w_lm.sign == NEAR)&(mode.step<0x90))
		{
			if((mode.sub_mode==YBS_SUB_LEFT))			//如果是左沿边,开启此检测
				{
					if((mode.bump == 0))		 //左中墙检靠近墙
						{
							mode.bump=8;//W_LM;
							mode.step_bp=0;
							stop_rap();
							if(last_Bump_Num == 8)
								{
									Same_Bump_Counter ++;
								}
							else
								{
									Same_Bump_Counter = 0;
								}
							last_Bump_Num = 8;
						}
					return 8;//W_LM;
				}
		}

#endif

	if((w_m.sign == NEAR)&(mode.step<0x90))
		{
			if((mode.bump == 0)|(mode.bump==BUMP_SEAT))	   //中墙检靠近墙
				{
					mode.bump=9;//W_M;
					mode.step_bp=0;
					stop_rap();
					if(last_Bump_Num == 9)
					{
						Same_Bump_Counter ++;
					}
					else
					{
						Same_Bump_Counter = 0;
					}
					last_Bump_Num = 9;
				}
				return 9;//W_M;
		}

//zdkshield
#if 1	//QZ:原来为1

	if((w_rm.sign == NEAR)&(mode.step<0x90))
		{
			if((mode.sub_mode==YBS_SUB_RIGHT))			//如果是右沿边,开启此检测
				{
					if((mode.bump == 0)|(mode.bump==BUMP_SEAT))		//右中墙检靠近墙
						{
							mode.bump=15;//W_RM;
							mode.step_bp=0;
							stop_rap();
							if(last_Bump_Num == 15)
								{
									Same_Bump_Counter ++;
								}
							else
								{
									Same_Bump_Counter = 0;
								}
							last_Bump_Num = 15;
						}
					return 15;//W_RM;
				}
	 }

#endif


#ifdef W_MM_ENABLE
		if(w_mm.sign==NEAR)
			{
				if(mode.bump==0)
					{
						mode.bump=99;
						mode.step_bp=0;
						stop_rap();
					}
				return 99;
			}
#endif

		last_Bump_Num = 0;

		//if(r_hw.effectNear|r_hw.effectTopReal|rm_hw.effectTopReal)
		//if(r_hw.effectNear|r_hw.effectTopReal)
		//if((lm_hw.effectNear)|(rm_hw.effectNear)|r_hw.effectNear)
#if 0
	if((r_hw.effectTop)&((mode.step==1)|(mode.step==2)|(mode.step==0x41)|(mode.step==0x42))&(mode.bump==0))
		{
#ifdef YBS_AVOID_SEAT
			if(mode.bump==0)
				{
					if(SLAM_DOCK)
						disable_hwincept();
					stop_rap();
#ifdef YBS_DEBUG
					TRACE("r_hw get NEAR in YBS mode!\r\n");
#endif
					mode.bump=BUMP_SEAT;
					mode.step_bp=0;
				}
			return BUMP_SEAT;
#endif
#ifdef YBS_SEAT_REPROT
			Slam_Data.ir_flag=true;
#endif
		}
#endif
	//if((r_hw.effectTop)&((mode.step==1)|(mode.step==2)|(mode.step==0x41)|(mode.step==0x42))&(mode.bump==0))
	//if((rm_hw.effectTop|lm_hw.effectTop)&&(mode.bump==0))
	if(top_time_sec>=9)
		{
			if(!find_seat_first)
				{
					//find_seat_time=giv_sys_time;
					find_seat_first=true;
				}
			//if((find_seat_first)&(giv_sys_time-find_seat_time>10000))
				{
					if((mode.bump==0)|(mode.bump>=BUMP_OUTRANGE))
						{
							stop_rap();
							mode.bump=BUMP_SEAT;
							mode.step_bp=0;
							find_seat_first=false;
						}
				}
			return BUMP_SEAT;
		}
	else
		{
			find_seat_first=false;
		}

	data1=Check_OutofRange_YBS(out_enable);
	if((data1!=0)&(mode.bump<BUMP_OUTRANGE))
		{
			stop_rap();
			mode.bump=data1;
			mode.step_bp=0;
		}
	return data1;
}
//===============================================================================================
//===============================================================================================





uint16_t  Return_Distance(uint16_t data)
{
static uint16_t 	Distance;

#if 0
	if(data < 0x30)
		{Distance = 200;}
		
	else if(data < 0x50)	//QZ:30<=DATA<40
		{
			Distance = 200 - (uint16_t)( ( (float)((data-0x30) * (200-150) / (0x50-0x30)) ) );	//QZ: 150<DISTANCE<=200
		}
	else if(data < 0x70)	//QZ:40<=DATA<63
		{
			Distance = 150 - (uint16_t)( ( (float)((data-0x50) * (150-120) / (0x70-0x50) )) 		);	//QZ: 120<DISTANCE<=150;
		}
	else if(data < 0xA0)	//QZ:63<=DATA<98
		{
			Distance = 120 - (uint16_t)( ( (float)((data-0x70) * (120-100) / (0xA0-0x70) )) 		);	//QZ:  100<DISTANCE<=120
		}
	else if(data < 0xC0)	//QZ:98<=DATA<167
		{
			Distance = 100 - (uint16_t)( ( (float)((data-0xA0) * (100-90) / (0xC0-0xA0) )) 		);	//QZ:  90<DISTANCE<=100
		}
	else if(data < 0xF0)	//QZ:167<=DATA<220
		{
			Distance = 90 - (uint16_t)( ( (float)((data-0xC0) * (90-80) / (0xF0-0xC0) )) 	);		//QZ:  80<DISTANCE<=90
		}
	else if(data < 0x140)	//QZ:220<=DATA<361
		{
			Distance = 80 - (uint16_t)( ( (float)((data-0xF0) * (80-70) / (0x140-0xF0) )) 	);		//QZ:  70<DISTANCE<=80
		}
	else if(data < 0x1A0)	//QZ:361<=DATA<470 //190
		{
			Distance = 70 - (uint16_t)( ( (float)((data-0x140) * (70-60) / (0x1A0-0x140) ))	);		//QZ:  60<DISTANCE<=70
		}
	else if(data < 0x240)	//QZ:470<=DATA<830	//220
		{
			Distance = 60 - (uint16_t)( ( (float)((data-0x1A0) * (60-50) / (0x240-0x1A0) )	) );	//QZ:  50<DISTANCE<=60
		}
	else if(data < 0x360)//QZ:830<=DATA<2050	//2e0
		{
			Distance = 50 -  (uint16_t)( ( (float)((data-0x240) * (50-40)/ (0x360-0x240) ) )		);	//QZ:40<DISTANCE<=50
		}
	else if(data < 0x4C0)//QZ:830<=DATA<2050
		{
			Distance = 40 -  (uint16_t)( ( (float)((data-0x360) * (40-30)/ (0x4C0-0x360) ) )		);	//QZ:30<DISTANCE<=40
		}
	else if(data < 0x6E0)//QZ:830<=DATA<2050
		{
			Distance = 30 -  (uint16_t)( ( (float)((data-0x4C0) * (30-20)/ (0x6E0-0x4C0) ) )		);	//QZ:20<DISTANCE<=30
		}

	
	else		//QZ:DATA>=2025
		{
			Distance = 20; 
		}

	if(Distance-20<0)
		Distance=0;
	else
		Distance-=20;
	return(Distance);
#endif

#if 1
if(data < VALUE200)
	{Distance = 200;}
else if(data<VALUE190)
	{
		Distance = 200 - (uint16_t)( ( (float)((data-VALUE200) * (200-190) / (VALUE190-VALUE200)) ) );	//QZ: 150<DISTANCE<=200
	}
else if(data<VALUE180)
	{
		Distance = 190 - (uint16_t)( ( (float)((data-VALUE190) * (190-180) / (VALUE180-VALUE190)) ) );	//QZ: 150<DISTANCE<=200
	}
else if(data<VALUE170)
	{
		Distance = 180 - (uint16_t)( ( (float)((data-VALUE180) * (180-170) / (VALUE170-VALUE180)) ) );	//QZ: 150<DISTANCE<=200
	}
else if(data<VALUE160)
	{
		Distance = 170 - (uint16_t)( ( (float)((data-VALUE170) * (170-160) / (VALUE160-VALUE170)) ) );	//QZ: 150<DISTANCE<=200
	}
else if(data < VALUE150)	//QZ:30<=DATA<40
	{
		Distance = 160 - (uint16_t)( ( (float)((data-VALUE160) * (160-150) / (VALUE150-VALUE160)) ) );	//QZ: 150<DISTANCE<=200
	}
else if(data<VALUE140)
	{
		Distance = 150 - (uint16_t)( ( (float)((data-VALUE150) * (150-140) / (VALUE140-VALUE150)) ) );	//QZ: 150<DISTANCE<=200
	}
else if(data<VALUE130)
	{
		Distance = 140 - (uint16_t)( ( (float)((data-VALUE140) * (140-130) / (VALUE130-VALUE140)) ) );	//QZ: 150<DISTANCE<=200
	}
else if(data < VALUE120)	//QZ:40<=DATA<63
	{
		Distance = 130 - (uint16_t)( ( (float)((data-VALUE130) * (130-120) / (VALUE120-VALUE130) )) 		);	//QZ: 120<DISTANCE<=150;
	}
else if(data<VALUE110)
	{
		Distance = 120 - (uint16_t)( ( (float)((data-VALUE120) * (120-110) / (VALUE110-VALUE120)) ) );	//QZ: 150<DISTANCE<=200
	}
else if(data < VALUE100)	//QZ:63<=DATA<98
	{
		Distance = 110 - (uint16_t)( ( (float)((data-VALUE110) * (110-100) / (VALUE100-VALUE110) )) 		);	//QZ:  100<DISTANCE<=120
	}
else if(data < VALUE90)	//QZ:98<=DATA<167
	{
		Distance = 100 - (uint16_t)( ( (float)((data-VALUE100) * (100-90) / (VALUE90-VALUE100) ))		);	//QZ:  90<DISTANCE<=100
	}
else if(data < VALUE80)	//QZ:167<=DATA<220
	{
		Distance = 90 - (uint16_t)( ( (float)((data-VALUE90) * (90-80) / (VALUE80-VALUE90) ))	);		//QZ:  80<DISTANCE<=90
	}
else if(data < VALUE70)	//QZ:220<=DATA<361
	{
		Distance = 80 - (uint16_t)( ( (float)((data-VALUE80) * (80-70) / (VALUE70-VALUE80) ))	);		//QZ:  70<DISTANCE<=80
	}
else if(data < VALUE60)	//QZ:361<=DATA<470 //190
	{
		Distance = 70 - (uint16_t)( ( (float)((data-VALUE70) * (70-60) / (VALUE60-VALUE70) )) );		//QZ:  60<DISTANCE<=70
	}
else if(data < VALUE50)	//QZ:470<=DATA<830	//220
	{
		Distance = 60 - (uint16_t)( ( (float)((data-VALUE60) * (60-50) / (VALUE50-VALUE60) )	) );	//QZ:  50<DISTANCE<=60
	}
else if(data < VALUE40)//QZ:830<=DATA<2050	//2e0
	{
		Distance = 50 -  (uint16_t)( ( (float)((data-VALUE50) * (50-40)/ (VALUE40-VALUE50) ) )		);	//QZ:40<DISTANCE<=50
	}
else if(data < VALUE30)//QZ:830<=DATA<2050
	{
		Distance = 40 -  (uint16_t)( ( (float)((data-VALUE40) * (40-30)/ (VALUE30-VALUE40) ) )		);	//QZ:30<DISTANCE<=40
	}
else if(data < VALUE20)//QZ:830<=DATA<2050
	{
		Distance = 30 -  (uint16_t)( ( (float)((data-VALUE30) * (30-20)/ (VALUE20-VALUE30) ) )		);	//QZ:20<DISTANCE<=30
	}
else if(data < VALUE10)//QZ:830<=DATA<2050
	{
		Distance = 20 -  (uint16_t)( ( (float)((data-VALUE20) * (20-10)/ (VALUE10-VALUE20) ) )		);	//QZ:20<DISTANCE<=30
	}

else if(data < VALUE05)//QZ:830<=DATA<2050
	{
		Distance = 20 -  (uint16_t)( ( (float)((data-VALUE10) * (10-5)/ (VALUE05-VALUE10) ) )		);	//QZ:20<DISTANCE<=30
	}
else
	Distance=0;
return Distance;
#endif
	
}


void  YBS_Check_corner(void)
{
	//YBS_Wall_Distance=Return_Distance(w_r.difference);
	YBS_Wall_Distance=w_r.dis;
}
//===============================================================================================
//===============================================================================================








//===============================================================================================
void  YBS_Distance_Check(uint8_t Left_Right)
{
static int16_t Error;
static int32_t xxxx;
static int32_t xxxx_2;


	//------------------------------------------
	if(YBS_Check_Flag == false)		
//		{return;}
	YBS_Check_Flag = false;
	//------------------------------------------

		
//			Error = w_r.difference - YBS_DISTANCE;
//			Error = corner_check - 250;
			Error = YBS_Wall_Distance - YBS_DISTANCE;
			xxxx_2 = (err_Distance[0]-err_Distance[1])*YBS_KP		+	err_Distance[0]*YBS_KI	+ (err_Distance[0]+err_Distance[2]-2*err_Distance[1])*YBS_KD;
//			xxxx_2 = (err_Distance[0]-err_Distance[1])/KP		+	err_Distance[0]/KI	+ (err_Distance[0]+err_Distance[2]-2*err_Distance[1])/KD;
		
			xxxx = xxxx_2 / 100;
			err_Distance[2] = err_Distance[1];
			err_Distance[1] = err_Distance[0];
			err_Distance[0] = Error;
							
					if(xxxx > 100)			//100
							xxxx = 100;
					
					if(xxxx < -100)
							xxxx = -100;
					l_rap.rap += xxxx;
					
#ifdef YBS_Straight_FAST
					if(mode.step==0x02)
						{
							if(giv_sys_time-YBS_Straight_Time>25000)
								{
									if(l_rap.rap>YBS_MAIN_HIGHSPD_H)
										l_rap.rap=YBS_MAIN_HIGHSPD_H;
									if(l_rap.rap<YBS_MAIN_HIGHSPD_L)
										l_rap.rap=YBS_MAIN_HIGHSPD_L;									
								}
							else
								{
									if(l_rap.rap > YBS_MAIN_LOWSPEED_H)
										{l_rap.rap = YBS_MAIN_LOWSPEED_H;} 	
									if(l_rap.rap < YBS_MAIN_LOWSPEED_L)
										{l_rap.rap = YBS_MAIN_LOWSPEED_L;}
								}
						}
					else
#endif
						{
							if(l_rap.rap > YBS_MAIN_LOWSPEED_H)
								{l_rap.rap = YBS_MAIN_LOWSPEED_H;} 	
							if(l_rap.rap < YBS_MAIN_LOWSPEED_L)
								{l_rap.rap = YBS_MAIN_LOWSPEED_L;}
						}
					
					if(xxxx > 80)			//40
							xxxx = 80;
					
					if(xxxx < -80)
							xxxx = -80;
					r_rap.rap -= xxxx;


					//qz add
//					if(w_m.high_sign!=NEAR)
					//qz add end
#ifdef YBS_Straight_FAST
					if(mode.step==0x02)
						{
							if(giv_sys_time-YBS_Straight_Time>25000)
								{
									if(r_rap.rap>YBS_SLAVE_HIGHSPD_H)
										r_rap.rap=YBS_SLAVE_HIGHSPD_H;
									if(r_rap.rap<YBS_SLAVE_HIGHSPD_L)
										r_rap.rap=YBS_SLAVE_HIGHSPD_L;
								}
							else
								{
									if(r_rap.rap > YBS_SLAVE_LOWSPEED_H)
										{r_rap.rap = YBS_SLAVE_LOWSPEED_H;} 									
									if(r_rap.rap < YBS_SLAVE_LOWSPEED_L)
										{r_rap.rap = YBS_SLAVE_LOWSPEED_L;}
								}
						}
					else
#endif
						{
							if(r_rap.rap > YBS_SLAVE_LOWSPEED_H)
								{r_rap.rap = YBS_SLAVE_LOWSPEED_H;}		
							
							if(r_rap.rap < YBS_SLAVE_LOWSPEED_L)
								{r_rap.rap = YBS_SLAVE_LOWSPEED_L;}
						}

}



void YBS_Comm_Rap_My(void)
{
// 	u32 l;
	float f_ec;
	if(rap_time==false)
		return;
	rap_time=false;

	Check_Speed_My(&(l_ring.real_speed), &(r_ring.real_speed));
	/////////////////////////////////右轮/////////////////////////// 

#ifdef PITCH_SPEEDUP
	if((mode.speed_up)&(mode.abnormity==0))
		{
			r_rap.rap=12*r_rap.rap/10;
			l_rap.rap=12*l_rap.rap/10;
		}
#endif

	if(r_rap.sign)
		{
			r_ring.f_ek[2]=r_ring.f_ek[1];
			r_ring.f_ek[1]=r_ring.f_ek[0];
			r_ring.f_ek[0]=(float)(r_rap.rap)-(float)(r_ring.real_speed);
			f_ec = ((r_ring.f_ek[0]-r_ring.f_ek[1])*YBS_FKP + r_ring.f_ek[0]*YBS_FKI +(r_ring.f_ek[0]+r_ring.f_ek[2]-2*r_ring.f_ek[1])*YBS_FKD);	
			r_rap.pwm=(int)(r_rap.pwm+f_ec);
			if(r_rap.pwm>PWM_RING_MAX)
				r_rap.pwm=PWM_RING_MAX;
			else if(r_rap.pwm<=0)
				r_rap.pwm=0;
					
					
		}

/////////////左轮脉冲所走时间////////////////////
	if(l_rap.sign)
		{
			l_ring.f_ek[2]=l_ring.f_ek[1];
			l_ring.f_ek[1]=l_ring.f_ek[0];
			l_ring.f_ek[0]=(float)(l_rap.rap)-(float)(l_ring.real_speed);
			f_ec = ((l_ring.f_ek[0]-l_ring.f_ek[1])*F_KP + l_ring.f_ek[0]*F_KI +(l_ring.f_ek[0]+l_ring.f_ek[2]-2*l_ring.f_ek[1])*F_KD);
			l_rap.pwm=(int)(l_rap.pwm+f_ec);
			if(l_rap.pwm>PWM_RING_MAX)
				l_rap.pwm=PWM_RING_MAX;
			else if(l_rap.pwm<=0)
				l_rap.pwm=0;
		}

	if(r_rap.sign)
		{
			if(r_rap.ori == FRONT)
					{enable_pwm(R_FRONT,r_rap.pwm);}
			else
					{enable_pwm(R_BACK,r_rap.pwm);}
			if(r_rap.length  <=  (r_ring.length+10) )
				{
						r_rap.sign		= 0;
						r_rap.pwm			= 0;
						r_rap.rap		= 0;
						disable_pwm(R_FRONT);
						disable_pwm(R_BACK);
				}
		}

	if(l_rap.sign)
		{
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
				}
		}
}

/************************************************************
功能  ：    每10ms 执行一次，自动根据车轮信息控制车轮
输入： 无
输出 ：无
************************************************************/




//===============================================================================================
//===============================================================================================
void  YBS_Left_Check_corner(void)
{
#if 0
int8_t i;
int32_t 	hehe;
int16_t 			ave;
	

	//------------------------------------------
//	if(YBS_Conor_Check_Flag == false)		
//		{return;}
//	YBS_Conor_Check_Flag = false;
	//------------------------------------------
	hehe = 0;
	for(i=0;i<15;i++)
		{
			corner[i] = corner[i+1];
			hehe +=	(int32_t)(corner[i]);
		}
		
	corner[15] =  w_l.difference - 8;
	if(corner[15] < 0)
		{corner[15] = 0;}
		
	ave = (int16_t)(hehe / 15);
	
	YBS_Wall_Distance = Return_Distance(ave);
#endif
	YBS_Wall_Distance=w_l.dis;
}
//===============================================================================================
//===============================================================================================








//===============================================================================================
void  YBS_Left_Distance_Check(void)
{
static int16_t Error;
static int32_t xxxx;
static int32_t xxxx_2;
	//------------------------------------------
	if(YBS_Check_Flag == false)		
		{return;}
	YBS_Check_Flag = false;
	//------------------------------------------

		
		
//			Error = w_r.difference - YBS_DISTANCE;
//			Error = corner_check - 250;
			Error = YBS_Wall_Distance - YBS_DISTANCE;
		
			err_Distance[2] = err_Distance[1];
			err_Distance[1] = err_Distance[0];
			err_Distance[0] = Error;
	
				
			
					xxxx_2 = (err_Distance[0]-err_Distance[1])*YBS_KP		+ 	err_Distance[0]*YBS_KI	+ (err_Distance[0]+err_Distance[2]-2*err_Distance[1])*YBS_KD;
					
					xxxx = xxxx_2 / 100;
	
							
							
							
							
							
					if(xxxx > 100)
							xxxx = 100;
					
					if(xxxx < -100)
							xxxx = -100;

					r_rap.rap += xxxx;
					
					if(r_rap.rap > YBS_MAIN_LOWSPEED_H)
							{r_rap.rap = YBS_MAIN_LOWSPEED_H;}		
	
					if(r_rap.rap < YBS_MAIN_LOWSPEED_L)
							{r_rap.rap = YBS_MAIN_LOWSPEED_L;}
				
											
					if(xxxx > 80)
							xxxx = 80;
					
					if(xxxx < -80)
							xxxx = -80;
					
					l_rap.rap -= xxxx;
					if(l_rap.rap > YBS_SLAVE_LOWSPEED_H)
							{l_rap.rap = YBS_SLAVE_LOWSPEED_H;}		
							
					if(l_rap.rap < YBS_SLAVE_LOWSPEED_L)
							{l_rap.rap = YBS_SLAVE_LOWSPEED_L;}
							
			
					
}
//================================================================================
//================================================================================



/*****************************************************************
功能：初始化定点清扫的程序
*****************************************************************/
void Init_Right_YBS(u8 pre_action,bool start_seat)
{

	if(mode.low_power)
		{
			Send_Voice(VOICE_POWER_LOW);
			Init_Cease();
			return;
		}

	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
    /******初始化显示***********/
	
	clr_ram();
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Enable_earth();						//打开地检
	Enable_wall();						//打开墙检
	enable_hwincept();					//允许红外接收电源
	Enable_Speed();					//打开速度检测
	Init_Action();
	//	ReInitAd();
	//clr_all_hw_struct();				//清零回充信号标志	//qz modify 20181210 effect-->struct	//qz mask 20181215
	WriteWorkState();					//功能：保存工作状态
	

	mode.mode = YBS;			
	mode.sub_mode = YBS_SUB_RIGHT;		
	mode.step_bp = 0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.time=giv_sys_time;
	mode.status=1;
	if(pre_action)
		{
			mode.step = 0x90;//0x88;//QZ:原来为0x88;
		}
	else
		{
			mode.step = 0;
		}

	if(start_seat)
		{
			motion1.start_seat=true;
		}
	else
		{
			motion1.start_seat=false;
		}
	
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();			//打开万向轮检测	
#endif
#ifdef ROTATE_SKID_CHECK	
	//Enable_Rotate_Skid_Check(0);
#endif

	while(giv_sys_time-mode.time<1000);
	motion1.xpos_ybs_start=Gyro_Data.x_pos;
	motion1.ypos_ybs_start=Gyro_Data.y_pos;
	grid.x_ybs_start=grid.x;
	grid.y_ybs_start=grid.y;
#ifdef DEBUG_Enter_Mode
	TRACE("Init Right YBS Mode Complete!\r\n");
#endif

	Sweep_Level_Set(sweep_level);
	YBS_DISTANCE=YBS_DISTANCE_CONST;

	TRACE("motion1.ybs_start_xpos=%d ypos=%d\r\n",motion1.xpos_ybs_start,motion1.ypos_ybs_start);
	motion1.continue_checkstep=0;			//qz add 20190328
	motion1.worktime=1;	
	motion1.clean_size=0;
	Gyro_Data.cal_flag=false;
	YBS_check_base=false;	
	Init_Check_Status();	
	Send_Voice(VOICE_SWEEP_START);
}

void Init_Left_YBS(u8 direct_first)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
	
	clr_ram();
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Enable_earth(); 					//打开地检
	Enable_wall();						//打开墙检
	enable_hwincept();					//允许红外接收电源
	Enable_Speed(); 				//打开速度检测
	Init_Action();
	//	ReInitAd();
	clr_all_hw_effect();				//清零回充信号标志
	WriteWorkState();					//功能：保存工作状态
	

	mode.mode = YBS;			
	mode.sub_mode = YBS_SUB_LEFT;		
	mode.step_bp = 0;
	mode.bump = 0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.time=giv_sys_time;

	mode.status=1;
	if(direct_first)
		mode.step = 0x88;//QZ:原来为0x88;
	else
		mode.step=0x40;
	Enable_Free_Skid_Check();			//打开万向轮检测 

#ifdef ROTATE_SKID_CHECK	
	//Enable_Rotate_Skid_Check(0);
#endif

	while(giv_sys_time-mode.time<1000);
	motion1.xpos_ybs_start=Gyro_Data.x_pos;
	motion1.ypos_ybs_start=Gyro_Data.y_pos;
	grid.x_ybs_start=grid.x;
	grid.y_ybs_start=grid.y;
#ifdef DEBUG_Enter_Mode
	TRACE("Init Left YBS Mode Complete!\r\n");
#endif
	TRACE("motion1.ybs_start_xpos=%d ypos=%d\r\n",motion1.xpos_ybs_start,motion1.ypos_ybs_start);
	Sweep_Level_Set(sweep_level);
	motion1.continue_checkstep=0;		//qz add 20190328
	YBS_check_base=false;
	CHECK_STATUS_FLAG=true;			//使能异常检测
	Init_Check_Status();
}

u8 Parse_ContinueInYBS(s8 now_gridx,s8 now_gridy)
{
	//static u8 check_step=0;
	switch(motion1.continue_checkstep)
		{
			case 0:
				if(Judge_Yaw_Reach(motion1.tgt_yaw,REPEAT_ANGLE_BIOS))
					{
						motion1.continue_checkstep++;
						return 0;
					}
				break;
			case 1:
				if(Judge_Ypos_Reach(motion1.ypos_ybs_start,POS_BIOS))
					{
						if((motion1.tgt_yaw==F_Angle_Const)&(now_gridx>grid.x_ybs_start))
							{
								motion1.continue_checkstep=0;
								return 1;
							}
						else if((motion1.tgt_yaw==B_Angle_Const)&(now_gridx<grid.x_ybs_start))
							{
								motion1.continue_checkstep=0;
								return 1;
							}
					}
				break;
		}
	return 0;
}

u8 Analysis_NeedBack_YBS(s8 ygrid_abort)
{
	s8 ygrid_analysis,now_gridx;//,now_gridy;
	s8 ydir=Read_Motion_YDir();
	now_gridx=grid.x;//now_gridy=grid.y;
	
	TRACE("Analysis NeedBack in YBS...\r\n");

	if((mode.last_sub_mode!=NORMAL_SWEEP)&(mode.last_sub_mode!=LEAK_SWEEP))
		return 0;

	if(!Read_LeftRight())		//需要准备左沿边
		{
			if(mode.sub_mode==YBS_SUB_LEFT)		//确实在左沿边
				{	
					TRACE("motion is now left YBS and leftright=0!\r\n");
					TRACE("No need analysis!!!\r\n");
					return 0;						//无需判断
				}
		}	
	else										//需要右沿边
		{
			if(mode.sub_mode==YBS_SUB_RIGHT)	//确实在右沿边
				{	
					TRACE("motion is now right YBS and leftright=1!\r\n");
					TRACE("No need analysis!!!\r\n");
					return 0;						//无需判断
				}
		}
	if(ygrid_abort==grid.y_area_start)
		{
			TRACE("ygrid_abort is sweep start.No need analysis!!!\r\n");
			return 0;
		}
	if(Read_Motion_BackSweep())
		{
			TRACE("Motion has in backsweep now! No need analysis!!\r\n");
			return 0;
		}
	
	if(ydir>0)		//沿Y轴正方向清扫
		ygrid_analysis=ygrid_abort-1;
	else if(ydir<0)
		ygrid_analysis=ygrid_abort+1;
	else
		return 0;
	
	if(motion1.tgt_yaw==F_Angle_Const)
		{
			if(now_gridx==0)
				return 0;
			if((!Read_Coordinate_Clean(now_gridx,ygrid_analysis))&(!Read_Coordinate_Clean(now_gridx-1,ygrid_analysis)))
				{
					if((!Read_Coordinate_Wall(now_gridx,ygrid_abort))&(!Read_Coordinate_Wall(now_gridx-1,ygrid_abort)))
						{
							TRACE("Need Back in YBS!!!\r\n");
							return 1;
						}
				}
		}
	else
		{
			if((now_gridx>=GRID_MAX))
				return 0;
			if((!Read_Coordinate_Clean(now_gridx,ygrid_analysis))&(!Read_Coordinate_Clean(now_gridx+1,ygrid_analysis)))
				{
					if((!Read_Coordinate_Wall(now_gridx,ygrid_abort))&(!Read_Coordinate_Wall(now_gridx+1,ygrid_abort)))
						{
							TRACE("Need Back in YBS!!!\r\n");
							return 1;
						}
				}
		}
	TRACE("No Need Back!\r\n");
	return 0;
}
