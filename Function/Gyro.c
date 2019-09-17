
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

#include "stdio.h"


//==============================================================================================================================
//******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义************
//=====================全局变量====================================


//=====================全局函数====================================

#define MAG_ADDRESS 0x0C

////////#include "stm32f4xx.h"
//////#include "stdio.h"

//////#include "uart.h"
//////#include "i2c.h"
//////#include "gpio.h"
//////#include "gyro_main.h"
//////#include "board-st_discovery.h"
//////    
////////#include "inv_mpu.h"
////////#include "inv_mpu_dmp_motion_driver.h"
////////#include "invensense.h"
////////#include "invensense_adv.h"
////////#include "eMPL_outputs.h"
////////#include "mltypes.h"
//#include "mpu.h"
////////#include "log.h"
////////#include "packet.h"








//==============================================================================================================================
//******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义************
//=====================私有变量====================================uint8——


//=====================私有函数====================================

//===============================================================================================================
//===============================================================================================================


uint8_t BUF[10];      //接收数据缓存区
//void			READ_MPU9250_MAG(void);
uint8_t read_data;
//===============================================================================================================================
void  Gyro_Task (void)
{	
uint16_t 	in16;
	

		Init_MPU9250();
	
		while (1)
				{	
						MPU_Read_Len(MAG_ADDRESS,0x00,10,&BUF[0]);
				
						
						
						
						in16 = 65000;
						while(in16--);
						
						 BUF[0]=0;
						 BUF[1]=0;
						 BUF[2]=0;
						 BUF[3]=0;
						 BUF[4]=0;
						 BUF[5]=0;
						 BUF[6]=0;
						 BUF[7]=0;
						 BUF[8]=0;
						 BUF[9]=0;
				
						in16 = 65000;
						while(in16--);
				}
		
		



}
//===============================================================================================================================





















//****************初始化MPU9250，根据需要请参考pdf进行修改************************
void Init_MPU9250(void)
{	
	if((I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,WHO_AM_I)) == 0x71)
		{	
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		PWR_MGMT_1,		0X80); 	//解除休眠状态
			
			delay(100);
			
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		PWR_MGMT_1,		0X00);
			
			
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		CONFIG,				0x07);  //低通滤波频率，典型值：0x07(3600Hz)此寄存器内决定Internal_Sample_Rate==8K
	
		//	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		CONFIG,				0x06);  //低通滤波频率，典型值：0x07(3600Hz)此寄存器内决定Internal_Sample_Rate==8K
	
/////**********************Init SLV0 i2c**********************************/	
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		INT_PIN_CFG,						0x30); 		// 	INT Pin / Bypass Enable Configuration  		
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		I2C_MST_CTRL,						0x4D);		//	I2C MAster mode and Speed 400 kHz 
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		USER_CTRL,							0x20);		// 	I2C_MST _EN  
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		I2C_MST_DELAY_CTRL,			0x01); 		//	延时使能I2C_SLV0 _DLY_ enable 
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		I2C_SLV0_CTRL,					0x81);  	//	enable IIC	and EXT_SENS_DATA==1 Byte
			
//*******************Init GYRO and ACCEL******************************/	
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		SMPLRT_DIV,							0x07);   	//陀螺仪采样率，典型值：0x07(1kHz) (SAMPLE_RATE= Internal_Sample_Rate / (1 + SMPLRT_DIV) )
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		GYRO_CONFIG,						0x18); 		//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s) 
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		ACCEL_CONFIG,						0x18); 		//加速计自检、测量范围及高通滤波频率，典型值：0x18(不自检，16G)
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		ACCEL_CONFIG_2,					0x08); 		//加速计高通滤波频率 典型值 ：0x08  （1.13kHz）	

/**********************Init MAG **********************************/
	//		i2c_Mag_write(AK8963_CNTL2_REG,	AK8963_CNTL2_SRST); 		// Reset AK8963
	//		i2c_Mag_write(AK8963_CNTL1_REG,	0x12); 									// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output		
		
		
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		0x37,			0x02);//turn on Bypass Mode 
			
			 //BUF[5]=I2C_Read_One_Byte (MPU9250_CHIP_ADRESS_X68,0x37);
			 BUF[0]=I2C_Read_One_Byte (MAG_ADDRESS,0x00);
			 BUF[1]=I2C_Read_One_Byte (MAG_ADDRESS,0x01);
		}
}
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		



























////////************************加速度读取**************************/
//////void READ_MPU9250_ACCEL(void)//
//////{ 
//////   BUF[0]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_XOUT_L); 
//////   BUF[1]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_XOUT_H); 
////////   BUF[0]=MPU9250_Read_Reg(ACCEL_XOUT_L); 
////// //  BUF[1]=MPU9250_Read_Reg(ACCEL_XOUT_H);
//////   mpu_value.Accel[0]=	(BUF[1]<<8)|BUF[0];
//////   mpu_value.Accel[0]/=164; 						   //读取计算X轴数据/
//////	
//////	
//////   BUF[2]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_YOUT_L); 
//////   BUF[3]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_YOUT_H); 
//////  // BUF[2]=MPU9250_Read_Reg(ACCEL_YOUT_L);
//////  // BUF[3]=MPU9250_Read_Reg(ACCEL_YOUT_H);
//////   mpu_value.Accel[1]=	(BUF[3]<<8)|BUF[2];
//////   mpu_value.Accel[1]/=164; 						   //读取计算Y轴数据
//////	
//////   BUF[4]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_ZOUT_L); 
//////   BUF[5]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_ZOUT_H); 
//////  // BUF[4]=MPU9250_Read_Reg(ACCEL_ZOUT_L); 
//////  // BUF[5]=MPU9250_Read_Reg(ACCEL_ZOUT_H);
//////   mpu_value.Accel[2]=  (BUF[5]<<8)|BUF[4];
//////   mpu_value.Accel[2]/=164; 					      //读取计算Z轴数据 
//////}




///////**********************陀螺仪读取*****************************/
//////void READ_MPU9250_GYRO(void)
//////{
//////   BUF[0]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_XOUT_L); 
//////   BUF[1]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_XOUT_H);
//////   mpu_value.Gyro[0]=	(BUF[1]<<8)|BUF[0];
//////   mpu_value.Gyro[0]/=16.4; 						   //读取计算X轴数据

//////   BUF[2]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_YOUT_L);
//////   BUF[3]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_YOUT_H);
//////   mpu_value.Gyro[1]=	(BUF[3]<<8)|BUF[2];
//////   mpu_value.Gyro[1]/=16.4; 						   //读取计算Y轴数据
//////	
//////   BUF[4]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_ZOUT_L);
//////   BUF[5]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_ZOUT_H);
//////   mpu_value.Gyro[2]=	(BUF[5]<<8)|BUF[4];
//////   mpu_value.Gyro[2]/=16.4; 					     //读取计算Z轴数据
//////}






///////***************************************************************/
//////// MPU内部i2c 写入
////////I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
////////I2C_SLVx_REG:   reg
////////I2C_SLVx_Data out:  value
///////***************************************************************/
//////void i2c_Mag_write(uint8_t reg,uint8_t value)
//////{
//////	uint16_t j = 500;
//////	
//////	//MPU9250_Write_Reg(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR);//设置磁力计地址,mode: write
//////	//MPU9250_Write_Reg(I2C_SLV0_REG ,reg);//set reg addr
//////	//MPU9250_Write_Reg(I2C_SLV0_DO ,value);//send value	
//////	
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_ADDR ,		MPU9250_AK8963_ADDR);//设置磁力计地址,mode: write
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_REG ,		reg	);
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_DO ,			value	);
//////	
//////	while(j--);//此处因为MPU内部I2C读取速度较慢，延时等待内部写完毕
//////}
///////***************************************************************/
//////// MPU内部i2c 读取
////////I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
////////I2C_SLVx_REG:   reg
////////return value:   EXT_SENS_DATA_00 register value
///////***************************************************************/
//////uint8_t i2c_Mag_read(uint8_t reg)
//////{
//////	uint16_t 	j = 5000;
//////	//MPU9250_Write_Reg(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR|0x80); //设置磁力计地址，mode：read
//////	//MPU9250_Write_Reg(I2C_SLV0_REG ,reg);// set reg addr
//////	//MPU9250_Write_Reg(I2C_SLV0_DO ,0xff);//read
//////	
//////	
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_ADDR ,	MPU9250_AK8963_ADDR | 0x80); //设置磁力计地址，mode：read
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_REG ,	reg		);
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_DO ,		0xff	);
//////	
//////	while(j--);//此处因为MPU内部I2C读取速度较慢，必须延时等待内部读取完毕
//////	return I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	EXT_SENS_DATA_00);
//////	//return MPU9250_Read_Reg(EXT_SENS_DATA_00);
//////}



























uint8_t GGPM_BUF[5];
uint8_t GGPM01_Flag;
float GGPM01_Angle;
//======================================================================
uint8_t GGPM_Read(uint8_t *buf)
{				  
	IIC_Start();  	 	   
	
	I2C_SendByte(0x6B);   	//进入接收模式	
			if(I2C_WaitAck())		//等待应答
					{IIC_Stop();	 return 0;}		

	*(buf++)=I2C_ReceiveByte(1);
	*(buf++)=I2C_ReceiveByte(1);
	*(buf++)=I2C_ReceiveByte(1);
	*(buf++)=I2C_ReceiveByte(0);

	IIC_Stop();			//产生一个停止条件 
	return 1;		
}
//======================================================================


//======================================================================
void EPSON_Gyro(void)
{
int16_t angle;
	
	if( GGPM_Read(GGPM_BUF) )
			{
				angle = (((uint16_t)(GGPM_BUF[0])<<8)|GGPM_BUF[1]);		
				GGPM01_Angle = (float)(angle)/100;
			}
}
//======================================================================



////						flo = (float)(uin16)/100;
////						flo_2 = (float)(temp[0])/100;
////						printf("Angle: %4.2f   Rate: %4.2f\r\n",flo,flo_2);




























