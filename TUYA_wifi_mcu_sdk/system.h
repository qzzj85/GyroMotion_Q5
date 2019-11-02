/****************************************Copyright (c)*************************
**                               版权所�?(C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**�?  �?  �? system.c
**�?       �? wifi数据处理函数
**�?�?�?�?: 用户无需关心该文件实现内�?
**
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
#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#ifdef SYSTEM_GLOBAL
  #define SYSTEM_EXTERN
#else
  #define SYSTEM_EXTERN   extern
#endif

//=============================================================================
//帧的字节顺序
//=============================================================================
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1        
#define         PROTOCOL_VERSION                2
#define         FRAME_TYPE                      3
#define         LENGTH_HIGH                     4
#define         LENGTH_LOW                      5
#define         DATA_START                      6

//=============================================================================
//数据帧类�?
//=============================================================================
#define         HEAT_BEAT_CMD                   0                               //心跳�?
#define         PRODUCT_INFO_CMD                1                               //产品信息
#define         WORK_MODE_CMD                   2                               //查询MCU 设定的模块工作模�?
#define         WIFI_STATE_CMD                  3                               //wifi工作状�?
#define         WIFI_RESET_CMD                  4                               //重置wifi
#define         WIFI_MODE_CMD                   5                               //选择smartconfig/AP模式	
#define         DATA_QUERT_CMD                  6                               //命令下发
#define         STATE_UPLOAD_CMD                7                               //状态上�? 
#define         STATE_QUERY_CMD                 8                               //状态查�?  
#define         UPDATE_QUERY_CMD                9                               //升级查询
#define         UPDATE_START_CMD                0x0a                            //升级开�?
#define         UPDATE_TRANS_CMD                0x0b                            //升级传输
#define         GET_ONLINE_TIME_CMD             0x0c                            //获取系统时间(格林威治时间)
#define         FACTORY_MODE_CMD                0x0d                            //进入产测模式    
#define         WIFI_TEST_CMD                   0x0e                            //wifi功能测试
#define         GET_LOCAL_TIME_CMD              0x1c                            //获取本地时间
#define         WEATHER_OPEN_CMD                0x20                            //打开天气          
#define         WEATHER_DATA_CMD                0x21                            //天气数据
#define         HEAT_BEAT_STOP                  0x25                            //关闭WIFI模组心跳
#define         STREAM_OPEN_CMD                 0x26                            //开启流服务功能
#define         STREAM_START_CMD                0x27                            //开启流数据传输
#define         STREAM_TRANS_CMD                0x28                            //流数据传�?
#define         STREAM_STOP_CMD                 0x29                            //结束流数据传�?


//=============================================================================
#define VERSION                 0x00                                            //协议版本�?
#define PROTOCOL_HEAD           0x07                                            //固定协议头长�?
#define FIRM_UPDATA_SIZE        256                                            //升级包大�?
#define FRAME_FIRST             0x55
#define FRAME_SECOND            0xaa
//============================================================================= 
SYSTEM_EXTERN volatile unsigned char wifi_queue_buf[PROTOCOL_HEAD + WIFI_UART_QUEUE_LMT];  //串口队列缓存
SYSTEM_EXTERN volatile unsigned char wifi_uart_rx_buf[PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT];         //串口接收缓存
//SYSTEM_EXTERN volatile unsigned char wifi_uart_rx_buf[128]; 

SYSTEM_EXTERN volatile unsigned char wifi_uart_tx_buf[PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT];        //串口发送缓�?
//
SYSTEM_EXTERN volatile unsigned char *queue_in;
SYSTEM_EXTERN volatile unsigned char *queue_out;

SYSTEM_EXTERN volatile unsigned char stop_update_flag;

#ifndef WIFI_CONTROL_SELF_MODE
SYSTEM_EXTERN volatile unsigned char reset_wifi_flag;                                                  //重置wifi标志(TRUE:成功/FALSE:失败)
SYSTEM_EXTERN volatile unsigned char set_wifimode_flag;                                                //设置WIFI工作模式标志(TRUE:成功/FALSE:失败)
SYSTEM_EXTERN volatile unsigned char wifi_work_state;                                                  //wifi模块当前工作状�?
#endif

#ifdef WIFI_STREAM_ENABLE
SYSTEM_EXTERN volatile char stream_status;                                                             //流服务发包返回状�?

extern void wifi_uart_write_stream_init(unsigned char  id, unsigned short map_len);


#endif

/*****************************************************************************
函数名称 : set_wifi_uart_byte
功能描述 : 写wifi_uart字节
输入参数 : dest:缓存区其实地址;
           byte:写入字节�?
返回参数 : 写入完成后的总长�?
*****************************************************************************/
unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte);

/*****************************************************************************
函数名称 : set_wifi_uart_buffer
功能描述 : 写wifi_uart_buffer
输入参数 : dest:目标地址
           src:源地址
           len:数据长度
返回参数 : �?
*****************************************************************************/
unsigned short set_wifi_uart_buffer(unsigned short dest, unsigned char *src, unsigned short len);

/*****************************************************************************
函数名称 : wifi_uart_write_frame
功能描述 : 向wifi串口发送一帧数�?
输入参数 : fr_type:帧类�?
           len:数据长度
返回参数 : �?
*****************************************************************************/
void wifi_uart_write_frame(unsigned char fr_type, unsigned short len);

/*****************************************************************************
函数名称 : get_check_sum
功能描述 : 计算校验�?
输入参数 : pack:数据源指�?
           pack_len:计算校验和长�?
返回参数 : 校验�?
*****************************************************************************/
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);

/*****************************************************************************
函数名称 : data_handle
功能描述 : 数据帧处�?
输入参数 : offset:数据起始�?
返回参数 : �?
*****************************************************************************/
void data_handle(unsigned short offset);

/*****************************************************************************
函数名称 : get_queue_total_data
功能描述 : 读取队列内数�?
输入参数 : �?
返回参数 : �?
*****************************************************************************/
unsigned char get_queue_total_data(void);

/*****************************************************************************
函数名称 : Queue_Read_Byte
功能描述 : 读取队列1字节数据
输入参数 : �?
返回参数 : �?
*****************************************************************************/
unsigned char Queue_Read_Byte(void);

#endif
  


