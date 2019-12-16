#include "AAA-include.h"
bool test_100ms_flag,test_500ms_flag,test_1s_flag;
u32 test_result=0; 
void Init_Test(u8 test_mode)
{
	stop_rap();
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	/******��ʼ����ʾ***********/
		
	/******��ʼ�����õ�ֵ********************/
	clr_ram();
//	ReInitAd();
	Enable_earth();
	Enable_wall();
	enable_hwincept();				//���������յ�Դ
	Enable_Speed(); 				//�����ٶȷ���
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
	Enable_Free_Skid_Check();		//�������ּ��
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
				TRACE("======================��ӭ���볧��ģʽ======================\r\n");
				TRACE("========================׼������============================\r\n");
				mode.step=100;
				break;
			case 100:
				if((giv_sys_time-mode.time>30000)&(Read_Key2()))
					mode.step=100;
				mode.step=1;
				mode.time=giv_sys_time;
				TRACE("======================1����ѹУ׼����========================\r\n");
				break;
				
			//////////////////��ѹУ׼����////////////////////////////
			case 1:
				if(giv_sys_time-mode.time<12*10000)
					return;
				full_power=battery_voltage_10s;
				TRACE("У׼ֵ:%d\r\n",full_power);
				if(full_power!=0)
					{
						TRACE("��ѹУ׼�ɹ�!!!\r\n");
						test_result|=TEST_VOLCAL_OK;
					}
				else
					{
						TRACE("��ѹУ׼ʧ��!!!\r\n");
					}
				TRACE("======================2����ײ����====================\r\n");
				TRACE("����10���ڣ���������ײ������3��!!\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				l_bump_cnt=0;r_bump_cnt=0;
				break;
			//////////////////��ײ����////////////////////////////
			case 2:
				if(giv_sys_time-mode.time<10*10000)
					{
						bump=Parse_BumpValue();
						if((bump==BUMP_ONLY_LEFT)&(test_500ms_flag))
							{
								test_500ms_flag=false;
								TRACE("����ײ!\r\n");
								l_bump_cnt++;
							}
						else if((bump==BUMP_ONLY_RIGHT)&(test_500ms_flag))
							{	
								test_500ms_flag=false;
								TRACE("����ײ!\r\n");
								r_bump_cnt++;
							}
						return;
					}
				TRACE("����ײ%d��!����ײ%d��\r\n",l_bump_cnt,r_bump_cnt);
				if((l_bump_cnt>=3)&(r_bump_cnt>=3))
					{
						TRACE("��ײ����ͨ��!!!\r\n");
						test_result|=TEST_BUMP_OK;
					}
				else
					{
						TRACE("��ײ����ʧ��!!!\r\n");
					}
				TRACE("======================3������¶Ȳ���====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////����¶Ȳ���////////////////////////////
			case 3:
				TRACE("����¶Ȳ���ͨ��!!!\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				TRACE("======================4�������ǲ���====================\r\n");
				break;
			//////////////////�����ǲ���////////////////////////////
			case 4:
				if(giv_sys_time-mode.time<50000)
					return;
				if(Gyro_Data.tick_flag)
					{
						test_result|=TEST_GRYO_OK;
						TRACE("�����ǲ���ͨ��!!!\r\n");
					}
				else
					TRACE("�����ǲ���ʧ��!!!\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;
				TRACE("======================5��ֱ�������====================\r\n");
				TRACE("����10���ڲ���ֱ������\r\n");
				break;
			//////////////////DC����////////////////////////////
			case 5:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(power.charge_dc)
							{
								TRACE("�Ѽ�⵽ֱ������!!!\r\n");
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
										TRACE("�����ֱ������!!!\r\n");
									}
							}
						return;
					}
				TRACE("δ��⵽ֱ������!!!\r\n");
				TRACE("ֱ�������ʧ��!!!\r\n");
				TRACE("======================6�����������====================\r\n");
				TRACE("����10���ڽ�������\r\n");
				dc_result=0;
				mode.step=7;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;
				break;
			case 6:
				ChargeControl_Test();
				if(!power.charge_dc)
					{
						TRACE("δ��⵽ֱ������!!\r\n");
						mode.step=5;
						return;
					}
				if(giv_sys_time-mode.time<15*10000)
					{
						if(test_1s_flag)
							{
								test_1s_flag=false;
								TRACE("������:%.2fmA\r\n",battery_chargecurrent_1s*CURR_CHG_CNT);
								//test_data1+=battery_chargecurrent_1s;
								//test_cnt++;
							}
						return;
					}
				//test_data1=test_data1/test_cnt;
				TRACE("���ճ�����:%.2fmA\r\n",battery_chargecurrent_1s*CURR_CHG_CNT);
				dc_result=battery_chargecurrent_1s;
				switch(power.step)
					{
						case 2:
						case 5:
							if(abs((u32)(battery_chargecurrent_1s*CURR_CHG_CNT)-360)<50)
								{
									test_result|=TEST_DC_OK;
									TRACE("ֱ�������ͨ��!!!\r\n");
								}
							else
								TRACE("ֱ�������ʧ��!!!\r\n");
							break;
						case 3:
							if(abs((u32)(battery_chargecurrent_1s*CURR_CHG_CNT)-800)<150)
								{
									test_result|=TEST_DC_OK;
									TRACE("ֱ�������ͨ��!!!\r\n");
								}
							else
								TRACE("ֱ�������ʧ��!!!\r\n");
							break;
						default:
							if(battery_chargecurrent_1s>0)
								{
									test_result|=TEST_DC_OK;
									TRACE("ֱ�������ͨ��!!!\r\n");
								}
							else
								TRACE("ֱ�������ʧ��!!!\r\n");
							break;
								
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;
				TRACE("======================6�����������====================\r\n");
				TRACE("����10���ڽ�������\r\n");
				break;
			//////////////////SEAT����////////////////////////////
			case 7:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(power.charge_seat)
							{
								TRACE("�Ѽ�⵽�����!!!\r\n");
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
										TRACE("���������!!!\r\n");
									}
							}
						return;
					}
				TRACE("δ��⵽�����!!!\r\n");
				TRACE("���������ʧ��!!!\r\n");
				mode.step=9;
				mode.time=giv_sys_time;
				seat_result=0;
				TRACE("====================7����ǰ������ղ���====================\r\n");
				break;
			case 8:
				ChargeControl_Test();
				if(!power.charge_seat)
					{
						TRACE("δ��⵽�����!!\r\n");
						mode.step=7;
						return;
					}
				if(giv_sys_time-mode.time<15*10000)
					{
						if(test_1s_flag)
							{
								test_1s_flag=false;
								TRACE("������:%.2fmA\r\n",battery_chargecurrent_1s*CURR_CHG_CNT);
								//test_data1+=battery_chargecurrent_1s;
								//test_cnt++;
							}
						return;
					}
				//test_data1=test_data1/test_cnt;
				TRACE("���ճ�����:%.2fmA\r\n",battery_chargecurrent_1s*CURR_CHG_CNT);
				seat_result=battery_chargecurrent_1s;
				switch(power.step)
					{
						case 2:
						case 5:
							if(abs((u32)(battery_chargecurrent_1s*CURR_CHG_CNT)-360)<50)
								{
									test_result|=TEST_SETA_OK;
									TRACE("���������ͨ��!!!\r\n");
								}
							else
								TRACE("���������ʧ��!!!\r\n");
							break;
						case 3:
							if(abs((u32)(battery_chargecurrent_1s*CURR_CHG_CNT)-800)<150)
								{
									test_result|=TEST_SETA_OK;
									TRACE("���������ͨ��!!!\r\n");
								}
							else
								TRACE("���������ʧ��!!!\r\n");
							break;
						default:
							if(battery_chargecurrent_1s>0)
								{
									test_result|=TEST_SETA_OK;
									TRACE("���������ͨ��!!!\r\n");
								}
							else
								TRACE("���������ʧ��!!!\r\n");
							break;
								
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;
				TRACE("====================7����ǰ������ղ���====================\r\n");
				break;
			//////////////////��ǰ������ղ���/////////////////
			case 9:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(l_hw.effect)
							{
								test_result|=TEST_LHW_OK;
								TRACE("��ǰ�������ͨ��!!!\r\n");
								TRACE("====================8����ǰ������ղ���====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("��ǰ�������ʧ��!!!\r\n");
				TRACE("====================8����ǰ������ղ���====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////��ǰ������ղ���/////////////////
			case 10:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(r_hw.effect)
							{
								test_result|=TEST_RHW_OK;
								TRACE("��ǰ�������ͨ��!!!\r\n");
								TRACE("====================9�����������ղ���====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("��ǰ�������ʧ��!!!\r\n");
				TRACE("====================9�����������ղ���====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////���������ղ���/////////////////
			case 11:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(lm_hw.effect)
							{
								test_result|=TEST_LMHW_OK;
								TRACE("����������ͨ��!!!\r\n");
								TRACE("====================10�����Һ�����ղ���====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("����������ʧ��!!!\r\n");
				TRACE("====================10�����Һ�����ղ���====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////���Һ�����ղ���/////////////////
			case 12:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(rm_hw.effect)
							{
								test_result|=TEST_RMHW_OK;
								TRACE("���Һ������ͨ��!!!\r\n");
								TRACE("====================11�����������ղ���====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("���Һ������ʧ��!!!\r\n");
				TRACE("====================11�����������ղ���====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////���������ղ���/////////////////
			case 13:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(lb_hw.effect)
							{
								test_result|=TEST_LBHW_OK;
								TRACE("����������ͨ��!!!\r\n");
								TRACE("====================12�����Һ�����ղ���====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("����������ʧ��!!!\r\n");
				TRACE("====================12�����Һ�����ղ���====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			//////////////////���Һ�����ղ���/////////////////
			case 14:
				if(giv_sys_time-mode.time<10*10000)
					{
						if(rb_hw.effect)
							{
								test_result|=TEST_RBHW_OK;
								TRACE("���Һ������ͨ��!!!\r\n");
								TRACE("====================13����ص�ѹ����====================\r\n");
								mode.step++;
								mode.time=giv_sys_time;
							}
						return;
					}
				TRACE("���Һ������ʧ��!!!\r\n");
				TRACE("====================13����ص�ѹ����====================\r\n");
				mode.step++;
				mode.time=giv_sys_time;
				break;
			/////////////////��ص�ѹ����///////////////////////
			case 15:
				vol_result=battery_voltage_1s;
				TRACE("��ص�ѹ:%.2fV\r\n",vol_result*VOLT_CHG_CNT);
				if((vol_result>=VOL_12_8V)&(vol_result<=VOL_17_1V))
					{						
						test_result|=TEST_VOL_OK;
						TRACE("��ص�ѹ����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("��ص�ѹ����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================14�����ؼ����====================\r\n");
				break;
			/////////////////���ؼ����/////////////////////
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
				TRACE("���ؼ�ֵ:%d\r\n",el_result);
				if((el_result>0)&(el_result<4000))
					{
						test_result|=TEST_EL_OK;
						TRACE("���ؼ����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("���ؼ����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================15���еؼ����====================\r\n");
				break;
			/////////////////���ؼ����/////////////////////
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
				TRACE("�еؼ�ֵ:%d\r\n",em_result);
				if((em_result>0)&(em_result<4000))
					{
						test_result|=TEST_EM_OK;
						TRACE("�еؼ����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("�еؼ����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================16���Ҳ�ؼ����====================\r\n");
				break;
			/////////////////�Ҳ�ؼ����/////////////////////
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
				TRACE("�Ҳ�ؼ�ֵ:%d\r\n",er_result);
				if((er_result>0)&(er_result<4000))
					{
						test_result|=TEST_ER_OK;
						TRACE("�Ҳ�����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("�Ҳ�����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================17�����ǽ�����====================\r\n");
				break;
			/////////////////���ǽ�����/////////////////////
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
				TRACE("���ǽ��ֵ:%d\r\n",wl_result);
				if((wl_result>0)&(wl_result<4000))
					{
						test_result|=TEST_WL_OK;
						TRACE("���ǽ�����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("���ǽ����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================18������ǽ�����====================\r\n");
				break;
			/////////////////����ǽ�����/////////////////////
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
				TRACE("����ǽ��ֵ:%d\r\n",wlm_result);
				if((wlm_result>0)&(wlm_result<4000))
					{
						test_result|=TEST_WLM_OK;
						TRACE("����ǽ�����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("����ǽ����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================19����ǽ�����====================\r\n");
				break;
			/////////////////��ǽ�����/////////////////////
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
				TRACE("��ǽ��ֵ:%d\r\n",wm_result);
				if((wm_result>0)&(wm_result<4000))
					{
						test_result|=TEST_WM_OK;
						TRACE("��ǽ�����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("��ǽ����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================20������ǽ�����====================\r\n");
				break;
			/////////////////����ǽ�����/////////////////////
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
				TRACE("����ǽ��ֵ:%d\r\n",wrm_result);
				if((wrm_result>0)&(wrm_result<4000))
					{
						test_result|=TEST_WRM_OK;
						TRACE("����ǽ�����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("����ǽ����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================21���Ҳ�ǽ�����====================\r\n");
				break;
			/////////////////�Ҳ�ǽ�����/////////////////////
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
				TRACE("����ǽ��ֵ:%d\r\n",wr_result);
				if((wr_result>0)&(wr_result<4000))
					{
						test_result|=TEST_WR_OK;
						TRACE("�Ҳ�ǽ�����ͨ��!!!\r\n");
					}
				else
					{
						TRACE("�Ҳ�ǽ����ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				TRACE("====================22����ˢ��������====================\r\n");
				Set_BS_Level(STANDARD_PWM);
				break;
			/////////////////��ˢ��������/////////////////////
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
				TRACE("��ˢ����ֵ:%.2fmA\r\n",sbcurr_result*CURR_SB_CNT_mA);
				if((sbcurr_result>0)&(sbcurr_result*CURR_SB_CNT_mA<SB_PROTECTCURRENT))
					{
						test_result|=TEST_SBCURR_OK;
						TRACE("��ˢ��������ͨ��!!!\r\n");
					}
				else
					{
						TRACE("��ˢ��������ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				Sweep_Level_Set(SWEEP_LEVEL_STOP);
				TRACE("====================23����ˢ��������====================\r\n");
				Set_ZS_Level(STANDARD_PWM);
			break;
			/////////////////��ˢ��������/////////////////////
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
				TRACE("��ˢ����ֵ:%.2fmA\r\n",mbcurr_result*CURR_MB_CNT_mA);
				if((mbcurr_result>0)&(mbcurr_result*CURR_MB_CNT_mA<M_PROTECTCURRENT))
					{
						test_result|=TEST_MBCURR_OK;
						TRACE("��ˢ��������ͨ��!!!\r\n");
					}
				else
					{
						TRACE("��ˢ��������ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_100ms_flag=false;
				Sweep_Level_Set(SWEEP_LEVEL_STOP);
				TRACE("====================24�������������====================\r\n");
				Set_FJ_Level(STANDARD_PWM);
			break;
			/////////////////�����������/////////////////////
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
				TRACE("��ˢ����ֵ:%.2fmA\r\n",fancurr_result*CURR_FAN_CNT_mA);
				if(fancurr_result*CURR_FAN_CNT_mA>DUST_PROTECTCURRENT)
					{
						test_result|=TEST_FANCURR_OK;
						TRACE("�����������ͨ��!!!\r\n");
					}
				else
					{
						TRACE("�����������ʧ��!!!\r\n");
					}
				mode.step++;
				mode.time=giv_sys_time;
				test_data1=0;test_cnt=0;test_500ms_flag=false;
				Sweep_Level_Set(SWEEP_LEVEL_STOP);
				motion1.tgt_yaw=Gyro_Data.yaw;
				TRACE("====================25�������ٶȲ���====================\r\n");
				TRACE("�����ٶ�Ŀ��ֵ:200mm/s!\r\n");				
			break;
			////////////////�����ٶȲ���/////////////////////
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
								TRACE("�����ٶ�ֵ:%dmm/s!\r\n",(u32)(l_ring.real_speed*PULSE_LENGTH));
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
						TRACE("�����ٶȲ���ͨ��!!\r\n");
					}
				else
					{
						TRACE("�����ٶȲ���ʧ��!!\r\n");
					}
				test_data1=0;test_cnt=0;test_500ms_flag=false;
				mode.step++;
				mode.time=giv_sys_time;
				TRACE("====================26�������ٶȲ���====================\r\n");
				TRACE("�����ٶ�Ŀ��ֵ:200mm/s!\r\n");				
				break;
			////////////////�����ٶȲ���/////////////////////
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
								TRACE("�����ٶ�ֵ:%dmm/s!\r\n",(u32)(r_ring.real_speed*PULSE_LENGTH));
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
						TRACE("�����ٶȲ���ͨ��!!\r\n");
					}
				else
					{
						TRACE("�����ٶȲ���ʧ��!!\r\n");
					}
				test_data1=0;test_cnt=0;test_500ms_flag=false;
				mode.step++;
				mode.time=giv_sys_time;
				break;
			case 31:
				if(test_1s_flag)
					{
						test_1s_flag=false;
						TRACE("===============���Խ��=======================\r\n");
						TRACE("��ѹУ׼����:");
						if(test_result&TEST_VOLCAL_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",full_power);
							}

						TRACE("��ײ����:");
						if(test_result&TEST_BUMP_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ײ����:%d ����ײ����:\r\n",l_bump_cnt,r_bump_cnt);
							}

						TRACE("����¶Ȳ���:");
						if(test_result&TEST_TEMP_OK)
							TRACE("PASS!\r\n");

						TRACE("�ߵ�����:");
						if(test_result&TEST_GRYO_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("ֱ�������:");
						if(test_result&TEST_DC_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%.2f\r\n",dc_result*CURR_CHG_CNT);
							}

						TRACE("���������:");
						if(test_result&TEST_SETA_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%.2f\r\n",seat_result*CURR_CHG_CNT);
							}

						TRACE("��ǰ������ղ���:");
						if(test_result&TEST_LHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("��ǰ������ղ���:");
						if(test_result&TEST_RHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("���к�����ղ���:");
						if(test_result&TEST_LMHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("���к�����ղ���:");
						if(test_result&TEST_RMHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("��������ղ���:");
						if(test_result&TEST_LBHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("�Һ������ղ���:");
						if(test_result&TEST_RBHW_OK)
							TRACE("PASS!\r\n");
						else
							TRACE("FAIL!\r\n");

						TRACE("��ص�ѹ����:");
						if(test_result&TEST_VOL_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%.2f\r\n",vol_result*VOLT_CHG_CNT);
							}

						TRACE("��ؼ����:");
						if(test_result&TEST_EL_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",el_result);
							}

						TRACE("�еؼ����:");
						if(test_result&TEST_EM_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",em_result);
							}

						TRACE("�ҵؼ����:");
						if(test_result&TEST_ER_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",er_result);
							}

						TRACE("��ǽ�����:");
						if(test_result&TEST_WL_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",wl_result);
							}

						TRACE("����ǽ�����:");
						if(test_result&TEST_WLM_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",wlm_result);
							}

						TRACE("��ǽ�����:");
						if(test_result&TEST_WM_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",wm_result);
							}

						TRACE("����ǽ�����:");
						if(test_result&TEST_WRM_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",wrm_result);
							}

						TRACE("��ǽ�����:");
						if(test_result&TEST_WR_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",wr_result);
							}

						TRACE("��ˢ��������:");
						if(test_result&TEST_SBCURR_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",sbcurr_result);
							}

						TRACE("��ˢ��������:");
						if(test_result&TEST_MBCURR_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",mbcurr_result);
							}

						TRACE("�����������:");
						if(test_result&TEST_FANCURR_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",fancurr_result);
							}

						TRACE("�����ٶȲ���:");
						if(test_result&TEST_LRING_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",lring_result);
							}

						TRACE("�����ٶȲ���:");
						if(test_result&TEST_RRING_OK)
							TRACE("PASS!\r\n");
						else
							{
								TRACE("FAIL!  ");
								TRACE("����ֵ:%d\r\n",rring_result);
							}

						if(test_result==0x3FFFFFF)
							{
								TRACE("=================����ͨ��!!!==============\r\n");
							}
						else
							{
								TRACE("==================���Բ�ͨ��!==============\r\n");
							}
					}
				break;
		}
}

void ChargeControl_Test(void)
{
	//////////////////������//////////////////////////////////////

	piv_current = account_current(ADC_BAT_CHGCURR);	 //������ʱ�ĵ���
	piv_voltage = account_current(ADC_BAT_VOL);   //������ʱ�ĵ�ѹ
	charge_data.piv_current_sum+=piv_current;
	charge_data.piv_current_count++;
	charge_data.piv_voltage_sum+=piv_voltage;
	charge_data.piv_voltage_count++;
	switch(power.step)
	{
		///////��ʼ���ǰ��׼��/////////////
		case 0:
			power.time = giv_sys_time;
			power.step = 10;
			disable_pwm(CHARGE_PWM);
			return;

		///////Ϊ��ֹ��翪�����µľ�̬�����������,
		///////���ڴ˵ȴ�40s,���ڵ����忪�����м���
		///////qz add 20180625
		case 10:
			if(giv_sys_time-power.time>10000)
			{
				if(battery_voltage_1s<VOL_13V)			//�����ص�ѹС��13V,��������
					{
						power.step=1;
						Battery.BatteryChargeForbid = 0;	//�����������
					}
				else if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > VOL_16_5V))
					{
						power.step=4;
					}
				else
					{
						Battery.BatteryChargeForbid = 0;	//�����������
						power.step=2;
					}
				Init_Charge_Data();
				jt_chargecurrent=battery_chargecurrent;
				flag_full = false;					//���δ����
				power.time=giv_sys_time;
			}
			
			break;
		/////////////////////////���ϵ缫10���ӣ��쿴����Ƿ����
		///////��ѹС��13Vʱ������360mA-��̬�����Ļָ����
		case 1:
			Charge_PID_Ctr(360);		//qz modify 150->360:�̶�360mA
			if((giv_sys_time-power.time>100000)&(battery_voltage>VOL_13V))
				{
					power.step++;
					power.time=giv_sys_time;
					Init_Charge_Data();
				}
			break;		//qz modify return->break 20180823
			
		//ʹ��180ma���10s���ҵ���¶ȵ���40�ȣ�������Ƿ�����������磬�����������������
		case 2:
		//����������150MA�򽵵�PWMֵ
			Charge_PID_Ctr(360);		//qz modify 150->360:�̶�360mA
			if(giv_sys_time-power.time>100000)
				{
					power.step++;
					power.time = giv_sys_time;
					power.step_time=giv_sys_time;
				}
			break;	
			
		//���������ѹ�Ĺ��̣��жϵ��תΪ����������ǣ�
		//1����س�����С��360ma��
		//2����صľ����¶ȴ���50��
		//3���ڵ���¶ȴ���40�ȵ�����£�����¶�ÿ���������ٶȴ���1.5��
		case 3:
		Charge_PID_Ctr(800);
		//�жϵ�صľ����¶ȴ���50�Ȼ��ߵ�ص�ѹ����21��,���תΪ������	 2369
			if((battery_voltage > full_power))	//qz modify 20180703:1805 16V 1861:16.5V
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//��صĳ�����С��360ma,����С�������
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //����С��360ma
			&&((giv_sys_time - power.time) > 600000))
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//�ڴ�������4Сʱ���ϣ�ǿ���Խ��������硣
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

		//������
		case 5:
#if 0
			Init_Charge_Data();
			power.time=giv_sys_time;
			power.step++;
#else
		//����������180MA�򽵵�PWMֵ
			Charge_PID_Ctr(360);	//qz modify 150->360:�̶�360mA
			////////ÿ�����ж�һ��////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false; 		
				//������С�ھ�̬����+100mA,��ת���ѹ���
				if((((battery_chargecurrent - jt_chargecurrent) < 248)) //100mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	��ֹ��1��ֱ�ӵ���4��ʱ���տ�ʼ��Ϊ���PWMû����������Ϊ0��С��52mA 
				&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
				{
					Init_Charge_Data();
					Battery.BatteryChargeForbid = 1;
					power.step++;
					power.time=giv_sys_time;
					return;
				}
	
				//丳����600������Ϊ��س�����
				if(((giv_sys_time - power.time) > 360000000)		//600����  
					&&(flag_full==false))						//qz add 20180910
					
				{
					flag_full = true;
					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,�̵Ƴ���5s.qz modify ����ʱ����
					power.step=7;
					return;
				}

			//�������ѹ����16.8V,��Ϊ�������
			//qz add 20180710
			//ʵ��1882(16.67V)ʱ,��ؽ��뱣��
			//�������ѹ����16.8V,�����ѹ���
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

		//��ѹ���
		case 6:
		//����������180MA�򽵵�PWMֵ
			Charge_PID_CtrPwr(full_power);	//qz modify 150->360:�̶�360mA
			Battery.BatteryChargeForbid = 1;	//��ѹ���,��ֹ��������
			////////ÿ�����ж�һ��////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;

#if 1
				if((((battery_chargecurrent - jt_chargecurrent) < 62)) //25mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	��ֹ��1��ֱ�ӵ���4��ʱ���տ�ʼ��Ϊ���PWMû����������Ϊ0��С��52mA 
				&& (flag_full == false))
				{
					flag_full = true;

					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,�̵Ƴ���5s.qz modify ����ʱ����
					power.step++;
				}
#endif
			//丳����120���ӻ��߳���ѹ����17V��Ϊ��س�����
				if((((giv_sys_time - power.time) > 72000000)|(battery_voltage>VOL_17V))&&(!flag_full))		//120����  
				{
					flag_full = true;
					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,�̵Ƴ���5s.qz modify ����ʱ����
					power.step++;			//qz add 20180910
				}
			}
			break;
		case 7:
			//��س����磬PWM�رպ󣬵�ػ��Էŵ磬��ʱ��(����һ����)�󣬵���貹�����
				if((flag_full == true)&&(battery_voltage_1s < VOL_15_8V))		//15.8V
				{
					power.step = 3;
					flag_full = false;
					power.time = giv_sys_time;
					Init_Charge_Data(); 	//qz add 20180522
					Send_Voice(VOICE_CHARGING_START);
				}
		
			//��ֹ��ر����廵ʱһֱ��磬���仵���  (2015.4.7)
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
		enable_pwm(CHARGE_PWM,power.pwm);	//ִ��pwm����
	}		
}

void Test_Nothing(void)
{
	
}
