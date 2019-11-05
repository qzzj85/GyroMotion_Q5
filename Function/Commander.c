
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

//******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义************
//=====================全局变量====================================


//=====================全局函数====================================



//******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义************
//=====================私有变量====================================
extern MOTO_TASK moto_task;
struct YANBIAN
{
	unsigned char mode;
	unsigned char find_wall_step;
}yb;

u8 read_bump_comm(void)
{
	u32 data1=0;
#ifdef CLIFF_ENABLE			//ZDK屏蔽
	
	   if((e_l.sign == FARN))
	   {
		  if((mode.bump > 4) || (mode.bump == 0))		//左地检悬空
		   { 
			 			stop_rap();
						mode.bump = 1;// ADC_EARTH_L;
						mode.step_bp=0;
						mode.Info_Abort=1;
			}
			return 1;//E_L;
	   }   
	   if((e_m.sign==FARN))
	   {
				if((mode.bump > 4) || (mode.bump == 0)) 	
					 {
							stop_rap();
							mode.bump= 2;//E_LM;
							mode.step_bp=0;
							mode.Info_Abort=1;
					 }
				return 2;//E_LM;
				
		}
		
	   if((e_r.sign==FARN))
	   {
		  if((mode.bump > 4) || (mode.bump == 0))	   
				{	
			  		stop_rap();
					mode.bump=4;//E_R;
					mode.step_bp=0;
					mode.Info_Abort=1;
				}
				return 4;//E_R;
	   }
	   
#endif	 
	
		data1=Parse_BumpValue();
		switch (data1)
			{
				case BUMP_ONLY_LEFT:
					if(!mode.bump_flag)
						{
							stop_rap();
							mode.bump=BUMP_ONLY_LEFT;
							mode.bump_flag=true;
							mode.step_bp=0;
							Slam_Data.l_bump_flag=true;
						}
					return BUMP_ONLY_LEFT;
				case BUMP_ONLY_LEFTMID:
					if(!mode.bump_flag)
						{
							stop_rap();
							mode.bump=BUMP_ONLY_LEFTMID;
							mode.bump_flag=true;
							mode.step_bp=0;
							Slam_Data.l_bump_flag=true;
						}
					return BUMP_ONLY_LEFTMID;
				case BUMP_LEFT_MID:
					if(!mode.bump_flag)
						{							
							stop_rap();
							mode.bump=BUMP_LEFT_MID;
							mode.bump_flag=true;
							mode.step_bp=0;
							Slam_Data.l_bump_flag=true;
						}
					return BUMP_LEFT_MID;
				case BUMP_ONLY_RIGHT:
					if(!mode.bump_flag)
						{							
							stop_rap();
							mode.bump=BUMP_ONLY_RIGHT;
							mode.bump_flag=true;
							mode.step_bp=0;
							Slam_Data.r_bump_flag=true;
						}
					return BUMP_ONLY_RIGHT;
				case BUMP_ONLY_RIGHTMID:
					if(!mode.bump_flag)
						{							
							stop_rap();
							mode.bump=BUMP_ONLY_RIGHTMID;
							mode.bump_flag=true;
							mode.step_bp=0;
							Slam_Data.r_bump_flag=true;
						}
					return BUMP_ONLY_RIGHTMID;
				case BUMP_RIGHT_MID:
					if(!mode.bump_flag)
						{							
							stop_rap();
							mode.bump=BUMP_RIGHT_MID;
							mode.bump_flag=true;
							mode.step_bp=0;
							Slam_Data.r_bump_flag=true;
						}
					return BUMP_RIGHT_MID;
				case BUMP_MID:
					if(!mode.bump_flag)
						{							
							stop_rap();
							mode.bump=BUMP_MID;
							mode.bump_flag=true;
							mode.step_bp=0;
							Slam_Data.m_bump_flag=true;
						}
					return BUMP_MID;
			}
	
		 if((up_hw.effect)||(dummy_wall.effect))
		 {
				 if(//((up_hw.data & 0x8f) == 0)|| 
					 (up_hw.data==0xec) || (up_hw.data==0xCC) || 
				 (up_hw.data==0xdc) || (dummy_wall.effect))
			 {
						 if((mode.bump == 0))
					 {
							 mode.bump=10;//INFRARED;
							 mode.step_bp=0;
							 stop_rap();
					 }
					 return 10;//INFRARED;
			 }
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

#ifdef COMMANDER_SEAT_REPORT						//qz add 20180814
	if(r_hw.effectNear|r_hw.effectTopReal)
		{
			Slam_Data.ir_flag=true;
		}
#endif

#ifdef SLAM_FIND_SEAT
	if(SLAM_DOCK)
		{
			data1=ReadHwSign_My();
			if(data1&0xD1111)						//only top signal
				{
					if(mode.bump==0)
						{
							stop_rap();
							mode.bump=0xfe;
							mode.step_bp=0;
							mode.Info_Abort=1;
						}
				}
		}
#endif


	return 0;
	
	
}


void  Motor_Speed_Run(u16 l_SPD, u16 r_SPD);
//=====================私有函数====================================



//void yanbian_init(void);
//void yanbian(void);
#ifdef COMMAND_CLIFF_ACTION

void Cliff_Bump_Action(u8 bump_temp)
{
	static u8 l_cliff_time=0,r_cliff_time=0;
	switch (mode.bump)
	{
		case BUMP_L_CLIFF:
			switch (mode.step_bp)
				{
					case 0:
						Speed=3000;
						if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))		//后退
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 1:
						stop_rap();
						if((e_l.sign==NEAR))					//左地检接地
							{
								mode.step_bp=0xF0;
							}
						else									//左地检悬崖
							{
								mode.step_bp++;
							}
						break;
					case 2:
						Speed=3000;								
						if(do_action(7,180*Angle_1))			//右轮不动向左转
							{
								stop_rap();
								mode.step_bp++;
							}
						if(e_l.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0xF0;
							}
						break;
					case 3:
						Speed=2000;									//左转10度
						if(do_action(1,10*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 4:
						Speed=2000;									//前进1cm
						if(do_action(3,1*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 5:
						Speed=3000;									//后退10cm
						if(do_action(4,10*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 6:											//检查左地检
						if(e_l.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								mode.step_bp++;
							}
						break;
					case 7:
						Speed=2000; 								//右转10度
						if(do_action(2,10*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 8:
						Speed=2000; 								//前进1cm
						if(do_action(3,1*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 9:
						Speed=3000; 								//后退10cm
						if(do_action(4,10*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 10: 										//检查左地检
						if(e_l.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								error_code=SEND_ERROR_DANGER;
								dis_err_code=DIS_ERROR_DANGER;
								mode.err_code|=WIFI_ERR_EARTH;
								Send_Voice(VOICE_ERROR_DANGER);
								l_cliff_time=0;			//qz add 20180927
							}
						break;
					case 0XF0:
						Speed=2000;
						if(do_action(1,90*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						if(bump_temp)
							{
								stop_rap();
								mode.bump=bump_temp;
								mode.step_bp=0;

								//qz add 20180927
								if(bump_temp==BUMP_L_CLIFF)
									{
										l_cliff_time++;
										if(l_cliff_time>3)
											mode.step_bp=10;
									}
								//qz add end
							}
						break;
					case 0xF1:
						mode.bump=0;
						mode.step_bp=0;
						mode.step=0xD0;
						mode.Info_Abort=0;
						l_cliff_time=0;	//qz add 20180927
						return;
				}
			break;
			
		case BUMP_M_CLIFF:
			switch (mode.step_bp)
				{
					case 0:
						Speed=3000;
						if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))		//后退
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 1:
						if(e_m.sign==NEAR)
							{
								mode.step_bp=0xF0;
							}
						else
							{
								mode.step_bp++;
							}
						break;
					case 2:
						mode.step_bp++;
						break;
					case 3:
						Speed=2000; 								//左转10度
						if(do_action(1,10*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 4:
						Speed=2000; 								//前进1cm
						if(do_action(3,1*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 5:
						Speed=3000; 								//后退10cm
						if(do_action(4,10*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 6: 										//检查左地检
						if(e_m.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								mode.step_bp++;
							}
						break;
					case 7:
						Speed=2000; 								//右转10度
						if(do_action(2,10*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 8:
						Speed=2000; 								//前进1cm
						if(do_action(3,1*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 9:
						Speed=3000; 								//后退10cm
						if(do_action(4,10*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
							}
						break;
					case 10:										//检查左地检
						if(e_m.sign==NEAR)
							{
								stop_rap();
								mode.step_bp=0XF0;					//OK去F0
							}
						else
							{
								error_code=SEND_ERROR_DANGER;
								dis_err_code=DIS_ERROR_DANGER;
								mode.err_code|=WIFI_ERR_EARTH;
								Send_Voice(VOICE_ERROR_DANGER);
							}
						break;
					case 0XF0:
						Speed=2000;
						if(do_action(1,90*Angle_1))
							{
								stop_rap();
								mode.step_bp++;
							}
						if(bump_temp)
							{
								stop_rap();
								mode.bump=bump_temp;
								mode.step_bp=0;
							}
						break;
					case 0xF1:
						mode.bump=0;
						mode.step_bp=0;
						mode.step=0xD0;
						mode.Info_Abort=0;
						return;
				}
			break;

		case BUMP_R_CLIFF:
			switch (mode.step_bp)
				{
				case 0:
					Speed=3000;
					if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS)) 		//后退
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 1:
					if((e_r.sign==NEAR))					//左地检接地
						{
							mode.step_bp=0xF0;
						}
					else									//左地检悬崖
						{
							mode.step_bp++;
						}
					break;
				case 2:
					Speed=3000; 							
					if(do_action(8,180*Angle_1))			//左轮不动向右转
						{
							stop_rap();
							mode.step_bp++;
						}
					if(e_r.sign==NEAR)			//qz modify 20180929 e_l--->e_r
						{
							mode.step_bp=0xf0;
						}
					break;
				case 3:
					Speed=2000; 								//左转10度
					if(do_action(1,10*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 4:
					Speed=2000; 								//前进1cm
					if(do_action(3,1*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 5:
					Speed=3000; 								//后退10cm
					if(do_action(4,10*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 6: 										//检查左地检
					if(e_r.sign==NEAR)
						{
							stop_rap();
							mode.step_bp=0XF0;					//OK去F0
						}
					else
						{
							mode.step_bp++;
						}
					break;
				case 7:
					Speed=2000; 								//右转10度
					if(do_action(2,10*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 8:
					Speed=2000; 								//前进1cm
					if(do_action(3,1*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 9:
					Speed=3000; 								//后退10cm
					if(do_action(4,10*CM_PLUS))
						{
							stop_rap();
							mode.step_bp++;
						}
					break;
				case 10:										//检查左地检
					if(e_r.sign==NEAR)
						{
							stop_rap();
							mode.step_bp=0XF0;					//OK去F0
						}
					else
						{
							error_code=SEND_ERROR_DANGER;
							dis_err_code=DIS_ERROR_DANGER;
							mode.err_code|=WIFI_ERR_EARTH;
							r_cliff_time=0;						//qz add 20180927
							Send_Voice(VOICE_ERROR_DANGER);
						}
					break;
				case 0XF0:
					Speed=2000;
					if(do_action(1,90*Angle_1))
						{
							stop_rap();
							mode.step_bp++;
						}
					if(bump_temp)
						{
							stop_rap();
							mode.bump=bump_temp;
							mode.step_bp=0;
							//qz add 20180927
							if(bump_temp==BUMP_R_CLIFF)
								{
									r_cliff_time++;
									if(r_cliff_time>3)
										mode.step_bp=10;
								}
							//qz add end
						}
					break;
				case 0xF1:
					mode.bump=0;
					mode.step_bp=0;
					mode.step=0xD0;
					mode.Info_Abort=0;
					r_cliff_time=0;			//qz add 20180927
					return;
				}
			break;
	}
}
#endif

void Commander_Bump_Action(u8 temp_bump)
{
		switch(mode.bump)
		{
				case 0:
					break;
		
#ifndef COMMAND_CLIFF_ACTION
				case 1: 			//左地检悬空
						mode.step = 0xD0;
						Speed = 3000;		//qz modify 20180814 2400--->3000
						if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))
						{
							mode.bump = 0;
							mode.step_bp=0;
							mode.Info_Abort=0;
						}
					return;
				case 2:
					mode.step = 0xD0;
					Speed = 3000;			//qz modify 20180814 2400--->3000
					if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))
					{
						mode.bump = 0;
						mode.step_bp=0;
						mode.Info_Abort=0;
					}
					return;
				case 4: 			//右地检悬空
						mode.step = 0xD0;
						Speed = 3000;		//qz modify 20180814 2400--->3000
						if(do_action(4,CLIFF_BACK_LENGTH*CM_PLUS))
						{
							mode.bump = 0;
							mode.step_bp=0;
							mode.Info_Abort=0;
						}
					return;
#else
				case 1:
				case 2:
				case 4:
					Cliff_Bump_Action(temp_bump);
					return;
#endif
	
				case BUMP_ONLY_LEFTMID:
				case BUMP_ONLY_LEFT:
				case BUMP_LEFT_MID:
				case BUMP_ONLY_RIGHT:
				case BUMP_ONLY_RIGHTMID:
				case BUMP_RIGHT_MID:
				case BUMP_MID:
					switch (mode.step_bp)
						{
							case 0:
								stop_rap();
								mode.Info_Abort=1;
								mode.step_bp=1;
								mode.bump_time=giv_sys_time;
								return;
							case 1:
								Speed=BUMP_BACK_SPEED;
								//qz add 20180314
								if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
									return;
								//qz add end
								if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
									{
										stop_rap();
										mode.step_bp=2;
										mode.bump_time=giv_sys_time;

										if(bump_value)
											mode.step_bp=0;
										//qz add end
										return; 								
									}
								return;
							case 2:
								if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
									return;
								mode.bump=0;
								mode.bump_flag=false;
								mode.step_bp=0;
								mode.Info_Abort=0;
								mode.step=0xD0;
								break;
						}
				break;
				case 99:	//	超声停止
				//////		Enable_Beep_number(3);
				//////		Init_Cease();	
					return;
				//qz add
				case 0xff:		//打滑
					switch (mode.step_bp)
						{
							case 0:
								Speed=6000;
								if(do_action(4,20*CM_PLUS))
									{
										stop_rap();
										mode.step_bp=1;
									}
								return;
							case 1:
								mode.bump=0;
								mode.step_bp=0;
								mode.Info_Abort=0;
								mode.step=0xF0;
								Slam_Data.skid_flag=0;	//打滑处理完毕，打滑上传标志清零（其实不需要，在sensor_report中会清零）
								return;
						}
					break;
					
#ifdef SLAM_FIND_SEAT
				case 0xfe:
					switch (mode.step_bp)
						{
							case 0:
								Speed=
						}
					break;
#endif

				default:
					break;
				
		}
}

//===============================================================================================================
//===============================================================================================================
void Commander_Yes_Sir(void)
{
	static uint32_t  uin32;
	float flo_1,flo_2;
	u8 m,temp_bump;

	//------------------------------------------------------------------------------
	/////有充电的电源插入
  #if 1		
#ifdef DC_NOBAT_RUN
	if((power.charge_dc)&(!dc_nobat_run))
#else
	if(power.charge_dc)
#endif
			{
					 stop_rap();
					 uin32 = giv_sys_time;
					 while(giv_sys_time - uin32 < 20000)
						 {
								AutoReadKey();
								judge_charge();
						 }
						 
#ifdef DC_NOBAT_RUN
					   	if((power.charge_dc)&(!dc_nobat_run))
#else
					   	if(power.charge_dc)
#endif
							 {
									 //Init_Chargeing(CHARGEING);

									//qz add 20180901
									 Init_Chargeing(DC_CHARGING);
									 return;
							 }					
			}
  #endif
	//------------------------------------------------------------------------------

#ifndef SLAM_FIND_SEAT
 	clr_hw_effect(&r_hw);
#else
	clr_all_hw_effect();
#endif

//	Dust_Level_Check();		 //污度等级检测	//qz add 20180725  //qz mask 20181215
		
	ACC_DEC_Curve();		  //左右轮调速			 

	m=Read_Protect();
	if(mode.abnormity)
		{
			Action_Protect_My(m);
#ifdef FREE_SKID_INDEP_CHECK
			Free_Skid_Indep.check_flag=false;
#endif
#ifdef FREE_SKID_CHECK			//qz add 20180904
			Free_Skid.check_flag=false;
			Disable_Free_Skid_Check();		//qz add 20180927
#endif
			return;
		}
			 
  	//在遥控的过程中有障碍信号
  	temp_bump=read_bump_comm();	//	读取碰撞信息	
	Commander_Bump_Action(temp_bump);
	if(mode.bump)
	{
#ifdef FREE_SKID_INDEP_CHECK
		Free_Skid_Indep.check_flag=false;
#endif
#ifdef FREE_SKID_CHECK			//qz add 20180904
		Free_Skid.check_flag=false;
		Disable_Free_Skid_Check();			//qz add 20180927
#endif
		return;
	}

	
   //命令动作执行
   //QZ:in command mode,mode.step=mode.command_motion_type 0x01:原地旋转,0x02:圆周运动,0x05:直线行走,0xA0:SPEED RUN
   switch (mode.step)
		 {
				//-----------------------------------------------------------------------		
				case 0x05:
						r_rap.State = 0x00;
						mode.step = 0x10;
						break;
				case 0x10:
						if(mode.Command_Desired_Positon >= 0 )
						{
								if(do_action(3,(uint32_t)(  mode.Command_Desired_Positon * CM_PLUS) )    == 1)	//原来为CM_PLUS
								{
										mode.step = 0xD0;
								}
						}
						else
						{
								if(do_action(4,(uint32_t)( (-mode.Command_Desired_Positon) * CM_PLUS) ) == 1)
								{
										mode.step = 0xD0;
								}
						}
						break;
				
				case 1:
						r_rap.State = 0x20;	//加速状态
						mode.step = 0x20;
						break;
								
				case 0x20:				   	//	原地旋转
						uin32 = (uint32_t)(	(float)((mode.Command_Desired_Degree) * (float)(Angle_1))  );
						if(mode.Command_Desired_Degree >= 0 )
								{
										mode.step = 0x11;
								}
						else{
										mode.step = 0x12;
								}
						break;
										case 0x11:
													if(do_action(1,	uin32) == 1)
															{
																	mode.step = 0xD0;
															}
												break;
												
										case 0x12:
													if(do_action(2,	uin32) == 1)
															{
																	mode.step = 0xD0;
															}
												break;
								
				case 2:		
							
				//		r_rap.State = 0x05;			//	半径转弯
						r_rap.State = 0x70;
						
						
						mode.Command_Desired_radius = mode.Command_Desired_radius /10;
			
						flo_1 = ((float)(mode.Command_Desired_radius) * CM_PLUS) * 2 * 3.14 ;
						flo_2 = (float)(mode.Command_Desired_Degree) / 360;
			
						mode.Command_Desired_Positon = (int32_t)(flo_1 * flo_2);	
					
						flo_1 =  	(float)(mode.Command_Desired_radius) / (float)((float)(mode.Command_Desired_radius) + 26.76);
						
						if(mode.Command_Desired_radius <= 0)
							{
									r_rap.length = mode.Command_Desired_Positon;
									r_rap.master = 1;   //主轮
								
									l_rap.length = mode.Command_Desired_Positon / flo_1;
									l_rap.master = 0;  
							}
						else
							{
									r_rap.length = mode.Command_Desired_Positon / flo_1;
									r_rap.master = 0;  
								
									l_rap.length = mode.Command_Desired_Positon;
									l_rap.master = 1;   //主轮
							}		

						Speed = mode.Command_Desired_Speed;		
						enable_rap(FRONT,l_rap.length ,				FRONT,  r_rap.length  );	
			
							
						mode.step 	= 0x30;	
						break;
							
		
				case 0x30:	//	等待
					if((r_rap.sign	== 0) && (l_rap.sign	== 0) )
							{
								mode.time = giv_sys_time;	
									mode.step = 0x31;
							}
							
							

						if(r_rap.length  <= r_ring.length)
								{
									r_rap.rap 	= 0;
									r_rap.sign	= 0;
									r_rap.pwm		= 0;
									Break_pwm(R_BACK);
									Break_pwm(R_FRONT);
								}

						if(l_rap.length  <= l_ring.length)
								{
									l_rap.rap 	= 0;
									l_rap.sign	= 0;
									l_rap.pwm		= 0;
									Break_pwm(L_FRONT);
									Break_pwm(L_BACK);
								}
					break;							
					
				case 0x31:	//	等待
					if((giv_sys_time - mode.time) > 100)
							{
									disable_pwm(R_BACK);
									disable_pwm(R_FRONT);
									disable_pwm(L_FRONT);
									disable_pwm(L_BACK);
								
									mode.step = 0xD0;
							}
					break;
							
				case 0xA0:
					#if 0
					//qz add
					if(mode.Command_Last_Spd_L!=mode.Command_Spd_L)
						l_rap.State 	= 0x70;
					if(mode.Command_Last_Spd_R!=mode.Command_Spd_R)
					//qz add end
						r_rap.State		=0x70;
					#endif
					mode.step 		= 0xA1;
			
			
					if((mode.Command_Last_Spd_R < 0)&&(mode.Command_Spd_R > 0))
							{		
									mode.step 		= 0xA3;
									//qz add 20180322
									return;
									//qz add end
							}
							
					if((mode.Command_Last_Spd_R > 0)&&(mode.Command_Spd_R < 0))
							{		
									mode.step		= 0xA3;
									//qz add 20180322
									return;
									//qz add end
							}
						
							
					if((mode.Command_Last_Spd_L < 0)&&(mode.Command_Spd_L > 0))
							{		
									mode.step		= 0xA3;
									//qz add 20180322
									return;
									//qz add end
							}
							
					if((mode.Command_Last_Spd_L > 0)&&(mode.Command_Spd_L < 0))
							{		
									mode.step		= 0xA3;
									//qz add 20180322
									return;
									//qz add end
							}

					//qz add
					mode.Command_Last_Spd_R = mode.Command_Spd_R;	
					mode.Command_Last_Spd_L = mode.Command_Spd_L;
					//qz add end
					
				break;
							
							
				case 0xA3:
//											l_rap.rap 	= 600;	
//										
//											l_rap.rap 	= 600;	//qz mask
//											//r_rap.rap=600;	//qz add
											stop_rap();
								
											mode.time = giv_sys_time;
											mode.step 		= 0xA4;
									break;

				case 0xA4:
										if((giv_sys_time - mode.time) >5000)		//延时500ms
													{
															mode.step 		= 0xA5;
													}	
									break;
													

				case 0xA5:
//											r_rap.rap 	= 0;
//											r_rap.sign	= 0;
//											r_rap.pwm		= 0;
//											Break_pwm(R_BACK);
//											Break_pwm(R_FRONT);
//										
//											l_rap.rap 	= 0;
//											l_rap.sign	= 0;
//											l_rap.pwm		= 0;
//											Break_pwm(L_FRONT);
//											Break_pwm(L_BACK);
										
								
								
											mode.Command_Last_Spd_R = mode.Command_Spd_R;	
											mode.Command_Last_Spd_L = mode.Command_Spd_L;	
								
								
											mode.time = giv_sys_time; 
											mode.step 		= 0xA6;
									break;

				case 0xA6:
//										if((giv_sys_time - mode.time) > 1500)
													{
															mode.step 		= 0xA1;
													}	
									break;
										
													

			    case 0xA1:		//	等待			
					//r_rap.State = 0x70;		
				
					if(mode.Command_Spd_R == 0)
							{
									r_rap.rap 	= 0;
									r_rap.sign	= 0;
									r_rap.pwm		= 0;
									disable_pwm(R_BACK);
									disable_pwm(R_FRONT);
									r_ring.real_speed=0;

									//qz add
									r_rap.Max_Speed=0;
									r_rap.State=0x70;
									r_rap.master=0;	//qz add 20180123
							}
					else if(mode.Command_Spd_R > 0)
							{
									#if 0
									//qz add
									r_rap.Max_Speed=mode.Command_Spd_R;
									if(r_rap.Max_Speed>r_rap.rap)
										{
											if(r_rap.Max_Speed-r_rap.rap>=900)
												{
													r_rap.AccDec=(r_rap.Max_Speed-r_rap.rap)/3;
													r_rap.State=0x80;	//加速
												}
											else
												{
													r_rap.rap=r_rap.Max_Speed;
													r_rap.AccDec=0;
													r_rap.State=0x70;
												}
										}
									else if(r_rap.Max_Speed<r_rap.rap)
										{
											if(r_rap.rap-r_rap.Max_Speed>=900)
												{
													r_rap.AccDec=(-(r_rap.rap-r_rap.Max_Speed)/3);
													r_rap.State=0x90;
												}
											else
												{
													r_rap.rap=r_rap.Max_Speed;
													r_rap.AccDec=0;
													r_rap.State=0x70;
												}											
										}
									//qz add end
									#else
									r_rap.rap 		= mode.Command_Spd_R;
									r_rap.State=0x70;
									#endif
									
									r_rap.sign		=	1;
//									r_rap.master	=	1;	//qz mask 20180123
									r_rap.ori 		= FRONT;
							}
					else if(mode.Command_Spd_R < 0)
							{
									//qz mask
									r_rap.rap 		= (-mode.Command_Spd_R); 

									//qz add
									//r_rap.Max_Speed=(-mode.Command_Spd_R);
									//if(r_rap.Max_Speed>r_rap.rap)
									//r_rap.State=0x80;	//加速
									//else if(r_rap.Max_Speed<r_rap.rap)
									//r_rap.State=0x90;	//减速
									//qz add end
									
									
									r_rap.sign		=	1;
//									r_rap.master	=	1;	//qz mask 20180123
									r_rap.ori 		= BACK;
							}
							
					r_rap.length = r_ring.length + 150000;	//10米
						
			
					if(mode.Command_Spd_L == 0)
							{
									l_rap.rap 	= 0;
									l_rap.sign	= 0;
									l_rap.pwm		= 0;
									disable_pwm(L_BACK);
									disable_pwm(L_FRONT);
									l_ring.real_speed=0;

									//qz add
									l_rap.Max_Speed=0;
									l_rap.State=0x70;
									l_rap.master=0;	//qz add 20180123
							}
					else if(mode.Command_Spd_L > 0)
							{
									#if 0
									//qz add
									l_rap.Max_Speed=mode.Command_Spd_L;
									if(l_rap.Max_Speed>l_rap.rap)
										{
											if(l_rap.Max_Speed-l_rap.rap>=900)
												{
													l_rap.AccDec=(l_rap.Max_Speed-l_rap.rap)/3;
													l_rap.State=0x80;	//加速
												}
											else
												{
													l_rap.rap=l_rap.Max_Speed;
													l_rap.AccDec=0;
													l_rap.State=0x70;
												}
										}
									else if(l_rap.Max_Speed<l_rap.rap)
										{
											if(l_rap.rap-l_rap.Max_Speed>=900)
												{
													l_rap.AccDec=(-(l_rap.rap-l_rap.Max_Speed)/3);
													l_rap.State=0x90;
												}
											else
												{
													l_rap.rap=l_rap.Max_Speed;
													l_rap.AccDec=0;
													l_rap.State=0x70;
												}											
										}
									//qz add end
									#else
									l_rap.rap 		= mode.Command_Spd_L; 
									l_rap.State=0x70;
									#endif
									
									l_rap.sign		=	1;
									l_rap.master	=	1;		//qz add:以左轮为主动轮,20180123
									l_rap.ori 		= FRONT;
							}
					else if(mode.Command_Spd_L < 0)
							{
									//qz mask
									l_rap.rap 		= (-mode.Command_Spd_L);

									//qz add
									//l_rap.Max_Speed=(-mode.Command_Spd_L);
									//if(l_rap.Max_Speed>l_rap.rap)
									//	l_rap.State=0x80;	//加速
									//else if(l_rap.Max_Speed<l_rap.rap)
									//	l_rap.State=0x90;	//减速
									//qz add end

									l_rap.sign		=	1;
									l_rap.master	=	1;
									l_rap.ori 		= BACK;
							}
							
					l_rap.length = l_ring.length + 150000;		//10米

					//qz add 20180123
//					if((mode.Command_Spd_L>100*MM_PLUS)&&(mode.Command_Spd_R>100*MM_PLUS))
					if((abs(mode.Command_Spd_L-mode.Command_Spd_R)<210)&&((mode.Command_Spd_L!=0)&&(mode.Command_Spd_R!=0)))
						{
							action.action_flag=1;						
#ifdef FREE_SKID_INDEP_CHECK
							Free_Skid_Indep.check_flag=true;
#endif
						}
					else
						{
							if((mode.Command_Spd_L==0)&(mode.Command_Spd_R==0))
								action.action_flag=0;
							else
								action.action_flag=2;

#ifdef FREE_SKID_INDEP_CHECK
							Free_Skid_Indep.check_flag=false;
#endif
						}
					//qz add end

#ifdef FREE_SKID_CHECK
					if((mode.Command_Spd_L==0)&(mode.Command_Spd_R==0)&(Free_Skid.check_flag))
						{
							Disable_Free_Skid_Check();
						}


					else if(((mode.Command_Spd_L!=0)|(mode.Command_Spd_R!=0))&(!Free_Skid.check_flag))
						{
							Enable_Free_Skid_Check();
						}
#endif

					//qz add 20180316
#ifdef	ROTATE_SKID_CHECK
					if(((mode.Command_Spd_L>0)&&(mode.Command_Spd_R<0))||((mode.Command_Spd_L<0)&&(mode.Command_Spd_R>0)))
						{
							if((mode.Command_Spd_L>0))
								{
									Enable_Rotate_Skid_Check(1);		//向右转
								}
							else if(mode.Command_Spd_L<0)
								{
									Enable_Rotate_Skid_Check(0);		//向左转
								}
						}
else
						Disable_Rotate_Skid_Check();
#endif
					//qz add 20180316


					//mode.step = 0xA1;		//qz mask 20180117
					mode.step=0xF0;
					break;

				//-----------------------------------------------------------------------	
				
				//-----------------------------------------------------------------------				
				case 0xD0:
						mode.Command_Finish_Answer_Flag = COMMAND_REPORT_MASTER_0x22;

						stop_rap();
						mode.step = 0xF0;
						break;
								
				case 0xF0:		//	等待	  
						mode.step = 0xF0;
						break;
				//-----------------------------------------------------------------------	
				
				default:
					mode.step = 0xF0;	 
						break;     	   	    
		 } 
	
}
//===============================================================================================================
//===============================================================================================================

	


/******************************************************************
功能：初始化遥控程序
******************************************************************/
void Init_Commander(void)
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

	#if 1
	if(DOCK_SWEEP)
		Sweep_Level_Set(DOCK_SWEEP_LEVEL);
	else
		Sweep_Level_Set(sweep_suction);
	#endif
	 
	Display_Real();
	Init_Action();
	
	mode.mode 											= COMMANDER_x10;
	mode.sub_mode										= COMMANDER_x10;
	mode.bump 											= 0;
	mode.action 										= 0;
	mode.Command_Running_Flag 							= COMMAND_RUNNING_0x11;
	mode.Command_Finish_Answer_Flag 					= COMMAND_REPORT_MASTER_FINISH_0x00;				
//	mode.step 											= mode.Command_Motion_Type;
	mode.Command_Spd_L=0;			//qz add 20180725
	mode.Command_Spd_R=0;			//qz add 20180725
	mode.step=0xA0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
	WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;

	//qz add 20180313
//	Write_Mode_Backup();
	//qz add end
#ifdef DEBUG_Enter_Mode
	TRACE("Init Commander Mode Complete!\r\n");
#endif
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	//memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0]));		//清除接收缓存qz add 20180703
	
	if(dis_err_code==DIS_ERROR_DOCK_FAIL)		//qz add 20180710
		{
			dis_err_code=0;
			error_code=0;
		}

#ifdef UV
	if((mode.status)&(!SLAM_DOCK))		//qz add 20180902
		Set_UV();
	else
		Reset_UV();
#endif
}




void  Motor_Speed_Run(u16 l_SPD, u16 r_SPD)
{
	if(giv_sys_err != 0)
			{
				return;
			}

	l_rap.rap 		= l_SPD;
	r_rap.rap 		= r_SPD;
	l_rap.sign   	=	1;   
	r_rap.sign		=	1;
	l_rap.master	=	1;
	r_rap.master	=	0;

}
//==============================================================================================
//==============================================================================================


