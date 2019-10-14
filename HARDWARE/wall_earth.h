
#ifndef	 WALL_EARTH_H
#define  WALL_EARTH_H

////////////////////////////////////ͷ�ļ�///////////////////
#include "soft.h"

#define	    E_LOWDOOR				100		//��ͨ����ש��Լ6~7cm,��ɫľ�ذ��Լ4~5����,����ɫ��Լ3����

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

///////////////////////////////////��������//////////////////
extern WALL_DATA  e_l,e_r,e_m,w_l,w_lm,w_r,w_rm,w_m;  //�ؼ��ǽ������ݽṹe_lm,e_rm
extern bool wall_earth_time;  //�ؼ��ǽ���
extern bool action_wall_time;	
extern bool check_earth_time;
extern bool check_wall_time;
//QZ ADD

extern bool action_wall;     //������ǽ���־
//////////////////////////////////�ӳ�����////////////////
void read_wallearth(void);//��ǿ��ؼ��ֵ���������ж�
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

