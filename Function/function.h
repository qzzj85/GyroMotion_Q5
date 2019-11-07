#ifndef  __FUNCTION_H
#define  __FUNCTION_H
////////////////////////头文件////////////////////////////////////	 
#include "stm32f10x.h"										
#include "soft.h"

#define		SB_CHECK_TIME	30000

#define		ABNORMAL_GYROBIOS_L		1
#define		ABNORMAL_GYROBIOS_R		2
#define		ABNORMAL_LSB			14
#define		ABNORMAL_RSB			15
#define		ABNORMAL_SB				16
#define		ABNORMAL_ROTATE_SKID	17
#define		ABNORMAL_LIFT			24
#define		ABNORMAL_BUMP			29
#define		ABNORMAL_RING			30
#define		ABNORMAL_PITCH_RISE		40
#define		ABNORMAL_PITCH_DOWN		41
#define		ABNORMAL_PITCHROLL		42

////////////////////////私有定义//////////////////////////////////
#define M_PROTECTCURRENT 		600//292;//中扫保护电流 600mA 
#define DUST_PROTECTCURRENT 	100 //灰尘风机电流 100mA		
#define WHEEL_PROTECTCURRENT 	800//左右轮电流 800mA 
#define	SB_PROTECTCURRENT		60

////////////////////////全局变量//////////////////////////////////	 
extern MODE mode;		 //当前机器所在的状态 
extern WORK_MODE gsv_work_mode;	 //当前机器所在的工作状态，其取值范围为SWEEP、
                             //SPOT,当机器在其他状态时不能改变，只有在进入SWEE或者PSPOT
							 //才会变为SWEEP或者SPOT
							 
							 	
extern u8 sweep_level;



							 
extern  u8 zhulujing[];
extern  u32 zhulujinglong[];
extern  u32 angle[52];
extern  ACTION action;
extern u8 giv_sys_err;         //系统的错误代码

//qz add 20180316
extern ROTATE_SKID	Rotate_Skid;
extern FREE_SKID	Free_Skid;
#ifdef FREE_SKID_INDEP_CHECK
extern FREE_SKID	Free_Skid_Indep;
#endif
#ifdef FREE_FIRST_BAD_CHECK
extern FREE_SKID	Free_Skid_First;
#endif

#ifdef YBS_DIS_RESTORE
extern ROTATE_ANGLE	rotate_angle;
#endif

extern bool BS_NO_TIME_FLAG;
extern bool CHECK_STATUS_TIME;
extern bool CHECK_STATUS_FLAG;
extern bool GET_DISPLAY_TIME;
///////////////////////全局函数////////////////////////////////////
void Init_Mode(void);
void Action_Mode(void);
u8 Action_Protect(void);
void Turn_UV(void);
void Change_Speed(void);
void Display_Work_Mode(void);
void  Enable_Sweep(void);
void  Disable_Sweep(void);

u32  read_bump(void);
u8 Read_Protect(void);  
void  back(u32 longth);
void  left(u32 longth);
void  right(u32 longth);
void  forward(u32 longth);
void clr_ram(void);
u8 do_action(u8 dm,u32 length);
u8 z_road(void);
u8 louxuan(  u8  f);
u8 read_earth(void);
void yanqiangzou(void);
u8 ybs(void);
u8 ybs_randrm(void);
u8 action_bump(void);
u8 		Check_PreengageTime(void);
void	Init_Action(void);
u8 		CheckMDustMotor(void);


u8 read_sideearth(void);	

u8 		Check_Free_Sikd(void);	//qz add 20180123
u8 SLAM_Tick_Check(void);
void YBS_Comm_Rap_My(void);				//qz add 20180203

void Enable_Free_Skid_Check(void);
void Disable_Free_Skid_Check(void);
u8 Check_Free_Sikd(void);

//qz add 20180316
void Enable_Rotate_Skid_Check(u8 dir);
void Disable_Rotate_Skid_Check(void);
int Check_Gyro_angle(u8 dir);
u8 Check_Rotate_Skid(void);

//qz add 20180319
void Sweep_Level_Set(u16 sweep_level);
void Check_Status(void);
u8 Key_Status_Check_Low(KEY* key);
u8 Key_Status_Check_High(KEY* key);
void Init_Check_Status(void);
u8 SB_FIX_Check(void);
u8 MB_OC_Check(void);
u8 Lift_Check(void);
u8 Bump_Fix_Check(void);
u8 Ring_Fix_Check(void);
u8 Action_Protect_My(u8 abnoraml);
void Dust_Level_Check(void);
void Set_FJ_Level(u16 level);
void Set_BS_Level(u16 level);
void Set_ZS_Level(u16 level);

#ifdef YBS_DIS_RESTORE
void Init_Rotate_Angle(void);
void Enable_Rotate_Angle(void);
void Disable_Rotate_Angle(void);
void Check_Rotate_Angle(void);
#endif

#ifdef FREE_FIRST_BAD_CHECK
void Enable_Free_First_Check(void);
void Disable_Free_First_Check(void);
void Free_First_Check(void);
#endif

u8 Gyro_Pitch_Check(void);
u8 Gyro_Pitch_Speedup(void);
u8 do_action_my(u8 dm,u32 length,short tgt_yaw);
void  enable_rap_yaw(u32 ori_l, u32 length_l,u32 ori_r,u32 length_r,short tgt_yaw);

u8 SB_OC_Check(void);
u8 Gyro_PitchRoll_Check(void);


#endif
