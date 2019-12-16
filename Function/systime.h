
#ifndef  __SYSTIME_H
#define  __SYSTIME_H

#include "stm32f10x.h"

void Init_Time_1(void);
void init_time_2(void);
void DelayMs(unsigned int Ms);
extern volatile  unsigned int  giv_sys_time;      	//定义为整个系统的系统时钟
extern volatile bool Sec;											   
extern bool gbv_minute;   													//分钟标志，用于定时采样电池的电压。
extern bool Five_Sec;
extern bool gyro_check_time;
	#endif
