#ifndef	 __IO_DRIVER_H
#define  __IO_DRIVER_H

////////////////////////////////////头文件///////////////////
#include "stm32f10x.h"
#include "soft.h"

//////////////////////输出函数///////////////////////////////
void Init_Io_Driver(void);
void Io_Driver(void);
void Enable_Beep(void);
void Beep_Driver(void);
void Enable_Beep_Five(void);
void Enable_Beep_number(u8 number);
//////////////////////输出变量///////////////////////////////
extern bool gbv_action_uv;	/*设置uv灯的状态，由外部模块读取和改变*/
extern bool gbv_action_s_motor;/*设置边扫马达的状态，由外部模块读取和改变*/
extern bool gbv_action_m_motor;/*设置中扫马达的状态，由外部模块读取和改变*/ 
extern bool gbv_action_dust_motor;	/*设置灰尘风机的状态，由外部模块读取和改变*/

extern bool pbv_state_m_motor;/*指示当前中扫马达的状态*/
extern bool pbv_state_m_fangxiang;/*指示当前中扫马达的状态*/

extern bool pbv_state_s_motor;/*指示当前边扫马达的状态*/
extern bool pbv_state_dust_motor;/*指示当前灰尘风机的状态*/
extern bool gbv_action_m_fangxiang;//中扫马达的运动方向，FALSE 表示向前，TRUE表示向后
////////////////////由模块调用,其他模块提供的函数///////////

#endif

