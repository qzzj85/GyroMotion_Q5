#ifndef  __HWINCEPT_H
#define  __HWINCEPT_H
////////////////////////头文件////////////////////////////////////	 
#include "stm32f10x.h"
#include "soft.h"
///////////////////下面是红外检测的数据hzh////////////////////////////
//hzh

#define  LEFT_SIGN       	   0x5C    
#define  LEFT_MID_SIGN         0x57   
#define  RIGHT_SIGN            0X55     
#define  RIGHT_MID_SIGN        0x5B
#define	 NEAR_SIGN			   0X53
///////////////////下面是红外区域hzh////////////////////////////

#define	 NEAR_INF_LEFTNEAR	   (0x00000001<<23)
#define	 NEAR_INF_MIDNEAR	   (0x00000001<<22)
#define  NEAR_INF_MMIDNEAR   (0X00000001<<21)
#define  NEAR_INF_RIGHTNEAR    (0x00000001<<20)

#define  LEFT_INF__ULTRA_LEFT       (0x00000001 << 19) //左红外找到极左
#define  LEFT_INF__MIDLEFT           (0x00000001 << 18) //          
#define  LEFT_INF__MIDRIGHT		(0x00000001 << 17) //    
#define  LEFT_INF__RIGHT              (0x00000001 << 16) //      

#define  MID_INF__ULTRA_LEFT   (0x00000001 << 15) //          
#define  MID_INF__MIDLEFT       (0x00000001 << 14) //          
#define  MID_INF__MIDRIGHT  (0x00000001 <<13) //
#define  MID_INF__RIGHT          (0x00000001 << 12) //

#define  MMID_INF__ULTRA_LEFT   (0x00000001 << 11) //          
#define  MMID_INF__MIDLEFT      (0x00000001 << 10) //          
#define  MMID_INF__MIDRIGHT  (0x00000001 << 9) //
#define  MMID_INF__RIGHT          (0x00000001 << 8) //

#define  RIGHT_INF__ULTRA_LEFT   (0x00000001 << 7) //          
#define  RIGHT_INF__MIDLEFT       (0x00000001 << 6) //          
#define  RIGHT_INF__MIDRIGHT  (0x00000001 << 5) //
#define  RIGHT_INF__RIGHT          (0x00000001 << 4) //

#define  BACK_INF__ULTRA_LEFT   (0x00000001 << 3) //          
#define  BACK_INF__MIDLEFT       (0x00000001 << 2) //          
#define  BACK_INF__MIDRIGHT  (0x00000001 << 1) //
#define  BACK_INF__RIGHT          (0x00000001 << 0) //

////////////////////////全局变量//////////////////////////////////

extern INFR_DATA l_hw,r_hw,rm_hw,b_hw,up_hw,dummy_wall,lm_hw;	  
extern bool Action_hwincept;
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
