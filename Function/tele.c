//////////////////////////头文件////////////////////////////////////	
//#include "function.h"
//#include "hardware.h"
//#include "io_driver.h"
//#include "key.h"
//#include "lcdkey.h"
//#include "power.h"
//#include "ring.h"
//#include "test.h"
//#include "A7105.h"
//#include "wall_earth.h"
//#include "hwincept.h"
//#include "ad.h"
//#include "pwm.h"
//#include "systime.h"
//#include "rtc.h"
//#include "flash.h"
//#include "cease.h" 
//#include "sweep.h" 
//#include "docking.h"
//#include "spot.h"
//#include "tele.h"
//#include "chargeing.h"
//#include "sleep.h"
//#include "err.h"
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////	  
void Init_Tele(void); 
void Do_Tele(void);
///////////////////////私有函数////////////////////////////////////	
///////////////////////函数实体////////////////////////////////////
/******************************************************************
功能：初始化遥控程序
******************************************************************/
void Init_Tele(void)
{
    /******初始化显示***********/
    clr_display();
    Dis_On = true;            //显示标志。
    Dis_PrOn = true; 			//预约天数亮
    Dis_HourOn = true; 		//小时亮
    Dis_MinuteOn = true; 		//分钟亮
    Dis_ColOn = true; 		//时间点亮
    Dis_WeekOn = true; 		//星期亮
    Dis_SpeedOn = true; 		//速度亮
    Dis_PowerOn = true; 		//电池图标亮
    Dis_ChargeOn = false ;		//充电图标亮
		Dis_ColGlint = true ;	    //显示时间点为闪烁
		Dis_SpeedGo = true; 
		
		/******初始化设置的值********************/
//////		mode.mode = TELE;
//////		clr_ram();
//////	
//////	ReInitAd();

////////    Enable_Sweep();

//////	Enable_earth();
//////	Enable_wall();
//////	enable_hwincept();				//允许红外接收电源

//////		Reset_Speed_Send();  			//允许红外发送 
//////		gbv_action_m_fangxiang = false;  
////////	 
//////	Display_Real();
//////	Init_Action();
//////	WriteWorkState();
//////		init_key();
		
	mode.mode = TELE;
	mode.bump = 0;
	mode.step = 0;	
	mode.action = 0;
	
	clr_ram();
//	ReInitAd();

	Enable_Sweep();

	Enable_earth();
	Enable_wall();
//	enable_hwincept();		//允许红外接收电源
//	Reset_Speed_Send();  	//允许红外发送 
	gbv_action_m_fangxiang = false;  
	 
	Display_Real();
	Init_Action();
	WriteWorkState();

init_wallearth();

	r_rap.State = 0x00;
}










/*******************************************************************
遥控器遥控工作程序
*******************************************************************/
void Do_Tele(void)
{
//  u32 m;

  //////电池电压低时进入自动回充
  if(battery_voltage < LOWPOWER)
		{
				if(battery_voltage < NOPOWER)
					{
							Enable_Beep_number(8); 
							giv_sys_err = 18;
							Init_Err();
					}
				else
					{				
							Enable_Beep_number(6); 
							Init_Docking() ;
					}
				return;
		}

		
		
  //在最开始启动时判断左右轮是否离地，否则退出扫地模式
  if((giv_sys_time - mode.time) < 5000)
			{
//					if((lidi.key == 0)||(lidi.key == 0))	//QZ:MASK
				if((l_lidi.key==0)||(r_lidi.key==0))		//QZ:ADD
				{
						Init_Cease();
					return ;
				}
				
			}
  if((giv_sys_time - mode.time) < 100)
			{
////////////					if(CheckMDustMotor())
////////////				{
////////////						return;
////////////				}
				if(gsv_yaokong.key == YAOKONG_BACK)
					{
						mode.step = 3;
					}
			}
			
			
  ///在面板上有三个按键中的任意一个按下了
////  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
////			{
////					key1.sign = 1;
////					key2.sign = 1;
////					key3.sign = 1;
////					Init_Cease();	 
////					Enable_Beep();
////					return;
////			}
  
  //hzh   20171204
	if(key1.sign == 0)		
	{
	   key1.sign = 1;  
	   return ;
	}			 
	if(key2.sign == 0)	
	{
	   key2.sign = 1;
//     mode.premode = mode.mode;    
	   Init_Cease();
	   return;
	}
	if(key3.sign == 0)	
	{
	   key3.sign = 1;  
	   Init_Docking();
     return;
	}
  
  
  
  //////显示显示屏的内容
  //Display_Real();
  Io_Driver();	 
  //Dis_Power = Get_Dispower();



	//系统有错误
	if(giv_sys_err != 0)
			{
				Init_Err();
				return ;
			}
  if(gsv_yaokong.effect == 1) /////有遥控器按键按下
				{
					gsv_yaokong.sign = 0;
					switch(	gsv_yaokong.key)
					{
							case YAOKONG_ESC:
								Init_Cease();	
							Enable_Beep();
								return ;
						case YAOKONG_AUTO_THU:
								Init_Sweep();
							Enable_Beep();	  
								return ;
						case YAOKONG_SPOT_FRI:	   
								Init_Spot();
							Enable_Beep();
								return ;
						case YAOKONG_DOCKING_SUN:  
								Init_Docking();
							Enable_Beep();
								return ;
								case YAOKONG_V_SAT:	
									Change_Speed();
							Enable_Beep();
							break;
							case YAOKONG_UV_WED:	
									Turn_UV();
									Enable_Sweep();
									Enable_Beep();
							break ;
								
							case YAOKONG_LEFT:
									if(mode.step != 1)
											{		   
													stop_rap();
													mode.step = 1;
											}
									break;
											
							case YAOKONG_RIGHT:	   
									if(mode.step != 2)
											{		   
													stop_rap();
													mode.step = 2;
											}
									break;
											
							case YAOKONG_FRONT:		 
									if(mode.step != 0)
										{		   
												stop_rap();
												mode.step = 0;
										}
										break;
										
							case YAOKONG_BACK:	   
									if(mode.step != 3)
											{		   
													stop_rap();
													mode.step = 3;
											}
									break;
					}
				 }
   else
			 {
		////////////       if((mode.step == 3))
		////////////	   {
		////////////	     Init_Cease();
		////////////		   return;
		////////////	   }
		////////////	   else
		////////////	   {
		////////////	      if(mode.step != 0)
		////////////		  {
		////////////		      stop_rap();
		////////////		      mode.step = 0;
		////////////		  }
		////////////	   }
		////////////		 
				 
				 
				if((mode.step == 3))		//	没有遥控按键。在后退运动状态   
						 {
								Init_Cease();
								return;
						 }
		////			 else if((mode.step == 0))
		////				 {
		////						why_Stop = 0x87;
		////						Init_Cease();
		////						return;
		////				 }
					 else			
						 {
								if(mode.step == 3)
									{
											stop_rap();
											mode.step = 0xF0;
									}
						 }
						
			 }
			 	 

  //左右轮调速
//  comm_rap();
	ACC_DEC_Curve();			  
			 
  //在遥控的过程中有障碍信号
  //////m=read_bump();
	read_bump();		//	读取碰撞信息
	switch(mode.bump)
			{
					case 0:		
						break;
					//----------------------
					default:
							if(do_action(4,500))
									{
										Init_Cease();	
									}
						return;
									
									
									
									
					case 5:	
							if(do_action(4,300))
									{
										Init_Cease();	
									}
							if (l_bump.key == 0)
								{
						//			Init_Cease();
								}
						return;
					case 6:	
							if(do_action(4,300))
									{
										Init_Cease();	
									}
							if (r_bump.key == 0)
								{
							//		Init_Cease();
								}
						return;

					case 99:	//	超声停止
							Enable_Beep_number(3);
							Init_Cease();	
						return;
		}
		
		
		
		
	Speed = 2400;

		
		
		
   //对各个按键的动作
   switch (mode.step)
		 {
				case 0:
						if(do_action(3,(uint32_t)(1 * METER_PLUS)) == 1)
								{
										mode.step = 0xF0;
								}
								

								
								
						break;
				case 1:				   
//////						if(do_action(1,	(uint32_t)(45 * Angle_1)) == 1)
//////								{
//////										mode.step = 0xF0;
//////								}
								
									   
						if(do_action(7,	(uint32_t)(360 * DuLun_Angle_1)) == 1)
								{
										mode.step = 0xF0;
								}
						break;
				case 2:					
//////						if(do_action(2,	(uint32_t)(45 * Angle_1)) == 1)
//////								{
//////										mode.step = 0xF0;
//////								}	   
				
						if(do_action(8,	(uint32_t)(360 * DuLun_Angle_1)) == 1)
								{
										mode.step = 0xF0;
								}
						break;
				case 3:			  
						if(do_action(4,(uint32_t)(1 * METER_PLUS)) == 1)
								{
										mode.step = 0xF0;
								}
							
		
	////					flo1 = GGPM01_Angle - Start_Aangle;
	  
	  
////						if(flo1 > 0.2)
////								{
////										mode.step = mode.step;
////////										mode.step = 0xF0;
////////										Init_Cease();	
////									Start_Aangle = GGPM01_Angle;
////									Enable_Beep_number(1);
////								}
////	
////						if(flo1 < -0.2)
////								{
////										mode.step = mode.step;
////									Start_Aangle = GGPM01_Angle;
////									Enable_Beep_number(1);
////								}
								
						break;
				
				case 0xF0:		//	等待	  
						mode.step = 0xF0;
						break;
				
				
				
				default:
					mode.step = 0;	      	   	    
		 } 

}































