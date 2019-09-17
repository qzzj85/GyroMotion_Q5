#include "AAA-Include.h"

void Init_Quit_Charging(void)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	mode.mode=CEASE;
	mode.sub_mode=QUIT_CHARGING;
	
	disable_hwincept();
	Init_Action();
	Enable_Speed();
//	mode.step=0x01;
	mode.step=0x00;				//初始化mode.step
	WriteWorkState();

	Sweep_Level_Set(STOP_ALL);
	//mode.Info_Abort=1;		//qz add 20180316:屏蔽导航板命令 //qz mask 20180522
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.Info_Abort=0;

#ifdef DEBUG_Enter_Mode
	TRACE("Init Quit_Charge Complete!\r\n");
#endif

#ifdef UV
	Reset_UV();
#endif
	Close_Led();
}

void Do_Quit_Chargeing(void)
{
#if 0
	if((power.charge_dc==1))
		{	//qz add 20180522
			if(giv_sys_time-mode.time>600000)			//一分钟后没有拔掉直充，重新进入充电模式
				{
					stop_rap();
					Init_Chargeing();
				}
			return;
		}

	//qz add 20180522
	if((!power.charge_dc)&(!power.charge_seat)&(mode.step==0x00))		//一分钟之内拔掉直充，且无充电座，进入待机
		{
			stop_rap();
			Init_Cease();
			return;
		}
#endif

	#if 1
	//if((power.charge_seat==1)&&(mode.step==0x00))
	if(mode.step==0)
		{
			dis_err_code=0;

			mode.step++;
//			mode.Info_Abort=1;
		}
	#endif


	ACC_DEC_Curve();

	switch(mode.step)
		{

			case 0x01:
				Speed=1000;
				if(do_action(4,50*CM_PLUS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 0x02:
				//Init_Cease();
				//mode.status=1;		//qz add 20180827
				//Init_Commander();
				//Init_Cease();
				Init_First_Sweep();
				mode.Info_Abort=0;	//qz add 20180316 starting recevie com from NAVI
				break;
				
		}
}

