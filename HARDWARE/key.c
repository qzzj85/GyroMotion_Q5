//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
#include "test.h"
//=====================˽�ж���====================================


////////////////////////ͷ�ļ�////////////////////////////////////
//////#include <stm32f10x.h>
//////#include "key.h"
//////#include "hardware.h"

///////////////////////˽�б���////////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
//KEY key1,key2,key3,lidi,last_lidi,l_bump,r_bump,l_last_bump,r_last_bump,charge_dc,charge_seat,s_current;
//KEY key1,key2,key3,lidi,l_bump,r_bump,l_last_bump,r_last_bump,charge_dc,charge_seat,s_current;	//QZ:ԭ���ı���
KEY key1,key2,key3,l_lidi,r_lidi,l_bump,r_bump,m_bump,lm_bump,rm_bump,charge_dc,charge_seat,s_current,dust_box,dust_sta;	//QZ:�����������
                                               //hzh  ���������������ײ��key1:ģʽ,key2:��ͣ,key3:�س�
bool key_time;
											   
////////////////////////˽�ж���//////////////////////////////////
#define HIGH_KEY_NUM 	4
#define	LOW_KEY_NUM		11	

KEY *hightaction[HIGH_KEY_NUM] = {&charge_dc,&charge_seat,&dust_box,&dust_sta};    //����Ч�Ŀ���������
//KEY *lowaction[6]  =  {&key1,&key2,&key3,&lidi,&lidi,	//QZ:MASK
KEY *lowaction[LOW_KEY_NUM]  =  {&key1,&key2,&key3,&l_lidi,&r_lidi,
                      &s_current,&l_bump,&r_bump,&m_bump,&lm_bump,&rm_bump};    //����Ч�Ŀ���������
                      															//qz add 20181120
///////////////////////����ʵ��////////////////////////////////////
/*************************************************************
��ʼ�������ͷ�����
*************************************************************/
void init_key(void)
{
	u8 x;
//////////////////�����ʼ����������״̬��ȡ����///////////////
   key1.read_start 			= Read_Key1;
   key2.read_start 			= Read_Key2;
   key3.read_start 			= Read_Key3;
   l_lidi.read_start 		= Read_L_LIDI;	//QZ: ADD
   l_lidi.MAX_CHECK_TIME	= 10000;		//qz modify 20181210 0.5s-->1.5s
   r_lidi.read_start 		= Read_R_LIDI;	//QZ: ADD
   r_lidi.MAX_CHECK_TIME	= 10000;		//qz modify 20181210 0.5s-->1.5s
   charge_dc.read_start 	= Read_Charge_Dc;
   charge_seat.read_start 	= Read_Charge_Seat;
   dust_box.read_start		= Read_Dust_Box;
   dust_box.MAX_CHECK_TIME	= 30000;		//3s
   l_bump.read_start 		= Read_L_Bump;
   l_bump.MAX_CHECK_TIME	= 30000;		//3s
   r_bump.read_start 		= Read_R_Bump;
   r_bump.MAX_CHECK_TIME	= 30000;		//3s
   lm_bump.read_start		= Read_LM_Bump;
   lm_bump.MAX_CHECK_TIME	= 30000;
   rm_bump.read_start		= Read_RM_Bump;
   rm_bump.MAX_CHECK_TIME	= 30000;
   for(x=0; x<HIGH_KEY_NUM; x++)
   {
     hightaction[x]->sign = 0;
	   hightaction[x]->number = 0;
	   hightaction[x]->key = 0;
	   hightaction[x]->check_step=0;	//qz add 20180601
	   hightaction[x]->fail=false;		//qz add 20180601
   }
   for(x=0; x<LOW_KEY_NUM; x++)
   {
     lowaction[x]->sign = 1;
	   lowaction[x]->number = 0;
	   lowaction[x]->key = 1;
	   lowaction[x]->check_step=0;	//qz add 20180601
	   lowaction[x]->fail=false;		//qz add 20180601
   }
   key_time = false;
   	 
   key1.name = 1;
   key2.name = 2;
   key3.name = 3;

#ifdef DEBUG_INIT
	TRACE("Key init OK!\r\n");
#endif

}

/************************************************************
��ȡ�����ļ�ֵ
************************************************************/
/*--------------KEY ����------------------
KEY1:ģʽ����S
KEY2:��/ͣ����
KEY3:�س䰴��
KEY1(ģʽ): 
1,	�����̰����л�ģʽ,����ӦLED������
2,	����3S+KEY3����3S�������Բ�ģʽ��
3,	��������6S�������ϻ����ԣ��ܻ����ԣ���
  	����6S+KEY2����6S�����DS1307���ݲ�������

KEY2(��/ͣ)
1��	�����̰����ϱ�ģʽ����
	�����SLEEPģʽ���˳�SLEEP������CEASE��
	�����ERRģʽ���˳�ERR������CEASE��
	�����DC��磬�ϱ����ε�����ߡ�
2��	��������3s������WIFI

KEY3(�س�)
1��	�����̰����ϱ��س䣨����г��⣩
2,	����3S+KEY1����3S�������Բ�ģʽ��
    ��������3s�����볧�⡣

------------------------------------------*/

void AutoReadKey(void)
{  
   //key_time 10msһ�ν���
	if((key_time != false)&(mode.sub_mode!=SUBMODE_CHARGE_SWITCHOFF))			//����ģʽ����SWITCHOFFʱ,��������
	{
		key_time = false;
		//read_key(&key1);		//&key1   ȡ�ñ���key1��ָ��				 
		read_key(&key2);	 
//		read_key(&key3);
#ifdef 		GYRO_PWM_RUN_STATE
						if ((action.sign == 1)&&(r_rap.ori == FRONT)&&(l_rap.ori == FRONT)){
									  if ( READ_GYRO_PWM_RUN_PIN ) GYRO_PWM_RUN_0;
						}
						else {
							 if (!( READ_GYRO_PWM_RUN_PIN ) )  GYRO_PWM_RUN_1;
						}
#endif	

		switch(Read_Button_Key(&key2))
			{
				case 1:
					key2.press=false;

					//if((mode.mode==MODE_CEASE)&(mode.sub_mode==FACTORY_TEST)&(!mode.factory_burnning))		//qz add 20181107
					//	break;
					switch(mode.mode)
						{
							case MODE_CHARGE:
								if(mode.sub_mode==SUBMODE_CHARGE_SWITCHOFF)
									break;
								else if(power.charge_dc)
									{
										Send_Voice(VOICE_ERROR_DC_EXIST);
									}
								else	//��������
									{
										//��Ҫ�����˳����������
										Init_Quit_Charging(SWEEP_METHOD_GUIHUA);
									}
								break;
							case MODE_CEASE:
								switch(mode.sub_mode)
									{
										case SUBMODE_CEASE:
#ifdef TUYA_WIFI
											Reset_Map();
#endif	
											Init_First_Sweep(0);
											//Init_Right_YBS(1);	
											//Init_Left_YBS(1);
											//Init_RunTest();
											//Init_SweepTest();
											//Init_Docking();
											break;
										case SUBMODE_ERR:
										case SUBMODE_VIRTUAL_SLEEP:
										case SUBMODE_SLEEP:
											Init_Cease();
											break;
										default:
											break;
									}
								break;
							case MODE_SWEEP:
							case MODE_YBS:
							case MODE_SHIFT:
							case MODE_PASS2INIT:
							case MODE_EXIT:
							case MODE_SPOT:
								Init_Cease();
								Send_Voice(VOICE_SWEEP_STOP);
#ifdef TUYA_WIFI  
								stream_stop(0,256);// ֹͣ��ͼ    WIFI_STREAM_ENABLE
#endif	
								break;
							case MODE_DOCK:
								Send_Voice(VOICE_DOCK_STOP);
								Init_Cease();
								break;
							case TEST:
								if((mode.sub_mode==RUN_TEST))
									{
										mode.step=0;
										mode.test_step++;
									}
								break;
						}
					 break;
				case 2:
					if((mode.mode==MODE_CEASE)&(mode.sub_mode==SUBMODE_FACT_TEST)&(!mode.factory_burnning))		//qz add 20181107
						break;

						if(wifi_clean)
							{
								wifi_clean=false;
							}
						key2.long_press=false;
				break;
				case 3:
					key2.long_long_press=false;
					//qz add 20181026
					//���DS1307������
					//if(key3.long_long_press)
						{
							//WriteBatInitFlash();
							//NVIC_GenerateSystemReset();
						}
				break;
				default:
					break;
			}

#if 0
		 switch(Read_Button_Key(&key3))
		 {
			 case 1:
				 key3.press=false;
			 	if((mode.mode==MODE_CEASE)&(mode.sub_mode==FACTORY_TEST)&(!mode.factory_burnning))		//qz add 20181107
					 break;
				 
			 	 //qz mask 20180902
			 	 if((mode.sub_mode==SUBMODE_ERR)|(mode.sub_mode==SUBMODE_QUITCHARGE)|(mode.sub_mode==SELF_TEST)) //����ģʽ��������
				 	break;
				 if(mode.mode==MODE_CHARGE)					 //����У����跢��
				 	break;
				 Slam_Data.dipan_req_pre=DIPAN_REQ_DOCK;
				 Slam_Data.dipan_req=Slam_Data.dipan_req_pre;		 //�س䰴��,����Ҫ������/ͣ����,������ֱ�ӽ���س�
				 if(!Dis_AllFlagOn)
					 Dis_AllFlagOn=true;
#ifdef SLEEP_SUBMODE
				 if((mode.mode==MODE_CEASE)&(mode.sub_mode==SUBMODE_SLEEP))	 //qz add 20180725:�������������ģʽ,�������
					 {
						 Init_Cease();
						 return;
					 }
#endif
				 if(mode.mode!=MODE_DOCK)
				 	{
						Send_Voice(VOICE_DOCK_START);
						Init_Docking();
				 	}
				 else
				 	{
				 		Send_Voice(VOICE_DOCK_STOP);
						Init_Cease();
				 	}
				 break;
			case 2:
				key3.long_press=false;
				if((mode.mode==MODE_CEASE)&(mode.sub_mode==FACTORY_TEST)&(!mode.factory_burnning))	   //qz add 20181107
					break;
				if((!key1.press)&(!key2.press)&(!key1.long_press)&(!key1.long_long_press)&(!key2.long_press)&(!key2.long_long_press))
					{
						Slam_Data.dipan_req=DIPAN_REQ_FACTORY;
						Init_FactoryTest();
					}
				else if(key1.long_press)
					{
						mode.sub_mode=SELF_TEST;
						Init_Test();
						key1.long_press=false;
					}
				break;
			case 3:
			   key3.long_long_press=false;
			   //qz add 20181026
			   //���DS1307������
			   if(key2.long_long_press)
				   {
					   WriteBatInitFlash();
					   NVIC_GenerateSystemReset();
				   }
			   break;
		 }
#endif
	//				read_key(&lidi);	//QZ:ԭ����
//	read_key(&l_lidi);	//QZ: ADD
//	read_key(&r_lidi);	//QZ: ADD 
//	read_key(&dust_box);

	Read_Key_High(&l_bump);			//��Ϊ��ײ�������жϽ����ж��õ�,�����ڴ˴���Ҫ�ж��Ƿ�ص�Ϊ��
	Read_Key_High(&r_bump);
//	Read_Key_High(&lm_bump);
//	Read_Key_High(&rm_bump);				 			
	Clear_BumpValue();

	read_key(&charge_dc);				 
	read_key(&charge_seat);
	}
}
/*****************************************************
���ܣ���ȡ����Ч�İ���
*****************************************************/
void read_key( KEY *key)	//KEY  flag
{
	if(key->key == 1)    //û�а���
		{
			if(key->read_start() == Bit_RESET)
				{
						key->number ++;        //ȥ��	-> ָ��ṹ��(KEY)�еı���(number)
						if(key->number > 5)
						 {
								key->sign = 0;        //ȷ�ϰ���̧��
								key->key = 0;
								key->number = 0;
						 }
				}
			else
				{
						key->number = 0;
				}
		}
	else///////////////�а�������
		{
				if(key->read_start() != Bit_RESET)
					{
						 key->number++;
						 if(key->number > 5)     //����ȥ��
							{
								 key->key = 1;       		//ȷ�ϰ���̧��
	//									 key->sign = 99;      //ȷ�ϰ��°���
								key->sign=1;			//QZ:ADD
								 key->number = 0;
							}
					}
				else
					{
						key->number = 0;
					}

		} 
}

//qz add 20180417	
void Read_Key_High(KEY *key)
{
	if(key->key==0)
		{
			if(key->read_start()!=Bit_RESET) 
				{
					key->number++;
					if(key->number>=3)
						key->key=1;
				}
		}
}

u8 Read_Button_Key(KEY *key)
{
	u8 result=0;
//	static bool long_press=0;
	if((mode.mode==MODE_CHARGE)&(mode.sub_mode==SUBMODE_CHARGE_SWITCHOFF))	//qz add 20180814
		return 0;
	switch(key->check_step)
		{
			case 0:
				if(!key->key)
					{
						key->start_check_time=giv_sys_time;
						key->check_step++;
						key->long_press=false;
						key->long_long_press=false;
						Send_Voice(VOICE_KEY_PRESS);
						
					}
				break;
			case 1:
				if(!key->key)
					{
						if((giv_sys_time-key->start_check_time>30000)&(!key->long_press))
						{
							if((key1.key)&(key3.key)&(key->name==2))		//ֻ������/ͣ������3s�ŷ���WIFI,qz add 20180605
								{
									if(!wifi_clean)
										{
											wifi_clean=true;
											Send_Voice(VOICE_WIFI_RST);			//����WIFI����
										}
								}
							if((key2.key)&(key->name==3))
								{
									Send_Voice(VOICE_VOLUME_4);
								}
							key->long_press=1;
						}
						if((giv_sys_time-key->start_check_time>50000)&(!key->long_long_press))		//qz add 20180605
						{
							key->long_long_press=1;	
							if(key2.key&key3.key&(key->name==1))
								{
									Send_Voice(VOICE_VOLUME_4);
								}
						}

#if 0
						switch(remote_info.remote_key)
							{
								case REMOTE_KEY_DOCK:
									//�����ϻ�ģʽ
									switch(mode.mode)
										{
											case MODE_CHARGE:
												switch(mode.sub_mode)
													{
														case SUBMODE_CHARGE_DC:
															Send_Voice(VOICE_ERROR_DC_EXIST);
															break;
														case SUBMODE_CHARGE_SEAT:
															mode.burning=true;
															Send_Voice(VOICE_VOLUME_2);
															Init_Quit_Charging(SWEEP_METHOD_GUIHUA);
															break;
														case SUBMODE_CHARGE_SWITCHOFF:
															break;
													}
												break;
											default:
												stop_rap();
#ifdef TUYA_WIFI
												Reset_Map();
#endif	
												mode.burning=true;
												Send_Voice(VOICE_VOLUME_2);
												Init_First_Sweep(0);
												break;
										}
									break;
								case REMOTE_KEY_BACK:
									//�����Բ�ģʽ
									Send_Voice(VOICE_VOLUME_2);
									Init_Test();
									break;
								case REMOTE_KEY_FAN:
									//���������ǲ���
									break;
								case REMOTE_KEY_GUIHUA:
									//�����������Ų���
									break;
								case REMOTE_KEY_YBS:
									//����WIFI����ģʽ
									break;
								case REMOTE_KEY_START:
									//���볧��ģʽ
									break;
								case REMOTE_KEY_RIGHT:
									WriteBatInitFlash();
									NVIC_GenerateSystemReset();
									break;
							}
#endif
					}
				if(key->key)
					key->check_step++;
				break;
			case 2:
				if(key->long_long_press)
					{
						key->long_press=0;
						result=3;
					}
				else if(key->long_press)
					{
						result=2;
					}
				else
					{
						key->press=true;	//qz add 20180801
						result=1;
					}
				key->check_step=0;
//				long_press=0;
				break;					
		}
	return result;
}








