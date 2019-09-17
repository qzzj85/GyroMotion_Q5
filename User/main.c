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
	TRACE("b_v=%.2f\r\n",battery_voltage_1s*CHG_VOL_CNT);
//	TRACE("batcap=%d\r\n",Battery.BatteryCapability);
//	TRACE("batfd=%d\r\n",Battery.BatteryFDCap);
//	TRACE("batper=%d\r\n",Slam_Data.bat);
//	TRACE("w_ring.leth=%d\r\n",w_ring.length);
  	if(mode.mode==CEASE)
		{
			//TRACE("w_l.dis=%d w_lm.dis=%d  w_m.dis=%d  w_rm.dis=%d  w_r.dis=%d\r\n",w_l.dis,w_lm.dis,\
			w_m.dis,w_rm.dis,w_r.dis);
			TRACE("e_l.dif=%d e_m.dif=%d e_r.dif=%d\r\n",e_l.difference,e_m.difference,e_r.difference);
	//		TRACE("batcap=%d\r\n",Battery.BatteryCapability);
	//		TRACE("batfd=%d\r\n",Battery.BatteryFDCap);
			if(mode.sub_mode==ERR)
				TRACE("errcode=0x%x\r\n",error_code);
		}
	else if(mode.mode==DOCKING)
		{
			TRACE("m.s_mk=%d\r\n",mode.step_mk);
			if(power.charge_seat)
				TRACE("pow.seat=%d\r\n",power.charge_seat);
			if(power.charge_dc)
				TRACE("pow.dc=%d\r\n",power.charge_dc);
			TRACE("m.bump=%d\r\n",mode.bump);
			TRACE("m.step_bp=%d\r\n",mode.step_bp);
			TRACE("l_rap.rap=%d\r\n",l_rap.rap);
			TRACE("r_rap.rap=%d\r\n",r_rap.rap);
		}
	else if(mode.mode==CHARGEING)
		{	
			TRACE("p.step=%d\r\n",power.step);
			TRACE("p.pwm=%d\r\n",power.pwm);
			TRACE("c.cur=%d\r\n",(u32)(charge_data.real_current));
			TRACE("c.vol=%f\r\n",charge_data.real_voltage*CHG_VOL_CNT);
			TRACE("slam_tick=%d\r\n",Slam_Data.tick_flag);
		}
	else if(mode.mode==SWEEP)
		{
			//TRACE("m.bump=%d\r\n",mode.bump);
			//TRACE("m.st_bp=%d\r\n",mode.step_bp);
		}
	else if(mode.sub_mode==ERR)
		{	
			TRACE("error_code=%d\r\n",error_code);
		}
#if 1
	TRACE("Gyro.yaw=%d\r\n",Gyro_Data.yaw);
	TRACE("Gyro.x=%d\r\n",Gyro_Data.x_pos);
	TRACE("Gyro.y=%d\r\n",Gyro_Data.y_pos);
	TRACE("grid.x=%d y=%d\r\n",now_gridx,now_gridy);
	TRACE("m.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("c_p.next_action=%d\r\n",check_point.next_action);
	TRACE("c_p.new x1=%d y1=%d\r\n",check_point.new_x1,check_point.new_y1);
	TRACE("c_p.new x2=%d y2=%d\r\n",check_point.new_x2,check_point.new_y2);
	TRACE("m.area_num=%d\r\n",motion1.area_num);
	TRACE("m.ydir=%d\r\n",motion1.y_dir);
	TRACE("c_p.ybs_dir=%d\r\n",check_point.ybs_dir);
	TRACE("re_sweep=%d\r\n",motion1.repeat_sweep);
#endif
}

//===============================================================================================================
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
//	Set_FJ_Level(400);
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
//	TRACE("angle=%d\r\n",(int)(atan2(fabs(-31),fabs(-26))*57.295));
//	while(1);

//	Send_Voice(VOICE_DOCK_START);
#ifdef DEBUG_Enter_Mode
	TRACE("============welcome to sweeper!=============\r\n");
#endif
//	Speed=2000;	do_action(3,500*CM_PLUS);
	while(1)
	{	
						Uart3_Communication();
#ifdef TUYA_WIFI						  
						Uart1_Comunication();						//串口数据解析函数	 
                        mcu_wifi_proc_key();						
#endif						
//						Uart1_Comunication();						//串口数据解析函数	
//					  	Uart2_Comunication();
//						Sensor_status_report();
//						Mile_Gyro_Report();
						Action_Mode();								//	工作的模式
#ifdef PWRCAP_CAL
						AccountCapability();						//	每相隔1秒计算电池剩余容量
#endif							
						ReadRealTime();
						AutoReadKey(); 								//	自动读取按键	
//						AutoDisplay(); 								//	自动显示				
						judge_charge(); 							//	自动判断是否有充电信号
						sampling_temp_voltage();					//	AD采样数据处理  
						//read_wallearth();		 					//	自动采样墙检和地检数据
						//read_wallearth_my();						//	读取墙检地检
						Read_Earth_My();							//	读取地检
						Read_Wall_My();								//	读取墙检
						APP_BAT_Handle();
						Check_Status();
						//Get_Dispower(); 
						Voice_Handle();
						YBS_Find_Seat();
						if(Check_PreengageTime())
							{
								Slam_Data.dipan_req=DIPAN_REQ_PREEN;
								//if(!mode.status)
									//Slam_Data.dipan_req_pre=DIPAN_REQ_PREEN;
							}
#ifdef DEBUG
						if(log_show)
							{
								log_show=false;
								log_out();
							}
#endif
#ifdef PWRCAP_CAL
						Battery_Reinit();
#endif

#ifdef REMOTE
						Remote_Handle();
#endif

						LED_Handle();
						WifiData_Handle();

//						Speed=500;
//						if(do_action(3,200*CM_PLUS));
//						ACC_DEC_Curve();
//						LED_Handle();
//						Speed=1000;
//						if(do_action(3,1000*CM_PLUS));
//						ACC_DEC_Curve();
	}	

}



