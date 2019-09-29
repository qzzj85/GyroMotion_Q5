
#ifndef  __SYSTIME_H
#define  __SYSTIME_H

#include "stm32f10x.h"

void Init_Time_1(void);
void init_time_2(void);
void DelayMs(unsigned int Ms);
extern volatile  unsigned int  giv_sys_time;      	//����Ϊ����ϵͳ��ϵͳʱ��
extern volatile  unsigned int  time_tmp[10];

extern unsigned int time_stemp;		//qz add
	
extern bool app_bat_min;

extern volatile bool Sec;											   
extern volatile bool Sec_Two;
extern bool gbv_minute;   													//���ӱ�־�����ڶ�ʱ������صĵ�ѹ��
extern bool Five_Sec;
extern bool Ten_Sec;
extern bool Half_Sec;
extern bool Half_Min;
extern bool MiliSec_50;	//50ms
extern bool time_speed;
extern bool gyro_check_time;
extern bool coordinate_show;
	#endif
