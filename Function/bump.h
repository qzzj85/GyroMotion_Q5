#ifndef BUMP_H
#define BUMP_H

#define BUMP_BACK_SPEED 	MID_MOVE_SPEED	//500//1000//1500			//qz modify 20180927 2500--->1500
#define BUMP_TIME_DELAY 		200//2000//1500	//500	//1000
#define BUMP_BACK_LENGTH		3	

#define LM_WALL_BUMP		8
#define MID_WALL_BUMP		9
#define RM_WALL_BUMP		15

#define L_BUMP_BIT 			0x8
#define LM_BUMP_BIT			0x4
#define RM_BUMP_BIT			0X2
#define R_BUMP_BIT			0X1
#define L_BUMP_MASK			0X7
#define LM_BUMP_MASK		0XB
#define RM_BUMP_MASK		0XD
#define R_BUMP_MASK			0XE

#define BUMP_ONLY_LEFT      50
#define BUMP_ONLY_LEFTMID   51
#define BUMP_LEFT_MID       52
#define BUMP_ONLY_RIGHT     60
#define BUMP_ONLY_RIGHTMID  61
#define BUMP_RIGHT_MID      62
#define BUMP_MID            70

#define	BUMP_OUTRANGE		100
#define BUMP_XOUTRANGE		101
#define BUMP_YOUTRANGE		102
#define BUMP_XMAX_OUT		103
#define BUMP_XMIN_OUT		104
#define BUMP_YMAX_OUT		105
#define BUMP_YMIN_OUT		106
#define BUMP_LF_OUT			107				//左上点
#define BUMP_RF_OUT			108				//右上点
#define BUMP_LB_OUT 		109				//左下点
#define BUMP_RB_OUT  		110				//右下点
extern u8 bump_value;
extern bool bump_exiton;
extern u32 bump_exitoff_time;
extern bool ring_cnt_flag;
void Init_Bump_Interrupt(void);
void Clear_BumpValue(void);
u8 Parse_BumpValue(void);
void Close_Bump_Exit(void);
void Open_Ring_Cnt(void);
void Close_Ring_Cnt(void);

#endif
