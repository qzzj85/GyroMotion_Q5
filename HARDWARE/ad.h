#ifndef  __AD_H
#define  __AD_H

#include "stm32f10x.h"


/////////下面是一些软件定义//////////////////////////////////
#define  BATTERY_VOLTAGE    0   //电池电压通道   //PA0 AD转换通道0
#define  CHARGE_CURRENT		2

#define  E_L                4   //左边地检通道  //PA3 AD转换通道3
#define  E_M                5   //中间地检  	  //PA4	AD转换通道4		
#define  E_R                6   //右边地检通道  //PA6 AD转换通道6
#define  W_L                14   //左边墙检通道   //PA7    AD转换通道7
#define  W_RM               8   //右中墙检通道  //PB0 AD转换通道8
#define  W_R                9   //右边墙检通道  //PB1 AD转换通道9
#define  M_CURRENT			10
#define  FAN_CURRNET		11
#define	 L_CURRENT			12
#define  R_CURRENT			13
#define  W_LM               7  //左中墙检通道   //PC4 AD转换通道14
#define  W_M                15  //中间墙检通道   //PC5 AD转换通道15



void init_ad(void);
u16 account_current (u8 channel);
void ReInitAd(void);

extern u16  ADC_ConvertedValue[256];
extern bool ADC_ready;



uint16 Grand_Sensor_AD (uint8 channel);
uint16 Wall_account_current (u8 channel);


#endif









