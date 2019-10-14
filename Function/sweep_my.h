#ifndef __SWEEP_MY_H
#define __SWEEP_MY_H
#define POS_BIOS			3
#define	TURN_ANGLE_BIOS		50		//0.5¶È
#define	HORIZON_LENGTH		20

extern short F_Angle,B_Angle,F_Angle_Const,B_Angle_Const,L_Angle_Const,R_Angle_Const;

void Init_NormalSweep(short tgt_yaw);
void Init_Pass2Sweep(void);
void Do_NormalSweep(void);
void Init_Init_Sweep(short tgt_yaw,u8 y_acc,u8 area);
void Continue_Sweep(void);
void Do_Back2YbsStart(void);
void Do_Pass2Sweep(void);
void Init_Back2YbsStart(void);
void Sweep_Bump_Action(u8 ir_enable,u8 out_enable);
void Do_BackSweep(void);
u8 Analysis_NeedBack(s8 ygrid_abort);
u8 Analysis_StopBack(short tgt_yaw);
void Init_Stop_BackSweep(void);
void Do_Stop_BackSweep(void);
void Init_Pass2Init(short tgt_yaw,u8 y_acc,u8 x_acc);
void Do_Pass2Init(void);
void Save_Abort_Data(void);
u8 Check_OutofRange(void);
void Load_Abort_Data(void);
void Init_First_Sweep(u8 start_seat);
u8 Read_Sweep_Bump(u8 ir_enable,u8 out_enable);
void Pass2Sweep_Bump_Action(void);
void Do_Stop_BackSweep(void);
void Init_Sweep_RightYBS(u8 avoid_staright);
void Init_Sweep_LeftYBS(u8 avoid_staright);
void Sweep_YBS(void);
void Init_Back_Sweep(short tgt_yaw);
void StopBack_Bump_Action(void);
u8 YBS_AbortFor_Sweep(void);
u8 Read_Cliff(void);
void Do_FirstInit_Sweep(void);

#endif

