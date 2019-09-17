//===========================================================================================================
#include "stm32f10x_conf.h"
//===========================================================================================================





//================================================================================================================
//================================================================================================================




int gyromain(void);


void EPSON_Gyro(void);


extern uint8_t GGPM_BUF[5];
extern uint8_t GGPM01_Flag;
extern float GGPM01_Angle;



extern float Start_Aangle;


/////////////* Hardware registers needed by driver. */
////////////struct gyro_reg_s {
////////////    unsigned char who_am_i;
////////////    unsigned char rate_div;
////////////    unsigned char lpf;
////////////    unsigned char prod_id;
////////////    unsigned char user_ctrl;
////////////    unsigned char fifo_en;
////////////    unsigned char gyro_cfg;
////////////    unsigned char accel_cfg;

////////////    //unsigned char accel_cfg2;
////////////    //unsigned char lp_accel_odr;

////////////    unsigned char motion_thr;
////////////    unsigned char motion_dur;
////////////    unsigned char fifo_count_h;
////////////    unsigned char fifo_r_w;
////////////    unsigned char raw_gyro;
////////////    unsigned char raw_accel;
////////////    unsigned char temp;
////////////    unsigned char int_enable;
////////////    unsigned char dmp_int_status;
////////////    unsigned char int_status;

////////////    //unsigned char accel_intel;

////////////    unsigned char pwr_mgmt_1;
////////////    unsigned char pwr_mgmt_2;
////////////    unsigned char int_pin_cfg;
////////////    unsigned char mem_r_w;
////////////    unsigned char accel_offs;
////////////    unsigned char i2c_mst;
////////////    unsigned char bank_sel;
////////////    unsigned char mem_start_addr;
////////////    unsigned char prgm_start_h;
////////////// #if defined AK89xx_SECONDARY
//////////////     unsigned char s0_addr;
//////////////     unsigned char s0_reg;
//////////////     unsigned char s0_ctrl;
//////////////     unsigned char s1_addr;
//////////////     unsigned char s1_reg;
//////////////     unsigned char s1_ctrl;
//////////////     unsigned char s4_ctrl;
//////////////     unsigned char s0_do;
//////////////     unsigned char s1_do;
//////////////     unsigned char i2c_delay_ctrl;
//////////////     unsigned char raw_compass;
//////////////     /* The I2C_MST_VDDIO bit is in this register. */
//////////////     unsigned char yg_offs_tc;
////////////// #endif
////////////};


/////////////* Information specific to a particular device. */
////////////struct hw_s {
////////////    unsigned char addr;
////////////    unsigned short max_fifo;
////////////    unsigned char num_reg;
////////////    unsigned short temp_sens;
////////////    short temp_offset;
////////////    unsigned short bank_size;

////////////};
/////////////* Information for self-test. */
////////////struct test_s {
////////////    unsigned long gyro_sens;
////////////    unsigned long accel_sens;
////////////    unsigned char reg_rate_div;
////////////    unsigned char reg_lpf;
////////////    unsigned char reg_gyro_fsr;
////////////    unsigned char reg_accel_fsr;
////////////    unsigned short wait_ms;
////////////    unsigned char packet_thresh;
////////////    float min_dps;
////////////    float max_dps;
////////////    float max_gyro_var;
////////////    float min_g;
////////////    float max_g;
////////////    float max_accel_var;
////////////};



////////////extern const struct test_s test;
////////////extern const struct gyro_reg_s reg;
////////////extern const struct hw_s hw;






















//////////////#ifndef _MPU9250_H
//////////////#define _MPU9250_H
//////////////#include "sys.h"
////////////typedef struct
////////////	{
////////////		int16_t X;
////////////		int16_t Y;
////////////		int16_t Z;
////////////	}
////////////S_INT16_XYZ;
////////////extern S_INT16_XYZ MPU9250_ACC_LAST;
////////////extern S_INT16_XYZ MPU9250_GYRO_LAST;
////////////extern S_INT16_XYZ MPU9250_MAG_LAST;
////////////extern int16_t MPU9250_TEMP_LAST;
////////////	
////////////	
////////////	
////////////	// ����MPU9250�ڲ���ַ
/////////////*****************************************************************/
////////////#define INT_PIN_CFG                         0x37 //�ж�����
////////////#define I2C_MST_CTRL                        0x24
////////////#define I2C_MST_DELAY_CTRL                  0x67
//////////////--------------------i2c slv0-------------------------------//
////////////#define I2C_SLV0_ADDR                       0x25  
////////////#define I2C_SLV0_REG                        0x26
////////////#define I2C_SLV0_CTRL                       0x27 
////////////#define I2C_SLV0_DO                         0x63 //output reg
//////////////--------------------AK8963 reg addr------------------------//
////////////#define MPU9250_AK8963_ADDR                 0x0C  //AKM addr
////////////#define AK8963_WHOAMI_REG                   0x00  //AKM ID addr
////////////#define AK8963_WHOAMI_ID                    0x48  //ID
////////////#define AK8963_ST1_REG                      0x02  //Data Status1
////////////#define AK8963_ST2_REG                      0x09  //Data reading end register & check Magnetic sensor overflow occurred
////////////#define AK8963_ST1_DOR                      0x02
////////////#define AK8963_ST1_DRDY                     0x01 //Data Ready
////////////#define AK8963_ST2_BITM                     0x10
////////////#define AK8963_ST2_HOFL                     0x08 // Magnetic sensor overflow 
////////////#define AK8963_CNTL1_REG                    0x0A
////////////#define AK8963_CNTL2_REG                    0x0B
////////////#define AK8963_CNTL2_SRST                   0x01 //soft Reset
////////////#define AK8963_ASAX                         0x10 //X-axis sensitivity adjustment value 
////////////#define AK8963_ASAY                         0x11 //Y-axis sensitivity adjustment value
////////////#define AK8963_ASAZ                         0x12 //Z-axis sensitivity adjustment value
////////////	
////////////	
//////////////--------------------9axis  reg addr-----------------------//
//////////////�Ĵ�������
////////////#define SELF_TEST_X_GYRO		0X00
////////////#define SELF_TEST_Y_GYRO		0X01
////////////#define SELF_TEST_Z_GYRO		0X02
////////////	
////////////	
////////////#define MAG_XOUT_L		0x03
////////////#define MAG_XOUT_H		0x04
////////////#define MAG_YOUT_L		0x05
////////////#define MAG_YOUT_H		0x06
////////////#define MAG_ZOUT_L		0x07
////////////#define MAG_ZOUT_H		0x08
////////////	
////////////	
////////////#define SELF_TEST_X_ACCEL		0X0D
////////////#define SELF_TEST_Y_ACCEL		0X0E
////////////#define SELF_TEST_Z_ACCEL		0X0F

////////////#define XG_OFFSET_H					0X13
////////////#define XG_OFFSET_L					0X14
////////////#define YG_OFFSET_H					0X15
////////////#define YG_OFFSET_L					0X16
////////////#define ZG_OFFSET_H					0X17
////////////#define ZG_OFFSET_L					0X18

#define SMPLRT_DIV					0X19 //�����ǲ����� ����ֵΪ0X07  1000/(1+7)=125HZ
#define CONFIG							0X1A //��ͨ�˲���  ����ֵ0x06 5hz
#define GYRO_CONFIG					0X1B //�����ǲ�����Χ 0X18 ����2000��
#define ACCEL_CONFIG				0X1C //���ٶȼƲ�����Χ 0X18 ����16g
#define ACCEL_CONFIG_2			0X1D //���ٶȼƵ�ͨ�˲��� 0x06 5hz


////////////#define LP_ACCEL_ODR				0X1E
////////////#define WOM_THR							0X1F
////////////#define FIFO_EN							0X23

////////////#define ACCEL_XOUT_H				0X3B  //���ٶȼ��������
////////////#define ACCEL_XOUT_L				0X3C
////////////#define ACCEL_YOUT_H				0X3D
////////////#define ACCEL_YOUT_L				0X3E
////////////#define ACCEL_ZOUT_H				0X3F
////////////#define ACCEL_ZOUT_L				0X40

////////////#define TEMP_OUT_H					0X41  //�¶ȼ��������
////////////#define TEMP_OUT_L					0X42

////////////#define GYRO_XOUT_H					0X43  //�������������
////////////#define GYRO_XOUT_L					0X44
////////////#define GYRO_YOUT_H					0X45
////////////#define GYRO_YOUT_L					0X46
////////////#define GYRO_ZOUT_H					0X47
////////////#define GYRO_ZOUT_L					0X48





////////////#define EXT_SENS_DATA_00    0x49  //MPU9250 IIC���������ȡ���ؼĴ���00
////////////#define EXT_SENS_DATA_01    0x4a  //MPU9250 IIC���������ȡ���ؼĴ���01
////////////#define EXT_SENS_DATA_02    0x4b  //MPU9250 IIC���������ȡ���ؼĴ���02
////////////#define EXT_SENS_DATA_03    0x4c  //MPU9250 IIC���������ȡ���ؼĴ���03



#define PWR_MGMT_1					0x6B //��Դ����1 ����ֵΪ0x00
#define PWR_MGMT_2					0x6C //��Դ����2 ����ֵΪ0X00

#define WHO_AM_I						0x75 //����ID MPU9250Ĭ��IDΪ0X71
#define USER_CTRL						0x6A //�û����� ��Ϊ0X10ʱʹ��SPIģʽ







////////////// ����MPU9250�ڲ���ַ
/////////////*****************************************************************/
////////////#define INT_PIN_CFG                         0x37 //�ж�����
////////////#define I2C_MST_CTRL                        0x24
////////////#define I2C_MST_DELAY_CTRL                  0x67
//////////////--------------------i2c slv0-------------------------------//
////////////#define I2C_SLV0_ADDR                       0x25  
////////////#define I2C_SLV0_REG                        0x26
////////////#define I2C_SLV0_CTRL                       0x27 
////////////#define I2C_SLV0_DO                         0x63 //output reg







///************************SPI CS ********************************/
//#define MPU_9250_DISENABLE  GPIOA->BSRR = GPIO_Pin_0;//Ƭѡ
//#define MPU_9250_ENABLE  GPIOA->BRR = GPIO_Pin_0;




//void MPU9250_IOAndSPI_Init(void);
//u8 MPU9250_Write_Reg(u8 reg,u8 value);
//u8 MPU9250_Read_Reg(u8 reg);
u8 MPU9250_Init(void);
//void MPU9250_ReadValue(void);



////#define MPU9250_CS			PDout(3) //MPU9250Ƭѡ�ź�






























