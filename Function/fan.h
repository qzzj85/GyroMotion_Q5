#ifndef FAN_H
#define	FAN_H

#define FAN_KP	20/100	//80/100
#define	FAN_KI	1/100	//10/100
#define	FAN_KD	1/100	//10/100

extern bool fanspd_flag;
extern u32 fanspd_cnt;
extern int fanspd,fanspd_tgt;
extern int fan_pwm;
void Init_FanSpd_Interrupt(void);
void FanSpd_Cal(void);
void Reset_FanSpd_Data(void);
void FanSpd_PID(void);

#endif
