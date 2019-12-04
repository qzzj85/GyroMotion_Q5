////////////////////////头文件////////////////////////////////////	
#include "AAA-include.h"
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
static u8 cliff_step=0;

///////////////////////全局函数////////////////////////////////////	  
void Init_Err(void); 
void Do_Err(void);
///////////////////////私有函数////////////////////////////////////	
///////////////////////函数实体////////////////////////////////////
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
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	stop_rap();						//停止轮子
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//关闭风机
	//Disable_earth();				//关闭地检
	Enable_earth();
	Disable_wall();					//关闭墙检
//	Enable_wall();
	disable_hwincept();				//禁止红外接收电源
	enable_hwincept();
	Reset_Speed_Send();  			//关闭速度检测
	disable_pwm(CHARGE_PWM);		//禁止充电
	mode.mode = CEASE;				//主模式为CEASE
	mode.sub_mode=ERR;				//子模式为ERR
	mode.Info_Abort =0;				//允许SLAM通信
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.status=0;					//非工作状态
	mode.time=giv_sys_time;
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
	Open_Led(0,error_code,3);
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

//qz add 20180913
  if(power.charge_seat)
  	Init_Chargeing(SEAT_CHARGING);
  if(power.charge_dc)
  	Init_Chargeing(DC_CHARGING);

  if(giv_sys_time-mode.time>6000000)
  	{
//	  	Slam_Data.dipan_req=DIPAN_REQ_TURNOFFSLAM;
	  	Init_ShutDown();
  	}
  //qz add end
}
