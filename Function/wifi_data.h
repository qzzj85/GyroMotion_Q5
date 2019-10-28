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

#define		WIFI_ERR_LRING_OC	1
#define		WIFI_ERR_RRING_OC	2
#define		WIFI_ERR_SB_OC		3
#define		WIFI_ERR_BUMP		4
#define		WIFI_ERR_EARTH		5	
#define		WIFI_ERR_FAN		6
#define		WIFI_ERR_MB			7
#define		WIFI_ERR_DUSTBOX	8
#define		WIFI_ERR_LOWPOWER	9
#define		WIFI_ERR_OTHER		10


extern WIFI_DATA wifidata[WIFIDATA_LEN];

u8 Read_WifiData_Status(void);
void WifiData_Handle(void);
void Set_WifiData_Empty(void);


#endif
