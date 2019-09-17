#ifndef	 __RING_H
#define  __RING_H

////////////////////////////////////ͷ�ļ�///////////////////
#include "stm32f10x.h"
#include "soft.h"
#include "key.h"

///////////////////////////////////��������//////////////////
extern RING  l_ring,r_ring,w_ring,all_ring,check_ring;       //���Գ����ٶȵĽṹ
extern BRUSH_DATA Fan,SideBrush,MidBrush,LSideBrush,RSideBrush;			//���,��ɨ,��ɨ���ݽṹ	QZ ADD
extern RAP  l_rap,r_rap,w_rap;           //�����ٶȾ���Ŀ��ƽṹ
extern bool rap_time;                    //�����ٶȵ���ʱ���־
//extern u16 jt_l_current;              //��������1n60����©��������ľ�̬������
//extern u16 jt_r_current;              //��������1n60����©��������ľ�̬������
//extern volatile u8 stop_waiting_flag;
//extern u8 frontwheelon;               //ǰ����Ч��־��1Ϊ��Ч��0Ϊ��Ч

extern u8 Wheel_State ;            //����������״̬ 0Ϊ�ϸ�1Ϊ������	 
extern u32 Speed ;                    //���ֵ�����ٶ�  ����ʾΪ����ʱ��
extern u8 err;
extern ACTION action;
//////////////////////////////////�ӳ�����////////////////
void Init_Ring(void);
u32 subbtime(u32 little,u32 big);



void yq_pwm(void);		//zdkadd
void yq_fast_pwm(void);//hzh add 
void yq_fast_findwall_pwm(void);//hzh  add
void yq_get_distance_k(void);//hzh 




void enable_rap(u32 ori_l, u32 length_l,u32 ori_r,u32 length_r);
void motor_run(u32 ori_l, u32 pwm_l,u32 ori_r,u32 pwm_r);
void stop_rap(void);

//��ɨ
void ZS_High_Back(void);
void ZS_Low_Back(void);
void ZS_High_Front(void);
void ZS_Low_Front(void);
void stop_bs(void);

//��ɨ
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




