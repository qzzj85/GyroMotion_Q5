/****************************************Copyright (c)*************************
**                               版权所�?(C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**�?  �?  �? protocol.c
**�?       �? 下发/上报数据处理函数
**�?�?�?�?:

                  *******非常重要，一定要看哦！！�?*******

** 1、用户在此文件中实现数据下发/上报功能
** 2、DP的ID/TYPE及数据处理函数都需要用户按照实际定义实�?
** 3、当开始某些宏定义后需要用户实现代码的函数内部�?err提示,完成函数后请删除�?err
**
**--------------当前版本修订---------------------------------------------------
** �? �? v2.5.1
** 日　�? 2018�?0�?7�?
** 描　�? 1:默认关闭流服务功�?
           2:增加03协议wifi状态宏定义
		   3:更新与修改部分函数注�?
		   
** �? �? v2.5.0
** 日　�? 2018�?�?8�?
** 描　�? 1:协议版本改为0x03
           2:增加WIFI模组心跳关闭功能
           3:增加天气功能

** �? �? v2.3.8
** 日　�? 2018�?�?7�?
** 描　�? 1:变量添加volatile防止编译器优�?
           2:添加#error提示

** �? �? v2.3.7
** 日　�? 2017�?�?8�?
** 描　�? 1:优化串口队列接收处理
		   
** �? �? v2.3.6
** 日　�? 2016�?�?1�?
** 描　�? 1:修复获取本地时间错误
           2:添加hex_to_bcd转换函数
		   
** �? �? v2.3.5
** 日　�? 2016�?�?�?
** 描　�? 1:修改返回协议版本�?x01
           2:固件升级数据偏移量修改为4字节

** �? �? v2.3.4
** 日　�? 2016�?�?6�?
** 描　�? 1:优化串口解析函数
           2:优化编译器兼容�?取消enum类型定义

** �? �? v2.3.3
** 日　�? 2016�?�?4�?
** 描　�? 1:修改mcu获取本地时间函数
           2:添加wifi功能测试

** �? �? v2.3.2
** 日　�? 2016�?�?3�?
** 描　�? 1:优化串口数据解析
           2:优化MCU固件升级流程
           3:优化上报流程

** �? �? v2.3.1
** 日　�? 2016�?�?5�?
** 描　�? 1:优化串口数据解析

** �? �? v2.3
** 日　�? 2016�?�?4�?
** 描　�? 1:支持MCU固件在线升级

** �? �? v2.2
** 日　�? 2016�?�?1�?
** 描　�? 1:修改串口数据接收方式

** �? �? v2.1
** 日　�? 2016�?�?�?
** 描　�? 1:加入某些编译器不支持函数指针兼容选项

** �? �? v2.0
** 日　�? 2016�?�?9�?
** 描　�? 1:优化代码结构
           2:节省RAM空间
**
**-----------------------------------------------------------------------------
******************************************************************************/

#include "wifi.h"
#include "usart.h"
#include "power.h"
#include "AAA-include.h"

#ifdef WEATHER_ENABLE
/******************************************************************************
                        天气数据参数选择数组
          **用户可以自定义需要的参数，注释或者取消注释即可，注意更改**         
******************************************************************************/
const char weather_choose[WEATHER_CHOOSE_CNT][10] = {
    "temp",
    "humidity",
    "condition",
    "pm25",
    /*"pressure",
    "realFeel",
    "uvi",
    "tips",
    "windDir",
    "windLevel",
    "windSpeed",
    "sunRise",
    "sunSet",
    "aqi",
    "so2 ",
    "rank",
    "pm10",
    "o3",
    "no2",
    "co",*/
 };
#endif

/******************************************************************************
                                移植须知:
1:MCU必须在while中直接调用mcu_api.c内的wifi_uart_service()函数
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢�?
3:请勿在中�?定时器中断内调用上报函数
******************************************************************************/

         
/******************************************************************************
                              第一�?初始�?
1:在需要使用到wifi相关文件的文件中include "wifi.h"
2:在MCU初始化中调用mcu_api.c文件中的wifi_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数�?并删�?error
4:在MCU串口接收函数中调用mcu_api.c文件内的uart_receive_input函数,并将接收到的字节作为参数传入
5:单片机进入while循环后调用mcu_api.c文件内的wifi_uart_service()函数
******************************************************************************/

/******************************************************************************
                        1:dp数据点序列类型对照表
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/
const DOWNLOAD_CMD_S download_cmd[] =
{
  {DPID_POWER, DP_TYPE_BOOL},
  {DPID_POWER_GO, DP_TYPE_BOOL},
  {DPID_MODE, DP_TYPE_ENUM},
  {DPID_DIRECTION_CONTROL, DP_TYPE_ENUM},
  {DPID_STATUS, DP_TYPE_ENUM},
  {DPID_ELECTRICITY_LEFT, DP_TYPE_VALUE},
  {DPID_SUCTION, DP_TYPE_ENUM},
  {DPID_CLEAN_RECORD, DP_TYPE_STRING},
  {DPID_CLEAN_AREA, DP_TYPE_VALUE},
  {DPID_CLEAN_TIME, DP_TYPE_VALUE},
  {DPID_FAULT, DP_TYPE_FAULT},
  {DPID_MAP_CONFIG, DP_TYPE_RAW},
};

extern void proc_wifi_play();
extern void proc_wifi_stop();		

/******************************************************************************
                           2:串口单字节发送函�?
请将MCU串口发送函数填入该函数�?并将接收到的数据作为参数传入串口发送函�?
******************************************************************************/

/*****************************************************************************
函数名称 : uart_transmit_output
功能描述 : 发数据处�?
输入参数 : value:串口收到字节数据
返回参数 : �?
使用说明 : 请将MCU串口发送函数填入该函数�?并将接收到的数据作为参数传入串口发送函�?
*****************************************************************************/
void uart_transmit_output(unsigned char value)
{
//  #error "请将MCU串口发送函数填入该函数,并删除该�?
/*
  //示例:
  extern void Uart_PutChar(unsigned char value);
  Uart_PutChar(value);	                                //串口发送函�?
*/
}
/******************************************************************************
                           第二�?实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处�?
当前函数处理全部数据上报(包括可下�?可上报和只上�?
  需要用户按照实际情况实�?
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上�?
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上�?
******************************************************************************/

//自动化生成数据上报函�?

/*****************************************************************************
函数名称 : all_data_update
功能描述 : 系统所有dp点信息上�?实现APP和muc数据同步
输入参数 : �?
返回参数 : �?
使用说明 : 此函数SDK内部需调用;
           MCU必须实现该函数内数据上报功能;包括只上报和可上报可下发型数�?
*****************************************************************************/

#include "soft.h"
extern MODE mode;		 //?????????????? 

void all_data_update(void)
{
	u8 power_switch=0,sweep_switch=0,work_sta=0,sweep_mode=0,err_code=0,suction=1;
	u32 clean_time=0;
	float clean_size=0;
	/* 
	//此代码为平台自动生成，请按照实际数据修改每个可下发可上报函数和只上报函数
	mcu_dp_bool_update(DPID_POWER,当前电源开�?; //BOOL型数据上�?
	mcu_dp_bool_update(DPID_POWER_GO,当前清扫开�?; //BOOL型数据上�?
	mcu_dp_enum_update(DPID_MODE,当前清扫模式); //枚举型数据上�?
	mcu_dp_enum_update(DPID_DIRECTION_CONTROL,当前方向); //枚举型数据上�?
	mcu_dp_enum_update(DPID_STATUS,当前工作状�?; //枚举型数据上�?
	mcu_dp_value_update(DPID_ELECTRICITY_LEFT,当前剩余电量); //VALUE型数据上�?
	mcu_dp_enum_update(DPID_SUCTION,当前吸力选择); //枚举型数据上�?
	mcu_dp_string_update(DPID_CLEAN_RECORD,当前清扫记录指针,当前清扫记录数据长度); //STRING型数据上�?
	mcu_dp_value_update(DPID_CLEAN_AREA,当前清扫面积); //VALUE型数据上�?
	mcu_dp_value_update(DPID_CLEAN_TIME,当前清扫时间); //VALUE型数据上�?
	mcu_dp_fault_update(DPID_FAULT,当前故障告警); //故障型数据上�?
	mcu_dp_raw_update(DPID_MAP_CONFIG,当前地图参数指针,当前地图参数数据长度); //RAW型数据上�?

	*/
	
	unsigned char   i;
	if(power.switch_flag)
		power_switch=1;
	else
		power_switch=0;

	switch(mode.mode)
		{
			case CEASE:
				sweep_switch=0;
				switch(mode.sub_mode)
					{
						case CEASE:
						case QUIT_CHARGING:
							//sweep_mode=0;
							work_sta=WIFI_STA_CEASE;
							break;
						case SLEEP:
						case SHUTDOWN:
						case DEAD:
							//sweep_mode=0;
							work_sta=WIFI_STA_SLEEP;
							break;
						case ERR:
							//sweep_mode=0;
							work_sta=WIFI_STA_ERR;
							break;
					}
				break;
			case SWEEP:
			case SHIFT:
			case EXIT:
			case PASS2INIT:
				sweep_switch=1;
				sweep_mode=WIFI_MOD_SWEEP;
				work_sta=WIFI_STA_WORKING;
				clean_size=motion1.clean_size;
				break;
			case YBS:
				sweep_switch=1;
				sweep_mode=WIFI_MOD_YBS;
				work_sta=WIFI_STA_WORKING;
				clean_size=motion1.clean_size;
				break;
			case SPOT:
				sweep_switch=1;
				sweep_mode=WIFI_MOD_SPOT;
				work_sta=WIFI_STA_WORKING;
				clean_size=motion1.clean_size;
				break;
			case DOCKING:
				sweep_switch=1;
				sweep_mode=WIFI_MOD_DOCK;
				work_sta=WIFI_STA_DOCK;
				clean_size=motion1.clean_size;
				break;
			case CHARGEING:
				sweep_switch=0;
				if(flag_full)
					work_sta=WIFI_STA_CHGDONE;
				else if(power.charge_dc)
					work_sta=WIFI_STA_CHGDC;
				else
					work_sta=WIFI_STA_CHGSEAT;
				break;
			case MODE_CTRL:
				sweep_switch=1;
				sweep_mode=WIFI_MOD_SWEEP;
				work_sta=WIFI_STA_CEASE;
				break;
		}

	mcu_dp_bool_update(DPID_POWER,power_switch); 
	mcu_dp_bool_update(DPID_POWER_GO,sweep_switch); 
	mcu_dp_enum_update(DPID_MODE,sweep_mode); 
	mcu_dp_enum_update(DPID_STATUS,work_sta);
	mcu_dp_value_update(DPID_CLEAN_AREA,(u32)(clean_size*10));
#if 0
	i = 0;
	// unsigned int action;		//????   0:??  1:??????  2?????? 3???   4????   5???1  6?????2  ??????18???14	??19????????
	if (mode.action == 0) i = 4;
	if (mode.action == 1) i = 2;	
	if (mode.action == 2) i = 3;
	if (mode.action == 3) i = 0;	
	if (mode.action == 4) i = 1;
	if (mode.action > 4) i = 2;	
	mcu_dp_enum_update(DPID_DIRECTION_CONTROL,i); 
#endif


	i=Slam_Data.bat;
	mcu_dp_value_update(DPID_ELECTRICITY_LEFT,i);

	if(sweep_level<SWEEP_LEVEL_FORCE)
		suction=1;
	else
		suction=0;
	mcu_dp_enum_update(DPID_SUCTION,suction);

	i = 0;
	mcu_dp_fault_update(DPID_FAULT,err_code);

	if(mode.status==0)
		clean_time=0;
	else
		clean_time=motion1.worktime/600000;
	mcu_dp_value_update(DPID_CLEAN_TIME,clean_time);
}
	/******************************************************************************
									WARNING!!!	  
								2:所有数据上报处�?
	自动化代码模板函�?具体请用户自行实现数据处�?
	******************************************************************************/
	
	
	/*****************************************************************************
	函数名称 : dp_download_power_handle
	功能描述 : 针对DPID_POWER的处理函�?
	输入参数 : value:数据源数�?
			: length:数据长度
	返回参数 : 成功返回:SUCCESS/失败返回:ERROR
	使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
	*****************************************************************************/
static unsigned char dp_download_power_handle(const unsigned char value[], unsigned short length)
		{
		  //示例:当前DP类型为BOOL
		  unsigned char ret;
		  //0:�?1:开
		  unsigned char power;
		  
		  power = mcu_get_dp_download_bool(value,length);
		  if(power == 0)
		  {
			//开关关
			  proc_wifi_stop();
		  }
		  else
		  {
			//开关开
		  }
		  
		  //处理完DP数据后应有反�?
		  ret = mcu_dp_bool_update(DPID_POWER,power);
		  if(ret == SUCCESS)
			return SUCCESS;
		  else
			return ERROR;
		}
		
		/*****************************************************************************
		函数名称 : dp_download_power_go_handle
		功能描述 : 针对DPID_POWER_GO的处理函�?
		输入参数 : value:数据源数�?
				: length:数据长度
		返回参数 : 成功返回:SUCCESS/失败返回:ERROR
		使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
		*****************************************************************************/	
static unsigned char dp_download_power_go_handle(const unsigned char value[], unsigned short length)
			{
			  //示例:当前DP类型为BOOL
			  unsigned char ret;
			  //0:�?1:开
			  unsigned char power_go;
			  
			  power_go = mcu_get_dp_download_bool(value,length);
			  if(power_go == 0)
			  {
			  proc_wifi_stop();
				//开关关
			  }
			  else
			  {
			  proc_wifi_play();
				//开关开
			  }
			  
			  //处理完DP数据后应有反�?
			  ret = mcu_dp_bool_update(DPID_POWER_GO,power_go);
			  if(ret == SUCCESS)
				return SUCCESS;
			  else
				return ERROR;
}

static unsigned char dp_download_mode_handle(const unsigned char value[], unsigned short length)
{
  //示例:当前DP类型为ENUM
  unsigned char ret;
  unsigned char mode;
  
  mode = mcu_get_dp_download_enum(value,length);
  switch(mode)
  {
    case 0:			//�滮��ɨ
      	  proc_wifi_play();
      break;
    
    case 1:			//�ر���ɨ
      	  proc_wifi_ybs();
      break;
      
    case 2:			//������ɨ
      	  proc_wifi_stop();
      break;
      
    case 3:			//�س�
		proc_wifi_dock();
      break;
      
    default:
      
      break;
  }
  
  //处理完DP数据后应有反�?
  ret = mcu_dp_enum_update(DPID_MODE,mode);
  if(ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_direction_control_handle
功能描述 : 针对DPID_DIRECTION_CONTROL的处理函�?
输入参数 : value:数据源数�?
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_direction_control_handle(const unsigned char value[], unsigned short length)
{
  //示例:当前DP类型为ENUM
  unsigned char ret;
  unsigned char direction_control;
  
  direction_control = mcu_get_dp_download_enum(value,length);
  switch(direction_control)
  {
    case 0:				//forward
      if((mode.mode==CEASE)&(mode.sub_mode==CEASE))
      	{
      		Init_App_Ctrl(direction_control);
      	}
      break;
      
    case 1:				//back
		if((mode.mode==CEASE)&(mode.sub_mode==CEASE))
		  {
			  Init_App_Ctrl(direction_control);
		  }
      break;
      
    case 2:				//left
		if((mode.mode==CEASE)&(mode.sub_mode==CEASE))
		  {
			  Init_App_Ctrl(direction_control);
		  }
      break;
      
    case 3:				//right
		if((mode.mode==CEASE)&(mode.sub_mode==CEASE))
		  {
			  Init_App_Ctrl(direction_control);
		  }
      break;
      
    case 4:				//stop
      stop_rap();
	  Init_Cease();
      break;
      
    default:
      
      break;
  }
  
  //处理完DP数据后应有反�?
  ret = mcu_dp_enum_update(DPID_DIRECTION_CONTROL,direction_control);
  if(ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_suction_handle
功能描述 : 针对DPID_SUCTION的处理函�?
输入参数 : value:数据源数�?
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_suction_handle(const unsigned char value[], unsigned short length)
{
	//示例:当前DP类型为ENUM
	unsigned char ret;
	unsigned char suction;

	suction = mcu_get_dp_download_enum(value,length);
	switch(suction)
	{
		case 0:				//force
			sweep_level=SWEEP_LEVEL_FORCE;
			if((mode.status)&(mode.mode!=DOCKING))
				Sweep_Level_Set(sweep_level);	  	
		break;

		case 1:				//standard
			sweep_level=SWEEP_LEVEL_STANDARD;
			if((mode.status)&(mode.mode!=DOCKING))
				Sweep_Level_Set(sweep_level); 	  
		break;

		default:
		break;
	}
  
  //处理完DP数据后应有反�?
  ret = mcu_dp_enum_update(DPID_SUCTION,suction);
  if(ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}


/******************************************************************************
                                WARNING!!!                     
此代码为SDK内部调用,请按照实际dp数据实现函数内部数据
******************************************************************************/
#ifdef SUPPORT_MCU_RTC_CHECK
/*****************************************************************************
函数名称 : mcu_write_rtctime
功能描述 : MCU校对本地RTC时钟
输入参数 : �?
返回参数 : �?
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void mcu_write_rtctime(unsigned char time[])
{
  #error "请自行完成RTC时钟写入代码,并删除该�?
  /*
  time[0]为是否获取时间成功标志，�?0 表示失败，为 1表示成功
  time[1] �?�?�?, 0x00 �?�?000 �?
  time[2]为月份，�?1 开始到12 结束
  time[3]为日期，�?1 开始到31 结束
  time[4]为时钟，�?0 开始到23 结束
  time[5]为分钟，�?0 开始到59 结束
  time[6]为秒钟，�?0 开始到59 结束
  time[7]为星期，�?1 开始到 7 结束�?代表星期一
 */
  if(time[0] == 1)
  {
    //正确接收到wifi模块返回的本地时钟数�?
	 
  }
  else
  {
  	//获取本地时钟数据出错,有可能是当前wifi模块未联�?
  }
}
#endif

#ifdef WIFI_TEST_ENABLE
/*****************************************************************************
函数名称 : wifi_test_result
功能描述 : wifi功能测试反馈
输入参数 : result:wifi功能测试结果;0:失败/1:成功
           rssi:测试成功表示wifi信号强度/测试失败表示错误类型
返回参数 : �?
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void wifi_test_result(unsigned char result,unsigned char rssi)
{
//  #error "请自行实现wifi功能测试成功/失败代码,完成后请删除该行"
  if(result == 0)
  {
    //测试失败
    if(rssi == 0x00)
    {
      //未扫描到名称为tuya_mdev_test路由�?请检�?
    }
    else if(rssi == 0x01)
    {
      //模块未授�?
    }
  }
  else
  {
    //测试成功
    //rssi为信号强�?0-100, 0信号最差，100信号最�?
  }
  
}
#endif

#ifdef SUPPORT_MCU_FIRM_UPDATE
/*****************************************************************************
函数名称 : mcu_firm_update_handle
功能描述 : MCU进入固件升级模式
输入参数 : value:固件缓冲�?
           position:当前数据包在于固件位�?
           length:当前固件包长�?固件包长度为0�?表示固件包发送完�?
返回参数 : �?
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length)
{
  #error "请自行完成MCU固件升级代码,完成后请删除该行"
  if(length == 0)
  {
    //固件数据发送完�?
    
  }
  else
  {
    //固件数据处理
  }
  
  return SUCCESS;
}
#endif
/******************************************************************************
                                WARNING!!!                     
以下函数用户请勿修改!!
******************************************************************************/

/*****************************************************************************
函数名称 : dp_download_handle
功能描述 : dp下发处理函数
输入参数 : dpid:DP序号
           value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERRO
使用说明 : 该函数用户不能修�?
*****************************************************************************/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
  /*********************************
  当前函数处理可下�?可上报数据调�?                   
  具体函数内需要实现下发数据处�?
  完成用需要将处理结果反馈至APP�?否则APP会认为下发失�?
  ***********************************/
  unsigned char ret;
  switch(dpid)
  {
    case DPID_POWER:
      //电源开关处理函�?
      ret = dp_download_power_handle(value,length);
      break;
    case DPID_POWER_GO:
      //清扫开关处理函�?
      ret = dp_download_power_go_handle(value,length);
      break;
    case DPID_MODE:
      //清扫模式处理函数
      ret = dp_download_mode_handle(value,length);
      break;
    case DPID_DIRECTION_CONTROL:
      //方向处理函数
      ret = dp_download_direction_control_handle(value,length);
      break;
    case DPID_SUCTION:
      //吸力选择处理函数
      ret = dp_download_suction_handle(value,length);
      break;

  default:
    break;
  }
  return ret;
}
/*****************************************************************************
函数名称 : get_download_cmd_total
功能描述 : 获取所有dp命令总和
输入参数 : �?
返回参数 : 下发命令总和
使用说明 : 该函数用户不能修�?
*****************************************************************************/
unsigned char get_download_cmd_total(void)
{
  return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}

#ifdef WEATHER_ENABLE

/*****************************************************************************
函数名称 : weather_open_return
功能描述 : 打开天气功能返回用户自处理函�?
输入参数 : 1:res:打开天气功能返回结果�?:成功�?：失�?

返回参数 : �?
*****************************************************************************/
void weather_open_return_handle(unsigned char res, unsigned char err)
{
  #error "请自行完成M打开天气功能返回数据处理代码,完成后请删除该行"
  unsigned char err_num = 0;
  
  if(res == 1)
  {
    //打开天气返回成功
  }
  else if(res == 0)
  {
    //打开天气返回失败
    err_num = err;//获取错误�?
  }
}

/*****************************************************************************
函数名称 : weather_data_user_handle
功能描述 : 天气数据用户自处理函�?
输入参数 : name:参数�?
           type:参数类型�?：int型；1：string�?
           data:参数值的地址
返回参数 : �?
*****************************************************************************/
void weather_data_user_handle(char *name, unsigned char type, char *data)
{
  #error "这里仅给出示例，请自行完善天气数据处理代�?完成后请删除该行"
  int value_int;
  char value_string[50];//由于有的参数内容较多，这里默认为50。您可以根据定义的参数，可以适当减少该�?
  
  my_memset(value_string, '/0', 50);
  
  //首先获取数据类型
  if(type == 0)//参数是INT�?
  {
    value_int = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
  }
  else if(type == 1)
  {
    my_strcpy(value_string, data);
  }
  
  //注意要根据所选参数类型来获得参数值！！！
  if(my_strcmp(name, "temp") == 0)
  {
    //printf("temp value is:%d", value_int);            //int�?
  }
  else if(my_strcmp(name, "humidity") == 0)
  {
    //printf("humidity value is:%d", value_int);        //int�?
  }
  else if(my_strcmp(name, "pm25") == 0)
  {
    //printf("pm25 value is:%d", value_int);            //int�?
  }
  else if(my_strcmp(name, "condition") == 0)
  {
    //printf("condition value is:%s", value_string);    //string�?
  }
}
#endif

#ifdef WIFI_STREAM_ENABLE
/*****************************************************************************
函数名称 : stream_file_trans
功能描述 : 流服务文件发�?
输入参数 : id:ID�?
          buffer:发送包的地址
          buf_len:发送包长度
返回参数 : �?
*****************************************************************************/
unsigned char stream_file_trans(unsigned int id, unsigned char *buffer, unsigned long buf_len)
{
 // #error "这里仅给出示例，请自行完善流服务处理代码,完成后请删除该行"
  unsigned short length = 0;
  unsigned long map_offset = 0;
  unsigned int pack_num = 0;
  unsigned int rest_length = 0;

  if(stop_update_flag == ENABLE)
    return SUCCESS;

  pack_num = buf_len / STREM_PACK_LEN;
  rest_length = buf_len - pack_num * STREM_PACK_LEN;
  if (rest_length > 0)
  {
    pack_num++;
  }

  int this_len = STREM_PACK_LEN;
  for (int cnt = 0; cnt < pack_num; cnt++)
  {
    if (cnt == pack_num - 1 && rest_length > 0)
    {
      this_len = rest_length;
    }
    else
    {
      this_len = STREM_PACK_LEN;
    }

    stream_trans(id, map_offset, buffer + map_offset, this_len);

    //while(stream_status == 0xff);//收到返回
    
    if(stream_status != 0)
    {
      return ERROR;
    }
  }
  
  return SUCCESS;
}

#endif
