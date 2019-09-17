#include "stm32f10x_conf.h"



void Ultrasonic(void);
void SPI3_Init(void);

void SPI3_SetSpeed(uint8_t SPI_BaudRatePrescaler);

void SPI3_Init(void);


struct  ULTRASONIC_DATA        
	{
		uint8_t State;	
		uint32_t time_start;	
		uint32_t time_stop;	
	
//		unsigned char key;		 						//键值  ，当前状态
//		unsigned char lastkey;
		unsigned char number;     				//去抖次数
//		unsigned char (*read_start)(); 	//读取得状态函数
//		unsigned char flag;
		
		
		unsigned char sign;	 						//标志  表示该按键有效，没有被处理
		bool Repeat_time;
		
		float Distance[4];	
		uint16_t UD_Time[8];	
		uint8_t UD_Time_Counter;	
	};

extern struct ULTRASONIC_DATA		Ultra;


void DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void DMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
	
	
void init_time_6(void);

	
//	陀螺数据解算后的数据
struct	GYRO_MODE	
{
 uint8_t  mode;	  
 
 int32_t 	X;
 int32_t 	Y;
 int32_t 	Theta;
	
};
	
extern struct GYRO_MODE Imu;
	


