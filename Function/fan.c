#include "AAA-include.h"
bool fanspd_flag=false;
u32 fanspd_cnt=0,fanspd_cnt_last=0;
int fanspd=0,fanspd_tgt=0;
int fanspd_ek[3],fan_pwm=0;


void Init_FanSpd_Interrupt(void)
{
	NVIC_InitTypeDef	NVIC_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line4);	
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStructure);

	Reset_FanSpd_Data();

}

void Reset_FanSpd_Data(void)
{
#ifdef DEBUG_Enter_Mode
	TRACE("Reset FanSpd Data!!\r\n");
#endif
	fanspd_cnt=0;
	fanspd_cnt_last=0;
	fan_pwm=0;
	fanspd=0;
	fanspd_ek[0]=0;fanspd_ek[1]=0;fanspd_ek[2]=0;
	fanspd_flag=false;
}

void FanSpd_Cal(void)
{
	int fanspd_dif=0;
	fanspd_dif=fanspd_cnt-fanspd_cnt_last;

	fanspd=(int)(fanspd_dif*50);
	fanspd_cnt_last=fanspd_cnt;
}

void FanSpd_PID(void)
{
	if(!Fan.flag)
		return;

	if(!fanspd_flag)
		return;

	fanspd_flag=false;
	int data1=0;
	fanspd_ek[2]=fanspd_ek[1];
	fanspd_ek[1]=fanspd_ek[0];
	fanspd_ek[0]=fanspd-fanspd_tgt;

	data1=(fanspd_ek[0]-fanspd_ek[1])*FAN_KP+fanspd_ek[0]*FAN_KI+(fanspd_ek[0]+fanspd_ek[2]-2*fanspd_ek[1])*FAN_KD;
	fan_pwm=fan_pwm-data1;

	if(fan_pwm<0)
		fan_pwm=0;
	else if(fan_pwm>2000)
		fan_pwm=2000;
	Set_Fan_Pwm(fan_pwm);
}

