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
	static u8 timeorpreen=false;	//��������ʱ�����û���ԤԼ���á�1Ϊʱ������,2ΪԤԼ����
	u8 temp_data1,i;
	//��������CEASE������ģʽ,�˳�

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
	
	//�������ڴ��Ϳ���δ��,�˳�
	if((mode.mode==CHARGEING)&(mode.sub_mode==SWITCHOFF))
		return;

	
	switch (remote_info.remote_key)
		{
			///////�����԰���///////
			///////�����԰���///////
			case REMOTE_KEY_FORWORD:
				//�������ڹ���״̬���˳�
				if(mode.status)
					break;
				
				/////�������ڳ��״̬��δ����ʱ��ԤԼ����ģʽ���˳�
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

				//��������Բ������������Խ׶Σ�����������Ӵ���
				else if((mode.sub_mode==SELF_TEST)&(mode.test_item==TST_VOCIE)&(mode.test_step==2))
					{
						voice_level++;
						if(voice_level>0xef)
							voice_level=0xef;
						Set_Voice_Level(voice_level);
						mode.test_step=1;
					}
				//��������Բ����Ļس���Խ׶Σ�����д˴���
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
				//�������ڹ���״̬���˳�
				if(mode.status)
					break;
				
				/////�������ڳ��״̬��δ����ʱ��ԤԼ����ģʽ���˳�
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
				//�������ڹ���״̬���˳�
				if(mode.status)
					break;
				/////�������ڳ��״̬��δ����ʱ��ԤԼ����ģʽ���˳�
				if((mode.mode==CHARGEING)&(!Dis_TimeSet))
					break;
				
				if(Dis_TimeSet)
					{
						Dis_H_M_Glint^=1;
					}
				/////�������ڻس����״̬�����Ҽ����Һ������
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
				//�������ڹ���״̬���˳�
				if(mode.status)
					break;
				/////�������ڳ��״̬��δ����ʱ��ԤԼ����ģʽ���˳�
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

			////////ģʽ�԰���///////
			////////ģʽ�԰���///////
			case REMOTE_KEY_DOCK:
				/////�������ڳ��״̬���˳�
				if(mode.mode==CHARGEING)
					break;
				////��������ʱ��ԤԼ�����У��˳�
				
				Slam_Data.dipan_req_pre=DIPAN_REQ_DOCK;
				Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
#ifdef REMOTE_DEBUG
				TRACE("Remote Dock key,request dock!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				break;
			case REMOTE_KEY_GUIHUA:
				//�������ڹ���״̬���˳�
				if(mode.status)
					break;
				////��������ʱ��ԤԼ�����У��˳�
				
				Slam_Data.dipan_req_pre=DIPAN_REQ_SWEEP;
				Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
#ifdef REMOTE_DEBUG
				TRACE("Remote GUIHUA key,request guihua!\r\n");
				TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
				break;
			case REMOTE_KEY_YBS:
				//�������ڹ���״̬���˳�
				if(mode.status)
					break;
				////��������ʱ��ԤԼ�����У��˳�
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
				//�������ڹ���״̬���˳�
				if(mode.status)
					break;
				////��������ʱ��ԤԼ�����У��˳�
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
				////��������ʱ������ģʽʱ���������߼���Ϊȡ��ԤԼ����

				/////�������ڳ��״̬����δ����ʱ��ԤԼ����ģʽ���˳�
				if(mode.mode==CHARGEING)
					break;
					
				////�������ڹ���״̬����������
				if(mode.status)
					{
						Send_Voice(VOICE_VOLUME_3);
						Init_Cease();
						Slam_Data.dipan_req=DIPAN_REQ_STOP;
					}
				////�������ڴ���״̬����������
				if((mode.mode==CEASE)&(mode.sub_mode==CEASE))
					{
						Send_Voice(VOICE_VOLUME_3);
						Init_Sleep();
						Slam_Data.dipan_req=DIPAN_REQ_SLEEP;
					}
				///������������״̬�����Ѵ���
				else if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))	
					{
						Send_Voice(VOICE_VOLUME_3);
						Init_Cease();
						Slam_Data.dipan_req=DIPAN_REQ_STOP;						
					}
				break;
			case REMOTE_KEY_OK:
				////��������ʱ��ԤԼ�����У��˳�ʱ��ԤԼ���á�
			
				if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))		//����ģʽ������
					{
						Send_Voice(VOICE_VOLUME_3);
						Init_Cease();
						break;
					}
				if(mode.status)										//����״̬��ֹͣ
					{
						Slam_Data.dipan_req=DIPAN_REQ_STOP;
#ifdef REMOTE_DEBUG
						TRACE("Remote OK key,request stop!\r\n");
						TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
						//qz add 2018028
						//��������ϻ�ģʽ���˳��ϻ�����ʾ״̬
					   		{
								dc_nobat_run=false;
					   		}
					}
				else												//�ǹ���״̬����ʼ�Զ���ɨ
					{
						Slam_Data.dipan_req_pre=DIPAN_REQ_SWEEP;
						Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
#ifdef REMOTE_DEBUG
						TRACE("Remote OK key,request sweep!\r\n");
						TRACE("Remote.ir=%d\r\n",remote_info.rece_ir);
#endif
					}
				
				break;

			////////ʱ��ԤԼ����////////
			////////ʱ��ԤԼ����////////
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
								temp_data1=Rtc_time/86400;		//��ǰ�ܼ�
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
										Preen_Data[i].Preen_Hour=Dis_Hour_Set;	//ԤԼСʱ
										Preen_Data[i].Preen_Min=Dis_Min_Set;	//ԤԼ����
										Preen_Data[i].Preen_Week_Num=0X00;		//���ظ�
										Preen_Data[i].Flag=1;
										if(Preen_Data[i].Preen_Week_Num)
											Preen_Data[i].Cycle=1;
										else
											Preen_Data[i].Cycle=0;
									}

								if(!WritePreenData())		//����ԤԼ
									//Send_Voice(VOICE_PREEN_SET_OK);
								Send_Voice(VOICE_VOLUME_3);
							}
					}
				
				//Send_Voice(VOICE_KEY_PRESS);
			break;
	}
}


