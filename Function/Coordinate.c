
#include "AAA-include.h"

/*************************************************************
左右轮直径：68mm
传动比  61.6
脉冲数  16  加倍
一个脉冲的长度是68*3.14/61.6/32=0.10831980mm。
每毫米脉冲数量：9.23192206819
每米脉冲数量：9231.92206819
两轮距离  267.6mm。
毛刷   左边82mm  右边  75mm ；  平均毛刷150mm

*************************************************************/
#define PPDIS		2/CM_PLUS
#define Todo		0.02 //
#define TR			0.2704//
#define   TT    (3.1415926f/180.0f)
#define   DIS_XiShu 	1.0255
#define  F2  44
//#define   MM_PLUS  			9.23192206819 * DIS_XiShu * F2/50
//#define   CM_PLUS  			92.3192206819 * DIS_XiShu * F2/50
//#define   METER_PLUS  	9231.92206819 * DIS_XiShu * F2/50
///////////////qz//////////////////
#define COOR_BIT_CLEAN 		0x01
#define COOR_BIT_CLEANMASK	0XFE

#define COOR_BIT_WALL		0X02
#define COOR_BIT_WALLMASK	0XFD
#define COOR_BIT_WALLCLEAN	0X03

#define AREA_NO_MASK    0XF8

#define	COOR_BIT_SEAT		0x04
#define	COOR_BIT_SEATMASK	0xFB

#define	COOR_BIT_DANGER		0X04

volatile uint32_t last_r, now_r; // 采样周期计数 单位 us
float hal_r;
float X_pos,Y_pos;
u16  check_whell,check_angle;

#if 1
int   last_l_all_length, last_r_all_length;
 short  old_angle = 0;

 float r_length,l_length,r1;
#else
float r_length,l_length;
float last_l_all_length, last_r_all_length;
float  old_angle = 0;
float  r1;	
#endif

//COORDINATE coordinate[COORDI_XLENGTH][COORDI_YLENGTH] __attribute__((at(0x20002000)));//qz add 20190307:4mX4m空间的坐标格,每20cm一个坐标
volatile u8 coordinate[COORDI_YLENGTH+1][COORDI_XLENGTH+1] __attribute__((at(0x20008000)));		//坐标格信息，高6位表示区域编号（从1号开始），低2位表示CLEAN和WALL标志
int x_max[20];		//x_max为20个y坐标上，每个y坐标能到达的最大x坐标。
int x_min[20];		//x_min为20个y坐标上，每个y坐标能到达的最小x坐标。
GRID grid;
CHECK_POINT check_point;
short gridy_pos[GRIDY_POS_CNT];		//每个Y坐标最精确的点，20cm间隔，从Y_RANGE_MIN(-1000cm)到Y_RANGE_MAX(1000cm)一共1001个点
short gridx_pos[GRIDY_POS_CNT];
GYRO_BIOS gyro_bios;

void Cal_Grid_Pos(void)
{
	int temp_ypos=(short)(motion1.ypos_start)-RANGE_MAX;
	int temp_xpos=(short)(motion1.xpos_start)-RANGE_MAX;
//	TRACE("gridy_pos_cnt=%d\r\n",GRIDY_POS_CNT);
//	TRACE("temp_ypos=%d\r\n",temp_ypos);
//	TRACE("temp_xpos=%d\r\n",temp_xpos);
	for(int i=0;i<GRIDY_POS_CNT;i++)
		{
			gridy_pos[i]=temp_ypos;
			gridx_pos[i]=temp_xpos;
			temp_ypos+=20;
			temp_xpos+=20;
//			TRACE("gridy_pos[%d]=%d\r\n",i,gridy_pos[i]);
//			TRACE("gridx_pos[%d]=%d\r\n",i,gridx_pos[i]);
		}
}

#ifdef 	GYRO_ROTATE_DET
       unsigned int gyro_det_time;
       short  old_det_angle = 0;
       float  gyro_angle = 0;   	   
       float  machine_angle = 0; 
	     float  temp_angle = 0;  
       bool gyro_det_flag = FALSE   ;
#endif
void Cal_xy(void)
{
#if  1
	float x_add, y_add,w_add;
	int   r1;
		 
	r1 = r_ring.all_length-last_r_all_length +l_ring.all_length-last_l_all_length ;
	x_add =(float) r1/2/CM_PLUS * cos( (float)old_angle /100*TT);
	y_add = (float)r1/2/CM_PLUS * sin( (float)old_angle /100*TT);
	X_pos +=x_add;
	Y_pos +=y_add;;
	Gyro_Data.x_pos = X_pos;
	Gyro_Data.y_pos =	Y_pos ;
	old_angle = Gyro_Data.yaw;
#ifdef 	GYRO_ROTATE_DET
 	r1 =  (l_ring.all_length-last_l_all_length)- (r_ring.all_length-last_r_all_length);       
	w_add = (float)r1/CM_PLUS/21.6 /TT ; // 21.6cm为左右轮之间的距离  
	if (gyro_det_flag == FALSE)
		{
			gyro_det_time = giv_sys_time;
			temp_angle  = 0;
			old_det_angle =   Gyro_Data.yaw;
			gyro_det_flag = TRUE;
		}
	else  
		{
			temp_angle	+= w_add;
            if ((giv_sys_time-   gyro_det_time)> 5000 ) //500ms
                {
					machine_angle   = temp_angle ;
					gyro_det_flag = FALSE;			  
					gyro_angle  =    (float) (Gyro_Data.yaw- old_det_angle )/100;
					if (gyro_angle > 270)      
						gyro_angle  -= 360;
					if (gyro_angle < -270)      
						gyro_angle  += 360;  
	                if (fabs(gyro_angle - machine_angle )>10) 
						Send_Voice(VOICE_ERROR_WARNING	);
                }  
		}
#endif
	last_r_all_length=r_ring.all_length;
	last_l_all_length=l_ring.all_length;
#else  // 0
  	float x_add, y_add;
  	float m_dis;//,m_angle ;
  	float r_length_temp,l_length_temp;

	//r_length = R_all_length - last_r_all_length;
	//l_length = L_all_length - last_l_all_length;	
	//last_l_all_length = L_all_length ;
	//last_r_all_length = R_all_length ;
	r_length_temp=(float)(r_ring.all_length)/2;
	l_length_temp=(float)(l_ring.all_length)/2;
	r_length=r_length_temp-last_r_all_length;
	l_length=l_length_temp-last_l_all_length;
	last_r_all_length=r_length_temp;
	last_l_all_length=l_length_temp;
	r1 = (r_length +l_length)/2 ;
	m_dis = r1 *(PPDIS);

#if 0
	float temp_angle;
	 new_angle =(float) (Gyro_Data.yaw)/100;
	 temp_angle= (new_angle + old_angle)/2 ;		
	 if (abs(new_angle-old_angle ) >180)	   
		 {
			 if (temp_angle >0) 
			 	temp_angle -=180;
			 else							 
			 	temp_angle +=180;
		 }
	  old_angle   = temp_angle ;
#endif

	x_add = m_dis * cos( old_angle*TT);
	y_add = m_dis * sin( old_angle*TT);
	X_pos +=x_add;
	Y_pos +=y_add;	
    //old_angle = ypr[0];
	old_angle = (float)(Gyro_Data.yaw)/100;

	Gyro_Data.x_pos=X_pos;
	Gyro_Data.y_pos=Y_pos;
#endif
}

void Reset_XY(void)
{
	TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
	GYRO_RST_0;
	X_pos=0;Y_pos=0;
	last_l_all_length=0;last_r_all_length=0;r_length=0;l_length=0;r1=0;
	old_angle=0;
	
	Gyro_Data.x_pos=0;Gyro_Data.y_pos=0;
	grid.x=0;grid.y=0;
	l_ring.all_length=0;
	r_ring.all_length=0;
	delay_ms(300);
	GYRO_RST_1;
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}

void Init_Real_X(void)
{
	for(int i=0;i<20;i++)
		{
			x_max[i]=motion1.xpos_start_area;
			x_min[i]=motion1.xpos_start_area;
		}
}

s8 Cal_Real_Grid (s8 grid)
{
	s8 grid_real;
	grid_real=grid+COORDI_XLENGTH/2;
	if(grid_real>COORDI_XLENGTH)
		grid_real=COORDI_XLENGTH;
	else if(grid_real<0)
		grid_real=0;
	return grid_real;
}

void Set_Coordinate_Clean(s8 gridx,s8 gridy)
{
	u8 area_no;
	s8 gridx_real,gridy_real;
	gridx_real=Cal_Real_Grid(gridx);
	gridy_real=Cal_Real_Grid(gridy);
	coordinate[gridy_real][gridx_real]|=COOR_BIT_CLEAN;
	if((gridx>grid.x_area_max)|(gridx<grid.x_area_min))
		return;
	if((gridy>grid.y_area_max)|(gridy<grid.y_area_min))
		return;
	
	area_no=Read_Coordinate_AreaNo(gridx, gridy);
	if((mode.mode==SHIFT)&(area_no))
		return;

	if(area_no==0)
		{
			if(mode.sub_mode!=SWEEP_FIRST_INIT)
				motion1.clean_size+=0.04;
			
			coordinate[gridy_real][gridx_real]|=motion1.area_num<<3;
		}
}

void Reset_Coordinate_Clean(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	coordinate[ygrid_real][xgrid_real]&=COOR_BIT_CLEANMASK;
}

u8 Read_Coordinate_Clean(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
//	coordinate[ygrid_real][xgrid_real]|=motion1.area_num<<2;
	//TRACE("coor[%d][%d]=%d\r\n",ygrid,xgrid,coordinate[ygrid_real][xgrid_real]);
	if(coordinate[ygrid_real][xgrid_real]&COOR_BIT_CLEAN)
		return 1;
	else
		return 0;
}

void Set_Coordinate_Wall(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	coordinate[ygrid_real][xgrid_real]|=COOR_BIT_WALL;
}

void Set_Coordinate_WallClean(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	coordinate[ygrid_real][xgrid_real]|=COOR_BIT_WALLCLEAN;
}

void Reset_Coordinate_Wall(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	coordinate[ygrid_real][xgrid_real]&=COOR_BIT_WALLMASK;
}

u8 Read_Coordinate_Wall(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	if(coordinate[ygrid_real][xgrid_real]&COOR_BIT_WALL)
		return 1;
	else
		return 0;
}

void Set_Coordinate_Seat(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	coordinate[ygrid_real][xgrid_real]|=COOR_BIT_SEAT;
}

u8 Read_Coordinate_Seat(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	if(coordinate[ygrid_real][xgrid_real]&COOR_BIT_SEAT)
		return 1;
	else
		return 0;
}

u8 Read_Coordinate_Data(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	u8 temp_data;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	temp_data=coordinate[ygrid_real][xgrid_real];
	return temp_data;
}

//function:读取某个坐标格是否有清扫标记但是没有墙标记
//return:0,没有，1，有
u8 Read_Coordinate_CleanNoWall(s8 xgrid,s8 ygrid)
{
	s8 xgrid_real,ygrid_real;
	u8 temp_data;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	temp_data=coordinate[ygrid_real][xgrid_real];
	if(temp_data&COOR_BIT_WALL)
		return 0;
	else if(temp_data&COOR_BIT_CLEAN)
		return 1;
	else
		return 0;
}

void Set_Coordinate_Danger(s8 gridx,s8 gridy)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(gridx);
	ygrid_real=Cal_Real_Grid(gridy);
	coordinate[ygrid_real][xgrid_real]|=COOR_BIT_DANGER;
}

u8 Read_Coordinate_Danger(s8 gridx,s8 gridy)
{
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(gridx);
	ygrid_real=Cal_Real_Grid(gridy);
	if(coordinate[ygrid_real][xgrid_real]&COOR_BIT_DANGER)
		return 1;
	else
		return 0;
}

u8 Read_Coordinate_AreaNo(s8 xgrid,s8 ygrid)
{
	u8 temp_data;
	s8 xgrid_real,ygrid_real;
	xgrid_real=Cal_Real_Grid(xgrid);
	ygrid_real=Cal_Real_Grid(ygrid);
	temp_data=coordinate[ygrid_real][xgrid_real]&AREA_NO_MASK;
	temp_data=temp_data>>3;
	return temp_data;
}

void Record_Coordinate_Intime(void)
{
	s8 gridx,gridy;
	gridx=grid.x;gridy=grid.y;

	switch(mode.mode)
		{
			case SWEEP:
				Set_Coordinate_Clean(gridx, gridy);
				if((mode.sub_mode==YBS_SUB_LEFT)|(mode.sub_mode==YBS_SUB_RIGHT))
					{
						if((mode.bump<BUMP_SEAT)&(mode.step<0x88))//&(mode.step!=0x11)&(mode.step!=0x51))
							Set_Coordinate_Wall(gridx,gridy);
					}
				break;
			case YBS:
				Set_Coordinate_Clean(gridx, gridy);
				if((mode.bump<BUMP_SEAT)&(mode.step<0x88))				//如果是超出区域的情况，另外处理
					Set_Coordinate_Wall(gridx, gridy);
				break;
			case SHIFT:			
				Set_Coordinate_Clean(gridx,gridy);
				if((mode.sub_mode==YBS_SUB_LEFT)|(mode.sub_mode==YBS_SUB_RIGHT))
					{
						if((mode.bump<BUMP_SEAT)&(mode.step<0x88))//&(mode.step!=0x11)&(mode.step!=0x51))
							Set_Coordinate_Wall(gridx,gridy);
					}
				break;
			case MODE_CTRL:
				Set_Coordinate_Clean(gridx,gridy);
				if(mode.bump)
					Set_Coordinate_Wall(gridx,gridy);
				break;
			case SPOT:
				Set_Coordinate_Clean(gridx,gridy);
				if(mode.bump)
					Set_Coordinate_Wall(gridx,gridy);
				break;
		}
}

s8 Return_Grid(short pos,short pos_base)
{
	s8 grid;
	grid=(pos-pos_base)/20;
	if(grid>GRID_MAX)
		grid=GRID_MAX;
	if(grid<GRID_MIN)
		grid=GRID_MIN;
	return grid;
}

void Cal_CoordinateXY(void)
{	
	short xpos,ypos;
	s8 temp_gridx,temp_gridy;
//	xpos=Gyro_Data.x_pos;ypos=Gyro_Data.y_pos;
	xpos=Gyro_Data.x_pos-motion1.xpos_start;
	ypos=Gyro_Data.y_pos-motion1.ypos_start;

	if(xpos>0)
		xpos+=10;
	else if(xpos<0)
		xpos-=10;
	if(ypos>0)
		ypos+=10;
	else if(ypos<0)
		ypos-=10;
	
	//grid.x=(xpos-motion1.xpos_start+10)/20;
	temp_gridx=xpos/20;
	if(temp_gridx>GRID_MAX)
		temp_gridx=GRID_MAX;
	if(temp_gridx<GRID_MIN)
		temp_gridx=GRID_MIN;
	grid.x=temp_gridx;

	//grid.y=(ypos-motion1.ypos_start+10)/20;
	temp_gridy=ypos/20;
	if(temp_gridy>GRID_MAX)
		temp_gridy=GRID_MAX;
	if(temp_gridy<GRID_MIN)
		temp_gridy=GRID_MIN;
	grid.y=temp_gridy;
}

void Cal_Xmaxmin(void)
{
	short xpos;
	xpos=Gyro_Data.x_pos;
	if(x_max[grid.y]<xpos)
		x_max[grid.y]=xpos;
	if(x_min[grid.y]>xpos)
		x_min[grid.y]=xpos;
}

void Init_Coordinate(void)
{

	for(int i=0;i<COORDI_YLENGTH+1;i++)
		{
			for(int j=0;j<COORDI_XLENGTH+1;j++)
				{
					coordinate[i][j]=0x00;
				}
		}
	TRACE("coordinate initail complete!\r\n");
}

void Logout_Area_Coordinate(void)
{
	s8 temp_gridx,temp_gridy,temp_data=0;
	s8 max_cleanx,min_cleanx;

	TRACE("LogOut Area Coordinate...\r\n");
	for(temp_gridy=grid.y_area_min;temp_gridy<=grid.y_area_max;temp_gridy++)
		{
			TRACE("gridy=%d\r\n",temp_gridy);
			max_cleanx=Return_MaxClean_GridX(temp_gridy,0);
			min_cleanx=Return_MinClean_GridX(temp_gridy,0);
			for(temp_gridx=min_cleanx;temp_gridx<=max_cleanx;temp_gridx++)
				{
					temp_data=Read_Coordinate_Data(temp_gridx,temp_gridy);
					if(temp_data)
						TRACE("coor[%d][%d]=%d\r\n",temp_gridy,temp_gridx,Read_Coordinate_Data(temp_gridx,temp_gridy));
				}
		}
		
}

u8 Judge_Yaw_Reach(short tgt_yaw,short bios_yaw)
{
 	int data1=0;
	data1=abs(Gyro_Data.yaw-tgt_yaw);
	if(data1>DEGREE_180)
		data1=DEGREE_360-data1;
	if(data1<bios_yaw)
		{
//			TRACE("yaw_dif=%d\r\n",data1);
			return 1;
		}
	else
		return 0;
}

u8 Judge_Xpos_Reach(short tgt_xpos,u8 xpos_bios)
{
	int xpos_dif;
	xpos_dif=abs(Gyro_Data.x_pos-tgt_xpos);
	if(xpos_dif<xpos_bios)
		return 1;
	else
		return 0;
}

u8 Judge_Ypos_Reach(short tgt_ypos,u8 pos_bios)
{
	int ypos_dif;
	ypos_dif=abs(Gyro_Data.y_pos-tgt_ypos);
	if(ypos_dif<pos_bios)
		return 1;
	else
		return 0;
}

u8 Judge_Pos_Reach(short tgt_xpos,short tgt_ypos)
{
	int xpos_dif,ypos_dif;
	xpos_dif=abs(Gyro_Data.x_pos-tgt_xpos);
	ypos_dif=abs(Gyro_Data.y_pos-tgt_ypos);
	if((xpos_dif<POS_BIOS)&(ypos_dif<POS_BIOS))
		return 1;
	else
		return 0;
}


u8 Judge_GridYPOS_Nearby_Reach(s8 gridy)
{
	s8 now_gridy,test_gridy1,test_gridy2;
	bool test_gridy1_enable=1,test_gridy2_enable=1;
	short tgt_ypos;

	now_gridy=gridy;
	now_gridy+=50;
	if(now_gridy>=GRIDY_POS_CNT)
		now_gridy=GRIDY_POS_CNT-1;
	if(now_gridy<0)
		now_gridy=0;
	test_gridy1=now_gridy-1;
	test_gridy2=now_gridy+1;
	
	if((test_gridy1<0)|(test_gridy1>=GRIDY_POS_CNT))
		test_gridy1_enable=0;
	if((test_gridy2<0)|(test_gridy2>=GRIDY_POS_CNT))
		test_gridy2_enable=0;

	if(test_gridy1_enable)
		{
			tgt_ypos=gridy_pos[test_gridy1];
			if((Judge_Ypos_Reach(tgt_ypos,POS_BIOS))|(Gyro_Data.y_pos<tgt_ypos))
				return 1;
		}

	if(test_gridy2_enable)
		{
			tgt_ypos=gridy_pos[test_gridy2];
			if((Judge_Ypos_Reach(tgt_ypos,POS_BIOS))|(Gyro_Data.y_pos>tgt_ypos))
				return 1;
		}
	return 0;
}

u8 Judge_GridYPOS_Reach_Large(s8 gridy)
{
	short tgt_ypos;
	tgt_ypos=Return_GridYPos_Point(gridy);

	if(Judge_Ypos_Reach(tgt_ypos,5))
		return 1;
	else
		return 0;
}

/*---------------------------------------------
FUNTCION:到达GRIDY POS坐标
INPUT:
	gridy:想要到达的目标Y坐标
	dir:（即前进角度）
		L_Angle_Const：ypos逐渐减小，ydir为-1
		R_Angle_Const：ypos逐渐增大，ydir为1
		其余：不考虑方向
OUTPUT:
	0:未到达预定坐标
	1:已到达预定坐标
----------------------------------------------*/
u8 Judge_GridYPOS_Reach(s8 gridy,short ydir)
{
	short tgt_ypos;
	tgt_ypos=Return_GridYPos_Point(gridy);

	if(Judge_Ypos_Reach(tgt_ypos,POS_BIOS))
		return 1;

	//以下为补充手段，先不启用
	if((ydir==R_Angle_Const)&(Gyro_Data.y_pos>tgt_ypos))		//方向角L_Angle_Const,从左接近，y_pos逐渐增大
		return 1;
	else if((ydir==L_Angle_Const)&(Gyro_Data.y_pos<tgt_ypos))		//方向角R_Angle_Const,从右接近，y_pos逐渐减小
		return 1;
	else
		return 0;	
}

/*---------------------------------------------
FUNTCION:到达GRIDX POS坐标
INPUT:
	gridx:想要到达的目标X坐标
	dir:（即前进角度）
		B_Angle_Const：xpos逐渐减小，xdir为-1
		F_Angle_Const：xpos逐渐增大，xdir为1
		其余：不考虑方向
OUTPUT:
	0:未到达预定坐标
	1:已到达预定坐标
----------------------------------------------*/
u8 Judge_GridXPOS_Reach(s8 gridx,short dir)
{
	short tgt_xpos;
	tgt_xpos=Return_GridXPos_Point(gridx);

	if(Judge_Xpos_Reach(tgt_xpos,POS_BIOS))
		return 1;

	//以下为补充手段，先不启用
	if((dir==F_Angle_Const)&(Gyro_Data.x_pos>tgt_xpos))		//从下接近，x_pos逐渐增大
		return 1;
	else if((dir==B_Angle_Const)&(Gyro_Data.x_pos<tgt_xpos))		//从上接近，x_pos逐渐减小
		return 1;
	else
		return 0;
	
}

short Return_GridXPos_Point(s8 gridx)
{
	s8 real_gridx;
	real_gridx=gridx+50;
	if(real_gridx>=GRIDY_POS_CNT)
		real_gridx=GRIDY_POS_CNT-1;
	return (gridx_pos[real_gridx]);
}

short Return_GridYPos_Point(s8 gridy)
{
	s8 real_gridy;
	real_gridy=gridy+50;
	if(real_gridy>=GRIDY_POS_CNT)
		real_gridy=GRIDY_POS_CNT-1;
	return (gridy_pos[real_gridy]);
}
/*------------------------------------------------
function:寻找同一个X坐标的所有Y点，清扫过的最大Y坐标
input:gridy----Y坐标
      nowall---0:该最大Y坐标点有没有墙无所谓；1:该最大Y坐标点必须没有墙标志
return:返回最大Y坐标
-------------------------------------------------*/
s8 Return_MaxClean_GridY(s8 gridx,u8 nowall)
{
	s8 temp_gridy;u8 area_num;
	for(temp_gridy=grid.y_area_max;temp_gridy!=grid.y_area_min;temp_gridy--)
		{
			if(!nowall)
				{
					if(Read_Coordinate_Clean(gridx,temp_gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(gridx,temp_gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
 				}
			else
				{
					if(Read_Coordinate_CleanNoWall(gridx,temp_gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(gridx,temp_gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
		}
	return temp_gridy;
}

/*------------------------------------------------
function:寻找同一个X坐标的所有Y点，清扫过的最小Y坐标
input:gridy----Y坐标
      nowall---0:该最小Y坐标点有没有墙无所谓；1:该最小Y坐标点必须没有墙标志
return:返回最小Y坐标
-------------------------------------------------*/
s8 Return_MinClean_GridY(s8 gridx,u8 nowall)
{
	s8 temp_gridy;u8 area_num;
	for(temp_gridy=grid.y_area_min;temp_gridy!=grid.y_area_max;temp_gridy++)
		{
			if(!nowall)
				{
					if(Read_Coordinate_Clean(gridx,temp_gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(gridx,temp_gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
			else
				{
					if(Read_Coordinate_CleanNoWall(gridx,temp_gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(gridx,temp_gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
		}
	return temp_gridy;
}

/*------------------------------------------------
function:寻找同一个Y坐标的所有X点，清扫过的最大X坐标
input:gridy----Y坐标
      nowall---0:该最大X坐标点有没有墙无所谓；1:该最大X坐标点必须没有墙标志
return:返回最大X坐标
-------------------------------------------------*/
s8 Return_MaxClean_GridX(s8 gridy,u8 nowall)
{	
	s8 temp_gridx;u8 area_num;
	for(temp_gridx=grid.x_area_max;temp_gridx!=grid.x_area_min;temp_gridx--)
		{
			if(!nowall)
				{
					if(Read_Coordinate_Clean(temp_gridx,gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(temp_gridx,gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
			else
				{
					if(Read_Coordinate_CleanNoWall(temp_gridx,gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(temp_gridx,gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
		}
	return temp_gridx;
//	return 2;
}

/*------------------------------------------------
function:寻找同一个Y坐标的所有X点，清扫过的最小X坐标
input:gridy----Y坐标
      nowall---0:该最小X坐标点有没有墙无所谓；1:该最小X坐标点必须没有墙标志
return:返回最小X坐标
-------------------------------------------------*/
s8 Return_MinClean_GridX(s8 gridy,u8 nowall)
{
	s8 temp_gridx;u8 area_num;
	for(temp_gridx=grid.x_area_min;temp_gridx!=grid.x_area_max;temp_gridx++)
		{
			if(!nowall)
				{
					if(Read_Coordinate_Clean(temp_gridx,gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(temp_gridx,gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
			else
				{
					if(Read_Coordinate_CleanNoWall(temp_gridx,gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(temp_gridx,gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
		}
	return temp_gridx;
}

s8 Return_MaxClean_AllGridy(s8 gridx,u8 nowall)
{
	s8 temp_gridy;u8 area_num;
	for(temp_gridy=GRID_MAX;temp_gridy!=GRID_MIN;temp_gridy--)
		{
			if(!nowall)
				{
					if(Read_Coordinate_Clean(gridx,temp_gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(gridx,temp_gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
			else
				{
					if(Read_Coordinate_CleanNoWall(gridx,temp_gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(gridx,temp_gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
		}
	return temp_gridy;
}

s8 Return_MinClean_AllGridY(s8 gridx,u8 nowall)
{
	s8 temp_gridy;u8 area_num;
	for(temp_gridy=GRID_MIN;temp_gridy!=GRID_MAX;temp_gridy++)
		{
			if(!nowall)
				{
					if(Read_Coordinate_Clean(gridx,temp_gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(gridx,temp_gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
			else
				{
					if(Read_Coordinate_CleanNoWall(gridx,temp_gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(gridx,temp_gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
		}
	return temp_gridy;
}

s8 Return_MaxClean_GridXAll(s8 gridy,u8 nowall)
{	
	s8 temp_gridx;u8 area_num;
	for(temp_gridx=GRID_MAX;temp_gridx!=GRID_MIN;temp_gridx--)
		{
			if(!nowall)
				{
					if(Read_Coordinate_Clean(temp_gridx,gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(temp_gridx,gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
			else
				{
					if(Read_Coordinate_CleanNoWall(temp_gridx,gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(temp_gridx,gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
		}
	return temp_gridx;
//	return 2;
}

s8 Return_MinClean_GridXAll(s8 gridy,u8 nowall)
{
	s8 temp_gridx;u8 area_num;
	for(temp_gridx=GRID_MIN;temp_gridx!=GRID_MAX;temp_gridx++)
		{
			if(!nowall)
				{
					if(Read_Coordinate_Clean(temp_gridx,gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(temp_gridx,gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
			else
				{
					if(Read_Coordinate_CleanNoWall(temp_gridx,gridy))
						{
#if 1
							area_num=Read_Coordinate_AreaNo(temp_gridx,gridy);
							if(area_num==motion1.area_num)
#endif
								break;
						}
				}
		}
	return temp_gridx;
}

/*--------------------------------------------
function:寻找下一区域的Y边界出口
input:minormax:可以从两个方向寻找 
      minormax=1,从最大Y坐标方向寻找
      minormax=0,从最小Y坐标方向寻找
output: temp_gridx:如果有出口，则返回X坐标
        0x7f:如果没有出口，则返回0x7f
/寻找方向:从grid.x_area_min,y_area_max为起点进行逆时针寻找，
		  从YMAX边界，到XMAX边界，再到YMIN边界，最后到XMIN边界
---------------------------------------------*/
s8 Analysis_Boundary_Y(u8 minormax)
{
	s8 temp_data1,temp_data2,temp_data3,result=0;
	s8 temp_gridx,temp_gridx1;
	if(minormax)					//minormax为1，YMAX边界，起点为x_area_min,y_area_max
		{
			temp_gridx=grid.x_area_min+1;
			TRACE("analysis Ymax next area entry...\r\n");
			while(temp_gridx<grid.x_area_max)
				{
					temp_data1=Return_MaxClean_GridY(temp_gridx-1,1);
					temp_data2=Return_MaxClean_GridY(temp_gridx,1);
					temp_data3=Return_MaxClean_GridY(temp_gridx+1,1);
					if((temp_data1!=grid.y_area_max)|(temp_data2!=grid.y_area_max)|(temp_data3!=grid.y_area_max))
						temp_gridx++;
					else
						{
							temp_data1=Read_Coordinate_AreaNo(temp_gridx-1, grid.y_area_max+1);
							temp_data2=Read_Coordinate_AreaNo(temp_gridx,	grid.y_area_max+1);
							temp_data3=Read_Coordinate_AreaNo(temp_gridx+1,grid.y_area_max+1);
							if((temp_data1==0)&(temp_data2==0)&(temp_data3==0))
								{
									temp_gridx1=temp_gridx+1;
									while(temp_gridx1<grid.x_area_max)
										{
											if((Return_MaxClean_GridY(temp_gridx1,1)==grid.y_area_max)&((Read_Coordinate_AreaNo(temp_gridx1,grid.y_area_max+1)==0)))
												temp_gridx1++;
											else
												break;
										}
									result=1;
									break;
								}
							else
								{
									temp_gridx++;
								}
						}
				}
			if(result)
				{
					temp_gridx=(temp_gridx1+temp_gridx-1)/2;
					TRACE("Find Ymax next area entry! gridx=%d\r\n",temp_gridx);
					return temp_gridx;
				}
			else
				{
					TRACE("Can't find Ymax next area entry!!!\r\n");
					return 0x7f;
				}
		}
	else						//minormax为0，YMIN边界，起点为x_area_max,y_area_min
		{
			temp_gridx=grid.x_area_max-1;
			TRACE("analysis Ymin next area entry...\r\n");
			while(temp_gridx>grid.x_area_min)
				{
					temp_data1=Return_MinClean_GridY(temp_gridx-1,1);
					temp_data2=Return_MinClean_GridY(temp_gridx,1);
					temp_data3=Return_MinClean_GridY(temp_gridx+1,1);
					if((temp_data1!=grid.y_area_min)|(temp_data2!=grid.y_area_min)|(temp_data3!=grid.y_area_min))
						temp_gridx--;
					else
						{
							temp_data1=Read_Coordinate_AreaNo(temp_gridx-1, grid.y_area_min-1);
							temp_data2=Read_Coordinate_AreaNo(temp_gridx,grid.y_area_min-1);
							temp_data3=Read_Coordinate_AreaNo(temp_gridx+1,grid.y_area_min-1);
							if((temp_data1==0)&(temp_data2==0)&(temp_data3==0))
								{
									temp_gridx1=temp_gridx-1;
									while(temp_gridx1>grid.x_area_min)
										{
											if((Return_MinClean_GridY(temp_gridx1,1)==grid.y_area_min)&((Read_Coordinate_AreaNo(temp_gridx1,grid.y_area_min-1)==0)))
												temp_gridx1--;
											else
												break;
										}
									result=1;
									break;
								}
							else
								{
									temp_gridx--;
								}
						}
				}
			if(result)
				{
					temp_gridx=(temp_gridx1+temp_gridx+1)/2;
					TRACE("Find Ymin next area entry! gridx=%d\r\n",temp_gridx);
					return temp_gridx;
				}
			else
				{
					TRACE("Can't find Ymin next area entry!!!\r\n");
					return 0x7f;
				}
		}
}

s8 Analysis_Boundary_Y_II(u8 minormax)
{
//	s8 temp_data2=0,temp_data3=0,temp_data4=0,temp_data5=0,result=0;
	s8 temp_gridmax1,temp_gridmax2,temp_gridmin1,temp_gridmin2;
	u8 temp_area1,temp_area2,temp_area3,temp_area4,result=0;
	s8 temp_gridx,temp_gridx1;
	CHECK_POINT temp_check_point;
	
	if(minormax)					//minormax为1，YMAX边界，起点为x_area_min,y_area_max
		{
			if(motion1.ymax_ok)
				{
					return 0x7f;
				}
			if(grid.y_area_max+2>=GRID_MAX)
				{
					motion1.ymax_ok=true;
					TRACE("ymax is near GRID_MAX,ymax area check complete!!!\r\n");
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.ymax_ok,DIR_YMAX);
#else
					Set_CurrNode_NewAreaInfo(motion1.ymax_ok, DIR_YMAX);
#endif
					return 0x7f;
				}
			if(motion1.sweep_time>=SWEEP_AREANUM_MAX)
				{
					motion1.ymax_ok=true;
					TRACE("sweep areanum is over max,ymax area check complete!!!\r\n");
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.ymax_ok,DIR_YMAX);
#else
					Set_CurrNode_NewAreaInfo(motion1.ymax_ok, DIR_YMAX);
#endif
					return 0x7f;
				}
			
			temp_gridx=grid.x_area_min;
			//TRACE("analysis Ymax next area entry...\r\n");
			while(temp_gridx<grid.x_area_max)
				{
#if 1
					temp_gridmax1=Return_MaxClean_GridY(temp_gridx,1);
					temp_gridmax2=Return_MaxClean_GridY(temp_gridx+1,1);
					if((temp_gridmax1!=grid.y_area_max)|(temp_gridmax2!=grid.y_area_max))
					//if((temp_data2!=grid.y_area_max))
						temp_gridx++;
#else
					temp_data2=Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy);
					temp_data3=Read_Coordinate_CleanNoWall(temp_gridx+1,temp_gridy);
					if((!temp_data2)|(!temp_data3))
						temp_gridy++;
#endif
					else if(Read_Coordinate_Seat(temp_gridx,grid.y_area_max))
						temp_gridx++;
					else
						{
							temp_area1=Read_Coordinate_AreaNo(temp_gridx,  grid.y_area_max+1);
							temp_area2=Read_Coordinate_AreaNo(temp_gridx+1,grid.y_area_max+1);
							temp_area3=Read_Coordinate_AreaNo(temp_gridx,  grid.y_area_max+2);
							temp_area4=Read_Coordinate_AreaNo(temp_gridx+1,grid.y_area_max+2);
							if((temp_area1==0)&(temp_area2==0)&(temp_area3==0)&(temp_area4==0))
								{
									temp_gridx1=temp_gridx;
									temp_check_point.backup_grid=0;
									while(temp_gridx1<grid.x_area_max)
										{
											temp_gridmax1=Return_MaxClean_GridY(temp_gridx1,1);
											temp_area1=Read_Coordinate_AreaNo(temp_gridx1,grid.y_area_max+1);
											temp_area2=Read_Coordinate_AreaNo(temp_gridx1,grid.y_area_max+2);
											//if((Return_MaxClean_GridY(temp_gridx1,1)==grid.y_area_max)&((Read_Coordinate_AreaNo(temp_gridx1,grid.y_area_max+1)==0)))
											if((temp_gridmax1==grid.y_area_max)&(temp_area1==0)&(temp_area2==0))
												{
													temp_gridx1++;
													temp_check_point.backup_grid++;
												}
											else
												break;
										}
									result=1;
									break;
								}
							else
								{
									temp_gridx++;
								}
						}
				}
			if(result)
				{
					temp_check_point.new_x1=temp_gridx;
					temp_check_point.new_y1=grid.y_area_max;
					temp_check_point.new_area_dir=DIR_YMAX;
					if(!Can_Entry_NewArea(&temp_check_point))
						{
							//TRACE("Can't find Ymax next area entry!!!\r\n");
							TRACE("YMAX area check complete!!!\r\n");
							motion1.ymax_ok=true;
#ifdef USE_AREA_TREE
							Set_Curr_AreaTree_NewInfo(motion1.ymax_ok,DIR_YMAX);
#else
							Set_CurrNode_NewAreaInfo(motion1.ymax_ok, DIR_YMAX);
#endif
							return 0x7f;
						}
					temp_gridx=(temp_gridx1+temp_gridx)/2;
					check_point.backup_grid=temp_check_point.backup_grid;
					return temp_gridx;
				}
			else
				{
					//TRACE("Can't find Ymax next area entry!!!\r\n");
					TRACE("YMAX area check complete!!!\r\n");
					motion1.ymax_ok=true;
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.ymax_ok,DIR_YMAX);
#else
					Set_CurrNode_NewAreaInfo(motion1.ymax_ok, DIR_YMAX);
#endif
					return 0x7f;
				}
		}
	else						//minormax为0，YMIN边界，起点为x_area_max,y_area_min
		{
			if(motion1.ymin_ok)
				return 0x7f;
			if(grid.y_area_min-2<=GRID_MIN)
				{
					motion1.ymin_ok=true;
					TRACE("ymin is near GRID_MIN,ymin area check complete!!!\r\n");
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.ymin_ok,DIR_YMIN);
#else
					Set_CurrNode_NewAreaInfo(motion1.ymin_ok, DIR_YMIN);
#endif
					return 0x7f;
				}
			if(motion1.sweep_time>=SWEEP_AREANUM_MAX)
				{
					motion1.ymin_ok=true;
					TRACE("sweep areanum is over max,ymin area check complete!!!\r\n");
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.ymin_ok,DIR_YMIN);
#else
					Set_CurrNode_NewAreaInfo(motion1.ymin_ok, DIR_YMIN);
#endif
					return 0x7f;
				}
			
			temp_gridx=grid.x_area_max;
			//TRACE("analysis Ymin next area entry...\r\n");
			while(temp_gridx>grid.x_area_min)
				{
#if 1
					temp_gridmin1=Return_MinClean_GridY(temp_gridx,1);
					temp_gridmin2=Return_MinClean_GridY(temp_gridx-1,1);
					if((temp_gridmin1!=grid.y_area_min)|(temp_gridmin2!=grid.y_area_min))
					//if((temp_data2!=grid.y_area_min))
						temp_gridx--;
#else
					temp_data2=Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy);
					temp_data3=Read_Coordinate_CleanNoWall(temp_gridx-1,temp_gridy);
					if((!temp_data2)|(!temp_data3))
						temp_gridy++;
#endif
					else if(Read_Coordinate_Seat(temp_gridx,grid.y_area_min))
						temp_gridx--;
					else
						{
							temp_area1=Read_Coordinate_AreaNo(temp_gridx,  grid.y_area_min-1);
							temp_area2=Read_Coordinate_AreaNo(temp_gridx-1,grid.y_area_min-1);
							temp_area3=Read_Coordinate_AreaNo(temp_gridx,  grid.y_area_min-2);
							temp_area4=Read_Coordinate_AreaNo(temp_gridx-1,grid.y_area_min-2);
							if((temp_area1==0)&(temp_area2==0)&(temp_area3==0)&(temp_area4==0))
							//if((temp_data2==0)&(temp_data4==0))
								{
									temp_gridx1=temp_gridx;
									temp_check_point.backup_grid=0;
									while(temp_gridx1>grid.x_area_min)
										{
											temp_gridmin1=Return_MinClean_GridY(temp_gridx1,1);
											temp_area1=Read_Coordinate_AreaNo(temp_gridx1,grid.y_area_min-1);
											temp_area2=Read_Coordinate_AreaNo(temp_gridx1,grid.y_area_min-2);
											//if((Return_MinClean_GridY(temp_gridx1,1)==grid.y_area_min)&((Read_Coordinate_AreaNo(temp_gridx1,grid.y_area_min-1)==0)))
											if((temp_gridmin1==grid.y_area_min)&(temp_area1==0)&(temp_area2==0))
												{
													temp_gridx1--;
													temp_check_point.backup_grid++;
												}
											else
												break;
										}
									result=1;
									break;
								}
							else
								{
									temp_gridx--;
								}
						}
				}
			if(result)
				{	
					temp_check_point.new_x1=temp_gridx;
					temp_check_point.new_y1=grid.y_area_min;
					temp_check_point.new_area_dir=DIR_YMIN;
					if(!Can_Entry_NewArea(&temp_check_point))
						{
							//TRACE("Can't find Ymin next area entry!!!\r\n");
							TRACE("YMIN area check complete!!!\r\n");
							motion1.ymin_ok=true;
#ifdef USE_AREA_TREE
							Set_Curr_AreaTree_NewInfo(motion1.ymin_ok,DIR_YMIN);
#else
							Set_CurrNode_NewAreaInfo(motion1.ymin_ok, DIR_YMIN);
#endif
							return 0x7f;
						}
					temp_gridx=(temp_gridx1+temp_gridx)/2;
					//temp_gridx=(temp_gridx1+temp_gridx-1)/2;
					check_point.backup_grid=temp_check_point.backup_grid;
					return temp_gridx;
				}
			else
				{
					//TRACE("Can't find Ymin next area entry!!!\r\n");
					TRACE("YMIN area check complete!!!\r\n");
					motion1.ymin_ok=true;
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.ymin_ok,DIR_YMIN);
#else
					Set_CurrNode_NewAreaInfo(motion1.ymin_ok, DIR_YMIN);
#endif
					return 0x7f;
				}
		}
}

/*--------------------------------------------
function:寻找下一区域的X边界出口
input:minormax:可以从两个方向寻找 
      minormax=1,从最大X坐标方向寻找
      minormax=0,从最小X坐标方向寻找
output: temp_gridx:如果有出口，则返回Y坐标
        0x7f:如果没有出口，则返回0x7f
寻找方向:从grid.x_area_min,y_area_max为起点进行逆时针寻找，
		 从YMAX边界，到XMAX边界，再到YMIN边界，最后到XMIN边界

v1.2.1:修改为不管是Xmax还是Xmin,都从Ymin处开始检查,且Y轴清扫方向都为正向

---------------------------------------------*/
u8 Analysis_Boundary_X(u8 minormax)
{
	s8 temp_data1,temp_data2,temp_data3,result=0;
	s8 temp_gridy;

#if 0
	if(minormax)					//minormax为1，XMAX边界，起点为x_area_max,y_area_max
		{
			temp_gridy=grid.y_area_max-1;
			TRACE("analysis Xmax next area entry...\r\n");			
			while(temp_gridy>grid.y_area_min)
				{
					temp_data1=Return_MaxClean_GridX(temp_gridy-1,1);
					temp_data2=Return_MaxClean_GridX(temp_gridy,1);
					temp_data3=Return_MaxClean_GridX(temp_gridy+1,1);
					if((temp_data1!=grid.x_area_max)|(temp_data2!=grid.x_area_max)|(temp_data3!=grid.x_area_max))
						temp_gridy--;
					else
						{
							temp_data1=Read_Coordinate_AreaNo(grid.x_area_max+1, temp_gridy-1);
							temp_data2=Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy);
							temp_data3=Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy+1);
							if((temp_data1==0)&(temp_data2==0)&(temp_data3==0))
								{
									temp_gridy1=temp_gridy-1;
									while(temp_gridy1>grid.y_area_min)
										{
											if((Return_MaxClean_GridX(temp_gridy1,1)==grid.x_area_max)&((Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy1)==0)))
												temp_gridy1--;
											else
												break;
										}
									result=1;
									break;
								}
							else
								{
									temp_gridy--;
								}
						}
				}
			if(result)
				{
					temp_gridy=(temp_gridy1+temp_gridy+1)/2;
					TRACE("Find Xmax next area entry! gridy=%d\r\n",temp_gridy);
					return temp_gridy;
				}
			else
				{
					TRACE("Can't find Xmax next area entry!!!\r\n");
					return 0x7f;
				}
		}
	else							//minormax为0，XMIN边界，起点为x_area_min,y_area_min
		{
			temp_gridy=grid.y_area_min+1;
			TRACE("analysis Xmin next area entry...\r\n");			
			while(temp_gridy<grid.y_area_max)
				{
					temp_data1=Return_MinClean_GridX(temp_gridy-1,1);
					temp_data2=Return_MinClean_GridX(temp_gridy,1);
					temp_data3=Return_MinClean_GridX(temp_gridy+1,1);
					if((temp_data1!=grid.x_area_min)|(temp_data2!=grid.x_area_min)|(temp_data3!=grid.x_area_min))
						temp_gridy++;
					else
						{
							temp_data1=Read_Coordinate_AreaNo(grid.x_area_min-1, temp_gridy-1);
							temp_data2=Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy);
							temp_data3=Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy+1);
							if((temp_data1==0)&(temp_data2==0)&(temp_data3==0))
								{
									temp_gridy1=temp_gridy+1;
									while(temp_gridy1<grid.y_area_max)
										{
											if((Return_MaxClean_GridX(temp_gridy1,1)==grid.x_area_min)&((Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy1)==0)))
												temp_gridy1++;
											else
												break;
										}
									result=1;
									break;
								}
							else
								{
									temp_gridy++;
								}
						}
				}
			if(result)
				{
					temp_gridy=(temp_gridy1+temp_gridy-1)/2;
					TRACE("Find Xmin next area entry! gridy=%d\r\n",temp_gridy);
					return temp_gridy;
				}
			else
				{
					TRACE("Can't find Xmin next area entry!!!\r\n");
					return 0x7f;
				}
		}
#else
	if(minormax)					//minormax为1，XMAX边界，起点为x_area_max,y_area_max
		{
			temp_gridy=grid.y_area_min+1;
			TRACE("analysis Xmax next area entry...\r\n");			
			while(temp_gridy<grid.y_area_max)
				{
					temp_data1=Return_MaxClean_GridX(temp_gridy-1,1);
					temp_data2=Return_MaxClean_GridX(temp_gridy,1);
					temp_data3=Return_MaxClean_GridX(temp_gridy+1,1);
					if((temp_data1!=grid.x_area_max)|(temp_data2!=grid.x_area_max)|(temp_data3!=grid.x_area_max))
						temp_gridy++;
					else
						{
							temp_data1=Read_Coordinate_AreaNo(grid.x_area_max+1, temp_gridy-1);
							temp_data2=Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy);
							temp_data3=Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy+1);
							if((temp_data1==0)&(temp_data2==0)&(temp_data3==0))
								{
#if 0
									temp_gridy1=temp_gridy-1;
									while(temp_gridy1>grid.y_area_min)
										{
											if((Return_MaxClean_GridX(temp_gridy1,1)==grid.x_area_max)&((Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy1)==0)))
												temp_gridy1--;
											else
												break;
										}
#endif
									result=1;
									break;
								}
							else
								{
									temp_gridy++;
								}
						}
				}
			if(result)
				{
					//temp_gridy=(temp_gridy1+temp_gridy+1)/2;
					TRACE("Find Xmax next area entry! gridy=%d\r\n",temp_gridy);
					return temp_gridy;
				}
			else
				{
					TRACE("Can't find Xmax next area entry!!!\r\n");
					return 0x7f;
				}
		}
	else							//minormax为0，XMIN边界，起点为x_area_min,y_area_min
		{
			temp_gridy=grid.y_area_min+1;
			TRACE("analysis Xmin next area entry...\r\n");			
			while(temp_gridy<grid.y_area_max)
				{
					temp_data1=Return_MinClean_GridX(temp_gridy-1,1);
					temp_data2=Return_MinClean_GridX(temp_gridy,1);
					temp_data3=Return_MinClean_GridX(temp_gridy+1,1);
					if((temp_data1!=grid.x_area_min)|(temp_data2!=grid.x_area_min)|(temp_data3!=grid.x_area_min))
						temp_gridy++;
					else
						{
							temp_data1=Read_Coordinate_AreaNo(grid.x_area_min-1, temp_gridy-1);
							temp_data2=Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy);
							temp_data3=Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy+1);
							if((temp_data1==0)&(temp_data2==0)&(temp_data3==0))
								{
#if 0
									temp_gridy1=temp_gridy+1;
									while(temp_gridy1<grid.y_area_max)
										{
											if((Return_MaxClean_GridX(temp_gridy1,1)==grid.x_area_min)&((Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy1)==0)))
												temp_gridy1++;
											else
												break;
										}
#endif
									result=1;
									break;
								}
							else
								{
									temp_gridy++;
								}
						}
				}
			if(result)
				{
					//temp_gridy=(temp_gridy1+temp_gridy-1)/2;
					TRACE("Find Xmin next area entry! gridy=%d\r\n",temp_gridy);
					return temp_gridy;
				}
			else
				{
					TRACE("Can't find Xmin next area entry!!!\r\n");
					return 0x7f;
				}
		}
#endif
}

u8 Analysis_Boundary_X_II(u8 minormax)
{
//	s8 temp_data2=0,temp_data3=0,temp_data4=0,temp_data5=0,result=0;
	s8 temp_gridmax1,temp_gridmax2;
	u8 temp_area1,temp_area2,temp_area3,temp_area4,result=0;
	s8 temp_gridy,temp_gridy1;
	CHECK_POINT temp_check_point;
	
	if(minormax)					//minormax为1，XMAX边界，起点为x_area_max,y_area_max
		{
			if(motion1.xmax_ok)
				return 0x7f;
			if(grid.x_area_max+2>=GRID_MAX)
				{
					motion1.xmax_ok=true;
					TRACE("xmax is near GRID_MAX,xmax area check complete!!!\r\n");
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.xmax_ok,DIR_XMAX);
#else
					Set_CurrNode_NewAreaInfo(motion1.xmax_ok, DIR_XMAX);
#endif
					return 0x7f;
				}
			if(motion1.sweep_time>=SWEEP_AREANUM_MAX)
				{
					motion1.xmax_ok=true;
					TRACE("sweep areanum is over max,Xmax area check complete!!!\r\n");
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.xmax_ok,DIR_XMAX);
#else
					Set_CurrNode_NewAreaInfo(motion1.xmax_ok, DIR_XMAX);
#endif
					return 0x7f;
				}
			
			temp_gridy=grid.y_area_max;//temp_gridy=grid.y_area_min;			
			while(temp_gridy>grid.y_area_min)//while(temp_gridy<grid.y_area_max)
				{
#if 1
					temp_gridmax1=Return_MaxClean_GridX(temp_gridy,1);
					temp_gridmax2=Return_MaxClean_GridX(temp_gridy-1,1);//temp_gridmax2=Return_MaxClean_GridX(temp_gridy+1,1);
					if((temp_gridmax1!=grid.x_area_max)|(temp_gridmax2!=grid.x_area_max))
					//if((temp_data2!=grid.x_area_max))
						temp_gridy--;//temp_gridy++;
#else
					temp_data2=Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy);
					temp_data3=Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy+1);
					if((!temp_data2)|(!temp_data3))
						temp_gridy++;
#endif
					else if(Read_Coordinate_Seat(grid.x_area_max,temp_gridy))
						temp_gridy--;//temp_gridy++;
					else
						{
							temp_area1=Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy);
							temp_area2=Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy-1);//temp_area2=Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy+1);
							temp_area3=Read_Coordinate_AreaNo(grid.x_area_max+2,temp_gridy);
							temp_area4=Read_Coordinate_AreaNo(grid.x_area_max+2,temp_gridy-1);//temp_area4=Read_Coordinate_AreaNo(grid.x_area_max+2,temp_gridy+1);
							if((temp_area1==0)&(temp_area2==0)&(temp_area3==0)&(temp_area4==0))
							//if((temp_data2==0)&(temp_data4==0))
								{
#if 1
									temp_gridy1=temp_gridy;
									temp_check_point.backup_grid=0;
									while(temp_gridy1>grid.y_area_min)//while(temp_gridy1<grid.y_area_max)
										{
											temp_area1=Read_Coordinate_AreaNo(grid.x_area_max+1,temp_gridy1);
											temp_area2=Read_Coordinate_AreaNo(grid.x_area_max+2,temp_gridy1);
											temp_gridmax1=Return_MaxClean_GridX(temp_gridy1,1);
											if((temp_gridmax1==grid.x_area_max)&(temp_area1==0)&(temp_area2==0))
												{
													temp_gridy1--;//temp_gridy1++;
													temp_check_point.backup_grid++;
												}
											else
												{
													break;
												}
										}
#endif
									result=1;
									break;
								}
							else
								{
									temp_gridy--;//temp_gridy++;
								}
						}
				}
			if(result)
				{
					temp_check_point.new_x1=grid.x_area_max;
					temp_check_point.new_y1=temp_gridy;
					temp_check_point.new_area_dir=DIR_XMAX;
					if(!Can_Entry_NewArea(&temp_check_point))
						{
							//TRACE("Can't find Xmax next area entry!!!\r\n");
							TRACE("XMAX area check complete!!!\r\n");
							motion1.xmax_ok=true;
#ifdef USE_AREA_TREE
							Set_Curr_AreaTree_NewInfo(motion1.xmax_ok,DIR_XMAX);
#else
							Set_CurrNode_NewAreaInfo(motion1.xmax_ok, DIR_XMAX);
#endif
							return 0x7f;
						}
					temp_gridy=(temp_gridy+temp_gridy1)/2;
					check_point.backup_grid=temp_check_point.backup_grid;
					//temp_gridy=temp_gridy+1;
					return temp_gridy;
				}
			else
				{
					//TRACE("Can't find Xmax next area entry!!!\r\n");
					TRACE("XMAX area check complete!!!\r\n");
					motion1.xmax_ok=true;
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.xmax_ok,DIR_XMAX);
#else
					Set_CurrNode_NewAreaInfo(motion1.xmax_ok, DIR_XMAX);
#endif
					return 0x7f;
				}
		}
	else							//minormax为0，XMIN边界，起点为x_area_min,y_area_min
		{
			if(motion1.xmin_ok)
				return 0x7f;
			if(grid.x_area_min-2<=GRID_MIN)
				{
					motion1.xmin_ok=true;
					TRACE("xmin is near GRID_MIN,xmin area check complete!!!\r\n");
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.xmin_ok,DIR_XMIN);
#else
					Set_CurrNode_NewAreaInfo(motion1.xmin_ok, DIR_XMIN);
#endif
					return 0x7f;
				}
			if(motion1.sweep_time>=SWEEP_AREANUM_MAX)
				{
					motion1.xmin_ok=true;
					TRACE("sweep areanum is over max,xmin area check complete!!!\r\n");
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.xmin_ok,DIR_XMIN);
#else
					Set_CurrNode_NewAreaInfo(motion1.xmin_ok, DIR_XMIN);
#endif
					return 0x7f;
				}
			
			temp_gridy=grid.y_area_min;
			//TRACE("analysis Xmin next area entry...\r\n");			
			while(temp_gridy<grid.y_area_max)
				{
#if 1
					temp_gridmax1=Return_MinClean_GridX(temp_gridy,1);
					temp_gridmax2=Return_MinClean_GridX(temp_gridy+1,1);
					if((temp_gridmax1!=grid.x_area_min)|(temp_gridmax2!=grid.x_area_min))
					//if((temp_data2!=grid.x_area_min))
						temp_gridy++;
#else
					temp_data2=Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy);
					temp_data3=Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy+1);
					if((!temp_data2)|(!temp_data3))
						temp_gridy++;
#endif
					else if(Read_Coordinate_Seat(grid.x_area_min,temp_gridy))
						temp_gridy++;
					else
						{
							temp_area1=Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy);
							temp_area2=Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy+1);
							temp_area3=Read_Coordinate_AreaNo(grid.x_area_min-2,temp_gridy);
							temp_area4=Read_Coordinate_AreaNo(grid.x_area_min-2,temp_gridy+1);
							if((temp_area1==0)&(temp_area2==0)&(temp_area3==0)&(temp_area4==0))
								{
#if 1
									temp_gridy1=temp_gridy;
									temp_check_point.backup_grid=0;
									while(temp_gridy1<grid.y_area_max)
										{
											temp_area1=Read_Coordinate_AreaNo(grid.x_area_min-1,temp_gridy1);
											temp_area2=Read_Coordinate_AreaNo(grid.x_area_min-2,temp_gridy1);
											temp_gridmax1=Return_MinClean_GridX(temp_gridy1,1);
											if((temp_gridmax1==grid.x_area_min)&(temp_area1==0)&(temp_area2==0))
												{
													temp_gridy1++;
													temp_check_point.backup_grid++;
												}
											else
												{
													break;
												}
										}
#endif
									result=1;
									break;
								}
							else
								{
									temp_gridy++;
								}
						}
				}
			if(result)
				{
					temp_check_point.new_x1=grid.x_area_min;
					temp_check_point.new_y1=temp_gridy;
					temp_check_point.new_area_dir=DIR_XMIN;
					if(!Can_Entry_NewArea(&temp_check_point))
						{
							//TRACE("Can't find Xmin next area entry!!!\r\n");
							TRACE("XMIN area check complete!!!\r\n");
							motion1.xmin_ok=true;
#ifdef USE_AREA_TREE
							Set_Curr_AreaTree_NewInfo(motion1.xmin_ok,DIR_XMIN);
#else
							Set_CurrNode_NewAreaInfo(motion1.xmin_ok, DIR_XMIN);
#endif
							return 0x7f;
						}
					//temp_gridy=temp_gridy+1;
					temp_gridy=(temp_gridy+temp_gridy1)/2;
					check_point.backup_grid=temp_check_point.backup_grid;
					return temp_gridy;
				}
			else
				{
					//TRACE("Can't find Xmin next area entry!!!\r\n");
					TRACE("XMIN area check complete!!!\r\n");
					motion1.xmin_ok=true;
#ifdef USE_AREA_TREE
					Set_Curr_AreaTree_NewInfo(motion1.xmin_ok,DIR_XMIN);
#else
					Set_CurrNode_NewAreaInfo(motion1.xmin_ok, DIR_XMIN);
#endif
					return 0x7f;
				}
		}
}



void Reset_CheckPoint_NextAction(void)
{
	check_point.next_action=CHECK_NORMALSWEEP;
}

void Set_CheckPoint_NextAction(u8 next_action)
{

	check_point.next_action=next_action;
}

u8 Read_CheckPoint_NextAction(void)
{
	u8 data=0;
	data=check_point.next_action;
	return data;
}

void Init_CheckPoint(void)
{
	check_point.yacc_anti=false;
	check_point.ydir=0;
	check_point.use_pathpoint=false;
	Reset_CheckPoint_NextAction();
	check_point.ybs_dir=0;
	check_point.backup_grid=0;
}

void Logout_CheckPoint(void)
{
	TRACE("check_point.new_x1=%d\r\n",check_point.new_x1);
	TRACE("check_point.new_y1=%d\r\n",check_point.new_y1);
	TRACE("check_point.new_x2=%d\r\n",check_point.new_x2);
	TRACE("check_point.new_y2=%d\r\n",check_point.new_y2);
	TRACE("check_point.next_action=%d\r\n",check_point.next_action);
	TRACE("check_point.ydir=%d\r\n",check_point.ydir);
	TRACE("check_point.ybs_dir=%d\r\n",check_point.ybs_dir);
//	TRACE("check_point.next_area=%d\r\n",check_point.next_area);
//	TRACE("check_point.leak_sweep=%d\r\n",check_point.leak_sweep);
//	if(check_point.go_exit|check_point.next_area)
	if(Read_CheckPoint_NextAction()>2)
		{
			TRACE("check_point.x_acc=%d\r\n",check_point.next_xacc);
			TRACE("check_point.y_acc=%d\r\n",check_point.next_yacc);
			TRACE("check_point.tgtyaw=%d\r\n",check_point.next_tgtyaw);			
		}
}

u8 Analysis_Leak_Point(void)
{
	s8 max_cleanx1,min_cleanx1,max_cleanx2,min_cleanx2,check_gridy1,check_gridy2,check_gridy3;
	s8 data1=0,temp_gridx,temp1_gridx;
	max_cleanx1=check_point.max_cleanx1;min_cleanx1=check_point.min_cleanx1;check_gridy1=check_point.y1;
	max_cleanx2=check_point.max_cleanx2;min_cleanx2=check_point.min_cleanx2;check_gridy2=check_point.y2;
	s8 start_checky=grid.y;
	u8 check_result=0;

	check_point.backup_grid=0;
	///////////先进行max min检查///////////////////
	///////////先进行max min检查///////////////////
	if((Read_Coordinate_Seat(max_cleanx1,check_gridy1))|(Read_Coordinate_Seat(max_cleanx2,check_gridy2)))
		{
			goto LEAK_POINT_MIN_CHECK;
		}
	data1=abs(max_cleanx1-max_cleanx2);				//两个相邻Y坐标的X轴方向最大清扫坐标比较
	if(data1>2)										//如果两个相邻Y坐标的X轴方向最大清扫坐标差距大于2个坐标格（40cm）
		{
			if(max_cleanx1>max_cleanx2)				//如果检查点X1大于相邻检查点X2
				{
					for(temp_gridx=max_cleanx2+1;temp_gridx<max_cleanx1;temp_gridx++)
						{
							if((Read_Coordinate_CleanNoWall(temp_gridx,check_gridy1))&(Read_Coordinate_Clean(temp_gridx+1,check_gridy1)))
								{
									//if(check_point->y2<start_checky)
									if(check_point.y2<check_point.y1)
										{
											check_point.ydir=0;
											check_point.ybs_dir=LEFT;
										}
									else
										{
											check_point.ydir=1;
											check_point.ybs_dir=RIGHT;
										}

									if(Analysis_Fake_Leak(temp_gridx,check_gridy1,check_point.ydir))
										{
											continue;
										}
									//if(Can_Entry_Point(temp_gridx, check_gridy1, check_point->ydir))
										{
		//									temp1_gridx=temp_gridx+1;
											temp1_gridx=temp_gridx;
											while(temp1_gridx<max_cleanx1)
												{
													check_result=1;
													if(Read_Coordinate_Clean(temp1_gridx,check_gridy2))
														{
															check_result=0;
															TRACE("grid.x=%d .y=%d has clean\r\n",temp1_gridx,check_gridy2);
															break;
														}
													
													if(Read_Coordinate_CleanNoWall(temp1_gridx,check_gridy1))
														{
															check_point.backup_grid++;
															temp1_gridx++;
														}
													else
														{
															check_result=1;
															break;
														}
												}

											if(check_result)
												{
				//									temp_gridx=(temp_gridx+temp1_gridx)/2;
													check_point.new_x1=temp_gridx;
													check_point.new_y1=check_gridy1;
													check_point.new_x2=temp_gridx;
													check_point.new_y2=check_gridy2;
													check_point.next_tgtyaw=F_Angle_Const;
													if(Can_Entry_Point())
														return 1;
												}
										}
								}
						}
				}
			else									//如果检查点X1小于相邻检查点X2
				{
					for(temp_gridx=max_cleanx1+1;temp_gridx<max_cleanx2;temp_gridx++)
						{
							if((Read_Coordinate_CleanNoWall(temp_gridx,check_gridy2))&(Read_Coordinate_Clean(temp_gridx+1,check_gridy2)))
								{
									//if(check_point->y2<start_checky)
									if(check_point.y2<check_point.y1)
										{
											check_point.ydir=1;
											check_point.ybs_dir=RIGHT;
										}
									else
										{
											check_point.ydir=0;
											check_point.ybs_dir=LEFT;
										}

									if(Analysis_Fake_Leak(temp_gridx,check_gridy2,check_point.ydir))
										{
											continue;
										}
									//if(Can_Entry_Point(temp_gridx, check_gridy2, check_point->ydir))
										{
		//									temp1_gridx=temp_gridx+1;
											temp1_gridx=temp_gridx;
											while(temp1_gridx<max_cleanx2)
												{
													check_result=1;
													if(Read_Coordinate_Clean(temp1_gridx,check_gridy1))
														{
															TRACE("grid.x=%d .y=%d has clean\r\n",temp1_gridx,check_gridy1);
															check_result=0;
															break;
														}
													if(Read_Coordinate_CleanNoWall(temp1_gridx,check_gridy2))
														{
															temp1_gridx++;
															check_point.backup_grid++;
														}
													else
														{
															check_result=1;
															break;
														}
												}
											
											if(check_result)
												{
				//									temp_gridx=(temp_gridx+temp1_gridx)/2;
													check_point.new_x1=temp_gridx;
													check_point.new_y1=check_gridy2;
													check_point.new_x2=temp_gridx;
													check_point.new_y2=check_gridy1;
													check_point.next_tgtyaw=F_Angle_Const;
													if(Can_Entry_Point())
														return 1;
												}
										}
								}
						}
				}
		}
	
LEAK_POINT_MIN_CHECK:
	if((Read_Coordinate_Seat(min_cleanx1,check_gridy1))|(Read_Coordinate_Seat(min_cleanx2,check_gridy2)))
		{
			return 0;
		}
	data1=abs(min_cleanx1-min_cleanx2);				//两个相邻Y坐标的X轴方向最小清扫坐标比较
	if(data1>2)										//如果两个相邻Y坐标的X轴方向最小清扫坐标差距大于2个坐标格（40cm）
		{
			if(min_cleanx1>min_cleanx2) 			//如果检查点X大于相邻检查点X
				{
					for(temp_gridx=min_cleanx1-1;temp_gridx>min_cleanx2;temp_gridx--)
						{
							if((Read_Coordinate_CleanNoWall(temp_gridx,check_gridy2))&(Read_Coordinate_Clean(temp_gridx-1,check_gridy2)))
								{
									//if(check_point->y2<start_checky)
									if(check_point.y2<check_point.y1)
										{
											check_point.ydir=1;
											check_point.ybs_dir=LEFT;
										}
									else
										{
											check_point.ydir=0;
											check_point.ybs_dir=RIGHT;
										}

									if(Analysis_Fake_Leak(temp_gridx,check_gridy2,check_point.ydir))
										{
											continue;
										}
									//if(Can_Entry_Point(temp_gridx, check_gridy2, check_point->ydir))
										{
		//									temp1_gridx=temp_gridx-1;
											temp1_gridx=temp_gridx;
											while(temp1_gridx>min_cleanx2)
												{
													check_result=1;
													if(Read_Coordinate_Clean(temp1_gridx,check_gridy1))
														{
															TRACE("grid.x=%d .y=%d has clean\r\n",temp1_gridx,check_gridy1);
															check_result=0;
															break;
														}
												
													if(Read_Coordinate_CleanNoWall(temp1_gridx,check_gridy2))
														{
															temp1_gridx--;
															check_point.backup_grid--;
														}
													else
														{
															check_result=1;
															break;
														}
												}
											if(check_result)
												{
				//									temp_gridx=(temp_gridx+temp1_gridx)/2;
													check_point.new_x1=temp_gridx;
													check_point.new_y1=check_gridy2;
													check_point.new_x2=temp_gridx;
													check_point.new_y2=check_gridy1;
													check_point.next_tgtyaw=B_Angle_Const;
													if(Can_Entry_Point())
														return 1;
												}
										}
								}
						}
				}
			else									//如果检查点X小于相邻检查点X
				{
					for(temp_gridx=min_cleanx2-1;temp_gridx>min_cleanx1;temp_gridx--)
						{
							if((Read_Coordinate_CleanNoWall(temp_gridx,check_gridy1))&(Read_Coordinate_Clean(temp_gridx-1,check_gridy1)))
								{
									//if(check_point->y2<start_checky)
									if(check_point.y2<check_point.y1)
										{
											check_point.ydir=0;
											check_point.ybs_dir=RIGHT;
										}
									else
										{
											check_point.ydir=1;
											check_point.ybs_dir=LEFT;
										}

									if(Analysis_Fake_Leak(temp_gridx,check_gridy1,check_point.ydir))
										{
											continue;
										}
									//if(Can_Entry_Point(temp_gridx, check_gridy1, check_point->ydir))
										{
		//									temp1_gridx=temp_gridx-1;
											temp1_gridx=temp_gridx;
											while(temp1_gridx>min_cleanx1)
												{
													check_result=1;
													if(Read_Coordinate_Clean(temp1_gridx,check_gridy2))
														{
															TRACE("grid.x=%d .y=%d has clean\r\n",temp1_gridx,check_gridy2);
															check_result=0;
															break;
														}
												
													if(Read_Coordinate_CleanNoWall(temp1_gridx,check_gridy1))
														{
															temp1_gridx--;
															check_point.backup_grid--;
														}
													else
														{
															check_result=1;
															break;
														}
												}
											if(check_result)
												{
				//									temp_gridx=(temp_gridx+temp1_gridx)/2;
													check_point.new_x1=temp_gridx;
													check_point.new_y1=check_gridy1;
													check_point.new_x2=temp_gridx;
													check_point.new_y2=check_gridy2;
													check_point.next_tgtyaw=B_Angle_Const;
													if(Can_Entry_Point())
														return 1;
												}
										}
								}
						}
				}
		}

#if 0
	////////////再进行中间漏扫检查/////////////
	////////////再进行中间漏扫检查/////////////
	if(check_gridy1>check_gridy2)
		{
			check_gridy3=check_gridy1+1;
			if(check_gridy3>grid.y_area_max)
				check_gridy3=grid.y_area_max;
		}
	else
		{
			check_gridy3=check_gridy1-1;
			if(check_gridy3<grid.y_area_min)
				check_gridy3=grid.y_area_min;
		}
	for(temp_gridx=min_cleanx1+1;temp_gridx<max_cleanx1;temp_gridx++)
		{
			if((!Read_Coordinate_Clean(temp_gridx-1,check_gridy1))&(!Read_Coordinate_Clean(temp_gridx,check_gridy1))&(!Read_Coordinate_Clean(temp_gridx+1,check_gridy1)))
				{
					if((!Read_Coordinate_Clean(temp_gridx-1,check_gridy3))&(!Read_Coordinate_Clean(temp_gridx,check_gridy3))&(!Read_Coordinate_Clean(temp_gridx+1,check_gridy3)))
					{
					if((Read_Coordinate_CleanNoWall(temp_gridx-1,check_gridy2))&(Read_Coordinate_CleanNoWall(temp_gridx,check_gridy2))&(Read_Coordinate_CleanNoWall(temp_gridx+1,check_gridy2)))
						{
							check_point->new_x1=temp_gridx;
							check_point->new_y1=check_gridy2;
							check_point->new_x2=temp_gridx;
							check_point->new_y2=check_gridy1;
							check_point->next_tgtyaw=B_Angle_Const;
							if(check_point->y2<start_checky)
								check_point->ydir=1;
							else
								check_point->ydir=0;
							return 1;
						}
					}
				}
		}
#endif

#if 0
	if(check_gridy2>check_gridy1)
		{
			check_gridy3=check_gridy2+1;
			if(check_gridy3>grid.y_area_max)
				check_gridy3=grid.y_area_max;
		}
	else
		{
			check_gridy3=check_gridy2-1;
			if(check_gridy3<grid.y_area_min)
				check_gridy3=grid.y_area_min;
		}
	for(temp_gridx=min_cleanx1+1;temp_gridx<max_cleanx1;temp_gridx++)
		{
			if((!Read_Coordinate_Clean(temp_gridx,check_gridy2))&(!Read_Coordinate_Clean(temp1_gridx+1,check_gridy2))&(!Read_Coordinate_Clean(temp_gridx+2,check_gridy2)))
				{
					if((!Read_Coordinate_Clean(temp_gridx,check_gridy3))&(!Read_Coordinate_Clean(temp_gridx+1,check_gridy3))&(!Read_Coordinate_Clean(temp_gridx+2,check_gridy3)))
					{
						if((Read_Coordinate_CleanNoWall(temp_gridx,check_gridy1))&(Read_Coordinate_CleanNoWall(temp_gridx+1,check_gridy1))&(Read_Coordinate_CleanNoWall(temp_gridx+2,check_gridy1)))
							{
								TRACE("Find Mid Leak!!!\r\n");
								check_point.new_x1=temp_gridx;
								check_point.new_y1=check_gridy1;
								check_point.new_x2=temp_gridx;
								check_point.new_y2=check_gridy2;
								check_point.backup_grid=3;
								check_point.next_tgtyaw=F_Angle_Const;
								if(check_point.y2<check_point.y1)
									check_point.ydir=0;
								else
									check_point.ydir=1;
								if(Can_Entry_Point())
									{
										check_point.new_x1=temp_gridx+1;
										check_point.new_x2=temp_gridx+1;
										return 1;
									}
							}
					}
				}
		}
#endif

#if 1
	check_point.backup_grid=0;
	s8 ydir=0;
	if(check_gridy1>check_gridy2)
		ydir=0;
	else
		ydir=1;
	for(temp_gridx=min_cleanx1+1;temp_gridx<max_cleanx1;temp_gridx++)
		{
			if((Read_Coordinate_CleanNoWall(temp_gridx,check_gridy1))&(!Read_Coordinate_Seat(temp_gridx,check_gridy1)))
				{
					if(Analysis_Fake_Leak(temp_gridx,check_gridy1,ydir))
						continue;
					
					temp1_gridx=temp_gridx;
					while(temp1_gridx<grid.x_area_max)
						{
							if((Read_Coordinate_CleanNoWall(temp1_gridx,check_gridy1))&(!Read_Coordinate_Seat(temp1_gridx,check_gridy1)))
								{
									check_point.backup_grid++;
								}
							temp1_gridx++;
						}
					check_point.new_x1=temp_gridx;
					check_point.new_y1=check_gridy1;
					check_point.new_x2=temp_gridx;
					check_point.new_y2=check_gridy2;
					check_point.next_tgtyaw=B_Angle_Const;
					if(ydir)
						check_point.ybs_dir=RIGHT;
					else
						check_point.ybs_dir=LEFT;
					if(Can_Entry_Point())
						return 1;
				}
		}
#endif
	return 0;
}

//function:检查漏扫区域
//return: 0,无漏扫，1，有漏扫
u8 Find_Leak_Area(void)
{
	s8 start_gridy,max_gridy,min_gridy;
	s8 check_gridy,check_gridx,temp_gridx;

	Init_CheckPoint();

	if(motion1.area_ok)
		return 0;
	
	start_gridy=grid.y;
	if(start_gridy>grid.y_area_max)
		start_gridy=grid.y_area_max;
	else if(start_gridy<grid.y_area_min)
		start_gridy=grid.y_area_min;
	max_gridy=grid.y_area_max;
	min_gridy=grid.y_area_min;
	s8 ydir=Read_Motion_YDir();
#if 0
	TRACE("Analysis leak area...!!!\r\n");
	TRACE("start_gridy=%d\r\n",start_gridy);
	TRACE("max_gridy=%d\r\n",max_gridy);
	TRACE("min_gridy=%d\r\n",min_gridy);
	TRACE("ydir=%d\r\n",ydir);
#endif
	if(ydir>0)							//当前正在沿Y轴正方向清扫
		{
			TRACE("prepare now point to Ymax check...\r\n");
			for(check_gridy=start_gridy;check_gridy<max_gridy;check_gridy++)
				{
					check_point.max_cleanx1=Return_MaxClean_GridX(check_gridy,0);
					check_point.min_cleanx1=Return_MinClean_GridX(check_gridy,0);
					check_point.max_cleanx2=Return_MaxClean_GridX(check_gridy+1,0);
					check_point.min_cleanx2=Return_MinClean_GridX(check_gridy+1,0);
					check_point.y1=check_gridy;
					check_point.y2=check_gridy+1;

					TRACE("check_point.max_cleanx1=%d min_cleanx1=%d\r\n",check_point.max_cleanx1,check_point.min_cleanx1);
					TRACE("check_point.max_cleanx2=%d min_cleanx2=%d\r\n",check_point.max_cleanx2,check_point.min_cleanx2);
					TRACE("check_point.y1=%d y2=%d\r\n",check_point.y1,check_point.y2);
					
					if((check_point.max_cleanx2==grid.x_area_min)|(check_point.min_cleanx2==grid.x_area_max))					//相邻检查点是否为空白区域					//相邻检查点出现空白未扫
						{
							TRACE("y2 hadnot sweeped!!!\r\n");
							if((check_point.max_cleanx1==grid.x_area_min)|(check_point.min_cleanx1==grid.x_area_max))			//检查点出现空白未扫
								{
									TRACE("y1 also hadnot sweeped!\r\n");
									break;//直接退出，漏扫检查完毕
								}
							else																								//检查点未出现空白
								{
									for(check_gridx=check_point.min_cleanx1;check_gridx<check_point.max_cleanx1;check_gridx++)	//寻找可进入口
										{
											if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy))&(Read_Coordinate_Clean(check_gridx+1,check_gridy)))
												{
													//temp_gridx=check_gridx+1;
													temp_gridx=check_gridx;
													while(temp_gridx<check_point.max_cleanx1)
														{
															if(Read_Coordinate_CleanNoWall(temp_gridx,check_gridy))
																{
																	temp_gridx++;
																	check_point.backup_grid++;
																}
															else
																break;
														}
													//check_gridx=(temp_gridx+check_gridx)/2;
													check_point.new_x1=check_gridx;
													check_point.new_y1=check_gridy;
													check_point.new_x2=check_gridx;
													check_point.new_y2=check_gridy+1;
													check_point.ydir=1;
													check_point.ybs_dir=RIGHT;
													check_point.next_tgtyaw=F_Angle_Const;
													Set_CheckPoint_NextAction(CHECK_NORMALSWEEP);
													TRACE("y1 find entry! tgt_x1=%d tgt_y1=%d \r\n",check_point.new_x1,check_point.new_y1);
													TRACE("tgt_x2=%d tgt_y2=%d\r\n",check_point.new_x2,check_point.new_y2);
													return 1;																	//发现可进入点								
												}
										}
									TRACE("y1 can't find entry!\r\n");
									//break;																					//未发现可进入点，直接退出，漏扫检查完毕 																			
									continue;
								}
						}
					if(Analysis_Leak_Point())
						{
							Set_CheckPoint_NextAction(CHECK_LEAKSWEEP);
							TRACE("find leak area!\r\n");
							TRACE("tgt_x1=%d tgt_y1=%d\r\n",check_point.new_x1,check_point.new_y1); 						
							TRACE("tgt_x2=%d tgt_y2=%d\r\n",check_point.new_x2,check_point.new_y2);
							return 1;
						}
				}
			TRACE("now point to Ymax check out!\r\n");
			TRACE("prepare now point to Ymin check...\r\n");
			for(check_gridy=start_gridy;check_gridy>min_gridy;check_gridy--)
				{
					check_point.max_cleanx1=Return_MaxClean_GridX(check_gridy,0);
					check_point.min_cleanx1=Return_MinClean_GridX(check_gridy,0);
					check_point.max_cleanx2=Return_MaxClean_GridX(check_gridy-1,0);
					check_point.min_cleanx2=Return_MinClean_GridX(check_gridy-1,0);
					check_point.y1=check_gridy;
					check_point.y2=check_gridy-1;
					
					TRACE("check_point.max_cleanx1=%d min_cleanx1=%d\r\n",check_point.max_cleanx1,check_point.min_cleanx1);
					TRACE("check_point.max_cleanx2=%d min_cleanx2=%d\r\n",check_point.max_cleanx2,check_point.min_cleanx2);
					TRACE("check_point.y1=%d y2=%d\r\n",check_point.y1,check_point.y2);
					
					if((check_point.max_cleanx2==grid.x_area_min)|(check_point.min_cleanx2==grid.x_area_max))
						{
							TRACE("y2 hadnot sweeped!!!\r\n");
							if((check_point.max_cleanx1==grid.x_area_min)|(check_point.min_cleanx1==grid.x_area_max))
								{
									TRACE("y1 also hadnot sweeped!\r\n");
									break;																					//直接退出，漏扫检查完毕
								}
							else
								{
									TRACE("point2 can't be sweep!\r\n");
									for(check_gridx=check_point.min_cleanx1;check_gridx<check_point.max_cleanx1;check_gridx++)
										{
											if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy))&(Read_Coordinate_Clean(check_gridx+1,check_gridy)))
												{
													//temp_gridx=check_gridx+1;
													temp_gridx=check_gridx;
													while(temp_gridx<check_point.max_cleanx1)
														{
															if(Read_Coordinate_CleanNoWall(temp_gridx,check_gridy))
																{
																	temp_gridx++;
																	check_point.backup_grid++;
																}
															else
																break;
														}
													//check_gridx=(check_gridx+temp_gridx)/2;
													check_point.new_x1=check_gridx;
													check_point.new_y1=check_gridy;
													check_point.new_x2=check_gridx;
													check_point.new_y2=check_gridy-1;
													//check_point.ydir=1;
													check_point.ydir=0;
													check_point.ybs_dir=LEFT;
													check_point.next_tgtyaw=F_Angle_Const;
													//Set_CheckPoint_NextAction(CHECK_NORMALSWEEP);
													Set_CheckPoint_NextAction(CHECK_LEAKSWEEP);
													TRACE("point1 find entry! tgt_x1=%d tgt_y1=%d \r\n",check_point.new_x1,check_point.new_y1);
													TRACE("tgt_x2=%d tgt_y2=%d\r\n",check_point.new_x2,check_point.new_y2);
													return 1;
												}
										}
									TRACE("y1 can't find entry!\r\n");
									//break;																					//未发现可进入点，直接退出，漏扫检查完毕 																			
									continue;
								}
						}					
					if(Analysis_Leak_Point())
						{
							Set_CheckPoint_NextAction(CHECK_LEAKSWEEP);
							TRACE("find leak area!\r\n");
							TRACE("tgt_x1=%d tgt_y1=%d\r\n",check_point.new_x1,check_point.new_y1); 						
							TRACE("tgt_x2=%d tgt_y2=%d\r\n",check_point.new_x2,check_point.new_y2);
							return 1;
						}
					}
			TRACE("now point Ymin check out,No leak!\r\n");
		}
	
	else				//当前正在沿Y轴负方向清扫
		{
			TRACE("prepare now point to Ymin check...\r\n");
			for(check_gridy=start_gridy;check_gridy>min_gridy;check_gridy--)
				{
					check_point.max_cleanx1=Return_MaxClean_GridX(check_gridy,0);
					check_point.min_cleanx1=Return_MinClean_GridX(check_gridy,0);
					check_point.max_cleanx2=Return_MaxClean_GridX(check_gridy-1,0);
					check_point.min_cleanx2=Return_MinClean_GridX(check_gridy-1,0);
					check_point.y1=check_gridy;
					check_point.y2=check_gridy-1;
					
					TRACE("check_point.max_cleanx1=%d min_cleanx1=%d\r\n",check_point.max_cleanx1,check_point.min_cleanx1);
					TRACE("check_point.max_cleanx2=%d min_cleanx2=%d\r\n",check_point.max_cleanx2,check_point.min_cleanx2);
					TRACE("check_point.y1=%d y2=%d\r\n",check_point.y1,check_point.y2);
					
					if((check_point.max_cleanx2==grid.x_area_min)|(check_point.min_cleanx2==grid.x_area_max))
						{
							TRACE("y2 hadnot sweeped!!!\r\n");
							if((check_point.max_cleanx1==grid.x_area_min)|(check_point.min_cleanx1==grid.x_area_max))
								{
									TRACE("y1 also hadnot sweeped!\r\n");
									break;																					//直接退出，漏扫检查完毕
								}
							else
								{
									TRACE("point2 can't be sweep!\r\n");
									for(check_gridx=check_point.min_cleanx1;check_gridx<check_point.max_cleanx1;check_gridx++)
										{
											if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy))&(Read_Coordinate_Clean(check_gridx+1,check_gridy)))
												{
													//temp_gridx=check_gridx+1;
													temp_gridx=check_gridx;
													while(temp_gridx<check_point.max_cleanx1)
														{
															if(Read_Coordinate_CleanNoWall(temp_gridx,check_gridy))
																{
																	temp_gridx++;
																	check_point.backup_grid++;
																}
															else
																break;
														}
													//check_gridx=(check_gridx+temp_gridx)/2;
													check_point.new_x1=check_gridx;
													check_point.new_y1=check_gridy;
													check_point.new_x2=check_gridx;
													check_point.new_y2=check_gridy-1;
													check_point.ydir=0;
													check_point.ybs_dir=LEFT;
													check_point.next_tgtyaw=F_Angle_Const;
													Set_CheckPoint_NextAction(CHECK_NORMALSWEEP);
													TRACE("point1 find entry! tgt_x1=%d tgt_y1=%d \r\n",check_point.new_x1,check_point.new_y1);
													TRACE("tgt_x2=%d tgt_y2=%d\r\n",check_point.new_x2,check_point.new_y2);
													return 1;
												}
										}
									TRACE("y1 can't find entry!\r\n");
									//break;																					//未发现可进入点，直接退出，漏扫检查完毕 																			
									continue;
								}
						}					
					if(Analysis_Leak_Point())
						{
							Set_CheckPoint_NextAction(CHECK_LEAKSWEEP);
							TRACE("find leak area!\r\n");
							TRACE("tgt_x1=%d tgt_y1=%d\r\n",check_point.new_x1,check_point.new_y1); 						
							TRACE("tgt_x2=%d tgt_y2=%d\r\n",check_point.new_x2,check_point.new_y2);
							return 1;
						}
					}
			TRACE("now point to Ymin check out!\r\n");
			TRACE("prepare now point to Ymax check...\r\n");
			for(check_gridy=start_gridy;check_gridy<max_gridy;check_gridy++)
				{
					check_point.max_cleanx1=Return_MaxClean_GridX(check_gridy,0);
					check_point.min_cleanx1=Return_MinClean_GridX(check_gridy,0);
					check_point.max_cleanx2=Return_MaxClean_GridX(check_gridy+1,0);
					check_point.min_cleanx2=Return_MinClean_GridX(check_gridy+1,0);
					check_point.y1=check_gridy;
					check_point.y2=check_gridy+1;

					TRACE("check_point.max_cleanx1=%d min_cleanx1=%d\r\n",check_point.max_cleanx1,check_point.min_cleanx1);
					TRACE("check_point.max_cleanx2=%d min_cleanx2=%d\r\n",check_point.max_cleanx2,check_point.min_cleanx2);
					TRACE("check_point.y1=%d y2=%d\r\n",check_point.y1,check_point.y2);
					
					if((check_point.max_cleanx2==grid.x_area_min)|(check_point.min_cleanx2==grid.x_area_max))					//相邻检查点是否为空白区域					//相邻检查点出现空白未扫
						{
							TRACE("y2 hadnot sweeped!!!\r\n");
							if((check_point.max_cleanx1==grid.x_area_min)|(check_point.min_cleanx1==grid.x_area_max))			//检查点出现空白未扫
								{
									TRACE("y1 also hadnot sweeped!\r\n");
									break;//直接退出，漏扫检查完毕
								}
							else																								//检查点未出现空白
								{
									for(check_gridx=check_point.min_cleanx1;check_gridx<check_point.max_cleanx1;check_gridx++)	//寻找可进入口
										{
											if((Read_Coordinate_CleanNoWall(check_gridx,check_gridy))&(Read_Coordinate_Clean(check_gridx+1,check_gridy)))
												{
													//temp_gridx=check_gridx+1;
													temp_gridx=check_gridx;
													while(temp_gridx<check_point.max_cleanx1)
														{
															if(Read_Coordinate_CleanNoWall(temp_gridx,check_gridy))
																{
																	temp_gridx++;
																	check_point.backup_grid++;
																}
															else
																break;
														}
													//check_gridx=(temp_gridx+check_gridx)/2;
													check_point.new_x1=check_gridx;
													check_point.new_y1=check_gridy;
													check_point.new_x2=check_gridx;
													check_point.new_y2=check_gridy+1;
													//check_point.ydir=0;
													check_point.ydir=1;
													check_point.ybs_dir=RIGHT;
													check_point.next_tgtyaw=F_Angle_Const;
													//Set_CheckPoint_NextAction(CHECK_NORMALSWEEP);
													Set_CheckPoint_NextAction(CHECK_LEAKSWEEP);
													TRACE("y1 find entry! tgt_x1=%d tgt_y1=%d \r\n",check_point.new_x1,check_point.new_y1);
													TRACE("tgt_x2=%d tgt_y2=%d\r\n",check_point.new_x2,check_point.new_y2);
													return 1;																	//发现可进入点								
												}
										}
									TRACE("y1 can't find entry!\r\n");
									//break;																					//未发现可进入点，直接退出，漏扫检查完毕 																			
									continue;
								}
						}
					if(Analysis_Leak_Point())
						{
							Set_CheckPoint_NextAction(CHECK_LEAKSWEEP);
							TRACE("find leak area!\r\n");
							TRACE("tgt_x1=%d tgt_y1=%d\r\n",check_point.new_x1,check_point.new_y1); 						
							TRACE("tgt_x2=%d tgt_y2=%d\r\n",check_point.new_x2,check_point.new_y2);
							return 1;
						}
				}
			TRACE("now point to Ymax check out!\r\n");
		}
	motion1.area_ok=true;
	//Logout_Area_Coordinate();
#ifdef USE_AREA_TREE
	Set_Curr_AreaTree_LeakInfo(motion1.area_ok);
#else
	Set_CurrNode_LeakInfo(motion1.area_ok);
#endif
	TRACE("now point to Ymax&Ymin check out! No leak!!\r\n");
	TRACE("leak area check complete!!!\r\n");
	return 0;
}

//function:寻找下一区域的入口
//return: 0:无法找到下一区域的入口，1:可以找到下一区域的入口，入口坐标计入check_point;
//寻找方向：从grid.x_area_min,y_area_max为起点进行逆时针寻找，从YMAX边界，到XMAX边界，再到YMIN边界，最后到XMIN边界
u8 Find_NextArea_Entry(void)
{
	s8 temp_data1,temp_data2,temp_data3,temp_data4;
	temp_data1=Analysis_Boundary_Y_II(1);	//YMAX边界，起点为x_area_min,y_area_max
	temp_data2=Analysis_Boundary_X_II(1);	//XMAX边界，起点为x_area_max,y_area_max
	temp_data3=Analysis_Boundary_Y_II(0);	//YMIN边界，起点为x_area_max,y_area_min
	temp_data4=Analysis_Boundary_X_II(0);	//XMIN边界，起点为x_area_min,y_area_min
	if(temp_data1!=0x7f)				//首先寻找最大Y边界出口
		{
			TRACE("Find Ymax next area entry!!!\r\n");
			//TRACE("Prepare goto Ymax entry!!!\r\n");
			check_point.new_x1=temp_data1;
			check_point.new_y1=grid.y_area_max;
			check_point.new_x2=temp_data1;
			check_point.new_y2=grid.y_area_max+1;
			check_point.next_yacc=1;					//下一区域按Y轴正方向清扫
			check_point.next_xacc=2;					//下一区域按X轴双方向清扫
			check_point.next_tgtyaw=F_Angle_Const;		//首先对正角度
			check_point.new_area_dir=DIR_YMAX;
			Set_CheckPoint_NextAction(CHECK_NEWAREA);
			return 1;
		}

	if(temp_data2!=0x7f)			//再然后寻找最大X边界出口
		{
			TRACE("Find Xmax next area entry!!!\r\n");
			//TRACE("Prepare goto Xmax entry!!!\r\n");
			check_point.new_x1=grid.x_area_max;
			check_point.new_y1=temp_data2;
			check_point.new_x2=grid.x_area_max+1;
			check_point.new_y2=temp_data2;
			check_point.next_yacc=2;					//下一区域按Y轴双方向清扫
			check_point.next_xacc=1;					//下一区域按X轴正方向清扫
			check_point.next_tgtyaw=F_Angle_Const;		//首先对正角度
			check_point.new_area_dir=DIR_XMAX;
			Set_CheckPoint_NextAction(CHECK_NEWAREA);
			return 1;
		}

	if(temp_data3!=0x7f)			//然后寻找最小Y边界出口
		{
			TRACE("Find Ymin next area entry!!!\r\n");
			//TRACE("Prepare goto Ymin entry!!!\r\n");
			check_point.new_x1=temp_data3;
			check_point.new_y1=grid.y_area_min;
			check_point.new_x2=temp_data3;
			check_point.new_y2=grid.y_area_min-1;
			check_point.next_yacc=0;					//下一区域按Y轴负方向清扫
			check_point.next_xacc=2;					//下一区域按X轴双方向清扫
			check_point.next_tgtyaw=F_Angle_Const;		//首先对正角度
			check_point.new_area_dir=DIR_YMIN;
			Set_CheckPoint_NextAction(CHECK_NEWAREA);
			return 1;
		}	

	if(temp_data4!=0x7f)			//最后寻找最小X边界出口
		{
			TRACE("Find Xmin next area entry!!!\r\n");
			//TRACE("Prepare goto Ymin entry!!!\r\n");
			check_point.new_x1=grid.x_area_min;
			check_point.new_y1=temp_data4;
			check_point.new_x2=grid.x_area_min-1;
			check_point.new_y2=temp_data4;
			check_point.next_yacc=2;					//下一区域按Y轴双方向清扫
			check_point.next_xacc=0;					//下一区域按X轴负方向清扫
			check_point.next_tgtyaw=B_Angle_Const;		//首先对正角度
			check_point.new_area_dir=DIR_XMIN;
			Set_CheckPoint_NextAction(CHECK_NEWAREA);
			return 1;
		}
	else
		{
			TRACE("XY all can't find any next area entry!!!\r\n");
			return 0;
		}
}

u8 Find_ExitArea_Entry(void)
{
#ifdef USE_AREA_TREE
	Out_Curr_AreaTree_Exit();
#else
	Out_CurrNode_Exit();
#endif
	return 0;
}

u8 Get_TurnDir(short tgt_angle)
{
	short now_angle;
	int temp_data;
	now_angle=Gyro_Data.yaw;
	temp_data=now_angle-tgt_angle;
	if(abs(temp_data)>18000)
		{
			if(temp_data<0)
				temp_data+=36000;
			else
				temp_data-=36000;
		}
//	TRACE("now=%d tgt=%d\r\n",now_angle,tgt_angle);
	if(temp_data>=0)
		{
//			TRACE("turn_dir=1\r\n");
			return 1;
		}
	else
		{
//			TRACE("turn_dir=2\r\n");
			return 2;
		}
}

//return 
//0:应该不会返回这个值
//1:检查到漏扫区域
//2:检查到新区域
//3:回到上一个区域
//4:已回到最开始出发点
u8 Area_Check(u8 avoid_ybs)
{
	u8 curr_areanum=0,return_data=0;
	TRACE("Enter in %s...\r\n",__func__);
#ifdef USE_AREA_TREE
	curr_areanum=Read_Curr_AreaTree_NO();
#else
	curr_areanum=Read_CurrNode_AreaNO();
#endif
	TRACE("curr_areanum=%d\r\n",curr_areanum);
	
	if((mode.mode!=SHIFT)&(mode.mode!=EXIT))
		{
			TRACE("mode.mode!=SHIFT!!\r\n");
			TRACE("set worktime_area 10!!\r\n");
			Set_AreaWorkTime(10);
		}
	
	if((!motion1.force_dock)&(Find_Leak_Area())	)					//寻找当前区域漏扫
		{
			TRACE("Go to LeakArea!!!\r\n");
			motion1.first_leak_y=check_point.new_y2;
			//Init_Shift_Point1(avoid_ybs);
			return_data=1;
		}
	else if((!motion1.force_dock)&(Find_NextArea_Entry()))			//无漏扫，寻找当前区域是否可以进入下一个新区域
		{
			TRACE("Go to NewArea!!!\r\n");
			//Init_Shift_Point1(avoid_ybs);
			return_data=2;
		}
	else										//无新区域
		{
			TRACE("Go to Exit!!!\r\n");
			Find_ExitArea_Entry();
			if(curr_areanum==1)		//是否是第一打扫区域,是则打扫完成
				{
					TRACE("This area is first sweep area!!");
					TRACE("Goto original point and prepare to Dock!!\r\n");
					if(!motion1.force_dock)
						{
							Send_Voice(VOICE_VOLUME_3);
							Send_Voice(VOICE_SWEEP_DONE);
						}
					//while(1);
					if(motion1.start_seat)
						{
							Set_CheckPoint_NextAction(CHECK_DOCK);
							Send_Voice(VOICE_VOLUME_3);
							Send_Voice(VOICE_DOCK_START);
						}
							
					return_data=3;
				}
			else if(curr_areanum==0)
				{
					TRACE("This is in original area!!!\r\n");
					TRACE("Prepare to Dock!!!\r\n");
					Init_Docking();
					return_data=4;
				}
			else								//不是，则进入下一区域
				{
					TRACE("This area isnot first sweep area!!\r\n");
					TRACE("Go to Exit!!!\r\n");
					return_data=3;
				}
			//Init_Shift_Point1(avoid_ybs);
		}
	TRACE("Out in %s...\r\n",__func__);
	return return_data;
}

void Init_Gyro_Bios(void)
{
	gyro_bios.check_step=0;
	gyro_bios.acc_angle=0;
}

/*---------------------------------------------------------------
function:判断惯导角度是否朝一个方向旋转了超360度
input:None
output:	0:没有超过阈值，1:朝左旋转超过360度，2:朝右旋转超过360度
----------------------------------------------------------------*/
u8 Gyro_Bios_Check(void)
{
	static bool inybs=false;
	short now_angle=Gyro_Data.yaw;
	int temp_angle;

	if(!gyro_bios.check_flag)
		return 0;
	gyro_bios.check_flag=false;
	switch(gyro_bios.check_step)
		{
			case 0:
				gyro_bios.check_angle=now_angle;
				gyro_bios.acc_angle=0;
				gyro_bios.check_step++;
				gyro_bios.island_gridx=grid.x;
				gyro_bios.island_gridy=grid.y;
				if((mode.sub_mode==YBS_SUB_LEFT)|(mode.sub_mode==YBS_SUB_RIGHT))
					{
						inybs=true;
					}
				else
					{
						inybs=false;
					}				
				break;
			case 1:
				temp_angle=now_angle-gyro_bios.check_angle;
				if(abs(temp_angle)>DEGREE_180)
					{
						if(temp_angle>0)
							temp_angle=temp_angle-DEGREE_360;
						else
							temp_angle=temp_angle+DEGREE_360;
					}
				gyro_bios.check_angle=now_angle;
				gyro_bios.acc_angle+=temp_angle;
				if(gyro_bios.acc_angle>DEGREE_360)
					{
						gyro_bios.result++;
						gyro_bios.check_step=0;
						Send_Voice(VOICE_VOLUME_4);
						if(inybs)
							{
							if((abs(grid.x-gyro_bios.island_gridx)<2)&(abs(grid.y-gyro_bios.island_gridy)<2))
								gyro_bios.island=true;
								inybs=false;
							}
						else
							{
								gyro_bios.island=false;
								inybs=false;
							}
						return 2;
					}
				else if(gyro_bios.acc_angle<(-DEGREE_360))
					{
						gyro_bios.result--;
						gyro_bios.check_step=0;
						Send_Voice(VOICE_VOLUME_4);
						if(inybs)
							{
							if((abs(grid.x-gyro_bios.island_gridx)<2)&(abs(grid.y-gyro_bios.island_gridy)<2))
								gyro_bios.island=true;
								inybs=false;
							}
						else
							{
								gyro_bios.island=false;
								inybs=false;
							}
						return 1;
					}

				if((mode.sub_mode!=YBS_SUB_LEFT)&(mode.sub_mode!=YBS_SUB_RIGHT))
					{
						inybs=false;
					}
				break;
		}
	return 0;
}

//function：查看Entry_Point是不是被墙包围
u8 Can_Entry_Point(void)
{
	s8 cnt;
	u8 ydir=0,len=0,i,first_len,check_result=0;//,second_len,
	ydir=check_point.ydir;
	cnt=check_point.backup_grid;
	len=abs(cnt);
	first_len=len+2;
	//second_len=len+4;
	if(len<=0)
		{
			TRACE("backup gridx=0!!\r\n");
			return 0;
		}
	if(len>20)
		{
			TRACE("backup gridx>20!!\r\n");
			return 0;
		}
	POINT_GRID entry_point[20];		//entry_point最大为20个点
	POINT_GRID first_check[25];//second_check[30];		//first_check最大为22个点,second_check最大30个点
	for(i=0;i<len;i++)
		{
			if(cnt>0)
				entry_point[i].gridx=check_point.new_x1+i;
			else
				entry_point[i].gridx=check_point.new_x1-i;
			entry_point[i].gridy=check_point.new_y1;
			first_check[i].gridx=entry_point[i].gridx;
			if(ydir)
				{
					first_check[i].gridy=check_point.new_y1-1;
					//second_check[i].gridy=check_point.new_y1-2;
				}
			else
				{
					first_check[i].gridy=check_point.new_y1+1;
					//second_check[i].gridy=check_point.new_y1+2;
				}
		}

	if(ydir)
		{
			if(cnt>0)
				{
					first_check[first_len-1].gridx=entry_point[0].gridx-1;
					first_check[first_len-1].gridy=check_point.new_y1;
					first_check[first_len-2].gridx=entry_point[len-1].gridx+1;
					first_check[first_len-2].gridy=check_point.new_y1;
#if 0
					second_check[second_len-1].gridx=first_check[first_len-1].gridx;
					second_check[second_len-1].gridy=first_check[first_len-1].gridy;
					second_check[second_len-2].gridx=first_check[first_len-2].gridx;
					second_check[second_len-2].gridy=first_check[first_len-2].gridy;

					second_check[second_len-3].gridx=entry_point[0].gridx-1;
					second_check[second_len-3].gridy=check_point.new_y1-1;
					second_check[second_len-4].gridx=entry_point[len-1].gridx+1;
					second_check[second_len-4].gridy=check_point.new_y1-1;
#endif
				}
			else
				{
					first_check[first_len-1].gridx=entry_point[0].gridx+1;
					first_check[first_len-1].gridy=check_point.new_y1;
					first_check[first_len-2].gridx=entry_point[len-1].gridx-1;
					first_check[first_len-2].gridy=check_point.new_y1;
#if 0
					second_check[second_len-1].gridx=first_check[first_len-1].gridx;
					second_check[second_len-1].gridy=first_check[first_len-1].gridy;
					second_check[second_len-2].gridx=first_check[first_len-2].gridx;
					second_check[second_len-2].gridy=first_check[first_len-2].gridy;

					second_check[second_len-3].gridx=entry_point[0].gridx+1;
					second_check[second_len-3].gridy=check_point.new_y1-1;
					second_check[second_len-4].gridx=entry_point[len-1].gridx-1;
					second_check[second_len-4].gridy=check_point.new_y1-1;
#endif
				}
		}
	else
		{
			if(cnt>0)
				{
					first_check[first_len-1].gridx=entry_point[0].gridx-1;
					first_check[first_len-1].gridy=check_point.new_y1;
					first_check[first_len-2].gridx=entry_point[len-1].gridx+1;
					first_check[first_len-2].gridy=check_point.new_y1;
#if 0					
					second_check[second_len-1].gridx=first_check[first_len-1].gridx;
					second_check[second_len-1].gridy=first_check[first_len-1].gridy;
					second_check[second_len-2].gridx=first_check[first_len-2].gridx;
					second_check[second_len-2].gridy=first_check[first_len-2].gridy;
					
					second_check[second_len-3].gridx=entry_point[0].gridx-1;
					second_check[second_len-3].gridy=check_point.new_y1+1;
					second_check[second_len-4].gridx=entry_point[len-1].gridx+1;
					second_check[second_len-4].gridy=check_point.new_y1+1;
#endif
				}
			else
				{
					first_check[first_len-1].gridx=entry_point[0].gridx+1;
					first_check[first_len-1].gridy=check_point.new_y1;
					first_check[first_len-2].gridx=entry_point[len-1].gridx-1;
					first_check[first_len-2].gridy=check_point.new_y1;
#if 0					
					second_check[second_len-1].gridx=first_check[first_len-1].gridx;
					second_check[second_len-1].gridy=first_check[first_len-1].gridy;
					second_check[second_len-2].gridx=first_check[first_len-2].gridx;
					second_check[second_len-2].gridy=first_check[first_len-2].gridy;
					
					second_check[second_len-3].gridx=entry_point[0].gridx+1;
					second_check[second_len-3].gridy=check_point.new_y1+1;
					second_check[second_len-4].gridx=entry_point[len-1].gridx-1;
					second_check[second_len-4].gridy=check_point.new_y1+1;
#endif
				}
		}
	
	check_result=0;
	for(i=0;i<first_len;i++)
		{
			if((Read_Coordinate_Wall(first_check[i].gridx, first_check[i].gridy))\
				|(!Read_Coordinate_Clean(first_check[i].gridx, first_check[i].gridy)))
				check_result++;
		}
	if(check_result>=first_len)
		{
			TRACE("Entry Point has surround by firstwall!!\r\n");
			for(i=0;i<len;i++)
				Set_Coordinate_Wall(entry_point[i].gridx,entry_point[i].gridy);
			return 0;
		}
#if 0
	check_result=0;
	for(i=0;i<second_len;i++)
		{
			if((Read_Coordinate_Wall(second_check[i].gridx, second_check[i].gridy))\
				|(!Read_Coordinate_Clean(second_check[i].gridx, second_check[i].gridy)))
				check_result++;
		}
	if(check_result>=second_len)
		{
			TRACE("Entry Point has surround by secondwall!!\r\n");
			for(i=0;i<len;i++)
				Set_Coordinate_Wall(entry_point[i].gridx,entry_point[i].gridy);
			return 0;
		}
#endif	
	return 1;
}

u8 Can_Entry_NewArea(CHECK_POINT *check_point)
{
	u8 new_area_dir,i=0,check_result=0;
	u8 len,first_len;//second_len;
	new_area_dir=check_point->new_area_dir;
	len=check_point->backup_grid;
	first_len=len+2;//second_len=len+4;
	POINT_GRID entry_point[20],first_check[25];//second_check[second_len];

	if(len<=0)
		{
			TRACE("backup grid=0!!\r\n");
			return 0;
		}
	if(len>20)
		{
			TRACE("backup grid>20!!\r\n");
			return 0;
		}
	
	if(new_area_dir==DIR_YMAX)
		{
			for(i=0;i<len;i++)
				{
					entry_point[i].gridx=check_point->new_x1+i;
					entry_point[i].gridy=check_point->new_y1;
					first_check[i].gridx=check_point->new_x1+i;
					first_check[i].gridy=check_point->new_y1-1;
					//second_check[i].gridx=check_point->new_x1+i;
					//second_check[i].gridy=check_point->new_y1-2;
				}
			first_check[first_len-1].gridx=first_check[0].gridx-1;
			first_check[first_len-1].gridy=check_point->new_y1;
			first_check[first_len-2].gridx=first_check[i-1].gridx+1;
			first_check[first_len-2].gridy=check_point->new_y1;
#if 0			
			second_check[second_len-1].gridx=first_check[first_len-1].gridx;
			second_check[second_len-1].gridy=first_check[first_len-1].gridy;
			second_check[second_len-2].gridx=first_check[first_len-2].gridx;
			second_check[second_len-2].gridy=first_check[first_len-2].gridy;
			
			second_check[second_len-3].gridx=second_check[0].gridx-1;
			second_check[second_len-3].gridy=check_point->new_y1-1;
			second_check[second_len-4].gridx=second_check[i-1].gridx+1;
			second_check[second_len-4].gridy=check_point->new_y1-1;
#endif
		}
	else if(new_area_dir==DIR_YMIN)
		{
			for(i=0;i<len;i++)
				{
					entry_point[i].gridx=check_point->new_x1-i;
					entry_point[i].gridy=check_point->new_y1;
					first_check[i].gridx=check_point->new_x1-i;
					first_check[i].gridy=check_point->new_y1+1;
					//second_check[i].gridx=check_point->new_x1-i;
					//second_check[i].gridy=check_point->new_y1+2;
				}
			first_check[first_len-1].gridx=first_check[0].gridx+1;
			first_check[first_len-1].gridy=check_point->new_y1;
			first_check[first_len-2].gridx=first_check[i-1].gridx-1;
			first_check[first_len-2].gridy=check_point->new_y1;
#if 0			
			second_check[second_len-1].gridx=first_check[first_len-1].gridx;
			second_check[second_len-1].gridy=first_check[first_len-1].gridy;
			second_check[second_len-2].gridx=first_check[first_len-2].gridx;
			second_check[second_len-2].gridy=first_check[first_len-2].gridy;
			
			second_check[second_len-3].gridx=second_check[0].gridx+1;
			second_check[second_len-3].gridy=check_point->new_y1+1;
			second_check[second_len-4].gridx=second_check[i-1].gridx-1;
			second_check[second_len-4].gridy=check_point->new_y1+1;
#endif
		}
	else if(new_area_dir==DIR_XMAX)
		{
			for(i=0;i<len;i++)
				{
					entry_point[i].gridx=check_point->new_x1;
					entry_point[i].gridy=check_point->new_y1-i;//entry_point[i].gridy=check_point->new_y1+i;
					first_check[i].gridx=check_point->new_x1-1;
					first_check[i].gridy=check_point->new_y1-i;//first_check[i].gridy=check_point->new_y1+i;
					//second_check[i].gridx=check_point->new_x1-2;
					//second_check[i].gridy=check_point->new_y1-i;//second_check[i].gridy=check_point->new_y1+i;
				}
			first_check[first_len-1].gridx=check_point->new_x1;
			first_check[first_len-1].gridy=first_check[0].gridy+1;//first_check[first_len-1].gridy=first_check[0].gridy-1;
			first_check[first_len-2].gridx=check_point->new_x1;
			first_check[first_len-2].gridy=first_check[i-1].gridy-1;//first_check[first_len-2].gridy=first_check[i-1].gridy+1;
#if 0			
			second_check[second_len-1].gridx=first_check[first_len-1].gridx;
			second_check[second_len-1].gridy=first_check[first_len-1].gridy;
			second_check[second_len-2].gridx=first_check[first_len-2].gridx;
			second_check[second_len-2].gridy=first_check[first_len-2].gridy;
			
			second_check[second_len-3].gridx=check_point->new_x1-1;
			second_check[second_len-3].gridy=second_check[0].gridy+1;//second_check[second_len-3].gridy=second_check[0].gridy-1;
			second_check[second_len-4].gridx=check_point->new_x1-1;
			second_check[second_len-4].gridy=second_check[i-1].gridy-1;//second_check[second_len-4].gridy=second_check[i-1].gridy+1;
#endif
		}
	else if(new_area_dir==DIR_XMIN)
		{
			for(i=0;i<len;i++)
				{
					entry_point[i].gridx=check_point->new_x1;
					entry_point[i].gridy=check_point->new_y1+i;
					first_check[i].gridx=check_point->new_x1+1;
					first_check[i].gridy=check_point->new_y1+i;
					//second_check[i].gridx=check_point->new_x1+2;
					//second_check[i].gridy=check_point->new_y1+i;
				}
			first_check[first_len-1].gridx=check_point->new_x1;
			first_check[first_len-1].gridy=first_check[0].gridy-1;
			first_check[first_len-2].gridx=check_point->new_x1;
			first_check[first_len-2].gridy=first_check[i-1].gridy+1;
#if 0			
			second_check[second_len-1].gridx=first_check[first_len-1].gridx;
			second_check[second_len-1].gridy=first_check[first_len-1].gridy;
			second_check[second_len-2].gridx=first_check[first_len-2].gridx;
			second_check[second_len-2].gridy=first_check[first_len-2].gridy;
			
			second_check[second_len-3].gridx=check_point->new_x1-1;
			second_check[second_len-3].gridy=second_check[0].gridy-1;
			second_check[second_len-4].gridx=check_point->new_x1-1;
			second_check[second_len-4].gridy=second_check[i-1].gridy+1;
#endif
		}
	else
		{
			TRACE("no check area dir!!!\r\n");
			return 0;
		}

	check_result=0;
	//for(i=0;i<first_len;i++)
	for(i=0;i<len;i++)
		{
			TRACE("first_check[%d].gridx=%d gridy=%d \r\n",i,first_check[i].gridx,first_check[i].gridy);
			TRACE("and wall=%d\r\n",Read_Coordinate_Wall(first_check[i].gridx,first_check[i].gridy));
			if(Read_Coordinate_Wall(first_check[i].gridx,first_check[i].gridy)\
				|(!Read_Coordinate_Clean(first_check[i].gridx,first_check[i].gridy)))
				check_result++;
		}
	//if(check_result>=first_len)
	if(check_result>=len)
		{
			TRACE("New Area has surround by firstwall!!\r\n");
			for(i=0;i<len;i++)
				{
					Set_Coordinate_Wall(entry_point[i].gridx,entry_point[i].gridy);
					return 0;
				}
		}

#if 0
	check_result=0;
	for(i=0;i<second_len;i++)
		{
			TRACE("second_check[%d].gridx=%d gridy=%d \r\n",i,second_check[i].gridx,second_check[i].gridy);
			TRACE("and wall=%d\r\n",Read_Coordinate_Wall(second_check[i].gridx,second_check[i].gridy));
			if(Read_Coordinate_Wall(second_check[i].gridx,second_check[i].gridy)\
				|(!Read_Coordinate_Clean(second_check[i].gridx,second_check[i].gridy)))
				check_result++;
		}
	if(check_result>=second_len)
		{
			TRACE("New Area has surround by secondwall!!\r\n");
			for(i=0;i<len;i++)
				{
					Set_Coordinate_Wall(entry_point[i].gridx,entry_point[i].gridy);
					return 0;
				}
		}
#endif
	return 1;

}

u8 Analysis_Fake_Leak(s8 xcheck,s8 ycheck,u8 ydir)
{
	s8 temp_ycheck=0;
	if(ydir>0)
		{
			for(temp_ycheck=ycheck+1;temp_ycheck<=grid.y_area_max;temp_ycheck++)
				{
					if(Read_Coordinate_Clean(xcheck,temp_ycheck))
						{
							//if(temp_ycheck!=grid.y_area_max)
							return 1;
						}
				}
		}
	else
		{
			for(temp_ycheck=ycheck-1;temp_ycheck>=grid.y_area_min;temp_ycheck--)
				{
					if(Read_Coordinate_Clean(xcheck,temp_ycheck))
						{
							return 1;
						}
				}
		}
	return 0;
}

void Coor_Nothing(void)
{
}
