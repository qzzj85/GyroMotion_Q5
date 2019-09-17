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
	static u8 timeorpreen=false;	//用于区分时间设置还是预约设置。1为时间设置,2为预约设置
	u8 temp_data1,i;
	//机器处于CEASE的其他模式,退出

	//Clr_Remote_Info();

	if((giv_sys_time-remote_info.key_time>100000)&(Dis_TimeSet))
		{
			Dis_TimeSet=false;
			Dis_H_M_Glint=false;
		}

	if(!remote_info.effect)
		return;

	remote_info.effect=false;
	
	if(((mode.mode==CEASE)&(mode.sub_mode!=CEASE)&(mode.sub_mode!=SLEEP)&(mode.sub_mode!=SELF_TEST)))	
		return;
	
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
				if(((mode.mode==CHARGEING))&(!Dis_TimeSet))
					return;
				if(Dis_TimeSet)
					{
						if(!Dis_H_M_Glint)
							{
								Dis_Hour_Set++;
								if(Dis_Hour_Set>23)
									Dis_Hour_Set=0;
							}
						else
							{
								Dis_Min_Set++;
								if(Dis_Min_Set>59)
									Dis_Min_Set=0;
							}
					}

				//如果处于自测程序的音量测试阶段，则进行音量加处理
				else if((mode.sub_mode==SELF_TEST)&(mode.test_item==TST_VOCIE)&(mode.test_step==2))
					{
						voice_level++;
						if(voice_level>0xef)
							voice_level=0xef;
						Set_Voice_Level(voice_level);
						mode.test_step=1;
					}
				//如果处于自测程序的回充测试阶段，则进行此处理
				else if((mode.sub_mode==SELF_TEST)&(mode.test_item==TST_DOCK)&(mode.test_step==1))
					{
						if((remote_info.rece_ir==RECE_MID)&(!m_hw_test))
							{
								m_hw_test=true;
								Send_Voice(VOICE_VOLUME_2);
								Dis_YBS=true;
							}
					}
				else 
					Slam_Data.dipan_req=DIPAN_REQ_FORWORD;
				remote_info.key_time=giv_sys_time;
				break;
			case REMOTE_KEY_LEFT:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				
				/////机器处于充电状态且未进入时间预约设置模式，退出
				if((mode.mode==CHARGEING)&(!Dis_TimeSet))
					return;
				
				if(Dis_TimeSet)
					{
						Dis_H_M_Glint^=1;
					}
				else if((mode.sub_mode==SELF_TEST)&(mode.test_item==TST_DOCK)&(mode.test_step==1))
					{
						if((remote_info.rece_ir==RECE_LEFT)&(!l_hw_test))
							{
								l_hw_test=true;
								Send_Voice(VOICE_VOLUME_2);
								Dis_Guihua=true;
							}
					}
				else
					Slam_Data.dipan_req=DIPAN_REQ_LEFT;
				
				remote_info.key_time=giv_sys_time;
				break;
			case REMOTE_KEY_RIGHT:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				/////机器处于充电状态且未进入时间预约设置模式，退出
				if((mode.mode==CHARGEING)&(!Dis_TimeSet))
					break;
				
				if(Dis_TimeSet)
					{
						Dis_H_M_Glint^=1;
					}
				/////机器处于回充测试状态，按右键给右红外测试
				else if((mode.sub_mode==SELF_TEST)&(mode.test_item==TST_DOCK)&(mode.test_step==1))
					{
						if((remote_info.rece_ir==RECE_RIGHT)&(!r_hw_test))
							{
								r_hw_test=true;
								Send_Voice(VOICE_VOLUME_2);
								Dis_Zhongdian=true;
							}
					}
				else
					Slam_Data.dipan_req=DIPAN_REQ_RIGHT;

				remote_info.key_time=giv_sys_time;
				break;
			case REMOTE_KEY_BACK:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				/////机器处于充电状态且未进入时间预约设置模式，退出
				if((mode.mode==CHARGEING)&(!Dis_TimeSet))
					break;

				if(Dis_TimeSet)
					{
						if(!Dis_H_M_Glint)
							{
								if(Dis_Hour_Set==0)
									Dis_Hour_Set=23;
								else
									Dis_Hour_Set--;
							}
						else
							{
								if(Dis_Min_Set==0)
									Dis_Min_Set=59;
								else
									Dis_Min_Set--;
							}
					}
				else if((mode.sub_mode==SELF_TEST)&(mode.test_item==TST_VOCIE)&(mode.test_step==2))
					{
						voice_level--;
						if(voice_level<0xe0)
							voice_level=0xe0;
						Set_Voice_Level(voice_level);
						mode.test_step=1;
					}
				//qz add 20180828
				else
					Slam_Data.dipan_req=DIPAN_REQ_BACK;

				remote_info.key_time=giv_sys_time;
				break;

			////////模式性按键///////
			////////模式性按键///////
			case REMOTE_KEY_DOCK:
				/////机器处于充电状态，退出
				if(mode.mode==CHARGEING)
					break;
				////机器出于时间预约设置中，退出
				
				Slam_Data.dipan_req_pre=DIPAN_REQ_DOCK;
				Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
#ifdef REMOTE_DEBUG
				TRACE("Remote Dock key,request dock!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				break;
			case REMOTE_KEY_GUIHUA:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				////机器出于时间预约设置中，退出
				
				Slam_Data.dipan_req_pre=DIPAN_REQ_SWEEP;
				Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
#ifdef REMOTE_DEBUG
				TRACE("Remote GUIHUA key,request guihua!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				break;
			case REMOTE_KEY_YBS:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				////机器出于时间预约设置中，退出
				if(Dis_TimeSet)
					break;
				Slam_Data.dipan_req_pre=DIPAN_REQ_YBS;
				Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
#ifdef REMOTE_DEBUG
				TRACE("Remote YBS key,request YBS!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				break;
			case REMOTE_KEY_SPOT:
				//机器处于工作状态，退出
				if(mode.status)
					break;
				////机器出于时间预约设置中，退出
				if(Dis_TimeSet)
					break;
				Slam_Data.dipan_req_pre=DIPAN_REQ_SPOT;
				Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
#ifdef REMOTE_DEBUG
				TRACE("Remote SPOT key,request SPOT!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				break;
			case REMOTE_KEY_SLEEP:
				////机器处于时间设置模式时，按下休眠键，为取消预约动作

				/////机器处于充电状态，且未进入时间预约设置模式，退出
				if(mode.mode==CHARGEING)
					break;
					
				////机器处于工作状态，进入休眠
				if(mode.status)
					{
						Send_Voice(VOICE_VOLUME_3);
						Init_Cease();
						Slam_Data.dipan_req=DIPAN_REQ_STOP;
					}
				////机器处于待机状态，进入休眠
				if((mode.mode==CEASE)&(mode.sub_mode==CEASE))
					{
						Send_Voice(VOICE_VOLUME_3);
						Init_Sleep();
						Slam_Data.dipan_req=DIPAN_REQ_SLEEP;
					}
				///机器处于休眠状态，唤醒待机
				else if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))	
					{
						Send_Voice(VOICE_VOLUME_3);
						Init_Cease();
						Slam_Data.dipan_req=DIPAN_REQ_STOP;						
					}
				break;
			case REMOTE_KEY_OK:
				////机器出于时间预约设置中，退出时间预约设置。
			
				if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))		//休眠模式，唤醒
					{
						Send_Voice(VOICE_VOLUME_3);
						Init_Cease();
						break;
					}
				if(mode.status)										//工作状态，停止
					{
						Slam_Data.dipan_req=DIPAN_REQ_STOP;
#ifdef REMOTE_DEBUG
						TRACE("Remote OK key,request stop!\r\n");
						TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
						//qz add 2018028
						//如果处于老化模式，退出老化的显示状态
					   		{
								dc_nobat_run=false;
					   		}
					}
				else												//非工作状态，开始自动清扫
					{
						Slam_Data.dipan_req_pre=DIPAN_REQ_SWEEP;
						Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
#ifdef REMOTE_DEBUG
						TRACE("Remote OK key,request sweep!\r\n");
						TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
					}
				
				break;

			////////时间预约功能////////
			////////时间预约功能////////
			case REMOTE_KEY_TIME:
				if(mode.status)
					break;
				if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))
					break;
				break;
			case REMOTE_KEY_PREEEN:
				if(mode.status)
					break;
				if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))
					break;
				break;
			case REMOTE_KEY_SET:
				if(mode.status)
					break;
				if(Dis_TimeSet)
					{
						if(timeorpreen==1)
							{
								temp_data1=Rtc_time/86400;		//当前周几
								Rtc_time=temp_data1*86400+Dis_Hour_Set*3600+Dis_Min_Set*60;
								WriteRtcTime();
								Dis_TimeSet=false;
								Dis_H_M_Glint=false;
								timeorpreen=0;
							}
						else if(timeorpreen==2)
							{
								Slam_Data.preen_hour=Dis_Hour_Set;
								Slam_Data.preen_min=Dis_Min_Set;
								Slam_Data.preen_on=true;
								Slam_Data.preen_off=false;
								Dis_TimeSet=false;
								Dis_H_M_Glint=false;
								timeorpreen=0;
								
								for(i=0;i<PREEN_LENGTH;i++)
									{
										Preen_Data[i].Preen_Hour=Dis_Hour_Set;	//预约小时
										Preen_Data[i].Preen_Min=Dis_Min_Set;	//预约分钟
										Preen_Data[i].Preen_Week_Num=0X00;		//不重复
										Preen_Data[i].Flag=1;
										if(Preen_Data[i].Preen_Week_Num)
											Preen_Data[i].Cycle=1;
										else
											Preen_Data[i].Cycle=0;
									}

								if(!WritePreenData())		//设置预约
									//Send_Voice(VOICE_PREEN_SET_OK);
								Send_Voice(VOICE_VOLUME_3);
							}
					}
				
				//Send_Voice(VOICE_KEY_PRESS);
			break;
	}
}


