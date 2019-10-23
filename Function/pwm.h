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

//hzh  风机等级
#define 	SILENCE_PWM   			PWM_CHARGE_MAX/5
#define		SB_STANDARD_PWM			500
#define 	STANDARD_PWM  			1600//PWM_CHARGE_MAX/2		//qz add 20180804
#define 	FORCE_PWM     			2000//PWM_CHARGE_MAX*4/5
#define		DOCK_SWEEP_PWM			200

#define 	SWEEP_LEVEL_STOP	  	0
#define		SWEEP_LEVEL_STANDARD	1
#define		SWEEP_LEVEL_FORCE		3
#define		SWEEP_LEVEL_SILENCE		2
#define		SWEEP_LEVEL_DOCK		4

#define		MB_STD_PWM				200
#define		FAN_STD_PWM				900

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
