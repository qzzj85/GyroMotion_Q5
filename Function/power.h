#ifndef  __POWER_H
#define  __POWER_H

#include "stm32f10x.h"
#include "soft.h"

/*
////////////////����PWM������Ĺ�ϵ///////////////
qz add 20180823
FAN PWM					FAN CUR
100
200
300
400						100mA
500						140mA
600						180mA
700						220mA
800						250mA
900						300mA
1000					350mA
1100					410mA
1200					480mA
1300					520mA
1400					580mA
1500					650mA
1600					720mA
1700					800mA
1800					860mA
1900					940mA
2000					1000mA
////////////////////////////////////////////////
*/

#define MAIN_PCB_CURRENT		50


/////���ֵ���,��λ��mA////////////
#define	SLEEP_CUR			200
#define	CEASE_CUR			220
#define	MAIN_CUR			260		//��50mA�ĵ�/ǽ/��/�ٵ���
#define FAN_SILENCE_CUR		250		//FAN_SILENCE_PWM:800
#define	FAN_STANDARD_CUR	720		//FAN_STANDARD_PWM:1600
#define	FAN_FORCE_CUR		1000		//FAN_FORCE_PWM:2000
#define	SB_SILENCE_CUR		30
#define	SB_DOCK_CUR			30
#define	SB_STANDARD_CUR		40
#define	SB_FORCE_CUR		40
#define	RING_MB_CUR_CAL		0.171418	//3.3/4096/0.47*100

#define	CURR_CEASE_NOWIFI	35		//35mA,����,ǽ�ؼ�ر�,WIFI�ر�
#define	CURR_CEASE			50		//35mA,����,ǽ�ؼ�ر�,WIFI�ر�
#define	CURR_WORK			120		//120mA,����,ǽ�ؼ��,WIFI��

////////�����ǵ�ص�ѹ����///////////////////////
#define 	CHARGE_PWM_MASK		0
#define 	CURR_CHG_CNT		0.402832	//piv_current/4096*3.3/20/0.1R*1000=real_current(unit:mA)
#define		VOLT_CHG_CNT		0.008862	//piv_voltgage/4096*3.3*11=real_voltage(unit:V)
#define		CURR_RING_CNT		0.001714	//ring_current/4096*3.3/0.47R
#define		CURR_FAN_CNT		0.008057	//fan_current/4096*3.3/0.1R
#define		CURR_MB_CNT			0.008057	//mb_current/4096*3.3/0.1R
#define		CURR_SB_CNT			0.000171	//sb_current/4096*3.3/4.7R

#define		CURR_RING_CNT_mA	1.714179
#define		CURR_FAN_CNT_mA		8.056641
#define		CURR_MB_CNT_mA		8.056641
#define		CURR_SB_CNT_mA		0.171419
/////// //17.1V~1929   17.5V~1974	16.9V~1909	17.3~1952
//16.8~1896  17~1919
#define		NUMBer_4_LI_BATTERY_0_Point_1_V		451.4285			//(1896/16.8)*4




#define LOWPOWER_BEEP_NUM 10
////////////#define NOPOWER             1399   //��ص�ѹ11V����Ϊ����	1241~11.0v	�õ�   1399~12.4v
////////////#define LOWPOWER            1557   //��ѹ1.1v	1489~13.2v			 	�õ�   1557~13.8v			
////////////#define FULLPOWER           1895   //��ѹ1.4v

//qz add 20180615
#define	CUTPOWER			1489	//	13.2V
#define	RETURNPWR			1534	//13.6V
//#define NOPOWER            	NUMBer_4_LI_BATTERY_0_Point_1_V * 3.0
#define NOPOWER				1444	//12.8V
#define LOWPOWER            NUMBer_4_LI_BATTERY_0_Point_1_V * 3.45       //��ѹ1.1v	1489~13.2v			 	�õ�   1557~13.8v	
#define FULLPOWER           NUMBer_4_LI_BATTERY_0_Point_1_V * 4.1

#define ONEPOWER            1624   //��ѹ1.2v
#define TWOPOWER            1760   //��ѹ1.3v
//#define FULLPOWER_CHARGE    2058   //���ʱ�������ѹ   1.52V   18.24V 
//#define TWOPOWER_CHARGE     1949   //���ʱ�������ѹ   1.44V   17.28V
//#define ONEPOWER_CHARGE     1895   //���ʱ��һ���ѹ   1.40V   16.80V
//#define FULLPOWER_CEASE     1692   //����ʱ�������ѹ   1.25V   15.00V
//#define TWOPOWER_CEASE      1651   //����ʱ�������ѹ   1.22V   14.64V
//#define ONEPOWER_CEASE      1624   //����ʱ��һ���ѹ   1.20V   14.40V
//#define FULLPOWER_WORK      1651   //����ʱ�������ѹ   1.22V   14.64V
//#define TWOPOWER_WORK       1624   //����ʱ�������ѹ   1.20V   14.40V
//#define ONEPOWER_WORK       1584   //����ʱ��һ���ѹ   1.17V   14.04V
#define	MAH2600				0x8ED280
#define	MAH2400				0x83D600
#define	MAH2000				0x6ddd00
#define	MAH1800				0x62E080
#define	MAH1600				0x57E400
#define	MAH1000				0X36EE80

#define VOL_12_8V			1444 //�൱��NOPOWER
#define	VOL_13V				1354
#define	VOL_14_6V			1650
#define	VOL_15V				1692
#define	VOL_15_8V			1783
#define	VOL_16_2V			1827
#define	VOL_16_5V			1861
#define	VOL_16_8V			1895
#define	VOL_17V				1918
#define	VOL_17_1V			1930
#define	VOL_17_25V			1946


extern uint16 piv_current;
extern uint16 piv_voltage;

void 	judge_charge(void);
void 	chargeing(void);
void 	init_power (void);
void  	sampling_temp_voltage(void);
void 		Get_Dispower(void);
u8 		Get_BatteryTemp(void);
void 	ChargeControl(void);
void	AccountCapability(void);
void	AccountCapabilityReal(void);
s8 	 Get_APPBat(void);
void APP_BAT_Handle(void);
void Battery_Reinit(void);
u8 Check_Return_Pwr(void);
void Battery_Data_Init(void);

extern bool VOL_TEMP_ready;
extern u16  jt_chargecurrent;   //��̬ʱ�������ĵ���
extern u16  battery_chargecurrent;//���5���ڵ�ƽ������
extern u16  battery_voltage;    //���1��ĵ�ѹ
extern u16  battery_temp;       //���1����¶�
extern u16  l_current;          //���ֵ���
extern u16  r_current;          //���ֵ���
extern u16  dust_current;       //�ҳ��������
extern u16  m_current;          //��ɨ����
extern u16 	sb_current;

extern u16  battery_chargecurrent_1s;//���1���ڵ�ƽ������
extern u16  battery_voltage_1s;    //���1��ĵ�ѹ
extern u16  battery_temp_1s;       //���1����¶�
extern u16  l_current_1s;          //���ֵ���
extern u16  r_current_1s;          //���ֵ���
extern u16  dust_current_1s;       //�ҳ��������
extern u16  m_current_1s;          //��ɨ����
extern u16  sb_current_1s;

extern u16  battery_voltage_10s;


extern u16 full_power;

extern POWER power ;
extern BATTERY Battery;
extern CHARGE_DATA charge_data;
extern const u16 temp[];

extern bool CHARGE_READY;
extern bool  flag_full;
extern bool dc_nobat_run;	//�޵��,DCֱ���־λ
void Init_Charge_Data(void);
void Charge_PID_Ctr(u32 tgt_current);
void ChargeControl_My(void);
void  AccountCapabilityReal_My(void);
u8 Check_Left_Ring_Cur(void);
void Charge_PID_CtrPwr(u32 tgt_voltage);
void ChargeControl_Volt_My(void);
void Parse_LowPower2Dock(void);
void Reset_Bat_Data(void);
void Reinit_Battery(void);
void Init_Battery_Data(void);

#endif
