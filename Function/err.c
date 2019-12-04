////////////////////////ͷ�ļ�////////////////////////////////////	
#include "AAA-include.h"
////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
static u8 cliff_step=0;

///////////////////////ȫ�ֺ���////////////////////////////////////	  
void Init_Err(void); 
void Do_Err(void);
///////////////////////˽�к���////////////////////////////////////	
///////////////////////����ʵ��////////////////////////////////////
/******************************************************************
���ܣ���ʼ��������
******************************************************************/
void Init_Err(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
    /******��ʼ����ʾ***********/
	/******��ʼ�����õ�ֵ********************/
	clr_ram();
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	stop_rap();						//ֹͣ����
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//�رշ��
	//Disable_earth();				//�رյؼ�
	Enable_earth();
	Disable_wall();					//�ر�ǽ��
//	Enable_wall();
	disable_hwincept();				//��ֹ������յ�Դ
	enable_hwincept();
	Reset_Speed_Send();  			//�ر��ٶȼ��
	disable_pwm(CHARGE_PWM);		//��ֹ���
	mode.mode = CEASE;				//��ģʽΪCEASE
	mode.sub_mode=ERR;				//��ģʽΪERR
	mode.Info_Abort =0;				//����SLAMͨ��
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.status=0;					//�ǹ���״̬
	mode.time=giv_sys_time;
	WriteWorkState();

	CHECK_STATUS_FLAG=false;		//��������쳣���

#ifdef DEBUG_Enter_Mode
	TRACE("Init Err Mode Complete!\r\n");
#endif

#if 1
	dc_nobat_run=false;
#endif

#ifdef UV
	Reset_UV();
#endif
	REYBS_TIME=0;					//qz add 20180910,С�س����������رߴ�����0
	//Open_Led(0,30000,2);			//qz add 20181120,��ƿ���3s
	//Open_Led(0,0,0);
	Open_Led(0,error_code,3);
#ifdef NEW_VOICE_IC
	Send_Voice(VOICE_ERROR_WARNING);
#endif
	cliff_step=0;
}
/******************************************************************
���ܣ�ִ�г��
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
 ///ң������������		     
//  if(Sweep_YaoKong_Manage() == 1)
//		{
//				giv_sys_err = 0;
//				return;
//		}
  ///������������������е�����һ��������
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
  //���ֹ�������������������ֻ������ֵ�������ʱ����������������������������󣬻��������򱨾���
  //���Ǳ��������쳣��һ�����ŵأ������Ͻ������״̬�������޷�֪���ղŷ�����ʲô�������Ͳ�����
  //���԰ѵ������յ�״̬���������Σ������Զ��ظ�
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
