#ifndef  __ERR_H
#define  __ERR_H


////////////////////////头文件////////////////////////////////////
#include "soft.h"

extern u16 	error_code;

#define ERROR_NODE_MALLOC		0X01
////////////////////////全局变量//////////////////////////////////

#define	ERR_BASE_NUM		0


#define	ERROR_DANGER			ERR_BASE_NUM+1			//地检危险位置
#define	ERROR_BUMPFIX			ERR_BASE_NUM+2			//碰撞故障
#define	ERROR_FANOC		        ERR_BASE_NUM+3			//风机过载
#define	ERROR_MBOC				ERR_BASE_NUM+4			//中扫过载
#define	ERROR_SBOC				ERR_BASE_NUM+5			//边扫过载
#define	ERROR_LEFTRINGOC		ERR_BASE_NUM+6			//左轮过载
#define	ERROR_RIGHTRINGOC		ERR_BASE_NUM+7			//右轮过载
#define	ERROR_RINGOC			ERR_BASE_NUM+8			//双轮过载
#define	ERROR_BAT				ERR_BASE_NUM+9			//电池故障
#define	ERROR_GYRO			 	ERR_BASE_NUM+10			//惯导故障


#define	ERROR_SWEEPER_FIX		ERR_BASE_NUM+11			//机器卡住
#define	ERROR_ROTATE_SKID		ERR_BASE_NUM+12			//旋转打滑故障
#define	ERROR_GYRO_PITCH		ERR_BASE_NUM+13			//惯导俯仰角故障
#define	ERROR_GYRO_ROLL			ERR_BASE_NUM+14			//惯导翻转角故障
#define	ERROR_GYRO_PITCHROLL	ERR_BASE_NUM+15			//惯导俯仰，翻转角故障

#define  ERROR_DCEXIST			ERR_BASE_NUM+16			//未拔掉充电线
#define	ERROR_DOCKFAIL			ERR_BASE_NUM+17			//回充失败

#define ERROR_NODEMALLOC		ERR_BASE_NUM+18			//内存分配失败
#define ERROR_PATHMALLOC		ERR_BASE_NUM+19			//内存分配失败
#define	ERROR_BACKMALLOC		ERR_BASE_NUM+20			//内存分配失败

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
///////////////////////全局函数////////////////////////////////////		 
void Init_Err(void); 
void Do_Err(void);

#endif
