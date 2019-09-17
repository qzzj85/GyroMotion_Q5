//=====================头文件====================================
#include "AAA-Include.h"

////////////////////////私有定义//////////////////////////////////

  #ifdef  	DEBUG   
  #define   DBGMCU_CR ((uint32 *)0xe0042004)
  #endif   
	
////////////////////////全局变量//////////////////////////////////
bool ExitInput;     //有按键、外部电源外部输入
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////	  
void Init_Sleep(void); 
void Do_Sleep(void);
///////////////////////私有函数////////////////////////////////////	
void RCC_EXITSTOP(void);
void Enable_KeyIni(void);
void Disable_KeyIni(void);
void Enable_YaoKongIni(void);
void Disable_YaoKongIni(void);
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
	
	mode.mode = CEASE; 
	mode.Info_Abort=0;				//打开SLAM通信，qz add 20180418
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.sub_mode=SLEEP;			//QZ ADD
	mode.step=0x00;					//qz add
	mode.status=0;					//非工作状态
	mode.time=giv_sys_time;			//qz add 20180703
	Slam_Data.no_msg=false;			//需要向导航板上传数据
	Slam_Data.dipan_req_pre=1;		//qz mask 20180522：默认为规划清扫
	
	Disable_earth();
	Disable_wall();
	enable_hwincept();								//禁止红外接收电源
	Enable_Speed();  								//关闭红外灯光发送 
	WriteWorkState(); 

	Sweep_Level_Set(STOP_ALL);						//关闭风机

	CHECK_STATUS_FLAG=false;						//禁止异常检测
	Init_Check_Status();
#ifdef DEBUG_Enter_Mode
	TRACE("Init sleep submode complete!\r\n");
#endif

#ifdef UV
	Reset_UV();
#endif
//	Send_Voice(VOICE_SLEEP_START);
}
/******************************************************************
功能：执行充电
******************************************************************/

//qz add
void Do_Sleep_My(void)
{
#ifdef DC_NOBAT_RUN
	  if(((power.charge_dc)|(power.charge_seat))&(!dc_nobat_run))
#else
	  if(power.charge_dc|power.charge_seat)
#endif
		  {
			   //Init_Chargeing(CHARGEING); 

			   //qz add 20180901
			   if(power.charge_seat)
				  Init_Chargeing(SEAT_CHARGING);
			   if(power.charge_dc)
				  Init_Chargeing(DC_CHARGING);
			   //qz add end
			   return;
		  }

	if((!key1.key)|(!key2.key)|(!key3.key))
		{
			Init_Cease();
		}

#ifdef SHUTDOWN_MODE	//qz add 20180901
	if((giv_sys_time-mode.time>4200000))
		{
//			Slam_Data.dipan_req=DIPAN_REQ_TURNOFFSLAM;
			Init_ShutDown();
		}
#endif
}
void Do_Sleep(void)
{
u32 t;
u8 s;
  //在调试时允许在停机状态下调试
  #ifdef  DEBUG   
  *((uint32 *)0xe0042004) |= 2;
  #endif   
   Set_LCD_Power();
   Set_Wall_Send();              //关闭发射
   Set_Earth_Send();             //关闭发射
   //Reset_Beep();
   ADC_Cmd(ADC1, DISABLE);
   	  
      IWDG_ReloadCounter();
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetPrescaler(IWDG_Prescaler_256);//使用
      IWDG_SetReload(1000);
      IWDG_ReloadCounter();
   for(;;)
								 {
										Enable_KeyIni();
							//	   Disable_YaoKongIni();
							//     StrobeCmd(CMD_SLEEP);	
										 ///////设置闹钟中断时间////////
										 t = (RTC_GetCounter() + 2);
										PWR_BackupAccessCmd(ENABLE);
										RTC_WaitForLastTask();
										RTC_SetAlarm(t);
										RTC_WaitForLastTask();
										PWR_BackupAccessCmd(DISABLE);
									 
										PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); 
										RCC_EXITSTOP();
										IWDG_ReloadCounter();
							//	   StrobeCmd(CMD_RX);         //进入接收模式 
										 Disable_KeyIni();
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
										AccountCapabilityReal(); //计算耗电量
									 
									 /////////机器的预约时间到，机器进入扫地模式//
										 if(Check_PreengageTime())
												 {
														//Init_Lcd();
														break;
												 }
										 //有遥控器电源按键按下
//										 if(gsv_yaokong.sign == 1)
//												 {
//														gsv_yaokong.sign = 0;
//														if(gsv_yaokong.key == YAOKONG_ESC)
//																{
//																	Init_Lcd();
//																	Init_Cease();
//																	break;
//																}
//												 }
//										 gbv_a7105_reset = true;
										 EXTI_GenerateSWInterrupt(EXTI_Line0);
										 if(ExitInput != false)
												 {
														ExitInput = false;
														t = giv_sys_time;
														s = 0;
														while((giv_sys_time - t) < 5000)
																 {
																		AutoReadKey();
																		judge_charge();
																		///在面板上有三个按键中的任意一个按下了
																		if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
																			 {
																					 s = 1;
																				 key1.sign = 1;
																				 key2.sign = 1;
																				 key3.sign = 1;
																				 //Init_Lcd();
																				 Init_Cease();
																				 break;
																			 }
																		 /////有充电的电源插入
																		if((power.charge_dc == 1) || (power.charge_seat == 1))
																			 {
																					 //Init_Lcd(); 
																				 Init_Chargeing(CHARGEING);
																				 s = 1;
																				 break;
																			 }
																 }
														 if(s == 1)
																 {
																		 break;
																 }
												 }
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
/************************************************************
功能：允许三个按键中断
输入：无
输出：无
************************************************************/
void Enable_KeyIni(void)
{
	EXTI->IMR |= 0x180e;
}
/************************************************************
功能：禁止三个按键中断
输入：无
输出：无
************************************************************/
void Disable_KeyIni(void)
{
	EXTI->IMR &= 0xffffE7f1;
}
/************************************************************
功能：允许三个按键中断
输入：无
输出：无
************************************************************/
void Enable_YaoKongIni(void)
{
	EXTI->IMR |= 0x1;
}
/************************************************************
功能：禁止三个按键中断
输入：无
输出：无
************************************************************/
void Disable_YaoKongIni(void)
{
	EXTI->IMR &= 0xfffffffe;
}
//==================================================================================
//==================================================================================



























