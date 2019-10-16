#include "AAA-Include.h"

void Init_Quit_Charging(u8 sweep_method)
{
	mode.last_mode=mode.mode;
	mode.last_sub_mode=mode.sub_mode;
	mode.mode=CEASE;
	mode.sub_mode=QUIT_CHARGING;

	Enable_wall();
	Enable_earth();
	enable_hwincept();
	Init_Action();
	Enable_Speed();
//	mode.step=0x01;
	mode.step=0x00;				//≥ı ºªØmode.step
	mode.time=giv_sys_time;
	mode.sweep_method=sweep_method;
	WriteWorkState();

	Sweep_Level_Set(STOP_ALL);
	//mode.Info_Abort=1;		//qz add 20180316:∆¡±Œµº∫Ω∞Â√¸¡Ó //qz mask 20180522
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
	static short tgt_angle=0,anti_tgt_angle;
	#if 0
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
			case 0:
				if(giv_sys_time-mode.time<2000)
					return;
				tgt_angle=Gyro_Data.yaw;
				anti_tgt_angle=Get_Reverse_Angle(tgt_angle);
				mode.step++;
			case 0x01:
				Speed=HIGH_MOVE_SPEED;
				if(do_action(4,40*CM_PLUS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 0x02:
#if 0
				Speed=TURN_SPEED;
				if(do_action(1,90*Angle_1))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 3:
				Speed=HIGH_MOVE_SPEED;
				if(do_action(3,50*CM_PLUS))
					{
						stop_rap();
						mode.step++;
					}
				break;
			case 4:
#endif
				Speed=TURN_SPEED;
				do_action(2,360*Angle_1);
				if(Judge_Yaw_Reach(anti_tgt_angle,TURN_ANGLE_BIOS))
					{
						stop_rap();
						switch(mode.sweep_method)
							{
								case 0:									
									Init_First_Sweep(1);
									break;
								case 1:
									Init_Right_YBS(1);
									break;
							}
					}
				break;
				
		}
}

