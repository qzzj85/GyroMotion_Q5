#ifndef WIFIDATA_H
#define WIFIDATA_H

#define WIFIDATA_LEN	5
#define	WIFIDATA_NOWPOINT	0X00
#define	WIFIDATA_WALL		0X01
#define	WIFIDATA_CLEAN		0X02
#define	WIFIDATA_DOCK		0X03



#define		WIFI_STA_SLEEP		0//1		//1
#define		WIFI_STA_CEASE 		1//2		//2
#define		WIFI_STA_WORKING 	2//3	//3
#define		WIFI_STA_DOCK 		3//4
#define		WIFI_STA_CHGDONE	4//5
#define		WIFI_STA_CHGSEAT	5//6
#define		WIFI_STA_CHGDC		6//7
#define		WIFI_STA_ERR		7//8
#define		WIFI_STA_OTHERERR	8//9

#define		WIFI_MOD_SWEEP		0
#define		WIFI_MOD_YBS		1
#define		WIFI_MOD_SPOT		2
#define		WIFI_MOD_DOCK		3

#define		WIFI_ERR_LRING_OC	0x0001
#define		WIFI_ERR_RRING_OC	0x0002
#define		WIFI_ERR_SB_OC		0x0004
#define		WIFI_ERR_BUMP		0x0008
#define		WIFI_ERR_EARTH		0x0010	
#define		WIFI_ERR_FAN		0x0020
#define		WIFI_ERR_MB			0x0040
#define		WIFI_ERR_DUSTBOX	0x0080
#define		WIFI_ERR_LOWPOWER	0x0100
#define		WIFI_ERR_OTHER		0x0200

#ifdef TUYA_WIFI

extern MAP_DATA mapdata[WIFIDATA_LEN];

u8 Read_MapData_Status(void);
void MapData_Handle(void);
void Set_MapData_Empty(void);

void dp_download_power_handle_my(u8 power);
void dp_download_mode_handle_my(u8 mode);
void dp_download_power_go_handle_my(u8 power_go);
void dp_download_suction_handle_my(u8 suction);
void dp_download_direction_control_handle_my(u8 direction_control);

extern bool wifi_time;
extern bool wifi_clean;
extern bool wifi_update_time;
void mcu_wifi_proc_key(void);
void proc_wifi_dock(void);
void proc_wifi_ybs(void);
void proc_wifi_play(void);
void proc_wifi_spot(void);
void proc_wifi_stop(void);

extern void wifi_uart_write_stream_init(unsigned char  id, unsigned short map_len);
unsigned char stream_file_trans(unsigned int id, unsigned char *buffer, unsigned long buf_len);
unsigned char stream_open(void);
unsigned char stream_start(unsigned int id);
unsigned char stream_stop(unsigned char id, unsigned long offset);


#endif


#endif
