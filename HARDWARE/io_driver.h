#ifndef	 __IO_DRIVER_H
#define  __IO_DRIVER_H

////////////////////////////////////ͷ�ļ�///////////////////
#include "stm32f10x.h"
#include "soft.h"

//////////////////////�������///////////////////////////////
void Init_Io_Driver(void);
void Io_Driver(void);
void Enable_Beep(void);
void Beep_Driver(void);
void Enable_Beep_Five(void);
void Enable_Beep_number(u8 number);
//////////////////////�������///////////////////////////////
extern bool gbv_action_uv;	/*����uv�Ƶ�״̬�����ⲿģ���ȡ�͸ı�*/
extern bool gbv_action_s_motor;/*���ñ�ɨ����״̬�����ⲿģ���ȡ�͸ı�*/
extern bool gbv_action_m_motor;/*������ɨ����״̬�����ⲿģ���ȡ�͸ı�*/ 
extern bool gbv_action_dust_motor;	/*���ûҳ������״̬�����ⲿģ���ȡ�͸ı�*/

extern bool pbv_state_m_motor;/*ָʾ��ǰ��ɨ����״̬*/
extern bool pbv_state_m_fangxiang;/*ָʾ��ǰ��ɨ����״̬*/

extern bool pbv_state_s_motor;/*ָʾ��ǰ��ɨ����״̬*/
extern bool pbv_state_dust_motor;/*ָʾ��ǰ�ҳ������״̬*/
extern bool gbv_action_m_fangxiang;//��ɨ�����˶�����FALSE ��ʾ��ǰ��TRUE��ʾ���
////////////////////��ģ�����,����ģ���ṩ�ĺ���///////////

#endif

