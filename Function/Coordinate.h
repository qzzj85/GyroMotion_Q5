#ifndef CORDINATE_H
#define	CORDINATE_H

#define DEGREE_180			18000
#define DEGREE_300			30000
#define	DEGREE_360			36000

#define	RANGE_MAX			1000
#define	RANGE_MIN			1000
#define RANGE_AREA_MAX		200
#define	RANGE_AREA_MIN		200

#define	COORDI_XLENGTH		(2*RANGE_MAX/20)	//100
#define	COORDI_YLENGTH		(2*RANGE_MAX/20)	//100
#define GRID_MAX			(RANGE_MAX/20)		//50
#define GRID_MIN			(-GRID_MAX)			//(-49)
#define GRIDY_POS_CNT		(2*RANGE_MAX/20+1)	//101

#define CHECK_NORMALSWEEP	0
#define	CHECK_BACK			1
#define	CHECK_LEAKSWEEP		2
#define	CHECK_NEWAREA		3
#define	CHECK_GOEXIT		4

#define	DIR_YMAX			1
#define	DIR_XMAX			2
#define	DIR_YMIN			3
#define	DIR_XMIN			4

extern float X_pos,Y_pos;
extern volatile u8 coordinate[COORDI_YLENGTH+1][COORDI_XLENGTH+1];
extern int x_max[20],x_min[20];
extern GRID grid;
extern CHECK_POINT check_point;
extern short gridy_pos[GRIDY_POS_CNT],gridx_pos[GRIDY_POS_CNT];
extern GYRO_BIOS gyro_bios;
void Cal_xy(void);
void Record_Coordinate_Intime(void);
void Logout_Area_Coordinate(void);
void Init_Coordinate(void);
void Cal_CoordinateXY(void);
u8 Judge_Yaw_Reach(short tgt_yaw,short bios_yaw);
u8 Judge_Pos_Reach(short tgt_xpos, short tgt_ypos);
u8 Judge_Xpos_Reach(short tgt_xpos,u8 xpos_bios);
u8 Judge_Ypos_Reach(short tgt_ypos,u8 pos_bios);
void Init_Real_X(void);
void Cal_Xmaxmin(void);
u8 Get_TurnDir(short tgt_angle);
u8 Read_Coordinate_Data(s8 xgrid,s8 ygrid);
u8 Read_Coordinate_Wall(s8 xgrid,s8 ygrid);
u8 Read_Coordinate_Clean(s8 xgrid,s8 ygrid);
u8 Read_Coordinate_AreaNo(s8 xgrid,s8 ygrid);

u8 Find_Leak_Area(void);
u8 Find_NextArea_Entry(void);
u8 Find_ExitArea_Entry(void);
s8 Return_Grid(short pos,short pos_base);
u8 Read_Coordinate_CleanNoWall(s8 xgrid,s8 ygrid);
s8 Return_MaxClean_GridX(s8 gridy,u8 nowall);
s8 Return_MinClean_GridX(s8 gridy,u8 nowall);
void Set_Coordinate_Clean(s8 xgrid,s8 ygrid);
void Set_Coordinate_Wall(s8 xgrid,s8 ygrid);
void Logout_CheckPoint(void);
u8 Judge_GridYPOS_Nearby_Reach(s8 gridy);
void Cal_Grid_Pos(void);
u8 Judge_GridYPOS_Reach(s8 gridy,short dir);
void Set_Coordinate_WallClean(s8 xgrid,s8 ygrid);
u8 Judge_GridXPOS_Reach(s8 gridx,short dir);
short Return_GridXPos_Point(s8 gridx);
short Return_GridYPos_Point(s8 gridy);
void Reset_CheckPoint_NextAction(void);
void Set_CheckPoint_NextAction(u8 next_action);
u8 Read_CheckPoint_NextAction(void);
void Area_Check(u8 avoid_ybs);
void Init_Gyro_Bios(void);
u8 Gyro_Bios_Check(void);
u8 Judge_GridYPOS_Reach_Large(s8 gridy);
s8 Cal_Real_Grid (s8 grid);
u8 Can_Entry_Point(s8 gridx,s8 gridy,u8 check_ydir);

#endif


