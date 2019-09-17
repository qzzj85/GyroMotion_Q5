//===========================================================================================================
#include "stm32f10x_conf.h"
//===========================================================================================================

////IO方向设置			3
//#define MPU_SDA_IN()  {GPIOE->CRL&=0XFFFF0FFF;GPIOE->CRL|=8<<12;}
//#define MPU_SDA_OUT() {GPIOE->CRL&=0XFFFF0FFF;GPIOE->CRL|=3<<12;}



void 			IIC_Stop(void);
void 			IIC_Start(void);
void 			I2C_Ack(void);
void 			I2C_NoAck(void);
void 			I2C_SendByte(uint8_t byte);
uint8_t 	I2C_ReceiveByte(uint8_t ack);

uint8_t I2C_WaitAck(void);

uint8_t 	I2C_Write_One_Byte(uint8_t addr,uint8_t reg,uint8_t data);
uint8_t 	I2C_Read_One_Byte(uint8_t addr,uint8_t reg);


//int8_t 		i2c_write		(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data);
//uint8_t 	i2c_read		(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);

uint16_t 	i2cGetErrorCounter(void);


//uint8_t 	i2cWriteBuffer(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data);
//int8_t 		i2cwrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data);


//uint8_t 	IIC_Read_AT24C02_OneByte(uint8_t address);
//uint8_t 	i2cWriteBuffer(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data);




uint8_t GGPM01_i2cread(uint8_t *buf);



//int Sensors_I2C_ReadRegister	(
//																	unsigned char Address, 					unsigned char RegisterAddr, 
//																	unsigned short RegisterLen, 		unsigned char *RegisterValue
//															);

u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);








#define  SDA_GPIOE							GPIO_Pin_3
				#define  SDA_OFF						GPIO_ResetBits(GPIOE, SDA_GPIOE)
				#define  SDA_ON 						GPIO_SetBits(GPIOE, SDA_GPIOE)				
				#define  SDA_0							GPIO_ResetBits(GPIOE, SDA_GPIOE)
				#define  SDA_1 							GPIO_SetBits(GPIOE, SDA_GPIOE)
				
				#define	 SDA_read      			GPIO_ReadInputDataBit(GPIOE , SDA_GPIOE) 	
				
				//IO方向设置
				//#define SDA_IN()  			//{GPIOE->MODER&=~(3<<(3*2));GPIOE->MODER|=0<<3*2;}			//PD3输入模式
				//#define SDA_OUT() 			//{GPIOE->MODER&=~(3<<(3*2));GPIOE->MODER|=1<<3*2;} 		//PD3输出模式


#define  SCL_GPIOE							GPIO_Pin_2
				#define  SCL_OFF						GPIO_ResetBits(GPIOE, SCL_GPIOE)
				#define  SCL_ON 						GPIO_SetBits(GPIOE, SCL_GPIOE)
				#define  SCL_0							GPIO_ResetBits(GPIOE, SCL_GPIOE)
				#define  SCL_1 							GPIO_SetBits(GPIOE, SCL_GPIOE)
	
				
				
				#define	 SCL_read      			GPIO_ReadInputDataBit(GPIOE , SCL_GPIOE) 	























