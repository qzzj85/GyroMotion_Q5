#ifndef WIFIDATA_H
#define WIFIDATA_H

#define WIFIDATA_LEN	5
#define	WIFIDATA_NOWPOINT	0X00
#define	WIFIDATA_WALL		0X01
#define	WIFIDATA_CLEAN		0X02
#define	WIFIDATA_DOCK		0X03


extern WIFI_DATA wifidata[WIFIDATA_LEN];

u8 Read_WifiData_Status(void);
void WifiData_Handle(void);
void Set_WifiData_Empty(void);


#endif
