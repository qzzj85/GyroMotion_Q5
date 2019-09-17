#include "AAA-include.h"
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////	  
void Init_Switchoff(void); 
void Do_Switchoff(void);
///////////////////////私有函数////////////////////////////////////	
///////////////////////函数实体////////////////////////////////////
/******************************************************************
功能：初始化充电程序
******************************************************************/
void Init_Switchoff(void)
{

    /******初始化显示***********/
	/******初始化设置的值********************/
	clr_ram();

	stop_rap();						//停止轮子
	Sweep_Level_Set(STOP_ALL);		//关闭风机
	Disable_earth();
	Disable_wall();
	disable_hwincept();//禁止红外接收电源
	Disable_Speed();  //禁止红外灯光发送
	power.pwm=0;
	mode.mode = CHARGEING;
	mode.sub_mode=SWITCHOFF;
	mode.Info_Abort =0;			//禁止SLAM通信
	mode.step=0xff;			//防止从充电座后退
	mode.status=0;			//qz add 20180422
	mode.time=giv_sys_time;
	WriteWorkState();
	
	power.pwm = 0;
	power.step=0;
	disable_pwm(CHARGE_PWM);
	Sweep_Level_Set(STOP_ALL);
	Init_Charge_Data();					//qz add 20180522

	
	CHECK_STATUS_FLAG=false;		//禁止异常检测
}
/******************************************************************
功能：执行充电
******************************************************************/
void Do_Switchoff(void)
{
	ChargeControl_My();
}

