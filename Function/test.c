#include "AAA-include.h"
bool test_100ms_flag,test_500ms_flag,test_1s_flag;
u32 test_result=0; 
void Init_Test(u8 test_mode)
{
	stop_rap();
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
		
	/******初始化设置的值********************/
	clr_ram();
//	ReInitAd();
	Enable_earth();
	Enable_wall();
	enable_hwincept();				//允许红外接收电源
	Enable_Speed(); 				//允许速度发送
	Init_Action();
	
	mode.mode = MODE_CEASE;			
	mode.sub_mode=test_mode;
	mode.step=0;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.status=1;
//		WriteWorkState();
	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	if(mode.sub_mode==SUBMODE_SELF_TEST)
		TRACE("Init SELF TEST Mode Complete!\r\n");
	else if(mode.sub_mode==SUBMODE_BURN_TEST)
		TRACE("Init Burn Test Mode Complete!\r\n");
	else
		TRACE("Init Factory Test Mode Complete!\r\n");
#endif
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
}

void Do_BurningTest(void)
{
	switch(mode.step)
		{
			case 0:
				mode.burning=true;
				Open_Led(2,0,2);
				mode.step++;
				break;
			case 1:
				if((giv_sys_time-mode.time>30000)&(Read_Key2()))
					mode.step++;
				break;
			case 2:
#ifdef TUYA_WIFI
				Reset_Map();
#endif
				if(mode.last_sub_mode==SUBMODE_CHARGE_SEAT)
					Init_Quit_Charging(SWEEP_METHOD_GUIHUA);
				else
					Init_First_Sweep(0);
				break;
		}
}

void Do_SelfTest(void)
{
	static u8 bump_test=0;
	u8 bump=0;
	bump=Parse_BumpValue();
	switch(mode.step)
		{
			case 0:
				Open_Led(3,0,2);
				bump_test=0;
				mode.step++;
				break;
			case 1:
				if(bump==BUMP_ONLY_LEFT)
					bump_test|=0x01;
				if(bump==BUMP_ONLY_RIGHT)
					bump_test|=0x02;
				if(bump_test==0x03)
					{
						mode.step++;
						return;
					}
				if(giv_sys_time-mode.time>100000)
					{
						Init_Cease();
					}
				break;
			case 2:
				mode.self_test=true;
				mode.self_test_time=giv_sys_time;
				Send_Voice(VOICE_SWEEP_START);
				Init_First_Sweep(0);
				break;
			
		}
}

u8 Parse_SelfTest_Stop(void)
{
	if(mode.self_test)
		{
			if(giv_sys_time-mode.self_test_time>60*10000)
				{
					stop_rap();
					Send_Voice(VOICE_VOLUME_2);
					Init_Cease();
				}
		}
	return 0;
}

void Do_FactoryTest(void)
{
	static u32 l_bump_cnt,r_bump_cnt,test_data1,test_cnt;
	static u32 dc_result,seat_result,vol_result,el_result,em_result,er_result,wl_result,wlm_result,wm_result,wrm_result,wr_result;
	static u32 sbcurr_result,mbcurr_result,fancurr_result,lring_result,rring_result;
	u8 bump;
	ACC_DEC_Curve();
	clr_all_hw_effect();

	switch(mode.step)
		{
			case 0:
				Open_Led(2,0,0);
				CHECK_STATUS_FLAG=false;
				test_result=0;
				TRACE("============================================================\r\n");
				TRACE("======================欢迎进入厂测模式======================\r\n");
				TRACE("========================准备测试============================\r\n");
				mode.step=100;
				break;
			case 100:
				if((giv_sys_time-mode.time>30000)&(Read_Key2()))
					mode.step=100;
				mode.step=1;
				mode.time=giv_sys_time;
				TRACE("======================1，电压校准测试========================\r\n");
				break;
				
			//////////////////电压校准测试////////////////////////////
			case 1:
				if(giv_sys_time-mode.time<12*10000)
					return;
				full_power=battery_voltage_10s;
				TRACE("校准值:%d\r\n",full_power);
				if(full_power!=0)
					{
						TRACE("电压校准成功!!!\r\n");
						test_result|=TEST_VOLCAL_OK;
					}
				else
					{
						TRACE("电压校准失败!!!\r\n");
					}
				TRACE("======================2，碰撞测试====================\r\n");
				TRACE("请在10秒内，按左、右碰撞各至少3次!!\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				l_bump_cnt=0;r_bump_cnt=0;
				break;
			//////////////////碰撞测试////////////////////////////
			case 2:
				if(giv_sys_time-mode.time<10*10000)
					{
						bump=Parse_BumpValue();
						if((bump==BUMP_ONLY_LEFT)&(test_500ms_flag))
							{
								test_500ms_flag=false;
								TRACE("左碰撞!\r\n");
								l_bump_cnt++;
							}
						else if((bump==BUMP_ONLY_RIGHT)&(test_500ms_flag))
							{	
								test_500ms_flag=false;
								TRACE("右碰撞!\r\n");
								r_bump_cnt++;
							}
						return;
					}
				TRACE("左碰撞%d次!右碰撞%d次\r\n",l_bump_cnt,r_bump_cnt);
				if((l_bump_cnt>=3)&(r_bump_cnt>=3))
					{
						TRACE("碰撞测试通过!!!\r\n");
						test_result|=TEST_BUMP_OK;
					}
				else
					{
						TRACE("碰撞测试失败!!!\r\n");
					}
				TRACE("======================3，电池温度测试====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////电池温度测试////////////////////////////
			case 3:
				TRACE("电池温度测试通过!!!\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				TRACE("======================4，陀螺仪测试====================\r\n");
				break;
			//////////////////陀螺仪测试////////////////////////////
			case 4:
				if(giv_sys_time-mode.time<50000)
					return;
				if(Gyro_Data.tick_flag)
					{
						test_result|=TEST_GRYO_OK;
						TRACE("陀螺仪测试通过!!!\r\n");
					}
				else
					TRACE("陀螺仪测试失败!!!\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;
				TRACE("======================5，直充充电测试====================\r\n");
				TRACE("请在10秒内插入直充充电线\r\n");
				break;
			//////////////////DC测试////////////////////////////
			case 5:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(power.charge_dc)
							{
								TRACE("已检测到直充充电线!!!\r\n");
								mode.step++;
								mode.time=giv_sys_time;
								test_data1=0;test_cnt=0;
								Init_Charge_Data();
								disable_pwm(CHARGE_PWM);
								power.pwm=0;
								power.step=0;
							}
						else
							{
								if(test_1s_flag)
									{
										test_1s_flag=false;
										TRACE("请插入直充充电线!!!\r\n");
									}
							}
						return;
					}
				TRACE("未检测到直充充电线!!!\r\n");
				TRACE("直充充电测试失败!!!\r\n");
				TRACE("======================6，座充充电测试====================\r\n");
				TRACE("请在10秒内接入充电座\r\n");
				dc_result=0;
				mode.step=7;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;
				break;
			case 6:
				ChargeControl_Test();
				if(!power.charge_dc)
					{
						TRACE("未检测到直充充电线!!\r\n");
						mode.step=5;
						return;
					}
				if(giv_sys_time-mode.time<15*10000)
					{
						if(test_1s_flag)
							{
								test_1s_flag=false;
								TRACE("充电电流:%.2fmA\r\n",battery_chargecurrent_1s*CURR_CHG_CNT);
								//test_data1+=battery_chargecurrent_1s;
								//test_cnt++;
							}
						return;
					}
				//test_data1=test_data1/test_cnt;
				TRACE("最终充电电流:%.2fmA\r\n",battery_chargecurrent_1s*CURR_CHG_CNT);
				dc_result=battery_chargecurrent_1s;
				switch(power.step)
					{
						case 2:
						case 5:
							if(abs((u32)(battery_chargecurrent_1s*CURR_CHG_CNT)-360)<50)
								{
									test_result|=TEST_DC_OK;
									TRACE("直充充电测试通过!!!\r\n");
								}
							else
								TRACE("直充充电测试失败!!!\r\n");
							break;
						case 3:
							if(abs((u32)(battery_chargecurrent_1s*CURR_CHG_CNT)-800)<150)
								{
									test_result|=TEST_DC_OK;
									TRACE("直充充电测试通过!!!\r\n");
								}
							else
								TRACE("直充充电测试失败!!!\r\n");
							break;
						default:
							if(battery_chargecurrent_1s>0)
								{
									test_result|=TEST_DC_OK;
									TRACE("直充充电测试通过!!!\r\n");
								}
							else
								TRACE("直充充电测试失败!!!\r\n");
							break;
								
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;
				TRACE("======================6，座充充电测试====================\r\n");
				TRACE("请在10秒内接入充电座\r\n");
				break;
			//////////////////SEAT测试////////////////////////////
			case 7:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(power.charge_seat)
							{
								TRACE("已检测到充电座!!!\r\n");
								mode.step++;
								test_data1=0;test_cnt=0;
								Init_Charge_Data();
								disable_pwm(CHARGE_PWM);
								power.pwm=0;
								power.step=0;
							}
						else
							{
								if(test_1s_flag)
									{
										test_1s_flag=false;
										TRACE("请接入充电座!!!\r\n");
									}
							}
						return;
					}
				TRACE("未检测到充电座!!!\r\n");
				TRACE("座充充电测试失败!!!\r\n");
				mode.step=9;
				mode.time=giv_sys_time;
				seat_result=0;
				TRACE("====================7，左前红外接收测试====================\r\n");
				break;
			case 8:
				ChargeControl_Test();
				if(!power.charge_seat)
					{
						TRACE("未检测到充电座!!\r\n");
						mode.step=7;
						return;
					}
				if(giv_sys_time-mode.time<15*10000)
					{
						if(test_1s_flag)
							{
								test_1s_flag=false;
								TRACE("充电电流:%.2fmA\r\n",battery_chargecurrent_1s*CURR_CHG_CNT);
								//test_data1+=battery_chargecurrent_1s;
								//test_cnt++;
							}
						return;
					}
				//test_data1=test_data1/test_cnt;
				TRACE("最终充电电流:%.2fmA\r\n",battery_chargecurrent_1s*CURR_CHG_CNT);
				seat_result=battery_chargecurrent_1s;
				switch(power.step)
					{
						case 2:
						case 5:
							if(abs((u32)(battery_chargecurrent_1s*CURR_CHG_CNT)-360)<50)
								{
									test_result|=TEST_SETA_OK;
									TRACE("座充充电测试通过!!!\r\n");
								}
							else
								TRACE("座充充电测试失败!!!\r\n");
							break;
						case 3:
							if(abs((u32)(battery_chargecurrent_1s*CURR_CHG_CNT)-800)<150)
								{
									test_result|=TEST_SETA_OK;
									TRACE("座充充电测试通过!!!\r\n");
								}
							else
								TRACE("座充充电测试失败!!!\r\n");
							break;
						default:
							if(battery_chargecurrent_1s>0)
								{
									test_result|=TEST_SETA_OK;
									TRACE("座充充电测试通过!!!\r\n");
								}
							else
								TRACE("座充充电测试失败!!!\r\n");
							break;
								
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;
				TRACE("====================7，左前红外接收测试====================\r\n");
				break;
			//////////////////左前红外接收测试/////////////////
			case 9:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(l_hw.effect)
							{
								test_result|=TEST_LHW_OK;
								TRACE("左前红外测试通过!!!\r\n");
								TRACE("====================8，右前红外接收测试====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("左前红外测试失败!!!\r\n");
				TRACE("====================8，右前红外接收测试====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////右前红外接收测试/////////////////
			case 10:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(r_hw.effect)
							{
								test_result|=TEST_RHW_OK;
								TRACE("右前红外测试通过!!!\r\n");
								TRACE("====================9，中左红外接收测试====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("右前红外测试失败!!!\r\n");
				TRACE("====================9，中左红外接收测试====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////中左红外接收测试/////////////////
			case 11:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(lm_hw.effect)
							{
								test_result|=TEST_LMHW_OK;
								TRACE("中左红外测试通过!!!\r\n");
								TRACE("====================10，中右红外接收测试====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("中左红外测试失败!!!\r\n");
				TRACE("====================10，中右红外接收测试====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////中右红外接收测试/////////////////
			case 12:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(rm_hw.effect)
							{
								test_result|=TEST_RMHW_OK;
								TRACE("中右红外测试通过!!!\r\n");
								TRACE("====================11，后左红外接收测试====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("中右红外测试失败!!!\r\n");
				TRACE("====================11，后左红外接收测试====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////后左红外接收测试/////////////////
			case 13:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(lb_hw.effect)
							{
								test_result|=TEST_LBHW_OK;
								TRACE("后左红外测试通过!!!\r\n");
								TRACE("====================12，后右红外接收测试====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("后左红外测试失败!!!\r\n");
				TRACE("====================12，后右红外接收测试====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////后右红外接收测试/////////////////
			case 14:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(rb_hw.effect)
							{
								test_result|=TEST_RBHW_OK;
								TRACE("后右红外测试通过!!!\r\n");
								TRACE("====================13，电池电压测试====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("后右红外测试失败!!!\r\n");
				TRACE("====================13，电池电压测试====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			/////////////////电池电压测试///////////////////////
			case 15:
				vol_result=battery_voltage_1s;
				TRACE("电池电压:%.2fV\r\n",vol_result*VOLT_CHG_CNT);
				if((vol_result>=VOL_12_8V)&(vol_result<=VOL_17_1V))
					{						
						test_result|=TEST_VOL_OK;
						TRACE("电池电压测试通过!!!\r\n");
					}
				else
					{
						TRACE("电池电压测试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================14，左侧地检测试====================\r\n");
				break;
			/////////////////左侧地检测试/////////////////////
			case 16:
				if(giv_sys_time-mode.time<TEST_ERATH_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=e_l.difference;
								test_cnt++;
							}
						return;
					}
				el_result=test_data1/test_cnt;
				TRACE("左侧地检值:%d\r\n",el_result);
				if((el_result>0)&(el_result<4000))
					{
						test_result|=TEST_EL_OK;
						TRACE("左侧地检测试通过!!!\r\n");
					}
				else
					{
						TRACE("左侧地检测试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================15，中地检测试====================\r\n");
				break;
			/////////////////左侧地检测试/////////////////////
			case 17:
				if(giv_sys_time-mode.time<TEST_ERATH_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=e_m.difference;
								test_cnt++;
							}
						return;
					}
				em_result=test_data1/test_cnt;
				TRACE("中地检值:%d\r\n",em_result);
				if((em_result>0)&(em_result<4000))
					{
						test_result|=TEST_EM_OK;
						TRACE("中地检测试通过!!!\r\n");
					}
				else
					{
						TRACE("中地检测试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================16，右侧地检测试====================\r\n");
				break;
			/////////////////右侧地检测试/////////////////////
			case 18:
				if(giv_sys_time-mode.time<TEST_ERATH_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=e_r.difference;
								test_cnt++;
							}
						return;
					}
				er_result=test_data1/test_cnt;
				TRACE("右侧地检值:%d\r\n",er_result);
				if((er_result>0)&(er_result<4000))
					{
						test_result|=TEST_ER_OK;
						TRACE("右侧检测试通过!!!\r\n");
					}
				else
					{
						TRACE("右侧检测试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================17，左侧墙检测试====================\r\n");
				break;
			/////////////////左侧墙检测试/////////////////////
			case 19:
				if(giv_sys_time-mode.time<TEST_WALL_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=w_l.difference;
								test_cnt++;
							}
						return;
					}
				wl_result=test_data1/test_cnt;
				TRACE("左侧墙检值:%d\r\n",wl_result);
				if((wl_result>0)&(wl_result<4000))
					{
						test_result|=TEST_WL_OK;
						TRACE("左侧墙检测试通过!!!\r\n");
					}
				else
					{
						TRACE("左侧墙检试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================18，中左墙检测试====================\r\n");
				break;
			/////////////////中左墙检测试/////////////////////
			case 20:
				if(giv_sys_time-mode.time<TEST_WALL_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=w_lm.difference;
								test_cnt++;
							}
						return;
					}
				wlm_result=test_data1/test_cnt;
				TRACE("中左墙检值:%d\r\n",wlm_result);
				if((wlm_result>0)&(wlm_result<4000))
					{
						test_result|=TEST_WLM_OK;
						TRACE("中左墙检测试通过!!!\r\n");
					}
				else
					{
						TRACE("中左墙检试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================19，中墙检测试====================\r\n");
				break;
			/////////////////中墙检测试/////////////////////
			case 21:
				if(giv_sys_time-mode.time<TEST_WALL_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=w_m.difference;
								test_cnt++;
							}
						return;
					}
				wm_result=test_data1/test_cnt;
				TRACE("中墙检值:%d\r\n",wm_result);
				if((wm_result>0)&(wm_result<4000))
					{
						test_result|=TEST_WM_OK;
						TRACE("中墙检测试通过!!!\r\n");
					}
				else
					{
						TRACE("中墙检试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================20，中右墙检测试====================\r\n");
				break;
			/////////////////中右墙检测试/////////////////////
			case 22:
				if(giv_sys_time-mode.time<TEST_WALL_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=w_rm.difference;
								test_cnt++;
							}
						return;
					}
				wrm_result=test_data1/test_cnt;
				TRACE("中右墙检值:%d\r\n",wrm_result);
				if((wrm_result>0)&(wrm_result<4000))
					{
						test_result|=TEST_WRM_OK;
						TRACE("中右墙检测试通过!!!\r\n");
					}
				else
					{
						TRACE("中右墙检试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================21，右侧墙检测试====================\r\n");
				break;
			/////////////////右侧墙检测试/////////////////////
			case 23:
				if(giv_sys_time-mode.time<TEST_WALL_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=w_r.difference;
								test_cnt++;
							}
						return;
					}
				wr_result=test_data1/test_cnt;
				TRACE("中右墙检值:%d\r\n",wr_result);
				if((wr_result>0)&(wr_result<4000))
					{
						test_result|=TEST_WR_OK;
						TRACE("右侧墙检测试通过!!!\r\n");
					}
				else
					{
						TRACE("右侧墙检试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================22，边刷电流测试====================\r\n");
				Set_BS_Level(STANDARD_PWM);
				break;
			/////////////////边刷电流测试/////////////////////
			case 24:
				if(giv_sys_time-mode.time<TEST_CURR_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=account_current(ADC_SB_CURRENT);
								test_cnt++;
							}
						return;
					}
				sbcurr_result=test_data1/test_cnt;
				TRACE("边刷电流值:%.2fmA\r\n",sbcurr_result*CURR_SB_CNT_mA);
				if((sbcurr_result>0)&(sbcurr_result*CURR_SB_CNT_mA<SB_PROTECTCURRENT))
					{
						test_result|=TEST_SBCURR_OK;
						TRACE("边刷电流测试通过!!!\r\n");
					}
				else
					{
						TRACE("边刷电流测试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				Sweep_Level_Set(SWEEP_LEVEL_STOP);
				TRACE("====================23，中刷电流测试====================\r\n");
				Set_ZS_Level(STANDARD_PWM);
			break;
			/////////////////中刷电流测试/////////////////////
			case 25:
				if(giv_sys_time-mode.time<TEST_CURR_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=account_current(ADC_MB_CURR);
								test_cnt++;
							}
						return;
					}
				mbcurr_result=test_data1/test_cnt;
				TRACE("边刷电流值:%.2fmA\r\n",mbcurr_result*CURR_MB_CNT_mA);
				if((mbcurr_result>0)&(mbcurr_result*CURR_MB_CNT_mA<M_PROTECTCURRENT))
					{
						test_result|=TEST_MBCURR_OK;
						TRACE("中刷电流测试通过!!!\r\n");
					}
				else
					{
						TRACE("中刷电流测试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				Sweep_Level_Set(SWEEP_LEVEL_STOP);
				TRACE("====================24，风机电流测试====================\r\n");
				Set_FJ_Level(STANDARD_PWM);
			break;
			/////////////////风机电流测试/////////////////////
			case 26:
				if(giv_sys_time-mode.time<TEST_CURR_TIME)
					{	
						if(test_100ms_flag)
							{
								test_100ms_flag=false;
								test_data1+=account_current(ADC_FAN_CURR);
								test_cnt++;
							}
						return;
					}
				fancurr_result=test_data1/test_cnt;
				TRACE("边刷电流值:%.2fmA\r\n",fancurr_result*CURR_FAN_CNT_mA);
				if(fancurr_result*CURR_FAN_CNT_mA>DUST_PROTECTCURRENT)
					{
						test_result|=TEST_FANCURR_OK;
						TRACE("风机电流测试通过!!!\r\n");
					}
				else
					{
						TRACE("风机电流测试失败!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_500ms_flag=false;
				Sweep_Level_Set(SWEEP_LEVEL_STOP);
				motion1.tgt_yaw=Gyro_Data.yaw;
				TRACE("====================25，左轮速度测试====================\r\n");
				TRACE("左轮速度目标值:200mm/s!\r\n");				
			break;
			////////////////左轮速度测试/////////////////////
			case 27:
				Speed=SPEED200;
				if(do_action_my(3,200*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				if((test_500ms_flag)&(giv_sys_time-mode.time>5000))
					{
						test_500ms_flag=false;
						if(l_ring.real_speed)
							{
								TRACE("左轮速度值:%dmm/s!\r\n",(u32)(l_ring.real_speed*PULSE_LENGTH));
								test_data1+=l_ring.real_speed;
								test_cnt++;
							}
					}
				break;
			case 28:
				lring_result=test_data1/test_cnt;
				if(abs(lring_result-SPEED200)<30)
					{
						test_result|=TEST_LRING_OK;
						TRACE("左轮速度测试通过!!\r\n");
					}
				else
					{
						TRACE("左轮速度测试失败!!\r\n");
					}
				test_data1=0;test_cnt=0;test_500ms_flag=false;
				mode.step++;
				mode.time=giv_sys_time;
				TRACE("====================26，右轮速度测试====================\r\n");
				TRACE("右轮速度目标值:200mm/s!\r\n");				
				break;
			////////////////右轮速度测试/////////////////////
			case 29:
				Speed=SPEED200;
				if(do_action_my(3,200*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
					}
				if((test_500ms_flag)&(giv_sys_time-mode.time>5000))
					{
						test_500ms_flag=false;
						if(r_ring.real_speed)
							{
								TRACE("右轮速度值:%dmm/s!\r\n",(u32)(r_ring.real_speed*PULSE_LENGTH));
								test_data1+=r_ring.real_speed;
								test_cnt++;
							}
					}
				break;
			case 30:
				rring_result=test_data1/test_cnt;
				if(abs(rring_result-SPEED200)<30)
					{
						test_result|=TEST_RRING_OK;
						TRACE("右轮速度测试通过!!\r\n");
					}
				else
					{
						TRACE("右轮速度测试失败!!\r\n");
					}
				test_data1=0;test_cnt=0;test_500ms_flag=false;
				mode.step++;
				mode.time=giv_sys_time;
				break;
			case 31:
				if(test_1s_flag)
					{
						test_1s_flag=false;
						TRACE("===============测试结果=======================\r\n");
						TRACE("电压校准测试:");
						if(test_result&TEST_VOLCAL_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",full_power);
							}

						TRACE("碰撞测试:");
						if(test_result&TEST_BUMP_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("左碰撞次数:%d 右碰撞次数:\r\n",l_bump_cnt,r_bump_cnt);
							}

						TRACE("电池温度测试:");
						if(test_result&TEST_TEMP_OK)
							TRACE("PASS!\r\n");

						TRACE("惯导测试:");
						if(test_result&TEST_GRYO_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("直充充电测试:");
						if(test_result&TEST_DC_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%.2f\r\n",dc_result*CURR_CHG_CNT);
							}

						TRACE("座充充电测试:");
						if(test_result&TEST_SETA_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%.2f\r\n",seat_result*CURR_CHG_CNT);
							}

						TRACE("左前红外接收测试:");
						if(test_result&TEST_LHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("右前红外接收测试:");
						if(test_result&TEST_RHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("左中红外接收测试:");
						if(test_result&TEST_LMHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("右中红外接收测试:");
						if(test_result&TEST_RMHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("左后红外接收测试:");
						if(test_result&TEST_LBHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("右后红外接收测试:");
						if(test_result&TEST_RBHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("电池电压测试:");
						if(test_result&TEST_VOL_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%.2f\r\n",vol_result*VOLT_CHG_CNT);
							}

						TRACE("左地检测试:");
						if(test_result&TEST_EL_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",el_result);
							}

						TRACE("中地检测试:");
						if(test_result&TEST_EM_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",em_result);
							}

						TRACE("右地检测试:");
						if(test_result&TEST_ER_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",er_result);
							}

						TRACE("左墙检测试:");
						if(test_result&TEST_WL_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",wl_result);
							}

						TRACE("左中墙检测试:");
						if(test_result&TEST_WLM_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",wlm_result);
							}

						TRACE("中墙检测试:");
						if(test_result&TEST_WM_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",wm_result);
							}

						TRACE("右中墙检测试:");
						if(test_result&TEST_WRM_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",wrm_result);
							}

						TRACE("右墙检测试:");
						if(test_result&TEST_WR_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",wr_result);
							}

						TRACE("边刷电流测试:");
						if(test_result&TEST_SBCURR_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",sbcurr_result);
							}

						TRACE("中刷电流测试:");
						if(test_result&TEST_MBCURR_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",mbcurr_result);
							}

						TRACE("风机电流测试:");
						if(test_result&TEST_FANCURR_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",fancurr_result);
							}

						TRACE("左轮速度测试:");
						if(test_result&TEST_LRING_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",lring_result);
							}

						TRACE("右轮速度测试:");
						if(test_result&TEST_RRING_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("错误值:%d\r\n",rring_result);
							}

						if(test_result==0x3FFFFFF)
							{
								TRACE("=================测试通过!!!==============\r\n");
							}
						else
							{
								TRACE("==================测试不通过!==============\r\n");
							}
					}
				break;
		}
}

void ChargeControl_Test(void)
{
	//////////////////充电控制//////////////////////////////////////

	piv_current = account_current(ADC_BAT_CHGCURR);	 //采样当时的电流
	piv_voltage = account_current(ADC_BAT_VOL);   //采样当时的电压
	charge_data.piv_current_sum+=piv_current;
	charge_data.piv_current_count++;
	charge_data.piv_voltage_sum+=piv_voltage;
	charge_data.piv_voltage_count++;
	switch(power.step)
	{
		///////开始充电前的准备/////////////
		case 0:
			power.time = giv_sys_time;
			power.step = 10;
			disable_pwm(CHARGE_PWM);
			return;

		///////为防止插电开机导致的静态电流计算错误,
		///////特在此等待40s,用于导航板开机再行计算
		///////qz add 20180625
		case 10:
			if(giv_sys_time-power.time>10000)
			{
				if(battery_voltage_1s<VOL_13V)			//如果电池电压小于13V,先涓流充电
					{
						power.step=1;
						Battery.BatteryChargeForbid = 0;	//允许大电流充电
					}
				else if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > VOL_16_5V))
					{
						power.step=4;
					}
				else
					{
						Battery.BatteryChargeForbid = 0;	//允许大电流充电
						power.step=2;
					}
				Init_Charge_Data();
				jt_chargecurrent=battery_chargecurrent;
				flag_full = false;					//电池未充满
				power.time=giv_sys_time;
			}
			
			break;
		/////////////////////////接上电极10秒钟，察看电池是否接上
		///////电压小于13V时，进行360mA-静态电流的恢复充电
		case 1:
			Charge_PID_Ctr(360);		//qz modify 150->360:固定360mA
			if((giv_sys_time-power.time>100000)&(battery_voltage>VOL_13V))
				{
					power.step++;
					power.time=giv_sys_time;
					Init_Charge_Data();
				}
			break;		//qz modify return->break 20180823
			
		//使用180ma充电10s并且电池温度低于40度，看电池是否允许大电流充电，不允许则进行涓流充电
		case 2:
		//充电电流大于150MA则降低PWM值
			Charge_PID_Ctr(360);		//qz modify 150->360:固定360mA
			if(giv_sys_time-power.time>100000)
				{
					power.step++;
					power.time = giv_sys_time;
					power.step_time=giv_sys_time;
				}
			break;	
			
		//电池限流限压的过程，判断电池转为涓流的条件是：
		//1、电池充电电流小于360ma；
		//2、电池的绝对温度大于50度
		//3、在电池温度大于40度的情况下，滇池温度每分钟上升速度大于1.5度
		case 3:
		Charge_PID_Ctr(800);
		//判断电池的绝对温度大于50度或者电池电压大于21伏,电池转为涓流充电	 2369
			if((battery_voltage > full_power))	//qz modify 20180703:1805 16V 1861:16.5V
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//电池的充电电流小于360ma,进入小恒流充电
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //电流小于360ma
			&&((giv_sys_time - power.time) > 600000))
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//在大电流充电4小时以上，强制性进行涓流充电。
			if((giv_sys_time - power.time) > 144000000) 	//qz modify 144000000 4h
			{
				power.step = 4;
				power.time = giv_sys_time;	  
				//Battery.BatteryFDCap = 0; 					//qz mask 20180625
				Battery.BatteryChargeForbid = 1;
			}

			break;	
		case 4:
			Init_Charge_Data();
			power.time=giv_sys_time;
			power.step++;
		break;

		//涓流充电
		case 5:
#if 0
			Init_Charge_Data();
			power.time=giv_sys_time;
			power.step++;
#else
		//充电电流大于180MA则降低PWM值
			Charge_PID_Ctr(360);	//qz modify 150->360:固定360mA
			////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false; 		
				//充电电流小于静态电流+100mA,则转入恒压充电
				if((((battery_chargecurrent - jt_chargecurrent) < 248)) //100mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	防止第1步直接到第4步时，刚开始因为充电PWM没开，充电电流为0，小于52mA 
				&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
				{
					Init_Charge_Data();
					Battery.BatteryChargeForbid = 1;
					power.step++;
					power.time=giv_sys_time;
					return;
				}
	
				//涓充大于600分钟认为电池充满电
				if(((giv_sys_time - power.time) > 360000000)		//600分钟  
					&&(flag_full==false))						//qz add 20180910
					
				{
					flag_full = true;
					//qz add 20180710 
					//如果电池充满时,电池放电量还大于1200(mAs),则当前电池电量和放电量明显偏大,需要调整
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,绿灯常亮5s.qz modify 不限时常亮
					power.step=7;
					return;
				}

			//涓流充电电压大于16.8V,认为充电已满
			//qz add 20180710
			//实测1882(16.67V)时,电池进入保护
			//涓流充电电压大于16.8V,进入恒压充电
			if(((giv_sys_time-power.time>100000)&&(battery_voltage>full_power)&&(!flag_full)))
				{
					Init_Charge_Data();
					Battery.BatteryChargeForbid = 1;
					power.step++;
					power.time=giv_sys_time;
					return;
				}
			//qz add end
			}
#endif
		break;

		//恒压充电
		case 6:
		//充电电流大于180MA则降低PWM值
			Charge_PID_CtrPwr(full_power);	//qz modify 150->360:固定360mA
			Battery.BatteryChargeForbid = 1;	//恒压充电,禁止大电流充电
			////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;

#if 1
				if((((battery_chargecurrent - jt_chargecurrent) < 62)) //25mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	防止第1步直接到第4步时，刚开始因为充电PWM没开，充电电流为0，小于52mA 
				&& (flag_full == false))
				{
					flag_full = true;

					//qz add 20180710 
					//如果电池充满时,电池放电量还大于1200(mAs),则当前电池电量和放电量明显偏大,需要调整
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,绿灯常亮5s.qz modify 不限时常亮
					power.step++;
				}
#endif
			//涓充大于120分钟或者充电电压大于17V认为电池充满电
				if((((giv_sys_time - power.time) > 72000000)|(battery_voltage>VOL_17V))&&(!flag_full))		//120分钟  
				{
					flag_full = true;
					//qz add 20180710 
					//如果电池充满时,电池放电量还大于1200(mAs),则当前电池电量和放电量明显偏大,需要调整
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,绿灯常亮5s.qz modify 不限时常亮
					power.step++;			//qz add 20180910
				}
			}
			break;
		case 7:
			//电池充满电，PWM关闭后，电池会自放电，长时间(比如一个月)后，电池需补充电量
				if((flag_full == true)&&(battery_voltage_1s < VOL_15_8V))		//15.8V
				{
					power.step = 3;
					flag_full = false;
					power.time = giv_sys_time;
					Init_Charge_Data(); 	//qz add 20180522
					Send_Voice(VOICE_CHARGING_START);
				}
		
			//防止电池保护板坏时一直充电，而充坏电池  (2015.4.7)
				if(flag_full == true)
				{
					power.pwm = 0;
					Init_Charge_Data(); 	//qz add 20180522
				}
			break;
		default :
			power.step = 0;
			break;
	}

	//if((charge_dc.key == 0)&&(charge_seat.key == 0))		//qz mask 20180515
	if(!(power.charge_dc)&&(!power.charge_seat))	//qz add 20180515
	{
		disable_pwm(CHARGE_PWM);
		power.pwm = 0;
		Init_Charge_Data();
	}
	else
	{
		enable_pwm(CHARGE_PWM,power.pwm);	//执行pwm动作
	}		
}

void Test_Nothing(void)
{
	
}
