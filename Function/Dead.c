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
	
}
/******************************************************************
���ܣ�ִ�г��
******************************************************************/
void Do_Dead(void)
{
#ifndef DS1307
	   uint32 t;
	   uint8 s;
#endif

	if(power.charge_dc|power.charge_seat)
		{
			//Init_Lcd();
			Init_Chargeing(CHARGEING);
				 
			//qz add 20180901
			if(power.charge_seat)
				Init_Chargeing(SEAT_CHARGING);
			if(power.charge_dc)
				Init_Chargeing(DC_CHARGING);
			//qz add end
				 
		}

		 //�ڵ���ʱ������ͣ��״̬�µ���
//  #ifdef  DEBUG   
		 *((uint32 *)0xe0042004) |= 2;
//  #endif   
	   switch (mode.step)
	   	{
	   		case 0:
				//qz add 20181110
				if(Slam_Data.tick_flag)				//���յ�SLAM��������ʶSLAM�����������Է��͹ػ�����
					{
						Slam_Data.dipan_req=DIPAN_REQ_TURNOFFSLAM;
						mode.time=giv_sys_time;
						mode.step++;
						break;
					}
				if(giv_sys_time-mode.time>1800000)	//����3min��ֱ��ȥ�͹���
					{
						mode.step=2;
						mode.time=giv_sys_time;
#ifdef DS1307
						ENABLE_PWROFF;
						Reset_GyroPwr();
#endif
					}
				break;
			case 1:
				if(giv_sys_time-mode.time>50000)	//qz add 20181110  wait 5s
				//if(SLAM_Tick_Check())				//������ֹͣ���������Ժ�
					{
						//Slam_Data.no_msg=true;		//ֹͣ�򵼺����ϴ���Ϣ
						mode.step++;
						mode.time=giv_sys_time;
						//Turnoff_AllUart();
#ifdef DS1307
						ENABLE_PWROFF;
						Reset_GyroPwr();
#endif
						
					}

				break;
			case 2:
#ifdef DS1307
				if(giv_sys_time-mode.time<5000)				//qz add 20181110
					{
						break;
					}
#ifdef STOP_WEEKUP										
				Enable_ExternInt_Weekup();	//����ʹ��KEY1,ֱ��,���份��				
#else
				Diable_AllExtInt_Weekup();	//���������ⲿ�жϣ��޷�����
#endif
				ADC_Cmd(ADC1, DISABLE);
				PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
//				RCC_EXITSTOP();

				Disable_ExternInt_Weekup();
				Init_System();
			break;
#else				   
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
						 }
						 if(s == 1)
						 {
							 break;
						 }
						 //AccountCapabilityReal(); //����ĵ���
					  
				  }   
				RCC_Configuration();		///////////��ʼ��ϵͳ�ľ���������ֲ��Ҫ�޸�
				Battery_Data_Init();
				Init_PWM();				//qz add 20180703,��Ȼ���Ȼ�ת
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
	   	}

}

void Enable_ExternInt_Weekup(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

//	EXTI_DeInit();
	
#if 0
	GPIOA->CRH&=0X0FFFFFFF;
	GPIOA->CRH|=0X80000000;
	AFIO->EXTICR[3]&=0XFFFF0000;
	AFIO->EXTICR[3]&=0XFFFF1000;

	EXTI->IMR&=0X00000000;
	EXTI->IMR|=0X00008000;
	EXTI->RTSR&=0X00000000;
	EXTI->RTSR|=0X00008000;
	EXTI->FTSR&=0X00000000;
	EXTI->FTSR|=0X00008000;
	EXTI->PR=0X00000000;
#endif
	EXTI->IMR&=0X00000000;
	EXTI->IMR&=0X00009800;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin=KEY_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_KEY2,&GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);

	/*PP2��ʱ���ܲ������ػ���·�����Խ���DEAD�͹����Ժ�Ϊ���ܹ�
	ֱ�ӳ�磬������DC����SEAT���Ϊ�ⲿ�жϣ�����ֱ�ӻ��ѳ��*/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;				
	GPIO_InitStructure.GPIO_Pin=CHARGE_DC;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=CHARGE_SEAT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2|EXTI_Line8|EXTI_Line12;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 		 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void Disable_ExternInt_Weekup(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI->IMR&=0XFFFF7FFF;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

void Diable_AllExtInt_Weekup(void)
{
	EXTI->IMR&=0;
}

