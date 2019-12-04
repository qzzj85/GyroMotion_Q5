#ifndef LED_H
#define LED_H

#define 	LED_STA_OFF 			0
#define 	LED_STA_REDON_SLOW		1
#define 	LED_STA_REDON_QUIC		2
#define		LED_STA_REDON_CONST		3
#define		LED_STA_GREENON_SLOW	4
#define		LED_STA_GREENON_QUIC	5
#define		LED_STA_GREENON_CONST	6
#define		LED_STA_RED_GREEN		7

typedef struct
{
	bool led_time;
	bool sec_flag;
	bool quic_flag;
	bool halfsec_flag;
	bool red_sta;
	bool green_sta;
	u8 	 status;
	u8   last_status;
	u32	 red_green_time;
	u32  show_time;
	u32  last_show_time;
	u32  start_time;
	u32	 step;
}LED;

#ifdef TUYA_WIFI
	extern unsigned char  wifi_led_begin ;
#endif	


extern LED led;


void Do_Led(u8 which2do);
void LED_Handle(void);
void Open_Led(u8 redorgreen,u32 show_time,u8 sloworquic);
void Close_Led(void);
#endif
