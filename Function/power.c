//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================

////////////////////////ͷ�ļ�////////////////////////////////////
//////#include "power.h"
//////#include "soft.h"	
//////#include "hardware.h"
//////#include "ad.h"
//////#include "pwm.h"
//////#include "lcdkey.h"
//////#include "systime.h"
#include "temp.h"	
//////#include "key.h"		
//////#include "function.h"
//////#include "err.h"
//////#include "rtc.h"
////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////

#define 	C_KP			0.5//0.025
#define		C_KI			0.1//0.1
#define		C_KD			0.01//0.01//0.0125//0.0125

POWER power ;
BATTERY	Battery;
CHARGE_DATA charge_data;	//qz add 20180522
bool CHARGE_READY;			//qz add 20180522

bool POWER_ready;
bool VOL_TEMP_ready;
bool VOL_TEMP_READY_MY;
bool get_dispow_flag;
u16  jt_chargecurrent;   //��̬ʱ�������ĵ���
u16  battery_chargecurrent;//���5���ڵ�ƽ������
u16  battery_voltage;    //���1��ĵ�ѹ
unsigned char battery_low_beep_flag = 1;
u16  battery_temp;       //���1����¶�
u16  l_current;          //���ֵ���
u16  r_current;          //���ֵ���
u16  dust_current;       //�ҳ��������
u16  m_current;          //��ɨ����	

u16  battery_chargecurrent_1s;//���1���ڵ�ƽ������
u16  battery_voltage_1s;    //���1��ĵ�ѹ
u16  battery_temp_1s;       //���1����¶�
u16  l_current_1s;          //���ֵ���
u16  r_current_1s;          //���ֵ���
u16  dust_current_1s;       //�ҳ��������
u16  m_current_1s;          //��ɨ����
bool dc_nobat_run=false;	//���õ��,DCֱ���ܻ���־λ

u16  l_current_50ms=0;
u16	 r_current_50ms=0;
u32  sample_number_50ms=0;
u32  l_temp_curr_50ms=0;
u32  r_temp_curr_50ms=0;
bool ms_curr_flag=false;	//���뼶����׼���ñ�־
u32 l_curr_buf[10]={0,0,0,0,0,0,0,0,0,0};
u32 r_curr_buf[10]={0,0,0,0,0,0,0,0,0,0};


bool  flag_full = false;			//��س������־
u8  bat_recal = false;				//������������س��־
///////////////////////˽�б���////////////////////////////////////
uint16 piv_current;
uint16 piv_voltage;
u32    real_chg_current;
u32    real_bat_voltage;

///////////////////////ȫ�ֺ���////////////////////////////////////	
///////////////////////˽�к���////////////////////////////////////
///////////////////////����ʵ��////////////////////////////////////
/**************************************************************
�����ܣ���ʼ����ص�Դ
���룺��
�������
˵������ʼ�����Դ�йص�io�������жϵ���Ƿ���NTC��
**************************************************************/
void init_power (void)
{
////////////  battery_temp = account_current(BATTERY_TEMP);
  power.voltage = account_current(BATTERY_VOLTAGE);


  battery_voltage = power.voltage;
  //battery_temp = power.temp;
 #if 1//shftemp   
  jt_chargecurrent = account_current(CHARGE_CURRENT);
   #else
 jt_chargecurrent = 800;
 #endif
  battery_chargecurrent = jt_chargecurrent;//���5���ڵ�ƽ������	
  power.current = 0;
  power.charge_dc = 0;
  power.charge_seat = 0;
  power.pwm = 0;
  power.low2dock=false;		//qz add 20180625
  Init_Charge_Data();		//qz add 20180522
  flag_full=false;			//qz add 20180625
#ifdef DEBUG_INIT
  TRACE("Power init OK!\r\n");
#endif
}

void Battery_Reinit(void)
{
  	s8 temp_data;
	//�������У��ʱ,��Ϊ����������ĵ�ѹ�����Եȵ�������������ɺ�(��������)
	//�ٿ�ʼʹ�õ�ѹ�ٷֱȼ�������ͷŵ���,�ڴ�֮ǰ,�ϴ���������ĵ����ٷֱ�Ϊ���԰ٷֱ�
	//���԰ٷֱ���APP_BAT_HANDLE������,���״̬Ϊ0,�ǳ��״̬Ϊ100
	//qz add 20181101
	if(giv_sys_time-Battery.start_time<50000)
		return;
	//qz add end
	
#ifdef DC_NOBAT_RUN
	if((Battery.reinit)&(!dc_nobat_run))
#else
	if((Battery.reinit))
#endif
		{
			//if(mode.mode)
						//qz add 20180615
#ifdef NEW_PWR_CAL
			Battery.BatteryCapability=MAH2600; 	//qz modify 20180801:ԭ��ΪMAH2600//2400mAh
#else	
			Battery.BatteryCapability=MAH1800;
#endif
			temp_data=Get_APPBat();
#ifdef NEW_PWR_CAL
			Battery.BatteryFDCap=(100-temp_data)*MAH2600/100;
#else
			Battery.BatteryFDCap=(100-temp_data)*MAH1800/100;
#endif
			Battery.BatteryChargeForbid=0;
			WriteBatteryCapability();
			WriteFDCap();
			Battery.reinit=false;
		}		
}

void Battery_Data_Init(void)
{
#ifdef DS1307
	u8 data=0;
	if(DS1307_Read_Backup(FLAG_BACKUP_ADDR, &data))
		return;
	if(data!=0x5A)												//��һ�ο�������DS1307���û����
		{
			DS1307_Data_Reinit();
			Battery.reinit=true;		//��ص������ñ�־
#ifdef DEBUG_INIT
			TRACE("DS1307 Data Reinit!\r\n");
#endif
		}
#else
	u16 data1;
	PWR_BackupAccessCmd(ENABLE);
	data1=BKP_ReadBackupRegister(BAT_REINIT_BKP);
	if(data1!=0xAA55)										//��һ�ο������ߵ��û����
		{
			BKP_WriteBackupRegister(BAT_REINIT_BKP, 0xAA55);		//�������־
			BKP_WriteBackupRegister(VOICE_BACKUP_BKP, 0x00EC);		//��������(Ĭ�ϵ�13������ԤԼʱ��Ϊû��
			BKP_WriteBackupRegister(PREEN_BACKUP_BKP, 0x0000);		//ԤԼ����Ϊû��
			BKP_WriteBackupRegister(MODE_BACKUP_BKP, 0x0101);		//BKP_DR4����ģʽ���棬��16λΪ��ģʽ����16λΪ��ģʽ��
			BKP_WriteBackupRegister(BATCAP_HADDR_BKP, 0x0);			//BKP_DR5���ڵ�������ĸ�16λ
			BKP_WriteBackupRegister(BATCAP_LADDR_BKP, 0x0);			//BKP_DR6���ڵ�������ĵ�16λ
			BKP_WriteBackupRegister(BATFDCAP_HADDR_BKP, 0x0);			//BKP_DR7���ڵ�طŵ����ĸ�16λ
			BKP_WriteBackupRegister(BATFDCAP_LADDR_BKP, 0x0);			//BKP_DR8���ڵ�طŵ����ĵ�16λ
			BKP_WriteBackupRegister(BATRECAL_BACKUP_BKP, 0X0);			//BKP_DR9���ڵ��RECAL��־,qz add 20181101
			Battery.BatteryCapability=0;
			Battery.BatteryFDCap=0;
			Battery.reinit=true;
			Battery.bat_recal=0;
#ifdef DEBUG_INIT
			TRACE("BKP Data Reinit!\r\n");
#endif
		}
	PWR_BackupAccessCmd(DISABLE);
#endif
	if(!Battery.reinit) 									//��ص�������Ҫ����
		{
			Battery.BatteryCapability=ReadBatteryCapability();
			Battery.BatteryFDCap=ReadFDCap();
			if((Battery.BatteryFDCap & 0x80000000) != 0)
				{
					Battery.BatteryChargeForbid = 1;
				}
			else
				{
					Battery.BatteryChargeForbid = 0;   
				}
			Battery.BatteryFDCap &= 0x7fffffff;
			if(Battery.BatteryCapability == 0)				//�����ص�������Ϊ0����ʾ��Ҫ���ݵ�ѹ���³�ʼ������
				{
					Battery.reinit=true;					//������³�ʼ��������־��1
#ifdef DEBUG_INIT
					TRACE("Battery.reinit=%d\r\n",Battery.reinit);
#endif
				}
		}
		
	Battery.bat_recal=(u8)(ReadBatRecalFlash());
	if((Battery.bat_recal!=0)&(Battery.bat_recal!=1))
		{
			Battery.bat_recal=0;
			WriteBatRecalFlash((u16)(Battery.bat_recal));
		}
	Battery.UsePowerTime = Rtc_time;
	Battery.start_time=giv_sys_time;
#ifdef DEBUG_INIT
	TRACE("Battery.recal=%d\r\n",Battery.bat_recal);
#endif
}

/*************************************************************
���ܣ� ������
���룺��
�������
*************************************************************/
void chargeing(void)
{
	if((power.charge_dc == 0)&&(power.charge_seat == 0))
			{
				 power.pwm = 0;
				 power.step = 0;
				 disable_pwm(CHARGE_PWM);
				 return;
			}
	if( POWER_ready != true )
			{
				 return;
			}
	POWER_ready = false;
	ChargeControl();	
}
/************************************************
���ܣ�������
************************************************/
#if 1
	void ChargeControl(void)
	{
	
	 static uint8  flag_full = false;			//��س������־		
		//////////////////������//////////////////////////////////////
		
		piv_current = account_current(CHARGE_CURRENT);	 //������ʱ�ĵ���
		piv_voltage = account_current(BATTERY_VOLTAGE);   //������ʱ�ĵ�ѹ
		real_chg_current= (u32)(piv_current*CHG_CUR_CNT);
		real_bat_voltage= (u32)(piv_voltage*CHG_VOL_CNT);
		
		switch(power.step)
		{
			///////��ʼ���ǰ��׼��/////////////
			case 0:
				power.time = giv_sys_time;
				power.step = 1;
				disable_pwm(CHARGE_PWM);
				return;
			/////////////////////////���ϵ缫10���ӣ��쿴����Ƿ����
			case 1:
				if((giv_sys_time - power.time) > 20000)//�Ӵ��ϵ�2����
				{
					//if( battery_temp_1s < 355)
					if((battery_temp_1s<355)|(battery_temp_1s>3902))	//qz modify 20180515
					{
//						giv_sys_err = 25;			//qz mask 20180515

						//qz add 20180515
						error_code=SEND_ERROR_BAT;
						dis_err_code=DIS_ERROR_BAT_DOWN;
						//Send_Voice(VOICE_ERROR_BAT_FAIL);
						//qz add end
						
						Init_Err();
						return ;
					}

					#if 0	//qz mask 20180515
					if(battery_temp_1s > 3902)
					{
//						giv_sys_err = 26;
						Init_Err();
						return ;
					}
					#endif
					jt_chargecurrent = battery_chargecurrent;
					power.step = 2;
					power.temp = battery_temp;//Get_BatteryTemp();
					power.time = giv_sys_time;
					//�����س���־Ϊ1����ֱ�ӽ����4����
	//				if(Battery.BatteryChargeForbid != 0)
	//				{
	//					power.step = 4;
	//				}
					if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > 1827)) //15.8V~1783   16.2V~1827
					{
						power.step = 4;
					}
					else//���������ʱ����ò��ã�����Էŵ���ɵ������ؼ���ʱ����Ҫ���д�������
					{
						Battery.BatteryChargeForbid = 0;	//�����������
					}
	
					flag_full = false;					//
				}
				return; 
				
		//ʹ��180ma���10s���ҵ���¶ȵ���40�ȣ�������Ƿ�����������磬�����������������
			case 2:
			//����������150MA�򽵵�PWMֵ
				if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 372))
				{
					if(power.pwm > 0)
					{
						power.pwm --;						//���������趨����
					}
				}
				else
				{
					
					if(power.pwm < (PWM_CHARGE_MAX-CHARGE_PWM_MASK))
					{
						power.pwm ++;						//����С���趨����
					}
				}
				if(((giv_sys_time - power.time) > 100000)&&(battery_temp > temp[70])) //���ʱ��Ϊ5���Ӳ��ҵ���¶ȵ���40��
				{
					power.step = 3;
					power.time = giv_sys_time;
				}
				if((giv_sys_time - power.time) > 72000000)
				{
					power.step = 3;
					power.time = giv_sys_time;
				}
				break;	
				
		//���������ѹ�Ĺ��̣��жϵ��תΪ����������ǣ�
		//1����س�����С��360ma��
		//2����صľ����¶ȴ���50��
		//3���ڵ���¶ȴ���40�ȵ�����£�����¶�ÿ���������ٶȴ���1.5��
			case 3:
			//�����������趨�����򽵵�PWMֵ  
				if(((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 1986)))		 //800mA
				{
					if(power.pwm > 0)
					{
						power.pwm --;						//���������趨����
					}
				}
				else
				{
					
					if(power.pwm < (PWM_CHARGE_MAX-CHARGE_PWM_MASK))
					{
						power.pwm ++;						//����С���趨����
					}
				}
			//�жϵ�صľ����¶ȴ���50�Ȼ��ߵ�ص�ѹ����21��,���תΪ������	 2369
				if((battery_temp < temp[80])|| (battery_voltage > 1919))  //17.1V~1929	 17.5V~1974 16.9V~1909	17.3~1952  16.8~1896  17~1919
				{
					power.step = 4;
					power.time = giv_sys_time;
				}
			//��صĳ�����С��360ma��Ϊ����Ѿ�����
				if(((battery_chargecurrent < jt_chargecurrent) 
				|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //����С��360ma
				&&((giv_sys_time - power.time) > 600000))
				{
					power.step = 4;
					power.time = giv_sys_time;
				}
			//�ڴ�������3Сʱ���ϣ�ǿ���Խ������Ϊ�䱥״̬��
				if((giv_sys_time - power.time) > 180000000)
				{
					power.step = 4;
					power.time = giv_sys_time;	  
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
				}
			////////ÿ�����ж�һ��////////////////////////
				if(gbv_minute != false)
				{
					gbv_minute = false;
					//piv_temp = Get_BatteryTemp();
				//��صľ����¶ȴ���40�ȣ�
					if(battery_temp < temp[70])
					{
				    //��ص������ٶȴ���1.5��
						//if((piv_temp > power.temp)&&((piv_temp - power.temp) > 3))
						if((battery_temp < power.temp)&&((power.temp - battery_temp) > 50))
						{
							power.step = 4;
							power.time = giv_sys_time;
						}
					}
					//power.temp = piv_temp;
					power.temp = battery_temp;
				}
				break;	
			case 4:
			//����������180MA�򽵵�PWMֵ
	//			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 372))
				if((piv_voltage > 1909)|((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 1986)))
				{
					if(power.pwm > 0)
					{
						power.pwm --;						//���������趨����
					}
				}
				else
				{
					
					if(power.pwm < (PWM_CHARGE_MAX-CHARGE_PWM_MASK))
					{
						power.pwm ++;						//����С���趨����
					}
				}
				////////ÿ�����ж�һ��////////////////////////
				if(gbv_minute != false)
				{
					gbv_minute = false;
	//				if((battery_temp > temp[70])//��صľ����¶�С��40�ȣ�����������100ma
	//				&& ((battery_chargecurrent > jt_chargecurrent)&&((battery_chargecurrent - jt_chargecurrent) > 248))
	//				&& (Battery.BatteryChargeForbid == 0))//��ص�ѹС��15v
	//				{
	//					power.step = 3;
	//					power.time = giv_sys_time; 
	//				}
	//				if(((giv_sys_time - power.time) > 12000000)
	//				  && (battery_temp < temp[70]))
	//				{
	//					Battery.BatteryFDCap = 0;
	//					Battery.BatteryChargeForbid = 1;
	//				}
	//				power.temp = battery_temp;
	
					if(((battery_chargecurrent > jt_chargecurrent)&&((battery_chargecurrent - jt_chargecurrent) < 248)) //100mA(0.02C)
					&& ((giv_sys_time - power.time) > 100000)//10s  ��ֹ��1��ֱ�ӵ���4��ʱ���տ�ʼ��Ϊ���PWMû����������Ϊ0��С��52mA 
					&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
					{
						flag_full = true;
						
						Battery.BatteryFDCap = 0;
						Battery.BatteryChargeForbid = 1;
					}
				//丳����600������Ϊ��س�����
					if(((giv_sys_time - power.time) > 360000000))		//600����
					{
						flag_full = true;
	
						Battery.BatteryFDCap = 0;
						Battery.BatteryChargeForbid = 1;
					}
				//��س����磬PWM�رպ󣬵�ػ��Էŵ磬��ʱ��(����һ����)�󣬵���貹�����
					if((flag_full == true)&&(battery_voltage_1s < 1783))		//15.8V
					{
						power.step = 3;
						flag_full = false;
						power.time = giv_sys_time;	
					}
				}
	
			//��ֹ��ر����廵ʱһֱ��磬���仵���  (2015.4.7)
				if(flag_full == true)
				{
					power.pwm = 0;	
				}
	
				break;
			default :
				power.step = 0; 	
		}
		//if((charge_dc.key == 0)&&(charge_seat.key == 0))		//qz mask 20180515
		if(!(power.charge_dc)&&(!power.charge_seat))	//qz add 20180515
		{
			disable_pwm(CHARGE_PWM);
			power.pwm = 0;
		}
		else
		{
			enable_pwm(CHARGE_PWM,power.pwm);	//ִ��pwm����
		}		
	}
//=======================================================================================================
//=======================================================================================================


//piv_current/4096*3.3/20/0.1R*1000=real_current(unit:mA)
//piv_current*0.402832=real_current(unit:mA)
void ChargeControl_My(void)
{

	//////////////////������//////////////////////////////////////

	piv_current = account_current(CHARGE_CURRENT);	 //������ʱ�ĵ���
	piv_voltage = account_current(BATTERY_VOLTAGE);   //������ʱ�ĵ�ѹ
	charge_data.piv_current_sum+=piv_current;
	charge_data.piv_current_count++;
	charge_data.piv_voltage_sum+=piv_voltage;
	charge_data.piv_voltage_count++;
	switch(power.step)
	{
		///////��ʼ���ǰ��׼��/////////////
		case 0:
			power.time = giv_sys_time;
			power.step = 10;
			disable_pwm(CHARGE_PWM);
			return;

		///////Ϊ��ֹ��翪�����µľ�̬�����������,
		///////���ڴ˵ȴ�40s,���ڵ����忪�����м���
		///////qz add 20180625
		case 10:
			if(giv_sys_time-power.time>400000)
				power.step=1;
			break;
		/////////////////////////���ϵ缫10���ӣ��쿴����Ƿ����
		case 1:
			if((giv_sys_time - power.time) > 20000)//�Ӵ��ϵ�2����
			{
				//if( battery_temp_1s < 355)
				if((battery_temp_1s<355)|(battery_temp_1s>3902))	//qz modify 20180515
				{
					error_code=SEND_ERROR_BAT;
					dis_err_code=DIS_ERROR_BAT_DOWN;
					Init_Err();
					return ;
				}

			#if 0	//qz mask 20180515
				if(battery_temp_1s > 3902)
				{
	//						giv_sys_err = 26;
					Init_Err();
					return ;
				}
			#endif
				jt_chargecurrent = battery_chargecurrent;
#ifdef DEBUG_CHARGE
				TRACE("jt_cur=%dmA\r\n",(u32)(jt_chargecurrent*CHG_CUR_CNT));
#endif
				power.step = 2;
				power.temp = battery_temp;//Get_BatteryTemp();
				power.time = giv_sys_time;

				//�����ص�ѹ����16.2V���Ҳ�������д������磬�������Ĳ�����������
				if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > 1827)) //15.8V~1783   16.2V~1827
				{
					power.step = 4;
				}
				else//���������ʱ����ò��ã�����Էŵ���ɵ������ؼ���ʱ����Ҫ���д�������
				{
					Battery.BatteryChargeForbid = 0;	//�����������
				}

				flag_full = false;					//���δ����
			}
			return; 
			
	//ʹ��180ma���10s���ҵ���¶ȵ���40�ȣ�������Ƿ�����������磬�����������������
		case 2:
		//����������150MA�򽵵�PWMֵ
		Charge_PID_Ctr(360);		//qz modify 150->360:�̶�360mA
			if(((giv_sys_time - power.time) > 100000)&&(battery_temp > temp[70])) //���ʱ��Ϊ5���Ӳ��ҵ���¶ȵ���40��
			{
				power.step = 3;
				power.time = giv_sys_time;
				power.step_time=giv_sys_time;
			}
			if((giv_sys_time - power.time) > 72000000)		//2 hour
			{
				power.step = 3;
				power.time = giv_sys_time;
				power.step_time=giv_sys_time;
			}
			break;	
			
			
	//���������ѹ�Ĺ��̣��жϵ��תΪ����������ǣ�
	//1����س�����С��360ma��
	//2����صľ����¶ȴ���50��
	//3���ڵ���¶ȴ���40�ȵ�����£�����¶�ÿ���������ٶȴ���1.5��
		case 3:
		//�����������趨�����򽵵�PWMֵ
		#if 1
		Charge_PID_Ctr(800);
		
		#endif
//		power.pwm=1625;	
		//�жϵ�صľ����¶ȴ���50�Ȼ��ߵ�ص�ѹ����21��,���תΪ������	 2369
			if((battery_temp < temp[80])|| (battery_voltage > 1861))	//qz modify 20180703:1805 16V 1861:16.5V
			//if((battery_temp < temp[80])|| (battery_voltage > 1919))  //17.1V~1929	 17.5V~1974 16.9V~1909	17.3~1952  16.8~1896  17~1919
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//��صĳ�����С��360ma��Ϊ����Ѿ�����
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //����С��360ma
			&&((giv_sys_time - power.time) > 600000))
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//�ڴ�������3Сʱ���ϣ�ǿ���Խ������Ϊ�䱥״̬��
			if((giv_sys_time - power.time) > 144000000)		//qz modify 144000000 4h
			{
				power.step = 4;
				power.time = giv_sys_time;	  
				//Battery.BatteryFDCap = 0;						//qz mask 20180625
				Battery.BatteryChargeForbid = 1;
			}
		////////ÿ�����ж�һ��////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
				//piv_temp = Get_BatteryTemp();
			//��صľ����¶ȴ���40�ȣ�
				if(battery_temp < temp[70])
				{
				//��ص������ٶȴ���1.5��
					//if((piv_temp > power.temp)&&((piv_temp - power.temp) > 3))
					if((battery_temp < power.temp)&&((power.temp - battery_temp) > 50))
					{
						power.step = 4;
						power.time = giv_sys_time;
					}
				}
				//power.temp = piv_temp;
				power.temp = battery_temp;
			}
			break;	
		case 4:
			Init_Charge_Data();
			power.step++;
		break;
		case 5:
		//����������180MA�򽵵�PWMֵ
			Charge_PID_Ctr(360);	//qz modify 150->360:�̶�360mA
			////////ÿ�����ж�һ��////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
#ifdef DEBUG_CHARGE
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CHG_CUR_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CHG_CUR_CNT));
#endif

				if((((battery_chargecurrent - jt_chargecurrent) < 248)) //100mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	��ֹ��1��ֱ�ӵ���4��ʱ���տ�ʼ��Ϊ���PWMû����������Ϊ0��С��52mA 
				&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
				{

#ifdef DEBUG_CHARGE
					TRACE("because overcurrent out!\r\n");
#endif
					flag_full = true;

					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
				}
			//丳����600������Ϊ��س�����
				if(((giv_sys_time - power.time) > 360000000))		//600����  
				{
					flag_full = true;
#ifdef DEBUG_CHARGE
					TRACE("because Time out!\r\n");
#endif
					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
				}

			//�������ѹ����16.8V,��Ϊ�������
			//qz add 20180710
			//ʵ��1882(16.67V)ʱ,��ؽ��뱣��
			if(((giv_sys_time-power.time>100000)&&(battery_voltage>1880)&&(!flag_full)))
				{
#ifdef DEBUG_CHARGE
					TRACE("Because overvolt out!\r\n");
#endif
					flag_full = true;
				
					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
					{
						Battery.BatteryCapability-=Battery.BatteryFDCap;
						WriteBatteryCapability();	
					}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
				}
			//qz add end
			
			//��س����磬PWM�رպ󣬵�ػ��Էŵ磬��ʱ��(����һ����)�󣬵���貹�����
				if((flag_full == true)&&(battery_voltage_1s < 1783))		//15.8V
				{
					power.step = 3;
					flag_full = false;
					power.time = giv_sys_time;
					Init_Charge_Data();		//qz add 20180522
					Send_Voice(VOICE_CHARGING_START);
				}
			}

		//��ֹ��ر����廵ʱһֱ��磬���仵���  (2015.4.7)
			if(flag_full == true)
			{
				power.pwm = 0;
				Init_Charge_Data();		//qz add 20180522
			}

			break;
		default :
			power.step = 0; 	
	}
	//if((charge_dc.key == 0)&&(charge_seat.key == 0))		//qz mask 20180515
	if(!(power.charge_dc)&&(!power.charge_seat))	//qz add 20180515
	{
		disable_pwm(CHARGE_PWM);
		power.pwm = 0;
		Init_Charge_Data();
	}
	else
	{
		enable_pwm(CHARGE_PWM,power.pwm);	//ִ��pwm����
	}		
}
#endif

void ChargeControl_Volt_My(void)
{

	//////////////////������//////////////////////////////////////

	piv_current = account_current(CHARGE_CURRENT);	 //������ʱ�ĵ���
	piv_voltage = account_current(BATTERY_VOLTAGE);   //������ʱ�ĵ�ѹ
	charge_data.piv_current_sum+=piv_current;
	charge_data.piv_current_count++;
	charge_data.piv_voltage_sum+=piv_voltage;
	charge_data.piv_voltage_count++;
	switch(power.step)
	{
		///////��ʼ���ǰ��׼��/////////////
		case 0:
			power.time = giv_sys_time;
			power.step = 10;
			disable_pwm(CHARGE_PWM);
			return;

		///////Ϊ��ֹ��翪�����µľ�̬�����������,
		///////���ڴ˵ȴ�40s,���ڵ����忪�����м���
		///////qz add 20180625
		case 10:
			if(giv_sys_time-power.time>400000)
			{
				if(battery_voltage_1s<VOL_13V)			//�����ص�ѹС��13V,��������
					{
						power.step=1;
						Battery.BatteryChargeForbid = 0;	//�����������
					}
				else if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > VOL_16_5V))
					{
						power.step=4;
					}
				else
					{
						Battery.BatteryChargeForbid = 0;	//�����������
						power.step=2;
					}
				Init_Charge_Data();
				jt_chargecurrent=battery_chargecurrent;
#ifdef DEBUG_CHARGE
				TRACE("jt_curr=%d\r\n",(u32)(jt_chargecurrent*CHG_CUR_CNT));
#endif
				flag_full = false;					//���δ����
				power.time=giv_sys_time;
			}
			
			break;
		/////////////////////////���ϵ缫10���ӣ��쿴����Ƿ����
		///////��ѹС��13Vʱ������360mA-��̬�����Ļָ����
		case 1:
			Charge_PID_Ctr(360);		//qz modify 150->360:�̶�360mA
			if((giv_sys_time-power.time>100000)&(battery_voltage>VOL_13V))
				{
					power.step++;
					power.time=giv_sys_time;
					Init_Charge_Data();
				}
			break; 		//qz modify return->break 20180823
			
		//ʹ��180ma���10s���ҵ���¶ȵ���40�ȣ�������Ƿ�����������磬�����������������
		case 2:
		//����������150MA�򽵵�PWMֵ
			Charge_PID_Ctr(360);		//qz modify 150->360:�̶�360mA
			if(giv_sys_time-power.time>100000)
				{
					power.step++;
					power.time = giv_sys_time;
					power.step_time=giv_sys_time;
				}
			break;	
			
		//���������ѹ�Ĺ��̣��жϵ��תΪ����������ǣ�
		//1����س�����С��360ma��
		//2����صľ����¶ȴ���50��
		//3���ڵ���¶ȴ���40�ȵ�����£�����¶�ÿ���������ٶȴ���1.5��
		case 3:
		Charge_PID_Ctr(800);
		//�жϵ�صľ����¶ȴ���50�Ȼ��ߵ�ص�ѹ����21��,���תΪ������	 2369
			if((battery_voltage > VOL_16_8V))	//qz modify 20180703:1805 16V 1861:16.5V
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//��صĳ�����С��360ma,����С�������
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //����С��360ma
			&&((giv_sys_time - power.time) > 600000))
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//�ڴ�������4Сʱ���ϣ�ǿ���Խ��������硣
			if((giv_sys_time - power.time) > 144000000)		//qz modify 144000000 4h
			{
				power.step = 4;
				power.time = giv_sys_time;	  
				//Battery.BatteryFDCap = 0;						//qz mask 20180625
				Battery.BatteryChargeForbid = 1;
			}

			break;	
		case 4:
			Init_Charge_Data();
			power.time=giv_sys_time;
			power.step++;
		break;

		//������
		case 5:
#if 0
			Init_Charge_Data();
			power.time=giv_sys_time;
			power.step++;
#else
		//����������180MA�򽵵�PWMֵ
			Charge_PID_Ctr(360);	//qz modify 150->360:�̶�360mA
			////////ÿ�����ж�һ��////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;			
#ifdef DEBUG_CHARGE
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CHG_CUR_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CHG_CUR_CNT));
#endif
				//������С�ھ�̬����+100mA,��ת���ѹ���
				if((((battery_chargecurrent - jt_chargecurrent) < 248)) //100mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	��ֹ��1��ֱ�ӵ���4��ʱ���տ�ʼ��Ϊ���PWMû����������Ϊ0��С��52mA 
				&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
				{
					Init_Charge_Data();
					Battery.BatteryChargeForbid = 1;
					power.step++;
					power.time=giv_sys_time;
					return;
				}
	
				//丳����600������Ϊ��س�����
				if(((giv_sys_time - power.time) > 360000000)		//600����  
					&&(flag_full==false))						//qz add 20180910
					
				{
					flag_full = true;
#ifdef DEBUG_CHARGE
					TRACE("because Time out in step5!\r\n");
#endif
					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,�̵Ƴ���5s.qz modify ����ʱ����
					power.step=7;
					return;
				}

			//�������ѹ����16.8V,��Ϊ�������
			//qz add 20180710
			//ʵ��1882(16.67V)ʱ,��ؽ��뱣��
			//�������ѹ����16.8V,�����ѹ���
			if(((giv_sys_time-power.time>100000)&&(battery_voltage>VOL_16_8V)&&(!flag_full)))
				{
					Init_Charge_Data();
					Battery.BatteryChargeForbid = 1;
					power.step++;
					power.time=giv_sys_time;
					return;
				}
			//qz add end
			}
#endif
		break;

		//��ѹ���
		case 6:
		//����������180MA�򽵵�PWMֵ
			Charge_PID_CtrPwr(VOL_16_8V);	//qz modify 150->360:�̶�360mA
			Battery.BatteryChargeForbid = 1;	//��ѹ���,��ֹ��������
			////////ÿ�����ж�һ��////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
#ifdef DEBUG_CHARGE
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CHG_CUR_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CHG_CUR_CNT));
#endif

#if 1
				if((((battery_chargecurrent - jt_chargecurrent) < 62)) //25mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	��ֹ��1��ֱ�ӵ���4��ʱ���տ�ʼ��Ϊ���PWMû����������Ϊ0��С��52mA 
				&& (flag_full == false))
				{

#ifdef DEBUG_CHARGE
					TRACE("because overcurrent out in step6!\r\n");
#endif
					flag_full = true;

					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,�̵Ƴ���5s.qz modify ����ʱ����
					power.step++;
				}
#endif
			//丳����120���ӻ��߳���ѹ����17V��Ϊ��س�����
				if((((giv_sys_time - power.time) > 72000000)|(battery_voltage>VOL_17V))&&(!flag_full))		//120����  
				{
					flag_full = true;
#ifdef DEBUG_CHARGE
					TRACE("because Time out in step6!\r\n");
#endif
					//qz add 20180710 
					//�����س���ʱ,��طŵ���������1200(mAs),��ǰ��ص����ͷŵ�������ƫ��,��Ҫ����
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,�̵Ƴ���5s.qz modify ����ʱ����
					power.step++;			//qz add 20180910
				}
			}
			break;
		case 7:
			//��س����磬PWM�رպ󣬵�ػ��Էŵ磬��ʱ��(����һ����)�󣬵���貹�����
				if((flag_full == true)&&(battery_voltage_1s < VOL_15_8V))		//15.8V
				{
					power.step = 3;
					flag_full = false;
					power.time = giv_sys_time;
					Init_Charge_Data(); 	//qz add 20180522
					Send_Voice(VOICE_CHARGING_START);
				}
		
			//��ֹ��ر����廵ʱһֱ��磬���仵���  (2015.4.7)
				if(flag_full == true)
				{
					power.pwm = 0;
					Init_Charge_Data(); 	//qz add 20180522
				}
			break;
		default :
			power.step = 0;
			break;
	}

	//if((charge_dc.key == 0)&&(charge_seat.key == 0))		//qz mask 20180515
	if(!(power.charge_dc)&&(!power.charge_seat))	//qz add 20180515
	{
		disable_pwm(CHARGE_PWM);
		power.pwm = 0;
		Init_Charge_Data();
	}
	else
	{
		enable_pwm(CHARGE_PWM,power.pwm);	//ִ��pwm����
	}		
}




/************************************************
���ܣ��ж��Ƿ����ⲿ��Դ���롣
���룺��
�������
************************************************/
void judge_charge(void)
{
static u32 time0,time1,time2,time3;
////////////////////�ж�dc���������/////////////////
#if 1
	if(Read_Charge_Seat())			//��������ź�
		{
			time1=giv_sys_time;
			if(!power.charge_seat)	//�������־֮ǰ��Ч
				{
					if(giv_sys_time-time0>100)	//10ms
						{
							power.charge_seat=1;
						}
				}
		}
	else
		{
			time0=giv_sys_time;
			if(power.charge_seat)	//�������־֮ǰ��Ч
				{
					if(giv_sys_time-time1>100)
						{
							power.charge_seat=0;
						}
				}
		}

	if(Read_Charge_Dc())
		{
			time3=giv_sys_time;
			if(!power.charge_dc)
				{
					if(giv_sys_time-time2>100)
						{
							power.charge_dc=1;
						}
				}
		}
	else
		{
			time2=giv_sys_time;
			if(power.charge_dc)
				{
					if(giv_sys_time-time3>100)
						{
							power.charge_dc=0;
						}
				}
		}
#endif
}


/*************************************************************
���ܣ��¶Ȳ�����1���Ӳ���һ�Σ�����10�μ���ƽ��ֵ���棬
*************************************************************/
void  sampling_temp_voltage(void)
{
static u32	l_linshi_current = 0,               //���ֵ���ʱ����
            r_linshi_current = 0,               //���ֵ���ʱ����
			m_linshi_current = 0,               //��ɨ����ʱ����
			dust_linshi_current = 0,            //�ҳ��������ʱ����
            battery_linshi_voltage = 0,         //��ص���ʱ��ѹ          
			battery_linshi_temp = 0,            //��ص���ʱ�¶�
            battery_linshi_chargecurrent = 0;//���5���ڵ�ƽ������
			  
static u16 sampling_number = 0;                //�������� 

static u32	l_linshi_current_1s = 0,               //���ֵ���ʱ����
            r_linshi_current_1s = 0,               //���ֵ���ʱ����
			m_linshi_current_1s = 0,               //��ɨ����ʱ����
			dust_linshi_current_1s = 0,            //�ҳ��������ʱ����
            battery_linshi_voltage_1s = 0,         //��ص���ʱ��ѹ          
			battery_linshi_temp_1s = 0,            //��ص���ʱ�¶�
            battery_linshi_chargecurrent_1s = 0;//���1���ڵ�ƽ������
			  
static u16 sampling_number_1s = 0;                //��������

u32 t;
   if(VOL_TEMP_ready == true)
			 {VOL_TEMP_ready = false;}
   else
			 {return;}
			 
			 
			 
			 
   sampling_number ++;
   t =	account_current(L_CURRENT)/4;		//qz modify 20181120,��Ψ��������Ϊ0.47ŷ,������Ϊ2ŷ, 2/0.47=4.25
   l_linshi_current += t;
	l_linshi_current_1s += t ;
	l_temp_curr_50ms+=t;
   t = 	account_current(R_CURRENT)/4;
   r_linshi_current += t;
   r_linshi_current_1s += t;
   r_temp_curr_50ms+=t;
   sample_number_50ms++;
   t = 	account_current(M_CURRENT)/4 ;
   m_linshi_current += t;
   m_linshi_current_1s += t;
   t =	account_current(FAN_CURRNET)/4;
   dust_linshi_current += t;
   dust_linshi_current_1s += t;
   t =	account_current(BATTERY_VOLTAGE);
   battery_linshi_voltage += t;
   battery_linshi_voltage_1s += t;
#if 0 //shftemp  
   t =	account_current(BATTERY_TEMP);
 #else
 	t =0;
 #endif   
   battery_linshi_temp += t;
   battery_linshi_temp_1s += t;	
   t =	 account_current(CHARGE_CURRENT);
   battery_linshi_chargecurrent += t;
   battery_linshi_chargecurrent_1s += t;
   if(sampling_number == 25000)			//5s
			 {
					battery_voltage = (u16) (battery_linshi_voltage/25000);    //���1��ĵ�ѹ
					battery_temp = (u16) (battery_linshi_temp/25000);       //���1����¶�
					l_current = (u16) (l_linshi_current/25000);          //���ֵ���
					r_current = (u16) (r_linshi_current/25000);          //���ֵ���
					dust_current = (u16) (dust_linshi_current/25000);       //�ҳ��������
					m_current = (u16) (m_linshi_current/25000);          //��ɨ����
					battery_chargecurrent = (u16)(battery_linshi_chargecurrent/25000);//���ƽ������
					sampling_number = 0;
					battery_linshi_voltage = 0;
					battery_linshi_temp = 0;
					l_linshi_current = 0;
					r_linshi_current = 0;
					dust_linshi_current = 0;
					m_linshi_current = 0;
					battery_linshi_chargecurrent = 0;
			 }		 
    			
   sampling_number_1s ++;
   if(sampling_number_1s == 5000)		//1s
			 {
					battery_voltage_1s = (u16) (battery_linshi_voltage_1s/5000);    //���1��ĵ�ѹ
					battery_temp_1s = (u16) (battery_linshi_temp_1s/5000);       //���1����¶�
					l_current_1s = (u16) (l_linshi_current_1s/5000);          //���ֵ���
					r_current_1s = (u16) (r_linshi_current_1s/5000);          //���ֵ���
					dust_current_1s = (u16) (dust_linshi_current_1s/5000);       //�ҳ��������
					m_current_1s = (u16) (m_linshi_current_1s/5000);          //��ɨ����
					battery_chargecurrent_1s = (u16)(battery_linshi_chargecurrent_1s/5000);//���ƽ������
					sampling_number_1s = 0;
					battery_linshi_voltage_1s = 0;
					battery_linshi_temp_1s = 0;
					l_linshi_current_1s = 0;
					r_linshi_current_1s = 0;
					dust_linshi_current_1s = 0;
					m_linshi_current_1s = 0;
					battery_linshi_chargecurrent_1s = 0;
			 }
   if(sample_number_50ms== 100)		//50ms
   	{
   		l_current_50ms=(u16)(l_temp_curr_50ms/100);
		r_current_50ms=(u16)(r_temp_curr_50ms/100);
		l_temp_curr_50ms=0;
		r_temp_curr_50ms=0;
		sample_number_50ms=0;
		ms_curr_flag=true;

   	}
   
}

//��Ϊһ��ADCת��ͨ����Ҫ1.75us,ת��16��ͨ���Ҵ���256�����ݹ���Ҫ1.75*256=448us
//���Խ���˺�������VOL_TEMP_READY_MYΪ500us����һ��
void  sampling_temp_voltage_my(void)
{
static u32 		l_linshi_current = 0,               //���ֵ���ʱ����
              	r_linshi_current = 0,               //���ֵ���ʱ����
				m_linshi_current = 0,               //��ɨ����ʱ����
				dust_linshi_current = 0,            //�ҳ��������ʱ����
              	battery_linshi_voltage = 0,         //��ص���ʱ��ѹ          
				battery_linshi_temp = 0,            //��ص���ʱ�¶�
              	battery_linshi_chargecurrent = 0;//���5���ڵ�ƽ������
			  
static u16 		sampling_number = 0;                //�������� 

static u32 		l_linshi_current_1s = 0,               //���ֵ���ʱ����
              	r_linshi_current_1s = 0,               //���ֵ���ʱ����
				m_linshi_current_1s = 0,               //��ɨ����ʱ����
				dust_linshi_current_1s = 0,            //�ҳ��������ʱ����
              	battery_linshi_voltage_1s = 0,         //��ص���ʱ��ѹ          
				battery_linshi_temp_1s = 0,            //��ص���ʱ�¶�
              	battery_linshi_chargecurrent_1s = 0;//���1���ڵ�ƽ������
			  
static u16 		sampling_number_1s = 0;                //��������

u32 t;
   if(VOL_TEMP_READY_MY == true)
			 {VOL_TEMP_READY_MY = false;}
   else
			 {return;}
			 
			 
			 
			 
   	sampling_number ++;
   	t =	account_current(L_CURRENT);
	l_linshi_current += t;
   	l_linshi_current_1s += t ;
	l_temp_curr_50ms+=t;
   	t = account_current(R_CURRENT);
   	r_linshi_current += t;
   	r_linshi_current_1s += t;
   	r_temp_curr_50ms+=t;
   	sample_number_50ms++;
#if 1 //shftemp 
   	t = account_current(M_CURRENT) ;
#else
	  t = 0;
#endif
   m_linshi_current += t;
   m_linshi_current_1s += t;
#if 0 //shftemp      
   t =	account_current(DUST_CURRENT);
#else
t = 0;
#endif   
   dust_linshi_current += t;
   dust_linshi_current_1s += t;
   t =	account_current(BATTERY_VOLTAGE);
   battery_linshi_voltage += t;
   battery_linshi_voltage_1s += t;
#if 0 //shftemp  
   t =	account_current(BATTERY_TEMP);
 #else
 t =	0;
 #endif   
   battery_linshi_temp += t;
   battery_linshi_temp_1s += t;	
#if 1 //shftemp 
   t =	 account_current(CHARGE_CURRENT);
  #else
  t =	 0;
 #endif  
   battery_linshi_chargecurrent += t;
   battery_linshi_chargecurrent_1s += t;
   if(sampling_number == 10000)		//5s��
			 {
					battery_voltage = (u16) (battery_linshi_voltage/10000);    //���1��ĵ�ѹ
					battery_temp = (u16) (battery_linshi_temp/10000);       //���1����¶�
					l_current = (u16) (l_linshi_current/10000);          //���ֵ���
					r_current = (u16) (r_linshi_current/10000);          //���ֵ���
					dust_current = (u16) (dust_linshi_current/10000);       //�ҳ��������
					m_current = (u16) (m_linshi_current/10000);          //��ɨ����
					battery_chargecurrent = (u16)(battery_linshi_chargecurrent/10000);//���ƽ������
					sampling_number = 0;
					battery_linshi_voltage = 0;
					battery_linshi_temp = 0;
					l_linshi_current = 0;
					r_linshi_current = 0;
					dust_linshi_current = 0;
					m_linshi_current = 0;
					battery_linshi_chargecurrent = 0;
			 }		 
    			
   sampling_number_1s ++;
   if(sampling_number_1s == 2000)		//1s��
			 {
					battery_voltage_1s = (u16) (battery_linshi_voltage_1s/2000);    //���1��ĵ�ѹ
					battery_temp_1s = (u16) (battery_linshi_temp_1s/2000);       //���1����¶�
					l_current_1s = (u16) (l_linshi_current_1s/2000);          //���ֵ���
					r_current_1s = (u16) (r_linshi_current_1s/2000);          //���ֵ���
					dust_current_1s = (u16) (dust_linshi_current_1s/2000);       //�ҳ��������
					m_current_1s = (u16) (m_linshi_current_1s/2000);          //��ɨ����
					battery_chargecurrent_1s = (u16)(battery_linshi_chargecurrent_1s/2000);//���ƽ������
					sampling_number_1s = 0;
					battery_linshi_voltage_1s = 0;
					battery_linshi_temp_1s = 0;	
					l_linshi_current_1s = 0;
					r_linshi_current_1s = 0;
					dust_linshi_current_1s = 0;
					m_linshi_current_1s = 0;
					battery_linshi_chargecurrent_1s = 0;
			 }
   if(sample_number_50ms== 100)		//50ms
   	{
   		l_current_50ms=(u16)(l_temp_curr_50ms/100);
		r_current_50ms=(u16)(r_temp_curr_50ms/100);
		l_temp_curr_50ms=0;
		r_temp_curr_50ms=0;
		sample_number_50ms=0;
		ms_curr_flag=true;
   	}
   	
}

/*************************************************************************
����: ��ȡ����¶�
����: ��ص��¶�ֵ
*************************************************************************/
u8 Get_BatteryTemp(void)
{
u8 i;
    if(battery_temp > temp[50])
	{
	    for(i = 50; i > 0; i--)
		{
		    if((battery_temp > temp[i+1])&&(battery_temp < temp[i]))
		    {
			    return i;
		    }
		}	
	}
	else
	{
	    for(i = 50;i < 118;i++)
		{
		    if((battery_temp > temp[i+1])&&(battery_temp < temp[i]))
		    {
			    return i ;
		    }
		}
	}
	return 0;
}
//===============================================================================================
/****************************************************
���ܣ����ݵ�ص�ѹ�ж���ʾ��ѹ
���룺��
�����
���أ���
****************************************************/
void Get_Dispower(void)
{
	//u32  t;
	u8	display = 0;
	if(!get_dispow_flag)
		return;
	get_dispow_flag=false;
	
#if 0	//qz mask
	////////////	t = (Battery.BatteryFDCap *10)/Battery.BatteryCapability;
	////////////	if(t > 7)
	////////////				{display = 1;}
	////////////		else if(t > 4)
	////////////				{display = 2;}
	////////////		else 
	////////////				{display = 3;}
	////////////				
					
				
	//		if((display == 2)&&(battery_voltage_1s > 1680)) //1680	��Ӧ	14.86V	 3.71V		4��﮵��
	//				{display = 3;}
	//				
	//		if((display == 1)&&(battery_voltage_1s > 1600)) //1680	��Ӧ	14.86V	 3.71V		4��﮵��	
	//				{display = 2;}	
	
	
	/////// //17.1V~1929   17.5V~1974	16.9V~1909	17.3~1952
	//16.8~1896  17~1919
#define		BAT_0_Point_1		451.4285			//(1896/16.8)*4
	
	
		if(mode.mode == CHARGEING)
			{
				if((battery_voltage_1s > (BAT_0_Point_1 * 4.15)))	
						{display = 4;}
				if((battery_voltage_1s > (BAT_0_Point_1 * 3.95)))	
						{display = 3;}	
				else if((battery_voltage_1s > (BAT_0_Point_1 * 3.8)))	
						{display = 2;}
				else if((battery_voltage_1s > (BAT_0_Point_1 * 3.7)))		
						{display = 1;}
				else	
						{display = 0;}
						
			}
	else{
		
		
			
	//				t = (Battery.BatteryFDCap *10)/Battery.BatteryCapability;
	//				if(t > 7)
	//						{display = 1;}
	//				else if(t > 4)
	//						{display = 2;}
	//				else 
	//						{display = 3;}
							
				
				if((battery_voltage_1s > (BAT_0_Point_1 * 3.9)))	
						{display = 4;}	
				if((battery_voltage_1s > (BAT_0_Point_1 * 3.8)))	
						{display = 3;}	
				else if((battery_voltage_1s > (BAT_0_Point_1 * 3.74)))	
						{display = 2;}
				else if((battery_voltage_1s > (BAT_0_Point_1 * 3.65)))		
						{display = 1;}
				else	
						{display = 0;}
	
						
						
	
			}
#endif

	//�Ե�ص�ѹ����AD��
	//0x509	11.2V
	//0x62A 13.5V
	//0x662 14V
	//0x6d6 15V
	//0x743 16V
	//0x77b 16.5V
	//0x795 16.73V(MAX),
	//��11V��16.5VΪ��ص�ѹ����,�������ݷ���,����ADֵ�ȵ�ѹֵ�����ADֵ��0x37~0x39����
	//���ϳ��ʱ,�Ⱦ�̬��ѹ��0.36~0.38V���ң�ȡ0.37V----ADֵ0x2A�����Գ��ʱ,ADֵ��Ҫ���ھ�̬ADԼ0x2A
	
#if 0
			if(mode.mode==CHARGEING)
				{ 
					if((battery_voltage_1s>=0x790))		//����ѹ�ﵽ16.73V,���и���
						{
							display=4;
						}  
					else if(battery_voltage_1s>=0x6d6)//0x743)	//����ѹ16V����һ�������������ĸ���˸
						{
							display=3;
						}
					else if(battery_voltage_1s>=0x600)//0x6d6)	//����ѹ15V����һ����������������˸
						{
							display=2;
						}
					else if(battery_voltage_1s>=0x500)//0x662)	//����ѹ14V����һ�������ڶ�����˸
						{
							display=1;
						}
					else
						display=0;						//��һ����˸
				}
			else
				{
					if(battery_voltage_1s>=0x743)
						{
							display=4;
						}
					else if(battery_voltage_1s>=0x6d6)	//��ص�ѹ15V
						{
							display=3;
						}
					else if(battery_voltage_1s>=0x600)//��ص�ѹ13.1V�� 0x662) //��ص�ѹ14V
						{
							display=2;
						}
					else if(battery_voltage_1s>=0x500)//��ص�ѹ11V��0x62a)	//��ص�ѹ13.5V
						{
							display=1;
						}
					else
						display=0;
				}
		#endif
		if(Slam_Data.bat<=25)
			display=1;
		else if(Slam_Data.bat<=50)
			display=2;
		else if(Slam_Data.bat<=75)
			display=3;
		else if(Slam_Data.bat<100)
			display=4;
		else 
			display=5;
}
//===============================================================================================


s8 Get_APPBat(void)
{
	//13.5V��16.5VΪ��ص�ѹ���䣬���ٷֱȼ���
	//13.5V��ADֵΪ0x62A,16.5V��ADֵΪ0x77B

	//�޸�Ϊ11V��16.5VΪ��ص�ѹ����
	//11V��ADֵΪ0X509,16.5V��ADֵΪ0X77B

	//�޸�Ϊ12.8V��16.8VΪ��ص�ѹ����
	//12.8V��ADֵΪ1444,16.8V��ADֵΪ1890

	//�޸�Ϊ12.8V��16.8VΪ��ص�ѹ����
	//12.8V��ADֵΪ1444,���״̬��ֵ��ѹΪ16.8V(1890),�ǳ��״̬��ֵΪ16.4V(1850)

	//20190709
	//�µ�ר�ùߵ��壬��ز��������иı�
	//12.8V��ADֵΪ2647,���״̬��ֵ��ѹΪ16.8V(3475),�ǳ��״̬��ֵΪ16.4V(3392)
	float a,b;
	if(mode.mode==CHARGEING)
		a=(float)(battery_voltage_1s-VOL_12_8V)/(float)(VOL_16_8V-VOL_12_8V);
	else
		a=(float)(battery_voltage_1s-VOL_12_8V)/(float)(VOL_16_5V-VOL_12_8V);
	b=a*100;
	
	if(a<0.0)	//qz add 20180625
		b=0.0;
	if(b>100.0)
		b=100.0;
	return (s8)b;
	
}

//qz add APP��ʾ�ĵ�ص����ճ�����
void APP_BAT_Handle(void)
{
	u16 temp_power=0;
	float temp_per,temp_data_float;
	u32 temp_data_u32;

	//qz add 20181011
	//����20s֮�ڣ�������˺���
	//Ŀ���Ǵ�ADCȡ��ص�ѹ��Ҫʱ�䣬��ֹ���ж�
	if(giv_sys_time-Battery.start_time<50000)
		return;
	//qz add end
	
	if(!Five_Sec)
		return;
	Five_Sec=false;

	
#ifdef DC_NOBAT_RUN
	if(dc_nobat_run)
		{
			Slam_Data.bat=100;
			return;
		}
#endif

	//�����ص���Ϊ0,��ʾ֮ǰ�����Ѿ�����û����ߵ�һ�ο���,��ʱ��ص������ɿ�,��Ҫ���¼���
	if((Battery.BatteryCapability==0))
		{
			if((Read_Charge_Dc())|(Read_Charge_Seat()))
				Slam_Data.bat=0;			//��������ڳ��,�ϱ�α�ٷֱ�0%
			else
				Slam_Data.bat=100;			//������ڳ��,�ϱ�α�ٷֱ�100%
			return; 						//qz add 20180801,ֱ���˳����������ŵ���������Ϻ��ٽ�������Ĳ���
		}
	
	//Check_Return_Pwr�����������30���ڵ�ص�ѹ�Ƿ����RETURN_PWR(13.6V)
	temp_power=Check_Return_Pwr();

	//��ص�ѹ����RETURNPWR,ֱ�������еķŵ������������,ͬʱ20%�ᵼ�µ�����س� qz add 20180801
	//if((battery_voltage!=0)&(battery_voltage<RETURNPWR)&(!Read_Charge_Dc())&(!Read_Charge_Seat())&(!bat_recal))
	if((temp_power)&(!Read_Charge_Dc())&(!Read_Charge_Seat())&(!Battery.bat_recal))
		{
			Send_Voice(VOICE_POWER_LOW);
			Battery.BatteryChargeForbid = 0;		//qz add 20180703
#ifdef DOCK_POWER
			TRACE("RETURN BAT!!!!!\r\n");
			TRACE("Bat_Cap before cal=%d\r\n",Battery.BatteryCapability);
#endif
			Battery.BatteryCapability=Battery.BatteryFDCap*10/8;
			if(Battery.BatteryCapability>=MAH2600)
				Battery.BatteryCapability=MAH2600;
#ifdef DOCK_POWER
			TRACE("Bat_Cap after cal=%d\r\n",Battery.BatteryCapability);
#endif
			WriteFDCap();
			WriteBatteryCapability();
			Battery.bat_recal=1;
			WriteBatRecalFlash(Battery.bat_recal);
		}
	//��ص�ѹ����NOPOWER,����Deadģʽ,qz add 20180625
	if((battery_voltage!=0)&(battery_voltage<NOPOWER)&(!Read_Charge_Dc())&(!Read_Charge_Seat()))
		{
			stop_rap();
			Slam_Data.bat=0;
			Battery.BatteryChargeForbid = 0;		//qz add 20180703
			//�����ص�ѹС��NOPOWERʱ����طŵ���С�ڵ�ص������򽫷ŵ������Ƹ����������ڶ�̬����
			if((Battery.BatteryFDCap>MAH1000)&(Battery.BatteryFDCap<=MAH2600))
				Battery.BatteryCapability=Battery.BatteryFDCap;
			else									//qz add 20180703
				Battery.BatteryFDCap=Battery.BatteryCapability;
			WriteFDCap();
			WriteBatteryCapability();
//			Slam_Data.dipan_req=DIPAN_REQ_TURNOFFSLAM;
			Init_Dead();
		}
	
	//�����ǰ��طŵ�������ڵ�ǰ��ص�������ʼ���򵼺����ϱ�1%��ֱ���ŵ�NOPOWER
	//�ҽ��µĵ�طŵ�����ֵ����ص������ŵ���ҪС��2600mAh���������µ�طŵ������µ�ص���
	if((Battery.BatteryFDCap>Battery.BatteryCapability)&&(Battery.BatteryFDCap<MAH2600))		
		{											
			Slam_Data.bat=1;
			Battery.BatteryCapability=Battery.BatteryFDCap;	//ͬʱ�µĵ�طŵ�������ݱ��뱣�棬���µ�ص���=�µ�طŵ����
			WriteFDCap();
			WriteBatteryCapability();
		}
	//��طŵ�������2600mAh���������
	else if(Battery.BatteryFDCap>MAH2600)
		{
			Battery.BatteryFDCap=MAH2600;
			Battery.BatteryCapability=MAH2600;
			Slam_Data.bat=1;
			WriteFDCap();
			WriteBatteryCapability();
		}
	//qz add end
	else 
		{
			temp_per=(Battery.BatteryCapability-Battery.BatteryFDCap)*100/Battery.BatteryCapability;	//ʣ���������	
			if((temp_per<1.0)&&(temp_per>0.0))
				temp_per=1.0;

			//qz add 20180801����ٷֱ�С�ڵ���20%,���ǵ�ѹ������Ҫ�س��ѹ,���ȵȴ��ϱ�21%
			//qz add 20180803����bat_recal��־λ��Ϊ�������������ڣ�����ѹ���ڻس��ѹ���е���У׼�Ժ�
			//�����ѹ�ָ��ڻس��ѹ�ˣ���ֹ�ٷֱȳ��ַ�����
			//qz add 20180902����100/79��Ŀ����Ϊ�˰ٷֱȼ���ﵽ21%�����ǵ�ѹ�����ڻس��ѹ����ʱ�ŵ����ڲ������ӣ�
			//���ǵ�ص������ֲ��䣬��ô����Ժ󣬵�ذٷֱȻ�Ѹ���½��ܶࡣ
			if((temp_per<=20.0)&(!temp_power)&(mode.mode!=CHARGEING)&(!Battery.bat_recal))
				{
					temp_per=21.0;

#if 1				
					//qz add 20180902
					temp_data_float=(float)(Battery.BatteryFDCap)*100/79;		//���µ�ص�������Ϊ�˿̵�ص���ƫС
					temp_data_u32=(u32)(temp_data_float);
					if(temp_data_u32>MAH2600)
						temp_data_u32=MAH2600;
					Battery.BatteryCapability=temp_data_u32;
					WriteBatteryCapability();
					//qz add end
#endif
				}
				
			Slam_Data.bat=(s8)(temp_per);
		}

	if((Battery.bat_recal)&(battery_voltage>VOL_14_6V))
		{
			Battery.bat_recal=0;
			WriteBatRecalFlash(Battery.bat_recal);
		}
}

/***************************************************
���ܣ�ÿ���1�������ʣ������
***************************************************/
void AccountCapability(void)
{
	if(Battery.UsePowerTime == Rtc_time)
		{return ;}
//	AccountCapabilityReal();
#ifdef DC_NOBAT_RUN
	if(dc_nobat_run)		//����ǲ���DCͷ����,��������ص���
		return;
#endif
	AccountCapabilityReal_My();
}

/****************************************************
���ܣ��������ʹ�õĵ�ص���
���룺��
�����
���أ���
˵����ͨ�������ڸ���ģʽ�¹���ʱ�����ж�ʹ�õĵ���
      �ı����ֵ
	  Battery.UsePowerTime    
	  Battery.BatteryFDCap
	  
	  �ڵ�ص�ѹ����13.2vʱ�������Ϊ�Ѿ�����磬���ҽ��ŵ��������뱸�ݼĴ���
****************************************************/

#define MAIN_PCB_CURRENT		50

void  AccountCapabilityReal(void)
{
static u8 entercapability = 0;
	char i;
u32 t;//�ĵ�ʱ��
u32 curr;//�ĵ����
	t = 0;
	curr = 0;
	if(Rtc_time < Battery.UsePowerTime)
			{
					t = 604800;
			}
	t = (Rtc_time + t) - Battery.UsePowerTime;
	Battery.UsePowerTime = Rtc_time;
	switch(mode.mode)
		{
				case CEASE: 
				case ERR:
						curr = 71 + MAIN_PCB_CURRENT;
						break;
				
				case SLEEP:
						curr = 20 + MAIN_PCB_CURRENT;
						break;
				
				case SWEEP:
				case SPOT:
						curr = 1200 + MAIN_PCB_CURRENT;
						break ;
				
				case DOCKING:
						curr = 400 + MAIN_PCB_CURRENT;
						break;
				
				case CHARGEING:
						switch(power.step)
								{
									case 2:
									case 4:
											curr = 120 + MAIN_PCB_CURRENT;
											break;
									case 3:
											curr = 800 + MAIN_PCB_CURRENT;
										break;
								}
						break;
		}
	//------------------------------------
		
	t = t*curr;
	if(t > 2600)
		{
				return;
		}
	
	
	if(mode.mode == CHARGEING)
			{
					if(Battery.BatteryFDCap > t)
					{
							Battery.BatteryFDCap -= t;
					}
					else
					{
							Battery.BatteryFDCap = 0;
					}
			}
	else 
			{
				if(Battery.BatteryFDCap < 0x83d600)
						Battery.BatteryFDCap += t;
			}
	if((((Battery.BatteryFDCap *10)/Battery.BatteryCapability) > 2)&&(Battery.BatteryChargeForbid == 1))
			{
					Battery.BatteryChargeForbid = 0;
			}
			
	WriteFDCap();
	if((battery_voltage < LOWPOWER) && (mode.mode == CEASE))
			{
					if(entercapability == 0)
							{
								entercapability = 1;
								Battery.BatteryChargeForbid = 0;
								if((Battery.BatteryFDCap > 0x36ee80)&&(Battery.BatteryFDCap < 0x83d600))
									{
												Battery.BatteryCapability = Battery.BatteryFDCap;
									}
								else
									{
										Battery.BatteryFDCap = 	 Battery.BatteryCapability;
									}					   
								WriteBatteryCapability();
							}
					if(battery_low_beep_flag)
							{
								battery_low_beep_flag = 0;
								for(i=0;i<LOWPOWER_BEEP_NUM;i++)
										{
											//Set_Beep();
											DelayMs(100);
											//Reset_Beep();
											DelayMs(100);
										}
							}

////////////						Init_Sleep();
////////////						Do_Sleep();
			}
	else
	{
			battery_low_beep_flag = 1;
	    if(battery_voltage > TWOPOWER)
				{
						entercapability = 0;
				}
	}
    return;
}


void  AccountCapabilityReal_My(void)
{
//static u8 entercapability = 0;
//	char i;
u32 t;//�ĵ�ʱ��
u32 curr;//�ĵ����
	t = 0;
	curr = 0;
	if(Rtc_time < Battery.UsePowerTime)
			{
					t = 604800;
			}
	t = (Rtc_time + t) - Battery.UsePowerTime;
	Battery.UsePowerTime = Rtc_time;
	switch(mode.mode)
		{
				case CEASE: 
					switch(mode.sub_mode)
						{
							case SLEEP:
								curr=(SLEEP_CUR);
								break;
							case CEASE:
							case ERR:
								curr=(CEASE_CUR);
								break;
							case QUIT_CHARGING:
							case SELF_TEST:
								if(MidBrush.flag)
									{
									switch(sweep_suction)
										{
											case STOP_ALL:
												curr=(MAIN_CUR);
												break;
											case SILENCE:
												curr=((MAIN_CUR+SB_SILENCE_CUR));	//qz modify 20181120:��Ϊ��������ɲ⣬����ȡ���������
												break;
											case STANDARD:								
												curr=((MAIN_CUR+SB_STANDARD_CUR));
												break;
											case FORCE:
												curr=((MAIN_CUR+SB_FORCE_CUR));
												break;
										}
									}
								else
									curr=MAIN_CUR;
									break ;
						}
					break;
				case YBS:
					if(MidBrush.flag)
						{
						switch(sweep_suction)
							{
								case STOP_ALL:
									curr=(MAIN_CUR);
									break;
								case SILENCE:
									curr=((MAIN_CUR+SB_SILENCE_CUR));
									break;
								case STANDARD:

									curr=((MAIN_CUR+SB_STANDARD_CUR));
									break;
								case FORCE:
									curr=((MAIN_CUR+SB_FORCE_CUR));
									break;
							}
						}
					else
						curr=MAIN_CUR;
						//curr = 1200 + MAIN_PCB_CURRENT;
						break ;
				
				case DOCKING:
						//curr = 400 + MAIN_PCB_CURRENT;
						curr=(u32)((MAIN_CUR+SB_DOCK_CUR));
						break;
				
				case CHARGEING:
						switch(power.step)
								{
									case 0:
										curr=0;
										break;
									case 1:
									case 2:
									case 4:
									case 5: //qz add 20180615
											curr = 100;//150;		//��ʼ��������ʱ����Ȼ�̶�����Ϊ360mA,���ǵ�����ռȥ210mA����
											break;
									case 3:
											curr = 400;//600;		//��������ʱ����Ȼ�̶�����Ϊ800mA,���ǵ�����ռȥ210mA����
										break;
									case 6:
											curr = battery_chargecurrent_1s;
										break;
								}
						break;
		}
	//------------------------------------
	//���������֡���ɨ����
	curr=curr+(u32)(l_current_1s*RING_MB_CUR_CAL)+(u32)(r_current_1s*RING_MB_CUR_CAL)+(u32)(m_current_1s*RING_MB_CUR_CAL)+(u32)(dust_current_1s*RING_MB_CUR_CAL);	//qz modify 20181120:����������

	//if(UV_FLAG)				//qz add 20180902:����UV����
		//curr=curr+140;		

	t = t*curr;
	if(t > 2600)
		{
				return;
		}
	
	
	if(mode.mode == CHARGEING)							//�������ڳ��ģʽ��
			{
				if(!flag_full)			//�����δ����
					{
						if(Battery.BatteryFDCap>t)
							{
								Battery.BatteryFDCap-=t;
							}
						else
							{
								Battery.BatteryFDCap--;
								if(Battery.BatteryFDCap<=1)			//��ص����ϱ�ʼ��Ϊ99%
									Battery.BatteryFDCap=1;
								//Battery.BatteryCapability+=t;		//�����δ����,���ŵ����ѿ�������,˵�����ڵ����ͷŵ���ƫС,���¼������
								//if(Battery.BatteryCapability>=MAH2600)
									//Battery.BatteryCapability=MAH2600;
								//WriteBatteryCapability();			
							}
					}
				//��س������Ժ����������
				//qz add end
			}
	else 
			{
				//if(Battery.BatteryFDCap < 0x83d600)//qz mask 20180625
				if(Battery.BatteryFDCap<MAH2600)	//qz add 20180625
					Battery.BatteryFDCap += t;
				//���������������APP_BAT_HANDLE������Ҳ�м���
			}
	if((((Battery.BatteryFDCap *10)/Battery.BatteryCapability) > 2)&&(Battery.BatteryChargeForbid == 1))
			{
					Battery.BatteryChargeForbid = 0;
			}

	//qz add 2080803,17V���Ͽ�ʼ�Ķ�����������������㣬����ʱ����һֱΪ100%
	if((mode.mode!=CHARGEING)&(Battery.BatteryFDCap<=t)&(battery_voltage>VOL_17V))
		{
			Battery.BatteryFDCap=0;
		}
			
	WriteFDCap();
#if 0		//qz mask 20180625
	//if((battery_voltage < LOWPOWER) && (mode.mode == CEASE))		//qz mask 20180615
	if((battery_voltage < CUTPOWER) && (mode.mode != CHARGEING))	//qz add 20180615
			{
					if(entercapability == 0)
							{
								entercapability = 1;
								Battery.BatteryChargeForbid = 0;

								//����2000mAh��С��2600mAh
								if((Battery.BatteryFDCap > 0x6ddd00)&&(Battery.BatteryFDCap <=0x8ed280))
									{
												Battery.BatteryCapability = Battery.BatteryFDCap;
									}
								else
									{
										Battery.BatteryFDCap = 	 Battery.BatteryCapability;
									}					   
								WriteBatteryCapability();
							}
					if(battery_low_beep_flag)
							{
								battery_low_beep_flag = 0;
								for(i=0;i<LOWPOWER_BEEP_NUM;i++)
										{
											//Set_Beep();
											DelayMs(100);
											//Reset_Beep();
											DelayMs(100);
										}
							}

////////////						Init_Sleep();
////////////						Do_Sleep();
			}
	else
	{
			battery_low_beep_flag = 1;
	    if(battery_voltage > TWOPOWER)
				{
						entercapability = 0;
				}
	}
#endif
    return;
}

void Charge_PID_Ctr(u32 tgt_current)
{
	float f_ec;
	if((!CHARGE_READY))
		return;
	CHARGE_READY=false;

	charge_data.real_current=(charge_data.piv_current_sum)/charge_data.piv_current_count*CHG_CUR_CNT;
	charge_data.piv_current_sum=0;
	charge_data.piv_current_count=0;

	#if 0
	if(!charge_data.pid_use)
		return;
	if(((charge_data.real_current>=780.0)&(charge_data.real_current<=820.0))&(power.step==3)
		&(giv_sys_time-power.step_time>50000))
	{
		charge_data.pid_use=false;
		return;
	}
	#endif
	
	charge_data.ek[2]=charge_data.ek[1];
	charge_data.ek[1]=charge_data.ek[0];
	charge_data.ek[0]=(float)(tgt_current)-charge_data.real_current;
	f_ec = ((charge_data.ek[0]-charge_data.ek[1])*C_KP + charge_data.ek[0]*C_KI+\
	(charge_data.ek[0]+charge_data.ek[2]-2*charge_data.ek[1])*C_KD);
	
	power.pwm=(int)(power.pwm+f_ec);

	if(power.pwm<=0)
		power.pwm=0;
	if(power.pwm > (PWM_CHARGE_MAX-CHARGE_PWM_MASK))
		power.pwm=PWM_CHARGE_MAX-CHARGE_PWM_MASK;

}

void Charge_PID_CtrPwr(u32 tgt_voltage)
{
	float f_ec;
	if((!CHARGE_READY))
		return;
	CHARGE_READY=false;

	charge_data.real_voltage=(float)(charge_data.piv_voltage_sum)/(float)(charge_data.piv_voltage_count);
	charge_data.piv_voltage_sum=0;
	charge_data.piv_voltage_count=0;
	
	charge_data.ek[2]=charge_data.ek[1];
	charge_data.ek[1]=charge_data.ek[0];
	charge_data.ek[0]=(float)(tgt_voltage)-charge_data.real_voltage;
	f_ec = ((charge_data.ek[0]-charge_data.ek[1])*C_KP + charge_data.ek[0]*C_KI+\
	(charge_data.ek[0]+charge_data.ek[2]-2*charge_data.ek[1])*C_KD);
	
	power.pwm=(int)(power.pwm+f_ec);

	if(power.pwm<=0)
		power.pwm=0;
	if(power.pwm > (PWM_CHARGE_MAX-CHARGE_PWM_MASK))
		power.pwm=PWM_CHARGE_MAX-CHARGE_PWM_MASK;
}

void Init_Charge_Data(void)
{
	charge_data.ek[0]=0;
	charge_data.ek[1]=0;
	charge_data.ek[2]=0;
	charge_data.piv_current_count=0;
	charge_data.piv_current_sum=0;
	charge_data.real_current=0;
	charge_data.piv_voltage_count=0;
	charge_data.piv_current_sum=0;
	charge_data.real_voltage=0;
}

void Disable_Charge(void)
{
	power.pwm=0;
}


//qz add 20180717������ƽ���˲�
u32 Check_curr_oc(u32 now_curr,u32 *buf)
{
	u32 sum;
	for(int i=0;i<9;i++)
		buf[i]=buf[i+1];
	buf[9]=now_curr;
	for(int i=0;i<10;i++)
		sum+=buf[i];
	return (sum/10);
}

void clr_curr_buf(u32 *buf)
{
	for(int i=0;i<9;i++)
		buf[i]=0;
}

//qz add 20180717
//���ڼ�����������ϰ�ʱ�������Ƿ�����
//��Ϊ�������ϰ�����סʱ�������ֵ��������󣬵��ǲ�����һֱ����һ���ܴ��ֵ
//�����жϵ���ֵʱ������һ��Buffer�����ڻ���ƽ���˲��������˵��Ǹ�Сֵ
u8 Check_Left_Ring_Cur(void)
{
	static u8 step=0;
	static u32 step1_time=0,step2_time=0;
	u32 l_curr;
	//l_curr=account_current(L_CURRENT);
	switch (step)
		{
			case 0:
				l_curr=Check_curr_oc(l_current_50ms,l_curr_buf);
				if(l_curr>=116)		//116-->200mA  105-->180mA
					{
						step++;
						step1_time=giv_sys_time;
#ifdef CURR_DEBUG
						TRACE("l_curr_oc=%d\r\n",l_curr);
						TRACE("l_curr=%d\r\n",l_current_50ms);
						TRACE("goto step1!\r\n");
#endif
					}
				break;
			case 1:
				l_curr=Check_curr_oc(l_current_50ms,l_curr_buf);
				if(giv_sys_time-step1_time>5000)
					{ 
#ifdef CURR_DEBUG
						TRACE("l_curr ov!\r\n");
#endif
						step=0;
						return 1;
					}
				if(l_curr<58)		//58--->100mA
					{
						step=0;
						step2_time=giv_sys_time;
#ifdef CURR_DEBUG
						TRACE("l_curr=%d\r\n",l_current_50ms);
						TRACE("goto step0!\r\n");
#endif						
					}
				break;
			case 2:
				if(giv_sys_time-step2_time>3000)
					{
#ifdef CURR_DEBUG
						TRACE("l_curr=%d\r\n",l_current_50ms);
						TRACE("times2up goback step0!\r\n");
#endif
						step=0;
						return 0;
					}
				if(l_current_50ms>116)
					{
						if(giv_sys_time-step1_time>5000)
							{
#ifdef CURR_DEBUG
								TRACE("times1up goback step0!\r\n");
#endif
								step=0;
								return 0;
							}
#ifdef CURR_DEBUG
						TRACE("l_curr=%d\r\n",l_current_50ms);
						TRACE("goback step1!\r\n");
#endif
						step=1;
					}
		}
	return 0;
}

u8 Check_Right_Ring_Cur(void)
{
	static u8 step=0;
	static u32 step1_time=0,step2_time=0;
	u32 r_curr;
	//l_curr=account_current(L_CURRENT);
	switch (step)
		{
			case 0:
				r_curr=Check_curr_oc(r_current_50ms,r_curr_buf);
				if(r_curr>=116)		//116-->200mA  105-->180mA
					{
						step++;
						step1_time=giv_sys_time;
#ifdef CURR_DEBUG
						TRACE("r_curr_oc=%d\r\n",r_curr);
						TRACE("r_curr=%d\r\n",r_current_50ms);
						TRACE("goto step1!\r\n");
#endif
					}
				break;
			case 1:
				r_curr=Check_curr_oc(r_current_50ms,r_curr_buf);
				if(giv_sys_time-step1_time>5000)
					{ 
#ifdef CURR_DEBUG
						TRACE("r_curr ov!\r\n");
#endif
						step=0;
						return 1;
					}
				if(r_curr<58)		//58--->100mA
					{
						step=0;
						step2_time=giv_sys_time;
#ifdef CURR_DEBUG
						TRACE("r_curr=%d\r\n",r_current_50ms);
						TRACE("goto step0!\r\n");
#endif						
					}
				break;
			case 2:
				if(giv_sys_time-step2_time>3000)
					{
#ifdef CURR_DEBUG
						TRACE("r_curr=%d\r\n",r_current_50ms);
						TRACE("times2up goback step0!\r\n");
#endif
						step=0;
						return 0;
					}
				if(r_current_50ms>116)
					{
						if(giv_sys_time-step1_time>5000)
							{
#ifdef CURR_DEBUG
								TRACE("times1up goback step0!\r\n");
#endif
								step=0;
								return 0;
							}
#ifdef CURR_DEBUG
						TRACE("r_curr=%d\r\n",r_current_50ms);
						TRACE("goback step1!\r\n");
#endif
						step=1;
					}
		}
	return 0;
}

/*---���30s�ڵ�ѹ�Ƿ����RETURN_PWR-----
//qz add 20180803
�����
	0:30s�ڵ�ѹû�е��ڻس��ѹ
	1:30s�ڵ�ѹȫ�����ڻس��ѹ
-----------------------------------------*/
u8 Check_Return_Pwr(void)
{
#if 1
	static u8 step=0;
	static u32 check_time=0;
	u16 temp_vol;
	temp_vol=battery_voltage;
	switch(step)
		{
			case 0:
				if((temp_vol<RETURNPWR)&(temp_vol!=0))
					{
						step++;
						check_time=giv_sys_time;
					}
				break;
			case 1:
				if((temp_vol>=RETURNPWR))
					{
						step=0;
						check_time=giv_sys_time;
						return 0;
					}
				if(giv_sys_time-check_time>300000)	//1min֮��,battery_voltage����RETURNPWR
					{
						step=0;
						check_time=giv_sys_time;
						return 1;
					}
		}
	return 0;
#endif
	
}

