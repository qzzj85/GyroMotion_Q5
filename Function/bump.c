#include "AAA-include.h"
u8 bump_value=0;
bool bump_exiton=true;
u32 bump_exitoff_time=0;
bool ring_cnt_flag=true;

void Init_Bump_Interrupt(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=L_BUMP_PIN|R_BUMP_PIN|LM_BUMP_PIN|RM_BUMP_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource6);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource9);
	EXTI_InitStructure.EXTI_Line=EXTI_Line6|EXTI_Line9;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line6);	//ÓÒÅö×²
	EXTI_ClearITPendingBit(EXTI_Line9);	//×óÅö×²
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn;
#ifdef MILE_COMPENSATION
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
#else
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
#endif
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	
}

#ifdef MILE_COMPENSATION
void back_speed(void)
{
	u8 i;
	disable_pwm(R_FRONT);
	disable_pwm(L_FRONT);
	disable_pwm(R_BACK);
	disable_pwm(L_BACK);	
	l_ring.stop_spd=0;r_ring.stop_spd=0;
	for(int i=0;i<10;i++)
		{
			l_ring.stop_spd+=l_ring.stop_buf[i];
			r_ring.stop_spd+=r_ring.stop_buf[i];
			l_ring.stop_buf[i]=0;
			r_ring.stop_buf[i]=0;
		}
    r_ring.stop_spd=r_ring.stop_spd*10;
    l_ring.stop_spd=l_ring.stop_spd*10;

	int l_spd,r_spd;
	u8  l1=L_FRONT,r1=R_FRONT;
	l_spd = 2000 * l_ring.stop_spd /1000;
	r_spd = 2000 * r_ring.stop_spd /1000;	
	if (l_spd >0) 
		l1 = L_BACK;
	if (l_spd == 0) 
		l1 = 0xff;
	if (r_spd >0) 
		r1 = R_BACK;
	if (r_spd == 0) 
		r1 = 0xff;	
	l_spd =abs(l_spd/50);
	r_spd =abs(r_spd/50);	
    for (i=50;i>0;i--)
		{
			enable_pwm(r1,r_spd*i);
			enable_pwm(l1,l_spd*i);      
			DelayMs(1);
		}
	disable_pwm(R_FRONT);
	disable_pwm(L_FRONT);
	disable_pwm(R_BACK);
	disable_pwm(L_BACK);
}

#endif

void Close_Bump_Exit(void)
{
	if((!bump_exiton)&(GPIO_ReadInputDataBit(GPIOB,RING_PWM_CTL_PIN)))
		return;
	EXTI->IMR&=0XFFFFFDBF;			//ÆÁ±ÎÅö×²ÖÐ¶Ï
	EXTI->PR&=0x00000240;			//Çå³ýÅö×²ÖÐ¶Ï
	
#ifdef MILE_COMPENSATION
	bump_exiton=false;
	bump_exitoff_time++;
	Close_Ring_Cnt();		
    back_speed();
	Disable_RingPWMCtrl();	
#else
	Disable_RingPWMCtrl();
	bump_exiton=false;
	bump_exitoff_time++;
	Close_Ring_Cnt();
#endif	
}

void Open_Bump_Exit(void)
{

	if((bump_exiton)&(!GPIO_ReadInputDataBit(GPIOB,RING_PWM_CTL_PIN)))
		return;
	EXTI->IMR|=0x00000240;			//¿ª·ÅÅö×²ÖÐ¶Ï
	bump_exiton=true;
	Enable_RingPWMCtrl();
}

void Clear_BumpValue(void)
{
	static u8 l_bump_num=0,r_bump_num=0,lm_bump_num=0,rm_bump_num=0;
	if(bump_value&L_BUMP_BIT)
		{
			if(Read_L_Bump())
				{
					l_bump_num++;
					if(l_bump_num>=3)
						{
							bump_value&=L_BUMP_MASK;
							l_bump_num=0;
						}
				}			
		}
	if(bump_value&LM_BUMP_BIT)
		{
			if(Read_LM_Bump())
				{
					lm_bump_num++;
					if(lm_bump_num>=3)
						{
							bump_value&=LM_BUMP_MASK;
							lm_bump_num=0;
						}
				}
		}
	if(bump_value&RM_BUMP_BIT)
		{
			if(Read_RM_Bump())
				{
					rm_bump_num++;
					if(rm_bump_num>=3)
						{
							bump_value&=RM_BUMP_MASK;
							rm_bump_num=0;
						}
				}
		}
	if(bump_value&R_BUMP_BIT)
		{
			if(Read_R_Bump())
				{
					r_bump_num++;
					if(r_bump_num>=3)
						{
							bump_value&=R_BUMP_MASK;
							r_bump_num=0;
						}
				}
		}
#ifdef RING_PWM_CTL
	if((bump_value==0))
		{
			Open_Bump_Exit();
		}
#endif
}

u8 Parse_BumpValue(void)
{
	switch (bump_value)
		{
			case 0:
				return 0;
			case 1:
				return BUMP_ONLY_RIGHT;
			case 2:
				return BUMP_ONLY_RIGHTMID;
			case 3:
				return BUMP_RIGHT_MID;
			case 4:
				return BUMP_ONLY_LEFTMID;
			case 8:
				return BUMP_ONLY_LEFT;
			case 0xc:
				return BUMP_LEFT_MID;
			case 5:
			case 6:
			case 7:
			case 9:
			case 0xa:
			case 0xb:
			case 0xd:
			case 0xe:
			case 0xf:
				return BUMP_MID;
			default:
				return 0;			
		}
//	return 0;
}

void Open_Ring_Cnt(void)
{
	ring_cnt_flag=true;	
}

void Close_Ring_Cnt(void)
{
	ring_cnt_flag=false;
}
