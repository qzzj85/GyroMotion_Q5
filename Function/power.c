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

bool VOL_TEMP_ready;
u16  jt_chargecurrent;   //��̬ʱ�������ĵ���
u16  battery_chargecurrent;//���5���ڵ�ƽ������
u16  battery_voltage;    //���1��ĵ�ѹ
u16  battery_temp;       //���1����¶�
u16  l_current;          //���ֵ���
u16  r_current;          //���ֵ���
u16  dust_current;       //�ҳ��������
u16  m_current;          //��ɨ����	
u16  sb_current;

u16  battery_chargecurrent_1s;//���1���ڵ�ƽ������
u16  battery_voltage_1s;    //���1��ĵ�ѹ
u16  battery_temp_1s;       //���1����¶�
u16  l_current_1s;          //���ֵ���
u16  r_current_1s;          //���ֵ���
u16  dust_current_1s;       //�ҳ��������
u16  m_current_1s;          //��ɨ����
u16  sb_current_1s;
bool dc_nobat_run=false;	//���õ��,DCֱ���ܻ���־λ
u16  battery_voltage_10s=0;	//���10s�ĵ�ѹֵ
u32  battery_voltage_10s_temp=0;
bool  flag_full = false;			//��س������־
///////////////////////˽�б���////////////////////////////////////
uint16 piv_current;
uint16 piv_voltage;

u16 full_power=VOL_16_8V;

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
  power.voltage = account_current(ADC_BAT_VOL);


  battery_voltage = power.voltage;
  battery_voltage_10s=power.voltage;
  //battery_temp = power.temp;
 #if 1//shftemp   
  jt_chargecurrent = account_current(ADC_BAT_CHGCURR);
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
			//Battery.BatteryCapability=MAH2600; 	//qz modify 20180801:ԭ��ΪMAH2600//2400mAh
			Battery.BatteryCapability=DEFAULT_CAP;
			temp_data=Get_APPBat();
			Battery.BatteryFDCap=(100-temp_data)*Battery.BatteryCapability/100;
			Battery.BatteryChargeForbid=0;
			WriteBatteryCapability();
			WriteFDCap();
			Battery.reinit=false;
		}		
}

void Reinit_Battery(void)
{
	PWR_BackupAccessCmd(ENABLE);

	BKP_WriteBackupRegister(BAT_REINIT_BKP, 0xAA55);		//�������־
	BKP_WriteBackupRegister(VOICE_BACKUP_BKP, 0x00EC);		//��������(Ĭ�ϵ�13������ԤԼʱ��Ϊû��
	BKP_WriteBackupRegister(PREEN_BACKUP_BKP, 0x0000);		//ԤԼ����Ϊû��
	BKP_WriteBackupRegister(MODE_BACKUP_BKP, 0x0101);		//BKP_DR4����ģʽ���棬��16λΪ��ģʽ����16λΪ��ģʽ��
	BKP_WriteBackupRegister(BATCAP_HADDR_BKP, 0x0); 		//BKP_DR5���ڵ�������ĸ�16λ
	BKP_WriteBackupRegister(BATCAP_LADDR_BKP, 0x0); 		//BKP_DR6���ڵ�������ĵ�16λ
	BKP_WriteBackupRegister(BATFDCAP_HADDR_BKP, 0x0);			//BKP_DR7���ڵ�طŵ����ĸ�16λ
	BKP_WriteBackupRegister(BATFDCAP_LADDR_BKP, 0x0);			//BKP_DR8���ڵ�طŵ����ĵ�16λ
	BKP_WriteBackupRegister(BATRECAL_BACKUP_BKP, 0X0);			//BKP_DR9���ڵ��RECAL��־,qz add 20181101
	Battery.BatteryCapability=0;
	Battery.BatteryFDCap=0;
	Battery.bat_recal=0;
#ifdef DEBUG_INIT
	TRACE("BKP Data Reinit!\r\n");
#endif
}

void Reinit_Battery_Data(void)
{
  	s8 temp_data;
	Battery.BatteryCapability=DEFAULT_CAP;	//qz modify 20180801:ԭ��ΪMAH2600//2400mAh
	temp_data=Get_APPBat();
	Battery.BatteryFDCap=(100-temp_data)*Battery.BatteryCapability/100;
	Battery.BatteryChargeForbid=0;
	Battery.bat_recal=1;
	WriteBatRecal(Battery.bat_recal);
	WriteBatteryCapability();
	WriteFDCap();
}

void Init_Battery_Data(void)
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
			Reinit_Battery_Data();
		}
		
	Battery.bat_recal=(u8)(ReadBatRecal());
	if((Battery.bat_recal!=0)&(Battery.bat_recal!=1))
		{
			Battery.bat_recal=0;
			WriteBatRecal((u16)(Battery.bat_recal));
		}
	Battery.UsePowerTime = Rtc_time;
	Battery.start_time=giv_sys_time;
#ifdef DEBUG_INIT
	TRACE("Battery.recal=%d\r\n",Battery.bat_recal);
#endif
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
		
	Battery.bat_recal=(u8)(ReadBatRecal());
	if((Battery.bat_recal!=0)&(Battery.bat_recal!=1))
		{
			Battery.bat_recal=0;
			WriteBatRecal((u16)(Battery.bat_recal));
		}
	Battery.UsePowerTime = Rtc_time;
	Battery.start_time=giv_sys_time;
#ifdef DEBUG_INIT
	TRACE("Battery.recal=%d\r\n",Battery.bat_recal);
#endif
}


void ChargeControl_Volt_My(void)
{

	//////////////////������//////////////////////////////////////

	piv_current = account_current(ADC_BAT_CHGCURR);	 //������ʱ�ĵ���
	piv_voltage = account_current(ADC_BAT_VOL);   //������ʱ�ĵ�ѹ
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
			if(giv_sys_time-power.time>30000)
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
				jt_chargecurrent=battery_chargecurrent_1s;
#ifdef DEBUG_CHARGE
				TRACE("jt_curr=%d\r\n",(u32)(jt_chargecurrent*CURR_CHG_CNT));
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
			if((battery_voltage > full_power))	//qz modify 20180703:1805 16V 1861:16.5V
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
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CURR_CHG_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CURR_CHG_CNT));
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
			if(((giv_sys_time-power.time>100000)&&(battery_voltage>full_power)&&(!flag_full)))
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
			Charge_PID_CtrPwr(full_power);	//qz modify 150->360:�̶�360mA
			Battery.BatteryChargeForbid = 1;	//��ѹ���,��ֹ��������
			////////ÿ�����ж�һ��////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
#ifdef DEBUG_CHARGE
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CURR_CHG_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CURR_CHG_CNT));
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
			sb_temp_current=0,
            battery_linshi_voltage = 0,         //��ص���ʱ��ѹ          
			battery_linshi_temp = 0,            //��ص���ʱ�¶�
            battery_linshi_chargecurrent = 0;//���5���ڵ�ƽ������
			  
static u16 sampling_number = 0;                //�������� 

static u32	l_linshi_current_1s = 0,               //���ֵ���ʱ����
            r_linshi_current_1s = 0,               //���ֵ���ʱ����
			m_linshi_current_1s = 0,               //��ɨ����ʱ����
			dust_linshi_current_1s = 0,            //�ҳ��������ʱ����
			sb_temp_current_1s=0,
            battery_linshi_voltage_1s = 0,         //��ص���ʱ��ѹ          
			battery_linshi_temp_1s = 0,            //��ص���ʱ�¶�
            battery_linshi_chargecurrent_1s = 0;//���1���ڵ�ƽ������
			  
static u16 sampling_number_1s = 0;                //��������

static u32 battery_linshi_chargecurrent_10s=0;
static u32 sampling_number_10s=0;
u32 t;
   if(VOL_TEMP_ready == true)
			 {VOL_TEMP_ready = false;}
   else
			 {return;}
			 
			 
			 
			 
   sampling_number ++;
   sampling_number_10s++;
   t =	account_current(ADC_LRING_CURR);		//qz modify 20181120,��Ψ��������Ϊ0.47ŷ,������Ϊ2ŷ, 2/0.47=4.25
   l_linshi_current += t;
	l_linshi_current_1s += t ;
	
   t = 	account_current(ADC_RRING_CURR);
   r_linshi_current += t;
   r_linshi_current_1s += t;
   
   t = 	account_current(ADC_MB_CURR) ;
   m_linshi_current += t;
   m_linshi_current_1s += t;
   
   t =	account_current(ADC_FAN_CURR);
   dust_linshi_current += t;
   dust_linshi_current_1s += t;

   t=account_current(ADC_SB_CURRENT);
   sb_temp_current+=t;
   sb_temp_current_1s+=t;
   
   t =	account_current(ADC_BAT_VOL);
   battery_linshi_voltage += t;
   battery_linshi_voltage_1s += t;
   battery_linshi_chargecurrent_10s+=t;
#if 0 //shftemp  
   t =	account_current(BATTERY_TEMP);
 #else
 	t =0;
 #endif   
   battery_linshi_temp += t;
   battery_linshi_temp_1s += t;	
   t =	 account_current(ADC_BAT_CHGCURR);
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
					sb_current=(u16)(sb_temp_current/25000);
					sampling_number = 0;
					battery_linshi_voltage = 0;
					battery_linshi_temp = 0;
					l_linshi_current = 0;
					r_linshi_current = 0;
					dust_linshi_current = 0;
					m_linshi_current = 0;
					battery_linshi_chargecurrent = 0;
					sb_temp_current=0;
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
					sb_current_1s=(u16)(sb_temp_current_1s/5000);
					sampling_number_1s = 0;
					battery_linshi_voltage_1s = 0;
					battery_linshi_temp_1s = 0;
					l_linshi_current_1s = 0;
					r_linshi_current_1s = 0;
					dust_linshi_current_1s = 0;
					m_linshi_current_1s = 0;
					battery_linshi_chargecurrent_1s = 0;
					sb_temp_current_1s=0;
			 }
   if(sampling_number_10s==50000)
   	{
   		battery_voltage_10s=(u16)(battery_linshi_chargecurrent_10s/50000);
		sampling_number_10s=0;
		battery_linshi_chargecurrent_10s=0;
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
	u16 battery_vol;
	//battery_vol=battery_voltage_1s;
	delay_ms(500);
	battery_vol=account_current(ADC_BAT_VOL);
	if(mode.mode==MODE_CHARGE)
		a=(float)(battery_vol-VOL_12_8V)/(float)(full_power-VOL_12_8V);
	else
		a=(float)(battery_vol-VOL_12_8V)/(float)(VOL_16_5V-VOL_12_8V);
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
			Battery.bat_per=100;
			return;
		}
#endif

	//�����ص���Ϊ0,��ʾ֮ǰ�����Ѿ�����û����ߵ�һ�ο���,��ʱ��ص������ɿ�,��Ҫ���¼���
	if((Battery.BatteryCapability==0))
		{
			if((Read_Charge_Dc())|(Read_Charge_Seat()))
				Battery.bat_per=0;			//��������ڳ��,�ϱ�α�ٷֱ�0%
			else
				Battery.bat_per=100;			//������ڳ��,�ϱ�α�ٷֱ�100%
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
#ifdef BAT_CAP_MAX
			if(Battery.BatteryCapability>=CAP_MAX)
				Battery.BatteryCapability=CAP_MAX;
#endif
#ifdef DOCK_POWER
			TRACE("Bat_Cap after cal=%d\r\n",Battery.BatteryCapability);
#endif
			WriteFDCap();
			WriteBatteryCapability();
			Battery.bat_recal=1;
			WriteBatRecal(Battery.bat_recal);

		}
	
#ifdef BAT_CAP_MAX
	//��ص�ѹ����NOPOWER,����Deadģʽ,qz add 20180625
	if((battery_voltage!=0)&(battery_voltage<NOPOWER)&(!Read_Charge_Dc())&(!Read_Charge_Seat()))
		{
			stop_rap();
			Battery.bat_per=0;
			Battery.BatteryChargeForbid = 0;		//qz add 20180703
			//�����ص�ѹС��NOPOWERʱ����طŵ���С�ڵ�ص������򽫷ŵ������Ƹ����������ڶ�̬����
			if((Battery.BatteryFDCap>MAH1000)&(Battery.BatteryFDCap<=CAP_MAX))
				Battery.BatteryCapability=Battery.BatteryFDCap;
			else									//qz add 20180703
				Battery.BatteryFDCap=Battery.BatteryCapability;
			WriteFDCap();
			WriteBatteryCapability();
			Init_Dead();
		}
	
	//�����ǰ��طŵ�������ڵ�ǰ��ص�������ʼ���򵼺����ϱ�1%��ֱ���ŵ�NOPOWER
	//�ҽ��µĵ�طŵ�����ֵ����ص������ŵ���ҪС��2600mAh���������µ�طŵ������µ�ص���
	if((Battery.BatteryFDCap>Battery.BatteryCapability)&&(Battery.BatteryFDCap<CAP_MAX))		
		{											
			Battery.bat_per=1;
			Battery.BatteryCapability=Battery.BatteryFDCap;	//ͬʱ�µĵ�طŵ�������ݱ��뱣�棬���µ�ص���=�µ�طŵ����
			WriteFDCap();
			WriteBatteryCapability();
		}
	//��طŵ�������2600mAh���������
	else if(Battery.BatteryFDCap>CAP_MAX)
		{
			Battery.BatteryFDCap=CAP_MAX;
			Battery.BatteryCapability=CAP_MAX;
			Battery.bat_per=1;
			WriteFDCap();
			WriteBatteryCapability();
		}
#else
	//��ص�ѹ����NOPOWER,����Deadģʽ,qz add 20180625
	if((battery_voltage!=0)&(battery_voltage<NOPOWER)&(!Read_Charge_Dc())&(!Read_Charge_Seat()))
		{
			stop_rap();
			Battery.bat_per=0;
			Battery.BatteryChargeForbid = 0;		//qz add 20180703
			Battery.BatteryCapability=Battery.BatteryFDCap;
			WriteFDCap();
			WriteBatteryCapability();
			Init_Dead();
			return;
		}

	//�����ǰ��طŵ�������ڵ�ǰ��ص�������ʼ���򵼺����ϱ�1%��ֱ���ŵ�NOPOWER
	//�ҽ��µĵ�طŵ�����ֵ����ص������ŵ���ҪС��2600mAh���������µ�طŵ������µ�ص���
	if((Battery.BatteryFDCap>Battery.BatteryCapability))		
		{											
			Battery.bat_per=1;
			Battery.BatteryCapability=Battery.BatteryFDCap; //ͬʱ�µĵ�طŵ�������ݱ��뱣�棬���µ�ص���=�µ�طŵ����
			WriteFDCap();
			WriteBatteryCapability();
		}
#endif
	//qz add end
	//else 
		{
			temp_per=(Battery.BatteryCapability-Battery.BatteryFDCap)*100/Battery.BatteryCapability;	//ʣ���������	
			//if((temp_per<1.0)&&(temp_per>0.0))
			if(temp_per<1.0)
				temp_per=1.0;

			//qz add 20180801����ٷֱ�С�ڵ���20%,���ǵ�ѹ������Ҫ�س��ѹ,���ȵȴ��ϱ�21%
			//qz add 20180803����bat_recal��־λ��Ϊ�������������ڣ�����ѹ���ڻس��ѹ���е���У׼�Ժ�
			//�����ѹ�ָ��ڻس��ѹ�ˣ���ֹ�ٷֱȳ��ַ�����
			//qz add 20180902����100/79��Ŀ����Ϊ�˰ٷֱȼ���ﵽ21%�����ǵ�ѹ�����ڻس��ѹ����ʱ�ŵ����ڲ������ӣ�
			//���ǵ�ص������ֲ��䣬��ô����Ժ󣬵�ذٷֱȻ�Ѹ���½��ܶࡣ
			if((temp_per<=20.0)&(!temp_power)&(mode.mode!=MODE_CHARGE)&(!Battery.bat_recal))
				{
					temp_per=21.0;

#if 1				
					//qz add 20180902
					temp_data_float=(float)(Battery.BatteryFDCap)*100/79;		//���µ�ص�������Ϊ�˿̵�ص���ƫС
					temp_data_u32=(u32)(temp_data_float);
#ifdef BAT_CAP_MAX
					if(temp_data_u32>CAP_MAX)
						temp_data_u32=CAP_MAX;
#endif
					Battery.BatteryCapability=temp_data_u32;
					WriteBatteryCapability();
					//qz add end
#endif
				}
				
			Battery.bat_per=(s8)(temp_per);
		}

	if((Battery.bat_recal)&(battery_voltage>VOL_14_6V))
		{
			Battery.bat_recal=0;
			WriteBatRecal(Battery.bat_recal);
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
	#if 1
	switch(mode.mode)
		{
			case MODE_CEASE: 
				switch(mode.sub_mode)
					{
						case SUBMODE_SLEEP:
							curr=(CURR_CEASE);
							break;
						case SUBMODE_CEASE:
						case SUBMODE_ERR:
							curr=(CURR_CEASE);
							break;
						case SUBMODE_QUITCHARGE:
						case SUBMODE_SELF_TEST:
							curr=CURR_WORK;
						break;
					}
				break;
			case MODE_YBS:				
			case MODE_DOCK:
			case MODE_SHIFT:
			case MODE_SWEEP:
			case MODE_EXIT:
			case MODE_PASS2INIT:	
			case MODE_CTRL:
			case MODE_SPOT:
				curr=CURR_WORK;
					break ;
			case MODE_CHARGE:
				switch(power.step)
						{
							case 0:
								curr=0;
								break;
							case 1:
							case 2:
							case 4:
							case 5: //qz add 20180615
									curr = 250;//100;//150;		//��ʼ��������ʱ����Ȼ�̶�����Ϊ360mA,ʵ�ʲⶨΪ250mA(20200120)
									break;
							case 3:
									curr = 700;//600;		//��������ʱ����Ȼ�̶�����Ϊ800mA,ʵ�ʲⶨΪ250mA(20200120)
								break;
							case 6:
									curr = battery_chargecurrent_1s;
								break;
						}
				break;
			default:
				curr=CURR_CEASE;
				break;
		}
	#else
	if(mode.mode!=MODE_CHARGE)
		{
			curr+=MAIN_CUR;
		}
	#endif
	//------------------------------------
	//���������֡���ɨ���������
	
	//curr=curr+(u32)(l_current_1s*RING_MB_CUR_CAL)+(u32)(r_current_1s*RING_MB_CUR_CAL)+(u32)(m_current_1s*RING_MB_CUR_CAL)+(u32)(dust_current_1s*RING_MB_CUR_CAL);	//qz modify 20181120:����������
	curr=curr+(u32)(l_current_1s*CURR_RING_CNT_mA)+(u32)(r_current_1s*CURR_RING_CNT_mA);
	curr=curr+(u32)(m_current_1s*CURR_MB_CNT_mA)+(u32)(dust_current_1s*CURR_FAN_CNT_mA);
	curr=curr+(u32)(sb_current_1s*CURR_SB_CNT_mA);
	
	//if(UV_FLAG)				//qz add 20180902:����UV����
		//curr=curr+140;		

	TRACE("curr=%d\r\n",curr);

	t = t*curr;
	if(t > 2600)
		{
			//return;
		}
	
	
	if(mode.mode == MODE_CHARGE)							//�������ڳ��ģʽ��
			{
				if(!flag_full)			//�����δ����
					{
						if(Battery.BatteryFDCap>t)			//����t
							{
								Battery.BatteryFDCap-=t;
							}
						else if(Battery.BatteryFDCap>1)		//С��t�Ҵ���1
							{
								Battery.BatteryFDCap--;
							}
						else								//С�ڻ��ߵ���1
							{
								Battery.BatteryFDCap=1;
							}
					}
				//��س������Ժ����������
				//qz add end
			}
	else 
			{
				//if(Battery.BatteryFDCap < 0x83d600)//qz mask 20180625
#ifdef BAT_CAP_MAX
				if(Battery.BatteryFDCap<CAP_MAX)	//qz add 20180625
#endif
					Battery.BatteryFDCap += t;
				//���������������APP_BAT_HANDLE������Ҳ�м���
			}
	if((((Battery.BatteryFDCap *10)/Battery.BatteryCapability) > 2)&&(Battery.BatteryChargeForbid == 1))
			{
					Battery.BatteryChargeForbid = 0;
			}

	//qz add 2080803,17V���Ͽ�ʼ�Ķ�����������������㣬����ʱ����һֱΪ100%
	if((mode.mode!=MODE_CHARGE)&(Battery.BatteryFDCap<=t)&(battery_voltage>VOL_17V))
		{
			Battery.BatteryFDCap=0;
		}
			
	WriteFDCap();
#if 0		//qz mask 20180625
	//if((battery_voltage < LOWPOWER) && (mode.mode == MODE_CEASE))		//qz mask 20180615
	if((battery_voltage < CUTPOWER) && (mode.mode != MODE_CHARGE))	//qz add 20180615
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

	charge_data.real_current=(charge_data.piv_current_sum)/charge_data.piv_current_count*CURR_CHG_CNT;
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

void Parse_LowPower2Dock(void)
{

	if(giv_sys_time-Battery.start_time<400000)
		return;


	if((Battery.bat_per<=20)&(!mode.low_power))
		{
			Send_Voice(VOICE_POWER_LOW);
			mode.low_power=true;
			switch(mode.mode)
				{
					case MODE_SWEEP:
					case MODE_PASS2INIT:
					case MODE_EXIT:
					case MODE_SHIFT:
						if(motion1.start_seat)
							{								
								stop_rap();
								delay_ms(1000);
								Send_Voice(VOICE_DOCK_START);
								motion1.force_dock=true;
								Force_Dock();
							}
						else
							{
								stop_rap();
								Init_Cease();
							}
						break;
					case MODE_YBS:
					case MODE_SPOT:
						if(motion1.start_seat)
							{
								stop_rap();
								Init_Docking();
							}
						else
							{
								stop_rap();
								Init_Cease();
							}
						break;
					default:
						break;
				}
		}

	if((mode.low_power)&(Battery.bat_per>=40))
		mode.low_power=false;
}


void Reset_Bat_Data(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
   
	PWR_BackupAccessCmd(ENABLE);
	
	BKP_DeInit();

	PWR_BackupAccessCmd(DISABLE);
}
