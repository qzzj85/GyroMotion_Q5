#ifndef LEAKSWEEP_H
#define LEAKSWEEP_H

#define LEAK_BACK_ENABLE

void LeakSweep_Bump_Action(void);
void Init_LeakSweep(short tgt_yaw);
void Do_LeakSweep(void);
void Init_Pass2LeakSweep(void);
void Do_Pass2LeakSweep(void);
void Init_Stop_LeakBack(void);
void Do_Stop_LeakBack(void);
void Do_Leak_BackSweep(void);
void Init_Leak_BackSweep(short tgt_yaw);


#endif
