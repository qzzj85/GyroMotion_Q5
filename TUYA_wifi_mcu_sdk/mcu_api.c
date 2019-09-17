/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: mcu_api.c
**描        述: 下发/上报数据处理函数
**使 用 说 明 : 此文件下函数无须用户修改,用户需要调用的文件都在该文件内
**
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
#define MCU_API_GLOBAL

#include "wifi.h"
  #if 1
#ifndef SUCCESS
#define         SUCCESS             1
#endif

#ifndef ERROR
#define         ERROR               0
#endif
#ifndef ENABLE
#define         ENABLE                1
#endif
  //
#ifndef DISABLE
#define         DISABLE               0
#endif
  #endif

/*****************************************************************************
函数名称 : hex_to_bcd
功能描述 : hex转bcd
输入参数 : Value_H:高字节/Value_L:低字节
返回参数 : bcd_value:转换完成后数据
*****************************************************************************/
unsigned char hex_to_bcd(unsigned char Value_H,unsigned char Value_L)
{
  unsigned char bcd_value;
  
  if((Value_H >= '0') && (Value_H <= '9'))
    Value_H -= '0';
  else if((Value_H >= 'A') && (Value_H <= 'F'))
    Value_H = Value_H - 'A' + 10;
  else if((Value_H >= 'a') && (Value_H <= 'f'))
    Value_H = Value_H - 'a' + 10;
   
  bcd_value = Value_H & 0x0f;
  
  bcd_value <<= 4;
  if((Value_L >= '0') && (Value_L <= '9'))
    Value_L -= '0';
  else if((Value_L >= 'A') && (Value_L <= 'F'))
    Value_L = Value_L - 'a' + 10;
  else if((Value_L >= 'a') && (Value_L <= 'f'))
    Value_L = Value_L - 'a' + 10;
  
  bcd_value |= Value_L & 0x0f;

  return bcd_value;
}
/*****************************************************************************
函数名称 : my_strlen
功能描述 : 求字符串长度
输入参数 : src:源地址
返回参数 : len:数据长度
*****************************************************************************/
unsigned long my_strlen(unsigned char *str)  
{
  unsigned long len = 0;
  if(str == NULL)
  { 
    return 0;
  }
  
  for(len = 0; *str ++ != '\0'; )
  {
    len ++;
  }
  
  return len;
}
/*****************************************************************************
函数名称 : my_memset
功能描述 : 把src所指内存区域的前count个字节设置成字符c
输入参数 : src:源地址
           ch:设置字符
           count:设置数据长度
返回参数 : src:数据处理完后的源地址
*****************************************************************************/
void *my_memset(void *src,unsigned char ch,unsigned short count)
{
  unsigned char *tmp = (unsigned char *)src;
  
  if(src == NULL)
  {
    return NULL;
  }
  
  while(count --)
  {
    *tmp ++ = ch;
  }
  
  return src;
}
/*****************************************************************************
函数名称 : mymemcpy
功能描述 : 内存拷贝
输入参数 : dest:目标地址
           src:源地址
           count:数据拷贝数量
返回参数 : src:数据处理完后的源地址
*****************************************************************************/
void *my_memcpy(void *dest, const void *src, unsigned short count)  
{  
  unsigned char *pdest = (unsigned char *)dest;  
  const unsigned char *psrc  = (const unsigned char *)src;  
  unsigned short i;
  
  if(dest == NULL || src == NULL)
  { 
    return NULL;
  }
  
  if((pdest <= psrc) || (pdest > psrc + count))
  {  
    for(i = 0; i < count; i ++)
    {  
      pdest[i] = psrc[i];  
    }  
  }
  else
  {
    for(i = count; i > 0; i --)
    {  
      pdest[i - 1] = psrc[i - 1];  
    }  
  }  
  
  return dest;  
}
/*****************************************************************************
函数名称 : my_strcpy
功能描述 : 内存拷贝
输入参数 : s1:目标地址
           s2:源地址
返回参数 : 数据处理完后的源地址
*****************************************************************************/
char *my_strcpy(char *dest, const char *src)  
{
  char *p = dest;
  while(*src!='\0')
  {
    *dest++ = *src++;
  }
  *dest = '\0';
  return p;
}
/*****************************************************************************
函数名称 : my_strcmp
功能描述 : 内存拷贝
输入参数 : s1:字符串1
           s2:字符串2
返回参数 : 大小比较值，0:s1=s2; -1:s1<s2; 1:s1>s2
*****************************************************************************/
int my_strcmp(char *s1 , char *s2)
{
  while( *s1 && *s2 && *s1 == *s2 )
  {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}
/*****************************************************************************
函数名称 : int_to_byte
功能描述 : 将int类型拆分四个字节
输入参数 : number:4字节原数据;value:处理完成后4字节数据
返回参数 :无
****************************************************************************/
void int_to_byte(unsigned long number,unsigned char value[4])
{
  value[0] = number >> 24;
  value[1] = number >> 16;
  value[2] = number >> 8;
  value[3] = number & 0xff;
}
/*****************************************************************************
函数名称 : byte_to_int
功能描述 : 将4字节合并为1个32bit变量
输入参数 : value:4字节数组
返回参数 : number:合并完成后的32bit变量
****************************************************************************/
unsigned long byte_to_int(const unsigned char value[4])
{
  unsigned long nubmer = 0;

  nubmer = (unsigned long)value[0];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[1];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[2];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[3];
  
  return nubmer;
}
#ifndef WIFI_CONTROL_SELF_MODE
/*****************************************************************************
函数名称 : mcu_get_reset_wifi_flag
功能描述 : MCU获取复位wifi成功标志
输入参数 : 无
返回参数 : 复位标志:RESET_WIFI_ERROR:失败/RESET_WIFI_SUCCESS:成功
使用说明 : 1:MCU主动调用mcu_reset_wifi()后调用该函数获取复位状态
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
unsigned char mcu_get_reset_wifi_flag(void)
{
  return reset_wifi_flag;
}
/*****************************************************************************
函数名称 : reset_wifi
功能描述 : MCU主动重置wifi工作模式
输入参数 : 无
返回参数 : 无
使用说明 : 1:MCU主动调用,通过mcu_get_reset_wifi_flag()函数获取重置wifi是否成功
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
void mcu_reset_wifi(void)
{
  reset_wifi_flag = RESET_WIFI_ERROR;
  
  wifi_uart_write_frame(WIFI_RESET_CMD, 0);
}
/*****************************************************************************
函数名称 : mcu_get_wifimode_flag
功能描述 : 获取设置wifi状态成功标志
输入参数 : 无
返回参数 : SET_WIFICONFIG_ERROR:失败/SET_WIFICONFIG_SUCCESS:成功
使用说明 : 1:MCU主动调用mcu_set_wifi_mode()后调用该函数获取复位状态
           2:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
unsigned char mcu_get_wifimode_flag(void)
{
  return set_wifimode_flag;
}
/*****************************************************************************
函数名称 : mcu_set_wifi_mode
功能描述 : MCU设置wifi工作模式
输入参数 : mode:
          SMART_CONFIG:进入smartconfig模式
          AP_CONFIG:进入AP模式
返回参数 : 无
使用说明 : 1:MCU主动调用
           2:成功后,可判断set_wifi_config_state是否为TRUE;TRUE表示为设置wifi工作模式成功
           3:如果为模块自处理模式,MCU无须调用该函数
*****************************************************************************/
void mcu_set_wifi_mode(unsigned char mode)
{
  unsigned char length = 0;
  
  set_wifimode_flag = SET_WIFICONFIG_ERROR;
  
  length = set_wifi_uart_byte(length, mode);
  
  wifi_uart_write_frame(WIFI_MODE_CMD, length);
}
/*****************************************************************************
函数名称 : mcu_get_wifi_work_state
功能描述 : MCU主动获取当前wifi工作状态
输入参数 : 无
返回参数 : WIFI_WORK_SATE_E:
          SMART_CONFIG_STATE:smartconfig配置状态
          AP_STATE:AP 配置状态
          WIFI_NOT_CONNECTED:WIFI 配置成功但未连上路由器
          WIFI_CONNECTED:WIFI 配置成功且连上路由器
          WIFI_CONN_CLOUD:WIFI 已经连接上云服务器
          WIFI_LOW_POWER:WIFI 处于低功耗模式
使用说明 : 无
*****************************************************************************/
unsigned char mcu_get_wifi_work_state(void)
{
  return wifi_work_state;
}
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
/*****************************************************************************
函数名称  : mcu_get_system_time
功能描述 : MCU获取系统时间,用于校对本地时钟
输入参数 : 无
返回参数 : 无
使用说明 : MCU主动调用完成后在mcu_write_rtctime函数内校对rtc时钟
*****************************************************************************/
void mcu_get_system_time(void)
{
  wifi_uart_write_frame(GET_LOCAL_TIME_CMD,0);
}
#endif

#ifdef WIFI_TEST_ENABLE
/*****************************************************************************
函数名称 : mcu_start_wifitest
功能描述 : mcu发起wifi功能测试
输入参数 : 无
返回参数 : 无
使用说明 : MCU需要自行调用该功能
*****************************************************************************/
void mcu_start_wifitest(void)
{
  wifi_uart_write_frame(WIFI_TEST_CMD,0);
}
#endif

#ifdef WIFI_HEARTSTOP_ENABLE
/*****************************************************************************
函数名称 : wifi_heart_stop
功能描述 : 通知WIFI模组关闭心跳
输入参数 : 无
返回参数 : 无
*****************************************************************************/
void wifi_heart_stop(void)
{
  wifi_uart_write_frame(HEAT_BEAT_STOP,0);
}
#endif

/*****************************************************************************
函数名称 : mcu_dp_raw_update
功能描述 : raw型dp数据上传
输入参数 : dpid:id号
           value:当前dp值指针
           len:数据长度
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_raw_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  //
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_RAW);
  //
  length = set_wifi_uart_byte(length,len / 0x100);
  length = set_wifi_uart_byte(length,len % 0x100);
  //
  length = set_wifi_uart_buffer(length,(unsigned char *)value,len);
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_bool_update
功能描述 : bool型dp数据上传
输入参数 : dpid:id号
           value:当前dp值
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_bool_update(unsigned char dpid,unsigned char value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_BOOL);
  //
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,1);
  //
  if(value == FALSE)
  {
    length = set_wifi_uart_byte(length,FALSE);
  }
  else
  {
    length = set_wifi_uart_byte(length,1);
  }
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_value_update
功能描述 : value型dp数据上传
输入参数 : dpid:id号
           value:当前dp值
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_value_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_VALUE);
  //
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,4);
  //
  length = set_wifi_uart_byte(length,value >> 24);
  length = set_wifi_uart_byte(length,value >> 16);
  length = set_wifi_uart_byte(length,value >> 8);
  length = set_wifi_uart_byte(length,value & 0xff);
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_string_update
功能描述 : rstring型dp数据上传
输入参数 : dpid:id号
           value:当前dp值指针
           len:数据长度
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_string_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  //
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_STRING);
  //
  length = set_wifi_uart_byte(length,len / 0x100);
  length = set_wifi_uart_byte(length,len % 0x100);
  //
  length = set_wifi_uart_buffer(length,(unsigned char *)value,len);
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_enum_update
功能描述 : enum型dp数据上传
输入参数 : dpid:id号
           value:当前dp值
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_enum_update(unsigned char dpid,unsigned char value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_ENUM);
  //
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,1);
  //
  length = set_wifi_uart_byte(length,value);
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_dp_fault_update
功能描述 : fault型dp数据上传
输入参数 : dpid:id号
           value:当前dp值
返回参数 : 无
*****************************************************************************/
unsigned char mcu_dp_fault_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;
   
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_FAULT);
  //
  length = set_wifi_uart_byte(length,0);
  
  if((value | 0xff) == 0xff)
  {
    length = set_wifi_uart_byte(length,1);
    length = set_wifi_uart_byte(length,value);
  }
  else if((value | 0xffff) == 0xffff)
  {
    length = set_wifi_uart_byte(length,2);
    length = set_wifi_uart_byte(length,value >> 8);
    length = set_wifi_uart_byte(length,value & 0xff);
  }
  else
  {
    length = set_wifi_uart_byte(length,4);
    length = set_wifi_uart_byte(length,value >> 24);
    length = set_wifi_uart_byte(length,value >> 16);
    length = set_wifi_uart_byte(length,value >> 8);
    length = set_wifi_uart_byte(length,value & 0xff);
  }    
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);

  return SUCCESS;
}
/*****************************************************************************
函数名称 : mcu_get_dp_download_bool
功能描述 : mcu获取bool型下发dp值
输入参数 : value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : bool:当前dp值
*****************************************************************************/
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len)
{
  return(value[0]);
}
/*****************************************************************************
函数名称 : mcu_get_dp_download_enum
功能描述 : mcu获取enum型下发dp值
输入参数 : value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : enum:当前dp值
*****************************************************************************/
unsigned char mcu_get_dp_download_enum(const unsigned char value[],unsigned short len)
{
  return(value[0]);
}
/*****************************************************************************
函数名称 : mcu_get_dp_download_value
功能描述 : mcu获取value型下发dp值
输入参数 : value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : value:当前dp值
*****************************************************************************/
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len)
{
  return(byte_to_int(value));
}
/*****************************************************************************
函数名称 : uart_receive_input
功能描述 : 收数据处理
输入参数 : value:串口收到字节数据
返回参数 : 无
使用说明 : 在MCU串口接收函数中调用该函数,并将接收到的数据作为参数传入
*****************************************************************************/
void uart_receive_input(unsigned char value)
{
//  #error "请在串口接收中断中调用uart_receive_input(value),串口数据由MCU_SDK处理,用户请勿再另行处理,完成后删除该行" 

  if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(wifi_queue_buf)))
  {
    //数据队列满
  }
  else if((queue_in < queue_out) && ((queue_out  - queue_in) == 0))
  {
    //数据队列满
  }
  else
  {
    //队列不满
    if(queue_in >= (unsigned char *)(wifi_queue_buf + sizeof(wifi_queue_buf)))
    {
      queue_in = (unsigned char *)(wifi_queue_buf);
    }
    
    *queue_in ++ = value;
  }
}
/*****************************************************************************
函数名称  : wifi_uart_service
功能描述  : wifi串口处理服务
输入参数 : 无
返回参数 : 无
使用说明 : 在MCU主函数while循环中调用该函数
*****************************************************************************/
#if  1//shf  ��ͨ�����жϷ�ʽ 
void wifi_uart_service(void)
{
//  #error "请直接在main函数的while(1){}中添加wifi_uart_service(),调用该函数不要加任何条件判断,完成后删除该行" 
  static unsigned short rx_in = 0;
  unsigned short offset = 0;
  unsigned short rx_value_len = 0;             //数据帧长度
  
  while((rx_in < sizeof(wifi_uart_rx_buf)) && get_queue_total_data() > 0)
  {
    wifi_uart_rx_buf[rx_in ++] = Queue_Read_Byte();
  }
  
  if(rx_in < PROTOCOL_HEAD)
    return;
  
  while((rx_in - offset) >= PROTOCOL_HEAD)
  {
    if(wifi_uart_rx_buf[offset + HEAD_FIRST] != FRAME_FIRST)
    {
      offset ++;
      continue;
    }
    
    if(wifi_uart_rx_buf[offset + HEAD_SECOND] != FRAME_SECOND)
    {
      offset ++;
      continue;
    }  
    
    if(wifi_uart_rx_buf[offset + PROTOCOL_VERSION] != VERSION)
    {
      offset += 2;
      continue;
    }      
    
    rx_value_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] * 0x100;
    rx_value_len += (wifi_uart_rx_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD);
    if(rx_value_len > sizeof(wifi_uart_rx_buf) + PROTOCOL_HEAD)
    {
      offset += 3;
      continue;
    }
    
    if((rx_in - offset) < rx_value_len)
    {
      break;
    }
    
    //数据接收完成
    if(get_check_sum((unsigned char *)wifi_uart_rx_buf + offset,rx_value_len - 1) != wifi_uart_rx_buf[offset + rx_value_len - 1])
    {
      //校验出错
      offset += 3;
      continue;
    }
    
    data_handle(offset);
    offset += rx_value_len;
  }//end while

  rx_in -= offset;
  if(rx_in > 0)
  {
    my_memcpy((char *)wifi_uart_rx_buf,(const char *)wifi_uart_rx_buf + offset,rx_in);
  }
}

#else // DMA  + �����жϷ�ʽ 
void wifi_uart_service(unsigned char offset ,unsigned int rx_value_len )
{
   //���ݽ������
    if(get_check_sum((unsigned char *)wifi_uart_rx_buf,rx_value_len - 1) == wifi_uart_rx_buf[0 + rx_value_len - 1])
    {
      //У��ok 
       data_handle(offset);
    }
}
#endif
/*****************************************************************************
函数名称 :  wifi_protocol_init
功能描述 : 协议串口初始化函数
输入参数 : 无
返回参数 : 无
使用说明 : 必须在MCU初始化代码中调用该函数
*****************************************************************************/
void wifi_protocol_init(void)
{
//  #error " 请在main函数中添加wifi_protocol_init()完成wifi协议初始化,并删除该行"
  queue_in = (unsigned char *)wifi_queue_buf;
  queue_out = (unsigned char *)wifi_queue_buf;
  //
#ifndef WIFI_CONTROL_SELF_MODE
  wifi_work_state = WIFI_SATE_UNKNOW;
#endif
}

#ifdef WIFI_STREAM_ENABLE
/*****************************************************************************
函数名称 : stream_open 
功能描述 : 开启流服务功能
输入参数 : 无
返回参数 : 无
*****************************************************************************/
unsigned char stream_open(void)
{
  stream_status = 0xff;

  if(stop_update_flag == ENABLE)
    return SUCCESS;

  wifi_uart_write_frame(STREAM_OPEN_CMD, 0);
}

/*****************************************************************************
函数名称 : stream_start 
功能描述 : 开启流数据传输
输入参数 : id:流服务标识
返回参数 : 无
*****************************************************************************/
unsigned char stream_start(unsigned int id)
{
  unsigned short length = 0;

  stream_status = 0xff;

  if(stop_update_flag == ENABLE)
    return SUCCESS;

  //ID
  length = set_wifi_uart_byte(length,id / 0x100);
  length = set_wifi_uart_byte(length,id % 0x100);

  wifi_uart_write_frame(STREAM_START_CMD, length);
}

/*****************************************************************************
函数名称 : stream_trans 
功能描述 : 流数据传输
输入参数 : id:流服务标识;offset:偏移量;buffer:数据地址;buf_len:数据长度
返回参数 : 无
*****************************************************************************/
unsigned char stream_trans(unsigned int id, unsigned char offset, unsigned char *buffer, unsigned long buf_len)
{
  unsigned short length = 0;

  stream_status = 0xff;

  if(stop_update_flag == ENABLE)
    return SUCCESS;

  //ID
  length = set_wifi_uart_byte(length,id / 0x100);
  length = set_wifi_uart_byte(length,id % 0x100);
  //偏移量
  length = set_wifi_uart_byte(length,offset >> 24);
  length = set_wifi_uart_byte(length,offset >> 16);
  length = set_wifi_uart_byte(length,offset >> 8);
  length = set_wifi_uart_byte(length,offset % 256);
  //数据
  length = set_wifi_uart_buffer(length, buffer, buf_len);
  wifi_uart_write_frame(STREAM_TRANS_CMD, length);
}

/*****************************************************************************
函数名称 : stream_close 
功能描述 : 结束流数据传输
输入参数 : id:流服务标识;offset:偏移量
返回参数 : 无
*****************************************************************************/
unsigned char stream_stop(unsigned char id, unsigned long offset)
{
  unsigned short length = 0;

  stream_status = 0xff;

  if(stop_update_flag == ENABLE)
    return SUCCESS;

  //ID
  length = set_wifi_uart_byte(length,id / 0x100);
  length = set_wifi_uart_byte(length,id % 0x100);

  //偏移量
  length = set_wifi_uart_byte(length,offset >> 24);
  length = set_wifi_uart_byte(length,offset >> 16);
  length = set_wifi_uart_byte(length,offset >> 8);
  length = set_wifi_uart_byte(length,offset % 256);

  //数据
  wifi_uart_write_frame(STREAM_STOP_CMD, length);
}
#endif
