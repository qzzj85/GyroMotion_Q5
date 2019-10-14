#include "AAA-include.h"
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////	  
void Init_Dead(void); 
void Do_Dead(void);
///////////////////////私有函数////////////////////////////////////	
///////////////////////函数实体////////////////////////////////////
/******************************************************************
功能：初始化充电程序
******************************************************************/
void Init_Dead(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
    /******初始化显示***********/
	/******初始化设置的值********************/
	clr_ram();

	stop_rap();						//停止轮子
	Sweep_Level_Set(STOP_ALL);		//关闭风机
	Disable_earth();
	Disable_wall();
	disable_hwincept();//禁止红外接收电源
	Disable_Speed();  //禁止红外灯光发送
//	mode.mode = ERR;
	mode.mode = CEASE;
	mode.Info_Abort =1;			//禁止SLAM通信
	mode.All_Info_Abort=0;			//qz add 20180919	//qz modify 20181110
	mode.sub_mode=DEAD;		//子模式为ERR
	mode.step=0;
	mode.status=0;			//qz add 20180422
	mode.time=giv_sys_time;
	WriteWorkState();

	CHECK_STATUS_FLAG=false;		//禁止异常检测
	Slam_Data.tick_check_step=0;	//重启SLAM_TICK检测
	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
#ifdef UV
	Reset_UV();
#endif

#ifdef DEBUG_Enter_Mode
	TRACE("Init Dead Mode Complete!\r\n");
#endif
	Send_Voice(VOICE_POWER_NONE);	//qz add 20181201
	//RST_S2D_1;			//qz add 20181110:禁止导航板复位底盘
	Close_Led();
}

//qz add 20180901
void Init_ShutDown(void)
{
		mode.last_mode=mode.mode;
		mode.last_sub_mode=mode.sub_mode;
		/******初始化显示***********/
		/******初始化设置的值********************/
		clr_ram();
	
		stop_rap(); 					//停止轮子
		Sweep_Level_Set(STOP_ALL);		//关闭风机
		Disable_earth();
		Disable_wall();
		disable_hwincept();//禁止红外接收电源
		Disable_Speed();  //禁止红外灯光发送
	//	mode.mode = ERR;
		mode.mode = CEASE;
		mode.Info_Abort =1; 		//禁止SLAM通信
		mode.All_Info_Abort=0;			//qz add 20180919	//qz modify 20181110
		mode.sub_mode=SHUTDOWN; 	//子模式为ERR
		mode.step=0;
		mode.status=0;			//qz add 20180422
		mode.time=giv_sys_time;
		WriteWorkState();
	
		CHECK_STATUS_FLAG=false;		//禁止异常检测
		Slam_Data.tick_check_step=0;	//重启SLAM_TICK检测

#ifdef UV
		Reset_UV();
#endif
#ifdef DEBUG_Enter_Mode
		TRACE("Init ShutDown Mode Complete!\r\n");
#endif
		//RST_S2D_1;			//qz add 20181110:禁止导航板复位底盘
		Close_Led();
	key_wakeup_flag=false;
}
/******************************************************************
功能：执行充电
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

		 //在调试时允许在停机状态下调试
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
				
						 RCC_EXITSTOP();
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

	/*PP2暂时可能不会加入关机电路，所以进入DEAD低功耗以后，为了能够
	直接充电，加入了DC检测和SEAT检测为外部中断，可以直接唤醒充电*/
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

