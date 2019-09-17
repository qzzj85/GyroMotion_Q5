//=====================ͷ�ļ�====================================
#include "AAA-Include.h"

////////////////////////˽�ж���//////////////////////////////////

  #ifdef  	DEBUG   
  #define   DBGMCU_CR ((uint32 *)0xe0042004)
  #endif   
	
////////////////////////ȫ�ֱ���//////////////////////////////////
bool ExitInput;     //�а������ⲿ��Դ�ⲿ����
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////	  
void Init_Sleep(void); 
void Do_Sleep(void);
///////////////////////˽�к���////////////////////////////////////	
void RCC_EXITSTOP(void);
void Enable_KeyIni(void);
void Disable_KeyIni(void);
void Enable_YaoKongIni(void);
void Disable_YaoKongIni(void);
///////////////////////����ʵ��////////////////////////////////////
/******************************************************************
���ܣ���ʼ��������
******************************************************************/
void Init_Sleep(void)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	/******��ʼ����ʾ***********/
	/******��ʼ�����õ�ֵ********************/
	stop_rap();				//qz add�ر�������
	
	mode.mode = CEASE; 
	mode.Info_Abort=0;				//��SLAMͨ�ţ�qz add 20180418
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.sub_mode=SLEEP;			//QZ ADD
	mode.step=0x00;					//qz add
	mode.status=0;					//�ǹ���״̬
	mode.time=giv_sys_time;			//qz add 20180703
	Slam_Data.no_msg=false;			//��Ҫ�򵼺����ϴ�����
	Slam_Data.dipan_req_pre=1;		//qz mask 20180522��Ĭ��Ϊ�滮��ɨ
	
	Disable_earth();
	Disable_wall();
	enable_hwincept();								//��ֹ������յ�Դ
	Enable_Speed();  								//�رպ���ƹⷢ�� 
	WriteWorkState(); 

	Sweep_Level_Set(STOP_ALL);						//�رշ��

	CHECK_STATUS_FLAG=false;						//��ֹ�쳣���
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
���ܣ�ִ�г��
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
  //�ڵ���ʱ������ͣ��״̬�µ���
  #ifdef  DEBUG   
  *((uint32 *)0xe0042004) |= 2;
  #endif   
   Set_LCD_Power();
   Set_Wall_Send();              //�رշ���
   Set_Earth_Send();             //�رշ���
   //Reset_Beep();
   ADC_Cmd(ADC1, DISABLE);
   	  
      IWDG_ReloadCounter();
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetPrescaler(IWDG_Prescaler_256);//ʹ��
      IWDG_SetReload(1000);
      IWDG_ReloadCounter();
   for(;;)
								 {
										Enable_KeyIni();
							//	   Disable_YaoKongIni();
							//     StrobeCmd(CMD_SLEEP);	
										 ///////���������ж�ʱ��////////
										 t = (RTC_GetCounter() + 2);
										PWR_BackupAccessCmd(ENABLE);
										RTC_WaitForLastTask();
										RTC_SetAlarm(t);
										RTC_WaitForLastTask();
										PWR_BackupAccessCmd(DISABLE);
									 
										PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); 
										RCC_EXITSTOP();
										IWDG_ReloadCounter();
							//	   StrobeCmd(CMD_RX);         //�������ģʽ 
										 Disable_KeyIni();
										//��ȡʵʱʱ��ֵ////////////
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
										AccountCapabilityReal(); //����ĵ���
									 
									 /////////������ԤԼʱ�䵽����������ɨ��ģʽ//
										 if(Check_PreengageTime())
												 {
														//Init_Lcd();
														break;
												 }
										 //��ң������Դ��������
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
																		///������������������е�����һ��������
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
																		 /////�г��ĵ�Դ����
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
���ܣ���ֹͣģʽ�˳�ʱ��ʱ�����á�
���룺��
�������
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
���ܣ��������������ж�
���룺��
�������
************************************************************/
void Enable_KeyIni(void)
{
	EXTI->IMR |= 0x180e;
}
/************************************************************
���ܣ���ֹ���������ж�
���룺��
�������
************************************************************/
void Disable_KeyIni(void)
{
	EXTI->IMR &= 0xffffE7f1;
}
/************************************************************
���ܣ��������������ж�
���룺��
�������
************************************************************/
void Enable_YaoKongIni(void)
{
	EXTI->IMR |= 0x1;
}
/************************************************************
���ܣ���ֹ���������ж�
���룺��
�������
************************************************************/
void Disable_YaoKongIni(void)
{
	EXTI->IMR &= 0xfffffffe;
}
//==================================================================================
//==================================================================================



























