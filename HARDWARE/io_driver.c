//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================


#include "io_driver.h"
#include "hardware.h"
#include "systime.h"




////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
bool gbv_action_uv;	/*设置uv灯的状态，由外部模块读取和改变*/
bool gbv_action_s_motor;/*设置边扫马达的状态，由外部模块读取和改变*/
bool gbv_action_m_motor;/*设置中扫马达的状态，由外部模块读取和改变*/ 
bool gbv_action_m_fangxiang;//中扫马达的运动方向，false 表示向前，true表示向后
bool gbv_action_dust_motor;	/*设置灰尘风机的状态，由外部模块读取和改变*/

////////////////////////私有变量//////////////////////////////////	 
bool pbv_state_uv;/*指示当前uv灯的状态*/
bool pbv_state_m_motor;/*指示当前中扫马达的状态*/
bool pbv_state_m_fangxiang;/*指示当前中扫马达的状态*/

bool 	pbv_state_s_motor;/*指示当前边扫马达的状态*/
bool 	pbv_state_dust_motor;/*指示当前灰尘风机的状态*/
u32 	piv_beep_time;     /*蜂鸣器响的时间*/

bool pbv_beep_on;        ///蜂鸣器响的标志
u8 piv_beep_number;   //蜂鸣器的声数
///////////////////////全局函数////////////////////////////////////
///////////////////////私有函数////////////////////////////////////
///////////////////////函数实体////////////////////////////////////
/******************************************************************
功能：初始化中扫驱动、灰尘盒风机驱动、边扫马达驱动、边扫电流输入
      紫外灯驱动。
输入：无
输出：无
******************************************************************/
void Init_Io_Driver(void)
	{  
			gbv_action_uv = false;
			gbv_action_s_motor = false;
			gbv_action_m_motor = false;
			gbv_action_dust_motor = false;
			gbv_action_m_fangxiang = false;

			Reset_UV();
			pbv_state_uv = false;
			//Reset_S_Motor();
//			stop_bs(); //hzh mask
			pbv_state_s_motor = false;
			Reset_Dust_Motor();
			pbv_state_dust_motor = false;

////////////Reset_M_FPMOS();
////////////Reset_M_ZPMOS();
////////////Set_M_FNMOS();
////////////Set_M_ZNMOS();
			Reset_M_FNMOS();		//	2017-2-10  修改添加
	
	
			pbv_state_m_motor = false;
			pbv_state_m_fangxiang = false;
			//Reset_Beep();    //关闭蜂鸣器
			//pbv_beep_on = false;
	}
/*****************************************************************
功能：驱动中扫风机边扫紫外脚
输入：无
输出：无
说明：在需要进行清扫时就可以循环调用
*****************************************************************/
void Io_Driver(void)
{
    /*********************驱动紫外灯*********************/
    if((gbv_action_uv == true) && (pbv_state_uv == false))
				{
							Set_UV();
							pbv_state_uv = true;
				}
		else if((gbv_action_uv == false) && (pbv_state_uv == true))
				 {
							Reset_UV();
							pbv_state_uv = false;
				 }
	 
    /*********************驱动边扫*********************/
    if((gbv_action_s_motor == true) && (pbv_state_s_motor == false))
				{
						//Set_S_Motor();
//						bian_sao_front();
						pbv_state_s_motor = true;
				}
		else if((gbv_action_s_motor == false) && (pbv_state_s_motor == true))
				 {
							 //Reset_S_Motor();
//							 stop_bs(); hzh  mask
							 pbv_state_s_motor = false;
				 }
	
//////////    /*********************驱动中扫*********************/
//////////    if((gbv_action_m_motor == true))
//////////    {
//////////	     if(gbv_action_m_fangxiang != pbv_state_m_fangxiang)
//////////					 {
//////////									 Reset_M_FPMOS();
//////////									 Reset_M_ZPMOS();
//////////							 Set_M_FNMOS();
//////////							 Set_M_ZNMOS();
//////////						 DelayMs(1);
//////////						 pbv_state_m_fangxiang = gbv_action_m_fangxiang;
//////////						 pbv_state_m_motor = false;
//////////					 }
//////////			 if(pbv_state_m_motor == false)
//////////			 {
//////////					 if(gbv_action_m_fangxiang == false)
//////////				 {
//////////									 Reset_M_ZNMOS();
//////////							 Set_M_ZPMOS();
//////////				 }
//////////				 else
//////////				 {
//////////									 Reset_M_FNMOS();
//////////							 Set_M_FPMOS();
//////////				 }
//////////							 pbv_state_m_motor = true;
//////////			 }
//////////			}
//////////		else if((gbv_action_m_motor == false) && (pbv_state_m_motor == true))
//////////				 {
//////////							 Reset_M_FPMOS();
//////////							 Reset_M_ZPMOS();
//////////					 Set_M_FNMOS();
//////////					 Set_M_ZNMOS();
//////////							 pbv_state_m_motor = false;
//////////				 }

			//	2017-2-10修改添加		 
			if((gbv_action_m_motor == true))
				{
						Set_M_FNMOS();							//	2017-2-10修改添加
						pbv_state_m_motor = true;
				}
			else if((gbv_action_m_motor == false) && (pbv_state_m_motor == true))
				{
						Reset_M_FNMOS();						//	2017-2-10修改添加
						pbv_state_m_motor = false;
				}
				
				
				
		
    /*********************驱动灰尘风机*********************/
			if((gbv_action_dust_motor == true) && (pbv_state_dust_motor == false))
					{
							Set_Dust_Motor();
							pbv_state_dust_motor = true;
					}
			else if((gbv_action_dust_motor == false) && (pbv_state_dust_motor == true))
				 {
							 Reset_Dust_Motor();
							 pbv_state_dust_motor = false;
				 }
} 
/**********************************************************
使能蜂鸣器响0.2秒
**********************************************************/
void Enable_Beep_Five(void)
{
   //Set_Beep();
   piv_beep_time = giv_sys_time;
   pbv_beep_on = true;
   piv_beep_number = 10;
}
/**********************************************************
使能蜂鸣器响0.2秒
**********************************************************/
void Enable_Beep(void)
{
   //Set_Beep();
   piv_beep_time = giv_sys_time;
   pbv_beep_on = true;
   piv_beep_number = 1;
}	  
/**********************************************************
使能蜂鸣器响0.2秒
**********************************************************/
void Enable_Beep_number(u8 number)
{
   //Set_Beep();
   piv_beep_time = giv_sys_time;
   pbv_beep_on = true;
   piv_beep_number = number;
}
/**********************************************************
驱动蜂鸣器响
**********************************************************/
void Beep_Driver(void)
{
    if((pbv_beep_on != false)&&(piv_beep_number!=0))
				{
			//		if((giv_sys_time - piv_beep_time) > 2000)
					if((giv_sys_time - piv_beep_time) > 400)
							{
									//Reset_Beep();
									if((giv_sys_time - piv_beep_time) > (400+400))
											{
													piv_beep_time = giv_sys_time;  
													piv_beep_number --;
											}
									if(piv_beep_number == 0)
											pbv_beep_on = false;
							}
					else
								{
										//Set_Beep();
								}
				}
}

