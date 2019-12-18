////////////////////////ͷ�ļ�////////////////////////////////////
#include "hwincept.h"
#include "hardware.h"
#include "systime.h"

#define CLEAN_EFFECT_TIME 3000			//qz modify 20181215 2000--->3000

////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
INFR_DATA l_hw,r_hw,rm_hw,lm_hw,lb_hw,rb_hw;
bool Action_hwincept;

u16 top_time,top_time_sec;

///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////
void  init_hwincept(void);
void read_hw_data(INFR_DATA *hw_struct);
void clr_hw_effect(INFR_DATA *hw_struct);  
void enable_hwincept(void);
void disable_hwincept(void);

///////////////////////˽�к���////////////////////////////////////
void clr_hw_struct(INFR_DATA *hw_struct);
void read_hw_bit(INFR_DATA *hw_struct);		
void read_dummy_wall_bit(INFR_DATA *hw_struct);

///////////////////////����ʵ��////////////////////////////////////
/******************************************************************
���ܣ���ʼ���������
******************************************************************/
void  init_hwincept(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
#ifndef HW_INT_REC
	l_hw.read_io 				= Read_L_HW;
	r_hw.read_io 				= Read_R_HW;
	rm_hw.read_io 				= Read_M_HW;


#else
#if 0
	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruture.GPIO_Pin=L_HW|R_HW;
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruture);

	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruture.GPIO_Pin=B_HW;
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruture);
#endif
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStructure);

/*--------------------------------------------
ע�⣺�Һ�����ж�ԴEXTI15_10_IRQn�Ѿ�����ײ������ʹ�ܣ�
�ʴ˴�����Ҫ��ʹ��
----------------------------------------------*/
#if 1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource10);		//������
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);		//��ǰ����
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);		//�����
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource13);		//�Һ���
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource15);		//�Һ����
#endif
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource14);		//��ǰ����

	EXTI_InitStructure.EXTI_Line=EXTI_Line_RMHW|EXTI_Line_LHW|EXTI_Line_LMHW|EXTI_Line_RHW|EXTI_Line_LBHW|EXTI_Line_RBHW;		//�����,������ж�
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line_RHW);			//�Һ���
	EXTI_ClearITPendingBit(EXTI_Line_LHW);			//�Һ���
	EXTI_ClearITPendingBit(EXTI_Line_RMHW);			//�Һ���
	EXTI_ClearITPendingBit(EXTI_Line_RBHW);			//�Һ���
	EXTI_ClearITPendingBit(EXTI_Line_LMHW);			//�Һ���
	EXTI_ClearITPendingBit(EXTI_Line_LBHW);			//�Һ���
#endif 

  
  	clr_all_hw_struct();
	//qz add 20180817
	l_hw.rece_ir=RECE_LEFT;
	rm_hw.rece_ir=RECE_MID;
	lm_hw.rece_ir=RECE_MID;
	r_hw.rece_ir=RECE_RIGHT;
	//qz add end

	disable_hwincept();
}

#if 0
u8 Check_Start(INFR_DATA *hw_struct)
{
	  u32 t;
	  u8 bit;
	
	  t = giv_sys_time - hw_struct->time;
	
	  if(hw_struct->read_io() == Bit_RESET)  //����ǰ�ĺ���״̬
			{
					bit = 0;
			}
	  else
			{
					bit = 1;
			}
	  //////////////////////����ǰ����//////////////////////////  
	  if(t > 41)	//QZ ADD:3.7ms
				{
					hw_struct->start = 0;
					hw_struct->tempbit=0;
					hw_struct->time=giv_sys_time;
					return 0;
				}
	  switch(t)
	  {
			
		 	case 1:		//ǰ�����һ�β���	QZ
			 case 5:	//ǰ����ڶ��β���	QZ		  
			 case 10:	//�������һ�β���	QZ
			 case 15:	//������ڶ��β���	QZ
			 case 20:	//����������β���	QZ
			 case 25:	//��������Ĵβ���	QZ
			 		hw_struct->tempbit+=bit;
			 break;
//			 case 28:
			 case 29:	//�������һ�β���	QZ,2.9ms
					 hw_struct->tempbit += bit;
			 		if(hw_struct->tempbit>2)
					{
//						hw_struct->tempbit=0;
						hw_struct->time=giv_sys_time;
//						return 0;
					}
					hw_struct->tempbit=0;
					 break;
			 case 32:	//ǰ��������β���	QZ	,0.6ms
			 case 34:	 //���������β���  QZ,2.6ms
			 case 35:
			 case 36:
			 case 37:
			 case 38:
			 		hw_struct->tempbit+=bit;
//					return 0;
			 break;
//			 case 39:
			 case 40:
			 		hw_struct->tempbit+=bit;
			 		if(hw_struct->tempbit<4)
					{
						hw_struct->tempbit=0;
						hw_struct->time=giv_sys_time;
						return 0;
					}
					else
						return 1;
	
	}
	return 0;
}
/******************************************************************
���ܣ���ȡһ���ֽں�������
���룺�������ݽṹ
������ɹ������ں���ṹ�����ñ�־
���أ���
˵�������������жϺ�������
******************************************************************/
//u32 g=0;
void read_hw_data(INFR_DATA *hw_struct)
{

  if(hw_struct->start == 0)   
		{
			
				if(hw_struct->read_io() == Bit_RESET)  //��ʼ��������λ
          {
              hw_struct->start 			= 1; //�������͵�ƽ
              hw_struct->time 			= giv_sys_time;
              hw_struct->bitnumber 	= 0x80;
              hw_struct->tempbit 		= 0; 
              hw_struct->signTop = 0; 
          }
		}
  else
		{
				//read_hw_bit(hw_struct);
				read_hw_bit_my(hw_struct);
		}
		
//		if(hw_struct->read_io() == Bit_RESET)
//		{
//		  hw_struct->effectSignal = 1;
//      hw_struct->effect_timeSignal = giv_sys_time;				
//		}

}
/******************************************************************
���ܣ���ȡһ���ֽں�������
���룺�������ݽṹ
������ɹ������ں���ṹ�����ñ�־
���أ���
˵�������������жϺ�������
******************************************************************/
void read_dummy_wall(INFR_DATA *hw_struct)
{		   
  if(hw_struct->start == 0)      //
		{
				if(hw_struct->read_io() == Bit_RESET)  //��ʼ��������λ
					{
							hw_struct->start 			= 1;
//							hw_struct->time 			= time1_50us;
							hw_struct->bitnumber 	= 1;
							hw_struct->tempbit 		= 0;
					}
		}
  else
		{
				read_dummy_wall_bit(hw_struct) ;
		}
  
}
/*********************************************************************
���ܣ���ȡһλ��������
���룺�������ݽṹ
������ɹ������ں���ṹ�����ñ�־
���أ���
˵������������read_hw_data����,��ǰ����ͽ��������3�Σ����������5��
*********************************************************************/
//u32 d=0;
//u32 f=0;
//u32 o=0;
//u32 t=0;

//u32 l=0;
//u32 r=0;
//u32 m=0;
//u32 d=0;
/**********************
������������
1������ʱΪ��
2��ǰ�����ǵ�3ms����1ms
3������0�ǵ�1.5ms����0.75ms
4��������������ԽԶ�ĵط��ź�Խ������Ϊ���մ�Ӳ����ԭ��
   �͵�ƽ��ߵ�ƽ����խ�������λ��խ������֮ǰ�ķ���������
***********************/
//u32 c=0,e=0,f=0;
void read_hw_bit(INFR_DATA *hw_struct)
{  
  //����ǰ�ĺ���״̬

  if(hw_struct->read_io() == Bit_RESET)  
		{    
      hw_struct->tempbit ++; 
 
      //��֤�ɹ��������м�������ͷ�Ƚ�
      //���յ������ֽڵ����ݣ����Լ����
      //�½������ж��ź�
      /*********************************/
      if(hw_struct->Cnt_fallRise == 0)
      {
        hw_struct->Cnt_fallRise =1; 
      }  
////      else if(hw_struct->Cnt_fallRise == 2)
////      {
////        hw_struct->Cnt_fallRise = 3;
////      }

      /*********************************/
      
		}
  else//��
		{   
      
      /*********************************/
      if(hw_struct->Cnt_fallRise == 1)
      {
        hw_struct->Cnt_fallRise =0;
        hw_struct->effectSignal = 1;
        hw_struct->effect_timeSignal = giv_sys_time;
      }
      /*********************************/
      
      if(hw_struct->signTop	== 0)
      {
        if(hw_struct->tempbit > 22) 
        {         
           hw_struct->signTop 				= 1; 
          //���ۺ�ʵ���ϣ������͵�ƽ�ȶ���ǰ4λ�Ŀ�����Ҫ
          //�ߣ�����������4��������ֻ�����͵�ƽ�����ɿ�����Ϊ
          //̫����Ҳ���������Ƶ�ʵĺ��⣬�������У���ȡһλ����λ
          //����,����Ϊ�Ƕ���������ź�
        }
        else 
        {
          hw_struct->start 				= 0;         
        } 
        hw_struct->tempbit =0;       
        return; //û�вɵ�ǰ���룬������������������
      } 
      //�ɵ�ǰ�����Ҫô�Ǹ�Ҫô�ǵͣ��;�++,�߾ͽ�����
      if(hw_struct->signTop ==1)
      { //�ɵ��������ɵ��ߵ�ƽ
        hw_struct->signTop =2;
        //ֻҪ�Ǹߵ�ƽ��һֱ����hw_struct->tempbit =0;
        //�͵�ƽ��һֱ��
      }  
      else if(hw_struct->signTop ==2)
      {
        if((hw_struct->tempbit >= 8)&&(hw_struct->tempbit <= 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//��ǰ�����ǵ�
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++; 
        }
        else if(hw_struct->tempbit == 0)
        {
          //ʲô������
        }
        else
        {
          hw_struct->start 				= 0;  
        }         
      }
      else if(hw_struct->signTop ==3)
      {
        if((hw_struct->tempbit >= 2)&&(hw_struct->tempbit <= 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//��ǰ�����Ǹ�
           hw_struct->bitnumber 	>>= 1;           
           hw_struct->signTop++;  
           hw_struct->effect_timeTop = giv_sys_time;
           hw_struct->effectTop 			= 1;          
        }  
        else if(hw_struct->tempbit == 0)
        {
          //ʲô������
        }
        else
        {
          hw_struct->start 				= 0;  
        }                
      }      
      else if(hw_struct->signTop ==4)
      {
        if((hw_struct->tempbit >= 8)&&(hw_struct->tempbit <=15))
        {
          hw_struct->tempdata &= ~hw_struct->bitnumber;//��ǰ�����ǵ�
          hw_struct->bitnumber >>= 1;          
          hw_struct->signTop++; 
        }
        else if(hw_struct->tempbit == 0)
        {
          //ʲô������
        }
        else
        {
          hw_struct->start 				= 0;  
        }         
      }
      else if(hw_struct->signTop ==5)
      {
        if((hw_struct->tempbit >= 2)&&(hw_struct->tempbit <= 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//��ǰ�����Ǹ�
           hw_struct->bitnumber 	>>= 1;          
           hw_struct->signTop++; 
        }
        else if(hw_struct->tempbit == 0)
        {
          //ʲô������
        }        
        else
        {
          hw_struct->start 				= 0;  
        }         
      }
      /************************************************/      
      //ǰ����λ��0101���κ�һλ���˶����²���          
      /************************************************/
      else if(hw_struct->signTop ==6)
      {
        if((hw_struct->tempbit > 8)&&(hw_struct->tempbit < 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//��ǰ�����ǵ�
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++;
        }
        else if((hw_struct->tempbit > 2)&&(hw_struct->tempbit < 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//��ǰ�����Ǹ�
           hw_struct->bitnumber 	>>= 1; 
           hw_struct->signTop++;          
        } 
        else if(hw_struct->tempbit == 0)
        {
          //ʲô������
        }
        else//����������¶�ȡ
        {
          hw_struct->start 				= 0;  
        }           
      }
      else if(hw_struct->signTop ==7)
      {
        if((hw_struct->tempbit > 8)&&(hw_struct->tempbit < 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//��ǰ�����ǵ�
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++; 
        }
        else if((hw_struct->tempbit > 2)&&(hw_struct->tempbit < 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//��ǰ�����Ǹ�
           hw_struct->bitnumber 	>>= 1;
           hw_struct->signTop++; 
        } 
        else if(hw_struct->tempbit == 0)
        {
          //ʲô������
        }
        else//����������¶�ȡ
        {
          hw_struct->start 				= 0;  
        }                       
      }
      else if(hw_struct->signTop ==8)
      {
        if((hw_struct->tempbit > 8)&&(hw_struct->tempbit < 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//��ǰ�����ǵ�
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++; 
        }
        else if((hw_struct->tempbit > 2)&&(hw_struct->tempbit < 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//��ǰ�����Ǹ�
           hw_struct->bitnumber 	>>= 1;
           hw_struct->signTop++; 
        } 
        else if(hw_struct->tempbit == 0)
        {
          //ʲô������
        }
        else//����������¶�ȡ
        {
          hw_struct->start 				= 0;  
        }        
      }
      else if(hw_struct->signTop ==9)
      {
        if((hw_struct->tempbit > 8)&&(hw_struct->tempbit < 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//��ǰ�����ǵ�
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++; 
        }
        else if((hw_struct->tempbit > 2)&&(hw_struct->tempbit < 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//��ǰ�����Ǹ�
           hw_struct->bitnumber 	>>= 1;
           hw_struct->signTop++; 
        } 
        else if(hw_struct->tempbit == 0)
        {
          //ʲô������
        }
        else//����������¶�ȡ
        {
          hw_struct->start 				= 0;  
        }        
      }      

      if(hw_struct->bitnumber == 0x00)	//������8λ���ݣ�ǿ�ƽ�����ϣ�׼����һ�ֽ��� QZ
      {
          hw_struct->bitnumber 		= 0x80;  //�ִӿ�ʼ��һλ�� 
          hw_struct->start 				= 0;  //�����ʼ��־.          
          hw_struct->data 				= hw_struct->tempdata;//�����ݴ��������
				  hw_struct->effect       = 1;
				  if(hw_struct->data == 0x5c)
					{//��					
            			hw_struct->effectTopReal = 1;
						hw_struct->effect_timeTopReal 	= giv_sys_time;
					}
					else if(hw_struct->data == 0x53)
					{//��
            			hw_struct->effectMid =1;
						hw_struct->effect_timeMid 	= giv_sys_time;							
					}
					else if(hw_struct->data == 0x5b)
					{//��
            			hw_struct->effectRight = 1;
						hw_struct->effect_timeRight 	= giv_sys_time;
					}
					else if(hw_struct->data == 0x57)
					{//��
            			hw_struct->effectLeft = 1;
						hw_struct->effect_timeLeft 	= giv_sys_time;
					}
          hw_struct->sign 				= 1;
          hw_struct->effect_time 	= giv_sys_time;
					
//					if(hw_struct->data != 0x5c)
//					{
//						hw_struct->effect 			= 1;	//
//					}
          
          hw_struct->signTop = 0;
      } 
      hw_struct->tempbit =0;
		}
}	

void read_hw_bit_my(INFR_DATA *hw_struct)
{
	u32 t;
	u8 bit;

	t = giv_sys_time - hw_struct->time;

	if(hw_struct->read_io() == Bit_RESET)  //����ǰ�ĺ���״̬
			{
					bit = 0;
			}
	else
			{
					bit = 1;
			}
	//////////////////////����ǰ����//////////////////////////  
	if(t > 37)	  //QZ ADD:3.7ms
				{
					hw_struct->start = 0;
					return;
				}
	switch(t)
	{
		 case 1:		  //ǰ�����һ�β���  QZ
		 case 3:	  //ǰ����ڶ��β���  QZ		
		 case 8:	  //�������һ�β���  QZ
		 case 10:   //������ڶ��β���  QZ
		 case 11:   //����������β���  QZ
		 case 12:   //��������Ĵβ���  QZ
		 case 16:   //�������һ�β���  QZ,2.9ms
		 case 18:   //������ڶ��β���  QZ
				 hw_struct->tempbit += bit;
				 break;
		 case 6:	  //ǰ��������β���  QZ  ,0.6ms
					hw_struct->tempbit += bit;
					if(hw_struct->tempbit > 1)  //ǰ���������θ� QZ
						 {
								 hw_struct->start = 0; //������ձ�־ QZ
						 }
					hw_struct->tempbit = 0; //���tempbit��׼����һ�׶�ʹ��  QZ
				 break;
			 
		 case 14:    //���������β���  QZ,2.6ms
				 hw_struct->tempbit += bit;
				 if(hw_struct->tempbit > 3)  //�����볬��3�θ�
						 {
								 hw_struct->tempdata |= hw_struct->bitnumber;//��ǰ�����Ǹ�
						 }
				 else
						 {
								if(hw_struct->tempbit < 2)  //�����볬��3�ε�
									 {
											 hw_struct->tempdata &=  ~hw_struct->bitnumber;//��ǰ�����ǵ�
									 }
								else
									 {
											 ////////�ж����Ӳ�����,Ҫ���¿�ʼ
											hw_struct->start = 0; //������ձ�־
									 }
						 }
				 hw_struct->tempbit = 0;	  //���tempbit��׼����һ�׶�ʹ��  QZ
				 break;
						
		 case 21: 	 //����������β���  QZ
				 hw_struct->tempbit += bit; 
				 if(hw_struct->tempbit < 2)  //�����������ε�
						 {
								 hw_struct->start = 0; //������ϣ�������ձ�־ QZ
						 }
				 else
						 {
								 if( hw_struct->bitnumber == 0x80)  //������8λ���ݣ�ǿ�ƽ�����ϣ�׼����һ�ֽ��� QZ
										 {
												hw_struct->bitnumber		  = 1;	//�ִӿ�ʼ��һλ�� 
												hw_struct->start				  = 0;	 //�����ʼ��־.			
												hw_struct->data				  = hw_struct->tempdata;//�����ݴ��������
												hw_struct->sign				  = 1;
												hw_struct->effect_time  = giv_sys_time;
												hw_struct->effect 		  = 1;
								
										 } 
						 }
				 hw_struct->tempbit = 0;	  //���tempbit��׼����һ�׶�ʹ��  QZ
				 break;
		 
		 case 22:
		 case 23: 
//		 case 37:
				 if(bit == 0)   //���λΪ0,һλ���ݽ������	QZ
						 {
									hw_struct->bitnumber	  = (hw_struct->bitnumber << 1);
									hw_struct->time			  = giv_sys_time;
						 }
				 break;

	}
}



/*********************************************************************
���ܣ���ȡһλ��������
���룺�������ݽṹ
������ɹ������ں���ṹ�����ñ�־
���أ���
˵������������read_hw_data����,��ǰ����ͽ��������3�Σ����������5��
*********************************************************************/
void read_dummy_wall_bit(INFR_DATA *hw_struct)
{
  u32 t;
  u8 bit;
	
  t = giv_sys_time - hw_struct->time;
  if(hw_struct->read_io() == Bit_RESET)  //����ǰ�ĺ���״̬
			{
					bit = 0;
			}
  else
			{
					bit = 1;
			}
  //////////////////////����ǰ����//////////////////////////  
  if(t > 22)
			{
				hw_struct->start = 0;
				return;
			}
		switch(t)
      {			 
        case 1:
        case 2:
        case 3:
	      case 4:
        case 5:  	  
	      case 6:
				case 7:  
				case 11:
				case 12:
				case 13:
	      case 14:
        case 15:  	  
	      case 16:
        case 17:
	          hw_struct->tempbit += bit;
	          break;
	      case 8:  	 	 
              hw_struct->tempbit += bit;
	          if(hw_struct->tempbit > 1)  //ǰ���������θ�
	          {
	              hw_struct->start = 0; //������ձ�־
	          }
	          hw_struct->tempbit = 0;
	          break;
	   
	      case 18:  		 
              hw_struct->tempbit += bit;
	          if(hw_struct->tempbit < 7)  //�����볬��3�θ�
	          {
	              hw_struct->start = 0; //������ձ�־
	          }
	          hw_struct->tempbit = 0;
	          break;
	      case 19:	 
	      case 20:	
	      case 21:	
	          if(t == 0)
								{
									 hw_struct->bitnumber++;
									hw_struct->time = giv_sys_time;
									if(hw_struct->bitnumber > 5)
											{
														hw_struct->start 				= 1;   	//	�����ʼ��־.     
														hw_struct->data 				= 0xdc;		//	�����ݴ��������  
														hw_struct->sign 				= 1; 		   
														hw_struct->effect_time 	= giv_sys_time;
														hw_struct->effect 			= 1;
														hw_struct->bitnumber 		= 1;
											}
								}	   	
	          hw_struct->tempbit = 0;
	          break;
      }
 				
} 
#endif
/***************************************************************************
����: �������ṹ������
***************************************************************************/
void clr_hw_struct(INFR_DATA *hw_struct)
{
  hw_struct->tempbit = 0;
  hw_struct->bitnumber = 0;
  hw_struct->effect = 0;
  hw_struct->effect_time = giv_sys_time;
  hw_struct->sign = 0;
  hw_struct->start = 0;
  hw_struct->data = 0;
  hw_struct->time = giv_sys_time;
  hw_struct->tempdata = 0;

  //qz add 20180423
  hw_struct->effectLeft=0;
  hw_struct->effect_timeLeft=giv_sys_time;
  hw_struct->effectRight=0;
  hw_struct->effect_timeRight=giv_sys_time;
  hw_struct->effectMid=0;
  hw_struct->effect_timeMid=giv_sys_time;
  hw_struct->effectTop=0;
  hw_struct->effect_timeTop=giv_sys_time;
  hw_struct->flag=0;

  hw_struct->effectNear=0;
  hw_struct->effect_timeNear=giv_sys_time;
  //qz add end

  //qz add 20181126
  hw_struct->effectMidLeft=0;
  hw_struct->effect_timeMidLeft=giv_sys_time;
  hw_struct->effectMidRight=0;
  hw_struct->effect_timeMidRight=giv_sys_time;

  hw_struct->rece_cnt=0;
  //qz add end
}

void clr_all_hw_struct(void)
{
	clr_hw_struct(&l_hw);
	clr_hw_struct(&r_hw);
	clr_hw_struct(&rm_hw);
	clr_hw_struct(&lm_hw);
	clr_hw_struct(&lb_hw);
	clr_hw_struct(&rb_hw);
}

void clr_all_hw_effect(void)
{
	clr_hw_effect(&l_hw);
	clr_hw_effect(&rm_hw);
	clr_hw_effect(&r_hw); 
	clr_hw_effect(&lm_hw);
	clr_hw_effect(&rb_hw); 
	clr_hw_effect(&lb_hw);
}
/***************************************************************************
����:����������Ч��
����:��
���:��
˵��:���������Чʱ����100ms���ڴ�ʱ����û�����յ��������Ч��־
***************************************************************************/
void clr_hw_effect(INFR_DATA *hw_struct)
{
	if((hw_struct->effect == 1) && ((giv_sys_time - hw_struct->effect_time) > CLEAN_EFFECT_TIME))
		{
				hw_struct->effect = 0;	
		}
    
   //hzh
	if((hw_struct->effectTop == 1) && ((giv_sys_time - hw_struct->effect_timeTop) > CLEAN_EFFECT_TIME))
		{
				hw_struct->effectTop = 0;	
		}   

   //hzh
	if((hw_struct->effectLeft == 1) && ((giv_sys_time - hw_struct->effect_timeLeft) > CLEAN_EFFECT_TIME))
		{
				hw_struct->effectLeft = 0;	
		} 
		
   //hzh
	if((hw_struct->effectRight == 1) && ((giv_sys_time - hw_struct->effect_timeRight) > CLEAN_EFFECT_TIME))
		{
				hw_struct->effectRight = 0;	
		} 
		
   //hzh   
	if((hw_struct->effectMid == 1) && ((giv_sys_time - hw_struct->effect_timeMid) > CLEAN_EFFECT_TIME))
		{
				hw_struct->effectMid = 0;	
		}
	if((hw_struct->effectNear == 1) && ((giv_sys_time - hw_struct->effect_timeNear) > CLEAN_EFFECT_TIME))
		{
				hw_struct->effectNear = 0;	
		}
	//qz add 20181126
	if((hw_struct->effectMidLeft == 1) && ((giv_sys_time - hw_struct->effect_timeMidLeft) > CLEAN_EFFECT_TIME))
		{
				hw_struct->effectMidLeft = 0;	
		}
	if((hw_struct->effectMidRight == 1) && ((giv_sys_time - hw_struct->effect_timeMidRight) > CLEAN_EFFECT_TIME))
		{
				hw_struct->effectMidRight = 0;	
		}
	//qz add end
}
/***************************************************************************
ʹ�ܺ������
***************************************************************************/
void enable_hwincept(void)
{
    if(Action_hwincept == false)
    {  
		clr_all_hw_struct();			
        Action_hwincept = true;
    }
	Set_HW_Power_ON();
}
/***************************************************************************
�رպ������																	 
***************************************************************************/
void disable_hwincept(void)
{
		Reset_HW_Power_OFF();
		Action_hwincept = false;

#if 0
		EXTI->IMR&=~(B_HW_MASK|L_HW_MASK|R_HW_MASK);	//�رպ�/��/������ж�
		EXTI->EMR&=~(B_HW_MASK|L_HW_MASK|R_HW_MASK);
		TIM1->CR&=~(0x0001);							//�ر��к���
#endif
}













