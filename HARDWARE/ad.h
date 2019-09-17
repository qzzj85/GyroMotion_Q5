#ifndef  __AD_H
#define  __AD_H

#include "stm32f10x.h"


/////////������һЩ�������//////////////////////////////////
#define  BATTERY_VOLTAGE    0   //��ص�ѹͨ��   //PA0 ADת��ͨ��0
#define  CHARGE_CURRENT		2

#define  E_L                4   //��ߵؼ�ͨ��  //PA3 ADת��ͨ��3
#define  E_M                5   //�м�ؼ�  	  //PA4	ADת��ͨ��4		
#define  E_R                6   //�ұߵؼ�ͨ��  //PA6 ADת��ͨ��6
#define  W_L                14   //���ǽ��ͨ��   //PA7    ADת��ͨ��7
#define  W_RM               8   //����ǽ��ͨ��  //PB0 ADת��ͨ��8
#define  W_R                9   //�ұ�ǽ��ͨ��  //PB1 ADת��ͨ��9
#define  M_CURRENT			10
#define  FAN_CURRNET		11
#define	 L_CURRENT			12
#define  R_CURRENT			13
#define  W_LM               7  //����ǽ��ͨ��   //PC4 ADת��ͨ��14
#define  W_M                15  //�м�ǽ��ͨ��   //PC5 ADת��ͨ��15



void init_ad(void);
u16 account_current (u8 channel);
void ReInitAd(void);

extern u16  ADC_ConvertedValue[256];
extern bool ADC_ready;



uint16 Grand_Sensor_AD (uint8 channel);
uint16 Wall_account_current (u8 channel);


#endif









