#include "AAA-include.h"

LED led;

//qz add 20181210
void Init_Led(void)
{
	led.status=LED_STA_OFF;
	led.last_status=LED_STA_OFF;
	led.green_sta=false;
	led.red_sta=false;
	led.quic_flag=false;
	led.slow_flag=false;
	led.led_time=false;
	led.show_time=0;
	led.start_time=giv_sys_time;
	led.last_show_time=0;
}

void LED_Handle(void)
{
#ifdef TUYA_WIFI
	if (wifi_led_begin == 1)
       return;
#endif	

	if(!led.led_time)
		return;
	led.led_time=false;
	switch (led.status)
		{
			case LED_STA_REDON_CONST:
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{	
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						//led.status=LED_STA_OFF;
						return;
					}
				LED_RED_ON;
				LED_GREEN_OFF;
				break;
			case LED_STA_REDON_SLOW:
				LED_GREEN_OFF;
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						//led.status=LED_STA_OFF;
						return;
					}
				if(led.slow_flag)
					{
						led.slow_flag=false;
						if(led.red_sta)
							{
								LED_RED_OFF;
								led.red_sta=false;
							}
						else
							{
								LED_RED_ON;
								led.red_sta=true;
							}
					}
				break;
			case LED_STA_REDON_QUIC:
				LED_GREEN_OFF;
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						//led.status=LED_STA_OFF;
						return;
					}
				if(led.quic_flag)
					{
						led.quic_flag=false;
						if(led.red_sta)
							{
								LED_RED_OFF;
								led.red_sta=false;
							}
						else
							{
								LED_RED_ON;
								led.red_sta=true;
							}
					}
				break;
			case LED_STA_GREENON_CONST:
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						//led.status=LED_STA_OFF;
						return;
					}
				LED_GREEN_ON;
				LED_RED_OFF;
				break;
			case LED_STA_GREENON_SLOW:
				LED_RED_OFF;
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						//led.status=LED_STA_OFF;
						return;
					}
				if(led.slow_flag)
					{
						led.slow_flag=false;
						if(led.green_sta)
							{
								LED_GREEN_OFF;
								led.green_sta=false;
							}
						else
							{
								LED_GREEN_ON;
								led.green_sta=true;
							}
					}
				break;
			case LED_STA_GREENON_QUIC:
				LED_RED_OFF;
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						//led.status=LED_STA_OFF;
						return;
					}
				if(led.quic_flag)
					{
						led.quic_flag=false;
						if(led.green_sta)
							{
								LED_GREEN_OFF;
								led.green_sta=false;
							}
						else
							{
								LED_GREEN_ON;
								led.green_sta=true;
							}
					}
				break;
			case LED_STA_OFF:
				LED_RED_OFF;
				LED_GREEN_OFF;
				led.green_sta=false;
				led.red_sta=false;
				led.show_time=0;
				break;
		}
}

/*-----------´ò¿ªLEDº¯Êý--------------
Input Prama£º
redorgreen: 0,RED
            1,GREEN
show_time:  LED show time,if show_time is 0,means always show
sloworquic: 0,ALWAYS SHOW (CONST)
            1,SLOW
            2,QUICK
--------------------------------------*/            
void Open_Led(u8 redorgreen,u32 show_time,u8 sloworquic)
{
	led.last_status=led.status;
	led.last_show_time=led.show_time;
	if(!redorgreen)
		{
			switch(sloworquic)
				{
					case 0:
						led.status=LED_STA_REDON_CONST;
						break;
					case 1:
						led.status=LED_STA_REDON_SLOW;
						break;
					case 2:
						led.status=LED_STA_REDON_QUIC;
						break;
				}
			led.show_time=show_time;
			led.start_time=giv_sys_time;
		}
	else
		{
			switch(sloworquic)
				{
					case 0:
						led.status=LED_STA_GREENON_CONST;
						break;
					case 1:
						led.status=LED_STA_GREENON_SLOW;
						break;
					case 2:
						led.status=LED_STA_GREENON_QUIC;
						break;
				}
			led.show_time=show_time;
			led.start_time=giv_sys_time;
		}	
}

void Close_Led(void)
{
	led.status=LED_STA_OFF;
	led.show_time=0;
}
