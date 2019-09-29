//=====================ͷ�ļ�====================================
#include "AAA-Include.h"

////////////////////////˽�ж���//////////////////////////////////

  #ifdef  	DEBUG   
  #define   DBGMCU_CR ((uint32 *)0xe0042004)
  #endif   
	
////////////////////////ȫ�ֱ���//////////////////////////////////
bool ExitInput;     //�а������ⲿ��Դ�ⲿ����
bool key_wakeup_flag=false;
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
	key_wakeup_flag=false;

}
/******************************************************************
���ܣ�ִ�г��
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
	
			 //�ڵ���ʱ������ͣ��״̬�µ���
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
				Enable_ExternInt_Weekup(1); //����ʹ��KEY1,ֱ��,���份��				
#else
				Diable_AllExtInt_Weekup();	//���������ⲿ�жϣ��޷�����
#endif
				IWDG_ReloadCounter();
				IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
				IWDG_SetPrescaler(IWDG_Prescaler_256);//ʹ��
				IWDG_SetReload(1000);
				IWDG_ReloadCounter();
				while(1)
					{
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
						 t = giv_sys_time;
						 s = 0;
						 while((giv_sys_time - t) < 5000)
						 {
							 judge_charge();
							 /////�г��ĵ�Դ����
							 
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
						 //AccountCapabilityReal(); //����ĵ���
					  
				  }
#if 0					
				RCC_Configuration();		///////////��ʼ��ϵͳ�ľ���������ֲ��Ҫ�޸�
				Battery_Data_Init();
				Init_PWM(); 			//qz add 20180703,��Ȼ���Ȼ�ת
				init_time_2();			////////	//	Timer2	   10K �ж�	  ������
				Init_Hardware();
//				init_hwincept();				///////////��ʼ��������ճ���
				Init_Bump_Interrupt();
				Init_Time_1();
				//Init_Lcd();
				init_ad();					//���³�ʼ��AD
				Del_AllNode();		//ɾ��֮ǰ����������
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



























