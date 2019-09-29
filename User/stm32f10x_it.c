/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.1
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/*--------------�ж����ȼ�--------------------
AD DMA�жϣ�								��ռ���ȼ�0�������ȼ�0			
�ⲿ�ж�5_9(���ڻس����):						��ռ���ȼ�0�������ȼ�0
�ⲿ�ж�10_15(������ײ):						��ռ���ȼ�0�������ȼ�0
TIM1���벶��(�����к���):						��ռ���ȼ�1�������ȼ�0(δʹ��)
TIM2����ж�(����ʱ��):							��ռ���ȼ�1�������ȼ�0

USART1�����ж�(���ڵ��������):					��ռ���ȼ�3�������ȼ�0(ȡ��)
USART1����DMA�ж�(���ڵ����������ϱ�):				��ռ���ȼ�4�������ȼ�0
USART2�����ж�(����DEBUG����):					��ռ���ȼ�3�������ȼ�0(ȡ��)
USART2����DMA�ж�(����DEBUG����):				��ռ���ȼ�5�������ȼ�0(ȡ��)
USART3�����ж�(���������ǽ���):					��ռ���ȼ�3�������ȼ�0(ȡ��)
USART3����DMA�ж�(���������ǿ���):					��ռ���ȼ�5�������ȼ�0(ȡ��)


�ⲿ�ж�10_15(����STOP����):					��ռ���ȼ�12�������ȼ�0
RTC�����ж�(PP2ȡ��):							��ռ���ȼ�12,�����ȼ�0
----------------------------------------------*/



//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================ͷ�ļ�====================================
#define  FENGRUI_FR1703

#ifdef	HW_INT_REC
#define BIT0_MIN_SAMPLE	14//14//12		//ms
#define BIT0_MAX_SAMPLE 16//16//17		//ms
#define BIT1_MIN_SAMPLE 7//6//5		//ms
#define BIT1_MAX_SAMPLE 9//8//9		//ms
#define START_FRAME_MIN	28
#define START_FRAME_MAX 32
#endif

static	 u8 r_step = 0;
static	 u8 l_step = 0;   


bool log_show;
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/////////////////////////////////////////////////////////////////////////////////

#ifdef REMOTE
void Parse_Remote_Signa(INFR_DATA *hw_info)
{
	//��ʱ200ms,��ֹң�ذ�����������һ�ΰ�����ŷ�3-4����ͬ���룬ʱ��Լ200ms����
	if(giv_sys_time-remote_info.effect_time<2000)
		return;
	switch (hw_info->data)
		{
			case REMOTE_SLEEP:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_SLEEP;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_DOCK:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_DOCK;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_GUIHUA:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_GUIHUA;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_FORWOARD:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_FORWORD;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_LEFT:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_LEFT;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_OK:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_OK;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_RIGHT:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_RIGHT;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_BACK:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_BACK;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_YBS:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_YBS;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_SPOT:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_SPOT;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_TIME:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_TIME;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_SET:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_SET;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
			case REMOTE_PREEN:
				remote_info.effect=true;
				remote_info.effect_time=giv_sys_time;
				remote_info.remote_key=REMOTE_KEY_PREEEN;
				remote_info.rece_ir=hw_info->rece_ir;
				break;
		}
}
#endif



static u8 w_step = 0;
//======================================================================================

void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
		{
		}
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
}

u32 data_temp;

#ifdef MHW_CAPTURE
void TIM1_CC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_CC2)!=RESET)
		{
			if(!GPIO_ReadInputDataBit(GPIOE,RM_HW))		//�����½���
				{
					TIM_OC2PolarityConfig(TIM1, TIM_ICPolarity_Rising);	//����Ϊ�����ز���
					TIM_SetCounter(TIM1, 0);
				}
			else										//����������
				{
					data_temp=TIM_GetCapture2(TIM1);	//��ȡ�͵�ƽʱ��
					TIM_OC2PolarityConfig(TIM1, TIM_ICPolarity_Falling); //����Ϊ�½��ز���
					if(rm_hw.start)
						{
							if((data_temp>=(BIT0_MIN_SAMPLE*100))&&(data_temp<=(BIT0_MAX_SAMPLE*100)))		//'0'
								{
									rm_hw.tempdata<<=1;
									rm_hw.tempdata|=0;
									rm_hw.bitnumber++;
								}
							else if((data_temp>=(BIT1_MIN_SAMPLE*100))&&(data_temp<=BIT1_MAX_SAMPLE*100))		//'1'
								{
									rm_hw.tempdata<<=1;
									rm_hw.tempdata|=1;
									rm_hw.bitnumber++;
								}

							//qz add 20180823��������
#ifdef INTERFERENCE_ACTION
							else
								{
									rm_hw.tempdata=0;
									rm_hw.start=0;
									rm_hw.bitnumber=0;
								}
#endif
							//qz add end
							if(rm_hw.bitnumber==8)
							{
								rm_hw.data=rm_hw.tempdata;
								rm_hw.tempdata=0;
								rm_hw.start=0;
								switch (rm_hw.data)
									{
									case RIGHT_SIGN:
									 rm_hw.effectRight=1;
									 rm_hw.effect_timeRight=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									 break;
									case LEFT_SIGN:
									 rm_hw.effectLeft=1;
									 rm_hw.effect_timeLeft=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									 break;
									case LEFT_MID_SIGN:
									 rm_hw.effectMidLeft=1;
									 rm_hw.effect_timeMidLeft=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									break;
									case RIGHT_MID_SIGN:
									 rm_hw.effectMidRight=1;
									 rm_hw.effect_timeMidRight=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									 break;
									case NEAR_SIGN:
									 rm_hw.effectNear=1;
									 rm_hw.effect_timeNear=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									 break;
#ifdef DOCK_NEAR						
										case 0x55:
											rm_hw.effectNear=1;
											rm_hw.effect_timeNear=giv_sys_time;
											rm_hw.effect=1;
											rm_hw.effect_time=giv_sys_time;
										break;
#endif
#ifdef REMOTE		
										default:
											Parse_Remote_Signa(&rm_hw);
										break;
#endif
									}
							}

						}
					else if((data_temp>=(START_FRAME_MIN*100))&&(data_temp<=(START_FRAME_MAX*100)))
						{
									rm_hw.start=1;
									rm_hw.bitnumber=0;
						}
							
				}
					
		}
	TIM_ClearITPendingBit(TIM1,TIM_IT_CC2);
}
#endif

void TIM2_IRQHandler(void)	//	10K �ж�
{ 
//	if((mode.mode==DOCKING)||(BS_NO_TIME_FLAG))			//�����С�س�ģʽ�����ͱ�ɨƵ��
//		GPIOA->ODR^=GPIO_Pin_2;

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update );
	giv_sys_time ++;
	
#ifdef FREE_SKID_INDEP_CHECK
	if(Free_Skid_Indep.check_flag)
		Free_Skid_Indep.check_start_time++;
#endif

	POWER_ready = true;           	//��س��ʱ��
	
	if((giv_sys_time % 2) == 0)
		{
			VOL_TEMP_ready = true;      //����������   
		}

	//qz add 20180615
	if((giv_sys_time%5)==0)
		{
			VOL_TEMP_READY_MY=true;
		}

	if((giv_sys_time %10) == 0)			//	1mS  1000HZ   ʵ��500����
			{   
				// if(ADC_ready	==false)
				//	 {
				//			 ReInitAd();     
				//	 }
//				 ADC_ready = false;
//				rap_time=true;

			}
	if((giv_sys_time%20)==0)			//2ms
		{
			dock_rap_time=true;
			check_earth_time=true;
			//YBS_rap_time=true;
			check_wall_time=true;
		}

	if((giv_sys_time%30)==0)
		{
//			YBS_rap_time=true;
		}
			
	if((giv_sys_time%50)==0)			//5ms
		{
//			YBS_rap_time=true;
//			rap_time=true;
//			YBS_Check_Flag  		= true;	
			wall_earth_time = true; 	
			gyro_bios.check_flag=true;
		}
				
			
//	if((giv_sys_time % 100) == 0)
	if((giv_sys_time % 100) == 0)		//	10mS
		{
			YBS_Check_Flag=true;
			key_time = true;          //����ʱ��	  
			rap_time = true;          //�������ٶȵ���ʱ��
//			gbv_yaokong_time = true;	
			
//			YBS_Conor_Check_Flag  = true;
			CHARGE_READY=true;		//qz add 20180522
			Rotate_Skid.time_flag=true;
			rotate_angle.time_flag=true;
//			time_speed=true;
			gyro_check_time=true;
			coordinate_show=true;
			Cal_xy();
			Cal_CoordinateXY();
			Record_Coordinate_Intime();
			//Cal_Xmaxmin();
		}			
		
		
		
			
	if((giv_sys_time % 200) == 0)				//20mS
		{
//			rap_time=true;
			sensor_report_time = true;			//��̼� �����ϱ�ʱ��
			Mileage_report_time=true;
//			YBS_Check_Flag  		= true;			
//			Ultra.Repeat_time 	= true;
			time_stemp=giv_sys_time;	//QZ ADD
//			YBS_Wall_Count_Time=true;			//qz add 20180902
//			Coordinate_report_time=true;
			time_speed=true;
			//Analysis_Coordinate();
		}

	//qz add 20180316
	if((giv_sys_time%400)==0)
		{
#ifdef TUYA_WIFI
			wifi_time=true;
#endif
		}
		
	if((giv_sys_time % 500) == 0)				//50 mS
		{
			led.led_time=true;
			gyro_comm_flag=true;
		}
		
	if((giv_sys_time % 1000) == 0)				//100ms			
		{
//			log_show = true;				//��ʾ����ʱ��		
			CHECK_STATUS_TIME=true; 		//qz add 20180417
			action_wall_time=true;
			spd_acc_flag=true;
		}
	
	if((giv_sys_time%3000)==0)
		{
//			spd_acc_flag=true;
			led.quic_flag=true;
#ifdef TUYA_WIFI
			wifi_update_time=true;
#endif

		}
	if((giv_sys_time % 5000) == 0)				//500 mS
		{
			log_show = true;				//��ʾ����ʱ��		
			Half_Sec  = true;
			Read_PwrSwitch_Status();
		}
	
	if((giv_sys_time%10000)==0)
		{
			Sec=true;
			led.slow_flag=true;
		}
	
	if((giv_sys_time%20000)==0)			//2s
		{
			get_dispow_flag=true;
			Sec_Two  = true; 
			watchdog_time = true;
//			abort_shiftybs_flag=true;
		}

	if((giv_sys_time%30000)==0)
		{
			abort_shiftybs_flag=true;
		}
	
	if((giv_sys_time % 50000) == 0)     //5���־
		{
			Five_Sec  = true;	
		}

	if((giv_sys_time%100000==0))		//10���־
		{
			Ten_Sec=true;
		}

	if((giv_sys_time%300000==0))		//30���־
		{
			Half_Min=true;
		}
	
	if((giv_sys_time % 600000) == 0)	//���ӱ�־
		{
			gbv_minute = true;
			app_bat_min=true;	//qz add
			if(mode.status)
				gyro_cal_flag=true;
			else
				gyro_cal_flag=false;
		}
}
//======================================================================================
//======================================================================================






















//======================================================================================
//======================================================================================
/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


uint8_t ADC_test_counter;

//////void ADC1_2_IRQHandler(void)
//////{
////////////		ADC_test_counter++;
////////////		if((ADC_test_counter& 0x01) == 0x01)
////////////			{BIRD_PE14_1();}
////////////		else{BIRD_PE14_0();}
////////////	
//////		
////////////	//	 ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
////////////		 ADC_ready = true;
////////////		 if(((DMA1_Channel1->CNDTR)&0XF) != 0)
////////////				 {
////////////					 ReInitAd();
////////////				 }
////////////		 else
////////////				 {			 
//////			
//////	
//////		ADC_SoftwareStartConvCmd(ADC1, ENABLE);  
//////	
//////	
////////////				 }

//////	
//////		 ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
//////}
//======================================================================================
//======================================================================================













/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void ADC_IRQHandler(void)
//{
// ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
// ADC_ready = true;
// if(((DMA1_Channel1->CNDTR)&0XF) != 0)
// {
//	 ReInitAd();
// }
// else
// {			 
//     ADC_SoftwareStartConvCmd(ADC1, ENABLE);  
// }	   
// ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
//}


void EXTI0_IRQHandler(void)
{
	//�����ٶ������ⲿ�жϺ���
	if(EXTI_GetITStatus(EXTI_Line_LSPD)!=RESET)
		{
			EXTI_ClearITPendingBit(EXTI_Line_LSPD);
			switch (l_step)
				{
					case 0:
						if(Read_L_Speed() == Bit_RESET)   //Bit_RESET = 0
							{
								if((giv_sys_time - l_ring.dq_time) > 1)
									{
										if(ring_cnt_flag)
											{
												l_ring.length++; 
												l_ring.speed_length++;		//qz add 20180203
												l_ring.dq_time = giv_sys_time;
												if(l_rap.ori == FRONT)
													{
														l_ring.all_length++;
													}
		//										else
												else if(l_rap.ori==BACK)
													{
														l_ring.all_length--;
													}
											}
									}
									l_step = 1; 
							}
						break;
					case 1:
						if(Read_L_Speed() != Bit_RESET)
							{
								if((giv_sys_time - l_ring.dq_time) > 1)
									{
										if(ring_cnt_flag)
											{
												l_ring.length++; 
												l_ring.speed_length++;		//qz add 20180203
												if(l_rap.ori == FRONT)
													{
														l_ring.all_length++;
													}
		//										else
												else if(l_rap.ori==BACK)
													{
														l_ring.all_length--;
													}
											}
																
										l_ring.dq_time = giv_sys_time;
									}
									l_step = 0; 
							}
						break;
					default :
						l_step = 0;
				 }
		}
}
/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
#ifdef SPEED_EXTI
	if(EXTI_GetITStatus(EXTI_Line_RSPD)!=RESET)
		{
			EXTI_ClearITPendingBit(EXTI_Line_RSPD);
			switch (r_step)
				{
					case 0:
						if(Read_R_Speed() == Bit_RESET)   //Bit_RESET = 0
							{
								if((giv_sys_time - r_ring.dq_time) > 1)
									{
										if(ring_cnt_flag)
											{
												r_ring.length++; 
												r_ring.speed_length++;		//qz add 20180203
												if(r_rap.ori == FRONT)
													{
														r_ring.all_length++;
													}
		//										else
												else if(r_rap.ori==BACK)
													{
														r_ring.all_length--;
													}
											}
										r_ring.dq_time = giv_sys_time;
									}
									r_step = 1; 
							}
						break;
					case 1:
						if(Read_R_Speed() != Bit_RESET)
							{
								if((giv_sys_time - r_ring.dq_time) > 1)
									{
										if(ring_cnt_flag)
											{
												r_ring.length++; 
												r_ring.speed_length++;		//qz add 20180203
												if(r_rap.ori == FRONT)
													{
														r_ring.all_length++;
													}
		//										else
												else if(r_rap.ori==BACK)
													{
														r_ring.all_length--;
													}
											}						
										r_ring.dq_time = giv_sys_time;
									}
									r_step = 0; 
							}
						break;
					default :
						r_step = 0;
						 }
		}
#endif
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
				{
						ExitInput = true;
						EXTI_ClearITPendingBit(EXTI_Line2);	  
				}
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
	
}

//======================================================================================
//======================================================================================

//�����ٶ������ⲿ�жϺ���
void EXTI4_IRQHandler(void)
{
	
}



/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
#if 0
	static u32 rm_hw_time=0,l_hw_time=0;
	u32 sample_time;
	static u32 r_hw_time,b_hw_time,mm_hw_time;
	 /////////////�Һ����ж�///////////////
	 if(EXTI_GetITStatus(EXTI_Line_RHW)!=RESET) 		 
	 {
		 if(Read_R_HW())	 //������
			 {
				 if(r_hw.start)
					 {
						 sample_time=giv_sys_time-r_hw_time;
						 if((sample_time>=BIT0_MIN_SAMPLE)&&(sample_time<=BIT0_MAX_SAMPLE)) 		 //bit '0'
							 {
								 r_hw.tempdata<<=1;
								 r_hw.tempdata|=0;
								 r_hw.bitnumber++;
							 }
						 else if((sample_time>=BIT1_MIN_SAMPLE)&&(sample_time<=BIT1_MAX_SAMPLE))	 //bit '1'
							 {
								 r_hw.tempdata<<=1;
								 r_hw.tempdata|=1;
								 r_hw.bitnumber++;
							 }
 
#ifdef INTERFERENCE_ACTION
						 //qz add 20180823��������
						 else
							 {
								 r_hw.tempdata=0;
								 r_hw.start=0;
								 r_hw.bitnumber=0;
							 }
#endif
						 if(r_hw.bitnumber==8)
							 {
								 r_hw.data=r_hw.tempdata;
								 r_hw.tempdata=0;
								 r_hw.start=0;
								 switch (r_hw.data)
									 {
									 case RIGHT_SIGN:
									  r_hw.effectRight=1;
									  r_hw.effect_timeRight=giv_sys_time;
									  r_hw.effect=1;
									  r_hw.effect_time=giv_sys_time;
									  break;
									 case LEFT_SIGN:
									  r_hw.effectLeft=1;
									  r_hw.effect_timeLeft=giv_sys_time;
									  r_hw.effect=1;
									  r_hw.effect_time=giv_sys_time;
									  break;
									 case LEFT_MID_SIGN:
									  r_hw.effectMidLeft=1;
									  r_hw.effect_timeMidLeft=giv_sys_time;
									  r_hw.effect=1;
									  r_hw.effect_time=giv_sys_time;
									 break;
									 case RIGHT_MID_SIGN:
									  r_hw.effectMidRight=1;
									  r_hw.effect_timeMidRight=giv_sys_time;
									  r_hw.effect=1;
									  r_hw.effect_time=giv_sys_time;
									  break;
									  case NEAR_SIGN:
									   r_hw.effectNear=1;
									   r_hw.effect_timeNear=giv_sys_time;
									   r_hw.effect=1;
									   r_hw.effect_time=giv_sys_time;
									   break;
#ifdef DOCK_NEAR						
									 case 0x55:
										 r_hw.effectNear=1;
										 r_hw.effect_timeNear=giv_sys_time;
										 r_hw.effect=1;
										 r_hw.effect_time=giv_sys_time;
										 break;
#endif
#ifdef REMOTE		
									 default:
										 Parse_Remote_Signa(&r_hw);
										 break;
#endif
 
									 }
							 }
					 }
				 else								 //���������
					 {
						 sample_time=giv_sys_time-r_hw_time;
						 if((sample_time>START_FRAME_MIN)&&(sample_time<START_FRAME_MAX))
							 {
								 r_hw.start=1;
								 r_hw.bitnumber=0;
								 r_hw.tempdata=0;
							 }
					 }
			 }
		 else										 //�½���
			 {
				 r_hw_time=giv_sys_time;
			 }
		 EXTI_ClearITPendingBit(EXTI_Line_RHW);
	 }
	 
	 ///////////////������ж�//////////////////
	 if(EXTI_GetITStatus(EXTI_Line_BHW)!=RESET) 		 
		 {
		 if(Read_B_HW())	 //������
			 {
				   sample_time=giv_sys_time-b_hw_time;
				   if(b_hw.start)
					 {
						 if((sample_time>=BIT0_MIN_SAMPLE)&&(sample_time<=BIT0_MAX_SAMPLE)) 	 //bit '0'
							 {
								 b_hw.tempdata<<=1;
								 b_hw.tempdata|=0;
								 b_hw.bitnumber++;
							 }
						 else if((sample_time>=BIT1_MIN_SAMPLE)&&(sample_time<=BIT1_MAX_SAMPLE))	 //bit '1'
							 {
								 b_hw.tempdata<<=1;
								 b_hw.tempdata|=1;
								 b_hw.bitnumber++;
							 }
#ifdef INTERFERENCE_ACTION
						 //qz add 20180823��������
						 else
							 {
								 b_hw.tempdata=0;
								 b_hw.start=0;
								 b_hw.bitnumber=0;
							 }
#endif
						 if(b_hw.bitnumber==8)
							 {
								 b_hw.data=b_hw.tempdata;
								 b_hw.tempdata=0;
								 b_hw.start=0;
								 switch (b_hw.data)
									 {
										 case RIGHT_SIGN:
											 b_hw.effectRight=1;
											 b_hw.effect_timeRight=giv_sys_time;
											 b_hw.effect=1;
											 b_hw.effect_time=giv_sys_time;
											 break;
										 case LEFT_SIGN:
											 b_hw.effectLeft=1;
											 b_hw.effect_timeLeft=giv_sys_time;
											 b_hw.effect=1;
											 b_hw.effect_time=giv_sys_time;
											 break;
										 case LEFT_MID_SIGN:
											 b_hw.effectMidLeft=1;
											 b_hw.effect_timeMidLeft=giv_sys_time;
											 b_hw.effect=1;
											 b_hw.effect_time=giv_sys_time;
										 break;
										 case RIGHT_MID_SIGN:
											 b_hw.effectMidRight=1;
											 b_hw.effect_timeMidRight=giv_sys_time;
											 b_hw.effect=1;
											 b_hw.effect_time=giv_sys_time;
											 break;
										 case NEAR_SIGN:
											  b_hw.effectNear=1;
											  b_hw.effect_timeNear=giv_sys_time;
											  b_hw.effect=1;
											  b_hw.effect_time=giv_sys_time;
											 break;
#ifdef DOCK_NEAR						
										 case 0x55:
										  b_hw.effectNear=1;
										  b_hw.effect_timeNear=giv_sys_time;
										  b_hw.effect=1;
										  b_hw.effect_time=giv_sys_time;
										 break;
										 
#endif
 
#ifdef REMOTE
										 default:
											 Parse_Remote_Signa(&b_hw);
											 break;
#endif
									 
 
									 }
							 }
					 }
				 else								 //���������
					 {
						 if((sample_time>START_FRAME_MIN)&&(sample_time<START_FRAME_MAX))
							 {
								 b_hw.start=1;
								 b_hw.bitnumber=0;
								 b_hw.tempdata=0;
							 }
					 }
			 }
		 else										 //�½���
			 {
				 b_hw_time=giv_sys_time;
			 }
		 EXTI_ClearITPendingBit(EXTI_Line_BHW);
		 }
 
	 if(EXTI_GetITStatus(EXTI_Line_LMHW)!=RESET)		
		{
			if(Read_LM_HW())	//������
				{
					if(lm_hw.start)
						{
							sample_time=giv_sys_time-mm_hw_time;
							if((sample_time>=BIT0_MIN_SAMPLE)&&(sample_time<=BIT0_MAX_SAMPLE))			//bit '0'
								{
									lm_hw.tempdata<<=1;
									lm_hw.tempdata|=0;
									lm_hw.bitnumber++;
								}
							else if((sample_time>=BIT1_MIN_SAMPLE)&&(sample_time<=BIT1_MAX_SAMPLE)) 	//bit '1'
								{
									lm_hw.tempdata<<=1;
									lm_hw.tempdata|=1;
									lm_hw.bitnumber++;
								}
							if(lm_hw.bitnumber==8)
								{
									lm_hw.data=lm_hw.tempdata;
									lm_hw.tempdata=0;
									lm_hw.start=0;
									switch (lm_hw.data)
										{
											case RIGHT_SIGN:
												lm_hw.effectRight=1;
												lm_hw.effect_timeRight=giv_sys_time;
												lm_hw.effect=1;
												lm_hw.effect_time=giv_sys_time;
												//if(!rm_hw.effectRight)
												 {
													 //rm_hw.effectRight=1;
													 //lm_hw.effect=1;
												 }
												break;
											case LEFT_SIGN:
												lm_hw.effectLeft=1;
												lm_hw.effect_timeLeft=giv_sys_time;
												lm_hw.effect=1;
												lm_hw.effect_time=giv_sys_time;
												//if(!rm_hw.effectLeft)
												 {
													 //rm_hw.effectLeft=1;
													 //lm_hw.effect=1;
												 }
												break;
												
											case LEFT_MID_SIGN:
												lm_hw.effectMidLeft=1;
												lm_hw.effect_timeMidLeft=giv_sys_time;
												lm_hw.effect=1;
												lm_hw.effect_time=giv_sys_time;
												 //if(!rm_hw.effectLeft)
												 {
													 //rm_hw.effectLeft=1;
													 //lm_hw.effect=1;
												 }
											case RIGHT_MID_SIGN:
												lm_hw.effectMidRight=1;
												lm_hw.effect_timeMidRight=giv_sys_time;
												lm_hw.effect=1;
												lm_hw.effect_time=giv_sys_time;
											break;
 
											case NEAR_SIGN:
												 lm_hw.effectNear=1;
												 lm_hw.effect_timeNear=giv_sys_time;
												 lm_hw.effect=1;
												 lm_hw.effect_time=giv_sys_time;
												break;
										}
								}
						}
					else								//���������
						{
							sample_time=giv_sys_time-mm_hw_time;
							if((sample_time>START_FRAME_MIN)&&(sample_time<START_FRAME_MAX))
								{
									lm_hw.start=1;
									lm_hw.bitnumber=0;
									lm_hw.tempdata=0;
								}
						}
				}
			else										//�½���
				{
					mm_hw_time=giv_sys_time;
				}
			EXTI_ClearITPendingBit(EXTI_Line_LMHW);
		}

	///////////������ж�////////////////
	if(EXTI_GetITStatus(EXTI_Line_LHW)!=RESET)				
	   {
		   if(Read_L_HW())	   //������
			   {
					 sample_time=giv_sys_time-l_hw_time;
					 if(l_hw.start)
					   {
						   if((sample_time>=BIT0_MIN_SAMPLE)&&(sample_time<=BIT0_MAX_SAMPLE))		   //bit '0'
							   {
								   l_hw.tempdata<<=1;
								   l_hw.tempdata|=0;
								   l_hw.bitnumber++;
							   }
						   else if((sample_time>=BIT1_MIN_SAMPLE)&&(sample_time<=BIT1_MAX_SAMPLE))	   //bit '1'
							   {
								   l_hw.tempdata<<=1;
								   l_hw.tempdata|=1;
								   l_hw.bitnumber++;
							   }
#ifdef INTERFERENCE_ACTION
						   //qz add 20180823��������
						   else
							   {
								   l_hw.tempdata=0;
								   l_hw.start=0;
								   l_hw.bitnumber=0;
							   }
#endif
						   if(l_hw.bitnumber==8)
							   {
								   l_hw.data=l_hw.tempdata;
								   l_hw.tempdata=0;
								   l_hw.start=0;
								   switch (l_hw.data)
									   {
										   case RIGHT_SIGN:
											l_hw.effectRight=1;
											l_hw.effect_timeRight=giv_sys_time;
										   	l_hw.effect=1;
										   	l_hw.effect_time=giv_sys_time;
											break;
										   case LEFT_SIGN:
											l_hw.effectLeft=1;
											l_hw.effect_timeLeft=giv_sys_time;
										   	l_hw.effect=1;
										   	l_hw.effect_time=giv_sys_time;
											break;
										   case LEFT_MID_SIGN:
											l_hw.effectMidLeft=1;
											l_hw.effect_timeMidLeft=giv_sys_time;
										   	l_hw.effect=1;
										   	l_hw.effect_time=giv_sys_time;
										   break;
										   case RIGHT_MID_SIGN:
										   	l_hw.effectMidRight=1;
											l_hw.effect_timeMidRight=giv_sys_time;
										    l_hw.effect=1;
											l_hw.effect_time=giv_sys_time;
											break;
											case NEAR_SIGN:
											 l_hw.effectNear=1;
											 l_hw.effect_timeNear=giv_sys_time;
											 l_hw.effect=1;
											 l_hw.effect_time=giv_sys_time;
											 break;
#ifdef DOCK_NEAR						
										 case 0x55:
											 l_hw.effectNear=1;
											 l_hw.effect_timeNear=giv_sys_time;
											 l_hw.effect=1;
											 l_hw.effect_time=giv_sys_time;
										 break;
#endif
#ifdef REMOTE		
										default:
											Parse_Remote_Signa(&l_hw);
										break;
#endif

									   }
							   }
					   }
				   else 							   //���������
					   {
						   if((sample_time>START_FRAME_MIN)&&(sample_time<START_FRAME_MAX))
							   {
								   l_hw.start=1;
								   l_hw.bitnumber=0;
								   l_hw.tempdata=0;
							   }
					   }
			   }
		   else 									   //�½���
			   {
				   l_hw_time=giv_sys_time;
			   }
		   EXTI_ClearITPendingBit(EXTI_Line_LHW);
	   }


#ifndef MHW_CAPTURE
 if(EXTI_GetITStatus(EXTI_Line_RMHW)!=RESET)			
 	{
 		if(Read_RM_HW())		//������
 			{
 				if(rm_hw.start)
 					{
 						sample_time=giv_sys_time-rm_hw_time;
						if((sample_time>=BIT0_MIN_SAMPLE)&&(sample_time<=BIT0_MAX_SAMPLE))			//bit '0'
							{
								rm_hw.tempdata<<=1;
								rm_hw.tempdata|=0;
								rm_hw.bitnumber++;
							}
						else if((sample_time>=BIT1_MIN_SAMPLE)&&(sample_time<=BIT1_MAX_SAMPLE))		//bit '1'
							{
								rm_hw.tempdata<<=1;
								rm_hw.tempdata|=1;
								rm_hw.bitnumber++;
							}

#ifdef INTERFERENCE_ACTION
						//qz add 20180823��������
						else
							{
								rm_hw.tempdata=0;
								rm_hw.start=0;
								rm_hw.bitnumber=0;
							}
#endif
						if(rm_hw.bitnumber==8)
							{
								rm_hw.data=rm_hw.tempdata;
								rm_hw.tempdata=0;
								rm_hw.start=0;
								switch (rm_hw.data)
									{
									case RIGHT_SIGN:
									 rm_hw.effectRight=1;
									 rm_hw.effect_timeRight=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									 break;
									case LEFT_SIGN:
									 rm_hw.effectLeft=1;
									 rm_hw.effect_timeLeft=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									 break;
									case LEFT_MID_SIGN:
									 rm_hw.effectMidLeft=1;
									 rm_hw.effect_timeMidLeft=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									break;
									case RIGHT_MID_SIGN:
									 rm_hw.effectMidRight=1;
									 rm_hw.effect_timeMidRight=giv_sys_time;
									 rm_hw.effect=1;
									 rm_hw.effect_time=giv_sys_time;
									 break;
									 case NEAR_SIGN:
									  rm_hw.effectNear=1;
									  rm_hw.effect_timeNear=giv_sys_time;
									  rm_hw.effect=1;
									  rm_hw.effect_time=giv_sys_time;
									  break;
#ifdef DOCK_NEAR						
										case 0x55:
											rm_hw.effectNear=1;
											rm_hw.effect_timeNear=giv_sys_time;
											rm_hw.effect=1;
											rm_hw.effect_time=giv_sys_time;
										break;
#endif
#ifdef REMOTE		
										default:
											Parse_Remote_Signa(&rm_hw);
										break;
#endif

									}
							}
 					}
				else								//���������
					{
						sample_time=giv_sys_time-rm_hw_time;
						if((sample_time>START_FRAME_MIN)&&(sample_time<START_FRAME_MAX))
							{
								rm_hw.start=1;
								rm_hw.bitnumber=0;
								rm_hw.tempdata=0;
							}
					}
 			}
		else										//�½���
			{
				rm_hw_time=giv_sys_time;
			}
		EXTI_ClearITPendingBit(EXTI_Line_RMHW);
 	}
#endif
#endif

	if(EXTI_GetFlagStatus(EXTI_Line10)!=RESET)			//����ײ
		{
			EXTI_ClearITPendingBit(EXTI_Line10);
			key_wakeup_flag=true;
		}
}
//======================================================================================
//======================================================================================











 



//======================================================================================
//======================================================================================
void EXTI9_5_IRQHandler(void)
{
	u8 i=0;

	 /////////////����ײ�ж�///////////////
	if(EXTI_GetFlagStatus(EXTI_Line_LBUMP)!=RESET)			//����ײ
		{
			EXTI_ClearITPendingBit(EXTI_Line_LBUMP);
				{
#ifdef RING_PWM_CTL
					i=0;
					while(i<10)
						{
							i++;
						}
					if(!Read_L_Bump())
						{
							bump_value|=L_BUMP_BIT;
							l_bump.key=0;
						}
					if(!Read_R_Bump())
						{
							bump_value|=R_BUMP_BIT;
							r_bump.key=0;
						}
#if 0
					if(!Read_RM_Bump())
						{
							bump_value|=RM_BUMP_BIT;
							rm_bump.key=0;
						}
					if(!Read_LM_Bump())
						{
	
							bump_value|=LM_BUMP_BIT;
							lm_bump.key=0;
						}
#endif
					if(bump_value)
						{
							Close_Bump_Exit();
						}
#else
					for(i=0;i<3;i++)
						{
							if(!Read_L_Bump())
							{
								if(i==2)
								{
									bump_value|=L_BUMP_BIT;
									l_bump.key=0;
								}
							}
							else
							{
								break;
							}
						}
#endif
				}
		}

	 ////////////����ײ�ж�///////////////////
	 if(EXTI_GetFlagStatus(EXTI_Line_RBUMP)!=RESET) 		//����ײ
		{
			EXTI_ClearITPendingBit(EXTI_Line_RBUMP);
				{
#ifdef RING_PWM_CTL
					i=0;
					while(i<10)
						{
							i++;
						}
					if(!Read_L_Bump())
						{
							bump_value|=L_BUMP_BIT;
							l_bump.key=0;
						}
					if(!Read_R_Bump())
						{
							bump_value|=R_BUMP_BIT;
							r_bump.key=0;
						}
#if 0
					if(!Read_RM_Bump())
						{
							bump_value|=RM_BUMP_BIT;
							rm_bump.key=0;
						}
					if(!Read_LM_Bump())
						{
	
							bump_value|=LM_BUMP_BIT;
							lm_bump.key=0;
						}
#endif
					if(bump_value)
						{
							Close_Bump_Exit();
						}
#else
					for(i=0;i<3;i++)
						{
							if(!Read_R_Bump())
							{
								if(i==2)
								{
									bump_value|=R_BUMP_BIT;
									r_bump.key=0;
								}
							}
							else
							{
								break;
							}
						}
#endif
				}
		}
}
//======================================================================================
//======================================================================================




















//==================================================================================
/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
//uint32 alr;
  if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
		{
	 // 	  ExitInput	= TRUE;
				RTC_ClearITPendingBit(RTC_IT_ALR);
				EXTI_ClearITPendingBit(EXTI_Line17);
		}
}
//==================================================================================

void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC)!=RESET)
		{
			RTC_GetCounter();
			RTC_ClearITPendingBit(RTC_IT_SEC);
		}			
}

#ifdef TUYA_WIFI
void USART1_IRQHandler(void)
{
#if 1
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET) //
	{ 
#ifdef TUYA_WIFI
		uart_receive_input((uint8_t)USART_ReceiveData(USART1));
#endif
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	/* ��� */
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)  
		{ 
			USART_ClearFlag(USART1,USART_FLAG_ORE); //��SR 
			USART_ReceiveData(USART1); //��DR 
		} 
#else
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_IDLE)!=RESET)
		{
			USART_ClearITPendingBit(USART1, USART_IT_IDLE);
			Res=USART1->SR;
			Res=USART1->DR;
			Res=Res;
			DMA_Cmd(DMA1_Channel5,DISABLE);
			//	UART1.RevLength=USART1_RX_SIZE;
			//	UART1.RevLength=DMA_GetCurrDataCounter(DMA1_Channel5);
			UART1.RevLength=USART1_RX_SIZE-DMA_GetCurrDataCounter(DMA1_Channel5);
			DMA_SetCurrDataCounter(DMA1_Channel5,USART1_RX_SIZE);

			DMA_Cmd(DMA1_Channel5, DISABLE);
			UART1.Rece_Done=true;	
		}
#endif
}
#endif

void USART3_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_IDLE)!=RESET)
		{
			USART_ClearITPendingBit(USART3, USART_IT_IDLE);
			Res=USART3->SR;
			Res=USART3->DR;
			Res=Res;
			DMA_Cmd(DMA1_Channel3,DISABLE);
			//UART2.RevLength=23-DMA_GetCurrDataCounter(DMA1_Channel6);
			UART3.RevLength=USART3_RX_SIZE-DMA_GetCurrDataCounter(DMA1_Channel3);
			DMA_SetCurrDataCounter(DMA1_Channel3,USART3_RX_SIZE);

			DMA_Cmd(DMA1_Channel3, DISABLE);
			UART3.Rece_Done=true;
		}
}

//ADC DMA��������ж�
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))
		{
			ADC_Cmd(ADC1,DISABLE);
			ADC_SoftwareStartConvCmd(ADC1, DISABLE);  
			DMA_Cmd(DMA1_Channel1, DISABLE);


			DMA_SetCurrDataCounter(DMA1_Channel1, 256);
			ADC_Cmd(ADC1,ENABLE);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			DMA_Cmd(DMA1_Channel1,ENABLE);
			DMA_ClearITPendingBit(DMA1_IT_TC1);
			
		}
}

//DMA1_Channel4��USART1��DMA�����жϺ���  QZ ADD
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_FLAG_TC4))
		{
			//DMA_ClearFlag(DMA1_FLAG_TC4);
			DMA_ClearITPendingBit(DMA1_FLAG_TC4);
			DMA_Cmd(DMA1_Channel4,DISABLE);
			UART1.Trans_Busy=false;
		}
}

//DMA1_Channel7��USART2��DMA�����жϺ���  QZ ADD
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_FLAG_TC7))
		{
			//DMA_ClearFlag(DMA1_FLAG_TC4);
			DMA_ClearITPendingBit(DMA1_FLAG_TC7);
			DMA_Cmd(DMA1_Channel7,DISABLE);
			UART1.Trans_Busy=false;
		}
}


