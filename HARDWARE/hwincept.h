#ifndef  __HWINCEPT_H
#define  __HWINCEPT_H
////////////////////////头文件////////////////////////////////////	 
#include "stm32f10x.h"
#include "soft.h"
///////////////////下面是红外检测的数据hzh////////////////////////////
//hzh

#define  LEFT_SIGN       	   0x5B    
#define  LEFT_MID_SIGN         0xff   
#define  RIGHT_SIGN            0x57   
#define  RIGHT_MID_SIGN        0xFE
#define	 NEAR_SIGN			   0XFD
#define	 TOP_SIGN			   0X5C
#define	 MID_SIGN			   0X53
///////////////////下面是红外区域hzh////////////////////////////
#define		BACKLEFT_INF_TOP		(1<<23)
#define		BACKLEFT_INF_LEFT		(1<<22)
#define		BACKLEFT_INF_RIGHT		(1<<21)
#define		BACKLEFT_INF_MID		(1<<20)

#define		BACKRIGHT_INF_TOP		(1<<19)
#define		BACKRIGHT_INF_LEFT		(1<<18)
#define		BACKRIGHT_INF_RIGHT		(1<<17)
#define		BACKRIGHT_INF_MID		(1<<16)

#define		LEFT_INF_TOP			(1<<15)
#define		LEFT_INF_LEFT			(1<<14)
#define		LEFT_INF_RIGHT			(1<<13)
#define		LEFT_INF_MID			(1<<12)

#define		MIDLEFT_INF_TOP			(1<<11)
#define		MIDLEFT_INF_LEFT		(1<<10)
#define		MIDLEFT_INF_RIGHT		(1<<9)
#define		MIDLEFT_INF_MID			(1<<8)

#define		MIDRIGHT_INF_TOP		(1<<7)
#define		MIDRIGHT_INF_LEFT		(1<<6)
#define		MIDRIGHT_INF_RIGHT		(1<<5)
#define		MIDRIGHT_INF_MID		(1<<4)

#define		RIGHT_INF_TOP			(1<<3)
#define		RIGHT_INF_LEFT			(1<<2)
#define		RIGHT_INF_RIGHT			(1<<1)
#define		RIGHT_INF_MID			(1<<0)

#define		ALL_TOP_ONLY			0x888888
#define 	ALL_TOP_MASK			0x777777
#define 	ALL_LEFT_ONLY			0x444444
#define 	ALL_RIGHT_ONLY			0X222222
#define 	ALL_MID_ONLY			0X111111
#define		ALL_MIDLEFT_ONLY		0X44444
#define		ALL_MIDRIGHT_ONLY		0X22222
#define		LEFTMIDLEFT_ONLY		0XCCCCC
#define		RIGHTMIDRIGHT_ONLY		0X33333
#define		ALL_TOP_MID				0X1111


////////////////////////全局变量//////////////////////////////////

extern INFR_DATA l_hw,r_hw,rm_hw,lm_hw,lb_hw,rb_hw;	  
extern bool Action_hwincept;
extern u16 top_time,top_time_sec;
///////////////////////全局函数////////////////////////////////////

void  init_hwincept(void);
void read_hw_data(INFR_DATA *hw_struct);
void read_dummy_wall(INFR_DATA *hw_struct);

void clr_hw_effect(INFR_DATA *hw_struct);
void enable_hwincept(void);
void disable_hwincept(void);
void read_hw_bit_my(INFR_DATA *hw_struct);
void clr_all_hw_effect(void);
void clr_hw_struct(INFR_DATA *hw_struct);
void clr_all_hw_struct(void);
#endif
