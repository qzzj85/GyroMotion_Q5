////////////////////////头文件////////////////////////////////////	
#include "AAA-include.h"
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////	  

///////////////////////私有函数////////////////////////////////////	
///////////////////////函数实体////////////////////////////////////
/******************************************************************
功能：初始化充电程序
******************************************************************/
void Init_Switchoff(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	Init_Ring();					//qz add 20180803重新初始化轮子有关信息
	/******初始化显示***********/
	/******初始化设置的值********************/
	clr_ram();
	Init_Sweep_Pwm(PWM_CHARGE_MAX,PWM_CHARGE_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//关闭风机
	stop_rap(); 					//停止轮子
	Disable_earth();
	Disable_wall();
	disable_hwincept();				//禁止红外接收电源
	Disable_Speed();  				//禁止红外灯光发送

	mode.mode = MODE_CHARGE;
	mode.sub_mode=SUBMODE_CHARGE_SWITCHOFF;
	mode.Info_Abort =1; 			//禁止SLAM速度通信
	mode.All_Info_Abort=1;			//禁止SLAM通信,qz add 20180919
	mode.step=0; 					//防止从充电座后退
	mode.status=0;					//非工作状态
	mode.time=giv_sys_time;
	WriteWorkState();

	power.pwm = 0;
	power.step=0;
	disable_pwm(CHARGE_PWM);
	Init_Charge_Data(); 				//qz add 20180522
	CHECK_STATUS_FLAG=false;			//禁止异常检测
	Init_Check_Status();
#ifdef DEBUG_Enter_Mode
	TRACE("Init switchoff submode Complete!\r\n");
#endif

#ifdef UV
	Reset_UV(); 					//关闭UV
#endif
	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
	Open_Led(0,0,1);				//qz add 20181120,红灯慢闪
}

void Init_Chargeing(u8 temp_sub_mode)
{
	mode.last_mode=mode.mode;		//qz add 20180814
	mode.last_sub_mode=mode.sub_mode;
    /******初始化显示***********/
	/******初始化设置的值********************/
	mode.mode = MODE_CHARGE;
	mode.sub_mode=temp_sub_mode;
	mode.step=0;		//qz add 20180522
	mode.Info_Abort=0;	//qz add 20180615
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.status=0;						//qz add 20180522
	WriteWorkState();
	clr_ram();
	Init_Sweep_Pwm(PWM_CHARGE_MAX,PWM_CHARGE_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//关闭风机
	//qz add 20180615:充电期间关闭墙检\地检\回充红外\速度检测
	Disable_earth();
	Disable_wall();
	enable_hwincept();
	Disable_Speed();	//qz add 20180615
	Disable_Free_Skid_Check();			//qz add 20180625 
	
	Init_Charge_Data();					//qz add 20180522
	power.pwm = 0;
	disable_pwm(CHARGE_PWM);
	power.step = 0;
	Sweep_Level_Set(SWEEP_LEVEL_STOP);
	
	
	CHECK_STATUS_FLAG=false;			//禁止异常检测		//qz add 20180913
	Init_Check_Status();				//qz add 20180522
	USART_Cmd(USART1, ENABLE);			//qz add 20180625,因为如果之前给导航板发送了关机,会关掉串口,现在充电后重新打开
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 	  //qz add 20180625 

	Send_Voice(VOICE_CHARGING_START);	//发送语音，开始充电
#ifdef DEBUG_Enter_Mode
	TRACE("Init Charge Mode Complete!\r\n");
#endif

#ifdef UV
	Reset_UV(); 					//关闭UV
#endif
	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
	Open_Led(0,0,1);				//qz add 20181120,红灯慢闪
}
/******************************************************************
功能：执行充电
******************************************************************/
void Do_Chargeing(void)
{
	switch (mode.step)
		{
			case 0:
				ChargeControl_Volt_My();
				if((mode.sub_mode==SUBMODE_CHARGE_SWITCHOFF)&(power.switch_flag))
					{
						if(power.charge_dc)
							{
								Init_Chargeing(SUBMODE_CHARGE_DC);
								return;
							}
						else if(power.charge_seat)
							{
								Init_Chargeing(SUBMODE_CHARGE_SEAT);
								return;
							}
					}

				if(((mode.sub_mode==SUBMODE_CHARGE_DC)|(mode.sub_mode==SUBMODE_CHARGE_SEAT))&(!power.switch_flag))
					{
						Init_Switchoff();
						return;
					}
				
				if((!power.charge_dc)&&(!power.charge_seat))
					{
#ifdef NEW_VOICE
						if(!flag_full)
							Send_Voice(VOICE_STOP_CHARGE);
#endif
						mode.time=giv_sys_time;
						mode.step++;
						disable_pwm(CHARGE_PWM);
						Close_Led();
					}
				break;
			case 1:
				if(giv_sys_time-mode.time>5000)
					mode.step++;
				if(power.charge_seat)
					Init_Chargeing(SUBMODE_CHARGE_SEAT);
				if(power.charge_dc)
					Init_Chargeing(SUBMODE_CHARGE_DC);
				break;
			case 2:
#if 0
				if((!l_lidi.key)|(!r_lidi.key))
					{
						Init_Cease();
						Init_Cease();
						return;
					}
#endif
				if(error_code)
					{
						Init_Err();
						return;
					}
				disable_pwm(CHARGE_PWM);
				Init_Cease();
				break;
		}
	//qz add end

#ifdef DC_NOBAT_RUN
	if(dc_nobat_run)
		Init_Cease();

#endif
}
//==========================================================================================================





























