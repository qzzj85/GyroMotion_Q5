
#ifndef  __TEST_H
#define  __TEST_H
						  
////////////////////////////////////头文件///////////////////

#include "soft.h"

#define		NONE   	0xFFFF
#define		PASS	0XFFFE
#define		FAIL	0XFFFD
#define		MAIN_MENU	0XFFFC
#define		FULL	0XFFFB
////////////////全局变量定义//////////////////////////
#define		TST_MENU			0
#define		TST_FAN_MB			1
#define		TST_VOCIE			2
#define		TST_SWITCH			3
#define		TST_WALL			4
#define		TST_EARTH			5
#define		TST_OC				6
#define		TST_DOCK			7
#define		TST_CHG				8
#define		TST_SPD				9
#define		TST_FREE			10
#define		TST_FREE_SPD		11
#define		TST_WALL_DATA		13
#define		TST_UV				14

#define		MAX_TST_ITEM		14
//////////////////////////////////子程序定义////////////////
extern bool m_hw_test,l_hw_test,r_hw_test,b_hw_test;
extern u32 start_time;
extern int length1,length2;
extern void frontandback(void) ;
extern void Do_Test(void);
extern void Do_Test_xxx(void);
void Init_Test(void);
void Do_Test_My(void);
void Do_Burnin_Test(void);
void ChargeControl_Test(void);

#endif

