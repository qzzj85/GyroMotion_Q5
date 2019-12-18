//=====================头文件====================================
#include "AAA-Include.h"
#include "soft.h"
//#include "stm32f10x_degmcu.h"
//=====================私有定义====================================

//******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义************
//=====================全局变量====================================

//=====================全局函数====================================


//******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义************
//=====================私有变量====================================

//=====================私有函数====================================
void log_out(void)
{
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	TRACE("===========\r\n");
	TRACE("m.mode=0x%x\r\n",mode.mode);
	TRACE("m.submode=0x%x\r\n",mode.sub_mode);
	TRACE("m.step=0x%x\r\n",mode.step);
	if(mode.bump)
		{
			TRACE("m.bump=%d\r\n",mode.bump);
			TRACE("m.st_bp=%d\r\n",mode.step_bp);
		}
	if(mode.abnormity)
		{
			TRACE("m.abn=%d\r\n",mode.abnormity);
			TRACE("m.abn_stp=%d\r\n",mode.step_abn);
		}
	TRACE("b_v=%.2f\r\n",battery_voltage_1s*VOLT_CHG_CNT);
	TRACE("batcap=%d\r\n",Battery.BatteryCapability);
	TRACE("batfd=%d\r\n",Battery.BatteryFDCap);
	TRACE("batper=%d\r\n",Battery.bat_per);
//	TRACE("w_ring.leth=%d\r\n",w_ring.length);
  	if(mode.mode==MODE_CEASE)
		{
			TRACE("w_l.dis=%d w_lm.dis=%d  w_m.dis=%d  w_rm.dis=%d  w_r.dis=%d\r\n",w_l.dis,w_lm.dis,\
			w_m.dis,w_rm.dis,w_r.dis);
			TRACE("w_l.dif=%d w_lm.dif=%d w_m.dif=%d w_rm.dif=%d w_r.dif=%d\r\n",w_l.difference,w_lm.difference,\
				w_m.difference,w_rm.difference,w_r.difference);
			TRACE("e_l.dif=%d e_m.dif=%d e_r.dif=%d\r\n",e_l.difference,e_m.difference,e_r.difference);
	//		TRACE("batcap=%d\r\n",Battery.BatteryCapability);
	//		TRACE("batfd=%d\r\n",Battery.BatteryFDCap);
			if(mode.sub_mode==SUBMODE_ERR)
				{
					TRACE("errcode=0x%x\r\n",error_code);
					TRACE("m.wifi_err_code=%d\r\n",mode.wifi_err_code);
				}
		}
	else if(mode.mode==MODE_DOCK)
		{
			TRACE("m.s_mk=%d\r\n",mode.step_mk);
			if(power.charge_seat)
				TRACE("pow.seat=%d\r\n",power.charge_seat);
			if(power.charge_dc)
				TRACE("pow.dc=%d\r\n",power.charge_dc);
			TRACE("m.bump=%d\r\n",mode.bump);
			TRACE("m.step_bp=%d\r\n",mode.step_bp);
		}
	else if(mode.mode==MODE_CHARGE)
		{	
			TRACE("p.step=%d\r\n",power.step);
			TRACE("p.pwm=%d\r\n",power.pwm);
			TRACE("c.cur=%d\r\n",(u32)(charge_data.real_current));
			TRACE("c.vol=%f\r\n",charge_data.real_voltage*VOLT_CHG_CNT);
		}
	else if(mode.mode==MODE_SWEEP)
		{
			//TRACE("m.bump=%d\r\n",mode.bump);
			//TRACE("m.st_bp=%d\r\n",mode.step_bp);
		}
#if 1
	TRACE("Gyro.yaw=%d\r\n",Gyro_Data.yaw);
	TRACE("Gyro.x=%f y=%f\r\n",X_pos,Y_pos);
	TRACE("grid.x=%d y=%d\r\n",now_gridx,now_gridy);
//	TRACE("grid.y_stari=%d\r\n",grid.y_straight_start);
	TRACE("m.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("c_p.next_action=%d\r\n",check_point.next_action);
	TRACE("c_p.new x1=%d y1=%d\r\n",check_point.new_x1,check_point.new_y1);
	TRACE("c_p.new x2=%d y2=%d\r\n",check_point.new_x2,check_point.new_y2);
	TRACE("c_p.ybs_dir=%d\r\n",check_point.ybs_dir);
	TRACE("m.area_num=%d\r\n",motion1.area_num);
	TRACE("m.ydir=%d\r\n",motion1.y_dir);
//	TRACE("re_sweep=%d\r\n",motion1.repeat_sweep);
//	TRACE("m.f_l_y=%d\r\n",motion1.first_leak_y);
	u32 min=(giv_sys_time-motion1.worktime_area)/10000/60;
	u32 sec=(giv_sys_time-motion1.worktime_area)/10000-min*60;
	TRACE("work time=%d min %d sec\r\n",min,sec);
	TRACE("max_time=%d\r\n",motion1.worktime_area_max);
	TRACE("version is V%d.%d.%d\r\n",MAIN_VERISON,SUB_VERSION,CORRECT_VERSION);
	TRACE("fanspd_tgt=%d\r\n",fanspd_tgt);
	TRACE("fanspd=%d r/s\r\n",fanspd);
	TRACE("fanpwm=%d\r\n",fan_pwm);
//	TRACE("top=%d\r\n",top_time_sec);
//	TRACE("start_seat=%d\r\n",motion1.start_seat);
#else
	//TRACE("gyro.first_pitch=%d\r\n",Gyro_Data.first_pitch);
	//TRACE("gyro.pitch=%d\r\n",Gyro_Data.pitch);
	//TRACE("gyro.first_roll=%d\r\n",Gyro_Data.first_roll);
	//TRACE("gyro.roll=%d\r\n",Gyro_Data.roll);
	//TRACE("mode.speed_up=%d\r\n",mode.speed_up);
	TRACE("l_hw.t=%d l=%d r=%d m=%d\r\n",l_hw.effectTop,l_hw.effectLeft,l_hw.effectRight,l_hw.effectMid);
	TRACE("lm_hw.t=%d l=%d r=%d m=%d\r\n",lm_hw.effectTop,lm_hw.effectLeft,lm_hw.effectRight,lm_hw.effectMid);
	TRACE("rm_hw.t=%d l=%d r=%d m=%d\r\n",rm_hw.effectTop,rm_hw.effectLeft,rm_hw.effectRight,rm_hw.effectMid);
	TRACE("r_hw.t=%d l=%d r=%d m=%d\r\n",r_hw.effectTop,r_hw.effectLeft,r_hw.effectRight,r_hw.effectMid);
#endif
}

//====================================	===========================================================================
//===============================================================================================================

//===============================================================================================================
//===============================================================================================================

int main(void)
{	   
	Init_System();
//	Init_Sweep_Bump_Node();
	//RST_S2D_0;			//qz add 20181110:允许导航板复位底盘
//  	DBGMCU_Config(DBGMCU_TIM4_STOP,ENABLE);
  
//	Init_Cease();			//qz add 20180201
//	Init_Dead();
//	Init_Right_YBS(1);	 		
//	Init_Commander(); 
//	Init_Spot(); 
//	Init_Docking();
//	Init_Chargeing();
//	Init_Sleep();
//	Init_Init_Sweep();
//	Init_Left_YBS(0);
//	Init_RunTest();
//	while(1)
//	{
////		Set_Charge_Pwm(0);
//		Set_Charge_Pwm(1300);
//	}
//	Init_Left_YBS(0);
//	Set_ZS_Level(STANDARD_PWM);
//	enable_pwm(L_FRONT,1000);
//	Set_BS_Level(500);
//	delay_ms(5000);
//	while(1);

#ifdef DEBUG_Enter_Mode
	TRACE("============welcome to sweeper!=============\r\n");
#endif
	while(1)
	{	
		//Uart3_Communication();
#ifdef TUYA_WIFI						  
		Uart1_Comunication();						//串口数据解析函数	 
        mcu_wifi_proc_key();						
#endif						
//						Uart1_Comunication();						//串口数据解析函数	
//					  	Uart2_Comunication();
//						Sensor_status_report();
//						Mile_Gyro_Report();
		Action_Mode();								//	工作的模式
		AccountCapability();						//	每相隔1秒计算电池剩余容量
		ReadRealTime();
		AutoReadKey(); 								//	自动读取按键	
		judge_charge(); 							//	自动判断是否有充电信号
		sampling_temp_voltage();					//	AD采样数据处理  
		//read_wallearth();		 					//	自动采样墙检和地检数据
		//read_wallearth_my();						//	读取墙检地检
#ifndef EARTH_IN_TIM2
		Read_Earth_My();							//	读取地检
#endif
		Read_Wall_My();								//	读取墙检
		APP_BAT_Handle();
		Check_Status();

#ifdef VOICE_LIST		//使用语音LIST发出语音
		Voice_Handle();
#endif

#ifdef PWRCAP_CAL
		//Battery_Reinit();
#endif

#ifdef REMOTE
		Remote_Handle();
#endif

		LED_Handle();
		MapData_Handle();
		Work_TimeOut_Handle();						
		Parse_LowPower2Dock();
		Parse_SelfTest_Stop();
#ifdef FAN_SPD_CTL
		FanSpd_PID();
#endif
#ifdef DEBUG
		if((log_show)&(mode.sub_mode!=SUBMODE_FACT_TEST))
			{
				log_show=false;
				log_out();
			}
#endif
	}	

}



