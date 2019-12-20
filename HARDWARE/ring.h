#ifndef	 __RING_H
#define  __RING_H

////////////////////////////////////头文件///////////////////
#include "stm32f10x.h"
#include "soft.h"
#include "key.h"

///////////////////////////////////变量定义//////////////////
extern RING  l_ring,r_ring,w_ring;       //测试车轮速度的结构
extern BRUSH_DATA Fan,SideBrush,MidBrush,LSideBrush,RSideBrush;			//风机,边扫,中扫数据结构	QZ ADD
extern RAP  l_rap,r_rap,w_rap;           //两轮速度距离的控制结构
extern bool rap_time;                    //两轮速度调整时间标志

extern u32 Speed ;                    //两轮的最大速度  ，表示为脉冲时间
extern ACTION action;
//////////////////////////////////子程序定义////////////////
void Init_Ring(void);
void enable_rap(u32 ori_l, u32 length_l,u32 ori_r,u32 length_r);
void stop_rap(void);
void stop(void);

void Change_Ring_Speed(int lspeed,int rspeed);
void enable_rap_no_length(u32 ori_l,u32 speed_l,u32 ori_r,u32 speed_r);
void Enable_Speed(void);
void Disable_Speed(void);


#ifdef   MOTOR_SELF_TEST
void motor_self_test(void);
#endif

#endif



