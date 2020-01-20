#ifndef _LD_DOCKING_H
#define _LD_DOCKING_H
#include "stm32f10x.h"

#define		STRAIGHT_SPEED		1500

#define		HIGH_HIGH_SPEED		2000	//1500
#define 	HIGH_FAST_SPEED		1500	//1000
#define 	HIGH_CONST_SPEED	1000	//800
#define		CONST_SPEED_800		800
#define		CONST_SPEED_700		700
#define     CONST_SPEED_600		600
#define 	CONST_SPEED 		500
#define 	MID_CONST_SPEED 	300
#define		CONST_SPEED_300		300
#define		CONST_SPEED_200		200
#define 	LOW_CONST_SPEED 	100
#define 	LEFT_DOCK			1

#define 	DOCKMODE_STEP_START		0
#define		DOCKMODE_STEP_TOP_SPOT	1
#define 	DOCKMODE_STEP_TOP		2
#define 	DOCKMODE_STEP_MID		3
#define 	DOCKMODE_STEP_RIGHT		4
#define 	DOCKMODE_STEP_LEFT		5
#define		DOCKMODE_STEP_CHARGE	6
#define		DOCKMODE_STEP_RECHARGE	7
#define		DOCKMODE_STEP_DOCKFAIL	9
#define		DOCKMODE_STEP_REYBS		10
#define		DOCKMODE_STEP_TOP_CIRCUM	11

///////////////////ÕÒ»Ø³äÊ±µÄcase hzh////////////////////////////
#define  DOCKMODE_STEP_FINDSEAT   0Xff

extern 	u8 		dock_fail_count;
extern 	bool 	findseat_skid_check_flag;
extern 	u8 		findseat_skid_check_step;
extern 	u32 	findseat_skid_check_length;
extern 	u32 	findseat_skid_check_time;
extern  u32     find_home;
extern 	u8 		REYBS_TIME;
extern bool lm_t_cnt_timeflag,rm_t_cnt_timeflag;

void Docking_Bump_My(void);
void Docking_Bump_My_Action(void);
void Start_Left(void);
void Start_Mid(void);
void Start_Right(void);
void Start_Top(void);
void Start_Top_Spot(void);
uint16 ReadHwSign(void);
void Start_Recharge(void);
u32 ReadHwSign_My(void);
void Do_Docking_My(void);
void Start_Top_Straight(void);
void Start_Left_Straight(void);
void Start_Right_Straight(void);
void Start_Right_Straight_Spiral(void);
void Start_Right_Straight_Turn(void);
void Start_Left_Straight_Spiral(void);
void Start_Left_Straight_Turn(void);
void Start_Top_Spot_My(void);
u8 Dock_read_bump(void);
void Start_Right_Deflect_Turn(void);
void Start_Left_Deflect_Turn(void);
void Start_Start(void);
void Start_Start_II(void);

u8 Find_Seat_Skid_Check(void);
void ReYBS2Dock(void);
u8 Find_Seat_My(void);
void Find_Seat_New(void);
u8 Find_Seat_New_II(void);
void Start_Start_V(void);
void Start_Start_VI(void);
void Start_Start_VII(void);

void Start_Top_Circum(void);


void Init_Dock_RightYBS(u8 temp_data);
void Init_Dock_LeftYBS(u8 temp_data);
void Do_Docking_YBS(void);

#endif
