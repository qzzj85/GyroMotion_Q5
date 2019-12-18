#ifndef MOTION_H
#define MOTION_H

extern short F_Angle_Const,B_Angle_Const,L_Angle_Const,R_Angle_Const;//机器开始清扫时的前向角度和后向角度，固定不变
extern short LF_Angle_Const,RF_Angle_Const,LB_Angle_Const,RB_Angle_Const;

extern MOTION motion1;			//qz add

short get_gyro_angle(void);
short Get_Back_Angle(void);
void Change_LeftRight(void);
u8 Read_LeftRight(void);
void Set_Motion_BackSweep(u8 back_sweep);
u8 Read_Motion_BackSweep(void);
void Set_Motion_Acc(u8 x_acc,u8 y_acc);
u8 Read_Motion_YAcc(void);
void Assign_LeftRight(void);
s8 Read_Motion_YDir(void);
void Set_LeftRight(u8 leftright);
void Set_Motion_YDir(s8 ydir);
void Cal_PosArea_Max(void);
void Get_Const_Angle(void);
short Get_Reverse_Angle(short forward_angle);
u8 Analysis_LastYClean(void);
u8 Set_Motion_YDir_Reverse(void);
u8 Analysis_Back_Leak(void);
u8 Analysis_StopBack_InBump(s8 ydir,s8 now_gridx,s8 now_gridy);
void Set_AreaWorkTime(u32 min_num);
void Work_TimeOut_Handle(void);
u8 Analysis_InSeatArea(s8 now_gridx,s8 now_gridy);
u8 Analysis_LastYClean_II(void);

u8 Analysis_Check_Dock(void);
u8 Force_Dock(void);
u8 Analysis_Stop_StartArea(void);

#endif
