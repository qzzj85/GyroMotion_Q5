#ifndef  __CEASE_H
#define  __CEASE_H
////////////////////////ͷ�ļ�////////////////////////////////////	   
#include "stm32f10x.h"										
#include "soft.h"

////////////////////////ȫ�ֱ���//////////////////////////////////	 
///////////////////////ȫ�ֺ���////////////////////////////////////	   
void Do_Cease(void);    //ִ���ڴ���״̬�ĳ���
void Init_Cease(void);
void Cease_No_Key(void);
void Cease_YaoKong_Manage(void);
void Init_VirtualSleep(void);
void Do_VirtualSleep(void);

///////////////////////ȫ�ֺ���////////////////////////////////////
///////////////////////˽�к���////////////////////////////////////	
void Cease_display_set(void);
void Cease_SetTime(void);
void Cease_Timing(void);
void Cease_Lode_Time(void);
void Cease_Save_Time(void);
void Cease_Save_Preengage(void);
void Cease_Lode_Preengage(void);
void Cease_Big_Subb_Time(void);
void Cease_Subb_Time(void);
void Cease_Big_Add_Time(void);
void Cease_Add_Time(void);

#endif
