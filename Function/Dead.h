#ifndef DEAD_H_
#define DEAD_H_
void Init_Dead(void); 
void Do_Dead(void);
void Init_ShutDown(void);

void Enable_ExternInt_Weekup(u8 use_key);
void Disable_ExternInt_Weekup(void);
void Diable_AllExtInt_Weekup(void);

#endif
