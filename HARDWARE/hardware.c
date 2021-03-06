////////////////////////头文件////////////////////////////////////
#include <stm32f10x.h>
#include "hardware.h"
bool UV_FLAG=false;
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================



////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////

///////////////////////私有函数////////////////////////////////////
///////////////////////函数实体////////////////////////////////////





//===============================================================================================================
//===============================================================================================================
/*******************************************************************************
函数名：Init_System
功能：初始化整个系统
输入：无
输出：无
返回：无
*******************************************************************************/
void  Init_System(void)
{
	uint32 	time;
  
	RCC_Configuration();				//初始化系统的晶振，如有移植需要修改
	 Init_8M_out ();
	delay_init(64);
	NVIC_Configuration();				//初始化系统的中断，如有移植需要修改
	init_time_2();        				//Timer2 10K中断  计数器，用于系统的基本心跳
#ifdef FAN_SPD_TIM1
	Init_Time_1();
#endif
	Init_Hardware();    				//初始化系统的硬件，如有移植需要修改
#ifdef FAN_SPD_CTL
	Init_FanSpd_Interrupt();
#endif
	init_ad();							//初始化AD	  
	URAT1_init(115200);

#ifdef   NEW_Q55_BOARD_1113   
	USART2_Init();
#ifdef TUYA_WIFI
	USART2_DMA_TX_Init(USART1_TX_SIZE);
#ifdef DMA_IRQ 
	USART2_DMA_RX_Init(USART1_RX_SIZE);
#endif
	wifi_protocol_init();
#else	//TUYA_WIFI			
	USART2_DMA_TX_Init(16);
	USART2_DMA_RX_Init(128);	
#endif		
	URAT1_init(115200);
	DMA_Uart1Tx_Config(USART1_TX_SIZE);
	DMA_Uart1Rx_Config(USART1_RX_SIZE);
		 
#else //NEW_Q55_BOARD_1113  
    URAT1_init(115200);
#ifdef TUYA_WIFI
	DMA_Uart1Tx_Config(USART1_TX_SIZE);
#ifdef DMA_IRQ 
	DMA_Uart1Rx_Config(USART1_RX_SIZE);
#endif
	wifi_protocol_init();
#else //TUYA_WIFI			
	DMA_Uart1Tx_Config(16);
	DMA_Uart1Rx_Config(128);
#endif	
	USART2_Init();
	USART2_DMA_TX_Init(16);
	USART2_DMA_RX_Init(USART2_RX_SIZE);
#endif// NEW_Q55_BOARD_1113  


	USART3_Init();
	USART3_DMA_TX_Init(USART3_TX_SIZE);
	USART3_DMA_RX_Init(USART3_RX_SIZE);
#ifdef VOICE_LIST
	Init_Voice();						//初始化语音芯片，包括音量设置
#endif
	Init_WatchDog();					//初始化看门狗
//	Reset_Bat_Data();
	Init_BKP_Bat_Rtc();
//	init_rtc();							//初始化RTC,BKP数据读取,建立闹钟中断
//	Battery_Data_Init();
//	Init_PWM();							//初始化PWM，PWM用于电机驱动、风扇驱动、充电驱动
	Init_Ring_Pwm(PWM_RING_MAX,PWM_RING_PRESCALER);
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	init_hwincept();					//初始化红外接收程序
//	Init_Remote_Info();					//qz add 20189817初始化遥控信息
           DelayMs(2);
	init_power();
	time = giv_sys_time;
	while((giv_sys_time - time) < 400){judge_charge();};
		Init_Bump_Interrupt();				//初始化碰撞中断
	init_key();							//初始化按键类信息，如：按键、开关等
	Init_Ring();
	init_wallearth();	
	Init_Gyro_Data();
	Gyro_Rst();							//qz add 20180927
#ifdef PREEN_SWEEP
	ReadPreengage();					//获取预约
#endif

	Init_Mode();
	Init_Check_Status();				//初始化异常检测
#ifdef REMOTE
	Init_Remote_Info();
#endif
}
//===============================================================================================================
//===============================================================================================================

  


//===============================================================================================================
//===============================================================================================================
void Init_8M_out (void)
{
#ifdef OUT_8MHZ
	GPIO_InitTypeDef GPIO_InitStructure; //管脚结构
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB 
                     | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD 
					           | RCC_APB2Periph_GPIOE , ENABLE);

  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;		//左边扫保护引脚，暂时按照GPIO读取
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	#ifdef USE_HSE	
	RCC_MCOConfig(RCC_MCO_HSE);
	#else
	#ifdef            RTC_8M_CORR   
	RCC_MCOConfig(RCC_MCO_HSI);
	#else
	RCC_MCOConfig(RCC_MCO_HSI);	
    #endif
	#endif		
#endif
}
void Init_Hardware (void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //管脚结构



	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB 
                     | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD 
					           | RCC_APB2Periph_GPIOE , ENABLE);
/********************************************/
//
//
//输入输入输入输入输入输入输入输入输入输入输入
//
//
/********************************************/
/**************************/
//
//浮空浮空浮空浮空浮空浮空
//
/**************************/
	 /////////////////////////PA口浮空输入：
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 
  #ifdef   NEW_Q55_BOARD_1113 
			GPIO_InitStructure.GPIO_Pin=VOICE_BUSY_PIN;   //左边扫保护引脚，暂时按照GPIO读取
  #else
		GPIO_InitStructure.GPIO_Pin=CHARGE_SEAT|VOICE_BUSY_PIN; 	//左边扫保护引脚，暂时按照GPIO读取
#endif

	GPIO_Init(GPIOA,&GPIO_InitStructure);
	///////////////////////PB浮空输入
  	///////////////////////PC浮空输入
  	GPIO_InitStructure.GPIO_Pin=KEY_3;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);  
  	///////////////////////PD浮空输入
  #ifdef   NEW_Q55_BOARD_1113 
    	GPIO_InitStructure.GPIO_Pin =L_HW|R_HW|LM_HW|LB_HW|RM_HW|RB_HW|CHARGE_SEAT ;
  #else
  	GPIO_InitStructure.GPIO_Pin =L_HW|R_HW|LM_HW|LB_HW|RM_HW|RB_HW;
  #endif	
  	GPIO_Init(GPIOD, &GPIO_InitStructure);  
  ///////////////////////PE浮空输入 
  	GPIO_InitStructure.GPIO_Pin =CHARGE_DC|R_SPEED|L_SPEED|PWR_SWITCH_PIN|KEY_2|FAN_SPEED_PIN|MB_SPEED_PIN;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
/*
#ifdef OUT_8MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;		//左边扫保护引脚，暂时按照GPIO读取
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	#ifdef USE_HSE	
	RCC_MCOConfig(RCC_MCO_HSE);
	#else
	#ifdef            RTC_8M_CORR   
	RCC_MCOConfig(RCC_MCO_HSI);
	#else
	RCC_MCOConfig(RCC_MCO_HSI);	
    #endif
	#endif		
#endif*/
/**************************/
//
//上拉上拉上拉上拉上拉上拉
//
/**************************/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 
//	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=BRUSH2SUCK_PIN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
  /////////////////////////PD口上拉输入：
// 	GPIO_Init(GPIOD, &GPIO_InitStructure);

/********************************************/
//
//
//输出输出输出输出输出输出输出输出输出输出输出
//
//
/********************************************/
/**************************/
//
//推挽推挽推挽推挽推挽推挽
//
/***************************/
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	//////////////////////////PA口推挽输出///////////////////
 #ifndef   NEW_Q55_BOARD_1113 	
	GPIO_InitStructure.GPIO_Pin = GYRO_CAL_PIN;
         GPIO_Init(GPIOA,&GPIO_InitStructure);
#endif		
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	/////////////////////////PB口推挽输出///////////////////
  	GPIO_InitStructure.GPIO_Pin = VOICE_CLK|VOICE_DATA|RING_PWM_CTL_PIN|LSB_PWR_PIN; 
    GPIO_Init(GPIOB,&GPIO_InitStructure);
	/////////////////////////PC口推挽输出///////////////////
#ifdef GYRO_PWM_RUN_STATE
    GPIO_InitStructure.GPIO_Pin=GYRO_RST_PIN |RSB_PWR_PIN| GYRO_PWM_RUN_PIN;
#else
 #ifdef 	  RTC_8M_CORR 
			 GPIO_InitStructure.GPIO_Pin=RSB_PWR_PIN;
 #else
		  GPIO_InitStructure.GPIO_Pin=GYRO_RST_PIN|RSB_PWR_PIN;
#endif
#endif	
    GPIO_Init(GPIOC,&GPIO_InitStructure);
	#ifdef   NEW_Q55_BOARD_1113 
	GPIO_InitStructure.GPIO_Pin=HOLDPWR_PIN|BAT_CHECK_PIN|PWR3V3_PIN|PWR5V_PIN|LED_RED|LED_GREEN|GYRO_CAL_PIN;
	#else
	GPIO_InitStructure.GPIO_Pin=HOLDPWR_PIN|BAT_CHECK_PIN|PWR3V3_PIN|PWR5V_PIN|LED_RED|LED_GREEN|HW_POWER;
         #endif	
    GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=WALL_SEND|EARTH_SEND|FAN_PWR_CTL;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	PWR3V3_ON
	PWR5V_ON
#ifdef  NEW_VOICE_IC
	V_CLK_1;				//拉高VOICE_CLK
	//V_DAT_0;				//拉高VOICE_DATA
	V_DAT_1;
#else
	V_CLK_1;				//拉高VOICE_CLK
	V_DAT_1;				//拉高VOICE_DATA
#endif
	GYRO_CAL_PIN_1;			//拉高惯导校准
	BAT_CHECK_1;			//拉高电池电压检测
	Enable_RingPWMCtrl();
	Open_Ring_Cnt();
#ifdef 		GYRO_PWM_RUN_STATE
            GYRO_PWM_RUN_1;
#endif	
/**************************/
//
//开漏开漏开漏开漏开漏开漏开漏
//
/***************************/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  	///////////////GPIOA开漏输出/////////////
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
  	///////////////GPIOB开漏输出/////////////
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	///////////////GPIOD开漏输出/////////////
  	GPIO_InitStructure.GPIO_Pin=SPEED_SEND;
  	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_SetBits(GPIOD,SPEED_SEND);
	/////////////////GPIOE开漏输出////////////////
//	GPIO_SetBits(EARTH_SEND_GPIO,EARTH_SEND);
//	GPIO_SetBits(WALL_SEND_GPIO,WALL_SEND);
	LED_GREEN_OFF;
	LED_RED_OFF;

}
//======================================================================================== 







 
/***********************************************************
功能：置高
***********************************************************/
//void Set_S_Motor(void)
//{           
  //GPIO_SetBits(GPIOA, S_MOTOR_DRIVER);
  //GPIO_ResetBits(GPIOC, S_MOTOR_DRIVER_D);
//}
#ifdef UV
/***********************************************************
功能：置高
***********************************************************/
void Set_UV(void) //hzh
{           
  GPIO_SetBits(GPIOA, UV_ON);  
  UV_FLAG=true;
}

void Reset_UV(void)//hzh
{           
  GPIO_ResetBits(GPIOA, UV_ON);
  UV_FLAG=false;
}
#endif
/***********************************************************
功能：置高
***********************************************************/
void Set_Dust_Motor(void)
{           
//  GPIO_SetBits(GPIOC, DUST_MOTOR_DRIVER);

//GPIO_ResetBits(GPIOC, DUST_MOTOR_DRIVER_D);

}

/***********************************************************
功能：置高
***********************************************************/
void Set_Wall_Send(void)
{    
//	GPIO_ResetBits(GPIOA, WALL_SEND);
	GPIO_SetBits(WALL_SEND_GPIO,WALL_SEND);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_LCD_Stb(void)
{           
  GPIO_SetBits(GPIOC, LCD_STB);
  
}
/***********************************************************
功能：置高
***********************************************************/
void Set_LCD_Power(void)
{           
	GPIO_SetBits(GPIOD, LCD_POW_ON);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_LCD_Clk(void)
{           
  GPIO_SetBits(GPIOD, LCD_CLK);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_LCD_Data(void)
{           
  GPIO_SetBits(GPIOB, LCD_DATA);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_LCD_BG(void)
{           
 	GPIO_SetBits(GPIOC, LCD_BL_ON);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_Speed_Send(void)
{           
	GPIO_ResetBits(GPIOD, SPEED_SEND ); //拉低开始发送
	//GPIO_SetBits(GPIOD,SPEED_SEND);//QZ
}
/***********************************************************
功能：置高
***********************************************************/
void Set_Earth_Send(void)
{           
  GPIO_SetBits(EARTH_SEND_GPIO, EARTH_SEND);
  //GPIO_ResetBits(GPIOD, EARTH_SEND);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_L_FrontOnOff(void)
{           
//  GPIO_SetBits(GPIOE, L_FRONTONOFF);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_L_BackOnOff(void)
{           
//  GPIO_SetBits(GPIOE, L_BACKONOFF);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_R_FrontOnOff(void)
{           
//  GPIO_SetBits(GPIOE, R_FRONTONOFF);                            
}
/***********************************************************
功能：置高
***********************************************************/
void Set_R_BackOnOff(void)
{           
//  GPIO_SetBits(GPIOE, R_BACKONOFF);
}
/***********************************************************
功能：置高
***********************************************************/
void Set_HW_Power_ON(void)
{      
#ifndef	 NEW_Q55_BOARD_1113 
	
		GPIO_ResetBits(GPIO_HWPWR, HW_POWER);
#endif
}


/***********************************************************
功能：置高
***********************************************************/
void Reset_Dust_Motor(void)
{           
//  	GPIO_ResetBits(GPIOC, DUST_MOTOR_DRIVER);
//	GPIO_ResetBits(GPIOC, DUST_MOTOR_DRIVER_D);
}


/***********************************************************
功能：置高
***********************************************************/
void Reset_Wall_Send(void)
{     
  //GPIO_SetBits(GPIOA, WALL_SEND);
  GPIO_ResetBits(WALL_SEND_GPIO, WALL_SEND);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_LCD_Stb(void)
{           
  GPIO_ResetBits(GPIOC, LCD_STB);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_LCD_Power(void)
{           
	GPIO_ResetBits(GPIOD, LCD_POW_ON);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_LCD_Clk(void)
{           
  GPIO_ResetBits(GPIOD, LCD_CLK);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_LCD_Data(void)
{           
  GPIO_ResetBits(GPIOB, LCD_DATA);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_LCD_BG(void)
{           
	GPIO_SetBits(GPIOC, LCD_BL_ON);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_Speed_Send(void)
{           
	//GPIO_ResetBits(GPIOD,SPEED_SEND);//QZ
	GPIO_SetBits(GPIOD,SPEED_SEND);		//拉高停止发送
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_Earth_Send(void)
{      
	GPIO_ResetBits(EARTH_SEND_GPIO, EARTH_SEND);
	//GPIO_SetBits(EARTH_SEND_GPIO, EARTH_SEND);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_L_FrontOnOff(void)
{           
//  GPIO_ResetBits(GPIOE, L_FRONTONOFF);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_L_BackOnOff(void)
{           
//  GPIO_ResetBits(GPIOE, L_BACKONOFF);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_R_FrontOnOff(void)
{           
//  GPIO_ResetBits(GPIOE, R_FRONTONOFF);                            
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_R_BackOnOff(void)
{           
//  GPIO_ResetBits(GPIOE, R_BACKONOFF);
}
/***********************************************************
功能：置高
***********************************************************/
void Reset_HW_Power_OFF(void)
{ 
#ifndef	 NEW_Q55_BOARD_1113 
	
		GPIO_ResetBits(GPIO_HWPWR, HW_POWER);
#endif

}
//=============

/***********************************************************
功能：置高
***********************************************************/
void BIRD_PE14_1(void)
{     

}
/***********************************************************
功能：置低
***********************************************************/
void BIRD_PE14_0(void)
{      
  
}
/////2017-2-10

//hzh
void TurnOn_BIRD(void)
{  
  GPIO_SetBits(GPIOD, BIRD_POW);
}

void TurnOff_BIRD(void)
{
  GPIO_ResetBits(GPIOD, BIRD_POW);
}

void TurnOn_NAV(void)
{
  GPIO_SetBits(GPIOD, NAV_POW);
}

void TurnOff_NAV(void)
{
  GPIO_ResetBits(GPIOD, NAV_POW);
}

































/**********************************************************
读取IO状态
**********************************************************/
uint8 Read_Front_Wheel(void)
{
	return 0;
////////////return GPIO_ReadInputDataBit(GPIOB, FRONT_WHEEL)  ;
}
 


/**********************************************************
读取IO状态
**********************************************************/

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_Key1(void)
{
#if 1
	return (GPIO_ReadInputDataBit(GPIOD,KEY_1));
#else
return 0;
#endif

}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_Key2(void)
{	
#if 1
	return (GPIO_ReadInputDataBit(GPIO_KEY2,KEY_2));
#else
	return 0;
#endif
}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_Key3(void)
{		
#if 1
	return (GPIO_ReadInputDataBit(GPIO_KEY3,KEY_3));
#else
	return 0;
#endif
}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_L_Speed(void)
{
	return GPIO_ReadInputDataBit(GPIO_LSPD, L_SPEED)  ;
}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_L_LIDI(void)
{
#if 1 //shftemp	//QZ:原来为0

return GPIO_ReadInputDataBit(GPIOD, L_LIDI)  ;
#else
return 1;	//QZ:原来为0
#endif


}
 
/**********************************************************
读取IO状态
**********************************************************/
u8 Read_Charge_Dc(void)
{
return GPIO_ReadInputDataBit(GPIO_DC, CHARGE_DC)  ;
}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_Charge_Seat(void)
{
#ifndef OUT_8MHZ
	return GPIO_ReadInputDataBit(GPIO_SEAT, CHARGE_SEAT)  ;
#else

  #ifdef   NEW_Q55_BOARD_1113 
	  return GPIO_ReadInputDataBit(GPIO_SEAT, CHARGE_SEAT)	;
  #else 
	return 0;
 #endif   

#endif
}
 
/**********************************************************
读取IO状态
**********************************************************/
u8 Read_R_Speed(void)
{
	return GPIO_ReadInputDataBit(GPIO_RSPD, R_SPEED)  ;
}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_R_LIDI(void)
{
#if 1 //shftemp	//QZ:原来为0

return GPIO_ReadInputDataBit(GPIOB, R_LIDI)  ;
#else
return 1;	//QZ:原来为0
#endif

}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_L_HW(void)
{
	return GPIO_ReadInputDataBit(GPIO_HWPWR, L_HW)  ;
}

u8 Read_LB_HW(void)
{
	return GPIO_ReadInputDataBit(GPIO_HWPWR,LB_HW);
}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_RM_HW(void)
{
	return GPIO_ReadInputDataBit(GPIO_HWPWR, RM_HW)  ;
}

u8 Read_RB_HW(void)
{
	return GPIO_ReadInputDataBit(GPIO_HWPWR, RB_HW)  ;
}
/**********************************************************
读取IO状态
**********************************************************/
u8 Read_R_HW(void)
{
	return GPIO_ReadInputDataBit(GPIO_HWPWR, R_HW)  ;
}
   
u8 Read_LM_HW(void)
{
	return GPIO_ReadInputDataBit(GPIO_HWPWR, LM_HW)  ;
}
/**********************************************************
读取IO状态
**********************************************************/
u8 Read_B_HW(void)//hzh
{
	return GPIO_ReadInputDataBit(GPIO_HWPWR, B_HW)  ;
}
/**********************************************************
读取IO状态
**********************************************************/
u8 Read_Up_HW(void)
{
//return GPIO_ReadInputDataBit(GPIOB, UP_HW)  ;		//QZ MASK
//  return GPIO_ReadInputDataBit(GPIOE, UP_HW);

	return 0;
}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_L_Bump(void)
{
	return GPIO_ReadInputDataBit(GPIOE, L_BUMP_PIN);
}
 
/**********************************************************
读取IO状态
**********************************************************/
u8 Read_R_Bump(void)
{
	return GPIO_ReadInputDataBit(GPIOE, R_BUMP_PIN);
}

/**********************************************************
读取IO状态
**********************************************************/
u8 Read_LM_Bump(void)
{
	return GPIO_ReadInputDataBit(GPIOE, LM_BUMP_PIN);
}

u8 Read_RM_Bump(void)
{
	return GPIO_ReadInputDataBit(GPIOE, RM_BUMP_PIN);
}
/**********************************************************
读取IO状态
**********************************************************/
u8 Read_LCD_Data(void)
{
	return GPIO_ReadInputDataBit(GPIOB, LCD_DATA);
}

u8 Read_Dust_Box(void)
{
	return GPIO_ReadInputDataBit(GPIOA,DUST_BOX);
}



//返回结果:0:左右无缠绕,1:左缠绕,2:右缠绕,3:左右缠绕。
u8 Read_SB_Status(void)
{
	u8 result=0;
	if(sb_current_1s*CURR_SB_CNT_mA>SB_PROTECTCURRENT)
		result=1;
	return result;
}

u8 Read_LSB_Status(void)
{
	u8 result=0;
	return result;
}

u8 Read_RSB_Status(void)
{
	u8 result=0;
	return result;
}





















//===============================================================================================================
//===============================================================================================================
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
	RCC_ClocksTypeDef rcc_clk;
	RCC_DeInit();
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//FLASH打开预取缓冲器，AHB分频系数不为1时，必须打开 
	FLASH_SetLatency(FLASH_Latency_2);						//SYSCLK大于48MHz时，FLASH时延两个等待状态

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				//使用PLL做为系统时钟,最大72MHz
	RCC_HCLKConfig(RCC_SYSCLK_Div1);						//AHB不分频,AHB=SYSCLK,最大72MHz
	RCC_PCLK2Config 	(RCC_HCLK_Div1);					//APB2不分频,PCLK2=AHB=SYSCLK,最大72MHz	
#ifndef USE_HSE	
	RCC_HSEConfig(RCC_HSE_OFF); 							//使用内部高速晶振做为系统时钟
	RCC_HSICmd(ENABLE);
#ifdef CPU_FREQ_36MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);	//使用内部高速晶振两分频，SYSCLK=8MHz/2*9,倍频到36MHz;
	RCC_PCLK1Config(RCC_HCLK_Div1); 						//APB1不分频,PCLK1=AHB=SYSCLK,不超过36MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);						//SYSADC=PCLK/4,9MHz,不超过14MHz
#else
#ifdef CPU_FREQ_64MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);	//使用内部高速晶振两分频，SYSCLK=8MHz/2*16,倍频到64MHz;
	RCC_PCLK1Config(RCC_HCLK_Div2); 						//HCLK为64MHz时,PCLK1=AHB/2,32MHz,不超过36MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);						//SYSADC=PCLK/8,8MHz,不超过14MHz
#endif
#endif
	
#else
	RCC_HSEConfig(RCC_HSE_ON);								//使用外部高速晶振做为系统时钟
	RCC_WaitForHSEStartUp();
#ifdef CPU_FREQ_64MHz
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_8); 	//使用外部高速晶振不分频，SYSCLK=8MHz*8,倍频到64MHz
	RCC_PCLK1Config(RCC_HCLK_Div2); 						//PCLK1两分频,PCLK1=AHB/2,32MHz,不超过36MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);						//SYSADC=PCLK/8,8MHz,不超过14MHz
#endif
#ifdef CPU_FREQ_72MHz
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); 	//使用外部高速晶振不分频，SYSCLK=8MHz*9,倍频到72MHz
	RCC_PCLK1Config(RCC_HCLK_Div2); 						//PCLK1两分频,PCLK1=AHB/2,36MHz,不超过36MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);						//SYSADC=PCLK/8,9MHz,不超过14MHz 	
#endif
#endif

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_PLLCmd(ENABLE);

	RCC_GetClocksFreq(&rcc_clk);

}




/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
////////////#ifdef  VECT_TAB_RAM  
////////////  /* Set the Vector Table base location at 0x20000000 */ 
////////////  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
////////////#else  /* VECT_TAB_FLASH  */
////////////  /* Set the Vector Table base location at 0x08000000 */ 
////////////  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
////////////#endif 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);   
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}
//===============================================================================================================
//===============================================================================================================


//qz add 20180823
u8 Read_Last_Is_Dead(void)
{
	u8 temp_mode=0,temp_submode=0;
#ifndef DS1307
	u16 temp_data=0;
	temp_data=BKP_ReadBackupRegister(MODE_BACKUP_BKP);
	temp_mode=temp_data>>8;
	temp_submode=temp_data&0x00ff;

#else
	DS1307_Read_Backup(MODE_BACKUP_ADDR, &temp_mode);
	DS1307_Read_Backup(SUBMODE_BACKUP_ADDR, &temp_submode);
#endif
	if((temp_mode==MODE_CEASE)&(temp_submode==SUBMODE_DEAD))
		{
#ifdef DEBUG_INIT
			TRACE("Last mode is DEAD!\r\n");
#endif
			return 1;
		}
	else
		{
#ifdef DEBUG_INIT
			TRACE("Last mode is not DEAD!\r\n");
#endif
			return 0;
		}
}

void Gyro_Rst(void)
{
#ifndef 		RTC_8M_CORR 
		//qz add 20180910
		GYRO_RST_0;
		delay_100us(3000);
		GYRO_RST_1;
		
		//qz add end
#else
	
	//qz add end
#endif
}

void Check_MideBrush2Suck(void)
{	
	if(!GPIO_ReadInputDataBit(BRUSH2SUCK_GPIO,BRUSH2SUCK_PIN))
		{
			MidBrush.use=true;			//中扫启用
			MidBrush.flag=true;			//中扫检测启用
		}
	else
		{
			MidBrush.use=false;			//吸口，中扫不启用
			MidBrush.flag=false;		//吸口，中扫检测关闭
		}
}

u8 Read_SlamIsDown(void)
{
	if(!GPIO_ReadInputDataBit(GPIOB,SLAMDOWN_PIN))
		return 1;
	else
		return 0;
}

u8 Read_PwrSwitch_Status(void)
{
	if(GPIO_ReadInputDataBit(GPIOE,PWR_SWITCH_PIN))
		power.switch_flag=true;
	else
		power.switch_flag=false;
	return power.switch_flag;
}

u8 Read_RingPWMCtrl_Status(void)
{
	if((READ_RING_PWM_CTL))
		return 1;
	else
		return 0;
}

void Enable_RingPWMCtrl(void)
{
#ifdef RING_PWM_CTL
	if(READ_RING_PWM_CTL)
		RING_PWM_CTL_0;
#else
	return;
#endif
}

void Disable_RingPWMCtrl(void)
{
#ifdef RING_PWM_CTL
	if(READ_RING_PWM_CTL)
		return;
	else
		RING_PWM_CTL_1;
#else
	return;

#endif
}

void Init_BKP_Bat_Rtc(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

#ifdef DS1307
	u8 data=0;
	if(DS1307_Read_Backup(FLAG_BACKUP_ADDR, &data))
		return;
	if(data!=0x5A)												//第一次开机或者DS1307电池没电了
		{
			DS1307_Data_Reinit();
			Battery.reinit=true;		//电池电量重置标志
#ifdef DEBUG_INIT
			TRACE("DS1307 Data Reinit!\r\n");
#endif
		}
#else
	u16 data1;
	PWR_BackupAccessCmd(ENABLE);
	data1=BKP_ReadBackupRegister(BAT_REINIT_BKP);
	if(data1!=0xAA55)										//第一次开机或者电池没电了
		{
			BKP_DeInit();											//备份域复位
			Reinit_Rtc();												//rtc时钟重新设置
			Reinit_Battery();
		}
	PWR_BackupAccessCmd(DISABLE);
#endif

	Init_Battery_Data();
	Init_Rtc_Data();

}
