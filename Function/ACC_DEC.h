#ifndef ACC_DEC_H
#define	ACC_DEC_H

//////////////////////////˽�ж���//////////////////////////////////
#define G_KP 80/100
#define	G_KI 10/100
#define G_KD 10/100
#define SPEED_MAX	50//100//800

#define	W_KP	50/100
#define	W_KI	10/100
#define	W_KD 	10/100
#define WALL_SPEED_MAX 200 	
//////////////////////////ȫ�ֱ���//////////////////////////////////
/////////////////////////ȫ�ֺ���////////////////////////////////////
/////////////////////////˽�к���////////////////////////////////////

extern bool gyro_comm_flag;
extern bool spd_acc_flag;
void  ACC_DEC_comm_rap(void);
void  ACC_DEC_Curve(void);
//=======================================================================
///////////////////////����ʵ��////////////////////////////////////
void Check_Speed_My(unsigned int *speed_l,unsigned int *speed_r);
void ACC_DEC_Comm_rap_My(void);
void Gyro_Comm_rap(void);
void Wall_Comm_Rap(void);
#endif
