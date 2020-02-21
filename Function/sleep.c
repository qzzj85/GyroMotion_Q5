//=====================头文件====================================
#include "AAA-Include.h"

////////////////////////私有定义//////////////////////////////////

  #ifdef  	DEBUG   
  #define   DBGMCU_CR ((uint32 *)0xe0042004)
  #endif   
	
////////////////////////全局变量//////////////////////////////////
bool ExitInput;     //有按键、外部电源外部输入
bool key_wakeup_flag=false;
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////	  
void Init_Sleep(void); 
void Do_Sleep(void);
///////////////////////私有函数////////////////////////////////////	
void RCC_EXITSTOP(void);
///////////////////////函数实体////////////////////////////////////
/******************************************************************
功能：初始化充电程序
******************************************************************/
void Init_Sleep(void)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
	/******初始化设置的值********************/
	stop_rap();				//qz add关闭左右轮
	
	mode.mode = MODE_CEASE; 
	mode.Info_Abort=0;				//打开SLAM通信，qz add 20180418
	mode.sub_mode=SLEEP;			//QZ ADD
	mode.step=0x00;					//qz add
	mode.status=0;					//非工作状态
	mode.time=giv_sys_time;			//qz add 20180703

	motion1.sweep_done=false;
	Disable_earth();
	Disable_wall();
	enable_hwincept();								//禁止红外接收电源
	Enable_Speed();  								//关闭红外灯光发送 
	WriteWorkState(); 

	Sweep_Level_Set(SWEEP_LEVEL_STOP);						//关闭风机

	CHECK_STATUS_FLAG=false;						//禁止异常检测
	Init_Check_Status();
#ifdef DEBUG_Enter_Mode
	TRACE("Init sleep submode complete!\r\n");
#endif

#ifdef UV
	Reset_UV();
#endif
//	Send_Voice(VOICE_SLEEP_START);
	key_wakeup_flag=false;

}
/******************************************************************
功能：执行充电
******************************************************************/

//qz add
void Do_Sleep_My(void)
{
	uint32 t;
	uint8 s=0;

	if(power.charge_dc|power.charge_seat)
		{
			if(power.charge_seat)
				Init_Chargeing(SEAT_CHARGING);
			if(power.charge_dc)
				Init_Chargeing(DC_CHARGING);
		}
	
			 //在调试时允许在停机状态下调试
	//	#ifdef	DEBUG	
	 *((uint32 *)0xe0042004) |= 2;
	//	#endif	 
	switch (mode.step)
		{
			case 0:
				if(giv_sys_time-mode.time<10000)
					return;
				PWR5V_OFF;
				PWR3V3_OFF;
				Disable_earth();
				Disable_wall();
				Disable_Speed();
				disable_hwincept();
				BAT_CHECK_0;
				LED_RED_OFF;
				LED_GREEN_OFF;
				ADC_Cmd(ADC1,DISABLE);
				GPIOA->CRL=0x44444444;
				GPIOA->CRH=0X44444444;
				GPIOB->CRL=0x44444444;
				GPIOB->CRH=0X44444444;
				GPIOC->CRL=0x44444444;
				GPIOC->CRH=0X44444444;
				GPIOD->CRL=0x44444444;
				GPIOD->CRH=0X44444444;
				GPIOE->CRL=0x44444444;
				GPIOE->CRH=0X44444444;
				mode.step++;
				break;
			case 1:
#ifdef STOP_WEEKUP										
				Enable_ExternInt_Weekup(1); //可以使用KEY1,直充,座充唤醒				
#else
				Diable_AllExtInt_Weekup();	//屏蔽所有外部中断，无法唤醒
#endif
				IWDG_ReloadCounter();
				IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
				IWDG_SetPrescaler(IWDG_Prescaler_256);//使用
				IWDG_SetReload(1000);
				IWDG_ReloadCounter();
				while(1)
					{
						  ///////设置闹钟中断时间////////
						 t = (RTC_GetCounter() + 2);
						 PWR_BackupAccessCmd(ENABLE);
						 RTC_WaitForLastTask();
						 RTC_SetAlarm(t);
						 RTC_WaitForLastTask();
						 PWR_BackupAccessCmd(DISABLE);
					  
						 PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); 

#ifndef USE_HSE				
						 RCC_EXITSTOP();
#endif
						 IWDG_ReloadCounter();
						 //读取实时时钟值////////////
						 Rtc_time = RTC_GetCounter();
						 if(Rtc_time >= 604800)
							  {
									 Rtc_time = Rtc_time % 604800;
									 PWR_BackupAccessCmd(ENABLE);
									 RTC_WaitForLastTask();
									 RTC_SetCounter(Rtc_time);
									 RTC_WaitForLastTask();
									 PWR_BackupAccessCmd(DISABLE);
							  }
						 t = giv_sys_time;
						 s = 0;
						 while((giv_sys_time - t) < 5000)
						 {
							 judge_charge();
							 /////有充电的电源插入
							 
							 if((power.charge_dc == 1) || (power.charge_seat == 1))
								 {
									 s = 1;
									 break;
								 }
							 if(!Read_Key2())
								 {
									 s = 2;
									 break;
								 }
							 if(key_wakeup_flag)
							 	{
							 		s=2;
									break;
							 	}
						 }
						 if(s>0)
						 {
							 break;
						 }
						 //AccountCapabilityReal(); //计算耗电量
					  
				  }
#if 0					
				RCC_Configuration();		///////////初始化系统的晶振，如有移植需要修改
				Battery_Data_Init();
				Init_PWM(); 			//qz add 20180703,不然风扇会转
				init_time_2();			////////	//	Timer2	   10K 中断	  计数器
				Init_Hardware();
//				init_hwincept();				///////////初始化红外接收程序
				Init_Bump_Interrupt();
				Init_Time_1();
				//Init_Lcd();
				init_ad();					//重新初始化AD
				Del_AllNode();		//删除之前的所有语音
				Init_Chargeing(CHARGEING);

				//qz add 20180901
				if(power.charge_seat)
				   Init_Chargeing(SEAT_CHARGING);
				if(power.charge_dc)
				   Init_Chargeing(DC_CHARGING);
				//qz add end
#endif
				key_wakeup_flag=false;
				PWR5V_ON;
				PWR3V3_ON;
				Init_System();
			}
	
}



/*************************************************************
功能：从停止模式退出时的时钟配置。
输入：无
输出：无
*************************************************************/
void RCC_EXITSTOP(void)
{
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			{}
	/* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    /* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
			{}
}






















