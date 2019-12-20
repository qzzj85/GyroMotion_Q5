#ifndef SHIFT_H
#define SHIFT_H

extern bool abort_shiftybs_flag,ybs_near_flag;

void Init_Shift_Point1(u8 pre_action);
void Do_Shift_Point1(void);
void Init_Shift_RightYBS(u8 pre_action);
void Init_Shift_LeftYBS(u8 pre_action);
void Do_ExitAtion(void);
u8 IS_Can_ReachPoint_X(s8 now_gridx,s8 tgt_gridx,s8 now_gridy);
void Init_Shift_Point2(void);
void Do_Shift_Point2(void);
void Do_ShiftYBS(void);
u8 Abort2Sweep(void);
void Init_ShiftExit_RightYBS(u8 pre_action);
void Init_ShiftExit_LeftYBS(u8 pre_action);
void Do_ShiftExit_YBS(void);


#endif
