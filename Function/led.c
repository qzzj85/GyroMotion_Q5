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
	led.sec_flag=false;
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
						led.step=0;
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
						led.step=0;
						return;
					}
				if(led.sec_flag)
					{
						led.sec_flag=false;
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
						led.step=0;
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
						led.step=0;
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
						led.step=0;
						return;
					}
				if(led.sec_flag)
					{
						led.sec_flag=false;
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
						led.step=0;
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
			case LED_STA_ALLON_CONST:
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						led.step=0;
						return;
					}
				LED_GREEN_ON;
				LED_RED_ON;
				led.green_sta=true;
				led.red_sta=true;
				break;
			case LED_STA_ALLON_SLOW:
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						led.step=0;
						return;
					}
				switch(led.step)
					{
						case 0:
							LED_GREEN_OFF;LED_RED_OFF;
							led.green_sta=false;led.red_sta=false;
							led.step++;
							led.sec_flag=false;
							break;
						case 1:
							if(led.sec_flag)
								{
									led.sec_flag=false;
									if(led.green_sta)
										{
											LED_GREEN_OFF;
											LED_RED_OFF;
											led.green_sta=false;
											led.red_sta=false;
										}
									else
										{
											LED_GREEN_ON;
											LED_RED_ON;
											led.green_sta=true;
											led.red_sta=true;
										}
								}
							break;
					}
				break;
			case LED_STA_ALLON_QUIC:
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						led.step=0;
						return;
					}
				switch(led.step)
					{
						case 0:
							LED_GREEN_OFF;LED_RED_OFF;
							led.green_sta=false;led.red_sta=false;
							led.quic_flag=false;
							led.step++;
							break;
						case 1:
							if(led.quic_flag)
								{
									led.quic_flag=false;
									if(led.green_sta)
										{
											LED_GREEN_OFF;
											LED_RED_OFF;
											led.green_sta=false;
											led.red_sta=false;
										}
									else
										{
											LED_GREEN_ON;
											LED_RED_ON;
											led.green_sta=true;
											led.red_sta=true;
										}
								}
							break;
					}
				break;
			case LED_STA_ALTERNATE_SLOW:
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						led.step=0;led.sec_flag=false;
						return;
					}
				switch(led.step)
					{
						case 0:
							LED_GREEN_OFF;LED_RED_OFF;
							led.green_sta=false;led.red_sta=false;
							led.step++;
							led.sec_flag=false;
							break;
						case 1:
							if(led.sec_flag)
								{
									led.sec_flag=false;
									LED_GREEN_ON;led.green_sta=true;
									LED_RED_OFF;led.red_sta=false;
									led.step++;
								}
							break;
						case 2:
							if(led.sec_flag)
								{
									led.sec_flag=false;
									LED_RED_ON;led.red_sta=true;
									LED_GREEN_OFF;led.green_sta=false;
									led.step=1;
								}
							break;
					}
				break;
			case LED_STA_ALTERNATE_QUIC:
				if((led.show_time!=0)&(giv_sys_time-led.start_time>led.show_time))
					{
						led.status=led.last_status;
						led.show_time=led.last_show_time;
						led.step=0;led.quic_flag=false;
						return;
					}
				switch(led.step)
					{
						case 0:
							LED_GREEN_OFF;LED_RED_OFF;
							led.green_sta=false;led.red_sta=false;
							led.step++;
							led.quic_flag=false;
							break;
						case 1:
							if(led.quic_flag)
								{
									led.quic_flag=false;
									LED_GREEN_ON;led.green_sta=true;
									LED_RED_OFF;led.red_sta=false;
									led.step++;
								}
							break;
						case 2:
							if(led.quic_flag)
								{
									led.quic_flag=false;
									LED_RED_ON;led.red_sta=true;
									LED_GREEN_OFF;led.green_sta=false;
									led.step=1;
								}
							break;
					}
				break;
			case LED_STA_ERR_SHOW:
				switch(led.step)
					{
						case 0:
							led.red_green_time=0;
							led.green_sta=false;
							led.red_sta=false;
							led.step++;
							led.halfsec_flag=false;
							LED_RED_OFF;
							LED_GREEN_OFF;
							break;
						case 1:
							if(led.halfsec_flag)
								{
									led.halfsec_flag=false;
									if(led.red_sta)
										{
											LED_RED_OFF;
											led.red_sta=false;
										}
									else
										{

											if((led.red_green_time>=led.show_time))
												{
													LED_RED_OFF;
													LED_GREEN_OFF;
													led.red_sta=false;
													led.green_sta=false;
													led.step++;
													led.sec_flag=false;
													led.start_time=giv_sys_time;
													return;
												}
										
											LED_RED_ON;
											led.red_sta=true;
											led.red_green_time++;
										}
								}
							break;
						case 2:
							if(led.sec_flag)
								{
									led.sec_flag=false;
									if(led.green_sta)
										{
											LED_GREEN_OFF;
											led.red_green_time=0;
											led.step=0;
											return;
										}
									else
										{
											LED_GREEN_ON;
											led.green_sta=true;
										}
								}
							break;
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

/*-----------打开LED函数--------------
Input Prama：
redorgreen: 0,RED
            1,GREEN
            2,RED&GREEN	
            3,ALTERNATE
            4,ERR_SHOW
show_time:  LED show time,if show_time is 0,means always show
sloworquic: 0,ALWAYS SHOW (CONST)
            1,SLOW
            2,QUICK
            3,red&green
--------------------------------------*/            
void Open_Led(u8 redorgreen,u32 show_time,u8 sloworquic)
{
	led.last_status=led.status;
	led.last_show_time=led.show_time;
	switch(redorgreen)
		{
			case 0:							//只显示红灯
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
				break;
			case 1:							//只显示绿灯
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
				break;
			case 2:
				switch(sloworquic)
					{
						case 0:
							led.status=LED_STA_ALLON_CONST;
							break;
						case 1:
							led.status=LED_STA_ALLON_SLOW;
							break;
						case 2:
							led.status=LED_STA_ALLON_QUIC;
							break;
					}
				led.show_time=show_time;
				led.start_time=giv_sys_time;
				break;
			case 3:
				switch(sloworquic)
					{
						case 1:
							led.status=LED_STA_ALTERNATE_SLOW;
							led.step=0;
							break;
						case 2:
							led.status=LED_STA_ALTERNATE_QUIC;
							led.step=0;
							break;
						default:
							led.status=LED_STA_ALTERNATE_SLOW;
							led.step=0;
							break;
					}
				led.show_time=show_time;
				led.start_time=giv_sys_time;
				break;
			case 4:
				led.status=LED_STA_ERR_SHOW;
				led.show_time=show_time;
				led.start_time=giv_sys_time;
				led.step=0;
			break;
			
		}
}

void Close_Led(void)
{
	led.status=LED_STA_OFF;
	led.show_time=0;
}
