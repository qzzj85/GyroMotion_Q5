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
#define 	ALL_TOP_MASK		0xEEEEE
#define 	ALL_LEFT_ONLY		0x88888
#define 	ALL_MID_ONLY		0x04444
#define 	ALL_RIGHT_ONLY		0X11111
//#define 	ALL_TOP_ONLY		0x01111
#define		ALL_MIDLEFT_ONLY	0X44444
#define		ALL_MIDRIGHT_ONLY	0X22222
#define		LEFTMIDLEFT_ONLY	0XCCCCC
#define		RIGHTMIDRIGHT_ONLY	0X33333
#define		ALL_TOP_ONLY		0X11111
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

///////////////////�һس�ʱ��case hzh////////////////////////////
#define  DOCKMODE_STEP_FINDSEAT   0Xff

extern 	bool 	DOCK_PREPARE;
extern 	u8 		dock_fail_count;
extern 	bool 	findseat_skid_check_flag;
extern 	u8 		findseat_skid_check_step;
extern 	u32 	findseat_skid_check_length;
extern 	u32 	findseat_skid_check_time;
extern  u32     find_home;
extern 	u8 		REYBS_TIME;
extern  bool 	dock_rap_time;

u8 Find_Seat_My(void);
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
s16 Cal_Angle(s16 angle_1,s16 angle_2);
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
void YBS_Find_Seat(void);

u8 Find_Seat_Skid_Check(void);
void ReYBS2Dock(void);
u8 Find_Seat_My_MMHW(void);
void Find_Seat_New(void);
u8 Find_Seat_New_II(void);
void Start_Start_III(void);
void Start_Start_IV(void);
void Start_Start_V(void);
#endif
