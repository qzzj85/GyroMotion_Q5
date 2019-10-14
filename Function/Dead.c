#include "AAA-include.h"
////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////	  
void Init_Dead(void); 
void Do_Dead(void);
///////////////////////˽�к���////////////////////////////////////	
///////////////////////����ʵ��////////////////////////////////////
/******************************************************************
���ܣ���ʼ��������
******************************************************************/
void Init_Dead(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
    /******��ʼ����ʾ***********/
	/******��ʼ�����õ�ֵ********************/
	clr_ram();

	stop_rap();						//ֹͣ����
	Sweep_Level_Set(STOP_ALL);		//�رշ��
	Disable_earth();
	Disable_wall();
	disable_hwincept();//��ֹ������յ�Դ
	Disable_Speed();  //��ֹ����ƹⷢ��
//	mode.mode = ERR;
	mode.mode = CEASE;
	mode.Info_Abort =1;			//��ֹSLAMͨ��
	mode.All_Info_Abort=0;			//qz add 20180919	//qz modify 20181110
	mode.sub_mode=DEAD;		//��ģʽΪERR
	mode.step=0;
	mode.status=0;			//qz add 20180422
	mode.time=giv_sys_time;
	WriteWorkState();

	CHECK_STATUS_FLAG=false;		//��ֹ�쳣���
	Slam_Data.tick_check_step=0;	//����SLAM_TICK���
	REYBS_TIME=0;					//qz add 20180910,С�س����������رߴ�����0
#ifdef UV
	Reset_UV();
#endif

#ifdef DEBUG_Enter_Mode
	TRACE("Init Dead Mode Complete!\r\n");
#endif
	Send_Voice(VOICE_POWER_NONE);	//qz add 20181201
	//RST_S2D_1;			//qz add 20181110:��ֹ�����帴λ����
	Close_Led();
}

//qz add 20180901
void Init_ShutDown(void)
{
		mode.last_mode=mode.mode;
		mode.last_sub_mode=mode.sub_mode;
		/******��ʼ����ʾ***********/
		/******��ʼ�����õ�ֵ********************/
		clr_ram();
	
		stop_rap(); 					//ֹͣ����
		Sweep_Level_Set(STOP_ALL);		//�رշ��
		Disable_earth();
		Disable_wall();
		disable_hwincept();//��ֹ������յ�Դ
		Disable_Speed();  //��ֹ����ƹⷢ��
	//	mode.mode = ERR;
		mode.mode = CEASE;
		mode.Info_Abort =1; 		//��ֹSLAMͨ��
		mode.All_Info_Abort=0;			//qz add 20180919	//qz modify 20181110
		mode.sub_mode=SHUTDOWN; 	//��ģʽΪERR
		mode.step=0;
		mode.status=0;			//qz add 20180422
		mode.time=giv_sys_time;
		WriteWorkState();
	
		CHECK_STATUS_FLAG=false;		//��ֹ�쳣���
		Slam_Data.tick_check_step=0;	//����SLAM_TICK���

#ifdef UV
		Reset_UV();
#endif
#ifdef DEBUG_Enter_Mode
		TRACE("Init ShutDown Mode Complete!\r\n");
#endif
		//RST_S2D_1;			//qz add 20181110:��ֹ�����帴λ����
		Close_Led();
	key_wakeup_flag=false;
}
/******************************************************************
���ܣ�ִ�г��
******************************************************************/
void Do_Dead(void)
{
	uint32 t;
	uint8 s;

	if(power.charge_dc|power.charge_seat)
		{
			if(power.charge_seat)
				Init_Chargeing(SEAT_CHARGING);
			if(power.charge_dc)
				Init_Chargeing(DC_CHARGING);
		}

		 //�ڵ���ʱ������ͣ��״̬�µ���
//  #ifdef  DEBUG   
		 *((uint32 *)0xe0042004) |= 2;
//  #endif   
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

void Enable_ExternInt_Weekup(u8 use_key)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	EXTI->IMR&=0X00000000;
	EXTI->IMR|=0X00021500;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	if(use_key)
		{
			GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
			GPIO_InitStructure.GPIO_Pin=KEY_2;
			GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
			GPIO_Init(GPIO_KEY2,&GPIO_InitStructure);
			GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource10);
		}

	/*PP2��ʱ���ܲ������ػ���·�����Խ���DEAD�͹����Ժ�Ϊ���ܹ�
	ֱ�ӳ�磬������DC����SEAT���Ϊ�ⲿ�жϣ�����ֱ�ӻ��ѳ��*/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;				
	GPIO_InitStructure.GPIO_Pin=CHARGE_DC;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DC,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=CHARGE_SEAT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_SEAT,&GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);

	if(use_key)
		EXTI_InitStructure.EXTI_Line = EXTI_Line10|EXTI_Line_DC|EXTI_Line_SEAT;
	else
		EXTI_InitStructure.EXTI_Line = EXTI_Line_DC|EXTI_Line_SEAT;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 		 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	if(use_key)
		EXTI_ClearITPendingBit(EXTI_Line10);
	EXTI_ClearITPendingBit(EXTI_Line_DC);
	EXTI_ClearITPendingBit(EXTI_Line_SEAT);
}

void Disable_ExternInt_Weekup(void)
{
	EXTI->IMR&=0;
}

void Diable_AllExtInt_Weekup(void)
{
	EXTI->IMR&=0;
}

