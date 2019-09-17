/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: protocol.c
**描        述: 下发/上报数据处理函数
**使 用 说 明 :

                  *******非常重要，一定要看哦！！！********

** 1、用户在此文件中实现数据下发/上报功能
** 2、DP的ID/TYPE及数据处理函数都需要用户按照实际定义实现
** 3、当开始某些宏定义后需要用户实现代码的函数内部有#err提示,完成函数后请删除该#err
**
**--------------当前版本修订---------------------------------------------------
** 版  本: v2.5.1
** 日　期: 2018年10月27日
** 描　述: 1:默认关闭流服务功能
           2:增加03协议wifi状态宏定义
		   3:更新与修改部分函数注释
		   
** 版  本: v2.5.0
** 日　期: 2018年4月18日
** 描　述: 1:协议版本改为0x03
           2:增加WIFI模组心跳关闭功能
           3:增加天气功能

** 版  本: v2.3.8
** 日　期: 2018年1月17日
** 描　述: 1:变量添加volatile防止编译器优化
           2:添加#error提示

** 版  本: v2.3.7
** 日　期: 2017年4月18日
** 描　述: 1:优化串口队列接收处理
		   
** 版  本: v2.3.6
** 日　期: 2016年7月21日
** 描　述: 1:修复获取本地时间错误
           2:添加hex_to_bcd转换函数
		   
** 版  本: v2.3.5
** 日　期: 2016年6月3日
** 描　述: 1:修改返回协议版本为0x01
           2:固件升级数据偏移量修改为4字节

** 版  本: v2.3.4
** 日　期: 2016年5月26日
** 描　述: 1:优化串口解析函数
           2:优化编译器兼容性,取消enum类型定义

** 版  本: v2.3.3
** 日　期: 2016年5月24日
** 描　述: 1:修改mcu获取本地时间函数
           2:添加wifi功能测试

** 版  本: v2.3.2
** 日　期: 2016年4月23日
** 描　述: 1:优化串口数据解析
           2:优化MCU固件升级流程
           3:优化上报流程

** 版  本: v2.3.1
** 日　期: 2016年4月15日
** 描　述: 1:优化串口数据解析

** 版  本: v2.3
** 日　期: 2016年4月14日
** 描　述: 1:支持MCU固件在线升级

** 版  本: v2.2
** 日　期: 2016年4月11日
** 描　述: 1:修改串口数据接收方式

** 版  本: v2.1
** 日　期: 2016年4月8日
** 描　述: 1:加入某些编译器不支持函数指针兼容选项

** 版  本: v2.0
** 日　期: 2016年3月29日
** 描　述: 1:优化代码结构
           2:节省RAM空间
**
**-----------------------------------------------------------------------------
******************************************************************************/

#include "wifi.h"
#include "usart.h"
#include "power.h"

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
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢失
3:请勿在中断/定时器中断内调用上报函数
******************************************************************************/

         
/******************************************************************************
                              第一步:初始化
1:在需要使用到wifi相关文件的文件中include "wifi.h"
2:在MCU初始化中调用mcu_api.c文件中的wifi_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数内,并删除#error
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


/******************************************************************************
                           2:串口单字节发送函数
请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
******************************************************************************/

/*****************************************************************************
函数名称 : uart_transmit_output
功能描述 : 发数据处理
输入参数 : value:串口收到字节数据
返回参数 : 无
使用说明 : 请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
*****************************************************************************/
void uart_transmit_output(unsigned char value)
{
//  #error "请将MCU串口发送函数填入该函数,并删除该行"
/*
  //示例:
  extern void Uart_PutChar(unsigned char value);
  Uart_PutChar(value);	                                //串口发送函数
*/
}
/******************************************************************************
                           第二步:实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处理
当前函数处理全部数据上报(包括可下发/可上报和只上报)
  需要用户按照实际情况实现:
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上报
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上报
******************************************************************************/

//自动化生成数据上报函数

/*****************************************************************************
函数名称 : all_data_update
功能描述 : 系统所有dp点信息上传,实现APP和muc数据同步
输入参数 : 无
返回参数 : 无
使用说明 : 此函数SDK内部需调用;
           MCU必须实现该函数内数据上报功能;包括只上报和可上报可下发型数据
*****************************************************************************/

#include "soft.h"
extern MODE mode;		 //?????????????? 

void all_data_update(void)
{
		  /* 
		  //此代码为平台自动生成，请按照实际数据修改每个可下发可上报函数和只上报函数
		  mcu_dp_bool_update(DPID_POWER,当前电源开关); //BOOL型数据上报;
		  mcu_dp_bool_update(DPID_POWER_GO,当前清扫开关); //BOOL型数据上报;
		  mcu_dp_enum_update(DPID_MODE,当前清扫模式); //枚举型数据上报;
		  mcu_dp_enum_update(DPID_DIRECTION_CONTROL,当前方向); //枚举型数据上报;
		  mcu_dp_enum_update(DPID_STATUS,当前工作状态); //枚举型数据上报;
		  mcu_dp_value_update(DPID_ELECTRICITY_LEFT,当前剩余电量); //VALUE型数据上报;
		  mcu_dp_enum_update(DPID_SUCTION,当前吸力选择); //枚举型数据上报;
		  mcu_dp_string_update(DPID_CLEAN_RECORD,当前清扫记录指针,当前清扫记录数据长度); //STRING型数据上报;
		  mcu_dp_value_update(DPID_CLEAN_AREA,当前清扫面积); //VALUE型数据上报;
		  mcu_dp_value_update(DPID_CLEAN_TIME,当前清扫时间); //VALUE型数据上报;
		  mcu_dp_fault_update(DPID_FAULT,当前故障告警); //故障型数据上报;
		  mcu_dp_raw_update(DPID_MAP_CONFIG,当前地图参数指针,当前地图参数数据长度); //RAW型数据上报;
		 
		 */
	 unsigned char   i;
	 if (mode.sub_mode  != SLEEP) i = 1;
	 mcu_dp_bool_update(DPID_POWER,i); 
	 if ((mode.mode==CEASE)||(mode.mode==CHARGEING ))
		  i = 0;
	 else i = 1;
	 mcu_dp_bool_update(DPID_POWER_GO,i); 
	i = 0;
	if (mode.mode==SWEEP)
		 i = 0;
	if (mode.mode==YBS)	
		  i = 1;
	if (mode.mode==SPOT)
		  i = 2;
	if (mode.mode==DOCKING)	
		  i = 3;		
	 mcu_dp_enum_update(DPID_MODE,i); 

	 i = 0;
	 		// unsigned int action;		//????   0:??  1:??????  2?????? 3???   4????   5???1  6?????2  ??????18???14	??19????????
	 if (mode.action == 0) i = 4;
	 if (mode.action == 1) i = 2;	
	 if (mode.action == 2) i = 3;
	 if (mode.action == 3) i = 0;	
	 if (mode.action == 4) i = 1;
	 if (mode.action > 4) i = 2;	
	 
	 mcu_dp_enum_update(DPID_DIRECTION_CONTROL,i); 
    i = 0;
	if (mode.mode == CEASE)  {
		 i = 1;	 
	    if (mode.sub_mode == SLEEP)  i = 0;
		if (mode.sub_mode == ERR)  i = 7;
		}	
	if (mode.mode == SWEEP)  i = 2;	
	if (mode.mode == SPOT)  i = 2;		
	if (mode.mode == DOCKING)  i = 3;
	if (mode.mode == CHARGEING)  {
		    if(power.charge_dc) i = 6;
			if (power.charge_seat)i =5;
			if(Slam_Data.bat == 100)i = 4;
		}	
	mcu_dp_enum_update(DPID_STATUS,i);
	 i=Slam_Data.bat;
	 mcu_dp_value_update(DPID_ELECTRICITY_LEFT,i);

     i = 0;
		 mcu_dp_enum_update(DPID_SUCTION,i);
		 i = 0;

		 mcu_dp_fault_update(DPID_FAULT,0);
}
	/******************************************************************************
									WARNING!!!	  
								2:所有数据上报处理
	自动化代码模板函数,具体请用户自行实现数据处理
	******************************************************************************/
	
	
	/*****************************************************************************
	函数名称 : dp_download_power_handle
	功能描述 : 针对DPID_POWER的处理函数
	输入参数 : value:数据源数据
			: length:数据长度
	返回参数 : 成功返回:SUCCESS/失败返回:ERROR
	使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
	*****************************************************************************/
static unsigned char dp_download_power_handle(const unsigned char value[], unsigned short length)
		{
		  //示例:当前DP类型为BOOL
		  unsigned char ret;
		  //0:关/1:开
		  unsigned char power;
		  
		  power = mcu_get_dp_download_bool(value,length);
		  if(power == 0)
		  {
			//开关关
		  }
		  else
		  {
			//开关开
		  }
		  
		  //处理完DP数据后应有反馈
		  ret = mcu_dp_bool_update(DPID_POWER,power);
		  if(ret == SUCCESS)
			return SUCCESS;
		  else
			return ERROR;
		}
		
		/*****************************************************************************
		函数名称 : dp_download_power_go_handle
		功能描述 : 针对DPID_POWER_GO的处理函数
		输入参数 : value:数据源数据
				: length:数据长度
		返回参数 : 成功返回:SUCCESS/失败返回:ERROR
		使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
		*****************************************************************************/	
extern	void proc_wifi_play();
extern void proc_wifi_stop();		
static unsigned char dp_download_power_go_handle(const unsigned char value[], unsigned short length)
			{
			  //示例:当前DP类型为BOOL
			  unsigned char ret;
			  //0:关/1:开
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
			  
			  //处理完DP数据后应有反馈
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
    case 0:
      	  proc_wifi_play();
      break;
    
    case 1:
      	  proc_wifi_play();
      break;
      
    case 2:
      	  proc_wifi_stop();
      break;
      
    case 3:
      
      break;
      
    default:
      
      break;
  }
  
  //处理完DP数据后应有反馈
  ret = mcu_dp_enum_update(DPID_MODE,mode);
  if(ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_direction_control_handle
功能描述 : 针对DPID_DIRECTION_CONTROL的处理函数
输入参数 : value:数据源数据
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
    case 0:
      
      break;
      
    case 1:
      
      break;
      
    case 2:
      
      break;
      
    case 3:
      
      break;
      
    case 4:
      
      break;
      
    default:
      
      break;
  }
  
  //处理完DP数据后应有反馈
  ret = mcu_dp_enum_update(DPID_DIRECTION_CONTROL,direction_control);
  if(ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_suction_handle
功能描述 : 针对DPID_SUCTION的处理函数
输入参数 : value:数据源数据
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
    case 0:
      
      break;
      
    case 1:
      
      break;
      
    default:
      
      break;
  }
  
  //处理完DP数据后应有反馈
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
输入参数 : 无
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void mcu_write_rtctime(unsigned char time[])
{
  #error "请自行完成RTC时钟写入代码,并删除该行"
  /*
  time[0]为是否获取时间成功标志，为 0 表示失败，为 1表示成功
  time[1] 为 年 份 , 0x00 表 示2000 年
  time[2]为月份，从 1 开始到12 结束
  time[3]为日期，从 1 开始到31 结束
  time[4]为时钟，从 0 开始到23 结束
  time[5]为分钟，从 0 开始到59 结束
  time[6]为秒钟，从 0 开始到59 结束
  time[7]为星期，从 1 开始到 7 结束，1代表星期一
 */
  if(time[0] == 1)
  {
    //正确接收到wifi模块返回的本地时钟数据 
	 
  }
  else
  {
  	//获取本地时钟数据出错,有可能是当前wifi模块未联网
  }
}
#endif

#ifdef WIFI_TEST_ENABLE
/*****************************************************************************
函数名称 : wifi_test_result
功能描述 : wifi功能测试反馈
输入参数 : result:wifi功能测试结果;0:失败/1:成功
           rssi:测试成功表示wifi信号强度/测试失败表示错误类型
返回参数 : 无
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
      //未扫描到名称为tuya_mdev_test路由器,请检查
    }
    else if(rssi == 0x01)
    {
      //模块未授权
    }
  }
  else
  {
    //测试成功
    //rssi为信号强度(0-100, 0信号最差，100信号最强)
  }
  
}
#endif

#ifdef SUPPORT_MCU_FIRM_UPDATE
/*****************************************************************************
函数名称 : mcu_firm_update_handle
功能描述 : MCU进入固件升级模式
输入参数 : value:固件缓冲区
           position:当前数据包在于固件位置
           length:当前固件包长度(固件包长度为0时,表示固件包发送完成)
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length)
{
  #error "请自行完成MCU固件升级代码,完成后请删除该行"
  if(length == 0)
  {
    //固件数据发送完成
    
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
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
  /*********************************
  当前函数处理可下发/可上报数据调用                    
  具体函数内需要实现下发数据处理
  完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
  ***********************************/
  unsigned char ret;
  switch(dpid)
  {
    case DPID_POWER:
      //电源开关处理函数
      ret = dp_download_power_handle(value,length);
      break;
    case DPID_POWER_GO:
      //清扫开关处理函数
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
输入参数 : 无
返回参数 : 下发命令总和
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char get_download_cmd_total(void)
{
  return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}

#ifdef WEATHER_ENABLE

/*****************************************************************************
函数名称 : weather_open_return
功能描述 : 打开天气功能返回用户自处理函数
输入参数 : 1:res:打开天气功能返回结果，1:成功；0：失败

返回参数 : 无
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
    err_num = err;//获取错误码
  }
}

/*****************************************************************************
函数名称 : weather_data_user_handle
功能描述 : 天气数据用户自处理函数
输入参数 : name:参数名
           type:参数类型，0：int型；1：string型
           data:参数值的地址
返回参数 : 无
*****************************************************************************/
void weather_data_user_handle(char *name, unsigned char type, char *data)
{
  #error "这里仅给出示例，请自行完善天气数据处理代码,完成后请删除该行"
  int value_int;
  char value_string[50];//由于有的参数内容较多，这里默认为50。您可以根据定义的参数，可以适当减少该值
  
  my_memset(value_string, '/0', 50);
  
  //首先获取数据类型
  if(type == 0)//参数是INT型
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
    //printf("temp value is:%d", value_int);            //int型
  }
  else if(my_strcmp(name, "humidity") == 0)
  {
    //printf("humidity value is:%d", value_int);        //int型
  }
  else if(my_strcmp(name, "pm25") == 0)
  {
    //printf("pm25 value is:%d", value_int);            //int型
  }
  else if(my_strcmp(name, "condition") == 0)
  {
    //printf("condition value is:%s", value_string);    //string型
  }
}
#endif

#ifdef WIFI_STREAM_ENABLE
/*****************************************************************************
函数名称 : stream_file_trans
功能描述 : 流服务文件发送
输入参数 : id:ID号
          buffer:发送包的地址
          buf_len:发送包长度
返回参数 : 无
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
