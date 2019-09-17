#ifndef	 __RING_H
#define  __RING_H

////////////////////////////////////头文件///////////////////
#include "stm32f10x.h"
#include "soft.h"
#include "key.h"

///////////////////////////////////变量定义//////////////////
extern RING  l_ring,r_ring,w_ring,all_ring,check_ring;       //测试车轮速度的结构
extern BRUSH_DATA Fan,SideBrush,MidBrush,LSideBrush,RSideBrush;			//风机,边扫,中扫数据结构	QZ ADD
extern RAP  l_rap,r_rap,w_rap;           //两轮速度距离的控制结构
extern bool rap_time;                    //两轮速度调整时间标志
//extern u16 jt_l_current;              //左轮由于1n60反向漏电流引起的静态电流。
//extern u16 jt_r_current;              //右轮由于1n60反向漏电流引起的静态电流。
//extern volatile u8 stop_waiting_flag;
//extern u8 frontwheelon;               //前轮有效标志，1为有效，0为无效

extern u8 Wheel_State ;            //左右轮驱动状态 0为合格，1为驱动坏	 
extern u32 Speed ;                    //两轮的最大速度  ，表示为脉冲时间
extern u8 err;
extern ACTION action;
//////////////////////////////////子程序定义////////////////
void Init_Ring(void);
u32 subbtime(u32 little,u32 big);



void yq_pwm(void);		//zdkadd
void yq_fast_pwm(void);//hzh add 
void yq_fast_findwall_pwm(void);//hzh  add
void yq_get_distance_k(void);//hzh 




void enable_rap(u32 ori_l, u32 length_l,u32 ori_r,u32 length_r);
void motor_run(u32 ori_l, u32 pwm_l,u32 ori_r,u32 pwm_r);
void stop_rap(void);

//边扫
void ZS_High_Back(void);
void ZS_Low_Back(void);
void ZS_High_Front(void);
void ZS_Low_Front(void);
void stop_bs(void);

//中扫
void zhong_sao_front(void);
void zhong_sao_back(void);
void stop_zs(void);



void  comm_rap(void);
void Motor_task(void);
u8 Test_DriveOfWheel(void) ;
u8 Test_DriveOfBrush(void);
void Change_Speed(void);
#endif






void  ACC_DEC_comm_rap(void);
void  ACC_DEC_Curve(void);


void enable_rap_no_length(u32 ori_l,u32 speed_l,u32 ori_r,u32 speed_r);

void Enable_Speed(void);
void Disable_Speed(void);




