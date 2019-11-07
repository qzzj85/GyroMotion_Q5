////////////////////////头文件////////////////////////////////////

#include "AAA-Include.h"
////////////////////////私有定义//////////////////////////////////
#define WAIT_TIME 3000000
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
u8 piv_yaokongstate = 0;	   //在设置时的设置状态,0没有设置，1设置
                               //当前时间，2设置预约时间

u8 piv_dis_hour;			   //显示的小时数

u8 piv_dis_minute;		   //显示的分钟数

u8 piv_dis_week;			   //显示的小时数

u8 piv_dis_preengage;	   //显示的预约天数

u8 piv_set_content;         //设置的内容	0为没有设置，1为设置天数、2为设置小时、3为设置分钟
                               //4为设置星期


///////////////////////函数实体////////////////////////////////////
/*****************************************************************
功能：在待机时的动作
*****************************************************************/

void Cease_Bump_Action(void)
                               	{
     u8 m=Read_Only_Collide();
	 switch(mode.bump)
	 	{
	 		case BUMP_ONLY_LEFT:
		 		switch(mode.step_bp)
		 			{
		 				case 0:
							Speed=BUMP_BACK_SPEED;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 1:
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							mode.step=0;
		 			}
				break;
			case BUMP_ONLY_RIGHT:
				switch(mode.step_bp)
					{
						case 0:
							Speed=BUMP_BACK_SPEED;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 1:
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							mode.step=0;
					}
				break;
			case BUMP_MID:
				switch(mode.step_bp)
					{
						case 0:
							Speed=BUMP_BACK_SPEED;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 1:
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							mode.step=0;
					}
				break;
			default:
				mode.bump=0;
				mode.step_bp=0;
				mode.bump_flag=false;
				break;
	 	}
}

void Do_Cease(void)
{
    /**************如果有遥控器按键*/
#ifdef DC_NOBAT_RUN
  if(((power.charge_dc)|(power.charge_seat))&(!dc_nobat_run))
#else
  if(power.charge_dc|power.charge_seat)
#endif
		{
			 //mode.last_mode=mode.mode;			//qz add 20180201
			 //Init_Chargeing(CHARGEING);

			 //qz add 20180901
			 if(power.charge_seat)
			 	Init_Chargeing(SEAT_CHARGING);
			 if(power.charge_dc)
			 	Init_Chargeing(DC_CHARGING);
			 //qz add end
			 
			 return;
		}
	/////////机器的预约时间到，机器进入扫地模式/////////////////////

	//计划是在charge和sleep下进行预约时间检查，输出一个flag，然后在charge和cease下检查这个flag
	#if 0
	if(Check_PreengageTime())
			{
//				 if(Slam_Data.tick_flag)			//判断导航板是否有心跳处于工作状态	//qz mask 20180420
				 	{
						//qz add 20180420
						Slam_Data.dipan_req_pre=DIPAN_REQ_PREEN;
						//qz add end
						Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
				 	}
				 return;
			}
	#endif
	//系统有错误
	if(giv_sys_err != 0)
			{
				Init_Err();
				return ;
			}
#ifdef SLEEP_SUBMODE
    /**********如果机器在3分钟内没有操作,机器进入睡眠模式**********/
	if(((giv_sys_time-mode.time)>10000*60)&(!mode.factory))			//qz mask 20181110
	//if(((giv_sys_time-mode.sleep_time)>1800000)&(!mode.factory))		//qz add 20181110
		{
			//Init_Sleep();
			//Init_ShutDown();
			Init_VirtualSleep();
			return;
		}
#endif
	clr_all_hw_effect();
	ACC_DEC_Curve();
//qz mask 20181215
//	find_home=ReadHwSign_My();			//qz mask 20181215
#ifdef REPEAT_DOCK	//qz add 20180901
	if((giv_sys_time-mode.time>40000)&(mode.last_mode==CHARGEING)&(mode.last_sub_mode==SEAT_CHARGING)&(!dc_nobat_run)&(mode.step==0))
		{
			Enable_Speed();
			stop_rap();
			mode.step++;
			mode.Info_Abort=1;
		}

#if 0
	u8 abnormal=Read_Protect();
	Action_Protect_My(abnormal);
	if(mode.abnormity)
		return;

	Cease_Bump_Action();		
	if(mode.bump)
		return;
#endif

	switch(mode.step)
		{
			case 0:
				break;
			case 1:
				Speed=2000;
				if(do_action(4,15*CM_PLUS))
					{
						stop_rap();
						mode.step++;
						mode.Info_Abort=0;
					}
				break;
			case 2:
				Slam_Data.dipan_req_pre=DIPAN_REQ_DOCK;
				Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
				mode.step++;
				break;
			case 3:
				Send_Voice(VOICE_DOCK_START);
				motion1.start_seat=true;
				Init_Docking();
				break;
		}
#endif
}







/******************************************************************
在待机状态的显示初始化
******************************************************************/
void Init_Cease(void)
{ 
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	
	/******初始化设置的值********************/
	piv_set_content = 0;			//开始设置为没有设置
	piv_yaokongstate = 0;

	/*******初始化输出的控制***************/
	stop_rap(); //关闭左右轮
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);
	//Disable_earth();				//关闭地检
	Disable_wall();					//关闭墙检
	Enable_earth();
	//Enable_wall();
	enable_hwincept();				//打开回充红外接收电源
	Enable_Speed();					//待机状态将速度检测打开，是为了防止进入CEASE时关闭速度检测会导致惯性脉冲无法计算。
		
	/****设置机器的工作模式******/   
	mode.mode = CEASE; 
	mode.Info_Abort=0;				//打开SLAM通信，qz add 20180418
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.sub_mode=CEASE;			//QZ ADD
	mode.step=0x00;					//qz add
	mode.status=0;					//qz add 20180625
	mode.time=giv_sys_time;			//qz add 20180703
	mode.init_mode_time=giv_sys_time;	//qz add 20180814
	motion1.start_seat=false;
	motion1.force_dock=false;
		
	WriteWorkState();
	Disable_Free_Skid_Check();		//关闭万向轮检测
	//初始化检测的条件
	CHECK_STATUS_FLAG=true;			//使能异常检测
	Init_Check_Status();//qz add 20180425

	Slam_Data.dipan_req_pre=1;		//qz mask 20180522：默认为规划清扫

#ifdef DEBUG_Enter_Mode
	TRACE("Init Cease Mode Complete!Prepare enter to Cease!\r\n");
#endif
	SLAM_DOCK=false;
	DOCK_SWEEP=false;				//qz add 20180803

#ifdef UV
	Reset_UV();						//关闭UV
#endif 
	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
	Open_Led(1,0,0);
	
}

/*****************************************************************
功能：待机遥控器处理程序
******************************************************************/
#if 0
void Cease_YaoKong_Manage(void)
{
	if(gsv_yaokong.sign == 1) /////有遥控器按键按下
			{
				gsv_yaokong.sign = 0; 
	//Enable_Beep();
				switch(piv_yaokongstate)
						{
								//在以前没有按键按下时的情况
								case 0:
									Cease_No_Key();
								break;

								//原来按过设定时间键
							case 1:
									Cease_SetTime();
									break;

							//原来按过设定预约键
							case 2:
									Cease_Timing();
									break;
							default :
							piv_yaokongstate = 0;

						}
				giv_cease_time = giv_sys_time;     
			}
}
#endif
/*******************************************************
功能：在没有预先按下按键时的处理程序
*******************************************************/
#if 0
void Cease_No_Key(void)
{
u32 liv_time;
	
    switch(	gsv_yaokong.key)
					{
						//////按下电源键或者取消键
					case YAOKONG_ESC:
							liv_time = giv_sys_time;
							////////等待电源按键弹起//////////////////
							do
								{
										A7105_rx();		//
										Beep_Driver();	
										IWDG_ReloadCounter();			   
										if((giv_sys_time - liv_time) > 30000)  //超过5秒
												{
														PreengageDate = 0;        //清除原来的预约设置	  
														PreengageTime = 0;
														WritePreengage();	
														Enable_Beep();
														break;			                         
												}
								}
							while(( gsv_yaokong.key == YAOKONG_ESC) &&(gsv_yaokong.effect != 0)) ;	
							//??????在A7105_rx()中，500ms内没有反应.effect会清零，撑不到3秒,且在EXIT0中断函数中，如果上次键值与这次相同不会将.effect置1 QZ
							////判断电源键是否按下时间超过3秒//////
							if((giv_sys_time - liv_time) < 30000)
									{
											Init_Sleep();        //进入睡眠模式
									}
							break;	
						
						///////按下预约按键进入预约设置	 
							case YAOKONG_TIMINE_MON:
								Cease_Lode_Preengage();//将预约内容装入临时变量中
								piv_set_content = 1;
								piv_yaokongstate = 2;
								break;

						///////按下时间按键进入时间设定
							case YAOKONG_SETTIME_TUE:	
								Cease_Lode_Time();
								piv_set_content = 4;
								piv_yaokongstate = 1;
								break;

						//////按下uv键将uv指示取反   		//		沿边线   延边
							case YAOKONG_UV_WED:
					//			Turn_UV();
							
									Enable_Beep_number(4);
									Init_Right_YBS();
	
								break;

						/////按下定点清扫进入定点清扫模式		//自动清扫??  QZ
							case YAOKONG_AUTO_THU:
								Init_Sweep();;
								break;

						////按下自动清扫键///////////////		//定点清扫??  QZ
							case YAOKONG_SPOT_FRI:
								Init_Spot();
								break;

							case YAOKONG_V_SAT:
								Change_Speed();
								break;

							case YAOKONG_DOCKING_SUN:  
								Init_Docking();
								break;
							
							case YAOKONG_LEFT:
							case YAOKONG_RIGHT:
							case YAOKONG_FRONT:
							case YAOKONG_BACK:
								Init_Tele();
								break;
							
							
							
						/*
							case YAOKONG_OK:
							if( gsv_work_mode.work_mode == SPOT)
							{
									Init_Spot();
							}
							else
							{
									Init_Sweep();
							}
								break;	
						*/
							
							
					}
}
#endif
/*************************************************************
装载时间
*************************************************************/
void Cease_Lode_Time(void)
{	
    piv_dis_preengage = (u8)PreengageDate;	//预约内容	
    piv_dis_week = (u8)((Rtc_time / 86400)) ;
    piv_dis_minute = (u8)((Rtc_time % 3600) / 60);
    piv_dis_hour = (u8)((Rtc_time % 86400) / 3600) ;

}
/*************************************************************
保存时间
*************************************************************/
void  Cease_Save_Time(void)
{
    Rtc_time = 	(piv_dis_hour*3600) + (piv_dis_minute*60) + ((piv_dis_week)*86400);
	WriteRtcTime();
}
/*************************************************************
装载时间
*************************************************************/
void Cease_Lode_Preengage(void)
{	
    piv_dis_preengage = (u8)PreengageDate;	//预约内容	
    piv_dis_week 			= (u8)(Rtc_time / 86400 );
    piv_dis_minute 		= (u8)(PreengageTime % 256);
    piv_dis_hour 			= (u8)(PreengageTime / 256);

}
/*************************************************************
保存时间
*************************************************************/
void  Cease_Save_Preengage(void)
{
    PreengageDate = piv_dis_preengage ;
		PreengageTime = piv_dis_minute + (piv_dis_hour<<8);
		WritePreenData();
}

/*************************************************************
在设置定时预约操作
*************************************************************/
#if 0
void Cease_Timing(void)
{	 
u32 liv_time;	 //按键的开始按下时间
u8 liv_number;	 
u8 liv_key_down_time;  //按键的临时按下时间
    switch(	gsv_yaokong.key)
	{
		//////按下电源键或者取消键
	    case YAOKONG_ESC:
			 liv_time = giv_sys_time;
		    ////////等待电源按键弹起//////////////////
		    do
			{
			    A7105_rx();		//	
				Beep_Driver();	 
				IWDG_ReloadCounter(); 
		        if((giv_sys_time - liv_time) > 30000)  //超过5秒
			    {
			        PreengageDate = 0;        //清除原来的预约设置	
				    PreengageTime = 0;
				    WritePreengage();         //
					Enable_Beep();
			    }
			}
			while(( gsv_yaokong.key == YAOKONG_ESC) &&(gsv_yaokong.effect != 0)) ;

		    ////判断电源键是否按下时间超过5秒//////
			Init_Cease();
		    break;	
		
		///////按下预约按键进入预约设置	 
	    case YAOKONG_V_SAT:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x01;	//相当于最低位取反 QZ  
			}
		    break;

		///////按下时间按键进入时间设定
//	    case YAOKONG_FRONT:	
//		    if(	piv_set_content == 1)
//			{
//			    piv_dis_preengage ^= 0x02;    
//			}
//		    break;

		//////按下uv键将uv指示取反
//	    case YAOKONG_BACK:
//		    if(	piv_set_content == 1)
//			{
//			    piv_dis_preengage ^= 0x04;  
//			}
//		    break;

		/////按下定点清扫进入定点清扫模式
	    case YAOKONG_SETTIME_TUE:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x08;   
			}
		    break;

		////按下自动清扫键///////////////
	    case YAOKONG_AUTO_THU:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x10;   
			}
		    break;

	    case YAOKONG_SPOT_FRI:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x20; 
			}
		    break;

	    case YAOKONG_DOCKING_SUN:  
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x40; 
			}
		    break;

	    case YAOKONG_UV_WED:	   
		    if( piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x80;    
			}
		    break;

	    case YAOKONG_LEFT:		 
		    piv_set_content--; 
		    if(piv_set_content == 0)
			{
			   piv_set_content = 3;
			}
			break;
	    case YAOKONG_RIGHT:	  
		    piv_set_content++; 
		    if(piv_set_content == 4)
			{
			   piv_set_content = 1;
			}
			break;
	    case YAOKONG_FRONT:
			if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x02;    
			}
			else
			{
			    liv_time = giv_sys_time; //保存开始按下按键的时间
				liv_number = 0;
			    do
				{
				    A7105_rx();		// 读取按键	  
					Beep_Driver();
					liv_key_down_time = (u8)((giv_sys_time - liv_time)/10000) ;
				    //按键时间大于3秒并且到加秒时间
					if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
					{
					   liv_number =  liv_key_down_time;
					   Cease_Big_Add_Time();
					}			 
					Cease_display_set();
					AutoDisplay();
					IWDG_ReloadCounter();
				}
				while(( gsv_yaokong.key == YAOKONG_FRONT) &&(gsv_yaokong.effect != 0)) ;	
			    Cease_Add_Time();
			}
			break;
	    case YAOKONG_BACK:
			if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x04;  
			}
			else
			{
			    liv_time = giv_sys_time; //保存开始按下按键的时间
				liv_number = 0;
			    do
				{
				    A7105_rx();		// 读取按键	  
					Beep_Driver();
					liv_key_down_time = (u8)((giv_sys_time - liv_time)/10000) ;
				    //按键时间大于3秒并且到加秒时间
					if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
					{
					   liv_number =  liv_key_down_time;
					   Cease_Big_Subb_Time();
					}			 
					Cease_display_set();
					AutoDisplay();
					IWDG_ReloadCounter();
				}
				while(( gsv_yaokong.key == YAOKONG_BACK) &&(gsv_yaokong.effect != 0)) ;
			    Cease_Subb_Time();
			}
			break;

	    case YAOKONG_OK:
		    Cease_Save_Preengage();	 
			Init_Cease();
		    break;
	}

}
#endif

/*************************************************************
在设置时间操作
*************************************************************/
#if 0
void Cease_SetTime(void)
{
u32 liv_time;	 //按键的开始按下时间
u8 liv_number;	 
u8 liv_key_down_time;  //按键的临时按下时间
    switch(	gsv_yaokong.key)
	{
		//////按下电源键或者取消键
	    case YAOKONG_ESC:				   
			Init_Cease();
		    break;
		///////按下预约按键进入预约设置	 
	    case YAOKONG_V_SAT:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 0;   
			}
		    break;

		///////按下时间按键进入时间设定
	    case YAOKONG_FRONT:	
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 1;  
			}
			else
			{
				liv_time = giv_sys_time; //保存开始按下按键的时间
				liv_number = 0;
			    do
				{
				    A7105_rx();		// 读取按键	
					Beep_Driver();
					liv_key_down_time = (u8)((giv_sys_time - liv_time)/10000) ;
				    //按键时间大于3秒并且到加秒时间
					if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
					{
					   liv_number =  liv_key_down_time;
					   Cease_Big_Add_Time();
					}			 
					Cease_display_set();
					AutoDisplay();
					IWDG_ReloadCounter();
				}
				while(( gsv_yaokong.key == YAOKONG_FRONT) &&(gsv_yaokong.effect != 0)) ;	
			    Cease_Add_Time();
			}

		    break;

		//////按下uv键将uv指示取反
	    case YAOKONG_BACK:
				if(	piv_set_content == 4)
						{
								piv_dis_week = 2;  
						}
				else
						{
							liv_time = giv_sys_time; //保存开始按下按键的时间
							liv_number = 0;
								do
							{
									A7105_rx();		// 读取按键	 
								Beep_Driver();
								liv_key_down_time = (u8)((giv_sys_time - liv_time)/10000) ;
									//按键时间大于3秒并且到加秒时间
								if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
								{
									 liv_number =  liv_key_down_time;
									 Cease_Big_Subb_Time();
								}			 
								Cease_display_set();
								AutoDisplay();
								IWDG_ReloadCounter();
							}
							while(( gsv_yaokong.key == YAOKONG_BACK) &&(gsv_yaokong.effect != 0)) ;
								Cease_Subb_Time();
						}
		    break;

		/////按下定点清扫进入定点清扫模式
	    case YAOKONG_SETTIME_TUE:
		    if(	piv_set_content == 4)
					{
							piv_dis_week = 3;  
					}
		    break;

		////按下自动清扫键///////////////
	    case YAOKONG_AUTO_THU:
		    if(	piv_set_content == 4)
					{
							piv_dis_week = 4; 
					}
		    break;

	    case YAOKONG_SPOT_FRI:
		    if(	piv_set_content == 4)
					{
							piv_dis_week = 5;
					}
		    break;

	    case YAOKONG_DOCKING_SUN:  
		    if(	piv_set_content == 4)
					{
							piv_dis_week = 6;
					}
		    break;
	
	    case YAOKONG_LEFT:	   
		    piv_set_content--;
				if(piv_set_content == 1)
					{
							piv_set_content = 4;
					}
			break;
	    case YAOKONG_RIGHT:
		    piv_set_content++;
				if(piv_set_content == 5)
					{
							piv_set_content = 2;
					}	 
			break;

	    case YAOKONG_OK:
		    Cease_Save_Time();	  	   
				Init_Cease();
		    break;
	}
}
#endif
/*******************************************************************************
连续增加时间
*******************************************************************************/
void Cease_Big_Add_Time(void)
{
    if(piv_set_content == 3)
    {
	    piv_dis_minute = (piv_dis_minute+3)%60;
	}
	else if(piv_set_content == 2)
	     {
	         piv_dis_hour = (piv_dis_hour + 3) % 24;
	     }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+3)%7;
			  }	  */
}

/*******************************************************************************
单独增加时间
*******************************************************************************/
void Cease_Add_Time(void)
{
    if(piv_set_content == 3)
	{
		piv_dis_minute = (piv_dis_minute+1)%60;
	}
	else if(piv_set_content == 2)
		 {
		     piv_dis_hour = (piv_dis_hour + 1) % 24;
		 }
		 /*else if(piv_set_content == 4)
			 {
				 piv_dis_week = (piv_dis_week+1)%7;
			 }	 */
}

/*******************************************************************************
连续减少时间
*******************************************************************************/
void Cease_Big_Subb_Time(void)
{
    if(piv_set_content == 3)
    {
	    piv_dis_minute = (piv_dis_minute+57)%60;
	}
	else if(piv_set_content == 2)
		 {
			 piv_dis_hour = (piv_dis_hour + 21) % 24;
		 }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+4)%7;
		      }*/
}

/*******************************************************************************
单一减少时间
*******************************************************************************/
void Cease_Subb_Time(void)
{
    if(piv_set_content == 3)
	{
	    piv_dis_minute = (piv_dis_minute+59)%60;
	}
	else if(piv_set_content == 2)
		 {
		     piv_dis_hour = (piv_dis_hour + 23) % 24;
		 }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+6)%7;
			  }	*/
}

void Init_VirtualSleep(void)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	
	/******初始化设置的值********************/
	piv_set_content = 0;			//开始设置为没有设置
	piv_yaokongstate = 0;

	/*******初始化输出的控制***************/
	stop_rap(); //关闭左右轮
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);
	Disable_earth();				//关闭地检
	Disable_wall();					//关闭墙检
	//Enable_earth();
	//Enable_wall();
	enable_hwincept();				//打开回充红外接收电源
	Enable_Speed(); 				//待机状态将速度检测打开，是为了防止进入CEASE时关闭速度检测会导致惯性脉冲无法计算。
		
	/****设置机器的工作模式******/   
	mode.mode = CEASE; 
	mode.Info_Abort=0;				//打开SLAM通信，qz add 20180418
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.sub_mode=SUBMODE_VIRTUAL_SLEEP;			//QZ ADD
	mode.step=0x00; 				//qz add
	mode.status=0;					//qz add 20180625
	mode.time=giv_sys_time; 		//qz add 20180703
	mode.init_mode_time=giv_sys_time;	//qz add 20180814
		
	WriteWorkState();
	Disable_Free_Skid_Check();		//关闭万向轮检测
	//初始化检测的条件
	CHECK_STATUS_FLAG=false; 		//使能异常检测
	Init_Check_Status();//qz add 20180425

	Slam_Data.dipan_req_pre=1;		//qz mask 20180522：默认为规划清扫

#ifdef DEBUG_Enter_Mode
	TRACE("Init Cease Mode Complete!Prepare enter to Cease!\r\n");
#endif
	SLAM_DOCK=false;
	DOCK_SWEEP=false;				//qz add 20180803

#ifdef UV
	Reset_UV(); 					//关闭UV
#endif 
	REYBS_TIME=0;					//qz add 20180910,小回充重新请求沿边次数清0
	Close_Led();
}

void Do_VirtualSleep(void)
{
	/**************如果有遥控器按键*/
#ifdef DC_NOBAT_RUN
	if(((power.charge_dc)|(power.charge_seat))&(!dc_nobat_run))
#else
	if(power.charge_dc|power.charge_seat)
#endif
		{
			 //mode.last_mode=mode.mode;			//qz add 20180201
			 //Init_Chargeing(CHARGEING);

			 //qz add 20180901
			 if(power.charge_seat)
				Init_Chargeing(SEAT_CHARGING);
			 if(power.charge_dc)
				Init_Chargeing(DC_CHARGING);
			 //qz add end
			 
			 return;
		}
	/////////机器的预约时间到，机器进入扫地模式/////////////////////

	//计划是在charge和sleep下进行预约时间检查，输出一个flag，然后在charge和cease下检查这个flag
#if 0
	if(Check_PreengageTime())
			{
	//				 if(Slam_Data.tick_flag)			//判断导航板是否有心跳处于工作状态	//qz mask 20180420
					{
						//qz add 20180420
						Slam_Data.dipan_req_pre=DIPAN_REQ_PREEN;
						//qz add end
						Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
					}
				 return;
			}
#endif
	//系统有错误
	if(giv_sys_err != 0)
			{
				Init_Err();
				return ;
			}
#ifdef SLEEP_SUBMODE
	/**********如果机器在3分钟内没有操作,机器进入睡眠模式**********/
	if(((giv_sys_time-mode.time)>10000*600)&(!mode.factory))			//qz mask 20181110
	//if(((giv_sys_time-mode.sleep_time)>1800000)&(!mode.factory))		//qz add 20181110
		{
			Init_Sleep();
			//Init_ShutDown();
			return;
		}
#endif
	clr_all_hw_effect();				//qz mask 20181215
	//	find_home=ReadHwSign_My();			//qz mask 20181215
}
