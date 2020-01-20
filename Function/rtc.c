#include "AAA-include.h"
u32  Rtc_time;      //实时时钟的值
bool watchdog_time;
#ifdef PREEN_SWEEP
PREEN_DATA		Preen_Data[PREEN_LENGTH];
#endif

void Reinit_Rtc(void)
{
	PWR_BackupAccessCmd(ENABLE);

#ifdef USE_LSE				 	 
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
#ifdef DEBUG_RTC
			TRACE("hang at %s %d\r\n",__func__,__LINE__);
#endif
		}
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#else
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#endif
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();
	
	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	Rtc_time=0;
	RTC_WaitForLastTask();
	WriteRtcTime();
	
	PWR_BackupAccessCmd(DISABLE);
	
}

void Init_Rtc_Data(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
  	EXTI_InitTypeDef EXTI_InitStructure;
  	//将闹钟中断与外部中断联系起来

	PWR_BackupAccessCmd(ENABLE);
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);    

	Rtc_time = RTC_GetCounter();
	RTC_WaitForLastTask();
	if(Rtc_time >= 604800)
		{
			Rtc_time = Rtc_time % 604800;
			RTC_SetCounter(Rtc_time);
			RTC_WaitForLastTask();
		}

	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);

  /* Enable the RTC Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//使能闹钟中断
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

void init_rtc(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
  	EXTI_InitTypeDef EXTI_InitStructure;
  
  	//将闹钟中断与外部中断联系起来
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);    
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
   
	PWR_BackupAccessCmd(ENABLE);
	
//	BKP_DeInit();
	//修改启动程序
#ifdef USE_LSE				 	 
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
#ifdef DEBUG_RTC
			TRACE("hang at %s %d\r\n",__func__,__LINE__);
#endif
		}
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#else
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#endif
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	RTC_WaitForLastTask();

	
	 
  	Rtc_time = RTC_GetCounter();
	RTC_WaitForLastTask();
	if(Rtc_time >= 604800)
    {
        Rtc_time = Rtc_time % 604800;
	    RTC_SetCounter(Rtc_time);
        RTC_WaitForLastTask();
    }

	RTC_WaitForLastTask();
  	RTC_ITConfig(RTC_IT_ALR, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
  	RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);

  /* Enable the RTC Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	//使能闹钟中断
  	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
  	NVIC_Init(&NVIC_InitStructure);
}


#ifdef  RTC_8M_CORR
unsigned int  second_time=0x2710; 
bool  rtc_8m_flag   = FALSE;
u8  rtc_8m_mode  =   WAIT_CORR;

void enable_rtc_second_irq(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
//  	EXTI_InitTypeDef EXTI_InitStructure;	
    /* 使能RTC秒中断 */ 
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* 等待对RTC寄存器最后的写操作完成 */         
    RTC_WaitForLastTask();
  /* Enable the RTC Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	//使能闹钟中断
  	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  	NVIC_Init(&NVIC_InitStructure);	
}
void disable_rtc_second_irq(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
  //	EXTI_InitTypeDef EXTI_InitStructure;	
    /* 使能RTC秒中断 */ 
    RTC_ITConfig(RTC_IT_SEC, DISABLE);
    /* 等待对RTC寄存器最后的写操作完成 */         
    RTC_WaitForLastTask();
  /* Enable the RTC Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  	//使能闹钟中断
  	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  	NVIC_Init(&NVIC_InitStructure);	
}

void  rtc_8m_corr(void)
{
	switch(rtc_8m_mode )
		{
			case WAIT_CORR:
				enable_rtc_second_irq();
				rtc_8m_mode  = BEGIN_CORR_1;
			//	second_time =  0;
				break;
			case BEGIN_CORR_1  :
				break;
			case BEGIN_CORR_2  :
				break;				
			case END_CORR:
				disable_rtc_second_irq();
				rtc_8m_flag   = TRUE;	
				   rtc_8m_mode  =  WAIT_CORR;
				break;
			default :		
				break;
        }	    
}
#endif



#ifdef PREEN_SWEEP
void ReadPreengage(void)
{
	u32 addr=PREEN_DATA_ADDR;
	for(int i=0;i<PREEN_LENGTH;i++)
		{
			Preen_Data[i].Cycle				=*(vu16*)addr;
			addr+=2;
			Preen_Data[i].Flag   			=*(vu16*)addr;
			addr+=2;
			Preen_Data[i].Hour_done			=*(vu16*)addr;
			addr+=2;
			Preen_Data[i].Preen_Hour		=*(vu16*)addr;
			addr+=2;
			Preen_Data[i].Preen_Min			=*(vu16*)addr;
			addr+=2;
			Preen_Data[i].Preen_Week_Num	=*(vu16*)addr;
			addr+=2;
#ifdef DEBUG_INIT
			TRACE("Preen_Data[%d].Cycle=%d\r\n",i,Preen_Data[i].Cycle);
			TRACE("Preen_Data[%d].Flag=%d\r\n",i,Preen_Data[i].Flag);
			TRACE("Preen_Data[%d].Hour_done=%d\r\n",i,Preen_Data[i].Hour_done);
			TRACE("Preen_Data[%d].Preen_Hour=%d\r\n",i,Preen_Data[i].Preen_Hour);
			TRACE("Preen_Data[%d].Preen_Min=%d\r\n",i,Preen_Data[i].Preen_Min);
			TRACE("Preen_Data[%d].Preen_Week_Num=%d\r\n",i,Preen_Data[i].Preen_Week_Num);
#endif
		}
}

u8 CleanPreenFlashPage(void)
{
	if(!FLASH_GetWriteProtectionOptionByte())
	{		
#ifdef DEBUG_PREEN
		TRACE("FLASH Write Protect!\r\n");
#endif
		return 1;
	}

	
	FLASH_Unlock();
	if(FLASH_ErasePage(PREEN_DATA_ADDR)!=FLASH_COMPLETE)
		{
			return 1;
		}
	FLASH_Lock();
#ifdef DEBUG_PREEN
	TRACE("Flash erase complete\r\n");
#endif


	return 0;
}

u8 CleanPreenData(void)
{
	for(int i=0;i<PREEN_LENGTH;i++)
		{
			Preen_Data[i].Preen_Hour=0;
			Preen_Data[i].Preen_Min=0;
			Preen_Data[i].Preen_Week_Num=0;
			Preen_Data[i].Flag=0;
			if(Preen_Data[i].Preen_Week_Num)
				Preen_Data[i].Cycle=0;
			else
				Preen_Data[i].Cycle=0;
		}
	
	if(CleanPreenFlashPage())		//清除FLASH
		return 1;
	else
		return 0;
}

/************************************************************************
功能：保存预约的时间
*************************************************************************/
//预约成功返回0，不成功返回1
u8  WritePreenData(void)
{

	   if(!FLASH_GetWriteProtectionOptionByte())
	   	{		
#ifdef DEBUG_PREEN
				TRACE("FLASH Write Protect!\r\n");
#endif
				return 1;
	   	}
	   
		if(CleanPreenFlashPage())			//清除之前预约数据
			{
#ifdef DEBUG_PREEN
	   			TRACE("Flash earse error\r\n");
#endif			
				return 1;
			}

		FLASH_Unlock();

	   u32 addr=PREEN_DATA_ADDR;
	   for(int i=0;i<PREEN_LENGTH;i++)
	   	{
	   		if(FLASH_ProgramHalfWord(addr,Preen_Data[i].Cycle)!=FLASH_COMPLETE)
				return 1;
			addr+=2;
			if(FLASH_ProgramHalfWord(addr,Preen_Data[i].Flag)!=FLASH_COMPLETE)
				return 1;
			addr+=2;
			if(FLASH_ProgramHalfWord(addr,Preen_Data[i].Hour_done)!=FLASH_COMPLETE)
				return 1;
			addr+=2;
	   		if(FLASH_ProgramHalfWord(addr,Preen_Data[i].Preen_Hour)!=FLASH_COMPLETE)
				return 1;
			addr+=2;
			if(FLASH_ProgramHalfWord(addr,Preen_Data[i].Preen_Min)!=FLASH_COMPLETE)
				return 1;
			addr+=2;
			if(FLASH_ProgramHalfWord(addr,Preen_Data[i].Preen_Week_Num)!=FLASH_COMPLETE)
				return 1;
			addr+=2;
	   	}

	   FLASH_Lock();

	   ReadPreengage();
#ifdef DEBUG_PREEN
	   TRACE("Flash write complete\r\n");
#endif	  
	   return 0;
}

/************************************************************
核对当前时间是否与预约时间一致，如果是则返回1；否则返回0
************************************************************/
u8 Check_PreengageTime(void)
{
	u8 week_day,piv_hour,piv_min;
	week_day=(Rtc_time)/86400;
    piv_hour = (u8)((Rtc_time % 86400) / 3600); 
    piv_min = (u8)((Rtc_time % 3600) / 60);
	if(week_day==0)
		week_day=(1<<6);			//周日为0100 0000b
	else
		week_day=1<<(week_day-1);
	for(int i=0;i<PREEN_LENGTH;i++)
		{
			if(Preen_Data[i].Flag)											//7个预约时间，每个的标记是否ENABLE,从第一个预约开始检查
				{															//预约时间标记有效
					if((Preen_Data[i].Preen_Hour==piv_hour)&(Preen_Data[i].Preen_Min==piv_min))
						{
							if(!Preen_Data[i].Cycle)
								{
									Preen_Data[i].Flag=0;
									CleanPreenData();			//qz add 20180428:Flag被清零，表示单次的预约完成，Flag被更改，需要重新保存，
																//不保存的话，如果中途被关机了，重新开机时，读到的预约数据Flag仍然为1，时间到后，又会开始。
									return 1;
								}
							else if(week_day&Preen_Data[i].Preen_Week_Num)
								{
									Preen_Data[i].Hour_done=1;
									return 1;
								}
						}
					return 0;
				}
		}
	return 0;																//一个预约时间都没有到，返回0
}
#endif

/************************************************************************
功能：写实时时钟时间
*************************************************************************/
	void  WriteRtcTime(void)
	{
#ifndef DS1307
		PWR_BackupAccessCmd(ENABLE);
		RTC_WaitForLastTask();
		RTC_SetCounter(Rtc_time);
		RTC_WaitForLastTask();
		PWR_BackupAccessCmd(DISABLE);
#else
		RtcTimetoDS1307();
#endif
	
	}

/************************************************************************
功能：保存工作状态
*************************************************************************/
void  WriteWorkState(void)
{
#ifndef DS1307
	u16 s=0;
	s=(mode.mode<<8)|(mode.sub_mode);
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(MODE_BACKUP_BKP, s);
	PWR_BackupAccessCmd(DISABLE);
#else
	DS1307_Write_Backup(MODE_BACKUP_ADDR, mode.mode);
	DS1307_Write_Backup(SUBMODE_BACKUP_ADDR,mode.sub_mode);
#endif


}

/************************************************************************
功能：保存放电容量
*************************************************************************/
void  WriteFDCap(void)
{
#ifndef DS1307
	u16 s;
	PWR_BackupAccessCmd(ENABLE);
	
	s = (u16)(Battery.BatteryFDCap&0xffff);
	BKP_WriteBackupRegister(BATFDCAP_LADDR_BKP, s);
	s = (u16)(Battery.BatteryFDCap/0x10000);
	if(Battery.BatteryChargeForbid != 0)
		{
			s |= 0x8000;
		}
	BKP_WriteBackupRegister(BATFDCAP_HADDR_BKP, s);
	PWR_BackupAccessCmd(DISABLE);
#else
	DS1307_WriteBat_Backup(BAT_FDCAP_ADDR,Battery.BatteryFDCap);
#endif
}

u32 ReadFDCap(void)
{
	u32 data32=0;
#ifndef DS1307
	u16 data1,data2;
	PWR_BackupAccessCmd(ENABLE);
	data1=BKP_ReadBackupRegister(BATFDCAP_HADDR_BKP);
	data2=BKP_ReadBackupRegister(BATFDCAP_LADDR_BKP);
	data32=((u32)(data1<<16))|((u32)(data2));
	PWR_BackupAccessCmd(DISABLE);
#else
	DS1307_Readu32data(BAT_FDCAP_ADDR,&data32);
#endif
	return data32;
}

u32  ReadBatteryCapability(void)
{
	u32 data32=0;
#ifndef DS1307
	u16 data1,data2;
	PWR_BackupAccessCmd(ENABLE);
	data1=BKP_ReadBackupRegister(BATCAP_HADDR_BKP);
	data2=BKP_ReadBackupRegister(BATCAP_LADDR_BKP);
	data32=((u32)(data1<<16))|((u32)(data2));
	PWR_BackupAccessCmd(DISABLE);
#else
	DS1307_Readu32data(BAT_CAP_ADDR,&data32);
#endif
	return data32;	
}

/************************************************************************
功能：保存电池容量
*************************************************************************/
void  WriteBatteryCapability(void)
{
#ifndef DS1307
			u16 s;
		PWR_BackupAccessCmd(ENABLE);
		s = (u16)(Battery.BatteryCapability&0xffff);
		BKP_WriteBackupRegister(BATCAP_LADDR_BKP, s);
		s = (u16)(Battery.BatteryCapability/0x10000);
		BKP_WriteBackupRegister(BATCAP_HADDR_BKP, s);
			PWR_BackupAccessCmd(DISABLE);
#else
		DS1307_WriteBat_Backup(BAT_CAP_ADDR,Battery.BatteryCapability);
#endif
}
/************************************************************************
功能：定时读取实时时钟的值
************************************************************************/
	void ReadRealTime(void)
	{
		if(Sec != false)
			{
					Sec = false; 
#ifndef DS1307
					u32 t;			
					t =  RTC_GetCounter();
					Rtc_time = t;
					if(Rtc_time >= 604800)		 //一周=7*24*60*60=604800(s)
						{
								Rtc_time = Rtc_time % 604800;
								PWR_BackupAccessCmd(ENABLE);
								RTC_WaitForLastTask();
								RTC_SetCounter(Rtc_time);
								RTC_WaitForLastTask();
								PWR_BackupAccessCmd(DISABLE);
						}
#else
				//qz add 20181026
				//因为使用了DS1307,它自带电池,所以无需给它校准时间,只需自身校准即可
				Rtc_time++;
				if(Rtc_time>=604800)
					{
						Rtc_time=Rtc_time % 604800;
					}
#endif
			}
			
		if( watchdog_time == true)
			{	
				IWDG_ReloadCounter();
				watchdog_time = false;
			}
			
			
	}

void WriteBatInitFlash(void)
{
#ifndef DS1307
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(BAT_REINIT_BKP,0);
	PWR_BackupAccessCmd(DISABLE);
#else
	DS1307_Writeu8data(FLAG_BACKUP_ADDR, 0X0);
#endif
}

u16 ReadBatRecal(void)
{
#ifndef DS1307
#if 0					//使用内部FLASH
	u32 data1;
	u32 addr=BAT_RECAL_ADDR;
	data1=*(vu32*)addr;
	return(data1&0x0000FFFF);
#else					//使用BKP
	u16 data1;
	PWR_BackupAccessCmd(ENABLE);
	data1=BKP_ReadBackupRegister(BATRECAL_BACKUP_BKP);
	PWR_BackupAccessCmd(DISABLE);
	return data1;
#endif
#else					//使用DS1307
	u8 data1=0;
	DS1307_Read_Backup(BAT_RECAL_ADDR,&data1);
	return (u16)(data1);
#endif
}

void WriteBatRecal(u16 data)
{
#ifndef DS1307
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(BKP_DR9,data);
	PWR_BackupAccessCmd(DISABLE);
#else					//使用DS1307
	DS1307_Write_Backup(BAT_RECAL_ADDR, (u8)(data));
#endif
}

void Set_BatReinit(void)
{
}

