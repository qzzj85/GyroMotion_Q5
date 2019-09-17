#ifndef  __PWM_H
#define  __PWM_H

#include "stm32f10x.h"

#if 0
#define PWM_RING_MAX 	1000
#define	PWM_RING_PRESCALER	3
#else
#define	PWM_RING_MAX	2000
#define	PWM_RING_PRESCALER	1
#endif
#define PWM_CHARGE_MAX 	2000
#define	PWM_SWEEP_PRESCALER	0

//hzh  ����ȼ�
#define		STANDARD				1
#define		FORCE					3
#define		SILENCE					2
#define 	SILENCE_PWM   			PWM_CHARGE_MAX/5
#define		SB_STANDARD_PWM			500
#define 	STANDARD_PWM  			1600//PWM_CHARGE_MAX/2		//qz add 20180804
#define 	FORCE_PWM     			2000//PWM_CHARGE_MAX*4/5
#define		DOCK_SWEEP_PWM			200
#define 	STOP_FJ   				0
#define 	STOP_ALL	  			0
#define		DOCK_SWEEP_LEVEL		4



void  Init_PWM(void);
void  enable_pwm(u16 channel,u16  pwm);
void  disable_pwm(u16 channel);
void  Set_Pwm(u16 channel,u16  pwm);


void  Break_pwm(u16 channel);

//qz add new
void Set_SideBrush_Pwm(u16 pwm);
void Set_MidBrush_Pwm(u16 pwm);
void Set_Fan_Pwm(u16 pwm);
void Set_Charge_Pwm(u16 pwm);




#endif
