#include"AAA-include.h"

FACTORY_DATA	factory_data;

void Init_FactoryData(void)
{	
	factory_data.oc_data=0;
}


void Init_FactoryTest(void)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
    /******初始化显示***********/
    clr_display();
	Clr_DisplayData();
    Dis_On = true;            		//显示标志。
    Dis_HourOn = true; 				//小时亮
    Dis_MinuteOn = true; 			//分钟亮
    Dis_SecOn=true;
    Dis_AllFlagOn=true;
	Dis_PowerOn=true;				
	Dis_StatusOn=true;
		
	/******初始化设置的值********************/
	clr_ram();
//	ReInitAd();

	Enable_earth();
	Enable_wall();
	enable_hwincept();				//允许红外接收电源
	Enable_Speed();  				//允许速度发送
//	Set_DustStatus_Send();			//允许垃圾已满检测
	Sweep_Level_Set(STOP_ALL);
	 
	Display_Real();
	Init_Action();
	
	mode.mode 											= CEASE;
	mode.sub_mode										= FACTORY_TEST;
	mode.bump 											= 0;
	mode.action 										= 0;
	mode.step=0;
	mode.Command_Spd_L=0;			//qz add 20180725
	mode.Command_Spd_R=0;			//qz add 20180725
	mode.step=0xA0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.factory=true;
	mode.factory_burnning=false;
	mode.factory_tst_item=0;
	Init_FactoryData();
	WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;

	//qz add 20180313
//	Write_Mode_Backup();
	//qz add end
#ifdef DEBUG_Enter_Mode
	TRACE("Init Factory Mode Complete!\r\n");
#endif
	//初始化检测的条件
	CHECK_STATUS_FLAG=false;		//qz add 20180725:如果机器处于休眠时,接到控制命令会直接进入此状态,因此需要打开检测开关
//	Init_Check_Status();//qz add 20180425
}


static void Ring_Test(void)
{
	if((mode.Command_Spd_L==0)&(mode.Command_Spd_R==0))
		{
			stop_rap();
		}
	else
		{
			if(mode.Command_Spd_L>0)
				{
					l_rap.rap=mode.Command_Spd_L;
					l_rap.length=0xFFFFFFFF;
					l_rap.sign=1;
					l_rap.ori=FRONT;
				}
			else if(mode.Command_Spd_L<0)
				{
					l_rap.rap=abs(mode.Command_Spd_L);
					l_rap.length=0xFFFFFFFF;
					l_rap.sign=1;
					l_rap.ori=BACK;
				}
			else
				{
					l_rap.rap=0;
					l_rap.length=0;
					l_rap.sign=0;
				}
	
			if(mode.Command_Spd_R>0)
				{
					r_rap.rap=mode.Command_Spd_R;
					r_rap.length=0xFFFFFFFF;
					r_rap.sign=1;
					r_rap.ori=FRONT;
				}
			else if(mode.Command_Spd_R<0)
				{
					r_rap.rap=abs(mode.Command_Spd_R);
					r_rap.length=0xFFFFFFFF;
					r_rap.sign=1;
					r_rap.ori=BACK;
				}
			else
				{
					r_rap.rap=0;
					r_rap.length=0;
					r_rap.sign=0;
				}
		}
}
static void Charge_Test(void)
{
	switch (mode.test_step)
		{
			case 0:
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
				if((!power.charge_dc)&(!power.charge_seat))
					{
						mode.test_step=0;
					}
				break;
		}
}


static void OC_Test(u8 test_item)
{
	u8 data1;
	switch(mode.test_step)
		{
			case 0:
				Enable_Speed();
				Sweep_Level_Set(STOP_ALL);
				stop_rap();
				factory_data.oc_data=0;
				mode.test_step++;
				break;
			case 1:
				switch(test_item)
					{
						case FAC_TST_LSBOC:
						case FAC_TST_RSBOC:
							Set_BS_Level(STANDARD_PWM);
							start_time=giv_sys_time;
							mode.test_step++;
							break;
						case FAC_TST_LRINGOC:
							enable_pwm(L_FRONT,800);
							l_rap.ori=FRONT;
							length1=l_ring.all_length;
							start_time=giv_sys_time;
							mode.test_step++;
							TRACE("length1=%d  l_ring.all_length=%d\r\n",length1,l_ring.all_length);
							break;
						case FAC_TST_RRINGOC:
							enable_pwm(R_FRONT,800);
							r_rap.ori=FRONT;
							length2=r_ring.all_length;
							start_time=giv_sys_time;
							mode.test_step++;
							break;
						case FAC_TST_MBOC:
							Set_ZS_Level(STANDARD_PWM);
							start_time=giv_sys_time;
							mode.test_step++;
							break;
						default:
							mode.test_step=4;
							break;
					}
				break;
			case 2:									//qz add 20180828
				if(giv_sys_time-start_time>20000)
					{
						mode.test_step++;
						start_time=giv_sys_time;
					}
				break;
			case 3:
				switch (test_item)
					{
						case FAC_TST_LSBOC:
						case FAC_TST_RSBOC:
							data1=SB_FIX_Check();
							if((data1==1))
								{
									factory_data.oc_data=0x04;
									mode.test_step++;
									Sweep_Level_Set(STOP_ALL);
									start_time=giv_sys_time;									
								}
							else if((data1==2))
								{
									factory_data.oc_data=0x08;
									mode.test_step++;
									Sweep_Level_Set(STOP_ALL);
									start_time=giv_sys_time;									
								}
							break;
						case FAC_TST_MBOC:
							if(MB_OC_Check())
								{
									factory_data.oc_data=0x10;
									mode.test_step++;
									Sweep_Level_Set(STOP_ALL);
									start_time=giv_sys_time;									
								}
							break;
						case FAC_TST_LRINGOC:
							if(giv_sys_time-start_time>10000)
								{
									if((l_ring.all_length>length1+30))
										{
											length1=l_ring.all_length;
											start_time=giv_sys_time;
											TRACE("length1=%d  l_ring.all_length=%d\r\n",length1,l_ring.all_length);
										}
									else 
										{
											disable_pwm(L_FRONT);
											factory_data.oc_data=0x01;
											mode.test_step++;
											start_time=giv_sys_time;
											TRACE("length1=%d  l_ring.all_length=%d\r\n",length1,l_ring.all_length);
										}
								}
							break;
						case FAC_TST_RRINGOC:
							if(giv_sys_time-start_time>10000)
								{
									if((r_ring.all_length>length2+30))
										{
											length2=r_ring.all_length;
											start_time=giv_sys_time;
										}
									else 
										{
											disable_pwm(R_FRONT);
											factory_data.oc_data=0x02;
											mode.test_step++;
											start_time=giv_sys_time;									
										}
								}
							break;
						default:
							mode.test_step++;
							start_time=giv_sys_time;									
							break;
					}
				break;
			case 4:
				if(giv_sys_time-start_time<5000)
					break;
				Enable_Speed();
				Sweep_Level_Set(STOP_ALL);
				stop_rap();
				factory_data.oc_data=0;
				mode.factory_tst_item=0;
				break;
		}
}

static void Periph_Test(u8 test_item)
{
	switch (test_item)
		{
			case FAC_TST_MBON:
				Set_ZS_Level(STANDARD_PWM);
				break;
			case FAC_TST_MBOFF:
				Set_ZS_Level(STOP_ALL);
				break;
			case FAC_TST_SBON:
				Set_BS_Level(STANDARD_PWM);
				break;
			case FAC_TST_SBOFF:
				Set_BS_Level(STOP_ALL);
				break;
			case FAC_TST_FANON:
				Set_FJ_Level(STANDARD_PWM);
				break;
			case FAC_TST_FANOFF:
				Set_FJ_Level(STOP_ALL);
				break;
			case FAC_TST_LEDON:
				Dis_AllLedOn=true;
				break;
			case FAC_TST_LEDOFF:
				Dis_AllLedOn=false;
				break;
			case FAC_TST_VOICE:
				Send_Voice(VOICE_VOLUME_TEST);
				mode.factory_tst_item=0;
				break;
#ifdef UV
			case FAC_TST_UVON:
				Set_UV();
				break;
			case FAC_TST_UVOFF:
				Reset_UV();
				break;
#endif
		}
}

void Do_FactoryTest(void)
{
	YBS_Comm_Rap_My();
	//clr_all_hw_effect();
	//enable_hwincept();
	clr_all_hw_effect();
	switch (mode.factory_tst_item)
		{
			case 0:
				Enable_wall();
				Enable_earth();
				enable_hwincept();
				Enable_Speed();
				stop_rap();
				Sweep_Level_Set(STOP_ALL);
				Dis_AllLedOn=false;
#ifdef UV
				Reset_UV();
#endif
				break;
			case FAC_TST_RING:
				Ring_Test();
				break;
			case FAC_TST_LRINGOC:
			case FAC_TST_RRINGOC:
			case FAC_TST_MBOC:
			case FAC_TST_LSBOC:
			case FAC_TST_RSBOC:
				OC_Test(mode.factory_tst_item);
				break;
			case FAC_TST_MBON:
			case FAC_TST_MBOFF:
			case FAC_TST_SBON:
			case FAC_TST_SBOFF:
			case FAC_TST_FANON:
			case FAC_TST_FANOFF:
			case FAC_TST_LEDON:
			case FAC_TST_LEDOFF:
			case FAC_TST_VOICE:
			case FAC_TST_UVON:
			case FAC_TST_UVOFF:
				Periph_Test(mode.factory_tst_item);
				break;
			case FAC_TST_CHARGE:
				Charge_Test();
				break;
			case FAC_TST_CHARGEOFF:
				Init_Charge_Data();
				power.pwm=0;
				power.step=0;
				disable_pwm(CHARGE_PWM);
				break;

			//厂测跑机，使能异常检测
			//同时会切换到其他模式进行运动。
			case FAC_TST_BURNNING:
				mode.factory_burnning=true;
				CHECK_STATUS_FLAG=true;
				Init_Check_Status();
				break;
		}
}

