//=====================头文件====================================
#include "AAA-Include.h"
#include "test.h"
//=====================私有定义====================================


////////////////////////头文件////////////////////////////////////
//////#include <stm32f10x.h>
//////#include "key.h"
//////#include "hardware.h"

///////////////////////私有变量////////////////////////////////////
////////////////////////全局变量//////////////////////////////////
//KEY key1,key2,key3,lidi,last_lidi,l_bump,r_bump,l_last_bump,r_last_bump,charge_dc,charge_seat,s_current;
//KEY key1,key2,key3,lidi,l_bump,r_bump,l_last_bump,r_last_bump,charge_dc,charge_seat,s_current;	//QZ:原来的变量
KEY key1,key2,key3,l_lidi,r_lidi,l_bump,r_bump,m_bump,lm_bump,rm_bump,charge_dc,charge_seat,s_current,dust_box,dust_sta;	//QZ:增加左右离地
                                               //hzh  李工新样机增加中碰撞，key1:模式,key2:启停,key3:回充
bool key_time;
											   
////////////////////////私有定义//////////////////////////////////
#define HIGH_KEY_NUM 	4
#define	LOW_KEY_NUM		11	

KEY *hightaction[HIGH_KEY_NUM] = {&charge_dc,&charge_seat,&dust_box,&dust_sta};    //高有效的开关量输入
//KEY *lowaction[6]  =  {&key1,&key2,&key3,&lidi,&lidi,	//QZ:MASK
KEY *lowaction[LOW_KEY_NUM]  =  {&key1,&key2,&key3,&l_lidi,&r_lidi,
                      &s_current,&l_bump,&r_bump,&m_bump,&lm_bump,&rm_bump};    //低有效的开关量输入
                      															//qz add 20181120
///////////////////////函数实体////////////////////////////////////
/*************************************************************
初始化按键和蜂鸣器
*************************************************************/
void init_key(void)
{
	u8 x;
//////////////////下面初始化开关量的状态读取函数///////////////
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
读取按键的键值
************************************************************/
/*--------------KEY 规则------------------
KEY1:模式按键S
KEY2:启/停按键
KEY3:回充按键
KEY1(模式): 
1,	单独短按。切换模式,且响应LED灯亮起。
2,	长按3S+KEY3长按3S。进入自测模式。
3,	单独长按6S，进入老化测试（跑机测试）。
  	长按6S+KEY2长按6S，清空DS1307数据并重启。

KEY2(启/停)
1，	单独短按，上报模式请求。
	如果在SLEEP模式，退出SLEEP，进入CEASE；
	如果在ERR模式，退出ERR，进入CEASE；
	如果在DC充电，上报“拔掉充电线”
2，	单独长按3s。重置WIFI

KEY3(回充)
1，	单独短按，上报回充（充电中除外）
2,	长按3S+KEY1长按3S。进入自测模式。
    单独长按3s，进入厂测。

------------------------------------------*/

void AutoReadKey(void)
{  
   //key_time 10ms一次进入
	if((key_time != false)&(mode.sub_mode!=SUBMODE_CHARGE_SWITCHOFF))			//当子模式处于SWITCHOFF时,不处理按键
	{
		key_time = false;
		//read_key(&key1);		//&key1   取得变量key1的指针				 
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
								else	//充电座充电
									{
										//需要补充退出充电座代码
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
								stream_stop(0,256);// 停止地图    WIFI_STREAM_ENABLE
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
					//清空DS1307的数据
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
			 	 if((mode.sub_mode==SUBMODE_ERR)|(mode.sub_mode==SUBMODE_QUITCHARGE)|(mode.sub_mode==SELF_TEST)) //错误模式，不进入
				 	break;
				 if(mode.mode==MODE_CHARGE)					 //充电中，无需发送
				 	break;
				 Slam_Data.dipan_req_pre=DIPAN_REQ_DOCK;
				 Slam_Data.dipan_req=Slam_Data.dipan_req_pre;		 //回充按键,不需要经过启/停按键,按键后直接进入回充
				 if(!Dis_AllFlagOn)
					 Dis_AllFlagOn=true;
#ifdef SLEEP_SUBMODE
				 if((mode.mode==MODE_CEASE)&(mode.sub_mode==SUBMODE_SLEEP))	 //qz add 20180725:如果处于休眠子模式,进入待机
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
			   //清空DS1307的数据
			   if(key2.long_long_press)
				   {
					   WriteBatInitFlash();
					   NVIC_GenerateSystemReset();
				   }
			   break;
		 }
#endif
	//				read_key(&lidi);	//QZ:原来的
//	read_key(&l_lidi);	//QZ: ADD
//	read_key(&r_lidi);	//QZ: ADD 
//	read_key(&dust_box);

	Read_Key_High(&l_bump);			//因为碰撞按键由中断进行判断置低,所以在此处需要判断是否回弹为高
	Read_Key_High(&r_bump);
//	Read_Key_High(&lm_bump);
//	Read_Key_High(&rm_bump);				 			
	Clear_BumpValue();

	read_key(&charge_dc);				 
	read_key(&charge_seat);
	}
}
/*****************************************************
功能：读取高有效的按键
*****************************************************/
void read_key( KEY *key)	//KEY  flag
{
	if(key->key == 1)    //没有按键
		{
			if(key->read_start() == Bit_RESET)
				{
						key->number ++;        //去抖	-> 指向结构体(KEY)中的变量(number)
						if(key->number > 5)
						 {
								key->sign = 0;        //确认按键抬起
								key->key = 0;
								key->number = 0;
						 }
				}
			else
				{
						key->number = 0;
				}
		}
	else///////////////有按键按下
		{
				if(key->read_start() != Bit_RESET)
					{
						 key->number++;
						 if(key->number > 5)     //按键去抖
							{
								 key->key = 1;       		//确认案件抬起
	//									 key->sign = 99;      //确认按下按键
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
							if((key1.key)&(key3.key)&(key->name==2))		//只有在启/停键长按3s才发清WIFI,qz add 20180605
								{
									if(!wifi_clean)
										{
											wifi_clean=true;
											Send_Voice(VOICE_WIFI_RST);			//重置WIFI语音
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
									//进入老化模式
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
									//进入自测模式
									Send_Voice(VOICE_VOLUME_2);
									Init_Test();
									break;
								case REMOTE_KEY_FAN:
									//进入陀螺仪测试
									break;
								case REMOTE_KEY_GUIHUA:
									//进入语音播放测试
									break;
								case REMOTE_KEY_YBS:
									//进入WIFI测试模式
									break;
								case REMOTE_KEY_START:
									//进入厂测模式
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








