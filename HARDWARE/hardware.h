#ifndef  __HARDWARE_H
#define  __HARDWARE_H
																  
////////////////////////ͷ�ļ�////////////////////////////////////
#include "stm32f10x.h"
#include "soft.h"

extern bool UV_FLAG;
extern bool pwrswitch_flag;
/*GPIO ʹ�����	
PA0		��ص�ѹ���			ADC1_CHN_0
PA1		����¶ȼ��			ADC1_CHN_1
PA2 	���������			ADC1_CHN_2
PA3 	�ұ�ɨ�������			ADC1_CHN_3		
PA4		��ؼ���			ADC1_CHN_4
PA5		�еؼ���			ADC1_CHN_5
PA6		�ҵؼ���			ADC1_CHN_6
PA7		��ǽ����			ADC1_CHN_7
PA8		��������//////���ɨ��ס���
PA9		USART1_TX
PA10	USART1_RX
PA11	NC
PA12	ɨ��������NC��
PA13	Download
PA14	Download
PA15	���м�⣨NC��

PB0		����ǽ����			ADC1_CHN_8
PB1		��ǽ����			ADC1_CHN_9
PB3		����оƬBUSY
PB4		����оƬDATA
PB5		����оƬCLK
PB6		����PWM
PB7		����PWM
PB8		����PWM
PB9		����PWM
PB10	DS1307_IIC_SCL(disbale)
PB11	DS1307_IIC_SDA(disable)
PB12	������ؼ�⣨NC��
PB13	������ؼ�⣨NC��
PB14	���ɨ��Դ����  
PB15	����PWM����

PC0		��ɨ�������			ADC1_CHN_10
PC1		���ȵ������			ADC1_CHN_11
PC2		���ֵ������			ADC1_CHN_12
PC3		���ֵ������			ADC1_CHN_13
PC4		����ǽ����			ADC1_CHN_14
PC5		��ǽ����			ADC1_CHN_15
PC6		���PWM
PC7		����PWM
PC8		�ұ�ɨ��Դ����
PC9		��ɨPWM
PC10	USART3_TX
PC11	USART3_RX
PC12	�س䰴�����
PC13	NC
PC14	LSE
PC15	LSE

PD0		��ص�ѹ������
PD1		HOLD_PWR_CTL(NC)
PD2		5V����
PD3		3.3V����
PD4		��ɫLED����
PD5		USART2_TX
PD6		USART2_RX
PD7		��ɫLED����
PD8		KEY3
PD9		�����Դ����
PD10	���������
PD11	��ǰ�������
PD12	���к������
PD13	���к������
PD14	��ǰ�������
PD15	�Һ�������

PE0		�����ٶȼ��
PE1		�����ٶȼ��
PE2		�����ּ�⣨NC��
PE3		��ɨ�ٶȼ��
PE4		����ٶȼ��
PE5		NC
PE6		����ײ
PE7		��ǰ��ײ��NC��
PE8		��ǰ��ײ��NC��
PE9		����ײ
PE10	��������
PE11	�����Դ����
PE12	DC�����
PE13	ǽ���Դ����
PE14	�ؼ��Դ����
PE15	���Ϳ��ؼ��

*/
//////////�������ٶȼ�ⷢ����ƹܽŶ���/////////////////////
#define		SPEED_SEND		GPIO_Pin_8		//�����ٶȼ���Դ����	       PD
//////////����������������ʹ�õ�io�Ŷ���///////////////////////
#define   	L_SPEED        	GPIO_Pin_0		//�����ٶȼ��       		PE
#define		EXTI_Line_LSPD	EXTI_Line0
#define		GPIO_LSPD		GPIOE
#define   	L_LIDI         	GPIO_Pin_12   	//������ؿ���   			PB
#define   	R_SPEED        	GPIO_Pin_1   	//�����ٶȼ�� 	  	    PE
#define		EXTI_Line_RSPD	EXTI_Line1
#define		GPIO_RSPD		GPIOE
#define   	R_LIDI         	GPIO_Pin_13   	//������ؿ���			PB
#define   	W_SPEED		 	GPIO_Pin_2		//�������ٶ�				PE
////////�����ǵؼ췢����������IO�ŵĶ���////////////////////////
#define  	EARTH_SEND      GPIO_Pin_14   	//�����     			PE 
#define		EARTH_SEND_GPIO	GPIOE
////////������ǽ�췢����������IO�ŵĶ���////////////////////////
#define  	WALL_SEND       GPIO_Pin_13   	//�����     			PE
#define		WALL_SEND_GPIO	GPIOE
/////////�����ǵ�س������Ŷ���////////////////////////////
#define  	CHARGE_DC   	GPIO_Pin_12   	//DC����     			PE
#define		GPIO_DC			GPIOE
#define		EXTI_Line_DC	EXTI_Line12	
#define  	CHARGE_SEAT 	GPIO_Pin_8   	//�س������� 			PA
#define		GPIO_SEAT		GPIOA
#define		EXTI_Line_SEAT	EXTI_Line8
#define		PWR_SWITCH_PIN	GPIO_Pin_15		//���Ϳ��ؼ��			PE	
///////�����Ǻ�����յĹܽŶ���/////////////////////////////// 
#define  	HW_POWER        GPIO_Pin_9   	//�����Դ�������    		PD //��
#define		GPIO_HWPWR		GPIOD
#define  	L_HW            GPIO_Pin_11   	//���������          	PD
#define	 	LM_HW			GPIO_Pin_12		//��ǰ����				PD
#define  	RM_HW           GPIO_Pin_13   	//�к�������     	    PD
#define  	R_HW            GPIO_Pin_14  	//�Һ�������     		PD	 
#define  	B_HW            GPIO_Pin_5  	//�����              	PD
#define		LB_HW			GPIO_Pin_10
#define		RB_HW			GPIO_Pin_15
#define	 	EXTI_Line_LMHW	EXTI_Line12
#define	 	EXTI_Line_LBHW	EXTI_Line10
#define	 	EXTI_Line_LHW	EXTI_Line11
#define	 	EXTI_Line_RHW   EXTI_Line14
#define	 	EXTI_Line_RMHW	EXTI_Line13
#define		EXTI_Line_RBHW	EXTI_Line15
////////��������ײ����ĹܽŶ���//////////////////////////////	
#define  	L_BUMP_PIN      GPIO_Pin_6  	//����ײ����     	    PE
#define  	LM_BUMP_PIN	 	GPIO_Pin_7  	//������ײ				PE
#define  	R_BUMP_PIN      GPIO_Pin_9  	//����ײ����     	    PE
#define  	RM_BUMP_PIN		GPIO_Pin_8		//������ײ				PE
#define		EXTI_Line_LBUMP	EXTI_Line6
#define		EXTI_Line_RBUMP	EXTI_Line9
#define		EXTI_Line_RMBUMP	EXTI_Line8
#define		EXTI_Line_LMBUMP	EXTI_Line9
///////////�����ǰ�������Ĺ̶ܽ���////////////////////////////
#define  	KEY_1			GPIO_Pin_8		//�������� 				PD
#define  	KEY_2			GPIO_Pin_10		//��ͣ�������� 			PE
#define	 	GPIO_KEY2		GPIOE
#define  	KEY_3			GPIO_Pin_12		//�س䰴������ 			PC
#define  	GPIO_KEY3       GPIOC
#define  	HOLDPWR_PIN		GPIO_Pin_1		//HOLD_PWR CTL		PD
#define  	HOLDPWR_ON		{GPIOD->BSRR|=1<<1;}
#define  	HOLDPWR_OFF    	{GPIOD->BRR|=1<<1;}
//////////���PWM�ܽŶ���/////////////////////////////////////		
#define	 	CHG_PWM			GPIO_Pin_6	  	//���PWM PC
//////////��ɨ�ܽŶ���////////////////////////////////////////
#define  	MIDBRUSH_PWM	GPIO_Pin_9	 	//��ɨPWM����PC	NEW	
#define	 	BRUSH2SUCK_PIN	GPIO_Pin_12	  	//��ˢ�����ж����� (0:���ڣ�1����ˢ)qz add 20190128
#define	 	BRUSH2SUCK_GPIO	GPIOA
#define		MB_SPEED_PIN	GPIO_Pin_3
#define		GPIO_MBSPD		GPIOE
///////��ɨ�ܽŶ���///////////////////////////////////////////
#define	 	SIDEBRUSH_PWM	GPIO_Pin_8	 	//��ɨPWM����			PC	NEW
#define		LSB_PWR_PIN		GPIO_Pin_14		//���ɨ��Դ����			PB
#define		LSB_PWR_1		{GPIOB->BSRR|=1<<14;}
#define		LSB_PWR_0		{GPIOB->BRR|=1<<14;}
#define		RSB_PWR_PIN		GPIO_Pin_8		//�ұ�ɨ��Դ����			PC
///////����ܽŶ���///////////////////////////////////////////
#define	 	FAN_PWM			GPIO_Pin_7	 	//���PWM����			PC
#define		FAN_SPEED_PIN	GPIO_Pin_4		//�����������			PE
#define		FAN_PWR_CTL		GPIO_Pin_11
#define		FAN_PWR_CTL_1	{GPIO_FANSPEED->BSRR|=1<<11;}
#define		FAN_PWR_CTL_0	{GPIO_FANSPEED->BRR|=1<<11;}
#define		GPIO_FANSPEED	GPIOE
///////////���йܽŶ���///////////////////////////////////////////
#define  	DUST_BOX		GPIO_Pin_15   	//���д��ڼ����������       PA     NEW		
//////���ֿ��ƹܽ�////////////////////////////////////////////
#define  	DL_FIN			GPIO_Pin_8    // ������ǰ����  PB//��
#define  	DL_RIN			GPIO_Pin_9    // ����������  PB
#define  	DR_FIN			GPIO_Pin_7    // ������ǰ����  PB//��
#define  	DR_RIN			GPIO_Pin_6    // ����������  PB
///////////////////�������Դ����hzh//////////////////////////////////
#define  	NAV_POW      	GPIO_Pin_0    //navigation������  PD0  hzh 
#define  	NAV_CS       	GPIO_Pin_14   //PE
#define  	NAV_IRQ      	GPIO_Pin_15   //PE
///////////////////С��ģ���Դ����hzh//////////////////////////////////
#define  	BIRD_POW     	GPIO_Pin_1	
#define	 	GYRO_RST_PIN	GPIO_Pin_10
#define	 	GYRO_RST_1		{GPIOC->BSRR|=1<<10;}
#define	 	GYRO_RST_0		{GPIOC->BRR|=1<<10;}
//////////////////LED�ƿ�������///////////////////////////	
#define  	LED_GREEN		GPIO_Pin_7		//PD
#define	 	LED_GREEN_ON	{GPIOD->BSRR|=1<<7;}
#define	 	LED_GREEN_OFF	{GPIOD->BRR|=1<<7;}	
#define  	LED_RED			GPIO_Pin_4		//PD
#define  	LED_RED_ON		{GPIOD->BSRR|=1<<4;}
#define  	LED_RED_OFF     {GPIOD->BRR|=1<<4;}

#define	 	SLAMDOWN_PIN	GPIO_Pin_15
//////////////
#define  	BAT_CHECK_PIN		GPIO_Pin_0		//PD
#define	 	BAT_CHECK_1		{GPIOD->BSRR|=1<<0;}
#define  	BAT_CHECK_0		{GPIOD->BRR|=1<<0;}
/////////////����PWM���ƹܽ�///////////////////////////////
#define  	RING_PWM_CTL_PIN	GPIO_Pin_15		//PB
#define		READ_RING_PWM_CTL	GPIOB->IDR&=0X8000
#define	 	RING_PWM_CTL_1		{GPIOB->BSRR|=1<<15;}
#define  	RING_PWM_CTL_0		{GPIOB->BRR|=1<<15;}
////////////�ߵ�������������/////////////////////////////////
#define     GYRO_CAL_PIN	GPIO_Pin_11			//PA
#define     GYRO_CAL_PIN_1	{GPIOA->BSRR|=1<<11;}
#define		GYRO_CAL_PIN_0	{GPIOA->BRR|=1<<11;}
////////////��Դ����///////////////////////////////////////
#define		PWR3V3_PIN		GPIO_Pin_3
#define		PWR3V3_ON		{GPIOD->BSRR|=1<<3;}
#define		PWR3V3_OFF		{GPIOD->BRR|=1<<3;}
#define		PWR5V_PIN		GPIO_Pin_2
#define		PWR5V_ON		{GPIOD->BSRR|=1<<2;}
#define		PWR5V_OFF		{GPIOD->BRR|=1<<2;}
////////////////////////ȫ�ֱ���//////////////////////////////////
extern 	bool gyro_cal_flag;

void Init_Hardware (void);									   
void Set_Beep(void);			
void Set_Dust_Motor(void);
void Set_Earth_Send(void);
void Set_HW_Power_ON(void); 
void Set_L_BackOnOff(void);
void Set_L_FrontOnOff(void); 
void Set_LCD_BG(void);		 
void Set_LCD_Clk(void);	   
void Set_LCD_Data(void); 									   
void Set_LCD_Power(void); 										 
void Set_LCD_Stb(void);	
void Set_M_FPMOS(void);	
void Set_M_ZPMOS(void);
void Set_M_ZNMOS(void);
void Set_M_FNMOS(void);  
void Set_R_BackOnOff(void);
void Set_R_FrontOnOff(void);
void Set_S_Motor(void);	
void Set_Speed_Send(void);
void Set_UV(void);
void Reset_UV(void);
void Set_Wall_Send(void);
void Set_DustStatus_Send(void);

void Reset_Beep(void);			
void Reset_Dust_Motor(void);
void Reset_Earth_Send(void);
void Reset_HW_Power_OFF(void); 
void Reset_L_BackOnOff(void);
void Reset_L_FrontOnOff(void); 
void Reset_LCD_BG(void);		 
void Reset_LCD_Clk(void);	   
void Reset_LCD_Data(void); 									   
void Reset_LCD_Power(void); 										 
void Reset_LCD_Stb(void);	
void Reset_M_FPMOS(void);	
void Reset_M_ZPMOS(void);	
void Reset_M_FNMOS(void);	
void Reset_M_ZNMOS(void);	  
void Reset_R_BackOnOff(void);
void Reset_R_FrontOnOff(void);
void Reset_S_Motor(void);	
void Reset_Speed_Send(void);
void Reset_UV(void);		   								  
void Reset_Wall_Send(void);
void Reset_DustStatus_Send(void);

//hzh
void TurnOn_BIRD(void);
void TurnOff_BIRD(void);
void TurnOn_NAV(void);
void TurnOff_NAV(void);

void BIRD_PE14_1(void);
void BIRD_PE14_0(void);


u8 Read_Charge_Dc(void);
u8 Read_Charge_Seat(void);
u8 Read_Front_Wheel(void);
u8 Read_Key1(void);
u8 Read_Key2(void);
u8 Read_Key3(void);
u8 Read_L_Bump(void);
u8 Read_L_HW(void);
u8 Read_L_LIDI(void);
u8 Read_L_Speed(void);
u8 Read_M_HW(void);	
u8 Read_B_HW(void);
u8 Read_R_Bump(void);
u8 Read_LM_Bump(void);//HZH
u8 Read_RM_Bump(void);
u8 Read_R_HW(void);
u8 Read_R_LIDI(void);
u8 Read_R_Speed(void);
u8 Read_S_Current(void);
u8 Read_Up_HW(void);
u8 Read_LCD_Data(void);
u8 Read_CHG_ING(void);
u8 Read_CHG_DONE(void);
u8 Read_Dust_Box(void);
u8 Read_SB_Status(void);
u8 Read_Dust_Status(void);
u8 Read_LSB_Status(void);
u8 Read_RSB_Status(void); 	
u8 Read_RM_HW(void);
u8 Read_LM_HW(void);

void  Init_System(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
u8 Read_Last_Is_Dead(void);
void Gyro_Rst(void);
u8 Read_PwrSwitch_Status(void);
void Enable_RingPWMCtrl(void);
void Disable_RingPWMCtrl(void);


///////////////////////ȫ�ֺ���////////////////////////////////////
#endif

