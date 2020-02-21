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
	
	mode.mode = MODE_CEASE; 
	mode.Info_Abort=0;				//��SLAMͨ�ţ�qz add 20180418
	mode.sub_mode=SLEEP;			//QZ ADD
	mode.step=0x00;					//qz add
	mode.status=0;					//�ǹ���״̬
	mode.time=giv_sys_time;			//qz add 20180703

	motion1.sweep_done=false;
	Disable_earth();
	Disable_wall();
	enable_hwincept();								//��ֹ������յ�Դ
	Enable_Speed();  								//�رպ���ƹⷢ�� 
	WriteWorkState(); 

	Sweep_Level_Set(SWEEP_LEVEL_STOP);						//�رշ��

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

#ifndef USE_HSE				
						 RCC_EXITSTOP();
#endif
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






















