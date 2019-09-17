#include "AAA_include.h"
#include "stm32f10x.h"

#define WAIT_TIME	3000000	//5min

static u32 work_time;


//Pre_Action״̬������ģʽ0x00��standby-0x00���û�ģʽΪApp_Data.user_mode
void Init_Pre_Action(void)
{
	stop_rap();

	mode.mode=PRE_ACTION;
	mode.step=0x00;
	Dis_Guihua=false;
	Dis_Zhongdian=false;
	Dis_YBS=false;
	

	ReadRealTime();
	work_time=giv_sys_time;
	
}



void Do_Pre_Action(void)			
{
	if(giv_sys_time-work_time>WAIT_TIME)	//����5min����Pre_Actionģʽ������SLEEP״̬
		{
			stop_rap();
			Init_Sleep();
			return;
		}

	stop_rap();
	return;
		
}


