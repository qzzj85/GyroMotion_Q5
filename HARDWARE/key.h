#ifndef  __KEY_H
#define  __KEY_H
////////////////////////////////////头文件///////////////////
#include "stm32f10x.h"
#include "soft.h"

///////////////////////////////////变量定义//////////////////

void init_key(void);
void AutoReadKey(void);
void Read_Key_High(KEY *key);
u8 Read_Button_Key(KEY *key);
void read_key( KEY *key);

//extern KEY key1,key2,key3,lidi,last_lidi,l_bump,r_bump,charge_dc,charge_seat,s_current,l_last_bump,r_last_bump;
//extern KEY key1,key2,key3,lidi,l_bump,r_bump,charge_dc,charge_seat,s_current,l_last_bump,r_last_bump;	//QZ:MASK
extern KEY key1,key2,key3,l_lidi,r_lidi,l_bump,r_bump,m_bump,charge_dc,charge_seat,s_current,dust_box,dust_sta;	//QZ:add
extern KEY lm_bump,rm_bump;	//qz add 20181120
extern bool key_time;
extern bool boxfullone;
#ifdef TUYA_WIFI
extern bool wifi_time;
extern bool wifi_update_time;
void mcu_wifi_proc_key(void);
void proc_wifi_dock(void);
#endif

#endif

