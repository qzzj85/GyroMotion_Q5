#include "AAA-include.h"

REMOTE_INFO	remote_info;

void Init_Remote_Info(void)
{
	remote_info.effect=false;
	remote_info.effect_time=giv_sys_time;
	remote_info.remote_key=REMOTE_KEY_NONE;
	remote_info.rece_ir=RECE_NONE;
}

void Clr_Remote_Info(void)
{
	if((remote_info.effect)&(giv_sys_time-remote_info.effect_time<1000))
		{
			//remote_info.effect=0;
			Reset_HW_Power_OFF();
		}
	else
		Set_HW_Power_ON();
}

void Remote_Handle(void)
{
	//机器处于CEASE的其他模式,退出

	//Clr_Remote_Info();

	if(!remote_info.effect)
		return;

	remote_info.effect=false;
	
//	if(((mode.mode==CEASE)&(mode.sub_mode!=CEASE)&(mode.sub_mode!=SLEEP)&(mode.sub_mode!=SELF_TEST)))	
	//	return;
	
	//机器处于船型开关未打开,退出
	if((mode.mode==CHARGEING)&(mode.sub_mode==SWITCHOFF))
		return;

	
	switch (remote_info.remote_key)
		{
			///////方向性按键///////
			///////方向性按键///////
			case REMOTE_KEY_FORWORD:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				/////机器处于充电状态且未进入时间预约设置模式，退出
				if((mode.mode==CHARGEING))
					return;
				if((mode.mode==CEASE)&((mode.sub_mode==CEASE)|(mode.sub_mode==SUBMODE_PAUSESWEEP)))
					Init_Remote_Move();
				else if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))
					Init_Cease();
				//remote_info.key_time=giv_sys_time;
				break;
			case REMOTE_KEY_LEFT:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				
				/////机器处于充电状态且未进入时间预约设置模式，退出
				if((mode.mode==CHARGEING))
					return;
				if((mode.mode==CEASE)&((mode.sub_mode==CEASE)|(mode.sub_mode==SUBMODE_PAUSESWEEP)))
					Init_Remote_Move();
				else if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))
					Init_Cease();
				break;
			case REMOTE_KEY_RIGHT:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				/////机器处于充电状态且未进入时间预约设置模式，退出
				if((mode.mode==CHARGEING))
					return;
				if((mode.mode==CEASE)&((mode.sub_mode==CEASE)|(mode.sub_mode==SUBMODE_PAUSESWEEP)))
					Init_Remote_Move();
				else if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))
					Init_Cease();
				break;
			case REMOTE_KEY_BACK:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				/////机器处于充电状态且未进入时间预约设置模式，退出
				if((mode.mode==CHARGEING))
					return;
				if((mode.mode==CEASE)&((mode.sub_mode==CEASE)|(mode.sub_mode==SUBMODE_PAUSESWEEP)))
					Init_Remote_Move();
				else if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))
					Init_Cease();
				break;

			////////模式性按键///////
			////////模式性按键///////
			case REMOTE_KEY_DOCK:
				motion1.force_dock=true;
				switch(mode.mode)
					{
						case CEASE:
							switch(mode.sub_mode)
								{
									case CEASE:
									case SUBMODE_PAUSESWEEP:
										
#ifdef TUYA_WIFI
										mcu_dp_enum_update(5,2);  //状态上报为工作模式  
										wifi_uart_write_stream_init(0,0);// 初始化地图参数	地图	0  
										DelayMs(1);
										stream_open();	// 申请传输  WIFI_STREAM_ENABLE
										DelayMs(1);
										stream_start(00);// 开始传输
#endif
										Init_Docking();
									break;
									case SLEEP:
										Init_Cease();
									break;
									default:
										break;
								}
							break;
						case SWEEP:
						case SHIFT:
						case PASS2INIT:
						case EXIT:
							stop_rap();
							Send_Voice(VOICE_DOCK_START);
							Sweep_Level_Set(SWEEP_LEVEL_DOCK);
							Force_Dock();
							break;
						case YBS:
						case SPOT:
							stop_rap();
							Send_Voice(VOICE_DOCK_START);
							Sweep_Level_Set(SWEEP_LEVEL_DOCK);
							Init_Docking();
							break;
						default:
							break;
					}				
				
#ifdef REMOTE_DEBUG
				TRACE("Remote Dock key,request dock!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				break;
			case REMOTE_KEY_GUIHUA:
				//机器处于工作状态，退出
				if(mode.status)
					break;				
#ifdef REMOTE_DEBUG
				TRACE("Remote GUIHUA key,request guihua!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				switch(mode.mode)
					{
						case CEASE:
							switch(mode.sub_mode)
								{
									case CEASE:
									case SUBMODE_PAUSESWEEP:
#ifdef TUYA_WIFI
										mcu_dp_enum_update(5,2);  //状态上报为工作模式  
										wifi_uart_write_stream_init(0,0);// 初始化地图参数	地图	0  
										DelayMs(1);
										stream_open();	// 申请传输  WIFI_STREAM_ENABLE
										DelayMs(1);
										stream_start(00);// 开始传输
#endif
										Init_First_Sweep(0);
										break;
									case SLEEP:
										Init_Cease();
										break;
									default:
										break;
										
								}
							break;
						case CHARGEING:
							switch(mode.sub_mode)
								{
									case DC_CHARGING:
										Send_Voice(VOICE_ERROR_DC_EXIST);
										break;
									case SEAT_CHARGING:
										Init_Quit_Charging(SWEEP_METHOD_GUIHUA);
										break;
									default:
										break;
								}
							break;
						default:
							break;
					}
				break;
			case REMOTE_KEY_YBS:
				switch(mode.mode)
					{
						case CEASE:
							switch(mode.sub_mode)
								{
									case CEASE:
									case SUBMODE_PAUSESWEEP:
#ifdef TUYA_WIFI
										mcu_dp_enum_update(5,2);  //状态上报为工作模式  
										wifi_uart_write_stream_init(0,0);// 初始化地图参数	地图	0  
										DelayMs(1);
										stream_open();	// 申请传输  WIFI_STREAM_ENABLE
										DelayMs(1);
										stream_start(00);// 开始传输
#endif
										Init_Right_YBS(1);
									break;
									case SLEEP:
										Init_Cease();
										break;
									default:
										break;
								}
							break;
						case CHARGEING:
							switch(mode.sub_mode)
								{
									case SWITCHOFF:
										break;
									case DC_CHARGING:
										Send_Voice(VOICE_ERROR_DC_EXIST);
										break;
									case SEAT_CHARGING:
										Init_Quit_Charging(SWEEP_METHOD_YBS);
										break;
									default:
										break;
								}
							break;
						default:
							break;
					}
				break;
			case REMOTE_KEY_START:
#ifdef REMOTE_DEBUG
				TRACE("Remote GUIHUA key,request guihua!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				switch(mode.mode)
					{
						case CEASE:
							switch(mode.sub_mode)
								{
									case CEASE:
#ifdef TUYA_WIFI
										mcu_dp_enum_update(5,2);  //状态上报为工作模式  
										wifi_uart_write_stream_init(0,0);// 初始化地图参数	地图	0  
										DelayMs(1);
										stream_open();	// 申请传输  WIFI_STREAM_ENABLE
										DelayMs(1);
										stream_start(00);// 开始传输
#endif
										Init_First_Sweep(0);
										break;
									case SLEEP:
									case ERR:
										Init_Cease();
										break;
									case SUBMODE_PAUSESWEEP:
										if(mode.step<1)
											mode.step=1;
										break;
									case SUBMODE_VIRTUAL_SLEEP:
										Send_Voice(VOICE_VOLUME_4);
										Init_Cease();
										break;
									default:
										break;
										
								}
							break;
						case CHARGEING:
							switch(mode.sub_mode)
								{
									case DC_CHARGING:
										Send_Voice(VOICE_ERROR_DC_EXIST);
										break;
									case SEAT_CHARGING:
										Init_Quit_Charging(SWEEP_METHOD_GUIHUA);
										break;
									default:
										break;
								}
							break;
						case SWEEP:
						case SHIFT:
						case PASS2INIT:
						case EXIT:
						case YBS:
							stop_rap();
							Save_Pause_Data();
							Send_Voice(VOICE_PAUSE_SWEEP);
							Send_Voice(VOICE_VOLUME_2);
							Init_PauseSweep();
							break;
						case DOCKING:
							stop_rap();
							Save_Pause_Data();							
							Send_Voice(VOICE_PAUSE_SWEEP);
							Send_Voice(VOICE_VOLUME_2);
							Init_PauseSweep();
							break;
						default:
							break;
					}
				break;
			case REMOTE_KEY_FAN:
				if(sweep_level==SWEEP_LEVEL_STANDARD)
					{
						sweep_level=SWEEP_LEVEL_FORCE;
					}
				else
					{
						sweep_level=SWEEP_LEVEL_STANDARD;
					}
				switch(mode.mode)
					{
						case CEASE:
							if(mode.sub_mode==CEASE)
								{
#ifdef TUYA_WIFI
									mcu_dp_enum_update(5,2);  //状态上报为工作模式  
									wifi_uart_write_stream_init(0,0);// 初始化地图参数	地图	0  
									DelayMs(1);
									stream_open();	// 申请传输  WIFI_STREAM_ENABLE
									DelayMs(1);
									stream_start(00);// 开始传输
#endif
									Init_First_Sweep(0);
								}
							break;
						case SWEEP:
						case YBS:
						case EXIT:
						case SHIFT:
						case PASS2INIT:
							Sweep_Level_Set(sweep_level);
							break;
						case CHARGEING:
							if(mode.sub_mode==SEAT_CHARGING)
								{
									Init_Quit_Charging(SWEEP_METHOD_GUIHUA);
								}
							else if(mode.sub_mode==DC_CHARGING)
								{
									Send_Voice(VOICE_ERROR_DC_EXIST);
								}
							break;
						default:
							break;
					}
				break;
	}
}

void Init_Remote_Move(void)
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
	mode.sub_mode=SUBMODE_REMOTE_CTRL;			//QZ ADD
	mode.step=0x00; 				//qz add
	mode.status=0;					//qz add 20180625
	mode.time=giv_sys_time; 		//qz add 20180703
	mode.init_mode_time=giv_sys_time;	//qz add 20180814
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
}

u8 Read_Remote_Bump(u8 ir_enable)
{
	u32 data1=0;
#ifdef CLIFF_ENABLE			//ZDK屏蔽
	data1=Read_Cliff();
	if(data1)
		{
			if((mode.bump>BUMP_ALL_CLIFF)|(mode.bump==0))
				{
					stop_rap();
					mode.bump=data1;
					mode.step_bp=0;
					
#ifdef EARTH_IN_TIM2
					enable_pwm(L_BACK,1200);
					enable_pwm(R_BACK,1200);
					l_rap.ori=BACK;
					r_rap.ori=BACK;
#endif
				}
			return data1;
		}
#endif	 

	data1=Parse_BumpValue();
	switch (data1)
	{
		case BUMP_ONLY_LEFT:						 //左碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.l_bump_flag=true;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_LEFT;
		 case BUMP_ONLY_LEFTMID:					 //左中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_LEFTMID;
		 case BUMP_LEFT_MID:						 //左左中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_LEFT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_LEFT_MID;
		 case BUMP_ONLY_RIGHT:						 //右碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_RIGHT;
		 case BUMP_ONLY_RIGHTMID:					 //右中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_ONLY_RIGHTMID;
		 case BUMP_RIGHT_MID:						 //右右中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_RIGHT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_RIGHT_MID;
		 case BUMP_MID: 							 //中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
				 }
			 return BUMP_MID;
	}

	if((w_m.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0)|(mode.bump==BUMP_SEAT)) 	//中墙检靠近墙
				{
					stop_rap();
					mode.bump=9;//W_M;
					mode.step_bp=0;
					mode.bump_time=giv_sys_time;
				}
			return 9;//W_M;
		}
	
	if((w_lm.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0))		 //左中墙检靠近墙
				{
					stop_rap();
					mode.bump=LM_WALL_BUMP;//W_LM;
					mode.step_bp=0;
				}
			return LM_WALL_BUMP;//W_LM;
		}

	if((w_rm.sign == NEAR)&(ir_enable))
		{
			if((mode.bump == 0))		 //左中墙检靠近墙
				{
					stop_rap();
					mode.bump=RM_WALL_BUMP;//W_LM;
					mode.step_bp=0;
				}
			return RM_WALL_BUMP;//W_LM;
		}

#ifdef FREE_SKID_CHECK
	if(Check_Free_Sikd())
		{
			Slam_Data.skid_flag=1;
#ifdef SKID_REPORT_TIME
			Slam_Data.skid_report_time=giv_sys_time;
#endif
#ifdef FREE_SKID_ACTION
			stop_rap();
			mode.bump=0xff;
			mode.step_bp=0;
			mode.Info_Abort=1;
#endif
		}
#endif
	return 0;		
}
void Remote_Bump_Action(void)
{
	//u8 m;
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
void Do_Remote_Move(void)
{
	if(giv_sys_time-remote_info.key_time>3000)
		{
			stop_rap();
			if(mode.last_sub_mode==SUBMODE_PAUSESWEEP)
				Init_PauseSweep();
			else
				Init_Cease();
		}
	
	ACC_DEC_Curve();
	Remote_Bump_Action();

	if(mode.bump)
		return;

	if(mode.sub_mode!=SUBMODE_REMOTE_CTRL)
		return;
	switch(remote_info.remote_key)
		{
			case REMOTE_KEY_FORWORD:
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
			case REMOTE_KEY_BACK:
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
			case REMOTE_KEY_LEFT:
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
			case REMOTE_KEY_RIGHT:
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
		}
}
