#include "AAA-include.h"
////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////	  
void Init_Switchoff(void); 
///////////////////////˽�к���////////////////////////////////////	
///////////////////////����ʵ��////////////////////////////////////
/******************************************************************
���ܣ���ʼ��������
******************************************************************/
void Init_Switchoff(void)
{

    /******��ʼ����ʾ***********/
	/******��ʼ�����õ�ֵ********************/
	clr_ram();

	stop_rap();						//ֹͣ����
	Sweep_Level_Set(SWEEP_LEVEL_STOP);		//�رշ��
	Disable_earth();
	Disable_wall();
	disable_hwincept();//��ֹ������յ�Դ
	Disable_Speed();  //��ֹ����ƹⷢ��
	power.pwm=0;
	mode.mode = MODE_CHARGE;
	mode.sub_mode=SUBMODE_CHARGE_SWITCHOFF;
	mode.Info_Abort =0;			//��ֹSLAMͨ��
	mode.step=0xff;			//��ֹ�ӳ��������
	mode.status=0;			//qz add 20180422
	mode.time=giv_sys_time;
	WriteWorkState();
	
	power.pwm = 0;
	power.step=0;
	disable_pwm(CHARGE_PWM);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);
	Init_Charge_Data();					//qz add 20180522

	
	CHECK_STATUS_FLAG=false;		//��ֹ�쳣���
}
