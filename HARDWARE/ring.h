#ifndef	 __RING_H
#define  __RING_H

////////////////////////////////////ͷ�ļ�///////////////////
#include "stm32f10x.h"
#include "soft.h"
#include "key.h"

///////////////////////////////////��������//////////////////
extern RING  l_ring,r_ring,w_ring;       //���Գ����ٶȵĽṹ
extern BRUSH_DATA Fan,SideBrush,MidBrush,LSideBrush,RSideBrush;			//���,��ɨ,��ɨ���ݽṹ	QZ ADD
extern RAP  l_rap,r_rap,w_rap;           //�����ٶȾ���Ŀ��ƽṹ
extern bool rap_time;                    //�����ٶȵ���ʱ���־

extern u32 Speed ;                    //���ֵ�����ٶ�  ����ʾΪ����ʱ��
extern ACTION action;
//////////////////////////////////�ӳ�����////////////////
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



