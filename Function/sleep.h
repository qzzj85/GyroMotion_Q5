
#ifndef  __SLEEP_H
#define  __SLEEP_H


////////////////////////头文件////////////////////////////////////
#include "stm32f10x.h"
#include "soft.h"



////////////////////////全局变量//////////////////////////////////
extern bool ExitInput;     //有按键、外部电源外部输入
extern bool key_wakeup_flag;
///////////////////////全局函数////////////////////////////////////		 
void Init_Sleep(void); 
void Do_Sleep(void);
void Do_Sleep_My(void);
void RCC_EXITSTOP(void);

#endif
