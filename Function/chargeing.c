////////////////////////ͷ�ļ�////////////////////////////////////	
#include "AAA-include.h"
////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////	  
void Do_Chargeing(void);

bool Quit_Charge_Button=false;
///////////////////////˽�к���////////////////////////////////////	
///////////////////////����ʵ��////////////////////////////////////
/******************************************************************
���ܣ���ʼ��������
******************************************************************/
void Init_Switchoff(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	Init_Ring();					//qz add 20180803���³�ʼ�������й���Ϣ
	/******��ʼ����ʾ***********/
	/******��ʼ�����õ�ֵ********************/
	clr_ram();
	Init_Sweep_Pwm(PWM_CHARGE_MAX,PWM_CHARGE_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//�رշ��
	stop_rap(); 					//ֹͣ����
	Disable_earth();
	Disable_wall();
	disable_hwincept();				//��ֹ������յ�Դ
	Disable_Speed();  				//��ֹ����ƹⷢ��

	mode.mode = CHARGEING;
	mode.sub_mode=SWITCHOFF;
	mode.Info_Abort =1; 			//��ֹSLAM�ٶ�ͨ��
	mode.All_Info_Abort=1;			//��ֹSLAMͨ��,qz add 20180919
	mode.step=0; 					//��ֹ�ӳ��������
	mode.status=0;					//�ǹ���״̬
	mode.time=giv_sys_time;
	WriteWorkState();

	power.pwm = 0;
	power.step=0;
	disable_pwm(CHARGE_PWM);
	Init_Charge_Data(); 				//qz add 20180522
	CHECK_STATUS_FLAG=false;			//��ֹ�쳣���
	Init_Check_Status();
#ifdef DEBUG_Enter_Mode
	TRACE("Init switchoff submode Complete!\r\n");
#endif

#ifdef UV
	Reset_UV(); 					//�ر�UV
#endif
	REYBS_TIME=0;					//qz add 20180910,С�س����������رߴ�����0
	Open_Led(0,0,1);				//qz add 20181120,�������
}

void Init_Chargeing(u8 temp_sub_mode)
{
	mode.last_mode=mode.mode;		//qz add 20180814
	mode.last_sub_mode=mode.sub_mode;
    /******��ʼ����ʾ***********/
	/******��ʼ�����õ�ֵ********************/
	mode.mode = CHARGEING;
	mode.sub_mode=temp_sub_mode;
	mode.step=0;		//qz add 20180522
	mode.Info_Abort=0;	//qz add 20180615
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.status=0;						//qz add 20180522
	WriteWorkState();
	clr_ram();
	Init_Sweep_Pwm(PWM_CHARGE_MAX,PWM_CHARGE_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//�رշ��
	//qz add 20180615:����ڼ�ر�ǽ��\�ؼ�\�س����\�ٶȼ��
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
	
	Slam_Data.dipan_req_pre=DIPAN_REQ_SWEEP;			//qz modify 1-->0 20180522
	Slam_Data.no_msg=false;
	SLAM_DOCK=false;					//qz add 20180522
	DOCK_SWEEP=false;				//qz add 20180803	
	
	CHECK_STATUS_FLAG=false;			//��ֹ�쳣���		//qz add 20180913
	Init_Check_Status();				//qz add 20180522
	USART_Cmd(USART1, ENABLE);			//qz add 20180625,��Ϊ���֮ǰ�������巢���˹ػ�,��ص�����,���ڳ������´�
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 	  //qz add 20180625 

	Send_Voice(VOICE_CHARGING_START);	//������������ʼ���
#ifdef DEBUG_Enter_Mode
	TRACE("Init Charge Mode Complete!\r\n");
#endif

#ifdef UV
	Reset_UV(); 					//�ر�UV
#endif
	REYBS_TIME=0;					//qz add 20180910,С�س����������رߴ�����0
	Open_Led(0,0,1);				//qz add 20181120,�������
}
/******************************************************************
���ܣ�ִ�г��
******************************************************************/
void Do_Chargeing(void)
{
	//static uint8 piv_mode = 0;

	//��δ�ε�����ߡ�״̬��,30s�Ժ�,��״̬ȡ��
	if((dis_err_code==DIS_ERROR_DC_EXSIT)&(giv_sys_time-dis_err_time)>300000)
		{
			dis_err_code=0;
			error_code=0;
		}
	//���֡�δ�ε�����ߡ�״̬��,���DCֱ���ź���ʧ,���״̬ȡ��
	if((dis_err_code==DIS_ERROR_DC_EXSIT)&(!power.charge_dc))
		{
			dis_err_code=0;
			error_code=0;
		}

	//qz add 20180919
	switch (mode.step)
		{
			case 0:
				ChargeControl_Volt_My();
				if((mode.sub_mode==SWITCHOFF)&(power.switch_flag))
					{
						if(power.charge_dc)
							{
								Init_Chargeing(DC_CHARGING);
								return;
							}
						else if(power.charge_seat)
							{
								Init_Chargeing(SEAT_CHARGING);
								return;
							}
					}

				if(((mode.sub_mode==DC_CHARGING)|(mode.sub_mode==SEAT_CHARGING))&(!power.switch_flag))
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
					Init_Chargeing(SEAT_CHARGING);
				if(power.charge_dc)
					Init_Chargeing(DC_CHARGING);
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
				if(dis_err_code)
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

#if 0
	if((mode.sub_mode==SWITCHOFF))
		{
			read_key(&key1);
			if(Read_Button_Key(&key1)==3)
				{
					Slam_Data.dipan_req=DIPAN_REQ_BURNNING;
					if(power.charge_dc)		//qz add 20180814
						dc_nobat_run=true;
				}
		}
#endif
#endif
}
//==========================================================================================================





























