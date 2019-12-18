#ifndef  __RTC_H
#define  __RTC_H
////////////////////////////////////头文件///////////////////
#include "stm32f10x.h"
#include "function.h"
///////////////////////////////////变量定义//////////////////
#define		BAT_REINIT_BKP		BKP_DR1
#define		VOICE_BACKUP_BKP	BKP_DR2
#define		PREEN_BACKUP_BKP	BKP_DR3
#define		MODE_BACKUP_BKP		BKP_DR4
#define 	BATCAP_HADDR_BKP	BKP_DR5
#define		BATCAP_LADDR_BKP	BKP_DR6
#define		BATFDCAP_HADDR_BKP	BKP_DR7
#define		BATFDCAP_LADDR_BKP	BKP_DR8
#define		BATRECAL_BACKUP_BKP	BKP_DR9

#define	PREEN_LENGTH				1		//预约时间的组数，目前为1组预约


extern u32  Rtc_time;
extern bool watchdog_time;
#ifdef PREEN_SWEEP
extern PREEN_DATA	Preen_Data[PREEN_LENGTH];
#endif
void init_rtc(void);
void  	WriteRtcTime(void);//写实时时钟
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
u8 	Check_PreengageTime(void);
void Init_Rtc_Data(void);


#endif
