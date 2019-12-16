#include "AAA-include.h"


void Init_App_Ctrl(u8 move_dir)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	/******初始化设置的值********************/

	/*******初始化输出的控制***************/
	stop_rap(); //关闭左右轮
		
		
	Disable_earth();				//关闭地检
	Disable_wall(); 				//关闭墙检
	Enable_earth();
	Enable_wall();
	enable_hwincept();				//打开回充红外接收电源
	Enable_Speed(); 				//待机状态将速度检测打开，是为了防止进入CEASE时关闭速度检测会导致惯性脉冲无法计算。
	Sweep_Level_Set(sweep_level);
		
	/****设置机器的工作模式******/   
	mode.mode = MODE_CTRL; 
	mode.sub_mode=SUBMODE_APP_CTRL;			//QZ ADD
	mode.step=0x00; 				//qz add
	mode.status=0;					//qz add 20180625
	mode.time=giv_sys_time; 		//qz add 20180703
	mode.bump=0;
	mode.step_bp=0;
	mode.bump_flag=false;
		
	WriteWorkState();
	Disable_Free_Skid_Check();		//关闭万向轮检测


#ifdef DEBUG_Enter_Mode
	TRACE("Init REMOTE_MOVE Mode Complete!Prepare enter to Cease!\r\n");
#endif

	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
	Open_Led(1,0,0);
	motion1.app_key=move_dir;
}

void AppCtrl_Bump_Action(void)
{
	Read_Remote_Bump(0);

	if(!mode.bump)
		return;
	switch(mode.step_bp)
		{
			case 0:
				Speed=HIGH_MOVE_SPEED;
				if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
					{
						stop_rap();
						mode.step_bp++;
					}
				break;
			case 1:
				mode.bump=0;
				mode.step_bp=0;
				mode.step=0;
				mode.bump_flag=0;
				break;
				
		}
}

void Do_App_Ctrl(void)
{
	ACC_DEC_Curve();
	Remote_Bump_Action();

	if(mode.bump)
		return;

	if(mode.sub_mode!=SUBMODE_APP_CTRL)
		return;
	switch(motion1.app_key)
		{
			case APP_KEY_FORWORD:
				switch(mode.step)
					{
						case 0:
							motion1.tgt_yaw=Gyro_Data.yaw;
							mode.step++;
						break;
						case 1:
							Speed=FAST_MOVE_SPEED;
							do_action_my(3,FARAWAY*CM_PLUS,motion1.tgt_yaw);
						break;
					}
				break;
			case APP_KEY_BACK:
				switch(mode.step)
					{
						case 0:
							motion1.tgt_yaw=Gyro_Data.yaw;
							mode.step++;
						break;
						case 1:
							Speed=HIGH_MOVE_SPEED;
							do_action_my(4,FARAWAY*CM_PLUS,motion1.tgt_yaw);
						break;
					}
				break;
			case APP_KEY_LEFT:
				switch(mode.step)
					{
						case 0:
							motion1.tgt_yaw=Gyro_Data.yaw;
							mode.step++;
						break;
						case 1:
							Speed=HIGH_MOVE_SPEED;
							do_action(1,360*Angle_1);
						break;
					}
				break;
			case APP_KEY_RIGHT:
				switch(mode.step)
					{
						case 0:
							motion1.tgt_yaw=Gyro_Data.yaw;
							mode.step++;
						break;
						case 1:
							Speed=HIGH_MOVE_SPEED;
							do_action(2,360*Angle_1);
						break;
					}
				break;
			case APP_KEY_STOP:
				stop_rap();
				if(mode.last_sub_mode==SUBMODE_PAUSESWEEP)
					Init_PauseSweep();
				else
					Init_Cease();
			break;
		}
}

