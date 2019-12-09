#ifndef  __AD_H
#define  __AD_H

#include "stm32f10x.h"


/////////������һЩ�������//////////////////////////////////
#define  ADC_BAT_VOL    0   //��ص�ѹͨ��   //PA0 ADת��ͨ��0
#define  ADC_BAT_CHGCURR		2

#define  ADC_EARTH_L        4   //��ߵؼ�ͨ��  //PA3 ADת��ͨ��3
#define  ADC_EARTH_M        5   //�м�ؼ�  	  //PA4	ADת��ͨ��4		
#define  ADC_EARTH_R        6   //�ұߵؼ�ͨ��  //PA6 ADת��ͨ��6
#define  ADC_WALL_L                14   //���ǽ��ͨ��   //PA7    ADת��ͨ��7
#define  ADC_WALL_RM               8   //����ǽ��ͨ��  //PB0 ADת��ͨ��8
#define  ADC_WALL_R                9   //�ұ�ǽ��ͨ��  //PB1 ADת��ͨ��9
#define  ADC_MB_CURR		10	//��ɨ����ͨ��
#define  ADC_FAN_CURR		11	//�������ͨ��
#define	 ADC_LRING_CURR		12	//���ֵ���ͨ��
#define  ADC_RRING_CURR		13	//���ֵ���ͨ��
#define	 ADC_SB_CURRENT		3	//��ɨ����ͨ��
#define  ADC_WALL_LM               7  //����ǽ��ͨ��   //PC4 ADת��ͨ��14
#define  ADC_WALL_M                15  //�м�ǽ��ͨ��   //PC5 ADת��ͨ��15



void init_ad(void);
u16 account_current (u8 channel);
void ReInitAd(void);

extern u16  ADC_ConvertedValue[256];
extern bool ADC_ready;



uint16 Grand_Sensor_AD (uint8 channel);
uint16 Wall_account_current (u8 channel);


#endif









