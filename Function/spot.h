
#ifndef  __SPOT_H
#define  __SPOT_H


////////////////////////头文件////////////////////////////////////
#include "stm32f10x.h"
#include "soft.h"

////////////////////////全局变量//////////////////////////////////
#define	SPOT_FROM_CEASE		0
#define	SPOT_FROM_SWEEP		1
#define	SPOT_FROM_CHARGE	2

extern s8 spot_gyrobios_cnt;
///////////////////////全局函数////////////////////////////////////
void Init_Spot(u8 end_work);
void Init_Sweep_Spot(u8 start_from);

void Do_Spot(void);
void Do_Spot_My(void);

#endif
