
#ifndef  __SLEEP_H
#define  __SLEEP_H


////////////////////////ͷ�ļ�////////////////////////////////////
#include "stm32f10x.h"
#include "soft.h"



////////////////////////ȫ�ֱ���//////////////////////////////////
extern bool ExitInput;     //�а������ⲿ��Դ�ⲿ����
extern bool key_wakeup_flag;
///////////////////////ȫ�ֺ���////////////////////////////////////		 
void Init_Sleep(void); 
void Do_Sleep(void);
void Do_Sleep_My(void);
void RCC_EXITSTOP(void);

#endif
