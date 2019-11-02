
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

//******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义************
//=====================全局变量====================================

//=====================全局函数====================================



//******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义************
//=====================私有变量====================================


//=====================私有函数====================================



//===============================================================================================================
//===============================================================================================================





bool m_hw_test,l_hw_test,r_hw_test,b_hw_test;




////////////////////////私有定义//////////////////////////////////
#define  VER   107
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////
void frontandback(void) ;
///////////////////////私有函数////////////////////////////////////
void Test_Earth(uint8 state);
void Test_Wall(uint8 state);
void Test_HwIncept(uint8 state);
void Test_Current(uint8 state);
void Test_Action(uint8 state);
void Test_KeyAndYaoKong(uint8 state);
void Test_Display(uint8 state);
void Test_MainBoard(uint8 state);
///////////////////////函数实体////////////////////////////////////
/******************************************************************
自检整个机器
******************************************************************/
u32 Dis_TestData=0,start_time,current;
int length1,length2;
void Init_Test(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	clr_display();
	Dis_On=true;
	Dis_StatusOn=true;
	Dis_Yuyue=false;
	Enable_wall();			//qz modify 20180902
	Enable_earth();
 	Enable_Speed();
	enable_hwincept();
	mode.Info_Abort=1;		//不接收SLAM指令
	mode.All_Info_Abort=1;			//qz add 20180919
	mode.step=0;
	mode.test_item=TST_MENU;
	mode.sub_tst_item=0;
	mode.test_dis_data=0;	
	mode.mode=CEASE;
	mode.sub_mode=SELF_TEST;
//	mode.run_time=giv_sys_time;
	mode.test_step=0;
	WriteWorkState();

	Sweep_Level_Set(STOP_ALL);
	stop_rap();
	
	CHECK_STATUS_FLAG=false;
	Init_Check_Status();

	Send_Voice(VOICE_VOLUME_3);

#ifdef UV
	Reset_UV();
#endif
}

void FAN_MB_Test(void)
{
	
	switch(mode.test_step)
		{
			case 0:
				length1=Fan.length;
				current=m_current_1s;
				mode.test_dis_data=1;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
						mode.test_dis_data=NONE;
					}
			break;
			case 1:
				Set_FJ_Level(STANDARD_PWM);
				Set_ZS_Level(STANDARD_PWM);
//				Set_BS_Level(STANDARD_PWM);
				start_time=giv_sys_time;
				mode.test_step++;
				break;
			case 2:
				if(giv_sys_time-start_time>50000)
					{
						if((Fan.length>length1+10)&(m_current_1s>(current+10)))
							{
								mode.test_dis_data=PASS;
							}
						else
							{
								mode.test_dis_data=FAIL;
							}
						mode.test_step++;
						Sweep_Level_Set(STOP_ALL);
					}
				break;
			case 3:
				if(Read_Button_Key(&key2)==1)
					{
						length1=Fan.length;
						current=m_current_1s;
						mode.test_step++;
						mode.test_dis_data=NONE;
					}
				break;
			case 4:
				Set_FJ_Level(FORCE_PWM);
				Set_ZS_Level(FORCE_PWM);
//				Set_BS_Level(STANDARD_PWM);
				start_time=giv_sys_time;
				mode.test_step++;
				break;
			case 5:
				if(giv_sys_time-start_time>50000)
					{
						if((Fan.length>length1+10)&(m_current_1s>(current+10)))
							{
								mode.test_dis_data=PASS;
							}
						else
							{
								mode.test_dis_data=FAIL;
							}
						mode.test_step++;
						Sweep_Level_Set(STOP_ALL);
					}
				break;
			case 6:
				break;
		}
}

void Voice_Test(void)
{
	mode.test_dis_data=2;
	switch(mode.test_step)
		{
			case 0:
				//Send_Voice(VOICE_VOLUME_TEST);
				if(Read_Button_Key(&key2)==1)
					mode.test_step++;
				break;
			case 1:
				Send_Voice(VOICE_VOLUME_TEST);
				mode.test_step++;
				break;
			case 2:
				if(Read_Button_Key(&key2)==1)
					mode.test_step=1;					
				break;
		}
}

void Switch_Test(void)
{
	switch (mode.test_step)
		{
			case 0:
				mode.test_dis_data=3;
				if(dust_box.key)
					dust_box.flag=true;
				else
					dust_box.flag=false;

				if(dust_sta.key)
					dust_sta.flag=true;
				else
					dust_sta.flag=false;

				if(l_bump.key)
					l_bump.flag=true;
				else
					l_bump.flag=false;

				if(r_bump.key)
					r_bump.flag=true;
				else
					r_bump.flag=false;

				if(m_bump.key)
					m_bump.flag=true;
				else
					m_bump.flag=false;

				if(l_lidi.key)
					l_lidi.flag=true;
				else
					l_lidi.flag=false;

				if(r_lidi.key)
					r_lidi.flag=true;
				else
					r_lidi.flag=false;

				dust_sta.fail=false;
				dust_box.fail=false;
				l_bump.fail=false;
				m_bump.fail=false;
				r_bump.fail=false;
				l_lidi.fail=false;
				r_lidi.fail=false;
				//Set_DustStatus_Send();

				mode.test_step++;

				
			break;
			case 1:
				if(dust_box.key)
					{
						if((!dust_box.flag)&(!dust_box.fail))
							{
								dust_box.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_Laji=true;
					}
				else
					{
						Dis_Laji=false;
						if((dust_box.flag)&(!dust_box.fail))
							{
								dust_box.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				
				if(dust_sta.key)
					{
						Dis_Laji=!Dis_Laji;
						if((!dust_sta.flag)&(!dust_sta.fail))
							{
								dust_sta.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				else if(!dust_sta.key)
					{
						if((dust_sta.flag)&(!dust_sta.fail))
							{
								dust_sta.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				
				if(l_bump.key)
					{
						Dis_Guihua=false;
						if((!l_bump.flag)&(!l_bump.fail))
							{
								l_bump.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				else
					{
						Dis_Guihua=true;
						if((l_bump.flag)&(!l_bump.fail))
							{
								l_bump.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				
				if(m_bump.key)
					{
						Dis_YBS=false;
						if((!m_bump.flag)&(!m_bump.fail))
							{
								m_bump.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				else
					{
						Dis_YBS=true;
						if((m_bump.flag)&(!m_bump.fail))
							{
								m_bump.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				
				if(r_bump.key)
					{
						Dis_Zhongdian=false;
						if((!r_bump.flag)&(!r_bump.fail))
							{
								r_bump.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				else
					{
						Dis_Zhongdian=true;
						if((r_bump.flag)&(!r_bump.fail))
							{
								r_bump.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}

				if(l_lidi.key)
					{
						Dis_Docking=false;
						if((!l_lidi.flag)&(!l_lidi.fail))
							{
								l_lidi.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				else
					{
						Dis_Docking=true;
						if((l_lidi.flag)&(!l_lidi.fail))
							{
								l_lidi.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				
				if(r_lidi.key)
					{
						Dis_Yuyue=false;
						if((!r_lidi.flag)&(!r_lidi.fail))
							{
								r_lidi.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}
				else
					{
						Dis_Yuyue=true;
						if((r_lidi.flag)&(!r_lidi.fail))
							{
								r_lidi.fail=true;
								Send_Voice(VOICE_VOLUME_2);
							}
					}

				if(l_lidi.fail&r_lidi.fail&l_bump.fail&r_bump.fail&m_bump.fail&dust_box.fail&dust_sta.fail)
					{
						mode.test_step++;
						dust_sta.fail=false;
						dust_box.fail=false;
						l_bump.fail=false;
						m_bump.fail=false;
						r_bump.fail=false;
						l_lidi.fail=false;
						r_lidi.fail=false;
						mode.test_dis_data=PASS;
						Clr_Alldisflag();
						Dis_Yuyue=false;
					}
				break;
			case 2:
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step=0;
					}
				
				break;
				
		}
}

void Wall_Test(void)
{
	switch (mode.test_step)
		{
			case 0:
				Enable_wall();
				Clr_Alldisflag();
				Dis_Yuyue=false;
				mode.test_dis_data=TST_WALL;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				break;
			case 1:
				Clr_Alldisflag();
				Dis_Yuyue=false;
				
				Dis_Guihua=true;
				mode.test_dis_data=w_l.dis;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				break;
			case 2:
				Clr_Alldisflag();
				Dis_Yuyue=false;
				
				Dis_YBS=true;
				mode.test_dis_data=w_lm.dis;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				break;
			case 3:
				Clr_Alldisflag();
				Dis_Yuyue=false;
				
				Dis_Zhongdian=true;
				mode.test_dis_data=w_m.dis;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				break;
			case 4:
				Clr_Alldisflag();
				Dis_Yuyue=false;
				
				Dis_Docking=true;
				mode.test_dis_data=w_rm.dis;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				break;
			case 5:
				Clr_Alldisflag();
				Dis_Yuyue=false;
				
				Dis_Yuyue=true;
				mode.test_dis_data=w_r.dis;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step=1;
					}
				break;
			
		}
}

void Wall_Test_II(void)
{
	static bool w_l_test,w_lm_test,w_m_test,w_rm_test;
	switch (mode.test_step)
		{
			case 0:
				Clr_Alldisflag();
				Dis_Yuyue=false;
				init_wallearth();
				Enable_wall();
				mode.test_step++;
				w_l_test=false;
				w_lm_test=false;
				w_m_test=false;
				w_rm_test=false;
				mode.test_dis_data=TST_WALL;
				break;
			case 1:
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				if((w_l.sign==NEAR))
					{
						Send_Voice(VOICE_VOLUME_4);
						Dis_Guihua=true;
						mode.test_step=10;
					}

				if((w_lm.sign==NEAR))
					{
						Send_Voice(VOICE_VOLUME_4);
						Dis_YBS=true;
						mode.test_step=10;
					}

				if((w_m.sign==NEAR))
					{
						Send_Voice(VOICE_VOLUME_4);
						Dis_Zhongdian=true;
						mode.test_step=10;
					}
				if((w_rm.sign==NEAR))
					{
						Send_Voice(VOICE_VOLUME_4);
						Dis_Docking=true;
						mode.test_step=10;
					}
				break;
			case 2:
				mode.test_dis_data=w_r.dis;
				if(Read_Button_Key(&key2)==1)
						{
						mode.test_step=0;
					}

				if((w_l.sign==NEAR)&(!w_l_test))
					{
						Send_Voice(VOICE_VOLUME_2);
						Dis_Guihua=true;
						w_l_test=true;
					}

				if((w_lm.sign==NEAR)&(!w_lm_test))
					{
						Send_Voice(VOICE_VOLUME_2);
						Dis_YBS=true;
						w_lm_test=true;
					}

				if((w_m.sign==NEAR)&(!w_m_test))
					{
						Send_Voice(VOICE_VOLUME_2);
						Dis_Zhongdian=true;
						w_m_test=true;
					}
				if((w_rm.sign==NEAR)&(!w_rm_test))
					{
						Send_Voice(VOICE_VOLUME_2);
						Dis_Docking=true;
						w_rm_test=true;
					}
				break;
			case 10:
				mode.test_dis_data=FAIL;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step=0;
					}
				break;

		}
}

void Earth_Test(void)
{
	static bool l_one,m_one,r_one;
	switch(mode.test_step)
		{
			case 0:
				Clr_Alldisflag();
				Dis_Yuyue=false;
				mode.test_dis_data=5;
				Enable_earth();
				if(e_l.sign==NEAR)
					e_l.flag=true;
				else
					e_l.flag=false;

				if(e_m.sign==NEAR)
					e_m.flag=true;
				else
					e_m.flag=false;

				if(e_r.sign==NEAR)
					e_r.flag=true;
				else
					e_r.flag=false;

				l_one=false;
				m_one=false;
				r_one=false;

				mode.test_step++;
				break;
			case 1:
				if(e_l.sign==NEAR)
					{
						if((!e_l.flag)&(!l_one))
							{
								l_one=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_YBS=false;
					}
				else
					{
						if((e_l.flag)&(!l_one))
							{
								l_one=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_YBS=true;
					}

				if(e_m.sign==NEAR)
					{
						if((!e_m.flag)&(!m_one))
							{
								m_one=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_Zhongdian=false;
					}
				else
					{
						if((e_m.flag)&(!m_one))
							{
								m_one=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_Zhongdian=true;
					}
				
				if(e_r.sign==NEAR)
					{
						if((!e_r.flag)&(!r_one))
							{
								r_one=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_Docking=false;
					}
				else
					{
						if((e_r.flag)&(!r_one))
							{
								r_one=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_Docking=true;
					}

				if(l_one&m_one&r_one)
					{
						mode.test_dis_data=PASS;
						Clr_Alldisflag();
						Dis_Yuyue=false;
						mode.test_step++;
					}
				break;
			case 2:
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step=0;
					}
				break;

					
		}
}

//qz add 20180927
void Earth_Test_II(void)
{
	switch (mode.test_step)
		{
			case 0:
				Clr_Alldisflag();
				Dis_Yuyue=false;
				mode.test_dis_data=5;
				Enable_earth();
				mode.time=giv_sys_time;
				mode.test_step++;
				e_l.flag=false;
				e_m.flag=false;
				e_r.flag=false;
				break;
			case 1:
				if(giv_sys_time-mode.time>1000)
					mode.test_step++;
				break;
			case 2:
				if(e_l.sign==FARN)
					{
						Dis_YBS=true;
						mode.test_step=10;
					}
				if(e_m.sign==FARN)
					{
						Dis_Zhongdian=true;
						mode.test_step=10;
					}
				if(e_r.sign==FARN)
					{
						Dis_Docking=true;
						mode.test_step=10;
					}
				if((e_l.sign==NEAR)&(e_m.sign==NEAR)&(e_r.sign==NEAR))
					{
						mode.test_step++;
					}
				break;
			case 3:
				if(e_l.sign==FARN)
					{
						if(!e_l.flag)
							{
								e_l.flag=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_YBS=true;
					}
				else
					{
						Dis_YBS=false;
					}

				if(e_m.sign==FARN)
					{
						if(!e_m.flag)
							{
								e_m.flag=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_Zhongdian=true;
					}
				else
					{
						Dis_Zhongdian=false;
					}

				if(e_r.sign==FARN)
					{
						if(!e_r.flag)
							{
								e_r.flag=true;
								Send_Voice(VOICE_VOLUME_2);
							}
						Dis_Docking=true;
					}
				else
					{
						Dis_Docking=false;
					}

				if(e_l.flag&e_m.flag&e_r.flag)
					{
						mode.test_dis_data=PASS;
						Clr_Alldisflag();
						Dis_Yuyue=false;
						mode.test_step++;
					}
				break;
			case 4:
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step=0;
					}
				break;
			case 10:
				mode.test_dis_data=FAIL;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step=0;
					}
				break;
		}
}
void OC_Test(void)
{
	static bool l_sb,r_sb,mb,lring,rring;
	u8 data1;
	switch(mode.test_step)
		{
			case 0:
				mode.test_dis_data=6;
				if(Read_Button_Key(&key2))
					{
						mode.test_step++;
					}
				break;
			case 1:
				Enable_Speed();
				Set_BS_Level(STANDARD_PWM);
				Set_ZS_Level(STANDARD_PWM);
				enable_pwm(L_FRONT,800);
				l_rap.ori=FRONT;
				enable_pwm(R_FRONT,800);
				r_rap.ori=FRONT;
				l_sb=false;
				r_sb=false;
				mb=false;
				lring=false;
				rring=false;
				length1=l_ring.all_length;
				length2=r_ring.all_length;
				start_time=giv_sys_time;
				mode.test_step++;
			case 2:									//qz add 20180828
				if(giv_sys_time-start_time>20000)
					{
						mode.test_step++;
						start_time=giv_sys_time;
					}
				break;
			case 3:
				data1=SB_FIX_Check();
				if((data1==1)&(!l_sb))
					{
						l_sb=true;
						Send_Voice(VOICE_VOLUME_2);
						Dis_Guihua=true;
					}
				if((data1==2)&(!r_sb))
					{
						r_sb=true;
						Send_Voice(VOICE_VOLUME_2);
						Dis_YBS=true;
					}
				if(l_sb&r_sb)
					Set_BS_Level(STOP_ALL);
	
				if(MB_OC_Check()&(!mb))
					{
						mb=true;
						Set_ZS_Level(STOP_ALL);
						Send_Voice(VOICE_VOLUME_2);
						Dis_Zhongdian=true;
					}

				if(giv_sys_time-start_time>10000)
					{
						if((l_ring.all_length>length1+30)&(!lring))
							{
								length1=l_ring.all_length;
								start_time=giv_sys_time;
							}
						else if(!lring)
							{
								lring=true;
								disable_pwm(L_FRONT);
								Send_Voice(VOICE_VOLUME_2);
								Dis_Docking=true;
							}

						if((r_ring.all_length>length2+30)&(!rring))
							{
								length2=r_ring.all_length;
								start_time=giv_sys_time;
							}
						else if(!rring)
							{
								rring=true;
								disable_pwm(R_FRONT);
								Send_Voice(VOICE_VOLUME_2);
								Dis_Yuyue=true;
							}
					}

				if(l_sb&r_sb&mb&lring&rring)
					{
						Sweep_Level_Set(STOP_ALL);
						stop_rap();
						mode.test_step++;
						mode.test_dis_data=PASS;
					}
				break;
			case 4:
				if(Read_Button_Key(&key2))
					{
						Clr_Alldisflag();
						Dis_Yuyue=false;
						mode.test_step=0;
					}
				break;
		}
}

void Dock_Test(void)
{
 	switch(mode.test_step)
		{
			case 0:
				mode.test_dis_data=TST_DOCK;
				if(Read_Button_Key(&key2)==1)
					{
						Clr_Alldisflag();
						Dis_Yuyue=false;
						m_hw_test=false;
						l_hw_test=false;
						r_hw_test=false;
						b_hw_test=false;
						Disable_wall();
						Disable_earth();
						enable_hwincept();
						clr_all_hw_effect();
						mode.test_step++;
					}
				break;
			case 1:
				clr_all_hw_effect();
				mode.test_dis_data=ReadHwSign_My();
				if(Read_Button_Key(&key2)==1)

					{
						Disable_wall();
						Disable_earth();
						disable_hwincept();
						clr_all_hw_effect();
						mode.test_step=0;
					}
				
				//在Remote_Handle()函数中有FORWORD/LEFT/RIGHT/BACK键，也可以进行测试
				if(l_hw_test&m_hw_test&r_hw_test&b_hw_test)
					{
						mode.test_step++;
						mode.test_dis_data=PASS;
						Clr_Alldisflag();
						Dis_Yuyue=false;
					}
				break;
			case 2:
				if(Read_Button_Key(&key2)==1)

					{
						Disable_wall();
						Disable_earth();
						disable_hwincept();
						clr_all_hw_effect();
						mode.test_step=0;
					}
				break;
		}
}

void ChargeControl_Test(void)
{
	u16 piv_current_tst;//piv_voltage_tst;

	//////////////////充电控制//////////////////////////////////////

	piv_current_tst = account_current(CHARGE_CURRENT);	 //采样当时的电流
//	piv_voltage_tst = account_current(BATTERY_VOLTAGE);   //采样当时的电压
	charge_data.piv_current_sum+=piv_current_tst;
	charge_data.piv_current_count++;
	switch(power.step)
	{
		///////开始充电前的准备/////////////
		case 0:
			power.time = giv_sys_time;
			power.step++;
			disable_pwm(CHARGE_PWM);
			return;
		/////////////////////////接上电极10秒钟，察看电池是否接上
		case 1:
			if((giv_sys_time - power.time) > 20000)//接触上电2秒钟
			{
				//if( battery_temp_1s < 355)
				if((battery_temp_1s<355)|(battery_temp_1s>3902))	//qz modify 20180515
				{
					error_code=SEND_ERROR_BAT;
					dis_err_code=DIS_ERROR_BAT_DOWN;
					Init_Err();
					return ;
				}

			#if 0	//qz mask 20180515
				if(battery_temp_1s > 3902)
				{
	//						giv_sys_err = 26;
					Init_Err();
					return ;
				}
			#endif
				jt_chargecurrent = (u32)(CEASE_CUR*2.5);		//1/CURR_CHG_CNT=2.5
				power.step = 2;
				power.temp = battery_temp;//Get_BatteryTemp();
				power.time = giv_sys_time;

				//如果电池电压大于16.2V，且不允许进行大电流充电，则进入第四步——涓流充电
				if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > 1827)) //15.8V~1783   16.2V~1827
				{
					power.step = 4;
				}
				else//处理机器长时间放置不用，电池自放电造成电量严重减少时，需要进行大电流充电
				{
					Battery.BatteryChargeForbid = 0;	//允许大电流充电
				}

				flag_full = false;					//电池未充满
			}
			return; 
			
	//使用180ma充电10s并且电池温度低于40度，看电池是否允许大电流充电，不允许则进行涓流充电
		case 2:
		//充电电流大于150MA则降低PWM值
		Charge_PID_Ctr(360);		//qz modify 150->360:固定360mA
			if(((giv_sys_time - power.time) > 100000)&&(battery_temp > temp[70])) //充电时间为5分钟并且电池温度低于40度
			{
				power.step = 3;
				power.time = giv_sys_time;
				power.step_time=giv_sys_time;
			}
			if((giv_sys_time - power.time) > 72000000)		//2 hour
			{
				power.step = 3;
				power.time = giv_sys_time;
				power.step_time=giv_sys_time;
			}
			break;	
			
			
	//电池限流限压的过程，判断电池转为涓流的条件是：
	//1、电池充电电流小于360ma；
	//2、电池的绝对温度大于50度
	//3、在电池温度大于40度的情况下，滇池温度每分钟上升速度大于1.5度
		case 3:
		//充电电流大于设定电流则降低PWM值
		#if 1
		Charge_PID_Ctr(800);
		
		#endif
//		power.pwm=1625;	
		//判断电池的绝对温度大于50度或者电池电压大于21伏,电池转为涓流充电	 2369
			if((battery_temp < temp[80])|| (battery_voltage > 1861))	//qz modify 20180703:1805 16V 1861:16.5V
			//if((battery_temp < temp[80])|| (battery_voltage > 1919))  //17.1V~1929	 17.5V~1974 16.9V~1909	17.3~1952  16.8~1896  17~1919
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//电池的充电电流小于360ma认为电池已经充满
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //电流小于360ma
			&&((giv_sys_time - power.time) > 600000))
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//在大电流充电3小时以上，强制性将电池置为充饱状态。
			if((giv_sys_time - power.time) > 144000000)		//qz modify 144000000 4h
			{
				power.step = 4;
				power.time = giv_sys_time;	  
				//Battery.BatteryFDCap = 0;						//qz mask 20180625
				Dis_PowerGlint = false;
				Battery.BatteryChargeForbid = 1;
			}
		////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
				//piv_temp = Get_BatteryTemp();
			//电池的绝对温度大于40度，
				if(battery_temp < temp[70])
				{
				//电池的温升速度大于1.5度
					//if((piv_temp > power.temp)&&((piv_temp - power.temp) > 3))
					if((battery_temp < power.temp)&&((power.temp - battery_temp) > 50))
					{
						power.step = 4;
						power.time = giv_sys_time;
					}
				}
				//power.temp = piv_temp;
				power.temp = battery_temp;
			}
			break;	
		case 4:
			Init_Charge_Data();
			power.step++;
		break;
		case 5:
		//充电电流大于180MA则降低PWM值
			Charge_PID_Ctr(360);	//qz modify 150->360:固定360mA
			////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;

				//TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CURR_CHG_CNT));
				//TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CURR_CHG_CNT));

				if((((battery_chargecurrent - jt_chargecurrent) < 248)) //100mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	防止第1步直接到第4步时，刚开始因为充电PWM没开，充电电流为0，小于52mA 
				&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
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
					Dis_PowerGlint = false;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
				}
			//涓充大于600分钟认为电池充满电
				if(((giv_sys_time - power.time) > 360000000)		//600分钟  
					&&(Dis_PowerGlint == true))
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
					Dis_PowerGlint = false;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
				}

			//涓流充电电压大于16.8V,认为充电已满
			//qz add 20180710
			//实测1882(16.67V)时,电池进入保护
			if(((giv_sys_time-power.time>100000)&&(battery_voltage>1880)&&(!flag_full)))
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
					Dis_PowerGlint = false;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
				}
			//qz add end
			
			//电池充满电，PWM关闭后，电池会自放电，长时间(比如一个月)后，电池需补充电量
				if((flag_full == true)&&(battery_voltage_1s < 1783))		//15.8V
				{
					power.step = 3;
					flag_full = false;
					Dis_PowerGlint = true;
					power.time = giv_sys_time;
					Init_Charge_Data();		//qz add 20180522
					Send_Voice(VOICE_CHARGING_START);
				}
			}

		//防止电池保护板坏时一直充电，而充坏电池  (2015.4.7)
			if(flag_full == true)
			{
				power.pwm = 0;
				Init_Charge_Data();		//qz add 20180522
			}

			break;
		default :
			power.step = 0; 	
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


void Charge_Test(void)
{
	switch (mode.test_step)
		{
			case 0:

				mode.test_dis_data=TST_CHG;
				if((power.charge_dc|power.charge_seat)&(Read_Button_Key(&key2)==1))
					{
						Init_Charge_Data();
						power.pwm = 0;
						disable_pwm(CHARGE_PWM);
						power.step = 0;
						mode.test_step++;
					}
				break;
			case 1:
				ChargeControl_Test();
				if(power.step==5)
					{
						mode.test_dis_data=FULL;
					}
				else
					{
						mode.test_dis_data=(u32)(battery_chargecurrent_1s*CURR_CHG_CNT);		//显示电流，单位mA
					}
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				if((!power.charge_dc)&(!power.charge_seat))
					{
						mode.test_step=0;
					}
				break;
			case 2:
				ChargeControl_Test();
				if(power.step==5)
					{
						mode.test_dis_data=FULL;
					}
				else
					{
						mode.test_dis_data=(u32)(battery_voltage_1s*VOLT_CHG_CNT*100);		//显示电流，单位V
					}
				if((!power.charge_dc)&(!power.charge_seat))
					{
						mode.test_step=0;
					}
				break;
		}
}


void Ring_Speed_Test(void)
{
	switch(mode.test_step)
		{
			case 0:
				mode.test_dis_data=TST_SPD;
				Enable_Speed();
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				break;
			case 1:
				Speed=3000;
				if(do_action(3, FARAWAY));
				if(Half_Sec)
					{
						Half_Sec=false;
						mode.test_dis_data=(u32)(l_ring.real_speed/PULSES_PER_MM);
					}
				Dis_Guihua=true;
				Dis_Yuyue=false;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				break;
			case 2:
				Speed=3000;
				if(do_action(3, FARAWAY));
				Dis_Guihua=false;
				Dis_Yuyue=true;
				if(Half_Sec)
					{
						Half_Sec=false;
						mode.test_dis_data=(u32)(r_ring.real_speed/PULSES_PER_MM);
					}
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step=1;
					}
				break;
		}
}

void Free_Ring_Test(void)
{
	switch(mode.test_step)
		{
			case 0:
				mode.test_dis_data=TST_FREE;
				if(Read_Button_Key(&key2)==1)
					{
						Enable_Speed();
						w_ring.length=0;
						mode.test_step++;
					}
				break;
			case 1:
				mode.test_dis_data=w_ring.length;
				if(Read_Button_Key(&key2)==1)
					{
						w_ring.length=0;
						mode.test_step=0;
					}
				break;
		}
}

void Free_Ring_Speed_Test(void)
{
	switch (mode.test_step)
		{
			case 0:
				mode.test_dis_data=TST_FREE_SPD;
				if(Read_Button_Key(&key2)==1)
					{
						Enable_Speed();
						w_ring.length=0;
						mode.test_step++;
					}
				break;
			case 1:
				mode.test_dis_data=w_ring.length;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
						start_time=giv_sys_time;
					}
				break;
			case 2:
				Speed=2000;
				if(do_action(3,FARAWAY));
				if(giv_sys_time-start_time>10000)
					{
						stop_rap();
						mode.test_step=1;
					}
				mode.test_dis_data=w_ring.length;
				break;
		}
}

void Ring_Test(void)
{
	switch (mode.test_step)
		{
			case 0:
				stop_rap();
				Enable_Speed();
				mode.test_dis_data=12;
				w_ring.length=0;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
						start_time=giv_sys_time;
					}
				break;
			case 1:										//delay 500ms
				if(giv_sys_time-start_time>5000)
					mode.test_step++;
				break;
			case 2:										//中速前进2m
				Speed=2500;
				mode.test_dis_data=w_ring.length;
				if(do_action(3,200*CM_PLUS))
					{
						stop_rap();
						mode.test_step++;
						start_time=giv_sys_time;
					}
				if(Read_Button_Key(&key2)==1)			//前往高速
					{
						stop_rap();
						mode.test_step=5;
						start_time=giv_sys_time;
					}
				break;
			case 3:										//delay 500ms
				if(giv_sys_time-start_time>5000)
					mode.test_step++;
				break;
			case 4:										//中速后退2m
				Speed=2500;
				mode.test_dis_data=w_ring.length;
				if(do_action(4,200*CM_PLUS))
					{	
						stop_rap();
						mode.test_step=1;
						start_time=giv_sys_time;
					}
				if(Read_Button_Key(&key2)==1)			//前往高速
					{
						stop_rap();
						mode.test_step=5;
						start_time=giv_sys_time;
					}
				break;
			case 5:										//delay 500ms
				if(giv_sys_time-start_time>5000)
					{
						mode.test_step++;
					}
				break;
			case 6:
				Speed=4000;								//高速前进2m
				mode.test_dis_data=w_ring.length;
				if(do_action(3,200*CM_PLUS))
					{
						stop_rap();
						mode.test_step++;
						start_time=giv_sys_time;
					}
				if(Read_Button_Key(&key2)==1)			//返回低速
					{
						stop_rap();
						mode.test_step=1;
						start_time=giv_sys_time;
					}
				break;
			case 7:										//delay 500ms
				if(giv_sys_time-start_time>5000)
					{
						mode.test_step++;
					}
				break;
			case 8:										//高速后退2m
				Speed=4000;
				mode.test_dis_data=w_ring.length;
				if(do_action(4,200*CM_PLUS))
					{
						stop_rap();
						mode.test_step=5;
						start_time=giv_sys_time;
					}
				if(Read_Button_Key(&key2)==1)			//返回低速
					{
						stop_rap();
						mode.test_step=1;
						start_time=giv_sys_time;
					}
				break;
		}
}

void Wall_Data_Test(void)
{
	switch (mode.test_step)
		{
			case 0:
				stop_rap();
				Enable_wall();
				mode.test_dis_data=13;
				break;
		}
	
	if(Read_Button_Key(&key2)==1)			//返回低速
		{
			stop_rap();
			mode.test_step++;
			if(mode.test_step>=6)
				mode.test_step=0;
			start_time=giv_sys_time;
		}
}

void UV_Test(void)
{
#ifdef UV
	switch (mode.test_step)
		{
			case 0:
				mode.test_dis_data=TST_UV;
				Reset_UV();
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step++;
					}
				break;
			case 1:
				Set_UV();
				mode.test_dis_data=NONE;
				if(Read_Button_Key(&key2)==1)
					{
						mode.test_step=0;
					}
				break;
		}
#endif
}

void Do_Test_My(void)
{

//	ACC_DEC_Comm_rap_My();
//	YBS_Comm_Rap_My();
	ACC_DEC_Curve();
	switch (mode.test_item)
		{
			case TST_MENU:			//测试界面
				mode.test_dis_data=MAIN_MENU;
				break;
			case TST_FAN_MB:		//风扇检查 滚刷检测
				FAN_MB_Test();
				break;
			case TST_VOCIE:			//声音检查
				Voice_Test();
				break;
			case TST_SWITCH:
				Switch_Test();
				break;
			case TST_WALL:
				//Wall_Test();
				Wall_Test_II();
				break;
			case TST_EARTH:
				//Earth_Test();
				Earth_Test_II();
				break;
			case TST_OC:
				OC_Test();
				break;
			case TST_DOCK:
				Dock_Test();
				break;
			case TST_CHG:
				Charge_Test();
				break;
			case TST_SPD:
				Ring_Speed_Test();
				break;
			case TST_FREE:
				Free_Ring_Test();
				break;
			case TST_FREE_SPD:
				Free_Ring_Speed_Test();
				break;
			case 12:
				Ring_Test();
				break;
			case TST_WALL_DATA:
				Wall_Data_Test();
				break;
#ifdef UV
			case TST_UV:
				UV_Test();
				break;
#endif
		}

	if(Read_Button_Key(&key1)==1)
		{
			mode.test_item++;
			if(mode.test_item>MAX_TST_ITEM)
				mode.test_item=0;
			mode.test_step=0;
			Sweep_Level_Set(STOP_ALL);
			stop_rap();
			Clr_Alldisflag();
			Dis_Yuyue=false;
#ifdef UV
			Reset_UV();
#endif
		}
	
	if(Read_Button_Key(&key3)==1)
		{
			Init_Cease();
		}
}


