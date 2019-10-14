#ifndef  __HARDWARE_H
#define  __HARDWARE_H
																  
////////////////////////头文件////////////////////////////////////
#include "stm32f10x.h"
#include "soft.h"

extern bool UV_FLAG;
extern bool pwrswitch_flag;
/*GPIO 使用情况	
PA0		电池电压检测			ADC1_CHN_0
PA1		电池温度检测			ADC1_CHN_1
PA2 	充电电流检测			ADC1_CHN_2
PA3 	右边扫电流检测			ADC1_CHN_3		
PA4		左地检检测			ADC1_CHN_4
PA5		中地检检测			ADC1_CHN_5
PA6		右地检检测			ADC1_CHN_6
PA7		左墙检检测			ADC1_CHN_7
PA8		充电座检测//////左边扫卡住检测
PA9		USART1_TX
PA10	USART1_RX
PA11	NC
PA12	扫吸互换（NC）
PA13	Download
PA14	Download
PA15	尘盒检测（NC）

PB0		右中墙检检测			ADC1_CHN_8
PB1		右墙检检测			ADC1_CHN_9
PB3		语音芯片BUSY
PB4		语音芯片DATA
PB5		语言芯片CLK
PB6		右轮PWM
PB7		右轮PWM
PB8		左轮PWM
PB9		左轮PWM
PB10	DS1307_IIC_SCL(disbale)
PB11	DS1307_IIC_SDA(disable)
PB12	左轮离地检测（NC）
PB13	右轮离地检测（NC）
PB14	左边扫电源控制  
PB15	轮子PWM控制

PC0		中扫电流检测			ADC1_CHN_10
PC1		风扇电流检测			ADC1_CHN_11
PC2		左轮电流检测			ADC1_CHN_12
PC3		右轮电流检测			ADC1_CHN_13
PC4		左中墙检检测			ADC1_CHN_14
PC5		中墙检检测			ADC1_CHN_15
PC6		充电PWM
PC7		风扇PWM
PC8		右边扫电源控制
PC9		中扫PWM
PC10	USART3_TX
PC11	USART3_RX
PC12	回充按键检测
PC13	NC
PC14	LSE
PC15	LSE

PD0		电池电压检测控制
PD1		HOLD_PWR_CTL(NC)
PD2		5V控制
PD3		3.3V控制
PD4		红色LED控制
PD5		USART2_TX
PD6		USART2_RX
PD7		绿色LED控制
PD8		KEY3
PD9		红外电源控制
PD10	左后红外接收
PD11	左前红外接收
PD12	左中红外接收
PD13	右中红外接收
PD14	右前红外接收
PD15	右后红外接收

PE0		左轮速度检测
PE1		右轮速度检测
PE2		万向轮检测（NC）
PE3		中扫速度检测
PE4		风机速度检测
PE5		NC
PE6		左碰撞
PE7		左前碰撞（NC）
PE8		右前碰撞（NC）
PE9		右碰撞
PE10	启动按键
PE11	风机电源控制
PE12	DC充电检测
PE13	墙检电源控制
PE14	地检电源控制
PE15	船型开关检测

*/
//////////下面是速度检测发射控制管脚定义/////////////////////
#define		SPEED_SEND		GPIO_Pin_8		//轮子速度检测电源控制	       PD
//////////下面是左右轮驱动使用的io脚定义///////////////////////
#define   	L_SPEED        	GPIO_Pin_0		//左轮速度检测       		PE
#define		EXTI_Line_LSPD	EXTI_Line0
#define		GPIO_LSPD		GPIOE
#define   	L_LIDI         	GPIO_Pin_12   	//左轮离地开关   			PB
#define   	R_SPEED        	GPIO_Pin_1   	//右轮速度检测 	  	    PE
#define		EXTI_Line_RSPD	EXTI_Line1
#define		GPIO_RSPD		GPIOE
#define   	R_LIDI         	GPIO_Pin_13   	//右轮离地开关			PB
#define   	W_SPEED		 	GPIO_Pin_2		//万向轮速度				PE
////////下面是地检发射管输出驱动IO脚的定义////////////////////////
#define  	EARTH_SEND      GPIO_Pin_14   	//发射灯     			PE 
#define		EARTH_SEND_GPIO	GPIOE
////////下面是墙检发射管输出驱动IO脚的定义////////////////////////
#define  	WALL_SEND       GPIO_Pin_13   	//发射灯     			PE
#define		WALL_SEND_GPIO	GPIOE
/////////下面是电池充电输入脚定义////////////////////////////
#define  	CHARGE_DC   	GPIO_Pin_12   	//DC输入     			PE
#define		GPIO_DC			GPIOE
#define		EXTI_Line_DC	EXTI_Line12	
#define  	CHARGE_SEAT 	GPIO_Pin_8   	//回充座输入 			PA
#define		GPIO_SEAT		GPIOA
#define		EXTI_Line_SEAT	EXTI_Line8
#define		PWR_SWITCH_PIN	GPIO_Pin_15		//船型开关检测			PE	
///////下面是红外接收的管脚定义/////////////////////////////// 
#define  	HW_POWER        GPIO_Pin_9   	//红外电源输出控制    		PD //☆
#define		GPIO_HWPWR		GPIOD
#define  	L_HW            GPIO_Pin_11   	//左红外输入          	PD
#define	 	LM_HW			GPIO_Pin_12		//左前红外				PD
#define  	RM_HW           GPIO_Pin_13   	//中红外输入     	    PD
#define  	R_HW            GPIO_Pin_14  	//右红外输入     		PD	 
#define  	B_HW            GPIO_Pin_5  	//后红外              	PD
#define		LB_HW			GPIO_Pin_10
#define		RB_HW			GPIO_Pin_15
#define	 	EXTI_Line_LMHW	EXTI_Line12
#define	 	EXTI_Line_LBHW	EXTI_Line10
#define	 	EXTI_Line_LHW	EXTI_Line11
#define	 	EXTI_Line_RHW   EXTI_Line14
#define	 	EXTI_Line_RMHW	EXTI_Line13
#define		EXTI_Line_RBHW	EXTI_Line15
////////下面是碰撞输入的管脚定义//////////////////////////////	
#define  	L_BUMP_PIN      GPIO_Pin_6  	//左碰撞输入     	    PE
#define  	LM_BUMP_PIN	 	GPIO_Pin_7  	//左中碰撞				PE
#define  	R_BUMP_PIN      GPIO_Pin_9  	//右碰撞输入     	    PE
#define  	RM_BUMP_PIN		GPIO_Pin_8		//右中碰撞				PE
#define		EXTI_Line_LBUMP	EXTI_Line6
#define		EXTI_Line_RBUMP	EXTI_Line9
#define		EXTI_Line_RMBUMP	EXTI_Line8
#define		EXTI_Line_LMBUMP	EXTI_Line9
///////////下面是按键输入的管教定义////////////////////////////
#define  	KEY_1			GPIO_Pin_8		//按键输入 				PD
#define  	KEY_2			GPIO_Pin_10		//启停按键输入 			PE
#define	 	GPIO_KEY2		GPIOE
#define  	KEY_3			GPIO_Pin_12		//回充按键输入 			PC
#define  	GPIO_KEY3       GPIOC
#define  	HOLDPWR_PIN		GPIO_Pin_1		//HOLD_PWR CTL		PD
#define  	HOLDPWR_ON		{GPIOD->BSRR|=1<<1;}
#define  	HOLDPWR_OFF    	{GPIOD->BRR|=1<<1;}
//////////充电PWM管脚定义/////////////////////////////////////		
#define	 	CHG_PWM			GPIO_Pin_6	  	//充电PWM PC
//////////中扫管脚定义////////////////////////////////////////
#define  	MIDBRUSH_PWM	GPIO_Pin_9	 	//中扫PWM引脚PC	NEW	
#define	 	BRUSH2SUCK_PIN	GPIO_Pin_12	  	//滚刷吸口判断输入 (0:吸口，1：滚刷)qz add 20190128
#define	 	BRUSH2SUCK_GPIO	GPIOA
#define		MB_SPEED_PIN	GPIO_Pin_3
#define		GPIO_MBSPD		GPIOE
///////边扫管脚定义///////////////////////////////////////////
#define	 	SIDEBRUSH_PWM	GPIO_Pin_8	 	//边扫PWM引脚			PC	NEW
#define		LSB_PWR_PIN		GPIO_Pin_14		//左边扫电源控制			PB
#define		LSB_PWR_1		{GPIOB->BSRR|=1<<14;}
#define		LSB_PWR_0		{GPIOB->BRR|=1<<14;}
#define		RSB_PWR_PIN		GPIO_Pin_8		//右边扫电源控制			PC
///////风机管脚定义///////////////////////////////////////////
#define	 	FAN_PWM			GPIO_Pin_7	 	//风机PWM引脚			PC
#define		FAN_SPEED_PIN	GPIO_Pin_4		//风机反馈引脚			PE
#define		FAN_PWR_CTL		GPIO_Pin_11
#define		FAN_PWR_CTL_1	{GPIO_FANSPEED->BSRR|=1<<11;}
#define		FAN_PWR_CTL_0	{GPIO_FANSPEED->BRR|=1<<11;}
#define		GPIO_FANSPEED	GPIOE
///////////尘盒管脚定义///////////////////////////////////////////
#define  	DUST_BOX		GPIO_Pin_15   	//尘盒存在检测输入引脚       PA     NEW		
//////车轮控制管脚////////////////////////////////////////////
#define  	DL_FIN			GPIO_Pin_8    // 左轮向前控制  PB//☆
#define  	DL_RIN			GPIO_Pin_9    // 左轮向后控制  PB
#define  	DR_FIN			GPIO_Pin_7    // 右轮向前控制  PB//☆
#define  	DR_RIN			GPIO_Pin_6    // 右轮向后控制  PB
///////////////////导航板电源开关hzh//////////////////////////////////
#define  	NAV_POW      	GPIO_Pin_0    //navigation导航板  PD0  hzh 
#define  	NAV_CS       	GPIO_Pin_14   //PE
#define  	NAV_IRQ      	GPIO_Pin_15   //PE
///////////////////小鸟模块电源开关hzh//////////////////////////////////
#define  	BIRD_POW     	GPIO_Pin_1	
#define	 	GYRO_RST_PIN	GPIO_Pin_10
#define	 	GYRO_RST_1		{GPIOC->BSRR|=1<<10;}
#define	 	GYRO_RST_0		{GPIOC->BRR|=1<<10;}
//////////////////LED灯控制引脚///////////////////////////	
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
/////////////轮子PWM控制管脚///////////////////////////////
#define  	RING_PWM_CTL_PIN	GPIO_Pin_15		//PB
#define		READ_RING_PWM_CTL	GPIOB->IDR&=0X8000
#define	 	RING_PWM_CTL_1		{GPIOB->BSRR|=1<<15;}
#define  	RING_PWM_CTL_0		{GPIOB->BRR|=1<<15;}
////////////惯导矫正控制引脚/////////////////////////////////
#define     GYRO_CAL_PIN	GPIO_Pin_11			//PA
#define     GYRO_CAL_PIN_1	{GPIOA->BSRR|=1<<11;}
#define		GYRO_CAL_PIN_0	{GPIOA->BRR|=1<<11;}
////////////电源控制///////////////////////////////////////
#define		PWR3V3_PIN		GPIO_Pin_3
#define		PWR3V3_ON		{GPIOD->BSRR|=1<<3;}
#define		PWR3V3_OFF		{GPIOD->BRR|=1<<3;}
#define		PWR5V_PIN		GPIO_Pin_2
#define		PWR5V_ON		{GPIOD->BSRR|=1<<2;}
#define		PWR5V_OFF		{GPIOD->BRR|=1<<2;}
////////////////////////全局变量//////////////////////////////////
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


///////////////////////全局函数////////////////////////////////////
#endif

