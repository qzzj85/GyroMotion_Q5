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

bool POWER_ready;
bool VOL_TEMP_ready;
bool VOL_TEMP_READY_MY;
bool get_dispow_flag;
u16  jt_chargecurrent;   //静态时主板所耗电流
u16  battery_chargecurrent;//电池5秒内的平均电流
u16  battery_voltage;    //电池1秒的电压
unsigned char battery_low_beep_flag = 1;
u16  battery_temp;       //电池1秒的温度
u16  l_current;          //左轮电流
u16  r_current;          //右轮电流
u16  dust_current;       //灰尘风机电流
u16  m_current;          //中扫电流	

u16  battery_chargecurrent_1s;//电池1秒内的平均电流
u16  battery_voltage_1s;    //电池1秒的电压
u16  battery_temp_1s;       //电池1秒的温度
u16  l_current_1s;          //左轮电流
u16  r_current_1s;          //右轮电流
u16  dust_current_1s;       //灰尘风机电流
u16  m_current_1s;          //中扫电流
bool dc_nobat_run=false;	//不用电池,DC直插跑机标志位

u16  l_current_50ms=0;
u16	 r_current_50ms=0;
u32  sample_number_50ms=0;
u32  l_temp_curr_50ms=0;
u32  r_temp_curr_50ms=0;
bool ms_curr_flag=false;	//毫秒级电流准备好标志
u32 l_curr_buf[10]={0,0,0,0,0,0,0,0,0,0};
u32 r_curr_buf[10]={0,0,0,0,0,0,0,0,0,0};


bool  flag_full = false;			//电池充满电标志
u8  bat_recal = false;				//底盘主动请求回充标志
///////////////////////私有变量////////////////////////////////////
uint16 piv_current;
uint16 piv_voltage;
u32    real_chg_current;
u32    real_bat_voltage;

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
  power.voltage = account_current(BATTERY_VOLTAGE);


  battery_voltage = power.voltage;
  //battery_temp = power.temp;
 #if 1//shftemp   
  jt_chargecurrent = account_current(CHARGE_CURRENT);
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
#ifdef NEW_PWR_CAL
			Battery.BatteryCapability=MAH2600; 	//qz modify 20180801:原来为MAH2600//2400mAh
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
功能： 充电控制
输入：无
输出：无
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
功能：充电控制
************************************************/
#if 1
	void ChargeControl(void)
	{
	
	 static uint8  flag_full = false;			//电池充满电标志		
		//////////////////充电控制//////////////////////////////////////
		
		piv_current = account_current(CHARGE_CURRENT);	 //采样当时的电流
		piv_voltage = account_current(BATTERY_VOLTAGE);   //采样当时的电压
		real_chg_current= (u32)(piv_current*CHG_CUR_CNT);
		real_bat_voltage= (u32)(piv_voltage*CHG_VOL_CNT);
		
		switch(power.step)
		{
			///////开始充电前的准备/////////////
			case 0:
				power.time = giv_sys_time;
				power.step = 1;
				disable_pwm(CHARGE_PWM);
				return;
			/////////////////////////接上电极10秒钟，察看电池是否接上
			case 1:
				if((giv_sys_time - power.time) > 20000)//接触上电2秒钟
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
					//如果电池充电标志为1，则直接进入第4步。
	//				if(Battery.BatteryChargeForbid != 0)
	//				{
	//					power.step = 4;
	//				}
					if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > 1827)) //15.8V~1783   16.2V~1827
					{
						power.step = 4;
					}
					else//处理机器长时间放置不用，电池自放电造成电量严重减少时，需要进行大电流充电
					{
						Battery.BatteryChargeForbid = 0;	//允许大电流充电
					}
	
					flag_full = false;					//
				}
				return; 
				
		//使用180ma充电10s并且电池温度低于40度，看电池是否允许大电流充电，不允许则进行涓流充电
			case 2:
			//充电电流大于150MA则降低PWM值
				if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 372))
				{
					if(power.pwm > 0)
					{
						power.pwm --;						//电流大于设定电流
					}
				}
				else
				{
					
					if(power.pwm < (PWM_CHARGE_MAX-CHARGE_PWM_MASK))
					{
						power.pwm ++;						//电流小于设定电流
					}
				}
				if(((giv_sys_time - power.time) > 100000)&&(battery_temp > temp[70])) //充电时间为5分钟并且电池温度低于40度
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
				
		//电池限流限压的过程，判断电池转为涓流的条件是：
		//1、电池充电电流小于360ma；
		//2、电池的绝对温度大于50度
		//3、在电池温度大于40度的情况下，滇池温度每分钟上升速度大于1.5度
			case 3:
			//充电电流大于设定电流则降低PWM值  
				if(((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 1986)))		 //800mA
				{
					if(power.pwm > 0)
					{
						power.pwm --;						//电流大于设定电流
					}
				}
				else
				{
					
					if(power.pwm < (PWM_CHARGE_MAX-CHARGE_PWM_MASK))
					{
						power.pwm ++;						//电流小于设定电流
					}
				}
			//判断电池的绝对温度大于50度或者电池电压大于21伏,电池转为涓流充电	 2369
				if((battery_temp < temp[80])|| (battery_voltage > 1919))  //17.1V~1929	 17.5V~1974 16.9V~1909	17.3~1952  16.8~1896  17~1919
				{
					power.step = 4;
					power.time = giv_sys_time;
				}
			//电池的充电电流小于360ma认为电池已经充满
				if(((battery_chargecurrent < jt_chargecurrent) 
				|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //电流小于360ma
				&&((giv_sys_time - power.time) > 600000))
				{
					power.step = 4;
					power.time = giv_sys_time;
				}
			//在大电流充电3小时以上，强制性将电池置为充饱状态。
				if((giv_sys_time - power.time) > 180000000)
				{
					power.step = 4;
					power.time = giv_sys_time;	  
					Battery.BatteryFDCap = 0;
					Battery.BatteryChargeForbid = 1;
				}
			////////每分钟判断一次////////////////////////
				if(gbv_minute != false)
				{
					gbv_minute = false;
					//piv_temp = Get_BatteryTemp();
				//电池的绝对温度大于40度，
					if(battery_temp < temp[70])
					{
				    //电池的温升速度大于1.5度
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
			//充电电流大于180MA则降低PWM值
	//			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 372))
				if((piv_voltage > 1909)|((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 1986)))
				{
					if(power.pwm > 0)
					{
						power.pwm --;						//电流大于设定电流
					}
				}
				else
				{
					
					if(power.pwm < (PWM_CHARGE_MAX-CHARGE_PWM_MASK))
					{
						power.pwm ++;						//电流小于设定电流
					}
				}
				////////每分钟判断一次////////////////////////
				if(gbv_minute != false)
				{
					gbv_minute = false;
	//				if((battery_temp > temp[70])//电池的绝对温度小于40度，充电电流大于100ma
	//				&& ((battery_chargecurrent > jt_chargecurrent)&&((battery_chargecurrent - jt_chargecurrent) > 248))
	//				&& (Battery.BatteryChargeForbid == 0))//电池电压小于15v
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
					&& ((giv_sys_time - power.time) > 100000)//10s  防止第1步直接到第4步时，刚开始因为充电PWM没开，充电电流为0，小于52mA 
					&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
					{
						flag_full = true;
						
						Battery.BatteryFDCap = 0;
						Battery.BatteryChargeForbid = 1;
					}
				//涓充大于600分钟认为电池充满电
					if(((giv_sys_time - power.time) > 360000000))		//600分钟
					{
						flag_full = true;
	
						Battery.BatteryFDCap = 0;
						Battery.BatteryChargeForbid = 1;
					}
				//电池充满电，PWM关闭后，电池会自放电，长时间(比如一个月)后，电池需补充电量
					if((flag_full == true)&&(battery_voltage_1s < 1783))		//15.8V
					{
						power.step = 3;
						flag_full = false;
						power.time = giv_sys_time;	
					}
				}
	
			//防止电池保护板坏时一直充电，而充坏电池  (2015.4.7)
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
			enable_pwm(CHARGE_PWM,power.pwm);	//执行pwm动作
		}		
	}
//=======================================================================================================
//=======================================================================================================


//piv_current/4096*3.3/20/0.1R*1000=real_current(unit:mA)
//piv_current*0.402832=real_current(unit:mA)
void ChargeControl_My(void)
{

	//////////////////充电控制//////////////////////////////////////

	piv_current = account_current(CHARGE_CURRENT);	 //采样当时的电流
	piv_voltage = account_current(BATTERY_VOLTAGE);   //采样当时的电压
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
			if(giv_sys_time-power.time>400000)
				power.step=1;
			break;
		/////////////////////////接上电极10秒钟，察看电池是否接上
		case 1:
			if((giv_sys_time - power.time) > 20000)//接触上电2秒钟
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

				//如果电池电压大于16.2V，且不允许进行大电流充电，则进入第四步——涓流充电
				if((Battery.BatteryChargeForbid != 0)&&(battery_voltage_1s > 1827)) //15.8V~1783   16.2V~1827
				{
					power.step = 4;
				}
				else//处理机器长时间放置不用，电池自放电造成电量严重减少时，需要进行大电流充电
				{
					Battery.BatteryChargeForbid = 0;	//允许大电流充电
				}

				flag_full = false;					//电池未充满
			}
			return; 
			
	//使用180ma充电10s并且电池温度低于40度，看电池是否允许大电流充电，不允许则进行涓流充电
		case 2:
		//充电电流大于150MA则降低PWM值
		Charge_PID_Ctr(360);		//qz modify 150->360:固定360mA
			if(((giv_sys_time - power.time) > 100000)&&(battery_temp > temp[70])) //充电时间为5分钟并且电池温度低于40度
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
			
			
	//电池限流限压的过程，判断电池转为涓流的条件是：
	//1、电池充电电流小于360ma；
	//2、电池的绝对温度大于50度
	//3、在电池温度大于40度的情况下，滇池温度每分钟上升速度大于1.5度
		case 3:
		//充电电流大于设定电流则降低PWM值
		#if 1
		Charge_PID_Ctr(800);
		
		#endif
//		power.pwm=1625;	
		//判断电池的绝对温度大于50度或者电池电压大于21伏,电池转为涓流充电	 2369
			if((battery_temp < temp[80])|| (battery_voltage > 1861))	//qz modify 20180703:1805 16V 1861:16.5V
			//if((battery_temp < temp[80])|| (battery_voltage > 1919))  //17.1V~1929	 17.5V~1974 16.9V~1909	17.3~1952  16.8~1896  17~1919
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//电池的充电电流小于360ma认为电池已经充满
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //电流小于360ma
			&&((giv_sys_time - power.time) > 600000))
			{
				power.step = 4;
				power.time = giv_sys_time;
			}
		//在大电流充电3小时以上，强制性将电池置为充饱状态。
			if((giv_sys_time - power.time) > 144000000)		//qz modify 144000000 4h
			{
				power.step = 4;
				power.time = giv_sys_time;	  
				//Battery.BatteryFDCap = 0;						//qz mask 20180625
				Battery.BatteryChargeForbid = 1;
			}
		////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
				//piv_temp = Get_BatteryTemp();
			//电池的绝对温度大于40度，
				if(battery_temp < temp[70])
				{
				//电池的温升速度大于1.5度
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
		//充电电流大于180MA则降低PWM值
			Charge_PID_Ctr(360);	//qz modify 150->360:固定360mA
			////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
#ifdef DEBUG_CHARGE
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CHG_CUR_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CHG_CUR_CNT));
#endif

				if((((battery_chargecurrent - jt_chargecurrent) < 248)) //100mA(0.02C)
				&& ((giv_sys_time - power.time) > 100000)//10s	防止第1步直接到第4步时，刚开始因为充电PWM没开，充电电流为0，小于52mA 
				&& (flag_full == false))
	//				&& (Battery.BatteryChargeForbid == 0))		
				{

#ifdef DEBUG_CHARGE
					TRACE("because overcurrent out!\r\n");
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
				}
			//涓充大于600分钟认为电池充满电
				if(((giv_sys_time - power.time) > 360000000))		//600分钟  
				{
					flag_full = true;
#ifdef DEBUG_CHARGE
					TRACE("because Time out!\r\n");
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
				}

			//涓流充电电压大于16.8V,认为充电已满
			//qz add 20180710
			//实测1882(16.67V)时,电池进入保护
			if(((giv_sys_time-power.time>100000)&&(battery_voltage>1880)&&(!flag_full)))
				{
#ifdef DEBUG_CHARGE
					TRACE("Because overvolt out!\r\n");
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
				}
			//qz add end
			
			//电池充满电，PWM关闭后，电池会自放电，长时间(比如一个月)后，电池需补充电量
				if((flag_full == true)&&(battery_voltage_1s < 1783))		//15.8V
				{
					power.step = 3;
					flag_full = false;
					power.time = giv_sys_time;
					Init_Charge_Data();		//qz add 20180522
					Send_Voice(VOICE_CHARGING_START);
				}
			}

		//防止电池保护板坏时一直充电，而充坏电池  (2015.4.7)
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
		enable_pwm(CHARGE_PWM,power.pwm);	//执行pwm动作
	}		
}
#endif

void ChargeControl_Volt_My(void)
{

	//////////////////充电控制//////////////////////////////////////

	piv_current = account_current(CHARGE_CURRENT);	 //采样当时的电流
	piv_voltage = account_current(BATTERY_VOLTAGE);   //采样当时的电压
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
			if(giv_sys_time-power.time>400000)
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
				jt_chargecurrent=battery_chargecurrent;
#ifdef DEBUG_CHARGE
				TRACE("jt_curr=%d\r\n",(u32)(jt_chargecurrent*CHG_CUR_CNT));
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
			if((battery_voltage > VOL_16_8V))	//qz modify 20180703:1805 16V 1861:16.5V
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
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CHG_CUR_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CHG_CUR_CNT));
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

		//恒压充电
		case 6:
		//充电电流大于180MA则降低PWM值
			Charge_PID_CtrPwr(VOL_16_8V);	//qz modify 150->360:固定360mA
			Battery.BatteryChargeForbid = 1;	//恒压充电,禁止大电流充电
			////////每分钟判断一次////////////////////////
			if(gbv_minute != false)
			{
				gbv_minute = false;
#ifdef DEBUG_CHARGE
				TRACE("battery_chargecur=%d\r\n",(u32)(battery_chargecurrent*CHG_CUR_CNT));
				TRACE("jt_chargecurrent=%d\r\n",(u32 )(jt_chargecurrent*CHG_CUR_CNT));
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
            battery_linshi_voltage = 0,         //电池的临时电压          
			battery_linshi_temp = 0,            //电池的临时温度
            battery_linshi_chargecurrent = 0;//电池5秒内的平均电流
			  
static u16 sampling_number = 0;                //采样次数 

static u32	l_linshi_current_1s = 0,               //左轮的临时电流
            r_linshi_current_1s = 0,               //右轮的临时电流
			m_linshi_current_1s = 0,               //中扫的临时电流
			dust_linshi_current_1s = 0,            //灰尘风机的临时电流
            battery_linshi_voltage_1s = 0,         //电池的临时电压          
			battery_linshi_temp_1s = 0,            //电池的临时温度
            battery_linshi_chargecurrent_1s = 0;//电池1秒内的平均电流
			  
static u16 sampling_number_1s = 0;                //采样次数

u32 t;
   if(VOL_TEMP_ready == true)
			 {VOL_TEMP_ready = false;}
   else
			 {return;}
			 
			 
			 
			 
   sampling_number ++;
   t =	account_current(L_CURRENT)/4;		//qz modify 20181120,冠唯采样电阻为0.47欧,贝莱恩为2欧, 2/0.47=4.25
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
					battery_voltage = (u16) (battery_linshi_voltage/25000);    //电池1秒的电压
					battery_temp = (u16) (battery_linshi_temp/25000);       //电池1秒的温度
					l_current = (u16) (l_linshi_current/25000);          //左轮电流
					r_current = (u16) (r_linshi_current/25000);          //右轮电流
					dust_current = (u16) (dust_linshi_current/25000);       //灰尘风机电流
					m_current = (u16) (m_linshi_current/25000);          //中扫电流
					battery_chargecurrent = (u16)(battery_linshi_chargecurrent/25000);//充电平均电流
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
					battery_voltage_1s = (u16) (battery_linshi_voltage_1s/5000);    //电池1秒的电压
					battery_temp_1s = (u16) (battery_linshi_temp_1s/5000);       //电池1秒的温度
					l_current_1s = (u16) (l_linshi_current_1s/5000);          //左轮电流
					r_current_1s = (u16) (r_linshi_current_1s/5000);          //右轮电流
					dust_current_1s = (u16) (dust_linshi_current_1s/5000);       //灰尘风机电流
					m_current_1s = (u16) (m_linshi_current_1s/5000);          //中扫电流
					battery_chargecurrent_1s = (u16)(battery_linshi_chargecurrent_1s/5000);//充电平均电流
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

//因为一个ADC转换通道需要1.75us,转换16个通道且存贮256个数据共需要1.75*256=448us
//所以进入此函数条件VOL_TEMP_READY_MY为500us进入一次
void  sampling_temp_voltage_my(void)
{
static u32 		l_linshi_current = 0,               //左轮的临时电流
              	r_linshi_current = 0,               //右轮的临时电流
				m_linshi_current = 0,               //中扫的临时电流
				dust_linshi_current = 0,            //灰尘风机的临时电流
              	battery_linshi_voltage = 0,         //电池的临时电压          
				battery_linshi_temp = 0,            //电池的临时温度
              	battery_linshi_chargecurrent = 0;//电池5秒内的平均电流
			  
static u16 		sampling_number = 0;                //采样次数 

static u32 		l_linshi_current_1s = 0,               //左轮的临时电流
              	r_linshi_current_1s = 0,               //右轮的临时电流
				m_linshi_current_1s = 0,               //中扫的临时电流
				dust_linshi_current_1s = 0,            //灰尘风机的临时电流
              	battery_linshi_voltage_1s = 0,         //电池的临时电压          
				battery_linshi_temp_1s = 0,            //电池的临时温度
              	battery_linshi_chargecurrent_1s = 0;//电池1秒内的平均电流
			  
static u16 		sampling_number_1s = 0;                //采样次数

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
   if(sampling_number == 10000)		//5s内
			 {
					battery_voltage = (u16) (battery_linshi_voltage/10000);    //电池1秒的电压
					battery_temp = (u16) (battery_linshi_temp/10000);       //电池1秒的温度
					l_current = (u16) (l_linshi_current/10000);          //左轮电流
					r_current = (u16) (r_linshi_current/10000);          //右轮电流
					dust_current = (u16) (dust_linshi_current/10000);       //灰尘风机电流
					m_current = (u16) (m_linshi_current/10000);          //中扫电流
					battery_chargecurrent = (u16)(battery_linshi_chargecurrent/10000);//充电平均电流
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
   if(sampling_number_1s == 2000)		//1s内
			 {
					battery_voltage_1s = (u16) (battery_linshi_voltage_1s/2000);    //电池1秒的电压
					battery_temp_1s = (u16) (battery_linshi_temp_1s/2000);       //电池1秒的温度
					l_current_1s = (u16) (l_linshi_current_1s/2000);          //左轮电流
					r_current_1s = (u16) (r_linshi_current_1s/2000);          //右轮电流
					dust_current_1s = (u16) (dust_linshi_current_1s/2000);       //灰尘风机电流
					m_current_1s = (u16) (m_linshi_current_1s/2000);          //中扫电流
					battery_chargecurrent_1s = (u16)(battery_linshi_chargecurrent_1s/2000);//充电平均电流
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
//===============================================================================================
/****************************************************
功能：根据电池电压判断显示电压
输入：无
输出：
返回：无
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
					
				
	//		if((display == 2)&&(battery_voltage_1s > 1680)) //1680	对应	14.86V	 3.71V		4节锂电池
	//				{display = 3;}
	//				
	//		if((display == 1)&&(battery_voltage_1s > 1600)) //1680	对应	14.86V	 3.71V		4节锂电池	
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

	//以电池电压采样AD：
	//0x509	11.2V
	//0x62A 13.5V
	//0x662 14V
	//0x6d6 15V
	//0x743 16V
	//0x77b 16.5V
	//0x795 16.73V(MAX),
	//以11V到16.5V为电池电压区间,以上数据分析,采样AD值比电压值计算的AD值大0x37~0x39左右
	//插上充电时,比静态电压大0.36~0.38V左右，取0.37V----AD值0x2A。所以充电时,AD值需要大于静态AD约0x2A
	
#if 0
			if(mode.mode==CHARGEING)
				{ 
					if((battery_voltage_1s>=0x790))		//充电电压达到16.73V,所有格亮
						{
							display=4;
						}  
					else if(battery_voltage_1s>=0x6d6)//0x743)	//充电电压16V。第一二三格亮，第四格闪烁
						{
							display=3;
						}
					else if(battery_voltage_1s>=0x600)//0x6d6)	//充电电压15V。第一二格亮，第三格闪烁
						{
							display=2;
						}
					else if(battery_voltage_1s>=0x500)//0x662)	//充电电压14V。第一个亮，第二格闪烁
						{
							display=1;
						}
					else
						display=0;						//第一格闪烁
				}
			else
				{
					if(battery_voltage_1s>=0x743)
						{
							display=4;
						}
					else if(battery_voltage_1s>=0x6d6)	//电池电压15V
						{
							display=3;
						}
					else if(battery_voltage_1s>=0x600)//电池电压13.1V。 0x662) //电池电压14V
						{
							display=2;
						}
					else if(battery_voltage_1s>=0x500)//电池电压11V。0x62a)	//电池电压13.5V
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
			Slam_Data.bat=100;
			return;
		}
#endif

	//如果电池电量为0,表示之前机器已经彻底没电或者第一次开机,此时电池电量不可靠,需要重新计算
	if((Battery.BatteryCapability==0))
		{
			if((Read_Charge_Dc())|(Read_Charge_Seat()))
				Slam_Data.bat=0;			//如果现在在充电,上报伪百分比0%
			else
				Slam_Data.bat=100;			//如果不在充电,上报伪百分比100%
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
	//电池电压低于NOPOWER,进入Dead模式,qz add 20180625
	if((battery_voltage!=0)&(battery_voltage<NOPOWER)&(!Read_Charge_Dc())&(!Read_Charge_Seat()))
		{
			stop_rap();
			Slam_Data.bat=0;
			Battery.BatteryChargeForbid = 0;		//qz add 20180703
			//如果电池电压小于NOPOWER时，电池放电量小于电池电量，则将放电量复制给电量，用于动态调整
			if((Battery.BatteryFDCap>MAH1000)&(Battery.BatteryFDCap<=MAH2600))
				Battery.BatteryCapability=Battery.BatteryFDCap;
			else									//qz add 20180703
				Battery.BatteryFDCap=Battery.BatteryCapability;
			WriteFDCap();
			WriteBatteryCapability();
//			Slam_Data.dipan_req=DIPAN_REQ_TURNOFFSLAM;
			Init_Dead();
		}
	
	//如果当前电池放电电量大于当前电池电量，则始终向导航板上报1%，直到撑到NOPOWER
	//且将新的电池放电量赋值给电池电量（放电量要小于2600mAh），保存新电池放电量和新电池电量
	if((Battery.BatteryFDCap>Battery.BatteryCapability)&&(Battery.BatteryFDCap<MAH2600))		
		{											
			Slam_Data.bat=1;
			Battery.BatteryCapability=Battery.BatteryFDCap;	//同时新的电池放电电量数据必须保存，且新电池电量=新电池放电电量
			WriteFDCap();
			WriteBatteryCapability();
		}
	//电池放电量大于2600mAh，不予理会
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
			temp_per=(Battery.BatteryCapability-Battery.BatteryFDCap)*100/Battery.BatteryCapability;	//剩余电量计算	
			if((temp_per<1.0)&&(temp_per>0.0))
				temp_per=1.0;

			//qz add 20180801如果百分比小于等于20%,但是电压大于需要回充电压,则先等待上报21%
			//qz add 20180803加入bat_recal标志位做为限制条件是用于，当电压低于回充电压进行电量校准以后，
			//如果电压又高于回充电压了，防止百分比出现反复。
			//qz add 20180902加入100/79的目的是为了百分比计算达到21%，但是电压还高于回充电压，此时放电量在不断增加，
			//但是电池电量保持不变，那么充电以后，电池百分比会迅速下降很多。
			if((temp_per<=20.0)&(!temp_power)&(mode.mode!=CHARGEING)&(!Battery.bat_recal))
				{
					temp_per=21.0;

#if 1				
					//qz add 20180902
					temp_data_float=(float)(Battery.BatteryFDCap)*100/79;		//更新电池电量，因为此刻电池电量偏小
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

#define MAIN_PCB_CURRENT		50

void  AccountCapabilityReal(void)
{
static u8 entercapability = 0;
	char i;
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
												curr=((MAIN_CUR+SB_SILENCE_CUR));	//qz modify 20181120:因为风机电流可测，所以取消风机电流
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
											curr = 100;//150;		//起始及涓流充电时，虽然固定电流为360mA,但是导航板占去210mA电流
											break;
									case 3:
											curr = 400;//600;		//大电流充电时，虽然固定电流为800mA,但是导航板占去210mA电流
										break;
									case 6:
											curr = battery_chargecurrent_1s;
										break;
								}
						break;
		}
	//------------------------------------
	//加上左右轮、中扫电流
	curr=curr+(u32)(l_current_1s*RING_MB_CUR_CAL)+(u32)(r_current_1s*RING_MB_CUR_CAL)+(u32)(m_current_1s*RING_MB_CUR_CAL)+(u32)(dust_current_1s*RING_MB_CUR_CAL);	//qz modify 20181120:加入风机电流

	//if(UV_FLAG)				//qz add 20180902:加入UV电流
		//curr=curr+140;		

	t = t*curr;
	if(t > 2600)
		{
				return;
		}
	
	
	if(mode.mode == CHARGEING)							//机器处于充电模式中
			{
				if(!flag_full)			//电池尚未充满
					{
						if(Battery.BatteryFDCap>t)
							{
								Battery.BatteryFDCap-=t;
							}
						else
							{
								Battery.BatteryFDCap--;
								if(Battery.BatteryFDCap<=1)			//电池电量上报始终为99%
									Battery.BatteryFDCap=1;
								//Battery.BatteryCapability+=t;		//电池尚未充满,但放电量已快计算完毕,说明现在电量和放电量偏小,重新计算电量
								//if(Battery.BatteryCapability>=MAH2600)
									//Battery.BatteryCapability=MAH2600;
								//WriteBatteryCapability();			
							}
					}
				//电池充满电以后不算电量消耗
				//qz add end
			}
	else 
			{
				//if(Battery.BatteryFDCap < 0x83d600)//qz mask 20180625
				if(Battery.BatteryFDCap<MAH2600)	//qz add 20180625
					Battery.BatteryFDCap += t;
				//电量的其他情况在APP_BAT_HANDLE函数中也有计算
			}
	if((((Battery.BatteryFDCap *10)/Battery.BatteryCapability) > 2)&&(Battery.BatteryChargeForbid == 1))
			{
					Battery.BatteryChargeForbid = 0;
			}

	//qz add 2080803,17V以上开始的动作，不计入电量计算，即此时电量一直为100%
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


//qz add 20180717，滑动平均滤波
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
//用于检测左轮遇到障碍时，电流是否增大。
//因为在遇到障碍被挂住时，左右轮电流会增大，但是并不是一直都是一个很大的值
//所以判断电流值时，做了一个Buffer，用于滑动平均滤波，来过滤掉那个小值
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

