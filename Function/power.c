//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

////////////////////////头文件////////////////////////////////////
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
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////

#define 	C_KP			0.5//0.025
#define		C_KI			0.1//0.1
#define		C_KD			0.01//0.01//0.0125//0.0125

POWER power ;
BATTERY	Battery;
CHARGE_DATA charge_data;	//qz add 20180522
bool CHARGE_READY;			//qz add 20180522

bool VOL_TEMP_ready;
u16  jt_chargecurrent;   //静态时主板所耗电流
u16  battery_chargecurrent;//电池5秒内的平均电流
u16  battery_voltage;    //电池1秒的电压
u16  battery_temp;       //电池1秒的温度
u16  l_current;          //左轮电流
u16  r_current;          //右轮电流
u16  dust_current;       //灰尘风机电流
u16  m_current;          //中扫电流	
u16  sb_current;

u16  battery_chargecurrent_1s;//电池1秒内的平均电流
u16  battery_voltage_1s;    //电池1秒的电压
u16  battery_temp_1s;       //电池1秒的温度
u16  l_current_1s;          //左轮电流
u16  r_current_1s;          //右轮电流
u16  dust_current_1s;       //灰尘风机电流
u16  m_current_1s;          //中扫电流
u16  sb_current_1s;
bool dc_nobat_run=false;	//不用电池,DC直插跑机标志位
u16  battery_voltage_10s=0;	//电池10s的电压值
u32  battery_voltage_10s_temp=0;
bool  flag_full = false;			//电池充满电标志
///////////////////////私有变量////////////////////////////////////
uint16 piv_current;
uint16 piv_voltage;

u16 full_power=VOL_16_8V;

///////////////////////全局函数////////////////////////////////////	
///////////////////////私有函数////////////////////////////////////
///////////////////////函数实体////////////////////////////////////
/**************************************************************
程序功能：初始化电池电源
输入：无
输出：无
说明：初始化与电源有关的io，并且判断电池是否有NTC。
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
  battery_chargecurrent = jt_chargecurrent;//电池5秒内的平均电流	
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
	//电池重新校验时,因为导航板会消耗电压，所以等到导航板启动完成后(发送心跳)
	//再开始使用电压百分比计算电量和放电量,在此之前,上传给导航板的电量百分比为假性百分比
	//假性百分比在APP_BAT_HANDLE函数中,充电状态为0,非充电状态为100
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
			//Battery.BatteryCapability=MAH2600; 	//qz modify 20180801:原来为MAH2600//2400mAh
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

	BKP_WriteBackupRegister(BAT_REINIT_BKP, 0xAA55);		//备份域标志
	BKP_WriteBackupRegister(VOICE_BACKUP_BKP, 0x00EC);		//音量备份(默认第13级）。预约时间为没有
	BKP_WriteBackupRegister(PREEN_BACKUP_BKP, 0x0000);		//预约天数为没有
	BKP_WriteBackupRegister(MODE_BACKUP_BKP, 0x0101);		//BKP_DR4用于模式保存，高16位为主模式，低16位为子模式。
	BKP_WriteBackupRegister(BATCAP_HADDR_BKP, 0x0); 		//BKP_DR5用于电池容量的高16位
	BKP_WriteBackupRegister(BATCAP_LADDR_BKP, 0x0); 		//BKP_DR6用于电池容量的低16位
	BKP_WriteBackupRegister(BATFDCAP_HADDR_BKP, 0x0);			//BKP_DR7用于电池放电量的高16位
	BKP_WriteBackupRegister(BATFDCAP_LADDR_BKP, 0x0);			//BKP_DR8用于电池放电量的低16位
	BKP_WriteBackupRegister(BATRECAL_BACKUP_BKP, 0X0);			//BKP_DR9用于电池RECAL标志,qz add 20181101
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
	Battery.BatteryCapability=DEFAULT_CAP;	//qz modify 20180801:原来为MAH2600//2400mAh
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
	if(Battery.BatteryCapability == 0)				//如果电池电量数据为0，表示需要根据电压重新初始化电量
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
	if(data!=0x5A)												//第一次开机或者DS1307电池没电了
		{
			DS1307_Data_Reinit();
			Battery.reinit=true;		//电池电量重置标志
#ifdef DEBUG_INIT
			TRACE("DS1307 Data Reinit!\r\n");
#endif
		}
#else
	u16 data1;
	PWR_BackupAccessCmd(ENABLE);
	data1=BKP_ReadBackupRegister(BAT_REINIT_BKP);
	if(data1!=0xAA55)										//第一次开机或者电池没电了
		{
			BKP_WriteBackupRegister(BAT_REINIT_BKP, 0xAA55);		//备份域标志
			BKP_WriteBackupRegister(VOICE_BACKUP_BKP, 0x00EC);		//音量备份(默认第13级）。预约时间为没有
			BKP_WriteBackupRegister(PREEN_BACKUP_BKP, 0x0000);		//预约天数为没有
			BKP_WriteBackupRegister(MODE_BACKUP_BKP, 0x0101);		//BKP_DR4用于模式保存，高16位为主模式，低16位为子模式。
			BKP_WriteBackupRegister(BATCAP_HADDR_BKP, 0x0);			//BKP_DR5用于电池容量的高16位
			BKP_WriteBackupRegister(BATCAP_LADDR_BKP, 0x0);			//BKP_DR6用于电池容量的低16位
			BKP_WriteBackupRegister(BATFDCAP_HADDR_BKP, 0x0);			//BKP_DR7用于电池放电量的高16位
			BKP_WriteBackupRegister(BATFDCAP_LADDR_BKP, 0x0);			//BKP_DR8用于电池放电量的低16位
			BKP_WriteBackupRegister(BATRECAL_BACKUP_BKP, 0X0);			//BKP_DR9用于电池RECAL标志,qz add 20181101
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
	if(!Battery.reinit) 									//电池电量不需要重置
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
			if(Battery.BatteryCapability == 0)				//如果电池电量数据为0，表示需要根据电压重新初始化电量
				{
					Battery.reinit=true;					//电池重新初始化电量标志置1
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

	//////////////////充电控制//////////////////////////////////////

	piv_current = account_current(ADC_BAT_CHGCURR);	 //采样当时的电流
	piv_voltage = account_current(ADC_BAT_VOL);   //采样当时的电压
	charge_data.piv_current_sum+=piv_current;
	charge_data.piv_current_count++;
	charge_data.piv_voltage_sum+=piv_voltage;
	charge_data.piv_voltage_count++;
	switch(power.step)
	{
		///////开始充电前的准备/////////////
		case 0:
			power.time = giv_sys_time;
			power.step = 10;
			disable_pwm(CHARGE_PWM);
			return;

		///////为防止插电开机导致的静态电流计算错误,
		///////特在此等待40s,用于导航板开机再行计算
		///////qz add 20180625
		case 10:
			if(giv_sys_time-power.time>30000)
			{
				if(battery_voltage_1s<VOL_13V)			//如果电池电压小于13V,先涓流充电
					{
						power.step=1;
						Battery.BatteryChargeForbid = 0;	//允许大电流充电
					}
				else if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > VOL_16_5V))
					{
						power.step=4;
					}
				else
					{
						Battery.BatteryChargeForbid = 0;	//允许大电流充电
						power.step=2;
					}
				Init_Charge_Data();
				jt_chargecurrent=battery_chargecurrent_1s;
#ifdef DEBUG_CHARGE
				TRACE("jt_curr=%d\r\n",(u32)(jt_chargecurrent*CURR_CHG_CNT));
#endif
				flag_full = false;					//电池未充满
				power.time=giv_sys_time;
			}
			
			break;
		/////////////////////////接上电极10秒钟，察看电池是否接上
		///////电压小于13V时，进行360mA-静态电流的恢复充电
		case 1:
			Charge_PID_Ctr(360);		//qz modify 150->360:固定360mA
			if((giv_sys_time-power.time>100000)&(battery_voltage>VOL_13V))
				{
					power.step++;
					power.time=giv_sys_time;
					Init_Charge_Data();
				}
			break; 		//qz modify return->break 20180823
			
		//使用180ma充电10s并且电池温度低于40度，看电池是否允许大电流充电，不允许则进行涓流充电
		case 2:
		//充电电流大于150MA则降低PWM值
			Charge_PID_Ctr(360);		//qz modify 150->360:固定360mA
			if(giv_sys_time-power.time>100000)
				{
					power.step++;
					power.time = giv_sys_time;
					power.step_time=giv_sys_time;
				}
			break;	
			
		//电池限流限压的过程，判断电池转为涓流的条件是：
		//1、电池充电电流小于360ma；
		//2、电池的绝对温度大于50度
		//3、在电池温度大于40度的情况下，滇池温度每分钟上升速度大于1.5度
		case 3:
		Charge_PID_Ctr(800);
		//判断电池的绝对温度大于50度或者电池电压大于21伏,电池转为涓流充电	 2369
			if((battery_voltage > full_power))	//qz modify 20180703:1805 16V 1861:16.5V
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//电池的充电电流小于360ma,进入小恒流充电
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //电流小于360ma
			&&((giv_sys_time - power.time) > 600000))
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//在大电流充电4小时以上，强制性进行涓流充电。
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

		//涓流充电
		case 5:
#if 0
			Init_Charge_Data();
			power.time=giv_sys_time;
			power.step++;
#else
		//充电电流大于180MA则降低PWM值
			Charge_PID_Ctr(360);	//qz modify 150->360:固定360mA
			////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;			
#ifdef DEBUG_CHARGE
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CURR_CHG_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CURR_CHG_CNT));
#endif
				//充电电流小于静态电流+100mA,则转入恒压充电
				if((((battery_chargecurrent - jt_chargecurrent) < 248)) //100mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	防止第1步直接到第4步时，刚开始因为充电PWM没开，充电电流为0，小于52mA 
				&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
				{
					Init_Charge_Data();
					Battery.BatteryChargeForbid = 1;
					power.step++;
					power.time=giv_sys_time;
					return;
				}
	
				//涓充大于600分钟认为电池充满电
				if(((giv_sys_time - power.time) > 360000000)		//600分钟  
					&&(flag_full==false))						//qz add 20180910
					
				{
					flag_full = true;
#ifdef DEBUG_CHARGE
					TRACE("because Time out in step5!\r\n");
#endif
					//qz add 20180710 
					//如果电池充满时,电池放电量还大于1200(mAs),则当前电池电量和放电量明显偏大,需要调整
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,绿灯常亮5s.qz modify 不限时常亮
					power.step=7;
					return;
				}

			//涓流充电电压大于16.8V,认为充电已满
			//qz add 20180710
			//实测1882(16.67V)时,电池进入保护
			//涓流充电电压大于16.8V,进入恒压充电
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

		//恒压充电
		case 6:
		//充电电流大于180MA则降低PWM值
			Charge_PID_CtrPwr(full_power);	//qz modify 150->360:固定360mA
			Battery.BatteryChargeForbid = 1;	//恒压充电,禁止大电流充电
			////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
#ifdef DEBUG_CHARGE
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CURR_CHG_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CURR_CHG_CNT));
#endif

#if 1
				if((((battery_chargecurrent - jt_chargecurrent) < 62)) //25mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	防止第1步直接到第4步时，刚开始因为充电PWM没开，充电电流为0，小于52mA 
				&& (flag_full == false))
				{

#ifdef DEBUG_CHARGE
					TRACE("because overcurrent out in step6!\r\n");
#endif
					flag_full = true;

					//qz add 20180710 
					//如果电池充满时,电池放电量还大于1200(mAs),则当前电池电量和放电量明显偏大,需要调整
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,绿灯常亮5s.qz modify 不限时常亮
					power.step++;
				}
#endif
			//涓充大于120分钟或者充电电压大于17V认为电池充满电
				if((((giv_sys_time - power.time) > 72000000)|(battery_voltage>VOL_17V))&&(!flag_full))		//120分钟  
				{
					flag_full = true;
#ifdef DEBUG_CHARGE
					TRACE("because Time out in step6!\r\n");
#endif
					//qz add 20180710 
					//如果电池充满时,电池放电量还大于1200(mAs),则当前电池电量和放电量明显偏大,需要调整
					if((Battery.BatteryFDCap>=1200)&(Battery.BatteryCapability>(2*Battery.BatteryFDCap)))
						{
							Battery.BatteryCapability-=Battery.BatteryFDCap;
							WriteBatteryCapability();	
						}
					//qz add end
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
					Send_Voice(VOICE_CHARGING_FULL);
					Open_Led(1,0,0);	//qz add 20181120,绿灯常亮5s.qz modify 不限时常亮
					power.step++;			//qz add 20180910
				}
			}
			break;
		case 7:
			//电池充满电，PWM关闭后，电池会自放电，长时间(比如一个月)后，电池需补充电量
				if((flag_full == true)&&(battery_voltage_1s < VOL_15_8V))		//15.8V
				{
					power.step = 3;
					flag_full = false;
					power.time = giv_sys_time;
					Init_Charge_Data(); 	//qz add 20180522
					Send_Voice(VOICE_CHARGING_START);
				}
		
			//防止电池保护板坏时一直充电，而充坏电池  (2015.4.7)
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
		enable_pwm(CHARGE_PWM,power.pwm);	//执行pwm动作
	}		
}




/************************************************
功能：判断是否有外部电源插入。
输入：无
输出：无
************************************************/
void judge_charge(void)
{
static u32 time0,time1,time2,time3;
////////////////////判断dc座充电输入/////////////////
#if 1
	if(Read_Charge_Seat())			//充电座有信号
		{
			time1=giv_sys_time;
			if(!power.charge_seat)	//充电座标志之前无效
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
			if(power.charge_seat)	//充电座标志之前有效
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
功能：温度采样，1秒钟采样一次，采样10次计算平均值保存，
*************************************************************/
void  sampling_temp_voltage(void)
{
static u32	l_linshi_current = 0,               //左轮的临时电流
            r_linshi_current = 0,               //右轮的临时电流
			m_linshi_current = 0,               //中扫的临时电流
			dust_linshi_current = 0,            //灰尘风机的临时电流
			sb_temp_current=0,
            battery_linshi_voltage = 0,         //电池的临时电压          
			battery_linshi_temp = 0,            //电池的临时温度
            battery_linshi_chargecurrent = 0;//电池5秒内的平均电流
			  
static u16 sampling_number = 0;                //采样次数 

static u32	l_linshi_current_1s = 0,               //左轮的临时电流
            r_linshi_current_1s = 0,               //右轮的临时电流
			m_linshi_current_1s = 0,               //中扫的临时电流
			dust_linshi_current_1s = 0,            //灰尘风机的临时电流
			sb_temp_current_1s=0,
            battery_linshi_voltage_1s = 0,         //电池的临时电压          
			battery_linshi_temp_1s = 0,            //电池的临时温度
            battery_linshi_chargecurrent_1s = 0;//电池1秒内的平均电流
			  
static u16 sampling_number_1s = 0;                //采样次数

static u32 battery_linshi_chargecurrent_10s=0;
static u32 sampling_number_10s=0;
u32 t;
   if(VOL_TEMP_ready == true)
			 {VOL_TEMP_ready = false;}
   else
			 {return;}
			 
			 
			 
			 
   sampling_number ++;
   sampling_number_10s++;
   t =	account_current(ADC_LRING_CURR);		//qz modify 20181120,冠唯采样电阻为0.47欧,贝莱恩为2欧, 2/0.47=4.25
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
					battery_voltage = (u16) (battery_linshi_voltage/25000);    //电池1秒的电压
					battery_temp = (u16) (battery_linshi_temp/25000);       //电池1秒的温度
					l_current = (u16) (l_linshi_current/25000);          //左轮电流
					r_current = (u16) (r_linshi_current/25000);          //右轮电流
					dust_current = (u16) (dust_linshi_current/25000);       //灰尘风机电流
					m_current = (u16) (m_linshi_current/25000);          //中扫电流
					battery_chargecurrent = (u16)(battery_linshi_chargecurrent/25000);//充电平均电流
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
					battery_voltage_1s = (u16) (battery_linshi_voltage_1s/5000);    //电池1秒的电压
					battery_temp_1s = (u16) (battery_linshi_temp_1s/5000);       //电池1秒的温度
					l_current_1s = (u16) (l_linshi_current_1s/5000);          //左轮电流
					r_current_1s = (u16) (r_linshi_current_1s/5000);          //右轮电流
					dust_current_1s = (u16) (dust_linshi_current_1s/5000);       //灰尘风机电流
					m_current_1s = (u16) (m_linshi_current_1s/5000);          //中扫电流
					battery_chargecurrent_1s = (u16)(battery_linshi_chargecurrent_1s/5000);//充电平均电流
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
功能: 读取电池温度
返回: 电池的温度值
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
	//13.5V到16.5V为电池电压区间，按百分比计算
	//13.5V的AD值为0x62A,16.5V的AD值为0x77B

	//修改为11V到16.5V为电池电压区间
	//11V的AD值为0X509,16.5V的AD值为0X77B

	//修改为12.8V到16.8V为电池电压区间
	//12.8V的AD值为1444,16.8V的AD值为1890

	//修改为12.8V到16.8V为电池电压区间
	//12.8V的AD值为1444,充电状态满值电压为16.8V(1890),非充电状态满值为16.4V(1850)

	//20190709
	//新的专用惯导板，相关采样参数有改变
	//12.8V的AD值为2647,充电状态满值电压为16.8V(3475),非充电状态满值为16.4V(3392)
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

//qz add APP显示的电池电量日常处理
void APP_BAT_Handle(void)
{
	u16 temp_power=0;
	float temp_per,temp_data_float;
	u32 temp_data_u32;

	//qz add 20181011
	//开机20s之内，不进入此函数
	//目的是从ADC取电池电压需要时间，防止误判断
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

	//如果电池电量为0,表示之前机器已经彻底没电或者第一次开机,此时电池电量不可靠,需要重新计算
	if((Battery.BatteryCapability==0))
		{
			if((Read_Charge_Dc())|(Read_Charge_Seat()))
				Battery.bat_per=0;			//如果现在在充电,上报伪百分比0%
			else
				Battery.bat_per=100;			//如果不在充电,上报伪百分比100%
			return; 						//qz add 20180801,直接退出等容量及放电量计算完毕后再进行下面的操作
		}
	
	//Check_Return_Pwr用于连续检测30秒内电池电压是否低于RETURN_PWR(13.6V)
	temp_power=Check_Return_Pwr();

	//电池电压低于RETURNPWR,直接由现有的放电量倒推算电量,同时20%会导致导航板回充 qz add 20180801
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
	//电池电压低于NOPOWER,进入Dead模式,qz add 20180625
	if((battery_voltage!=0)&(battery_voltage<NOPOWER)&(!Read_Charge_Dc())&(!Read_Charge_Seat()))
		{
			stop_rap();
			Battery.bat_per=0;
			Battery.BatteryChargeForbid = 0;		//qz add 20180703
			//如果电池电压小于NOPOWER时，电池放电量小于电池电量，则将放电量复制给电量，用于动态调整
			if((Battery.BatteryFDCap>MAH1000)&(Battery.BatteryFDCap<=CAP_MAX))
				Battery.BatteryCapability=Battery.BatteryFDCap;
			else									//qz add 20180703
				Battery.BatteryFDCap=Battery.BatteryCapability;
			WriteFDCap();
			WriteBatteryCapability();
			Init_Dead();
		}
	
	//如果当前电池放电电量大于当前电池电量，则始终向导航板上报1%，直到撑到NOPOWER
	//且将新的电池放电量赋值给电池电量（放电量要小于2600mAh），保存新电池放电量和新电池电量
	if((Battery.BatteryFDCap>Battery.BatteryCapability)&&(Battery.BatteryFDCap<CAP_MAX))		
		{											
			Battery.bat_per=1;
			Battery.BatteryCapability=Battery.BatteryFDCap;	//同时新的电池放电电量数据必须保存，且新电池电量=新电池放电电量
			WriteFDCap();
			WriteBatteryCapability();
		}
	//电池放电量大于2600mAh，不予理会
	else if(Battery.BatteryFDCap>CAP_MAX)
		{
			Battery.BatteryFDCap=CAP_MAX;
			Battery.BatteryCapability=CAP_MAX;
			Battery.bat_per=1;
			WriteFDCap();
			WriteBatteryCapability();
		}
#else
	//电池电压低于NOPOWER,进入Dead模式,qz add 20180625
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

	//如果当前电池放电电量大于当前电池电量，则始终向导航板上报1%，直到撑到NOPOWER
	//且将新的电池放电量赋值给电池电量（放电量要小于2600mAh），保存新电池放电量和新电池电量
	if((Battery.BatteryFDCap>Battery.BatteryCapability))		
		{											
			Battery.bat_per=1;
			Battery.BatteryCapability=Battery.BatteryFDCap; //同时新的电池放电电量数据必须保存，且新电池电量=新电池放电电量
			WriteFDCap();
			WriteBatteryCapability();
		}
#endif
	//qz add end
	//else 
		{
			temp_per=(Battery.BatteryCapability-Battery.BatteryFDCap)*100/Battery.BatteryCapability;	//剩余电量计算	
			//if((temp_per<1.0)&&(temp_per>0.0))
			if(temp_per<1.0)
				temp_per=1.0;

			//qz add 20180801如果百分比小于等于20%,但是电压大于需要回充电压,则先等待上报21%
			//qz add 20180803加入bat_recal标志位做为限制条件是用于，当电压低于回充电压进行电量校准以后，
			//如果电压又高于回充电压了，防止百分比出现反复。
			//qz add 20180902加入100/79的目的是为了百分比计算达到21%，但是电压还高于回充电压，此时放电量在不断增加，
			//但是电池电量保持不变，那么充电以后，电池百分比会迅速下降很多。
			if((temp_per<=20.0)&(!temp_power)&(mode.mode!=MODE_CHARGE)&(!Battery.bat_recal))
				{
					temp_per=21.0;

#if 1				
					//qz add 20180902
					temp_data_float=(float)(Battery.BatteryFDCap)*100/79;		//更新电池电量，因为此刻电池电量偏小
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
功能：每相隔1秒计算电池剩余容量
***************************************************/
void AccountCapability(void)
{
	if(Battery.UsePowerTime == Rtc_time)
		{return ;}
//	AccountCapabilityReal();
#ifdef DC_NOBAT_RUN
	if(dc_nobat_run)		//如果是插入DC头来跑,无需计算电池电量
		return;
#endif
	AccountCapabilityReal_My();
}

/****************************************************
功能：计算机器使用的电池电量
输入：无
输出：
返回：无
说明：通过计算在各种模式下工作时间来判断使用的电量
      改变计数值
	  Battery.UsePowerTime    
	  Battery.BatteryFDCap
	  
	  在电池电压低于13.2v时，电池认为已经放完电，并且将放电容量存入备份寄存器
****************************************************/
void  AccountCapabilityReal_My(void)
{
//static u8 entercapability = 0;
//	char i;
u32 t;//耗电时间
u32 curr;//耗电电流
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
									curr = 250;//100;//150;		//起始及涓流充电时，虽然固定电流为360mA,实际测定为250mA(20200120)
									break;
							case 3:
									curr = 700;//600;		//大电流充电时，虽然固定电流为800mA,实际测定为250mA(20200120)
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
	//加上左右轮、中扫、风机电流
	
	//curr=curr+(u32)(l_current_1s*RING_MB_CUR_CAL)+(u32)(r_current_1s*RING_MB_CUR_CAL)+(u32)(m_current_1s*RING_MB_CUR_CAL)+(u32)(dust_current_1s*RING_MB_CUR_CAL);	//qz modify 20181120:加入风机电流
	curr=curr+(u32)(l_current_1s*CURR_RING_CNT_mA)+(u32)(r_current_1s*CURR_RING_CNT_mA);
	curr=curr+(u32)(m_current_1s*CURR_MB_CNT_mA)+(u32)(dust_current_1s*CURR_FAN_CNT_mA);
	curr=curr+(u32)(sb_current_1s*CURR_SB_CNT_mA);
	
	//if(UV_FLAG)				//qz add 20180902:加入UV电流
		//curr=curr+140;		

	TRACE("curr=%d\r\n",curr);

	t = t*curr;
	if(t > 2600)
		{
			//return;
		}
	
	
	if(mode.mode == MODE_CHARGE)							//机器处于充电模式中
			{
				if(!flag_full)			//电池尚未充满
					{
						if(Battery.BatteryFDCap>t)			//大于t
							{
								Battery.BatteryFDCap-=t;
							}
						else if(Battery.BatteryFDCap>1)		//小于t且大于1
							{
								Battery.BatteryFDCap--;
							}
						else								//小于或者等于1
							{
								Battery.BatteryFDCap=1;
							}
					}
				//电池充满电以后不算电量消耗
				//qz add end
			}
	else 
			{
				//if(Battery.BatteryFDCap < 0x83d600)//qz mask 20180625
#ifdef BAT_CAP_MAX
				if(Battery.BatteryFDCap<CAP_MAX)	//qz add 20180625
#endif
					Battery.BatteryFDCap += t;
				//电量的其他情况在APP_BAT_HANDLE函数中也有计算
			}
	if((((Battery.BatteryFDCap *10)/Battery.BatteryCapability) > 2)&&(Battery.BatteryChargeForbid == 1))
			{
					Battery.BatteryChargeForbid = 0;
			}

	//qz add 2080803,17V以上开始的动作，不计入电量计算，即此时电量一直为100%
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

								//大于2000mAh且小于2600mAh
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

/*---检测30s内电压是否低于RETURN_PWR-----
//qz add 20180803
输出：
	0:30s内电压没有低于回充电压
	1:30s内电压全部低于回充电压
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
				if(giv_sys_time-check_time>300000)	//1min之内,battery_voltage低于RETURNPWR
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
