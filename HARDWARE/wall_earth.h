
#ifndef	 WALL_EARTH_H
#define  WALL_EARTH_H

////////////////////////////////////头文件///////////////////
#include "soft.h"

#define	    E_LOWDOOR				100		//普通地面砖大约6~7cm,灰色木地板大约4~5公分,纯黑色大约3公分

//qz add 20180319
#define 	W_L_LOWDOOR				0x100
#define		W_LM_LOWDOOR			0x100
#define 	W_M_LOWDOOR				0X200
#define		W_RM_LOWDOOR			0X100
#define		W_R_LOWDOOR				0X100
#define		W_L_HIGHDOOR			0X50
#define		W_LM_HIGHDOOR			0X50
#define		W_M_HIGHDOOR			0X100
#define		W_RM_HIGHDOOR			0X50
#define		W_R_HIGHDOOR			0X50

///////////////////////////////////变量定义//////////////////
extern WALL_DATA  e_l,e_r,e_m,w_l,w_lm,w_r,w_rm,w_m;  //地检和墙检的数据结构e_lm,e_rm
extern bool wall_earth_time;  //地检和墙检的
extern bool action_wall_time;	
extern bool check_earth_time;
extern bool check_wall_time;
//QZ ADD

extern bool action_wall;     //允许检测墙检标志
//////////////////////////////////子程序定义////////////////
void read_wallearth(void);//读强检地检的值并且做出判断
void init_wallearth(void);
void Disable_wall(void);
void Disable_earth(void);
void Enable_wall(void);
void Enable_earth(void);
void Wall_SensitivityHigh(void);
void Wall_SensitivityLow(void);
void  check_Wall( WALL_DATA *earth,u16 difference);
void  check_earth( WALL_DATA *earth,u16 difference);	
void  check_near( WALL_DATA *earth);

void Wall_Sensitivity_YanQiang(void);
void read_wallearth_my(void);

void Read_Earth_My(void);
void Read_Wall_My(void);

#endif

