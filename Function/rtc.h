#ifndef  __RTC_H
#define  __RTC_H
////////////////////////////////////ͷ�ļ�///////////////////
#include "stm32f10x.h"
#include "function.h"
///////////////////////////////////��������//////////////////
#define		BAT_REINIT_BKP		BKP_DR1
#define		VOICE_BACKUP_BKP	BKP_DR2
#define		PREEN_BACKUP_BKP	BKP_DR3
#define		MODE_BACKUP_BKP		BKP_DR4
#define 	BATCAP_HADDR_BKP	BKP_DR5
#define		BATCAP_LADDR_BKP	BKP_DR6
#define		BATFDCAP_HADDR_BKP	BKP_DR7
#define		BATFDCAP_LADDR_BKP	BKP_DR8
#define		BATRECAL_BACKUP_BKP	BKP_DR9

extern u16  PreengageTime;
extern u32  Rtc_time;
extern u16  PreengageDate;
extern u32  giv_BatteryCapability;  //��ص�����
extern u32  giv_BatteryFDCap;//��طŵ�ĵ���
extern u32  giv_UsePowerTime;// �õ�ʱ�䡣
extern u8   giv_BatteryChargeForbid; //�����ش��������־��0Ϊ����1Ϊ������
extern bool watchdog_time;
extern bool PREEN_TIME_UP;

void init_rtc(void);
void  	WriteRtcTime(void);//дʵʱʱ��
void  	WriteWorkState(void);
void  	WriteFDCap(void);
void  	WriteBatteryCapability(void);
void 	ReadRealTime(void);
u8 		CleanPreengage(void);
void 	ReadPreengage(void);
void 	WriteBatInitFlash(void);
u16  	ReadBatInitFlash(void);	
u16 ReadBatRecal(void);
void WriteBatRecal(u16 data);
u8 CleanPreenData(void);
u32  ReadBatteryCapability(void);
u8  WritePreenData(void);
u32 ReadFDCap(void);
void Reinit_Rtc(void);


#endif
