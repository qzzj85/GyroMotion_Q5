#ifndef  __ERR_H
#define  __ERR_H


////////////////////////ͷ�ļ�////////////////////////////////////
#include "soft.h"

extern u16 	error_code;

#define ERROR_NODE_MALLOC		0X01
////////////////////////ȫ�ֱ���//////////////////////////////////

#define	ERR_BASE_NUM		0


#define	ERROR_DANGER			ERR_BASE_NUM+1			//�ؼ�Σ��λ��
#define	ERROR_BUMPFIX			ERR_BASE_NUM+2			//��ײ����
#define	ERROR_FANOC		        ERR_BASE_NUM+3			//�������
#define	ERROR_MBOC				ERR_BASE_NUM+4			//��ɨ����
#define	ERROR_SBOC				ERR_BASE_NUM+5			//��ɨ����
#define	ERROR_LEFTRINGOC		ERR_BASE_NUM+6			//���ֹ���
#define	ERROR_RIGHTRINGOC		ERR_BASE_NUM+7			//���ֹ���
#define	ERROR_RINGOC			ERR_BASE_NUM+8			//˫�ֹ���
#define	ERROR_BAT				ERR_BASE_NUM+9			//��ع���
#define	ERROR_GYRO			 	ERR_BASE_NUM+10			//�ߵ�����


#define	ERROR_SWEEPER_FIX		ERR_BASE_NUM+11			//������ס
#define	ERROR_ROTATE_SKID		ERR_BASE_NUM+12			//��ת�򻬹���
#define	ERROR_GYRO_PITCH		ERR_BASE_NUM+13			//�ߵ������ǹ���
#define	ERROR_GYRO_ROLL			ERR_BASE_NUM+14			//�ߵ���ת�ǹ���
#define	ERROR_GYRO_PITCHROLL	ERR_BASE_NUM+15			//�ߵ���������ת�ǹ���

#define  ERROR_DCEXIST			ERR_BASE_NUM+16			//δ�ε������
#define	ERROR_DOCKFAIL			ERR_BASE_NUM+17			//�س�ʧ��

#define ERROR_NODEMALLOC		ERR_BASE_NUM+18			//�ڴ����ʧ��
#define ERROR_PATHMALLOC		ERR_BASE_NUM+19			//�ڴ����ʧ��
#define	ERROR_BACKMALLOC		ERR_BASE_NUM+20			//�ڴ����ʧ��

#define	ERROR_BOXNONE			ERR_BASE_NUM+21
#define	ERROR_BIRD				ERR_BASE_NUM+22
#define	ERROR_BIRDCOM			ERR_BASE_NUM+23
#define	ERROR_VIDEO				ERR_BASE_NUM+24
#define	ERROR_SLAMCOM			ERR_BASE_NUM+25
#define	ERROR_MOTION_TICK		ERR_BASE_NUM+26
#define	ERROR_MOTION_LIFT		ERR_BASE_NUM+27
#define	ERROR_LEFT_LIFT			ERR_BASE_NUM+28
#define ERROR_RIGHT_LIFT		ERR_BASE_NUM+29
#define	ERROR_LEFT_WALL			ERR_BASE_NUM+30
#define	ERROR_MID_WALL			ERR_BASE_NUM+31
#define	ERROR_RIGHT_WALL		ERR_BASE_NUM+32
#define	ERROR_LEFT_EARTH		ERR_BASE_NUM+33
#define	ERROR_MID_EARTH			ERR_BASE_NUM+34
#define	ERROR_RIGHT_EARTH		ERR_BASE_NUM+35
#define	ERROR_LEFTSBOC			ERR_BASE_NUM+36
#define	ERROR_RIGHTSBOC			ERR_BASE_NUM+37
///////////////////////ȫ�ֺ���////////////////////////////////////		 
void Init_Err(void); 
void Do_Err(void);

#endif
