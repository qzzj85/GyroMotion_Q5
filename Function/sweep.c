////////////////////////头文件////////////////////////////////////
#include "function.h"
#include "hardware.h"
#include "io_driver.h"
#include "key.h"
#include "lcdkey.h"
#include "power.h"
#include "ring.h"
#include "test.h"
#include "A7105.h"
#include "wall_earth.h"
#include "hwincept.h"
#include "ad.h"
#include "pwm.h"
#include "systime.h"
#include "rtc.h"
#include "flash.h"
#include "cease.h" 
#include "sweep.h" 
#include "docking.h"
#include "spot.h"
#include "tele.h"
#include "chargeing.h"
#include "sleep.h"
#include "err.h"

////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////
void Init_Sweep(void);
void Do_Sweep(void);		   
u8 Sweep_YaoKong_Manage(void) ;
///////////////////////私有函数////////////////////////////////////	
///////////////////////函数实体////////////////////////////////////
/*****************************************************************
功能：初始化自动清扫
*****************************************************************/
void Init_Sweep(void)
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
	Dis_AuGlint = true;
	
	Dis_SpeedGo = true; 
	/******初始化设置的值********************/
	gsv_work_mode.work_mode = SWEEP;	 //工作模式为清扫
	mode.mode = SWEEP;
	clr_ram();
	
    Enable_Sweep();

	Enable_earth();
	Enable_wall();
//	enable_hwincept();//允许红外接收电源
//	Reset_Speed_Send();  //允许红外发送  
	gbv_action_m_fangxiang = false; 	//false:向前
	Display_Real();
	Init_Action();
	mode.step = 0xfe;
	WriteWorkState();
//	ReInitAd();
}
/******************************************************************
功能：在清扫时的工作
******************************************************************/
void Do_Sweep(void)
{
 u8 y = 0;//有碰撞信号时 y = 1;否则为0
// u8 z = 0;//在执行来回扫时 z = 1,否则为0

   //NVIC_GenerateSystemReset();
  //////电池电压低时进入自动回充
  if(battery_voltage < LOWPOWER)
  {
     if(battery_voltage < NOPOWER)
	 {
	     giv_sys_err = 18;
		 Init_Err();
	 }
	 else
	 {			 
         Init_Docking() ;
		 Enable_Beep_Five();
	 }
	 return;
  }
  //在最开始启动时判断左右轮是否离地，否则退出扫地模式
  if((giv_sys_time - mode.time) < 5000)
  {
//      if((lidi.key == 0)||(lidi.key == 0))	//qz:MASK
	  if((l_lidi.key==0)||(r_lidi.key==0))	//QZ:ADD
	  {
	      Init_Cease();
		  return ;
	  }
	  
  }
////////////  //检查中扫与风机
////////////  if((giv_sys_time - mode.time) < 100)
////////////  { 
////////////      if(CheckMDustMotor())
////////////	  {
////////////	      return;
////////////	  }
////////////  }
  //系统有错误
  if(giv_sys_err != 0)
  {
	  Init_Err();
	  return ;
  }
  ///遥控器按键管理		     
//  if(Sweep_YaoKong_Manage() == 1)
//  {
//      return;
//  }

  ///在面板上有三个按键中的任意一个按下了
////  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
////  {
////    key1.sign = 1;
////	  key2.sign = 1;
////	  key3.sign = 1;
////	  Init_Cease();	 
////	  Enable_Beep();
////	  return;
////  }
  
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
  Display_Real();
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&rm_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  ////下面是一些需要驱动的行动////////////
  Io_Driver();	 

  ////在靠近墙壁时机器减速
  if((w_l.high_sign == NEAR)||(w_lm.high_sign == NEAR)||(w_m.high_sign == NEAR)||
     (w_r.high_sign == NEAR)||(w_rm.high_sign == NEAR))
			{
					if((l_rap.rap > 1000)||(r_rap.rap > 1000))
						{
								l_rap.rap = (l_rap.rap*2)/3;
								r_rap.rap = (r_rap.rap*2)/3;
						}
			}
  else
			{
					if((l_rap.rap < Speed)&&(r_rap.rap < Speed)&&(mode.bump == 0))
							{
									l_rap.rap = (l_rap.rap*3)/2;
									r_rap.rap = (r_rap.rap*3)/2;
							}
			}
  //在回充座附近将墙检的灵敏度设置为高
  if((up_hw.sign != 0) && ((up_hw.data&0x8f)==0))
			{
					Wall_SensitivityHigh();
					mode.angle_wall = 0;
			}
  else
			{
					Wall_SensitivityLow();
			}

  ////有遥控器时机器停止按照遥控方向运动
//  if(YaoKongFangXiang())
//		{
//				return;
//		}
  ////是否有需要保护的内容
  if(Action_Protect())
		{
			 return;
		}
  
  if((mode.fangxiang != 0)&&(mode.fangxiang != 1)&&(mode.fangxiang != 2))
		{
				mode.fangxiang = 0;
		}
  /////下面是正常扫地的路径
//  if(z == 0)
  {
      if(action_bump())
				{
						y=1;
						return;
				} 
  }
///////////////////////////////////下面是正常在扫地时的路径////////////////////////////
  ////有过碰撞，碰撞后主路径只能是直线和Z字形动作
  if(1 == y)	
				{ 
						y = 0;
					if((mode.step==0xfe) || (mode.step==0xff))
						{
								mode.step = 0;
						}
					do
						{
								mode.step++;
							if(22 < mode.step)
							{
									mode.step = 0;
							}
						}
						while(3 != zhulujing[mode.step]);
				} 
  	  
  if(mode.step==0xfe)	  //在最开始的时候
			{
					if(!do_action(3,METE))
					{
							return;
					}
							mode.step++;
			}
  if(mode.step ==0xff)	 //掉头
			{
					if(!do_action(1,angle180))
					{
							return;
					}
					mode.step++;
			}

  //如果是螺旋动作或者Z字形动作，碰到墙后沿墙走 
 /* if((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step])||(21 == zhulujing[mode.step]))
  {
	  mode.ybs_sign = 1;
	  mode.angle_wall=angle720;	  
	  mode.long_wall = 0;
  }
  else
  {
      mode.ybs_sign = 0;
  }*/
  if((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step]))
			{
				mode.sum_bump = 0;
				mode.fangxiang = 0;
			}
  if(((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step])||(mode.sum_bump > 10)))
			{
				mode.ybs_sign = 1;
				mode.angle_wall=angle720;	  
				mode.long_wall = 0;
			}
  else
			{
				mode.ybs_sign = 0;
				Enable_wall();
			}
  ////在走Z字形时不做碰撞处理 
//  if(21 == zhulujing[mode.step])  
//  {
//      z = 1;
//      if(read_earth())
//      {
//          z = 0;
//          return ;
//      }
//  }
//  else
//  {
//      z = 0;
//  }
	if(do_action(zhulujing[mode.step],zhulujinglong[mode.step]))
			 {
					 mode.step++;
					 if(mode.step > 22)
					 {
							 mode.step=0;
					 }
			 }
}
/******************************************************************
在清扫时的遥控器程序
******************************************************************/
#if 0
u8 Sweep_YaoKong_Manage(void) 
{
	if(gsv_yaokong.sign == 1) /////有遥控器按键按下
				{
						gsv_yaokong.sign = 0;
					switch(	gsv_yaokong.key)
					{
							case YAOKONG_ESC:
						case YAOKONG_AUTO_THU:
								Init_Cease();
							Enable_Beep();
								return 1;
						case YAOKONG_SPOT_FRI:	
								Init_Spot();
							Enable_Beep();
								return 1;
						case YAOKONG_DOCKING_SUN: 
								Init_Docking();
							Enable_Beep();
								return 1;
								case YAOKONG_V_SAT:
									Change_Speed();
							Enable_Beep();
									return 0;
							//////按下uv键将uv指示取反
								case YAOKONG_UV_WED:
									Turn_UV();
							Enable_Sweep();
							Enable_Beep();
							return 0;
					}
				}
	return 0;
}
#endif
/***************************************************
功能：按下遥控器的前后左右按键之后，机器按照遥控器的方向运动
      不要管理红外碰撞
***************************************************/
#if 0
u8 YaoKongFangXiang(void)
{
static u8 piv_yaokong = 0;
static u8 piv_pengzhuang = 0;
u32 m;
    //在有遥控的时候有碰撞信号，机器后退
	if(piv_yaokong == 1)	//处于遥控动作状态QZ
				{
						m = read_bump();
							if(mode.bump != 0)        //机器有碰撞
							{
								if((m != 0))//碰撞信号不是红外碰撞信号
								{
									piv_pengzhuang = 1;			//遥控碰撞状态置1 QZ
									if(do_action(4,angle240))	//后退一段距离  do_action返回1表示动作执行完成  QZ
									{
										mode.step = 0;	
										mode.bump = 0;			//mode.bump清零，QZ
										mode.step_yaokong = 0;	//mode.step_yaokong清零，QZ
										piv_pengzhuang = 0;
										piv_yaokong = 0;
										return 0; 	//后退动作执行完成，return 0  QZ
									}
									return 1;	//后退动作未完成，return 1  QZ
								}
								if(piv_pengzhuang != 0)
								{
									stop_rap();
									piv_pengzhuang = 0;
									mode.step = 0;
									mode.bump = 0;
									mode.step_yaokong = 0;
									piv_yaokong = 0;
									return 0;
								}
							}
				}
	if((gsv_yaokong.effect == 1)||(gsv_yaokong.sign == 1))	//有遥控按键信号输入  QZ
				{
						gsv_yaokong.sign = 0;
						switch(gsv_yaokong.key)
					{
								case YAOKONG_LEFT:	//LEFT遥控按键  QZ
								if(mode.step_yaokong != 1)
							{		   
									stop_rap();
									mode.step_yaokong = 1;
							}
							break;
								case YAOKONG_RIGHT:	   
								if(mode.step_yaokong != 2)
							{		   
									stop_rap();
									mode.step_yaokong = 2;
							}
							break;
								case YAOKONG_FRONT:		 
								if(mode.step_yaokong != 4)
							{		   
									stop_rap();
									mode.step_yaokong = 4;
							}
							break;
								case YAOKONG_BACK:	   
								if(mode.step_yaokong != 3)
							{		   
									stop_rap();
									mode.step_yaokong = 3;
							}
							break;
						default :		//其余按键,不做处理，return 0 piv_yaokong=0 直接返回 QZ
								piv_yaokong = 0;
							return 0;
					}
					if(piv_yaokong == 0)	//LEFT/RIGHT/FRONT/BACK按键，如果之前不是遥控状态，则停止机器，进入遥控状态piv_yaokong=1  QZ
					{
							stop_rap();
						piv_yaokong = 1;
					}
				}
	else 
				{		//无遥控按键信号，但是机器之前处于遥控状态，QZ
						if((piv_yaokong != 0)&&(mode.step_yaokong != 5))
					{
						stop_rap();
						mode.step_yaokong = 5;
					}
				}
				
				
				
	if(piv_yaokong != 0)
				{
						switch(mode.step_yaokong)
						{
							case 0:
								break;
							case 1:
								do_action(1,angle360);
									break;
							case 2:
								do_action(2,angle360);
								break;
							case 3:
						 //   do_action(4,METE_TWO);
								do_action(4,(uint32_t)(2 * METER_PLUS));
								break;
							case 4:
					//	    do_action(3,METE_TWO);
								do_action(3,(uint32_t)(2 * METER_PLUS));
									break;
							case 5:
								if(do_action(3,angle120))
									{
												piv_yaokong = 0;
										mode.bump = 0;
											mode.step_yaokong = 0;
										mode.step = 0;
								}
						}
				}
	return piv_yaokong;
}
#endif





