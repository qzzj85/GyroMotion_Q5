#include "AAA-include.h"

#define PI180				57.295791		//180/PI

short F_Angle_Const,B_Angle_Const,L_Angle_Const,R_Angle_Const;//������ʼ��ɨʱ��ǰ��ǶȺͺ���Ƕȣ��̶�����
short LF_Angle_Const,RF_Angle_Const,LB_Angle_Const,RB_Angle_Const;

//ȡ��ǰ�ǶȵĶԽ�
short Get_Reverse_Angle(short forward_angle)
{
	short reverse_angle=0;
	if(forward_angle>0)
		reverse_angle=forward_angle-180*100;
	else
		reverse_angle=180*100+forward_angle;
	return reverse_angle;
}

//���ɶ����ϴ����˹����б��
int Get_Pythagorean(short a,short b)
{
	int c=0;
	c=sqrt(a*a+b*b);
	return c;
}

short Get_Back_Angle(void)
{
	short xpos,ypos,angle,back_angle;
	delay_ms(1000);
	xpos=Gyro_Data.x_pos;ypos=Gyro_Data.y_pos;
	TRACE("motion1.xpos_ybs_start=%d ypos_ybs_start=%d in %s\r\n",motion1.xpos_ybs_start,motion1.ypos_ybs_start,__func__);
	TRACE("now xpos=%d ypos=%d in %s\r\n",xpos,ypos,__func__);
	angle=(short)(atan2(abs(ypos-motion1.ypos_ybs_start),abs(xpos-motion1.xpos_ybs_start))*PI180);
	angle=abs(angle)*100;
	TRACE("angle=%d in %s\r\n",angle,__func__);
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	if(mode.last_sub_mode==YBS_SUB_RIGHT)
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
	TRACE("back_angle=%d\r\n",back_angle);
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
	TRACE("Saving Abort Data...\r\n");
	motion1.xpos_abort=Gyro_Data.x_pos;
	motion1.ypos_abort=Gyro_Data.y_pos;
	motion1.tgtyaw_abort=motion1.tgt_yaw;
	motion1.anti_tgtyaw_abort=motion1.anti_tgt_yaw;
	grid.x_abort=grid.x;
	grid.y_abort=grid.y;
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgtyaw_abort);
	TRACE("motion1.leftright=%d\r\n",motion1.leftright);
	TRACE("Saving Abort Data complete!\r\n");
	TRACE("Ready to back sweep!\r\n");
}

void Load_Abort_Data(void)
{
	TRACE("Loading Abort Data...\r\n");
#if 0
	motion1.tgt_yaw=motion1.tgtyaw_abort;
	motion1.anti_tgt_yaw=motion1.anti_tgtyaw_abort;
	motion1.leftright=motion1.leftright_abort;
	motion1.repeat_sweep=motion1.repeat_sweep_abort;
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgtyaw_abort);
	TRACE("motion1.leftright=%d\r\n",motion1.leftright);
	TRACE("Ready to Normal Sweep from Back Sweep!\r\n");
	TRACE("Loading Abort Data complete!\r\n");
#endif
	Get_Curr_BackInfo();
}


u8 Analysis_Back_Leak(void)
{
	s8 check_gridy1,check_gridy2,check_gridx,now_gridx;
	s8 xmax_check1,xmin_check1,xmax_check2,xmin_check2;
	if(grid.y_abort==grid.y_back_last)
		return 0;

	TRACE("Enter in %s...\r\n",__func__);
	check_gridy1=grid.y_abort;
	now_gridx=grid.x;
	while(check_gridy1!=grid.y_back_last)
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
							check_point.new_x1=check_gridx;
							check_point.new_y1=check_gridy1;
							check_point.new_x2=check_gridx;
							check_point.new_y2=check_gridy2;
							check_point.next_tgtyaw=F_Angle_Const;
							Set_CheckPoint_NextAction(CHECK_BACK);
							if(check_gridy1>check_gridy2)
								check_point.ybs_dir=LEFT;
							else
								check_point.ybs_dir=RIGHT;
							return 1;
						}
				}
			for(check_gridx=now_gridx;check_gridx>xmin_check1;check_gridx--)
				{
					if((Read_Coordinate_Seat(xmin_check2,check_gridy2))|(Read_Coordinate_Seat(xmin_check1,check_gridy1)))
						break;
					if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy1))&(!Read_Coordinate_Clean(check_gridx,check_gridy2)))
						{
							check_point.new_x1=check_gridx;
							check_point.new_y1=check_gridy1;
							check_point.new_x2=check_gridx;
							check_point.new_y2=check_gridy2;
							check_point.next_tgtyaw=B_Angle_Const;
							Set_CheckPoint_NextAction(CHECK_BACK);
							if(check_gridy1>check_gridy2)
								{
									 check_point.ybs_dir=RIGHT;
								}
							else
								{
									check_point.ybs_dir=LEFT;
								}
							return 1;
						}
				}
			if(check_gridy1>grid.y_back_last)
				check_gridy1--;
			else
				check_gridy1++;
		}
	TRACE("%s complete! No leak area!!\r\n",__func__);
	return 0;
}

//���ܣ�������Ҫ��ɨ
//����0�������ɨ
//�Ż�1����Ҫ��ɨ
u8 Analysis_NeedBack(s8 ygrid_abort)
{
	s8 ygrid_analysis;
	s8 now_gridx,now_gridy;
	s8 gridx_abort,gridx_analysis,check_gridx;
	s8 ydir=Read_Motion_YDir();
	now_gridx=grid.x;now_gridy=grid.y;

	if(ydir>0)			//��Y����������ɨ
		ygrid_analysis=ygrid_abort-1;
	else if(ydir<0)
		ygrid_analysis=ygrid_abort+1;
	else
		return 0;

	if(ygrid_abort==grid.y_area_start)
		return 0;
	if(Read_Motion_BackSweep())
		return 0;
	TRACE("Analysis NeedBack...\r\n");
	//if(motion1.tgt_yaw==F_Angle)
#if 0
	if(motion1.tgt_yaw==F_Angle_Const)
		{
			if(now_gridx==0)
				return 0;
			if((!Read_Coordinate_Clean(now_gridx,ygrid_analysis))&(!Read_Coordinate_Clean(now_gridx-1,ygrid_analysis)))
				{
					if((!Read_Coordinate_Wall(now_gridx,ygrid_abort))&(!Read_Coordinate_Wall(now_gridx-1,ygrid_abort)))
						{
							TRACE("Need Back!!!\r\n");
							return 1;
						}
				}
		}
	else
		{
			if(now_gridx>=GRID_MAX)
				return 0;
			if((!Read_Coordinate_Clean(now_gridx,ygrid_analysis))&(!Read_Coordinate_Clean(now_gridx+1,ygrid_analysis)))
				{
					if((!Read_Coordinate_Wall(now_gridx,ygrid_abort))&(!Read_Coordinate_Wall(now_gridx+1,ygrid_abort)))
						{
							TRACE("Need Back!!!\r\n");
							return 1;
						}
				}
		}
	TRACE("No Need Back!\r\n");
	return 0;
#else
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

									TRACE("Need Back!!!\r\n");
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

									TRACE("Need Back!!!\r\n");
									return 1;
								}
						}
				}
		}
	TRACE("No Need Back!\r\n");
	return 0;
#endif
}

//return:
//0:��Ҫ������ɨ��1:ֹͣ��ɨ
u8 Analysis_StopBack_InBump(s8 ydir,s8 now_gridx,s8 now_gridy)
{
	s8 next_gridy;
	if(ydir>0)					//��Y����������ɨ
		{
			next_gridy=now_gridy+1;
			if(next_gridy>grid.y_area_max)							//��ǰY�������������Y����
				return 1;											//����1���˳���ɨ
			else if(Read_Coordinate_Clean(now_gridx,next_gridy))	//��ǰY���겻���������Y����
				return 1;											//����һ��Y�����Ѿ���ɨ���˳���ɨ
		}
	else if(ydir<0)
		{
			next_gridy=now_gridy-1;									
			if(next_gridy<grid.y_area_min)							//��ǰY������������СY����
				return 1;											//����1���˳���ɨ
			else if(Read_Coordinate_Clean(now_gridx,next_gridy))	//��ǰY���겻��������СY����
				return 1;											//����һ��Y�����Ѿ���ɨ���˳���ɨ
		}
	return 0;
}

/*------------------------------------------
FUNCTION:����Ƿ������ɨ
INPUT:
	tgt_yaw:��ǰ������ǰ���Ƕ�
OUTPUT:
	0:δ����������Ҫɨ
	1:��ǰY�������һY���������Ҫ��ɨ��������ɨ
	2:��ǰY���������ɨ����һY������Ҫ��ɨ��
---------------------------------------------*/
u8 Analysis_StopBack(short tgt_yaw)
{
	s8 temp_x,ydir;
	s8 now_gridx,now_gridy,next_gridy,next_gridx;
	now_gridx=grid.x;now_gridy=grid.y;
	
	ydir=Read_Motion_YDir();
	if(ydir>0)
		{
			next_gridy=grid.y+1;
			if(next_gridy>grid.y_area_max)
				next_gridy=grid.y;
		}
	else
		{
			next_gridy=grid.y-1;
			if(next_gridy<grid.y_area_min)
				next_gridy=grid.y;
		}

	
	//if(tgt_yaw==F_Angle)
	if(tgt_yaw==F_Angle_Const)
		{
			temp_x=grid.x;
#if 0
			if(temp_x+1>grid.x_area_max)
				temp_x=grid.x;
			else
				temp_x+=1;
#endif			
			if(temp_x+1<=grid.x_area_max)
				temp_x+=1;
			next_gridx=now_gridx;
			if(Read_Coordinate_Clean(temp_x,grid.y))
				{
					if(Read_Coordinate_Clean(next_gridx,next_gridy))
						return 1;
					else
						return 2;
				}
		}
	else
		{
			temp_x=grid.x;
#if 0
			if(temp_x-1<grid.x_area_min)
				temp_x=GRID_MIN;
			else
				temp_x-=1;
#endif
			if(temp_x-1>=grid.x_area_min)
				temp_x-=1;
			next_gridx=now_gridx;
			if(Read_Coordinate_Clean(temp_x,grid.y))
				{
					if(Read_Coordinate_Clean(next_gridx,next_gridy))
						return 1;
					else
						return 2;
				}
		}
	return 0;
}

//������/���ر߱�־
//0:���رߣ���ת����1�����رߣ���ת��
void Set_LeftRight(u8 leftright)
{	
	motion1.leftright=leftright;
}

//��ȡ��/���ر߱�־
//0:���رߣ���ת����1�����رߣ���ת��
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
					Set_LeftRight(1);			//���÷������ر�
				else
					Set_LeftRight(0);			//���÷������ر�
				break;
			case 0:
			case 1:
				if(motion1.tgt_yaw==F_Angle_Const)
					Set_LeftRight(0);			//���÷������ر�
				else
					Set_LeftRight(1);			//���÷������ر�
				break;
		}
	
}

void Set_Motion_BackSweep(u8 back_sweep)
{
	motion1.back_sweep=back_sweep;
}

//function:��ȡ��ǰ�����Ƿ��ڻ�ɨ��
//output
//0:���ڻ�ɨ
//1:�ڻ�ɨ��
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

//Y����ɨʱ����������(0:������,1:������),��Ϊy_acc��0,1,2����ֵ��2����˫������ɨ���޷�����y_acc��ʾĿǰ������ɨ�ķ���
//function:���ػ�����ǰY����ɨ���򣬰���y_accΪ2ʱ��˫������ɨ
//output:
//0:Y�Ḻ������ɨ
//1:Y����������ɨ
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
//�����ɨ����
//X_ACC: 0:��X��������ɨ;1:X�ᶼֻ��һ��������ɨ
//Y_ACC: 0:��Y�Ḻ������ɨ;1:��Y����������ɨ;2:��˫������ɨ
void Cal_PosArea_Max(void)
{
//	short xmax,xmin,ymax,ymin;
	short temp_pos;

	TRACE("Enter in %s...\r\n",__func__);
	TRACE("motion1.xacc=%d yacc=%d\r\n",motion1.x_acc,motion1.y_acc);

	motion1.xpos_start_area=Return_GridXPos_Point(grid.x_area_start);
	motion1.ypos_start_area=Return_GridYPos_Point(grid.y_area_start);

	switch(motion1.x_acc)
		{
			case 0:				//��X�Ḻ����(B_Angle_Const)��ɨ
				grid.x_area_max=grid.x_area_start;
				if(grid.x_area_max>GRID_MAX)
					grid.x_area_max=GRID_MAX;
				grid.x_area_min=grid.x_area_start-20;
				if(grid.x_area_min<GRID_MIN)
					grid.x_area_min=GRID_MIN;
				break;
			case 1:				//��X��������(F_Angle_Const)��ɨ
				grid.x_area_max=grid.x_area_start+20;
				if(grid.x_area_max>GRID_MAX)
					grid.x_area_max=GRID_MAX;
				grid.x_area_min=grid.x_area_start;
				if(grid.x_area_min<GRID_MIN)
					grid.x_area_min=GRID_MIN;
				break;
			case 2:				//��X��˫������ɨ
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
			case 0:				//��Y�Ḻ����(L_Angle_Const)��ɨ
				grid.y_area_max=grid.y_area_start;
				if(grid.y_area_max>GRID_MAX)
					grid.y_area_max=GRID_MAX;
				grid.y_area_min=grid.y_area_start-20;
				if(grid.y_area_min<GRID_MIN)
					grid.y_area_min=GRID_MIN;
				break;
			case 1:				//��Y��������(R_Angle_Const)��ɨ			
				grid.y_area_max=grid.y_area_start+20;
				if(grid.y_area_max>GRID_MAX)
					grid.y_area_max=GRID_MAX;
				grid.y_area_min=grid.y_area_start;
				if(grid.y_area_min<GRID_MIN)
					grid.y_area_min=GRID_MIN;
				break;
			case 2:				//��Y��˫������ɨ
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
}

u8 Analysis_LastYClean(void)
{
	s8 ydir,now_gridx,now_gridy;
	s8 check_gridx1,check_gridx2,check_gridx3,check_gridy;
	u8 check_data1,check_data2,check_data3;

	if((motion1.tgt_yaw!=F_Angle_Const)&(motion1.tgt_yaw!=B_Angle_Const))
		return 0;
	
	ydir=Read_Motion_YDir();
	now_gridx=grid.x;
	now_gridy=grid.y;
	switch(ydir)
		{
			case 0:
				return 0;
			case 1:											//Y������������
				if(now_gridy-1<grid.y_area_min)
					return 0;
				check_gridy=now_gridy-1;
				if(motion1.tgt_yaw==F_Angle_Const)			//��ɨ�Ƕ�ΪF_Angle_Const
					{
						if(now_gridx+1>grid.x_area_max)
							return 0;
						check_gridx1=now_gridx+1;
					}
				else										//��ɨ�Ƕ�ΪB_Angle_Const
					{
						if(now_gridx-1<grid.x_area_min)
							return 0;
						check_gridx1=now_gridx-1;
						
					}
				check_data1=Read_Coordinate_CleanNoWall(check_gridx1,check_gridy);
				check_data2=Read_Coordinate_CleanNoWall(check_gridx2,check_gridy);
				check_data3=Read_Coordinate_Clean(check_gridx3,check_gridy);
				//if((check_data1|check_data2)&(check_data3))
				if(check_data1)
					return 1;
				break;
			case -1:			
				if(now_gridy+1>grid.y_area_max)
					return 0;
				check_gridy=now_gridy+1;
				if(motion1.tgt_yaw==F_Angle_Const)			//��ɨ�Ƕ�ΪF_Angle_Const
					{
						if(now_gridx+1>grid.x_area_max)
							return 0;
						check_gridx1=now_gridx+1;
					}
				else										//��ɨ�Ƕ�ΪB_Angle_Const
					{
						if(now_gridx-1<grid.x_area_min)
							return 0;
						check_gridx1=now_gridx-1;
						
					}
				check_data1=Read_Coordinate_CleanNoWall(check_gridx1,check_gridy);
				check_data2=Read_Coordinate_CleanNoWall(check_gridx2,check_gridy);
				check_data3=Read_Coordinate_Clean(check_gridx3,check_gridy);
				//if((check_data1|check_data2)&(check_data3))
				if(check_data1)
					return 1;
				break;
		}
	return 0;
}

void Set_AreaWorkTime(u32 min_num)
{
	u32 sec_num=min_num*60*10000;
	motion1.worktime=giv_sys_time;
	motion1.worktime_max=sec_num;
}

void Work_TimeOut_Handle(void)
{
	u8 check_result=0;
	u8 temp_nextaction=Read_CheckPoint_NextAction();
	if((mode.mode==SWEEP)|(mode.mode==SHIFT))
		{
			if(giv_sys_time-motion1.worktime>motion1.worktime_max)
				{
					if(mode.mode==SWEEP)
						{
							stop_rap();
							TRACE("working time out!!!\r\n");
							TRACE("now is sweep,goto area_check!!!\r\n");
							motion1.area_ok=true;
							Set_CurrNode_LeakInfo(motion1.area_ok);
							Area_Check(0);
							Init_Shift_Point1(0);
						}
					else
						{
							switch(temp_nextaction)
								{
									case CHECK_NORMALSWEEP:
									case CHECK_BACK:
									case CHECK_LEAKSWEEP:
										stop_rap();
										TRACE("working time out!!!\r\n");
										TRACE("now is shift,set leak_area ok,and goto new_area!!!\r\n");
										motion1.area_ok=true;
										Set_CurrNode_LeakInfo(motion1.area_ok);
										Set_AreaWorkTime(5);
										Area_Check(0);
										Init_Shift_Point1(0);
										break;
									case CHECK_NEWAREA:
										stop_rap();
										TRACE("working time out!!!\r\n");
										switch(check_point.new_area_dir)
											{
												case DIR_YMAX:
													TRACE("Dir Ymax timeout!!!,Set Ymax Ok!\r\n");
													motion1.ymax_ok=true;
													Set_CurrNode_NewAreaInfo(motion1.ymax_ok, DIR_YMAX);
												break;
												case DIR_XMAX:
													TRACE("DIR_XMAX timeout!!!,Set Xmax Ok!\r\n");
													motion1.xmax_ok=true;
													Set_CurrNode_NewAreaInfo(motion1.xmax_ok, DIR_XMAX);
												break;
												case DIR_YMIN:
													TRACE("DIR_YMIN timeout!!!,Set Ymin Ok!\r\n");
													motion1.ymin_ok=true;
													Set_CurrNode_NewAreaInfo(motion1.ymin_ok, DIR_YMIN);
												break;
												case DIR_XMIN:
													TRACE("DIR_XMIN timeout!!!,Set Xmin Ok!\r\n");
													motion1.xmin_ok=true;
													Set_CurrNode_NewAreaInfo(motion1.xmin_ok, DIR_XMIN);
												break;
												default:
													TRACE("Set AllNewArea OK!!\r\n");
													Set_Curr_AllNewAreaOK();
													break;
													
											}
										//Set_Curr_AllNewAreaOK();
										TRACE("Set Working time max 10min!\r\n");
										Set_AreaWorkTime(10);
										Area_Check(0);
										Init_Shift_Point1(0);
										break;
									case CHECK_GOEXIT:
									case CHECK_DOCK:
										stop_rap();
										TRACE("working time out!!!\r\n");
										TRACE("now is exit,goto dock!!!\r\n");
										Init_Docking();
										break;
								}
						}
				}
		}
}

void Set_Seat_Grid(void)
{
	//���������(-2,0)��X����
	s8 gridx,gridy;
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
