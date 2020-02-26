////////////////////////头文件////////////////////////////////////

#include "AAA-Include.h"
///////////////////////全局函数////////////////////////////////////	  
///////////////////////私有函数////////////////////////////////////	
///////////////////////函数实体////////////////////////////////////
////////////////////////私有定义//////////////////////////////////
bool wifi_ok=false;
  #ifdef  	DEBUG   
  #define   DBGMCU_CR ((uint32 *)0xe0042004)
  #endif   
	
////////////////////////全局变量//////////////////////////////////
bool ExitInput;     //有按键、外部电源外部输入
bool key_wakeup_flag=false;

////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
u16 error_code=0;					//底盘检测的异常代码，两字节，
bool gyrotick_ok_timeflag=false;

///////////////////////私有变量////////////////////////////////////
static u8 cliff_step=0;
///////////////////////函数实体////////////////////////////////////
/*****************************************************************
功能：在待机时的动作
*****************************************************************/
void Do_Cease(void)
{
    /**************如果有遥控器按键*/
#ifdef DC_NOBAT_RUN
  if(((power.charge_dc)|(power.charge_seat))&(!dc_nobat_run))
#else
  if(power.charge_dc|power.charge_seat)
#endif
		{
			 //mode.last_mode=mode.mode;			//qz add 20180201
			 //Init_Chargeing(CHARGEING);

			 //qz add 20180901
			 if(power.charge_seat)
			 	Init_Chargeing(SUBMODE_CHARGE_SEAT);
			 if(power.charge_dc)
			 	Init_Chargeing(SUBMODE_CHARGE_DC);
			 //qz add end
			 
			 return;
		}
	/////////机器的预约时间到，机器进入扫地模式/////////////////////

#ifdef SLEEP_SUBMODE
    /**********如果机器在3分钟内没有操作,机器进入睡眠模式**********/
	if(wifi_ok)
		{
			if((giv_sys_time-mode.time)>10000*60)			//如果WIFI连接上，则1min之内进入
			//if(((giv_sys_time-mode.sleep_time)>1800000)&(!mode.factory))		//qz add 20181110
				{
					//Init_Sleep();
					//Init_ShutDown();
					Init_VirtualSleep();
					return;
				}
		}
	else
		{
			if((giv_sys_time-mode.time)>10000*300)			//如果WIFI没有连接上，则5min之内进入
			//if(((giv_sys_time-mode.sleep_time)>1800000)&(!mode.factory))		//qz add 20181110
				{
					//Init_Sleep();
					//Init_ShutDown();
					Init_VirtualSleep();
					return;
				}
		}

#endif
	clr_all_hw_effect();
	ACC_DEC_Curve();
//qz mask 20181215
//	find_home=ReadHwSign_My();			//qz mask 20181215
#ifdef REPEAT_DOCK	//qz add 20180901
	if((giv_sys_time-mode.time>40000)&(mode.last_mode==MODE_CHARGE)&(mode.last_sub_mode==SUBMODE_CHARGE_SEAT)&(!dc_nobat_run)&(mode.step==0))
		{
			Enable_Speed();
			stop_rap();
			mode.step++;
			mode.Info_Abort=1;
		}

#if 0
	u8 abnormal=Read_Protect();
	Action_Protect_My(abnormal);
	if(mode.abnormity)
		return;

	Cease_Bump_Action();		
	if(mode.bump)
		return;
#endif

	switch(mode.step)
		{
			case 0:
				break;
			case 1:
				Speed=FAST_MOVE_SPEED;
				if(do_action(4,15*CM_PLUS))
					{
						stop_rap();
						mode.step++;
						mode.Info_Abort=0;
					}
				break;
			case 2:
				mode.step++;
				break;
			case 3:
				motion1.start_seat=true;
				Init_Docking();
				break;
		}
#endif
}

/******************************************************************
在待机状态的显示初始化
******************************************************************/
void Init_Cease(void)
{ 
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	
	/******初始化设置的值********************/

	/*******初始化输出的控制***************/
	stop_rap(); //关闭左右轮
	//Disable_earth();				//关闭地检
	Disable_wall();					//关闭墙检
	Enable_earth();
	Enable_wall();
	enable_hwincept();				//打开回充红外接收电源
	Enable_Speed();					//待机状态将速度检测打开，是为了防止进入CEASE时关闭速度检测会导致惯性脉冲无法计算。
		
	/****设置机器的工作模式******/   
	mode.mode = MODE_CEASE; 
	mode.Info_Abort=0;				//打开SLAM通信，qz add 20180418
	mode.sub_mode=SUBMODE_CEASE;			//QZ ADD
	mode.step=0x00;					//qz add
	mode.status=0;					//qz add 20180625
	mode.time=giv_sys_time;			//qz add 20180703
	mode.burning=false;
	mode.self_test=false;
	motion1.start_seat=false;
	motion1.force_dock=false;
	motion1.sweep_done=false;

	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);
		
	WriteWorkState();
	Disable_Free_Skid_Check();		//关闭万向轮检测
	//初始化检测的条件
	CHECK_STATUS_FLAG=true;			//使能异常检测
	Init_Check_Status();//qz add 20180425

#ifdef DEBUG_Enter_Mode
	TRACE("Init Cease Mode Complete!Prepare enter to Cease!\r\n");
#endif

#ifdef UV
	Reset_UV();						//关闭UV
#endif 
	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
	Open_Led(1,0,0);
	
}



void Init_VirtualSleep(void)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	
	/******初始化设置的值********************/

	/*******初始化输出的控制***************/
	stop_rap(); //关闭左右轮
	Disable_earth();				//关闭地检
	Disable_wall();					//关闭墙检
	//Enable_earth();
	//Enable_wall();
	enable_hwincept();				//打开回充红外接收电源
	Enable_Speed(); 				//待机状态将速度检测打开，是为了防止进入CEASE时关闭速度检测会导致惯性脉冲无法计算。
		
	/****设置机器的工作模式******/   
	mode.mode = MODE_CEASE; 
	mode.Info_Abort=0;				//打开SLAM通信，qz add 20180418
	mode.sub_mode=SUBMODE_VIRTUAL_SLEEP;			//QZ ADD
	mode.step=0x00; 				//qz add
	mode.status=0;					//qz add 20180625
	mode.time=giv_sys_time; 		//qz add 20180703

	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);
		
	WriteWorkState();
	Disable_Free_Skid_Check();		//关闭万向轮检测
	//初始化检测的条件
	CHECK_STATUS_FLAG=false; 		//使能异常检测
	Init_Check_Status();//qz add 20180425

#ifdef DEBUG_Enter_Mode
	TRACE("Init Cease Mode Complete!Prepare enter to Cease!\r\n");
#endif

#ifdef UV
	Reset_UV(); 					//关闭UV
#endif 
	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
	Close_Led();
}

void Do_VirtualSleep(void)
{
	/**************如果有遥控器按键*/
#ifdef DC_NOBAT_RUN
	if(((power.charge_dc)|(power.charge_seat))&(!dc_nobat_run))
#else
	if(power.charge_dc|power.charge_seat)
#endif
		{
			 //mode.last_mode=mode.mode;			//qz add 20180201
			 //Init_Chargeing(CHARGEING);

			 //qz add 20180901
			 if(power.charge_seat)
				Init_Chargeing(SUBMODE_CHARGE_SEAT);
			 if(power.charge_dc)
				Init_Chargeing(SUBMODE_CHARGE_DC);
			 //qz add end
			 
			 return;
		}
	/////////机器的预约时间到，机器进入扫地模式/////////////////////

#ifdef SLEEP_SUBMODE
	/**********如果机器在3分钟内没有操作,机器进入睡眠模式**********/
	if((giv_sys_time-mode.time)>10000*600)			//qz mask 20181110
	//if(((giv_sys_time-mode.sleep_time)>1800000)&(!mode.factory))		//qz add 20181110
		{
			Init_Sleep();
			//Init_ShutDown();
			return;
		}
#endif
	clr_all_hw_effect();				//qz mask 20181215
	//	find_home=ReadHwSign_My();			//qz mask 20181215
}

void Init_Quit_Charging(u8 sweep_method)
{
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	mode.mode=MODE_CEASE;
	mode.sub_mode=SUBMODE_QUITCHARGE;
	motion1.sweep_done=false;

	Enable_wall();
	Enable_earth();
	enable_hwincept();
	Init_Action();
	Enable_Speed();
//	mode.step=0x01;
	mode.step=0x00;				//初始化mode.step
	mode.time=giv_sys_time;
	mode.sweep_method=sweep_method;
	WriteWorkState();

	Sweep_Level_Set(SWEEP_LEVEL_STOP);
	//mode.Info_Abort=1;		//qz add 20180316:屏蔽导航板命令 //qz mask 20180522
	mode.Info_Abort=0;

	CHECK_STATUS_FLAG=true;
	Init_Check_Status();

#ifdef DEBUG_Enter_Mode
	TRACE("Init Quit_Charge Complete!\r\n");
#endif

#ifdef UV
	Reset_UV();
#endif
	Close_Led();
	if(mode.low_power)
		{
			Send_Voice(VOICE_POWER_LOW);
			mode.step=0xf0;
			mode.time=giv_sys_time;
		}


}

void Do_Quit_Chargeing(void)
{
	static short tgt_angle=0,anti_tgt_angle;
	#if 0
	//if((power.charge_seat==1)&&(mode.step==0x00))
	if(mode.step==0)
		{

			mode.step++;
//			mode.Info_Abort=1;
		}
	#endif


	ACC_DEC_Curve();

	switch(mode.step)
		{
			case 0:
				if(giv_sys_time-mode.time<2000)
					return;
				tgt_angle=Gyro_Data.yaw;
				anti_tgt_angle=Get_Reverse_Angle(tgt_angle);

#ifdef TUYA_WIFI
				Reset_Map();
#endif
				mode.step++;
			case 0x01:
				Speed=HIGH_MOVE_SPEED;
				if(do_action(4,20*CM_PLUS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 0x02:
				Speed=TURN_SPEED;
				do_action(2,360*Angle_1);
				if(Judge_Yaw_Reach(anti_tgt_angle,TURN_ANGLE_BIOS,1))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 3:
				Speed=HIGH_MOVE_SPEED;
				if(do_action_my(3,10*CM_PLUS,anti_tgt_angle))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 4:
				motion1.start_seat=true;
				switch(mode.sweep_method)
					{
						case SWEEP_METHOD_GUIHUA:									
							Init_First_Sweep(1);
							break;
						case SWEEP_METHOD_YBS:
							Reset_XY();
							//delay_ms(3000);
							Init_Right_YBS(1,1);
							break;
						case SWEEP_METHOD_SPOT:
							Reset_XY();
							//delay_ms(3000);
							Init_Sweep_Spot(SPOT_FROM_CHARGE);
							break;
					}
				break;
			case 0xf0:
				if(giv_sys_time-mode.time<30000)
					return;
				Init_Cease();
				break;
				
		}
}

/******************************************************************
功能：初始化充电程序
******************************************************************/
void Init_Err(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
    /******初始化显示***********/
	/******初始化设置的值********************/
	clr_ram();
	stop_rap();						//停止轮子
	//Disable_earth();				//关闭地检
	Enable_earth();
	Disable_wall();					//关闭墙检
//	Enable_wall();
	disable_hwincept();				//禁止红外接收电源
	enable_hwincept();
	Reset_Speed_Send();  			//关闭速度检测
	disable_pwm(CHARGE_PWM);		//禁止充电
	mode.mode = MODE_CEASE;				//主模式为CEASE
	mode.sub_mode=SUBMODE_ERR;				//子模式为ERR
	mode.Info_Abort =0;				//允许SLAM通信
	mode.status=0;					//非工作状态
	mode.time=giv_sys_time;
	
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//关闭风机
	
	WriteWorkState();
	

	CHECK_STATUS_FLAG=false;		//无需进行异常检测

#ifdef DEBUG_Enter_Mode
	TRACE("Init Err Mode Complete!\r\n");
#endif

#if 1
	dc_nobat_run=false;
#endif

#ifdef UV
	Reset_UV();
#endif
	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
	//Open_Led(0,30000,2);			//qz add 20181120,红灯快闪3s
	//Open_Led(0,0,0);
	Open_Led(4,error_code,0);
#ifdef NEW_VOICE_IC
	Send_Voice(VOICE_ERROR_WARNING);
#endif
	cliff_step=0;
}
/******************************************************************
功能：执行充电
******************************************************************/

u8 Check_All_Cliff_OK(void)
{
	static u32 check_time=0;
	switch(cliff_step)
		{
			case 0:
				if((e_r.sign!=FARN)&(e_m.sign!=FARN)&(e_r.sign!=FARN))
					{
						cliff_step++;
						check_time=giv_sys_time;
					}
				break;
			case 1:
				if((e_r.sign==FARN)|(e_m.sign==FARN)|(e_r.sign==FARN))
					{
						cliff_step=0;
						return 0;
					}
				if(giv_sys_time-check_time>10000)
					{
						cliff_step=0;
						return 1;
					}
				
		}
	return 0;
}

u8 Check_Gyro_Tick_OK(void)
{
	static uint8_t gyro_ok_cnt=0;
	if(!gyrotick_ok_timeflag)
		return 0;
	gyrotick_ok_timeflag=false;		//500ms enter
	switch(Gyro_Data.tick_check_step)
		{
			case 0:
				gyro_ok_cnt=0;
				Gyro_Data.tick_check_step++;
				break;
			case 1:
				if(giv_sys_time-Gyro_Data.tick_check_time<10000)
					gyro_ok_cnt++;
				if(gyro_ok_cnt>=10)
					return 1;
				break;		
		}
	return 0;
}


void Do_Err(void)
{
 ///遥控器按键管理		     
//  if(Sweep_YaoKong_Manage() == 1)
//		{
//				giv_sys_err = 0;
//				return;
//		}
  ///在面板上有三个按键中的任意一个按下了
  #if 0
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
		{
				key1.sign = 1;
				key2.sign = 1;
				key3.sign = 1;
				Init_Cease();	 
				giv_sys_err = 0;
				return;
		}
  if((giv_sys_time % 100000) < 1000)
		{
		}
  #endif
  if(error_code==ERROR_BOXNONE)
  	{
  		if(!Read_Dust_Box())
  			{
				Init_Cease();
  				Send_Voice(VOICE_BOX_EXIST);
				dust_box.fail=false;
  			}
  	}

  //qz mask 20180913
  //出现过如下情况：当机器左轮或者右轮单独悬空时，会做保护动作，保护动作做完后，还是悬空则报警。
  //但是报警进入异常后，一旦又着地，则马上进入待机状态，让人无法知道刚才发生了什么，机器就不动了
  //所以把单轮悬空的状态从这里屏蔽，不能自动回复
  //if((error_code==ERROR_MOTION_LIFT)|(error_code==ERROR_LEFT_LIFT)|(error_code==ERROR_RIGHT_LIFT))
  //if((error_code==ERROR_MOTION_LIFT))	//qz add 20180913
  if(error_code==ERROR_DANGER)
  	{
  		//if((l_lidi.key)&(r_lidi.key)&((e_r.sign!=FARN)&(e_m.sign!=FARN)&(e_r.sign!=FARN)))
  		if(Check_All_Cliff_OK())
  			{
  				Send_Voice(VOICE_VOLUME_2);
  				Init_Cease();				
  			}
  	}

  
  if(error_code==ERROR_GYRO)
  	{
  		if((mode.last_mode==MODE_CEASE)&(mode.last_sub_mode==SUBMODE_CEASE))
  			{
		  		if(Check_Gyro_Tick_OK())
		  			{
		  				Send_Voice(VOICE_VOLUME_2);
						Init_Cease();
		  			}
  			}
  	}

//qz add 20180913
  if(power.charge_seat)
  	Init_Chargeing(SUBMODE_CHARGE_SEAT);
  if(power.charge_dc)
  	Init_Chargeing(SUBMODE_CHARGE_DC);

  if(giv_sys_time-mode.time>6000000)
  	{
	  	//Init_ShutDown();
	  	Init_Sleep();
  	}
  //qz add end
}

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
	mode.sub_mode=SUBMODE_SLEEP;			//QZ ADD
	mode.step=0x00;					//qz add
	mode.status=0;					//非工作状态
	mode.time=giv_sys_time;			//qz add 20180703
	
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
				Init_Chargeing(SUBMODE_CHARGE_SEAT);
			if(power.charge_dc)
				Init_Chargeing(SUBMODE_CHARGE_DC);
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
				   Init_Chargeing(SUBMODE_CHARGE_SEAT);
				if(power.charge_dc)
				   Init_Chargeing(SUBMODE_CHARGE_DC);
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
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//关闭风机
	Disable_earth();
	Disable_wall();
	disable_hwincept();//禁止红外接收电源
	Disable_Speed();  //禁止红外灯光发送
//	mode.mode = ERR;
	mode.mode = MODE_CEASE;
	mode.Info_Abort =1;			//禁止SLAM通信
	mode.sub_mode=SUBMODE_DEAD;		//子模式为ERR
	mode.step=0;
	mode.status=0;			//qz add 20180422
	mode.time=giv_sys_time;
	WriteWorkState();

	CHECK_STATUS_FLAG=false;		//禁止异常检测
	Init_Check_Status();
	
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
		Sweep_Level_Set(SWEEP_LEVEL_STOP);		//关闭风机
		Disable_earth();
		Disable_wall();
		disable_hwincept();//禁止红外接收电源
		Disable_Speed();  //禁止红外灯光发送
	//	mode.mode = ERR;
		mode.mode = MODE_CEASE;
		mode.Info_Abort =1; 		//禁止SLAM通信
		mode.sub_mode=SUBMODE_SHUTDOWN; 	//子模式为ERR
		mode.step=0;
		mode.status=0;			//qz add 20180422
		mode.time=giv_sys_time;
		WriteWorkState();
	
		CHECK_STATUS_FLAG=false;		//禁止异常检测
		Init_Check_Status();

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
				Init_Chargeing(SUBMODE_CHARGE_SEAT);
			if(power.charge_dc)
				Init_Chargeing(SUBMODE_CHARGE_DC);
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
				Enable_ExternInt_Weekup(0); //可以使用KEY1,直充,座充唤醒				
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
				   Init_Chargeing(SUBMODE_CHARGE_SEAT);
				if(power.charge_dc)
				   Init_Chargeing(SUBMODE_CHARGE_DC);
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
			   #ifdef   NEW_Q55_BOARD_1113 
			GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);
			   #else
			GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource10);
			   #endif			
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

	    #ifdef   NEW_Q55_BOARD_1113 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource9);
	if(use_key)
		EXTI_InitStructure.EXTI_Line = EXTI_Line2|EXTI_Line_DC|EXTI_Line_SEAT;			
		#else	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);
	if(use_key)
		EXTI_InitStructure.EXTI_Line = EXTI_Line10|EXTI_Line_DC|EXTI_Line_SEAT;		
        #endif	
	else
		EXTI_InitStructure.EXTI_Line = EXTI_Line_DC|EXTI_Line_SEAT;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 		 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	if(use_key)
	#ifdef   NEW_Q55_BOARD_1113 
	    EXTI_ClearITPendingBit(EXTI_Line2);
	#else
		EXTI_ClearITPendingBit(EXTI_Line10);
    #endif	
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



