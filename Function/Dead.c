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
	
}
/******************************************************************
功能：执行充电
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

		 //在调试时允许在停机状态下调试
//  #ifdef  DEBUG   
		 *((uint32 *)0xe0042004) |= 2;
//  #endif   
	   switch (mode.step)
	   	{
	   		case 0:
				//qz add 20181110
				if(Slam_Data.tick_flag)				//接收到SLAM心跳，标识SLAM已启动，可以发送关机命令
					{
						Slam_Data.dipan_req=DIPAN_REQ_TURNOFFSLAM;
						mode.time=giv_sys_time;
						mode.step++;
						break;
					}
				if(giv_sys_time-mode.time>1800000)	//超过3min，直接去低功耗
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
				//if(SLAM_Tick_Check())				//导航板停止发送心跳以后
					{
						//Slam_Data.no_msg=true;		//停止向导航板上传信息
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
				Enable_ExternInt_Weekup();	//可以使用KEY1,直充,座充唤醒				
#else
				Diable_AllExtInt_Weekup();	//屏蔽所有外部中断，无法唤醒
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
						 }
						 if(s == 1)
						 {
							 break;
						 }
						 //AccountCapabilityReal(); //计算耗电量
					  
				  }   
				RCC_Configuration();		///////////初始化系统的晶振，如有移植需要修改
				Battery_Data_Init();
				Init_PWM();				//qz add 20180703,不然风扇会转
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

	/*PP2暂时可能不会加入关机电路，所以进入DEAD低功耗以后，为了能够
	直接充电，加入了DC检测和SEAT检测为外部中断，可以直接唤醒充电*/
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

