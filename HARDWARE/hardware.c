////////////////////////ͷ�ļ�////////////////////////////////////
#include <stm32f10x.h>
#include "hardware.h"
bool UV_FLAG=false;
bool pwrswitch_flag=false;
bool gyro_cal_flag=false;
//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================



////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////

///////////////////////˽�к���////////////////////////////////////
///////////////////////����ʵ��////////////////////////////////////





//===============================================================================================================
//===============================================================================================================
/*******************************************************************************
��������Init_System
���ܣ���ʼ������ϵͳ
���룺��
�������
���أ���
*******************************************************************************/
void  Init_System(void)
{
	uint32 	time;
  
	RCC_Configuration();				//��ʼ��ϵͳ�ľ���������ֲ��Ҫ�޸�
	delay_init(64);
	NVIC_Configuration();				//��ʼ��ϵͳ���жϣ�������ֲ��Ҫ�޸�
	init_time_2();        				//Timer2 10K�ж�  ������������ϵͳ�Ļ�������
	Init_Hardware();    				//��ʼ��ϵͳ��Ӳ����������ֲ��Ҫ�޸�
	URAT1_init(115200);
#ifdef TUYA_WIFI
	DMA_Uart1Tx_Config(USART1_TX_SIZE);
#ifdef DMA_IRQ 
	DMA_Uart1Rx_Config(USART1_RX_SIZE);
#endif
	wifi_protocol_init();
#else			
	DMA_Uart1Tx_Config(16);
	DMA_Uart1Rx_Config(128);
#endif		
	USART2_Init();
	USART2_DMA_TX_Init(16);
	USART2_DMA_RX_Init(USART2_RX_SIZE);
	USART3_Init();
	USART3_DMA_TX_Init(USART3_TX_SIZE);
	USART3_DMA_RX_Init(USART3_RX_SIZE);
	
	Init_Voice();						//��ʼ������оƬ��������������
#ifdef MHW_CAPTURE
	Init_Time_1();						//��ʼ��TIM1��TIM1�����к��Ⲷ��
#endif	  
	Init_WatchDog();					//��ʼ�����Ź�
	init_rtc();							//��ʼ��RTC,BKP���ݶ�ȡ,���������ж�
	Battery_Data_Init();
	Init_PWM();							//��ʼ��PWM��PWM���ڵ�������������������������
//		init_hwincept();					//��ʼ��������ճ���
//	Init_Remote_Info();					//qz add 20189817��ʼ��ң����Ϣ
	init_power();
	time = giv_sys_time;
	while((giv_sys_time - time) < 400){judge_charge();};
		Init_Bump_Interrupt();				//��ʼ����ײ�ж�
	init_ad();							//��ʼ��AD	  
	init_key();							//��ʼ����������Ϣ���磺���������ص�
	Init_Ring();
	init_wallearth();	
	Init_SLAM_DATA();					//��ʼ���뵼����ͨ������
	Init_Gyro_Data();
	Gyro_Rst();							//qz add 20180927
	ReadPreengage();					//��ȡԤԼ

	Init_Mode();
	Init_Check_Status();				//��ʼ���쳣���
}
//===============================================================================================================
//===============================================================================================================

  


//===============================================================================================================
//===============================================================================================================

void Init_Hardware (void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //�ܽŽṹ



	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB 
                     | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD 
					           | RCC_APB2Periph_GPIOE , ENABLE);
/********************************************/
//
//
//��������������������������������������������
//
//
/********************************************/
/**************************/
//
//���ո��ո��ո��ո��ո���
//
/**************************/
	 /////////////////////////PA�ڸ������룺
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin=CHARGE_SEAT|VOICE_BUSY_PIN;		//���ɨ�������ţ���ʱ����GPIO��ȡ
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	///////////////////////PB��������
  	///////////////////////PC��������
  	GPIO_InitStructure.GPIO_Pin=KEY_3;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);  
  	///////////////////////PD��������
//  	GPIO_Init(GPIOD, &GPIO_InitStructure);  
  ///////////////////////PE�������� 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Pin =CHARGE_DC|R_SPEED|L_SPEED|PWR_SWITCH_PIN|KEY_2|FAN_SPEED_PIN|MB_SPEED_PIN;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
/**************************/
//
//������������������������
//
/**************************/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; 
//	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=BRUSH2SUCK_PIN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
  /////////////////////////PD���������룺
// 	GPIO_Init(GPIOD, &GPIO_InitStructure);

/********************************************/
//
//
//���������������������������������
//
//
/********************************************/
/**************************/
//
//������������������������
//
/***************************/
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	//////////////////////////PA���������///////////////////
	GPIO_InitStructure.GPIO_Pin = GYRO_CAL_PIN;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	/////////////////////////PB���������///////////////////
  	GPIO_InitStructure.GPIO_Pin = VOICE_CLK|VOICE_DATA|RING_PWM_CTL_PIN|LSB_PWR_PIN; 
    GPIO_Init(GPIOB,&GPIO_InitStructure);
	/////////////////////////PC���������///////////////////
	GPIO_InitStructure.GPIO_Pin=HOLDPWR_PIN|BAT_CHECK_PIN|PWR3V3_PIN|PWR5V_PIN|LED_RED|LED_GREEN;
    GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=WALL_SEND|EARTH_SEND;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	PWR3V3_ON
	PWR5V_ON
	V_CLK_1;				//����VOICE_CLK
	V_DAT_1;				//����VOICE_DATA
	GYRO_CAL_PIN_1;			//���߹ߵ�У׼
	BAT_CHECK_1;			//���ߵ�ص�ѹ���
	Enable_RingPWMCtrl();
	Open_Ring_Cnt();
/**************************/
//
//��©��©��©��©��©��©��©
//
/***************************/
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  	///////////////GPIOA��©���/////////////
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
  	///////////////GPIOB��©���/////////////
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	///////////////GPIOD��©���/////////////
  	GPIO_InitStructure.GPIO_Pin=SPEED_SEND;
  	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_SetBits(GPIOD,SPEED_SEND);
	/////////////////GPIOE��©���////////////////
//	GPIO_SetBits(EARTH_SEND_GPIO,EARTH_SEND);
//	GPIO_SetBits(WALL_SEND_GPIO,WALL_SEND);
	LED_GREEN_OFF;
	LED_RED_OFF;

}
//======================================================================================== 







 
/***********************************************************
���ܣ��ø�
***********************************************************/
//void Set_S_Motor(void)
//{           
  //GPIO_SetBits(GPIOA, S_MOTOR_DRIVER);
  //GPIO_ResetBits(GPIOC, S_MOTOR_DRIVER_D);
//}
#ifdef UV
/***********************************************************
���ܣ��ø�
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
���ܣ��ø�
***********************************************************/
void Set_Dust_Motor(void)
{           
//  GPIO_SetBits(GPIOC, DUST_MOTOR_DRIVER);

//GPIO_ResetBits(GPIOC, DUST_MOTOR_DRIVER_D);

}

/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_Wall_Send(void)
{    
//	GPIO_ResetBits(GPIOA, WALL_SEND);
	GPIO_SetBits(WALL_SEND_GPIO,WALL_SEND);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_Stb(void)
{           
  GPIO_SetBits(GPIOC, LCD_STB);
  
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_Power(void)
{           
	GPIO_SetBits(GPIOD, LCD_POW_ON);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_Clk(void)
{           
  GPIO_SetBits(GPIOD, LCD_CLK);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_Data(void)
{           
  GPIO_SetBits(GPIOB, LCD_DATA);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_BG(void)
{           
 	GPIO_SetBits(GPIOC, LCD_BL_ON);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_Speed_Send(void)
{           
	GPIO_ResetBits(GPIOD, SPEED_SEND ); //���Ϳ�ʼ����
	//GPIO_SetBits(GPIOD,SPEED_SEND);//QZ
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_Earth_Send(void)
{           
  GPIO_SetBits(EARTH_SEND_GPIO, EARTH_SEND);
  //GPIO_ResetBits(GPIOD, EARTH_SEND);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_L_FrontOnOff(void)
{           
//  GPIO_SetBits(GPIOE, L_FRONTONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_L_BackOnOff(void)
{           
//  GPIO_SetBits(GPIOE, L_BACKONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_R_FrontOnOff(void)
{           
//  GPIO_SetBits(GPIOE, R_FRONTONOFF);                            
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_R_BackOnOff(void)
{           
//  GPIO_SetBits(GPIOE, R_BACKONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_HW_Power_ON(void)
{           
	GPIO_ResetBits(GPIO_HWPWR, HW_POWER);
}


/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Dust_Motor(void)
{           
//  	GPIO_ResetBits(GPIOC, DUST_MOTOR_DRIVER);
//	GPIO_ResetBits(GPIOC, DUST_MOTOR_DRIVER_D);
}


/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Wall_Send(void)
{     
  //GPIO_SetBits(GPIOA, WALL_SEND);
  GPIO_ResetBits(WALL_SEND_GPIO, WALL_SEND);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_Stb(void)
{           
  GPIO_ResetBits(GPIOC, LCD_STB);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_Power(void)
{           
	GPIO_ResetBits(GPIOD, LCD_POW_ON);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_Clk(void)
{           
  GPIO_ResetBits(GPIOD, LCD_CLK);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_Data(void)
{           
  GPIO_ResetBits(GPIOB, LCD_DATA);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_BG(void)
{           
	GPIO_SetBits(GPIOC, LCD_BL_ON);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Speed_Send(void)
{           
	//GPIO_ResetBits(GPIOD,SPEED_SEND);//QZ
	GPIO_SetBits(GPIOD,SPEED_SEND);		//����ֹͣ����
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Earth_Send(void)
{      
	GPIO_ResetBits(EARTH_SEND_GPIO, EARTH_SEND);
	//GPIO_SetBits(EARTH_SEND_GPIO, EARTH_SEND);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_L_FrontOnOff(void)
{           
//  GPIO_ResetBits(GPIOE, L_FRONTONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_L_BackOnOff(void)
{           
//  GPIO_ResetBits(GPIOE, L_BACKONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_R_FrontOnOff(void)
{           
//  GPIO_ResetBits(GPIOE, R_FRONTONOFF);                            
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_R_BackOnOff(void)
{           
//  GPIO_ResetBits(GPIOE, R_BACKONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_HW_Power_OFF(void)
{ 
	GPIO_SetBits(GPIO_HWPWR, HW_POWER);
}
//=============

/***********************************************************
���ܣ��ø�
***********************************************************/
void BIRD_PE14_1(void)
{     

}
/***********************************************************
���ܣ��õ�
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
��ȡIO״̬
**********************************************************/
uint8 Read_Front_Wheel(void)
{
	return 0;
////////////return GPIO_ReadInputDataBit(GPIOB, FRONT_WHEEL)  ;
}
 


/**********************************************************
��ȡIO״̬
**********************************************************/

/**********************************************************
��ȡIO״̬
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
��ȡIO״̬
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
��ȡIO״̬
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
��ȡIO״̬
**********************************************************/
u8 Read_L_Speed(void)
{
	return GPIO_ReadInputDataBit(GPIO_LSPD, L_SPEED)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_L_LIDI(void)
{
#if 1 //shftemp	//QZ:ԭ��Ϊ0

return GPIO_ReadInputDataBit(GPIOD, L_LIDI)  ;
#else
return 1;	//QZ:ԭ��Ϊ0
#endif


}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_Charge_Dc(void)
{
return GPIO_ReadInputDataBit(GPIO_DC, CHARGE_DC)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_Charge_Seat(void)
{
return GPIO_ReadInputDataBit(GPIO_SEAT, CHARGE_SEAT)  ;
}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_R_Speed(void)
{
	return GPIO_ReadInputDataBit(GPIO_RSPD, R_SPEED)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_R_LIDI(void)
{
#if 1 //shftemp	//QZ:ԭ��Ϊ0

return GPIO_ReadInputDataBit(GPIOB, R_LIDI)  ;
#else
return 1;	//QZ:ԭ��Ϊ0
#endif

}

/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_L_HW(void)
{
	return GPIO_ReadInputDataBit(GPIOE, L_HW)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_RM_HW(void)
{
	return GPIO_ReadInputDataBit(GPIOE, RM_HW)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_R_HW(void)
{
	return GPIO_ReadInputDataBit(GPIOE, R_HW)  ;
}
   
u8 Read_LM_HW(void)
{
	return GPIO_ReadInputDataBit(GPIOE, LM_HW)  ;
}
/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_B_HW(void)//hzh
{
	return GPIO_ReadInputDataBit(GPIOE, B_HW)  ;
}
/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_Up_HW(void)
{
//return GPIO_ReadInputDataBit(GPIOB, UP_HW)  ;		//QZ MASK
//  return GPIO_ReadInputDataBit(GPIOE, UP_HW);

	return 0;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_L_Bump(void)
{
	return GPIO_ReadInputDataBit(GPIOE, L_BUMP_PIN);
}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
u8 Read_R_Bump(void)
{
	return GPIO_ReadInputDataBit(GPIOE, R_BUMP_PIN);
}

/**********************************************************
��ȡIO״̬
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
��ȡIO״̬
**********************************************************/
u8 Read_LCD_Data(void)
{
	return GPIO_ReadInputDataBit(GPIOB, LCD_DATA);
}

u8 Read_Dust_Box(void)
{
	return GPIO_ReadInputDataBit(GPIOA,DUST_BOX);
}



//���ؽ��:0:�����޲���,1:�����,2:�Ҳ���,3:���Ҳ��ơ�
u8 Read_SB_Status(void)
{
	u8 result=0;
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
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//FLASH��Ԥȡ��������AHB��Ƶϵ����Ϊ1ʱ������� 
	FLASH_SetLatency(FLASH_Latency_2);						//SYSCLK����48MHzʱ��FLASHʱ�������ȴ�״̬

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				//ʹ��PLL��Ϊϵͳʱ��,���72MHz
	RCC_HCLKConfig(RCC_SYSCLK_Div1);						//AHB����Ƶ,AHB=SYSCLK,���72MHz
	RCC_PCLK2Config 	(RCC_HCLK_Div1);					//APB2����Ƶ,PCLK2=AHB=SYSCLK,���72MHz	
#ifndef USE_HSE	
	RCC_HSEConfig(RCC_HSE_OFF); 							//ʹ���ڲ����پ�����Ϊϵͳʱ��
	RCC_HSICmd(ENABLE);
#ifdef CPU_FREQ_36MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);	//ʹ���ڲ����پ�������Ƶ��SYSCLK=8MHz/2*9,��Ƶ��36MHz;
	RCC_PCLK1Config(RCC_HCLK_Div1); 						//APB1����Ƶ,PCLK1=AHB=SYSCLK,������36MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);						//SYSADC=PCLK/4,9MHz,������14MHz
#else
#ifdef CPU_FREQ_64MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);	//ʹ���ڲ����پ�������Ƶ��SYSCLK=8MHz/2*16,��Ƶ��64MHz;
	RCC_PCLK1Config(RCC_HCLK_Div2); 						//HCLKΪ64MHzʱ,PCLK1=AHB/2,32MHz,������36MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);						//SYSADC=PCLK/8,8MHz,������14MHz
#endif
#endif
	
#else
	RCC_HSEConfig(RCC_HSE_ON);								//ʹ���ⲿ���پ�����Ϊϵͳʱ��
	RCC_WaitForHSEStartUp();
#ifdef CPU_FREQ_64MHz
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_8); 	//ʹ���ⲿ���پ��񲻷�Ƶ��SYSCLK=8MHz*8,��Ƶ��64MHz
	RCC_PCLK1Config(RCC_HCLK_Div2); 						//PCLK1����Ƶ,PCLK1=AHB/2,32MHz,������36MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);						//SYSADC=PCLK/8,8MHz,������14MHz
#endif
#ifdef CPU_FREQ_72MHz
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); 	//ʹ���ⲿ���پ��񲻷�Ƶ��SYSCLK=8MHz*9,��Ƶ��72MHz
	RCC_PCLK1Config(RCC_HCLK_Div2); 						//PCLK1����Ƶ,PCLK1=AHB/2,36MHz,������36MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);						//SYSADC=PCLK/8,9MHz,������14MHz 	
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
	if((temp_mode==CEASE)&(temp_submode==DEAD))
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
#if 1
	//qz add 20180910
	GYRO_RST_0;
	delay_100us(3000);
	GYRO_RST_1;
	
	//qz add end
#endif
}

//qz add 2090126
void HoldPwrCheck(void)
{
	u32 start_time,end_time;
	start_time=giv_sys_time;
	while(giv_sys_time-start_time<2000)		//qz modify 20190218
		{
			if(!Read_Key2())
				//Slam_Data.pwrswitch_on=true;
			Uart1_Comunication();						//�������ݽ�������
		}
	LED_GREEN_ON;	
	HOLDPWR_ON;
	while(!Read_Key2())
		{
			Uart1_Comunication();						//�������ݽ�������
		}
	end_time=giv_sys_time;
	if(end_time-start_time<12000)		//qz modify 20190218
		{
			while(giv_sys_time-end_time<10000) 	//qz modify 20190218
				{
					Uart1_Comunication();						//�������ݽ�������
				}
		}
}

void Check_MideBrush2Suck(void)
{	
	if(!GPIO_ReadInputDataBit(BRUSH2SUCK_GPIO,BRUSH2SUCK_PIN))
		{
			MidBrush.use=true;			//��ɨ����
			MidBrush.flag=true;			//��ɨ�������
		}
	else
		{
			MidBrush.use=false;			//���ڣ���ɨ������
			MidBrush.flag=false;		//���ڣ���ɨ���ر�
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
		pwrswitch_flag=true;
	else
		pwrswitch_flag=false;
	return pwrswitch_flag;
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

