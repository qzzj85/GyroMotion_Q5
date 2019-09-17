#include "AAA-include.h"


WIFI_DATA wifidata[WIFIDATA_LEN];
bool wifidata_full=false;
u8 wifidata_cnt=0;

void Init_WifiData(void)
{
	for(int i=0;i<WIFIDATA_LEN;i++)
		{
			//wifidata[i].gridx=0x7f;
			//wifidata[i].gridy=0x7f;
			//wifidata[i].data=0;
			wifidata[i].gridx=0xff;
			wifidata[i].gridy=0xff;			
			wifidata[i].data=0xff;  /// §°§À§²¡ì§¬§ï§¹§¸  
		}
}

void Set_WifiData_Full(void)
{
	wifidata_full=true;
}

void Set_WifiData_Empty(void)
{
	wifidata_full=false;
	wifidata_cnt=0;
	Init_WifiData();
}

u8 Read_WifiData_Status(void)
{
	if(wifidata_full)
		return 1;
	else
		return 0;
}

void WifiData_Handle(void)
{
	static s8 gridx,gridy;
	u8 real_gridx,real_gridy;
	if(Read_WifiData_Status())
		return;

	if(wifidata_cnt>=WIFIDATA_LEN)
		{
			Set_WifiData_Full();
			return;
		}

	if(wifidata_cnt==0)
		{
			gridx=grid.x;gridy=grid.y;
			real_gridx=Cal_Real_Grid(gridx);
			real_gridy=Cal_Real_Grid(gridy);
			wifidata[wifidata_cnt].gridx=real_gridy;//real_gridx;
			wifidata[wifidata_cnt].gridy=real_gridx;//real_gridy;
			wifidata[wifidata_cnt].data=WIFIDATA_NOWPOINT;
			wifidata_cnt++;
			return;
		}

	if((gridx!=grid.x)|(gridy!=grid.y))
		{
			if(Read_Coordinate_Wall(gridx,gridy))
				{
					wifidata[wifidata_cnt-1].data=WIFIDATA_WALL;
				}
			else
				{
					wifidata[wifidata_cnt-1].data=WIFIDATA_CLEAN;
				}
			gridx=grid.x;gridy=grid.y;
			real_gridx=Cal_Real_Grid(gridx);
			real_gridy=Cal_Real_Grid(gridy);
			wifidata[wifidata_cnt].gridx=real_gridy;//real_gridx;
			wifidata[wifidata_cnt].gridy=real_gridx;//real_gridy;
			wifidata[wifidata_cnt].data=WIFIDATA_NOWPOINT;
			wifidata_cnt++;
		}	
}
