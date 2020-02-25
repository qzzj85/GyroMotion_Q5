#include "AAA-include.h"

#ifdef TUYA_WIFI
bool wifi_time;
bool wifi_update_time;
unsigned char   wifi_key ;
unsigned char  wifi_reset_begin = 0;
unsigned char    wifi_led_begin = 0;
unsigned long  wifi_begin_time = 0;
unsigned long  wifi_stop_time = 0;

unsigned char    wifi_state = 0;
unsigned long  wifi_led_time = 0;
unsigned char    led_flag = 0;
unsigned char wifi_led_poweron= 0;
/*
#define         SMART_CONFIG_STATE              0x00
#define         AP_STATE                        0x01
#define         WIFI_NOT_CONNECTED              0x02
#define         WIFI_CONNECTED                  0x03
#define			WIFI_CONN_CLOUD					0x04
#define			WIFI_LOW_POWER					0x05
#define         WIFI_SATE_UNKNOW                0xff
*/

#endif

MAP_DATA mapdata[WIFIDATA_LEN];
bool mapdata_full=false;
u8 mapdata_cnt=0;
bool wifi_clean=false;

void Init_MapData(void)
{
	for(int i=0;i<WIFIDATA_LEN;i++)
		{
			//mapdata[i].gridx=0x7f;
			//mapdata[i].gridy=0x7f;
			//mapdata[i].data=0;
			mapdata[i].gridx=0xff;
			mapdata[i].gridy=0xff;			
			mapdata[i].data=0xff;  /// ОЮР§КэЧЦ  
		}
}

void Set_MapData_Full(void)
{
	mapdata_full=true;
}

void Set_MapData_Empty(void)
{
	mapdata_full=false;
	mapdata_cnt=0;
	Init_MapData();
}

u8 Read_MapData_Status(void)
{
	if(mapdata_full)
		return 1;
	else
		return 0;
}

void MapData_Handle(void)
{
	static s8 gridx,gridy;
	u8 real_gridx,real_gridy;
	if(Read_MapData_Status())
		return;

	if(mapdata_cnt>=WIFIDATA_LEN)
		{
			Set_MapData_Full();
			return;
		}

	if(mapdata_cnt==0)
		{
			gridx=grid.x;gridy=grid.y;
			real_gridx=Cal_Real_Grid(gridx);
			real_gridy=Cal_Real_Grid(gridy);
			mapdata[mapdata_cnt].gridx=real_gridy;//real_gridx;
			mapdata[mapdata_cnt].gridy=real_gridx;//real_gridy;
			mapdata[mapdata_cnt].data=WIFIDATA_NOWPOINT;
			mapdata_cnt++;
			return;
		}

	if((gridx!=grid.x)|(gridy!=grid.y))
		{
			if(Read_Coordinate_Wall(gridx,gridy))
				{
					mapdata[mapdata_cnt-1].data=WIFIDATA_WALL;
				}
			else
				{
					mapdata[mapdata_cnt-1].data=WIFIDATA_CLEAN;
				}
			gridx=grid.x;gridy=grid.y;
			real_gridx=Cal_Real_Grid(gridx);
			real_gridy=Cal_Real_Grid(gridy);
			mapdata[mapdata_cnt].gridx=real_gridy;//real_gridx;
			mapdata[mapdata_cnt].gridy=real_gridx;//real_gridy;
			mapdata[mapdata_cnt].data=WIFIDATA_NOWPOINT;
			mapdata_cnt++;
		}	
}

void dp_download_power_handle_my(u8 power)
{
	if(power == 0)
	{
	  //寮�鍏冲叧
		//proc_wifi_stop();
		if(mode.mode!=MODE_CHARGE)
		  {
			  if(mode.status)
				  Send_Voice(VOICE_SWEEP_STOP);
				else
				  Send_Voice(VOICE_VOLUME_3);
				Init_VirtualSleep();
		  }
	}
	else
	{
	  //寮�鍏冲紑
	  Send_Voice(VOICE_VOLUME_4);
	  Init_Cease();
	}
}

void dp_download_power_go_handle_my(u8 power_go)
{
	if(power_go == 0)
		{
			proc_wifi_stop();
		  //寮�鍏冲叧
		}
	else
		{
			proc_wifi_play();
		  //寮�鍏冲紑
		}
}

void dp_download_mode_handle_my(u8 mode)
{
	switch(mode)
		{
			case 0:		  //规划清扫
				proc_wifi_play();
			break;

			case 1:		  //沿边清扫
				proc_wifi_ybs();
			break;

			case 2:		  //定点清扫
				proc_wifi_spot();
			break;

			case 3:		  //回充
				proc_wifi_dock();
			break;

			default:
			break;
		}
}

void dp_download_suction_handle_my(u8 suction)
{
	switch(suction)
	{
		case 0:				//force
			sweep_level=SWEEP_LEVEL_FORCE;
			if((mode.status)&(mode.mode!=MODE_DOCK))
				Sweep_Level_Set(sweep_level);	  	
		break;

		case 1:				//standard
			sweep_level=SWEEP_LEVEL_STANDARD;
			if((mode.status)&(mode.mode!=MODE_DOCK))
				Sweep_Level_Set(sweep_level); 	  
		break;

		default:
		break;
	}
}

void dp_download_direction_control_handle_my(u8 direction_control)
{

	switch(direction_control)
		{
			case 0:			  //forward
			if((mode.mode==MODE_CEASE)&((mode.sub_mode==SUBMODE_CEASE)|(mode.sub_mode==SUBMODE_PAUSESWEEP)))
				{
					Init_App_Ctrl(direction_control);
				}
			break;

			case 1:			  //back
			if((mode.mode==MODE_CEASE)&((mode.sub_mode==SUBMODE_CEASE)|(mode.sub_mode==SUBMODE_PAUSESWEEP)))
				{
					Init_App_Ctrl(direction_control);
				}
			break;

			case 2:			  //left
			if((mode.mode==MODE_CEASE)&((mode.sub_mode==SUBMODE_CEASE)|(mode.sub_mode==SUBMODE_PAUSESWEEP)))
				{
					Init_App_Ctrl(direction_control);
				}
			break;

			case 3:			  //right
			if((mode.mode==MODE_CEASE)&((mode.sub_mode==SUBMODE_CEASE)|(mode.sub_mode==SUBMODE_PAUSESWEEP)))
				{
					Init_App_Ctrl(direction_control);
				}
			break;

			case 4:			  //stop
				stop_rap();
				if(mode.last_sub_mode==SUBMODE_PAUSESWEEP)
					Init_PauseSweep();
				else
					Init_Cease();
			break;

			default:
			break;
		}
}

u8 map_data[WIFIDATA_LEN*3];

void mcu_wifi_proc_key(void)
{
	u8 i; 
      
	if(wifi_update_time == 1)
        {	
		    all_data_update();
			wifi_update_time = 0;
		}
	else  
		{
#ifdef   NEW_Q55_BOARD_1113   
                           if ((Read_MapData_Status() == 1)&&(UART2.Trans_Busy==0))
#else
			if ((Read_MapData_Status() == 1)&&(UART1.Trans_Busy==0))
#endif			
				{
					for (i=0;i< WIFIDATA_LEN;i++)
						{
							map_data[i*3] =  mapdata[i].gridx;
							map_data[i*3+1] =  mapdata[i].gridy;
							map_data[i*3+2 ]=  mapdata[i].data;
						}
					stream_file_trans(0x00, map_data, WIFIDATA_LEN *3); // 60 = 20*3 
					Set_MapData_Empty();
				}
		}
	
    if(wifi_time == 0)		
        return;
    wifi_time = 0;
	
#if 1
	if(wifi_clean==1) 
		{
			wifi_key = 1;
			wifi_clean=0;
		}
#endif		  
	if (wifi_reset_begin == 0) 
		{
			if (wifi_key == 1)	
				{
					mcu_reset_wifi();
					//while (UART1.Trans_Busy == 1) ;
					
					mcu_set_wifi_mode(SMART_CONFIG);
					wifi_reset_begin = 1;
					wifi_led_begin = 1;
					wifi_begin_time =wifi_stop_time= giv_sys_time;
				 }
			if (wifi_key == 2)	
				{
					mcu_reset_wifi();
					//while (UART1.Trans_Busy == true) ;
					mcu_set_wifi_mode(AP_CONFIG);
					wifi_reset_begin = 1;
					wifi_led_begin = 1;
					wifi_begin_time =wifi_stop_time= giv_sys_time;
				 }		
		}	
	if(wifi_reset_begin == 1) 
		{
			// mcu_get_reset_wifi_flag();
			if ((reset_wifi_flag ==1)&&(set_wifimode_flag == 1))
				{  //回复已经接收到复位信号 
					wifi_key = 0;
					wifi_reset_begin = 0;
				}
			if ((giv_sys_time-wifi_begin_time)>30000) 
				{ //  超过3 s没有回应成功则再次发复位信号过去 
					wifi_begin_time = giv_sys_time; //新修改 
					if (wifi_key == 1) 
						{
							mcu_reset_wifi();
							mcu_set_wifi_mode(SMART_CONFIG);}
					if (wifi_key == 2) 
						{
							mcu_reset_wifi();
							mcu_set_wifi_mode(AP_CONFIG);
						}				 
				}	 
		}
	if (wifi_led_begin == 1) 
		{
		//  HaltTime = 0;  // 配网时候不要马上待机 
			if ((giv_sys_time-wifi_stop_time)>300000)
				{ // 超过30秒结束
					wifi_led_begin = 0;
					Open_Led(0,1,0); // 红灯灭
					Open_Led(1,0,0);// 绿灯常亮 
					wifi_reset_begin = 0;   //新修改 退出后设置程序也同时退出,  实际wifi模块还可以配网
					wifi_key = 0;
					return;
			 	}
			switch (wifi_work_state) 
				{
					case SMART_CONFIG_STATE :
					if ((giv_sys_time - wifi_led_time) >2500) 
						{
							led_flag ^=1;
							if (led_flag ==0)
								{
									LED_RED_OFF;
									LED_GREEN_OFF; 
								}
							else 
								{
									LED_RED_ON;
									LED_GREEN_ON; 						
								}
							wifi_led_time = giv_sys_time ;
						}
					break;
					case AP_STATE :
						if ((giv_sys_time - wifi_led_time) >10000)
							{
								led_flag ^=1;
								if (led_flag ==0)
									{
										LED_RED_OFF;
										LED_GREEN_OFF;  
									}
								else 
									{
										LED_RED_ON;
										LED_GREEN_ON; 						
									} 
								wifi_led_time = giv_sys_time ;
							}			   
					break;
					case WIFI_NOT_CONNECTED  :
						LED_RED_ON;
						LED_GREEN_OFF; 
						led_flag = 0;
					break;
					case WIFI_CONNECTED  :
						if (led_flag ==0) 
							Send_Voice(VOICE_WIFI_OK);					  
						led_flag = 1;
						LED_RED_OFF;
						LED_GREEN_ON;  
						wifi_stop_time = giv_sys_time-wifi_stop_time + 20000 ;// 2秒后就退出LED状态   新修改 
					break;  
					default :
						//wifi_led_begin = 0;
						LED_RED_OFF;
						LED_GREEN_ON;  
					break;
				}


		}
	else 
		{
		// 开机或者工作时候连接wifi成功显示状态 3秒 
#if 1
	 //   if ((wifi_led_poweron < 2)&&(robot_status.system_status == ROBOTHALTING)) {
			if (wifi_led_poweron < 2)   
				{			
				    if (((wifi_work_state == WIFI_CONNECTED)||(wifi_work_state == 0x04))&&(wifi_led_poweron < 1)) 
						{
					        wifi_led_poweron = 1;
							wifi_begin_time = giv_sys_time;
							LED_RED_ON;
							LED_GREEN_ON; 			
							Send_Voice(VOICE_WIFI_OK);	
						}
					if (((giv_sys_time-wifi_begin_time)>30000)&&(wifi_led_poweron ==1))
						{ // 超过3秒结束
							wifi_led_poweron  = 2;
							LED_RED_OFF;
							LED_GREEN_ON; 
						}
				}
			else 
				{
					if (wifi_work_state < 0x03)  wifi_led_poweron = 0; //断往后重新连接网络    新修改 
				}
#endif
		}
}

void proc_wifi_ybs(void)
{
	switch(mode.mode)
		{
			case MODE_CEASE:
				switch(mode.sub_mode)
					{
						case SUBMODE_CEASE:
#ifdef TUYA_WIFI
							Reset_Map();
#endif
							Reset_XY();
							//delay_ms(3000);
							Init_Right_YBS(1,0);
						break;
						case SUBMODE_ERR:
						case SUBMODE_SLEEP:
							Init_Cease();
						break;
						default:
							break;
					}
				break;
			case MODE_CHARGE:
				switch(mode.sub_mode)
					{
						case SUBMODE_CHARGE_SWITCHOFF:
							break;
						case SUBMODE_CHARGE_DC:
							Send_Voice(VOICE_ERROR_DC_EXIST);
							break;
						case SUBMODE_CHARGE_SEAT:
							Init_Quit_Charging(SWEEP_METHOD_YBS);
							break;
						default:
							break;
					}
				break;
			default:
				break;
		}
}

void proc_wifi_dock(void)
{
	motion1.force_dock=true;
	switch(mode.mode)
		{
			case MODE_CEASE:
				switch(mode.sub_mode)
					{
						case SUBMODE_CEASE:
#ifdef TUYA_WIFI
							Reset_Map();
#endif
							Init_Docking();
						break;
						case SUBMODE_ERR:
						case SUBMODE_SLEEP:
							Init_Docking();
						break;
						default:
							break;
					}
				break;
			case MODE_SWEEP:
			case MODE_SHIFT:
			case MODE_PASS2INIT:
			case MODE_EXIT:
				stop_rap();
				Send_Voice(VOICE_DOCK_START);
				Sweep_Level_Set(SWEEP_LEVEL_DOCK);
				Force_Dock();
				break;
			case MODE_YBS:
			case MODE_SPOT:
				stop_rap();
				Init_Docking();
				break;
			default:
				break;
		}
}

void proc_wifi_play(void)
{
	switch(mode.mode)
		{
			case MODE_CEASE:
				switch(mode.sub_mode)
					{
						case SUBMODE_CEASE:
#ifdef TUYA_WIFI
							Reset_Map();
#endif
							Init_First_Sweep(0);
						break;
						case SUBMODE_ERR:
						case SUBMODE_SLEEP:
							Init_Cease();
						break;
						default:
							break;
					}
				break;
			case MODE_CHARGE:
				switch(mode.sub_mode)
					{
						case SUBMODE_CHARGE_SWITCHOFF:
							break;
						case SUBMODE_CHARGE_DC:
							Send_Voice(VOICE_ERROR_DC_EXIST);
							break;
						case SUBMODE_CHARGE_SEAT:
							Init_Quit_Charging(SWEEP_METHOD_GUIHUA);
							break;
						default:
							break;
					}
				break;
			default:
				break;
		}

}
void proc_wifi_stop(void)
{
	switch(mode.mode)
		{
			case MODE_SWEEP:
			case MODE_SHIFT:
			case MODE_YBS:
			case MODE_EXIT:
			case MODE_PASS2INIT:
				Send_Voice(VOICE_SWEEP_STOP);
				Init_Cease();
			break;
			case MODE_DOCK:
				Send_Voice(VOICE_DOCK_STOP);
				Init_Cease();
			break;
			case MODE_CTRL:
				stop_rap();
				if(mode.last_sub_mode==SUBMODE_PAUSESWEEP)
					Init_PauseSweep();
				else
					Init_Cease();
			break;
			case MODE_SPOT:
				Send_Voice(VOICE_SWEEP_STOP);
				Init_Cease();
			break;
			default:
				break;
		}
#ifdef TUYA_WIFI  
       stream_stop(0,256);// 停止地图    WIFI_STREAM_ENABLE
#endif	
}

void proc_wifi_spot(void)
{
	switch(mode.mode)
		{
			case MODE_CEASE:
				switch(mode.sub_mode)
					{
						case SUBMODE_CEASE:
#ifdef TUYA_WIFI
							Reset_Map();
#endif
							Init_Spot(SPOT_FROM_CEASE);
						break;
						default:
							break;
					}
				break;
			case MODE_CHARGE:
				switch(mode.sub_mode)
					{
						case SUBMODE_CHARGE_SWITCHOFF:
							break;
						case SUBMODE_CHARGE_DC:
							Send_Voice(VOICE_ERROR_DC_EXIST);
							break;
						case SUBMODE_CHARGE_SEAT:
							Init_Quit_Charging(SWEEP_METHOD_SPOT);
							break;
						default:
							break;
					}
				break;
			case MODE_SWEEP:
			case MODE_YBS:
			case MODE_SHIFT:
			case MODE_EXIT:
			case MODE_PASS2INIT:
				Send_Voice(VOICE_VOLUME_3);
				stop_rap();
				Init_Spot(SPOT_FROM_SWEEP);
				break;
			default:
				break;
		}
}

void Reset_Map(void)
{
	mcu_dp_enum_update(5,2);  //状态上报为工作模式  
	wifi_uart_write_stream_init(0,0);// 初始化地图参数	地图	0  
	DelayMs(1);
	stream_open();	// 申请传输  WIFI_STREAM_ENABLE
	DelayMs(1);
	stream_start(00);// 开始传输
}
