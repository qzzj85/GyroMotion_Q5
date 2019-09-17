#ifndef MOTION_H
#define MOTION_H

extern short F_Angle_Const,B_Angle_Const,L_Angle_Const,R_Angle_Const;//机器开始清扫时的前向角度和后向角度，固定不变

short get_gyro_angle(void);
short Get_Back_Angle(void);
void Change_LeftRight(void);
u8 Read_LeftRight(void);
void Set_Motion_BackSweep(u8 back_sweep);
u8 Read_Motion_BackSweep(void);
void Set_Motion_Acc(u8 x_acc,u8 y_acc);
u8 Read_Motion_YAcc(void);
void Assign_LeftRight(u8 reverse);
s8 Read_Motion_YDir(void);
void Set_LeftRight(u8 leftright);
void Set_Motion_YDir(s8 ydir);
void Cal_PosArea_Max(void);
void Get_Const_Angle(void);
short Get_Reverse_Angle(short forward_angle);
u8 Analysis_LastYClean(void);
u8 Set_Motion_YDir_Reverse(void);
u8 Analysis_Back_Leak(void);



#endif
