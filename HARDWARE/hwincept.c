////////////////////////头文件////////////////////////////////////
#include "hwincept.h"
#include "hardware.h"
#include "systime.h"

#define CLEAN_EFFECT_TIME 3000			//qz modify 20181215 2000--->3000

////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
INFR_DATA l_hw,r_hw,rm_hw,lm_hw,lb_hw,rb_hw;
bool Action_hwincept;

u16 top_time,top_time_sec;

///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////
void  init_hwincept(void);
void read_hw_data(INFR_DATA *hw_struct);
void clr_hw_effect(INFR_DATA *hw_struct);  
void enable_hwincept(void);
void disable_hwincept(void);

///////////////////////私有函数////////////////////////////////////
void clr_hw_struct(INFR_DATA *hw_struct);
void read_hw_bit(INFR_DATA *hw_struct);		
void read_dummy_wall_bit(INFR_DATA *hw_struct);

///////////////////////函数实体////////////////////////////////////
/******************************************************************
功能：初始化红外接收
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
注意：右红外的中断源EXTI15_10_IRQn已经在碰撞红外中使能，
故此处不需要在使能
----------------------------------------------*/
#if 1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource10);		//左后红外
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);		//左前红外
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);		//左红外
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource13);		//右红外
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource15);		//右后红外
#endif
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource14);		//右前红外

	EXTI_InitStructure.EXTI_Line=EXTI_Line_RMHW|EXTI_Line_LHW|EXTI_Line_LMHW|EXTI_Line_RHW|EXTI_Line_LBHW|EXTI_Line_RBHW;		//后红外,左红外中断
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line_RHW);			//右红外
	EXTI_ClearITPendingBit(EXTI_Line_LHW);			//右红外
	EXTI_ClearITPendingBit(EXTI_Line_RMHW);			//右红外
	EXTI_ClearITPendingBit(EXTI_Line_RBHW);			//右红外
	EXTI_ClearITPendingBit(EXTI_Line_LMHW);			//右红外
	EXTI_ClearITPendingBit(EXTI_Line_LBHW);			//右红外
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
	
	  if(hw_struct->read_io() == Bit_RESET)  //读当前的红外状态
			{
					bit = 0;
			}
	  else
			{
					bit = 1;
			}
	  //////////////////////采样前导码//////////////////////////  
	  if(t > 41)	//QZ ADD:3.7ms
				{
					hw_struct->start = 0;
					hw_struct->tempbit=0;
					hw_struct->time=giv_sys_time;
					return 0;
				}
	  switch(t)
	  {
			
		 	case 1:		//前导码第一次采样	QZ
			 case 5:	//前导码第二次采样	QZ		  
			 case 10:	//数据码第一次采样	QZ
			 case 15:	//数据码第二次采样	QZ
			 case 20:	//数据码第三次采样	QZ
			 case 25:	//数据码第四次采样	QZ
			 		hw_struct->tempbit+=bit;
			 break;
//			 case 28:
			 case 29:	//结束码第一次采样	QZ,2.9ms
					 hw_struct->tempbit += bit;
			 		if(hw_struct->tempbit>2)
					{
//						hw_struct->tempbit=0;
						hw_struct->time=giv_sys_time;
//						return 0;
					}
					hw_struct->tempbit=0;
					 break;
			 case 32:	//前导码第三次采样	QZ	,0.6ms
			 case 34:	 //数据码第五次采样  QZ,2.6ms
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
功能：读取一个字节红外数据
输入：红外数据结构
输出：成功接收在红外结构中设置标志
返回：无
说明：本程序由中断函数调用
******************************************************************/
//u32 g=0;
void read_hw_data(INFR_DATA *hw_struct)
{

  if(hw_struct->start == 0)   
		{
			
				if(hw_struct->read_io() == Bit_RESET)  //开始有启动的位
          {
              hw_struct->start 			= 1; //采样到低电平
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
功能：读取一个字节红外数据
输入：红外数据结构
输出：成功接收在红外结构中设置标志
返回：无
说明：本程序由中断函数调用
******************************************************************/
void read_dummy_wall(INFR_DATA *hw_struct)
{		   
  if(hw_struct->start == 0)      //
		{
				if(hw_struct->read_io() == Bit_RESET)  //开始有启动的位
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
功能：读取一位红外数据
输入：红外数据结构
输出：成功接收在红外结构中设置标志
返回：无
说明：本程序由read_hw_data调用,在前导码和结束码采样3次，数据码采样5次
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
李工样机充电座：
1，空闲时为高
2，前导码是低3ms，高1ms
3，数据0是低1.5ms，高0.75ms
4，由于在离充电座越远的地方信号越弱，因为接收处硬件的原因，
   低电平或高电平会收窄，即波形会变窄，所以之前的方案不管用
***********************/
//u32 c=0,e=0,f=0;
void read_hw_bit(INFR_DATA *hw_struct)
{  
  //读当前的红外状态

  if(hw_struct->read_io() == Bit_RESET)  
		{    
      hw_struct->tempbit ++; 
 
      //验证成功，由于中间红外接收头比较
      //难收到整个字节的数据，所以检测上
      //下降沿来判断信号
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
  else//高
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
          //理论和实际上，读到低电平比读到前4位的可能性要
          //高，测出来大概是4倍，但是只读到低电平并不可靠，因为
          //太阳光也可能有这个频率的红外，所以折中，读取一位或两位
          //数据,就认为是读到充电座信号
        }
        else 
        {
          hw_struct->start 				= 0;         
        } 
        hw_struct->tempbit =0;       
        return; //没有采到前导码，做下面的事是无意义的
      } 
      //采到前导码后要么是高要么是低，低就++,高就进这里
      if(hw_struct->signTop ==1)
      { //采到引导码后采到高电平
        hw_struct->signTop =2;
        //只要是高电平就一直清零hw_struct->tempbit =0;
        //低电平就一直加
      }  
      else if(hw_struct->signTop ==2)
      {
        if((hw_struct->tempbit >= 8)&&(hw_struct->tempbit <= 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//当前数据是低
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++; 
        }
        else if(hw_struct->tempbit == 0)
        {
          //什么都不做
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
           hw_struct->tempdata |= hw_struct->bitnumber;//当前数据是高
           hw_struct->bitnumber 	>>= 1;           
           hw_struct->signTop++;  
           hw_struct->effect_timeTop = giv_sys_time;
           hw_struct->effectTop 			= 1;          
        }  
        else if(hw_struct->tempbit == 0)
        {
          //什么都不做
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
          hw_struct->tempdata &= ~hw_struct->bitnumber;//当前数据是低
          hw_struct->bitnumber >>= 1;          
          hw_struct->signTop++; 
        }
        else if(hw_struct->tempbit == 0)
        {
          //什么都不做
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
           hw_struct->tempdata |= hw_struct->bitnumber;//当前数据是高
           hw_struct->bitnumber 	>>= 1;          
           hw_struct->signTop++; 
        }
        else if(hw_struct->tempbit == 0)
        {
          //什么都不做
        }        
        else
        {
          hw_struct->start 				= 0;  
        }         
      }
      /************************************************/      
      //前面四位是0101，任何一位错了都重新采样          
      /************************************************/
      else if(hw_struct->signTop ==6)
      {
        if((hw_struct->tempbit > 8)&&(hw_struct->tempbit < 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//当前数据是低
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++;
        }
        else if((hw_struct->tempbit > 2)&&(hw_struct->tempbit < 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//当前数据是高
           hw_struct->bitnumber 	>>= 1; 
           hw_struct->signTop++;          
        } 
        else if(hw_struct->tempbit == 0)
        {
          //什么都不做
        }
        else//其他情况从新读取
        {
          hw_struct->start 				= 0;  
        }           
      }
      else if(hw_struct->signTop ==7)
      {
        if((hw_struct->tempbit > 8)&&(hw_struct->tempbit < 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//当前数据是低
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++; 
        }
        else if((hw_struct->tempbit > 2)&&(hw_struct->tempbit < 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//当前数据是高
           hw_struct->bitnumber 	>>= 1;
           hw_struct->signTop++; 
        } 
        else if(hw_struct->tempbit == 0)
        {
          //什么都不做
        }
        else//其他情况从新读取
        {
          hw_struct->start 				= 0;  
        }                       
      }
      else if(hw_struct->signTop ==8)
      {
        if((hw_struct->tempbit > 8)&&(hw_struct->tempbit < 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//当前数据是低
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++; 
        }
        else if((hw_struct->tempbit > 2)&&(hw_struct->tempbit < 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//当前数据是高
           hw_struct->bitnumber 	>>= 1;
           hw_struct->signTop++; 
        } 
        else if(hw_struct->tempbit == 0)
        {
          //什么都不做
        }
        else//其他情况从新读取
        {
          hw_struct->start 				= 0;  
        }        
      }
      else if(hw_struct->signTop ==9)
      {
        if((hw_struct->tempbit > 8)&&(hw_struct->tempbit < 15))
        {
           hw_struct->tempdata &= ~hw_struct->bitnumber;//当前数据是低
           hw_struct->bitnumber >>= 1;
           hw_struct->signTop++; 
        }
        else if((hw_struct->tempbit > 2)&&(hw_struct->tempbit < 8))
        {
           hw_struct->tempdata |= hw_struct->bitnumber;//当前数据是高
           hw_struct->bitnumber 	>>= 1;
           hw_struct->signTop++; 
        } 
        else if(hw_struct->tempbit == 0)
        {
          //什么都不做
        }
        else//其他情况从新读取
        {
          hw_struct->start 				= 0;  
        }        
      }      

      if(hw_struct->bitnumber == 0x00)	//接收完8位数据，强制接收完毕，准备下一轮接收 QZ
      {
          hw_struct->bitnumber 		= 0x80;  //又从开始第一位读 
          hw_struct->start 				= 0;  //清除开始标志.          
          hw_struct->data 				= hw_struct->tempdata;//把数据存入最后结果
				  hw_struct->effect       = 1;
				  if(hw_struct->data == 0x5c)
					{//顶					
            			hw_struct->effectTopReal = 1;
						hw_struct->effect_timeTopReal 	= giv_sys_time;
					}
					else if(hw_struct->data == 0x53)
					{//中
            			hw_struct->effectMid =1;
						hw_struct->effect_timeMid 	= giv_sys_time;							
					}
					else if(hw_struct->data == 0x5b)
					{//右
            			hw_struct->effectRight = 1;
						hw_struct->effect_timeRight 	= giv_sys_time;
					}
					else if(hw_struct->data == 0x57)
					{//左
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

	if(hw_struct->read_io() == Bit_RESET)  //读当前的红外状态
			{
					bit = 0;
			}
	else
			{
					bit = 1;
			}
	//////////////////////采样前导码//////////////////////////  
	if(t > 37)	  //QZ ADD:3.7ms
				{
					hw_struct->start = 0;
					return;
				}
	switch(t)
	{
		 case 1:		  //前导码第一次采样  QZ
		 case 3:	  //前导码第二次采样  QZ		
		 case 8:	  //数据码第一次采样  QZ
		 case 10:   //数据码第二次采样  QZ
		 case 11:   //数据码第三次采样  QZ
		 case 12:   //数据码第四次采样  QZ
		 case 16:   //结束码第一次采样  QZ,2.9ms
		 case 18:   //结束码第二次采样  QZ
				 hw_struct->tempbit += bit;
				 break;
		 case 6:	  //前导码第三次采样  QZ  ,0.6ms
					hw_struct->tempbit += bit;
					if(hw_struct->tempbit > 1)  //前导码有两次高 QZ
						 {
								 hw_struct->start = 0; //清除接收标志 QZ
						 }
					hw_struct->tempbit = 0; //清除tempbit，准备下一阶段使用  QZ
				 break;
			 
		 case 14:    //数据码第五次采样  QZ,2.6ms
				 hw_struct->tempbit += bit;
				 if(hw_struct->tempbit > 3)  //数据码超过3次高
						 {
								 hw_struct->tempdata |= hw_struct->bitnumber;//当前数据是高
						 }
				 else
						 {
								if(hw_struct->tempbit < 2)  //数据码超过3次低
									 {
											 hw_struct->tempdata &=  ~hw_struct->bitnumber;//当前数据是低
									 }
								else
									 {
											 ////////判断是杂波干扰,要重新开始
											hw_struct->start = 0; //清除接收标志
									 }
						 }
				 hw_struct->tempbit = 0;	  //清除tempbit，准备下一阶段使用  QZ
				 break;
						
		 case 21: 	 //结束码第三次采样  QZ
				 hw_struct->tempbit += bit; 
				 if(hw_struct->tempbit < 2)  //结束码有两次低
						 {
								 hw_struct->start = 0; //接收完毕，清除接收标志 QZ
						 }
				 else
						 {
								 if( hw_struct->bitnumber == 0x80)  //接收完8位数据，强制接收完毕，准备下一轮接收 QZ
										 {
												hw_struct->bitnumber		  = 1;	//又从开始第一位读 
												hw_struct->start				  = 0;	 //清除开始标志.			
												hw_struct->data				  = hw_struct->tempdata;//把数据存入最后结果
												hw_struct->sign				  = 1;
												hw_struct->effect_time  = giv_sys_time;
												hw_struct->effect 		  = 1;
								
										 } 
						 }
				 hw_struct->tempbit = 0;	  //清除tempbit，准备下一阶段使用  QZ
				 break;
		 
		 case 22:
		 case 23: 
//		 case 37:
				 if(bit == 0)   //最后位为0,一位数据接收完毕	QZ
						 {
									hw_struct->bitnumber	  = (hw_struct->bitnumber << 1);
									hw_struct->time			  = giv_sys_time;
						 }
				 break;

	}
}



/*********************************************************************
功能：读取一位红外数据
输入：红外数据结构
输出：成功接收在红外结构中设置标志
返回：无
说明：本程序由read_hw_data调用,在前导码和结束码采样3次，数据码采样5次
*********************************************************************/
void read_dummy_wall_bit(INFR_DATA *hw_struct)
{
  u32 t;
  u8 bit;
	
  t = giv_sys_time - hw_struct->time;
  if(hw_struct->read_io() == Bit_RESET)  //读当前的红外状态
			{
					bit = 0;
			}
  else
			{
					bit = 1;
			}
  //////////////////////采样前导码//////////////////////////  
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
	          if(hw_struct->tempbit > 1)  //前导码有两次高
	          {
	              hw_struct->start = 0; //清除接收标志
	          }
	          hw_struct->tempbit = 0;
	          break;
	   
	      case 18:  		 
              hw_struct->tempbit += bit;
	          if(hw_struct->tempbit < 7)  //数据码超过3次高
	          {
	              hw_struct->start = 0; //清除接收标志
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
														hw_struct->start 				= 1;   	//	清除开始标志.     
														hw_struct->data 				= 0xdc;		//	把数据存入最后结果  
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
功能: 清除红外结构的数据
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
功能:清除红外的有效性
输入:无
输出:无
说明:红外接收有效时间是100ms，在此时间内没有再收到则清除有效标志
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
使能红外接收
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
关闭红外接收																	 
***************************************************************************/
void disable_hwincept(void)
{
		Reset_HW_Power_OFF();
		Action_hwincept = false;

#if 0
		EXTI->IMR&=~(B_HW_MASK|L_HW_MASK|R_HW_MASK);	//关闭后/右/左红外中断
		EXTI->EMR&=~(B_HW_MASK|L_HW_MASK|R_HW_MASK);
		TIM1->CR&=~(0x0001);							//关闭中红外
#endif
}













