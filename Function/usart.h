#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h"
#include "soft.h"
#include "stdarg.h"	 	 
#include "string.h"	   
////////////////////////////////////////////////////////////////////////////////// 	
#define TRACE u2_printf

#define DMA_UART1_Tx			DMA1_Channel4		//UART1_TX DMA通道定义
#define DMA_UART1_Rx			DMA1_Channel5		//UART1_RX DMA通道定义
#define DMA_UART3_TX_Channel	DMA1_Channel2		//QZ:UART3_TX DMA通道定义
#define DMA_UART3_RX			DMA1_Channel3		//QZ:UART3_RX DMA通道定义
#define DMA_UART2_TX    		DMA1_Channel7		//UART2_TX DMA通道定义
#define DMA_UART2_RX    		DMA1_Channel6		//UART2_RX DMA通道定义

#define	REPORT_DOCK_FAIL		0XF001
#define	REPORT_NO_DUST			0XE052
#define REPORT_TICK_FAIL		0XE041
#define	REPORT_LIDI				0XE051
#define	REPORT_FAN_OC			0XE101
#define REPORT_FAN_FIX			0XE102
#define REPORT_BUMP_FIX			0XE111
#define REPORT_RING_FIX			0XE131
#define REPORT_RING_OC			0XE132
#define REPORT_SKID				0XE133
#define REPORT_ZS_OC			0XE141
#define REPORT_ZS_FIX			0XE142
#define REPORT_BS_OC			0XE151
#define	REPORT_BS_FIX			0XE152
#define BAT_FAIL				0XE09X

//#define USART3_DMA 1		//QZ
#define	USART3_TX_SIZE	128

#ifdef YIS055
	#define	USART3_RX_SIZE	23
#else
	#define USART3_RX_SIZE	19//23
#endif

#if 0
#define	USART2_TX_SIZE	128
#define	USART2_RX_SIZE	128
#ifdef TUYA_WIFI
//#include "protocol.h"
//#include "system.h"
#define PROTOCOL_HEAD           0x07  // 需要和tuyasdk system.h里面一致 
#define WIFI_UART_QUEUE_LMT             16             
#define WIFI_UART_RECV_BUF_LMT          24 
#define WIFIR_UART_SEND_BUF_LMT         48  



#define	USART1_TX_SIZE	PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT
#ifdef DMA_IRQ   // DMA+串口中断方式
#define	USART1_RX_SIZE	PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT
#else  // DMA+查询方式  
#define	USART1_RX_SIZE	128
#endif

#else
#define	USART1_TX_SIZE	16
#define	USART1_RX_SIZE	128

#endif	  	
#endif

#ifdef TUYA_WIFI
	#define PROTOCOL_HEAD			0x07  // 需要和tuyasdk system.h里面一致 
	#define WIFI_UART_QUEUE_LMT 			16			   
	#define WIFI_UART_RECV_BUF_LMT			24 
	#define WIFIR_UART_SEND_BUF_LMT 		48
#endif

#ifdef   NEW_Q55_BOARD_1113
	#define USART1_TX_SIZE	128
	#define USART1_RX_SIZE	128
	#ifdef TUYA_WIFI
		#define	USART2_TX_SIZE	PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT
			#ifdef DMA_IRQ	 // DMA+串口中断方式
				#define USART2_RX_SIZE	PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT
			#else  // DMA+查询方式	
				#define USART2_RX_SIZE	128
			#endif
	#else
		#define USART2_TX_SIZE	16
		#define USART2_RX_SIZE	128
	#endif
#else
	#define USART2_TX_SIZE	128
	#define USART2_RX_SIZE	128
	#ifdef TUYA_WIFI
		#define USART1_TX_SIZE	PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT
		#ifdef DMA_IRQ   // DMA+串口中断方式
			#define	USART1_RX_SIZE	PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT
		#else  // DMA+查询方式  
			#define	USART1_RX_SIZE	128
		#endif
	#else	
		#define USART1_TX_SIZE	16
		#define USART1_RX_SIZE	128
	#endif
#endif

extern UART UART1;			//串口数据结构
extern UART UART2;
extern UART UART3;
extern GYRO_DATA	Gyro_Data;

extern u8 USART1_TX_BUF[USART1_TX_SIZE];
extern u8 USART1_RX_BUF[USART1_RX_SIZE];
extern u8 USART2_TX_BUF[USART2_TX_SIZE];
extern u8 USART2_RX_BUF[USART2_RX_SIZE];
extern u8 USART3_TX_BUF[USART3_TX_SIZE];
extern u8 USART3_RX_BUF[USART3_RX_SIZE];	//需存放小鸟数据
//extern bool Wall_report_time;	//QZ
//如果想串口中断接收，请不要注释以下宏定义
void URAT1_init(u32 bound);
void Usart1_Trans_Data(u8 length);
void DMA_Uart1Tx_Config(u16 DataLength);
void DMA_Uart1Rx_Config(unsigned short DataLength);
void DMA_UART1_NVIC_config(void);
void Uart1_Comunication(void);
void Uart2_Comunication(void);
void UART_TX_DMA_Config(void);
void UART_TX_DMA_Enable(void);
void Sensor_status_report(void);
void USART2_Init(void);
void USART2_DMA_RX_Init(u16 BufferSize);
void USART2_DMA_TX_Init(u16 BufferSize);
void USART2_TX_DMA_Enable(void);
void URAT3_init(u32 bound);
void USART3_Init(void);
void USART3_DMA_TX_Init(u16 BufferSize);
void USART3_DMA_RX_Init(u16 BufferSize);
void USART3_Putc(u8 c);
void DMA_USART3_TX_Length(u32 length);
void Uart3_Communication(void);
void Uart3_Communication_II(void);


void USART1_Putc(unsigned char c);
void USART1_Init(void);
void USART1_Send_Enter(void);
void USART1_Send_Str(char * s);
void u32tostr(unsigned long dat,unsigned char * str);

void u2_printf(char * fmt, ...);
void u3_printf(char* fmt,...);

void DMA_USART2_TX_Length(u32 length);

void DMA_USART1_TX_Length(u32 length);

void USART2_Putc(u8 c);
void USART2_Send_Enter(void);
void USART2_Send_Str(char *s);
void Init_Gyro_Data(void);

#endif



