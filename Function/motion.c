#include "AAA-include.h"

#define PI180				57.295791		//180/PI
MOTION motion1;

short F_Angle_Const,B_Angle_Const,L_Angle_Const,R_Angle_Const;//机器开始清扫时的前向角度和后向角度，固定不变
short LF_Angle_Const,RF_Angle_Const,LB_Angle_Const,RB_Angle_Const;

//取当前角度的对角
short Get_Reverse_Angle(short forward_angle)
{
	short reverse_angle=0;
	if(forward_angle>0)
		reverse_angle=forward_angle-180*100;
	else
		reverse_angle=180*100+forward_angle;
	return reverse_angle;
}

//勾股定理（毕达哥拉斯）求斜边
int Get_Pythagorean(short a,short b)
{
	int c=0;
	c=sqrt(a*a+b*b);
	return c;
}

short Get_Back_Angle(void)
{
	short xpos,ypos,angle,back_angle;
//	delay_ms(1000);
	xpos=Gyro_Data.x_pos;ypos=Gyro_Data.y_pos;
	angle=(short)(atan2(abs(ypos-motion1.ypos_ybs_start),abs(xpos-motion1.xpos_ybs_start))*PI180);
	angle=abs(angle)*100;
#ifdef DEBUG_MOTION
	TRACE("motion1.xpos_ybs_start=%d ypos_ybs_start=%d in %s\r\n",motion1.xpos_ybs_start,motion1.ypos_ybs_start,__func__);
	TRACE("now xpos=%d ypos=%d in %s\r\n",xpos,ypos,__func__);
	TRACE("angle=%d in %s\r\n",angle,__func__);
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
#endif
	if(mode.last_sub_mode==SUBMODE_YBS_RIGHT)
		{
			angle=motion1.tgt_yaw-angle;
			if(angle<=(-180*100))
				angle+=360*100;
		}
	else{
			angle=motion1.tgt_yaw+angle;
			if(angle>=180*100)
				angle=angle-360*100;
		}
	back_angle=Get_Reverse_Angle(angle);
#ifdef DEBUG_MOTION
	TRACE("back_angle=%d\r\n",back_angle);
#endif
	return back_angle;
}

void Get_Const_Angle(void)
{
	F_Angle_Const=Gyro_Data.yaw;
	
	B_Angle_Const=Get_Reverse_Angle(F_Angle_Const);
	
	L_Angle_Const=F_Angle_Const-90*100;
	if(L_Angle_Const<(-18000))
		L_Angle_Const+=360*100;
	
	R_Angle_Const=F_Angle_Const+90*100;
	if(R_Angle_Const>18000)
		R_Angle_Const=R_Angle_Const-360*100;

	LF_Angle_Const=F_Angle_Const-45*100;
	if(LF_Angle_Const<(-18000))
		LF_Angle_Const+=36000;

	RF_Angle_Const=F_Angle_Const+45000;
	if(RF_Angle_Const>18000)
		RF_Angle_Const-=36000;

	RB_Angle_Const=Get_Reverse_Angle(LF_Angle_Const);
	LB_Angle_Const=Get_Reverse_Angle(RF_Angle_Const);
}

void Save_Abort_Data(void)
{
#if 1
	motion1.tgtyaw_abort=motion1.tgt_yaw;
	motion1.leftright_abort=motion1.leftright;
	motion1.anti_tgtyaw_abort=motion1.anti_tgt_yaw;
	motion1.repeat_sweep_abort=motion1.repeat_sweep;
	motion1.ydir_abort=motion1.y_dir;
	grid.x_abort=grid.x;
	grid.y_abort=grid.y;
#ifdef DEBUG_MOTION
	TRACE("Saving Abort Data...\r\n");
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgtyaw_abort);
	TRACE("motion1.leftright=%d\r\n",motion1.leftright);
	TRACE("Saving Abort Data complete!\r\n");
	TRACE("Ready to back sweep!\r\n");
#endif
#else
	Add_BackInfo();
#endif
}

void Restore_Abort_Data(void)
{
#if 1
	motion1.tgt_yaw=motion1.tgtyaw_abort;
	motion1.anti_tgt_yaw=motion1.anti_tgtyaw_abort;
	motion1.leftright=motion1.leftright_abort;
	motion1.repeat_sweep=motion1.repeat_sweep_abort;
	motion1.y_dir=motion1.ydir_abort;
#ifdef DEBUG_MOTION
	TRACE("Restore Abort Data...\r\n");
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgtyaw_abort);
	TRACE("motion1.leftright=%d\r\n",motion1.leftright);
	TRACE("Ready to Normal Sweep from Back Sweep!\r\n");
	TRACE("Loading Abort Data complete!\r\n");
#endif
#else
	Get_Curr_BackInfo();
#endif
}

/*-------------------------------------------------
function:用于回扫的漏扫检查，有些回扫区域完成后，可能还有漏扫
input:None

output:
      0:没有回扫漏扫区域，1:有回扫漏扫区域
--------------------------------------------------*/
u8 Analysis_Back_Leak(void)
{
	s8 check_gridy1,check_gridy2,check_gridx,check_gridx1,now_gridx;
	s8 xmax_check1,xmin_check1,xmax_check2,xmin_check2;
	if(grid.y_abort==grid.y_back_last)			//如果最后一次回扫的Y坐标与中断Y坐标一致，则直接退出
		return 0;
#ifdef DEBUG_MOTION
	TRACE("Enter in %s...\r\n",__func__);
#endif
	check_gridy1=grid.y_abort;
	now_gridx=grid.x;
	while(check_gridy1!=grid.y_back_last)		//从当前中断Y坐标开始，直到最后一次回扫Y坐标为止，每个Y坐标进行检查
		{
			if(grid.y_back_last>grid.y_abort)
				check_gridy2=check_gridy1+1;
			else
				check_gridy2=check_gridy1-1;
			xmax_check1=Return_MaxClean_GridX(check_gridy1,0);
			xmin_check1=Return_MinClean_GridX(check_gridy1,0);
			xmax_check2=Return_MaxClean_GridX(check_gridy2,0);
			xmin_check2=Return_MinClean_GridX(check_gridy2,0);
			for(check_gridx=now_gridx;check_gridx<xmax_check1;check_gridx++)
				{
					if((Read_Coordinate_Seat(xmax_check2,check_gridy2))|(Read_Coordinate_Seat(xmax_check1,check_gridy1)))
						break;
					if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy1))&(!Read_Coordinate_Clean(check_gridx,check_gridy2)))
						{		
							check_point.backup_grid=0;					
							check_gridx1=check_gridx;
							while(check_gridx1<xmax_check1)
								{
									if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy1))&(!Read_Coordinate_Clean(check_gridx,check_gridy2)))
										{
											check_point.backup_grid++;
											check_gridx1++;
										}
									else
										break;
								}
							check_point.new_x1=check_gridx;
							check_point.new_y1=check_gridy1;
							check_point.new_x2=check_gridx;
							check_point.new_y2=check_gridy2;
							check_point.next_tgtyaw=F_Angle_Const;
							if(check_gridy1>check_gridy2)
								{
									check_point.ydir=0;
									check_point.ybs_dir=DIR_LEFT;
								}
							else
								{
									check_point.ydir=1;
									check_point.ybs_dir=DIR_RIGHT;
								}
							Set_CheckPoint_NextAction(CHECK_BACKSWEEP);	//CHECK_ACTION设置为回扫（回扫漏扫仍是回扫）
							if(Can_Entry_Point())
								return 1;
						}
				}
			
			for(check_gridx=now_gridx;check_gridx>xmin_check1;check_gridx--)
				{
					if((Read_Coordinate_Seat(xmin_check2,check_gridy2))|(Read_Coordinate_Seat(xmin_check1,check_gridy1)))
						break;
					if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy1))&(!Read_Coordinate_Clean(check_gridx,check_gridy2)))
						{
							check_point.backup_grid=0;					
							check_gridx1=check_gridx;
							while(check_gridx1>xmin_check1)
								{
									if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy1))&(!Read_Coordinate_Clean(check_gridx,check_gridy2)))
										{
											check_point.backup_grid++;
											check_gridx1--;
										}
									else
										break;
								}
							check_point.new_x1=check_gridx;
							check_point.new_y1=check_gridy1;
							check_point.new_x2=check_gridx;
							check_point.new_y2=check_gridy2;
							check_point.next_tgtyaw=B_Angle_Const;
							Set_CheckPoint_NextAction(CHECK_BACKSWEEP);

							if(check_gridy1>check_gridy2)
								{
									check_point.ydir=0;
									check_point.ybs_dir=DIR_RIGHT;
								}
							else
								{
									check_point.ydir=1;
									check_point.ybs_dir=DIR_LEFT;
								}
							if(Can_Entry_Point())
								return 1;
						}
				}
			if(check_gridy1>grid.y_back_last)
				check_gridy1--;
			else
				check_gridy1++;
		}
#ifdef DEBUG_MOTION
	TRACE("%s complete! No leak area!!\r\n",__func__);
#endif
	return 0;
}

//功能：分析需要回扫
//返回0：无需回扫
//放回1：需要回扫
u8 Analysis_NeedBack(s8 ygrid_abort)
{
	s8 ygrid_analysis;
	s8 gridx_abort,gridx_analysis,check_gridx;
	s8 ydir=Read_Motion_YDir();

	if(ydir>0)			//沿Y轴正方向清扫
		ygrid_analysis=ygrid_abort-1;
	else if(ydir<0)
		ygrid_analysis=ygrid_abort+1;
	else
		return 0;

	if(ygrid_abort==grid.y_area_start)
		return 0;
	if(Read_Motion_BackSweep())
		return 0;
	//if(motion1.first_leak_y)
		//return 0;
#ifdef DEBUG_MOTION
	TRACE("Analysis NeedBack...\r\n");
#endif
	//if(motion1.tgt_yaw==F_Angle)
	if(motion1.tgt_yaw==F_Angle_Const)
		{
			gridx_abort=Return_MaxClean_GridX(ygrid_abort,0);
			gridx_analysis=Return_MaxClean_GridX(ygrid_analysis,0);
			if(Read_Coordinate_Seat(gridx_analysis,ygrid_analysis))
				{
					return 0;
				}
			if(gridx_abort-gridx_analysis>2)
				{
					for(check_gridx=gridx_abort-1;check_gridx>gridx_analysis;check_gridx--)
						{
							if((Read_Coordinate_CleanNoWall(check_gridx,ygrid_abort))&(Read_Coordinate_Clean(check_gridx-1,ygrid_abort)))
								{
#ifdef DEBUG_MOTION
									TRACE("Need Back!!!\r\n");
#endif
									return 1;
								}
						}
				}
		}
	else
		{
			gridx_abort=Return_MinClean_GridX(ygrid_abort,0);
			gridx_analysis=Return_MinClean_GridX(ygrid_analysis,0);
			if(Read_Coordinate_Seat(gridx_analysis,ygrid_analysis))
				{
					return 0;
				}
			if(gridx_analysis-gridx_abort>2)
				{
					for(check_gridx=gridx_abort+1;check_gridx<gridx_analysis;check_gridx++)
						{
							if((Read_Coordinate_CleanNoWall(check_gridx,ygrid_abort))&(Read_Coordinate_Clean(check_gridx+1,ygrid_abort)))
								{
#ifdef DEBUG_MOTION
									TRACE("Need Back!!!\r\n");
#endif
									return 1;
								}
						}
				}
		}
#ifdef DEBUG_MOTION
	TRACE("No Need Back!\r\n");
#endif
	return 0;
}

/*--------------------------------------
function:用于回扫碰撞时的停止分析检查
         主要是分析Y坐标是否已经到达边界
return:
	0:需要继续回扫，1:停止回扫
----------------------------------------*/
u8 Analysis_StopBack_InBump(s8 ydir,s8 now_gridx,s8 now_gridy)
{
	s8 next_gridy;
	if(ydir>0)					//沿Y轴正方向清扫
		{
			next_gridy=now_gridy+1;
			if(next_gridy>grid.y_area_max)							//当前Y坐标是区域最大Y坐标
				return 1;											//返回1，退出回扫
			//else if(Read_Coordinate_Clean(now_gridx,next_gridy))	//当前Y坐标不是区域最大Y坐标
				//return 1;											//且下一个Y坐标已经清扫，退出回扫
		}
	else if(ydir<0)
		{
			next_gridy=now_gridy-1;									
			if(next_gridy<grid.y_area_min)							//当前Y坐标是区域最小Y坐标
				return 1;											//返回1，退出回扫
			//else if(Read_Coordinate_Clean(now_gridx,next_gridy))	//当前Y坐标不是区域最小Y坐标
				//return 1;											//且下一个Y坐标已经清扫，退出回扫
		}
	return 0;
}

/*------------------------------------------
FUNCTION:检查是否结束回扫
INPUT:
	tgt_yaw:当前机器的前进角度
OUTPUT:
	0:未结束，还需要扫
	1:当前Y坐标和下一Y坐标均不需要回扫，结束回扫
	2:当前Y坐标无需回扫，下一Y坐标需要回扫。
---------------------------------------------*/
u8 Analysis_StopBack(short tgt_yaw)
{
	s8 temp_x,temp_x2,ydir;
	s8 now_gridx,now_gridy,next_gridy,next_gridx;
	now_gridx=grid.x;now_gridy=grid.y;
#ifdef DEBUG_MOTION
	TRACE("Enter in %s...\r\n",__func__);
#endif
	ydir=Read_Motion_YDir();
	if(ydir>0)
		{
			next_gridy=grid.y+1;
			if(next_gridy>grid.y_area_max)
				next_gridy=now_gridy;
		}
	else
		{
			next_gridy=grid.y-1;
			if(next_gridy<grid.y_area_min)
				next_gridy=now_gridy;
		}

	
	//if(tgt_yaw==F_Angle)
	if(tgt_yaw==F_Angle_Const)
		{
			temp_x=grid.x;
			temp_x2=grid.x;
#if 0
			if(temp_x+1>grid.x_area_max)
				temp_x=grid.x;
			else
				temp_x+=1;
#endif			
			if(temp_x+1<=grid.x_area_max)
				temp_x+=1;
			if(temp_x2+2<=grid.x_area_max)
				temp_x2+=2;
			
			next_gridx=now_gridx;
			//if(Read_Coordinate_Clean(temp_x,grid.y))
			if((Read_Coordinate_Clean(temp_x,grid.y))&(Read_Coordinate_Clean(temp_x2,grid.y)))
				{
#ifdef DEBUG_MOTION
					TRACE("coor[%d][%d] has clean !!\r\n",grid.y,temp_x);
					TRACE("coor[%d][%d] has clean !!\r\n",grid.y,temp_x2);
#endif
					if(Read_Coordinate_Clean(next_gridx,next_gridy))
						{
#ifdef DEBUG_MOTION
							TRACE("coor[%d][%d] has clean !!\r\n",next_gridy,next_gridx);
							TRACE("return 1\r\n");
#endif
							return 1;
						}
					else
						{
#ifdef DEBUG_MOTION
							TRACE("return 2!!\r\n");
#endif
							return 2;
						}
				}
		}
	else
		{
			temp_x=grid.x;
			temp_x2=grid.x;
#if 0
			if(temp_x-1<grid.x_area_min)
				temp_x=GRID_MIN;
			else
				temp_x-=1;
#endif
			if(temp_x-1>=grid.x_area_min)
				temp_x-=1;
			if(temp_x2-2>=grid.x_area_min)
				temp_x2-=2;
			
			next_gridx=now_gridx;
			//if(Read_Coordinate_Clean(temp_x,grid.y))
			if((Read_Coordinate_Clean(temp_x,grid.y))&(Read_Coordinate_Clean(temp_x2,grid.y)))
				{
#ifdef DEBUG_MOTION
					TRACE("coor[%d][%d] has clean !!\r\n",grid.y,temp_x);
					TRACE("coor[%d][%d] has clean !!\r\n",grid.y,temp_x2);
#endif
					if(Read_Coordinate_Clean(next_gridx,next_gridy))
						{
#ifdef DEBUG_MOTION
							TRACE("coor[%d][%d] has clean !!\r\n",next_gridy,next_gridx);
							TRACE("return 1\r\n");
#endif
							return 1;
						}
					else
						{
#ifdef DEBUG_MOTION
							TRACE("return 2!!\r\n");
#endif
							return 2;
						}
				}
		}
	return 0;
}

//设置左/右沿边标志
//0:左沿边（右转），1：右沿边（左转）
void Set_LeftRight(u8 leftright)
{	
	motion1.leftright=leftright;
}

//读取左/右沿边标志
//0:左沿边（右转），1：右沿边（左转）
u8 Read_LeftRight(void)
{
	if(motion1.leftright)
		return 1;
	else
		return 0;
}

void Change_LeftRight(void)
{
	motion1.leftright=!motion1.leftright;
}

void Assign_LeftRight(void)
{
	s8 ydir=Read_Motion_YDir();
	switch(ydir)
		{
			case -1:
				if(motion1.tgt_yaw==F_Angle_Const)
					Set_LeftRight(1);			//设置方向右沿边
				else
					Set_LeftRight(0);			//设置方向左沿边
				break;
			case 0:
			case 1:
				if(motion1.tgt_yaw==F_Angle_Const)
					Set_LeftRight(0);			//设置方向左沿边
				else
					Set_LeftRight(1);			//设置方向右沿边
				break;
		}
	
}

void Set_Motion_BackSweep(u8 back_sweep)
{
	motion1.back_sweep=back_sweep;
}

//function:读取当前机器是否在回扫中
//output
//0:不在回扫
//1:在回扫中
u8 Read_Motion_BackSweep(void)
{
	if(motion1.back_sweep)
		return 1;
	else
		return 0;
}

void Set_Motion_Acc(u8 x_acc,u8 y_acc)
{
	motion1.y_acc=y_acc;
	motion1.x_acc=x_acc;
}

u8 Read_Motion_YAcc(void)
{
	if(motion1.y_acc)
		return 1;
	else
		return 0;
}

//Y轴清扫时的增长方向(0:负方向,1:正方向),因为y_acc有0,1,2三个值，2代表双方向清扫，无法再用y_acc表示目前正在清扫的方向
//function:返回机器当前Y轴清扫方向，包括y_acc为2时的双方向清扫
//output:
//0:Y轴负方向清扫
//1:Y轴正方向清扫
void Set_Motion_YDir(s8 ydir)
{
	motion1.y_dir=ydir;
}

u8 Set_Motion_YDir_Reverse(void)
{
	s8 ydir=Read_Motion_YDir();
	if(ydir>0)
		Set_Motion_YDir(-1);
	else if(ydir<0)
		Set_Motion_YDir(1);
	else
		return 1;
	return 0;
}

s8 Read_Motion_YDir(void)
{
	return motion1.y_dir;
}
//计算打扫区域
//X_ACC: 0:按X轴正负清扫;1:X轴都只朝一个方向清扫
//Y_ACC: 0:按Y轴负方向清扫;1:按Y轴正方向清扫;2:按双方向清扫
void Cal_PosArea_Max(void)
{
//	short xmax,xmin,ymax,ymin;
	short temp_pos;
#ifdef DEBUG_MOTION
	TRACE("Enter in %s...\r\n",__func__);
	TRACE("motion1.xacc=%d yacc=%d\r\n",motion1.x_acc,motion1.y_acc);
#endif
	motion1.xpos_start_area=Return_GridXPos_Point(grid.x_area_start);
	motion1.ypos_start_area=Return_GridYPos_Point(grid.y_area_start);

	switch(motion1.x_acc)
		{
			case 0:				//沿X轴负方向(B_Angle_Const)清扫
				grid.x_area_max=grid.x_area_start;
				if(grid.x_area_max>GRID_MAX)
					grid.x_area_max=GRID_MAX;
				grid.x_area_min=grid.x_area_start-20;
				if(grid.x_area_min<GRID_MIN)
					grid.x_area_min=GRID_MIN;
				break;
			case 1:				//沿X轴正方向(F_Angle_Const)清扫
				grid.x_area_max=grid.x_area_start+20;
				if(grid.x_area_max>GRID_MAX)
					grid.x_area_max=GRID_MAX;
				grid.x_area_min=grid.x_area_start;
				if(grid.x_area_min<GRID_MIN)
					grid.x_area_min=GRID_MIN;
				break;
			case 2:				//沿X轴双方向清扫
				grid.x_area_max=grid.x_area_start+10;
				if(grid.x_area_max>GRID_MAX)
					grid.x_area_max=GRID_MAX;
				grid.x_area_min=grid.x_area_start-10;
				if(grid.x_area_min<GRID_MIN)
					grid.x_area_min=GRID_MIN;
			default:
				break;
		}
	temp_pos=Return_GridXPos_Point(grid.x_area_max);
	temp_pos+=8;
	if(temp_pos>motion1.xpos_start+RANGE_MAX)
		temp_pos=motion1.xpos_start+RANGE_MAX;
	motion1.xpos_max_area=temp_pos;
	
	temp_pos=Return_GridXPos_Point(grid.x_area_min);
	temp_pos-=8;
	if(temp_pos<motion1.xpos_start-RANGE_MIN)
		temp_pos=motion1.xpos_start-RANGE_MIN;
	motion1.xpos_min_area=temp_pos;

	switch (motion1.y_acc)
		{
			case 0:				//沿Y轴负方向(L_Angle_Const)清扫
				grid.y_area_max=grid.y_area_start;
				if(grid.y_area_max>GRID_MAX)
					grid.y_area_max=GRID_MAX;
				grid.y_area_min=grid.y_area_start-20;
				if(grid.y_area_min<GRID_MIN)
					grid.y_area_min=GRID_MIN;
				break;
			case 1:				//沿Y轴正方向(R_Angle_Const)清扫			
				grid.y_area_max=grid.y_area_start+20;
				if(grid.y_area_max>GRID_MAX)
					grid.y_area_max=GRID_MAX;
				grid.y_area_min=grid.y_area_start;
				if(grid.y_area_min<GRID_MIN)
					grid.y_area_min=GRID_MIN;
				break;
			case 2:				//沿Y轴双方向清扫
			default:
				grid.y_area_max=grid.y_area_start+10;
				if(grid.y_area_max>GRID_MAX)
					grid.y_area_max=GRID_MAX;
				grid.y_area_min=grid.y_area_start-10;
				if(grid.y_area_min<GRID_MIN)
					grid.y_area_min=GRID_MIN;				
				break;				
		}
	
	temp_pos=Return_GridYPos_Point(grid.y_area_min);
	temp_pos-=8;
	if(temp_pos<motion1.ypos_start-RANGE_MAX)
		temp_pos=motion1.ypos_start-RANGE_MAX;
	motion1.ypos_min_area=temp_pos;
	temp_pos=Return_GridYPos_Point(grid.y_area_max);
	temp_pos+=8;
	if(temp_pos>motion1.ypos_start+RANGE_MAX)
		temp_pos=motion1.ypos_start+RANGE_MAX;
	motion1.ypos_max_area=temp_pos;
#ifdef DEBUG_MOTION
	TRACE("Grid.x_area_start=%d\r\n",grid.x_area_start);
	TRACE("Gird.y_area_start=%d\r\n",grid.y_area_start);
	TRACE("Grid.x_area_max=%d\r\n",grid.x_area_max);
	TRACE("Grid.x_area_min=%d\r\n",grid.x_area_min);
	TRACE("Grid.y_area_max=%d\r\n",grid.y_area_max);
	TRACE("Grid.y_area_min=%d\r\n",grid.y_area_min);
	TRACE("motion1.xpos_max_area=%d\r\n",motion1.xpos_max_area);
	TRACE("motion1.xpos_min_area=%d\r\n",motion1.xpos_min_area);
	TRACE("motion1.ypos_max_area=%d\r\n",motion1.ypos_max_area);
	TRACE("motion1.ypos_min_area=%d\r\n",motion1.ypos_min_area);
#endif
}

u8 Analysis_LastYClean(void)
{
	s8 ydir,now_gridx,now_gridy;
	s8 check_gridx1,check_gridy;//check_gridx2,check_gridx3;
	u8 check_data1;

	if((motion1.tgt_yaw!=F_Angle_Const)&(motion1.tgt_yaw!=B_Angle_Const))
		return 0;
	
	ydir=Read_Motion_YDir();
	now_gridx=grid.x;
	now_gridy=grid.y;
	switch(ydir)
		{
			case 0:
				return 0;
			case 1:											//Y轴正增长方向
				if(now_gridy-1<grid.y_area_min)
					return 0;
				check_gridy=now_gridy-1;
				if(motion1.tgt_yaw==F_Angle_Const)			//清扫角度为F_Angle_Const
					{
						if(now_gridx+1>grid.x_area_max)
							return 0;
						check_gridx1=now_gridx+1;
					}
				else										//清扫角度为B_Angle_Const
					{
						if(now_gridx-1<grid.x_area_min)
							return 0;
						check_gridx1=now_gridx-1;
						
					}
				check_data1=Read_Coordinate_CleanNoWall(check_gridx1,check_gridy);
				//check_data2=Read_Coordinate_CleanNoWall(check_gridx2,check_gridy);
				//check_data3=Read_Coordinate_Clean(check_gridx3,check_gridy);
				//if((check_data1|check_data2)&(check_data3))
				if(check_data1)
					return 1;
				break;
			case -1:			
				if(now_gridy+1>grid.y_area_max)
					return 0;
				check_gridy=now_gridy+1;
				if(motion1.tgt_yaw==F_Angle_Const)			//清扫角度为F_Angle_Const
					{
						if(now_gridx+1>grid.x_area_max)
							return 0;
						check_gridx1=now_gridx+1;
					}
				else										//清扫角度为B_Angle_Const
					{
						if(now_gridx-1<grid.x_area_min)
							return 0;
						check_gridx1=now_gridx-1;
						
					}
				check_data1=Read_Coordinate_CleanNoWall(check_gridx1,check_gridy);
				//check_data2=Read_Coordinate_CleanNoWall(check_gridx2,check_gridy);
				//check_data3=Read_Coordinate_Clean(check_gridx3,check_gridy);
				//if((check_data1|check_data2)&(check_data3))
				if(check_data1)
					return 1;
				break;
		}
	return 0;
}

void Set_AreaWorkTime(u32 min_num)
{
#ifdef DEBUG_MOTION
	TRACE("set max time=%d\r\n",min_num);
#endif
	u32 sec_num=min_num*60*10000;
	motion1.worktime_area=giv_sys_time;
	motion1.worktime_area_max=sec_num;
}

void Work_TimeOut_Handle(void)
{
	u8 temp_nextaction=Read_CheckPoint_NextAction();
	if((mode.mode==MODE_SWEEP)|(mode.mode==MODE_SHIFT))
		{
			if(giv_sys_time-motion1.worktime_area>motion1.worktime_area_max)
				{
					if(mode.mode==MODE_SWEEP)
						{
							stop_rap();
#ifdef DEBUG_MOTION
							TRACE("working time out!!!\r\n");
							TRACE("now is sweep,goto area_check!!!\r\n");
#endif
							motion1.area_ok=true;
#ifdef USE_AREA_TREE
							Set_Curr_AreaTree_LeakInfo(motion1.area_ok);
#else
							Set_CurrNode_LeakInfo(motion1.area_ok);
#endif
							Area_Check(0);
							Init_Shift_Point1(0);
						}
					else
						{
							switch(temp_nextaction)
								{
									case CHECK_BACK2NORMAL:
										Del_All_BackInfo();
										Area_Check(0);
										Init_Shift_Point1(0);
										break;
									case CHECK_NORMALSWEEP:
									case CHECK_BACKSWEEP:
									case CHECK_LEAKSWEEP:
										stop_rap();
#ifdef DEBUG_MOTION
										TRACE("working time out!!!\r\n");
										TRACE("now is shift,set leak_area ok,and goto new_area!!!\r\n");
#endif
										motion1.area_ok=true;
#ifdef USE_AREA_TREE
										Set_Curr_AreaTree_LeakInfo(motion1.area_ok);
#else
										Set_CurrNode_LeakInfo(motion1.area_ok);
#endif
										Set_AreaWorkTime(5);
										Area_Check(0);
										Init_Shift_Point1(0);
										break;
									case CHECK_NEWAREA:
										stop_rap();
#ifdef DEBUG_MOTION
										TRACE("working time out!!!\r\n");
#endif
										switch(check_point.new_area_dir)
											{
												case DIR_YMAX:
#ifdef DEBUG_MOTION
													TRACE("Dir Ymax timeout!!!,Set Ymax Ok!\r\n");
#endif
													motion1.ymax_ok=true;
#ifdef USE_AREA_TREE
													Set_Curr_AreaTree_NewInfo(motion1.ymax_ok,DIR_YMAX);
#else
													Set_CurrNode_NewAreaInfo(motion1.ymax_ok, DIR_YMAX);
#endif
												break;
												case DIR_XMAX:
#ifdef DEBUG_MOTION
													TRACE("DIR_XMAX timeout!!!,Set Xmax Ok!\r\n");
#endif
													motion1.xmax_ok=true;
#ifdef USE_AREA_TREE
													Set_Curr_AreaTree_NewInfo(motion1.xmax_ok,DIR_XMAX);
#else
													Set_CurrNode_NewAreaInfo(motion1.xmax_ok, DIR_XMAX);
#endif
												break;
												case DIR_YMIN:
#ifdef DEBUG_MOTION
													TRACE("DIR_YMIN timeout!!!,Set Ymin Ok!\r\n");
#endif
													motion1.ymin_ok=true;
#ifdef USE_AREA_TREE
													Set_Curr_AreaTree_NewInfo(motion1.ymin_ok,DIR_YMIN);
#else
													Set_CurrNode_NewAreaInfo(motion1.ymin_ok, DIR_YMIN);
#endif
												break;
												case DIR_XMIN:
#ifdef DEBUG_MOTION
													TRACE("DIR_XMIN timeout!!!,Set Xmin Ok!\r\n");
#endif
													motion1.xmin_ok=true;
#ifdef USE_AREA_TREE
													Set_Curr_AreaTree_NewInfo(motion1.xmin_ok,DIR_XMIN);
#else
													Set_CurrNode_NewAreaInfo(motion1.xmin_ok, DIR_XMIN);
#endif
												break;
												default:
#ifdef DEBUG_MOTION
													TRACE("Set AllNewArea OK!!\r\n");
#endif
#ifdef USE_AREA_TREE
													Set_Curr_AreaTree_AllNewOK();
#else
													Set_Curr_AllNewAreaOK();
#endif
													break;
													
											}
										//Set_Curr_AllNewAreaOK();
#ifdef DEBUG_MOTION
										TRACE("Set Working time max 10min!\r\n");
#endif
										Set_AreaWorkTime(10);
										Area_Check(0);
										Init_Shift_Point1(0);
										break;
									case CHECK_GOEXIT:
									case CHECK_DOCK:
										stop_rap();
#ifdef DEBUG_MOTION
										TRACE("working time out!!!\r\n");
										TRACE("now is exit,goto dock!!!\r\n");
#endif
										//Init_Docking();
										Init_Sweep_Done();
										break;
								}
						}
				}
		}
	else if((mode.mode==MODE_YBS)|(mode.mode==MODE_SPOT))
		{
			if(giv_sys_time-motion1.worktime_area>motion1.worktime_area_max)
				{
					stop_rap();
#ifdef DEBUG_MOTION
					TRACE("working time out!!!\r\n");
					TRACE("goto dock!!!\r\n");
#endif
					Init_Sweep_Done();
				}
		}
}

void Set_Seat_Grid(void)
{
	//充电座坐标(-2,0)，X坐标
//	s8 gridx,gridy;
	Set_Coordinate_Seat(-1,-2);Set_Coordinate_Seat(-1,-1);Set_Coordinate_Seat(-1,0);Set_Coordinate_Seat(-1,1);Set_Coordinate_Seat(-1,2);
	Set_Coordinate_Seat(-2,-2);Set_Coordinate_Seat(-2,-1);Set_Coordinate_Seat(-2,0);Set_Coordinate_Seat(-2,1);Set_Coordinate_Seat(-2,2);
	Set_Coordinate_Seat(-3,-2);Set_Coordinate_Seat(-3,-1);Set_Coordinate_Seat(-3,0);Set_Coordinate_Seat(-3,1);Set_Coordinate_Seat(-3,2);
}

u8 Analysis_InSeatArea(s8 now_gridx,s8 now_gridy)
{
	if((now_gridx<=-1)&(now_gridx>=-3))
		{
			if((now_gridy<=2)&(now_gridy>=-2))
				{
					return 1;
				}
		}
	return 0;
}

u8 Analysis_Check_Dock(void)
{
#if 1
	u32 find_home=ReadHwSign_My();
	u8 next_action=Read_CheckPoint_NextAction();

	if(next_action!=CHECK_DOCK)
		return 0;

	if(top_time_sec>=6)
		{
			if((find_home&ALL_TOP_MASK)&(mode.bump!=BUMP_SEAT))
				{
					stop_rap();
#ifdef DEBUG_MOTION
					TRACE("Call this in %s %d\r\n",__func__,__LINE__);
#endif
					//Init_Docking();
					Init_Sweep_Done();
					return 1;
				}
		}

	if(Analysis_InSeatArea(grid.x, grid.y))
		{
			stop_rap();
#ifdef DEBUG_MOTION
			TRACE("Motion in seat area!!!\r\n");
#endif
			//Init_Docking();
			Init_Sweep_Done();
			return 1;
		}
	return 0;
#else
	return 0;
#endif
}

u8 Analysis_Stop_StartArea(void)
{

	if(motion1.area_num==1)
		{
			if(Read_CheckPoint_NextAction()==CHECK_GOEXIT)
				{
					if((abs(grid.x-grid.x_start)<2)&(abs(grid.y-grid.y_start)<2))
						return 1;
				}
		}
	return 0;
}


u8 Force_Dock(void)
{
	u8 temp_data1=0,temp_data2=0;
#ifdef DEBUG_MOTION
	TRACE("Enter in %s...\r\n",__func__);
#endif
	temp_data1=Find_Directly_Way(grid.x_start,grid.y_start);
	temp_data2=Find_PathPoint_WayAll(grid.x_start,grid.y_start);
	
	//motion1.area_ok=true;
	//Set_CurrNode_LeakInfo(motion1.area_ok);
	//Set_Curr_AllNewAreaOK();	
	if(temp_data1|temp_data2)
		{
			check_point.new_x1=grid.x_start;
			check_point.new_y1=grid.y_start;
			check_point.new_x2=grid.x_start;
			check_point.new_y2=grid.y_start;
			Set_CheckPoint_NextAction(CHECK_DOCK);
			Init_Shift_Point1(0);
		}
	else
		{
#ifdef DEBUG_MOTION
			TRACE("Can't Directly to 00!!\r\n");
#endif
			Area_Check(0);						
			//Set_CheckPoint_NextAction(CHECK_DOCK);
			Init_Shift_Point1(0);
		}
#ifdef DEBUG_MOTION
	TRACE("Out %s!!\r\n",__func__);
#endif
	return 0;
}
