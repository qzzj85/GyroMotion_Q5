
//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================

#include "stdio.h"


//==============================================================================================================================
//******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���************
//=====================ȫ�ֱ���====================================


//=====================ȫ�ֺ���====================================

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
//******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���************
//=====================˽�б���====================================uint8����


//=====================˽�к���====================================

//===============================================================================================================
//===============================================================================================================


uint8_t BUF[10];      //�������ݻ�����
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





















//****************��ʼ��MPU9250��������Ҫ��ο�pdf�����޸�************************
void Init_MPU9250(void)
{	
	if((I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,WHO_AM_I)) == 0x71)
		{	
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		PWR_MGMT_1,		0X80); 	//�������״̬
			
			delay(100);
			
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		PWR_MGMT_1,		0X00);
			
			
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		CONFIG,				0x07);  //��ͨ�˲�Ƶ�ʣ�����ֵ��0x07(3600Hz)�˼Ĵ����ھ���Internal_Sample_Rate==8K
	
		//	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		CONFIG,				0x06);  //��ͨ�˲�Ƶ�ʣ�����ֵ��0x07(3600Hz)�˼Ĵ����ھ���Internal_Sample_Rate==8K
	
/////**********************Init SLV0 i2c**********************************/	
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		INT_PIN_CFG,						0x30); 		// 	INT Pin / Bypass Enable Configuration  		
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		I2C_MST_CTRL,						0x4D);		//	I2C MAster mode and Speed 400 kHz 
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		USER_CTRL,							0x20);		// 	I2C_MST _EN  
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		I2C_MST_DELAY_CTRL,			0x01); 		//	��ʱʹ��I2C_SLV0 _DLY_ enable 
//////			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		I2C_SLV0_CTRL,					0x81);  	//	enable IIC	and EXT_SENS_DATA==1 Byte
			
//*******************Init GYRO and ACCEL******************************/	
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		SMPLRT_DIV,							0x07);   	//�����ǲ����ʣ�����ֵ��0x07(1kHz) (SAMPLE_RATE= Internal_Sample_Rate / (1 + SMPLRT_DIV) )
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		GYRO_CONFIG,						0x18); 		//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s) 
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		ACCEL_CONFIG,						0x18); 		//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x18(���Լ죬16G)
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		ACCEL_CONFIG_2,					0x08); 		//���ټƸ�ͨ�˲�Ƶ�� ����ֵ ��0x08  ��1.13kHz��	

/**********************Init MAG **********************************/
	//		i2c_Mag_write(AK8963_CNTL2_REG,	AK8963_CNTL2_SRST); 		// Reset AK8963
	//		i2c_Mag_write(AK8963_CNTL1_REG,	0x12); 									// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output		
		
		
			I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,		0x37,			0x02);//turn on Bypass Mode 
			
			 //BUF[5]=I2C_Read_One_Byte (MPU9250_CHIP_ADRESS_X68,0x37);
			 BUF[0]=I2C_Read_One_Byte (MAG_ADDRESS,0x00);
			 BUF[1]=I2C_Read_One_Byte (MAG_ADDRESS,0x01);
		}
}
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		



























////////************************���ٶȶ�ȡ**************************/
//////void READ_MPU9250_ACCEL(void)//
//////{ 
//////   BUF[0]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_XOUT_L); 
//////   BUF[1]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_XOUT_H); 
////////   BUF[0]=MPU9250_Read_Reg(ACCEL_XOUT_L); 
////// //  BUF[1]=MPU9250_Read_Reg(ACCEL_XOUT_H);
//////   mpu_value.Accel[0]=	(BUF[1]<<8)|BUF[0];
//////   mpu_value.Accel[0]/=164; 						   //��ȡ����X������/
//////	
//////	
//////   BUF[2]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_YOUT_L); 
//////   BUF[3]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_YOUT_H); 
//////  // BUF[2]=MPU9250_Read_Reg(ACCEL_YOUT_L);
//////  // BUF[3]=MPU9250_Read_Reg(ACCEL_YOUT_H);
//////   mpu_value.Accel[1]=	(BUF[3]<<8)|BUF[2];
//////   mpu_value.Accel[1]/=164; 						   //��ȡ����Y������
//////	
//////   BUF[4]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_ZOUT_L); 
//////   BUF[5]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,ACCEL_ZOUT_H); 
//////  // BUF[4]=MPU9250_Read_Reg(ACCEL_ZOUT_L); 
//////  // BUF[5]=MPU9250_Read_Reg(ACCEL_ZOUT_H);
//////   mpu_value.Accel[2]=  (BUF[5]<<8)|BUF[4];
//////   mpu_value.Accel[2]/=164; 					      //��ȡ����Z������ 
//////}




///////**********************�����Ƕ�ȡ*****************************/
//////void READ_MPU9250_GYRO(void)
//////{
//////   BUF[0]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_XOUT_L); 
//////   BUF[1]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_XOUT_H);
//////   mpu_value.Gyro[0]=	(BUF[1]<<8)|BUF[0];
//////   mpu_value.Gyro[0]/=16.4; 						   //��ȡ����X������

//////   BUF[2]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_YOUT_L);
//////   BUF[3]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_YOUT_H);
//////   mpu_value.Gyro[1]=	(BUF[3]<<8)|BUF[2];
//////   mpu_value.Gyro[1]/=16.4; 						   //��ȡ����Y������
//////	
//////   BUF[4]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_ZOUT_L);
//////   BUF[5]=I2C_Read_One_Byte(MPU9250_CHIP_ADRESS_X68,	GYRO_ZOUT_H);
//////   mpu_value.Gyro[2]=	(BUF[5]<<8)|BUF[4];
//////   mpu_value.Gyro[2]/=16.4; 					     //��ȡ����Z������
//////}






///////***************************************************************/
//////// MPU�ڲ�i2c д��
////////I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
////////I2C_SLVx_REG:   reg
////////I2C_SLVx_Data out:  value
///////***************************************************************/
//////void i2c_Mag_write(uint8_t reg,uint8_t value)
//////{
//////	uint16_t j = 500;
//////	
//////	//MPU9250_Write_Reg(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR);//���ô����Ƶ�ַ,mode: write
//////	//MPU9250_Write_Reg(I2C_SLV0_REG ,reg);//set reg addr
//////	//MPU9250_Write_Reg(I2C_SLV0_DO ,value);//send value	
//////	
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_ADDR ,		MPU9250_AK8963_ADDR);//���ô����Ƶ�ַ,mode: write
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_REG ,		reg	);
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_DO ,			value	);
//////	
//////	while(j--);//�˴���ΪMPU�ڲ�I2C��ȡ�ٶȽ�������ʱ�ȴ��ڲ�д���
//////}
///////***************************************************************/
//////// MPU�ڲ�i2c ��ȡ
////////I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
////////I2C_SLVx_REG:   reg
////////return value:   EXT_SENS_DATA_00 register value
///////***************************************************************/
//////uint8_t i2c_Mag_read(uint8_t reg)
//////{
//////	uint16_t 	j = 5000;
//////	//MPU9250_Write_Reg(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR|0x80); //���ô����Ƶ�ַ��mode��read
//////	//MPU9250_Write_Reg(I2C_SLV0_REG ,reg);// set reg addr
//////	//MPU9250_Write_Reg(I2C_SLV0_DO ,0xff);//read
//////	
//////	
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_ADDR ,	MPU9250_AK8963_ADDR | 0x80); //���ô����Ƶ�ַ��mode��read
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_REG ,	reg		);
//////	I2C_Write_One_Byte(MPU9250_CHIP_ADRESS_X68,	I2C_SLV0_DO ,		0xff	);
//////	
//////	while(j--);//�˴���ΪMPU�ڲ�I2C��ȡ�ٶȽ�����������ʱ�ȴ��ڲ���ȡ���
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
	
	I2C_SendByte(0x6B);   	//�������ģʽ	
			if(I2C_WaitAck())		//�ȴ�Ӧ��
					{IIC_Stop();	 return 0;}		

	*(buf++)=I2C_ReceiveByte(1);
	*(buf++)=I2C_ReceiveByte(1);
	*(buf++)=I2C_ReceiveByte(1);
	*(buf++)=I2C_ReceiveByte(0);

	IIC_Stop();			//����һ��ֹͣ���� 
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




























