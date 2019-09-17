//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

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
////#include "time1.h"
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
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
uint8 Docking_YaoKong_Manage(void) ;
///////////////////////全局函数////////////////////////////////////
///////////////////////私有函数////////////////////////////////////	

uint8 FindSeat(void);
/******************************************************************
回充程序的初始化
******************************************************************/
void Init_Docking(void)
{
    clr_display();
    Dis_On = true;            //显示标志。
    Dis_PrOn = true; 			//预约天数亮
    Dis_HourOn = true; 		//小时亮
    Dis_MinuteOn = true; 		//分钟亮
    Dis_ColOn = true; 		//时间点亮
    Dis_WeekOn = true; 		//星期亮
    Dis_SpeedOn = true; 		//速度亮
    Dis_PowerOn = true; 		//电池图标亮
//    Dis_AllFlagOn=true;
	/******初始化设置的值********************/
	mode.mode = DOCKING;
	clr_ram();
	
	Enable_earth();
	Enable_wall();
	enable_hwincept();//允许红外接收电源
//	Reset_Speed_Send();  //允许红外发送	//qz mask 20180201  
	Set_Speed_Send();	//qz add 20180201
	 									 
	gbv_action_s_motor = true;
	Display_Real();
	Init_Action();
	WriteWorkState();

	//qz add 20180319
	stop_zs();
	Stop_FJ();
//	ReInitAd();
}

/******************************************************************
在清扫时的遥控器程序
******************************************************************/
#if 0
uint8 Docking_YaoKong_Manage(void) 
{
    if(gsv_yaokong.sign == 1) /////有遥控器按键按下
	{
	    gsv_yaokong.sign = 0;
		switch(	gsv_yaokong.key)
		{
		    case YAOKONG_ESC:	   
			case YAOKONG_DOCKING_SUN:
			    Init_Cease();
				Enable_Beep();
			    return 1;
			case YAOKONG_SPOT_FRI:	
			    Init_Spot();
				Enable_Beep();
			    return 1; 		
			case YAOKONG_AUTO_THU:
			    Init_Sweep();	
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
/******************************************************************
回充程序
******************************************************************/
void Do_Docking(void)
{
uint32 m;	
uint16 s;		  
	//系统有错误
	if(giv_sys_err != 0)
	{
	    Init_Err();
		return ;
	}
  ///在面板上有三个按键中的任意一个按下了
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
  {
      key1.sign = 1;
	  key2.sign = 1;
	  key3.sign = 1;
	  Init_Cease();	 
	  Enable_Beep();
	  return;
  }
    //////显示显示屏的内容
  Display_Real();

   /////有充电的电源插入
  if((power.charge_dc == 1) || (power.charge_seat == 1))
  {
     if((mode.step != 3) || ((mode.step_mk != 4)&&(mode.step_mk != 5)))
				 {	
						 stop_rap();
					 DelayMs(200);
						 mode.step = 3;
					 mode.step_mk = 5;
					 mode.step_bp = 0;
				 }
     stop_rap();
		 m = giv_sys_time;
		 while(giv_sys_time - m < 20000)
		 {
				 AutoReadKey();
			 judge_charge();
		 }
		 DelayMs(2000);
#ifdef DC_NOBAT_RUN
		 if(((power.charge_dc)|(power.charge_seat))&(!dc_nobat_run))
#else
		 if(power.charge_dc|power.charge_seat)
#endif
		 {
				 Init_Chargeing();
				 return;
		 }
		 else
		 {
				 mode.step = 3;
			 mode.step_mk = 4;
			 mode.step_bp = 0;
		 }		  
  }

  ///遥控器按键管理		     
  if(Docking_YaoKong_Manage() == 1)
  {
      return;
  }
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&rm_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw);
  clr_hw_effect(&dummy_wall);  
  
  Io_Driver();
  //////电池彻底没电，机器右没有找到回冲座，机器停止找，进入睡眠模式
  if((battery_voltage < NOPOWER)&&(mode.step != 3))
  {
     Init_Sleep() ;
	 return;
  }
  
  ////有遥控器时机器停止按照遥控方向运动
  if(YaoKongFangXiang())
  {
      return;
  }
  ////是否有需要保护的内容
  if((mode.step != 3)||((mode.step_mk != 3)&&(mode.step_mk != 5)))
  {
      if(Action_Protect())
      {
          return;
      }
  }

  /////在直走和沿边走时收到回充座信号
  if( mode.step != 3)
  {		
      s = ReadHwSign();		
      if((s&0x8) != 0)		//QZ:UP_HW收到顶信号
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 0;//顶上区域
	  }		  	
      if((s&0x5555) != 0)	//QZ ADD: 0X5555:中间区域或左边区域
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 1;//左边区域
	  }	
      if((s&0x2222) != 0)
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 2;//右边区域
	  }	
  /*    if((s&0x4444) != 0)
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 3;
	  }*/
  }
  m = read_bump();
  
  switch(mode.step)
  {
      case 0:
	      if(do_action(3,METE_H5))		//直线行走 QZ:ADD
		  {
		      mode.step = 1;
		  } 
		  if( m != 0)	//QZ:有碰撞,停止,准备沿边
		  {
		      stop_rap();
			  mode.angle_wall = angle720;
			  mode.step = 2;
			  mode.step_bp=0;
		  }
		  return; 
	  case 1:
	       if(do_action(1,angle360))		//左转360度 QZ:ADD
		  {
		      mode.step = 0;
		  }	
		  if( m != 0)
		  {						//QZ:有碰撞,停止,准备沿边
		      stop_rap();	
			  mode.step = 2;	 
			  mode.angle_wall = angle720;
			  mode.step_bp=0;
		  }
		  return;
	  case 2:
	      if(ybs() == 0)
		  {
		      mode.step = 4;
		  }
		  return;
	  case 3:
	      if( FindSeat() == 0)	//QZ:收到红外信号,准备进入寻找充电座
		  {
		      mode.step = 0;
		  }
	      return;
	  case 4:
	      if(do_action(1,angle90))
		  {
		      mode.step = 0;
		  }
		  return;
	  default :
	      mode.step = 0;	       
  }
}
/**********************************************
读取红外信号
**********************************************/
uint16 ReadHwSign(void)
{
    uint16 s;
	    s = 0; 
	    if((l_hw.effect == 1)&&((l_hw.data&0xf) == 0))
		{
		    //红外信号是顶上信号
		    if(l_hw.data == 0x70)
			{		 
		        s |= 0x8000	 ;
			}
				 //红外信号是左边信号
			else if((l_hw.data == 0xe0) || (l_hw.data == 0xa0)
			     || (l_hw.data == 0x60) || (l_hw.data == 0x20))
				 {
				     s |= 0x1000;
				 }
				 //红外信号是中间区域信号
				 else if((l_hw.data == 0xb0)|| (l_hw.data == 0x30)
			          || (l_hw.data == 0x80)|| (l_hw.data == 0x00)
					  || (l_hw.data == 0xc0)|| (l_hw.data == 0x40))
					  {
					      s |= 0x4000;
					  }
					  //红外信号是右边信号
					  else if((l_hw.data == 0xd0)|| (l_hw.data == 0x90)
					       || (l_hw.data == 0x50)|| (l_hw.data == 0x10))
					       {
					           s |= 0x2000;
					       }
		}
	    if((rm_hw.effect == 1)&&((rm_hw.data&0xf) == 0))
		{
		    if(rm_hw.data == 0x70)
			{		 
		        s |= 0x800	 ;
			}
			else if((rm_hw.data == 0xe0) || (rm_hw.data == 0xa0)
			     || (rm_hw.data == 0x60) || (rm_hw.data == 0x20))
				 {
				     s |= 0x100;
				 }
				 //红外信号是中间区域信号
				 else if((rm_hw.data == 0xb0)|| (rm_hw.data == 0x30)
			          || (rm_hw.data == 0x80)|| (rm_hw.data == 0x00)
					  || (rm_hw.data == 0xc0)|| (rm_hw.data == 0x40))
					  {
					      s |= 0x400;
					  }
					  //红外信号是右边信号
					  else if((rm_hw.data == 0xd0)|| (rm_hw.data == 0x90)
					       || (rm_hw.data == 0x50)|| (rm_hw.data == 0x10))
					       {
					           s |= 0x200;
					       }
		}
	    if((r_hw.effect == 1)&&((r_hw.data&0xf) == 0))
		{
		    if(r_hw.data == 0x70)
			{		 
		        s |= 0x80	 ;
			}
			else if((r_hw.data == 0xe0) || (r_hw.data == 0xa0)
			     || (r_hw.data == 0x60) || (r_hw.data == 0x20))
				 {
				     s |= 0x10;
				 }
				 //红外信号是中间区域信号
				 else if((r_hw.data == 0xb0)|| (r_hw.data == 0x30)
			          || (r_hw.data == 0x80)|| (r_hw.data == 0x00)
					  || (r_hw.data == 0xc0)|| (r_hw.data == 0x40))
					  {
					      s |= 0x40;
					  }
					  //红外信号是右边信号
					  else if((r_hw.data == 0xd0)|| (r_hw.data == 0x90)
					       || (r_hw.data == 0x50)|| (r_hw.data == 0x10))
					       {
					           s |= 0x20;
					       }
		}
	    
	    if((up_hw.effect == 1)&&((up_hw.data&0xf) == 0))
		{
		    if(up_hw.data == 0x70)
			{		 
		        s |= 0x8	 ;
			}
			else if((up_hw.data == 0xe0) || (up_hw.data == 0xa0)
			     || (up_hw.data == 0x60) || (up_hw.data == 0x20))
				 {
				     s |= 0x1;
				 }
				 //红外信号是中间区域信号
				 else if((up_hw.data == 0xb0)|| (up_hw.data == 0x30)
			          || (up_hw.data == 0x80)|| (up_hw.data == 0x00)
					  || (up_hw.data == 0xc0)|| (up_hw.data == 0x40))
					  {
					      s |= 0x4;
					  }
					  //红外信号是右边信号
					  else if((up_hw.data == 0xd0)|| (up_hw.data == 0x90)
					       || (up_hw.data == 0x50)|| (up_hw.data == 0x10))
					       {
					           s |= 0x2;
					       }
		}
	return s;
}
/**********************************************
发现回充座信号后找回充座
**********************************************/
uint8 FindSeat(void)
{
    uint32 m;  //碰撞信号
	uint16 s;//  红外信号
    static uint32 piv_stime = 0;
//	static uint32 piv_length = 0;
	    if(mode.bump == 0)
		{
		    mode.bump = 9;
		}
	    m = read_bump();
		s = ReadHwSign();
	   // if(( l_rap.rap > 750) || (r_rap.rap > 750) )
	    if(( l_rap.rap > 1500) || (r_rap.rap > 1500) )
		{
	        l_rap.rap = l_rap.rap/2;
	        r_rap.rap = r_rap.rap/2;
	    }
		switch (mode.step_mk )
		{			
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
		    //找到顶上的信号
		    case 0:
			    if(((s & 0x7777) != 0)&&(mode.step_bp < 7))	//QZ ADD: s&0x7777!=0,表示还收到其他方向的红外信号
							{
									stop_rap();
									if((s & 0x1111) != 0)    //找到左边信号
											{
													mode.step_bp = 7;
											}	 
									if((s & 0x6666) != 0)	//找到右边信号
											{
													mode.step_bp = 8;
											}
							//	if((s & 0x4444) != 0)  //找到中间信号
							//	{
							//	    mode.step_bp = 9;
							//	}
								return 1;
							}
			    switch(mode.step_bp)		//QZ:在这里表示除了顶红外，还没有收到其他方向的红外信号
				{
				    case 0:
					    do_action(3,angle720);	//QZ ADD:前进
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
					    if(((s & 0xf) == 0)&&(l_ring.length > angle60))
						{
						    stop_rap();		//QZ:UP_HW失去顶红外信号(反方向?)
							mode.step_bp = 1;
						}
						break;
					case 1:
					    if(do_action(1,angle360+angle30))	//掉头
						{
						    mode.step_bp = 2;	
						}
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
						if((s & 0xf) != 0) 	//QZ:UP_HW收到顶红外信号
						{
						    stop_rap();
							mode.step_bp = 5;
						}
						break;
					case 2:
					    if(do_action(3,angle90))	//开始时旋转360度，希望用左边或者右边接收头收到信号	//QZ 前进
						{
							mode.step_bp = 1;
						}
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
					    if(((s & 0xf) != 0))
						{
						    stop_rap();
							mode.step_bp = 5;	//QZ:UP_HW收到顶红外信号
						}
						break;
					case 3:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							return 1;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 4;
						}
						break;
				    case 4:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 2;
						}
						break;
						
					//向右转动希望找到信号
					case 5:					  
					    if(do_action(3,angle150))
						{
						    mode.step_bp = 6;
						}
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
					    if(((s & 0xf) == 0))
						{
						    stop_rap();
							mode.step_bp = 1;
						}
						break;
						
					//向左转动360度，希望中间接收头收到信号
					case 6:		   			   //向左转动
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 5;
						}	 
						break;
					case 7:						//QZ:收到左边信号,进入此位置,mode.step_mk=1,进入下一阶段
					    if(do_action(3,angle75))
						{
					        mode.step_bp = 13;
						    mode.step_mk = 1;
						}
						break;
					case 8:						//QZ:收到右边信号,进入此位置,
					    if(do_action(1,angle90))
						{
					        mode.step_bp = 9;
						}
						break;
					case 9:
					    if(do_action(3,angle90))
						{
					        mode.step_bp = 12;
						}
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 10;
						} 
						break;
					case 10:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							return 1;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 11;
						}
						break;
					case 11:
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 9;
						}
						return 1;
					case 12:
					    if(do_action(1,angle330))
						{
						    mode.step_bp = 9;
						}
						if((s & 0x0600) != 0)	//QZ:M_HW收到中红外和右红外信号,mode.step_mk=2，进入下一阶段
						{
						    stop_rap();
							mode.step_bp = 12;
							mode.step_mk = 2;
						}
						break;
					default :
					    mode.step_bp = 0;
				}
				return 1;	
				
				
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================

		    //找到左边的信号				    
			case 1:
			    if( ((up_hw.effect != 0) && ((up_hw.data & 0x80) == 0 ))	//QZ:UP_HW	
				   && (mode.step_bp < 12))	 //中间接收头收到顶上的信号
				{
				    stop_rap();
					mode.step_bp = 12;
				}
				if(((s&0x0200) != 0) ) 		//QZ:MID_HW收到右边信号
				{
				    if((mode.step_bp < 12))  //收到右边或者中间信号
					{			 
					    mode.step_mk = 2;	//QZ:进入下一阶段	
					}
					else
					{
					    if(mode.step_bp <17)
						{
						    mode.step_bp = 17;
						}
					}	
				}
			    switch(mode.step_bp)
			    {
			        //向左旋转希望中间的接收头能收到左边信号
			        case 0:
				        if(do_action(2,angle360))
					    {
					        mode.step_bp = 1; //转完一圈后，中间没有收到信号
					    }
					    if(rm_hw.effect != 0) //中间接收头收到信号，机器下一步向前进
					    {                     
							DelayMs(100);
					        stop_rap();
							DelayMs(200);
						    mode.step_bp = 9;
					    }
						if((m != 0)&&((m <7)))  //有碰撞信号、第一次
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    return 1;

				    //转完一圈后前接收头不能收到信号，继续向前进1米
				    case 1:
				        if(do_action(3,METE))
						{		 
						    mode.step_bp = 0;	//QZ:M_HW和UP_HW可以接收到信号,走完一米后,mode.step_bp=0,从第0步再开始
						}
						if((m != 0)&&((m <7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						if((rm_hw.effect == 0)&&(up_hw.effect == 0)) //接收头收不到信号
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break;
				   	//没有收到信号掉头
					case 2:
					    if(do_action(6,angle720+angle120))
						{
							mode.step = 0;		//QZ:掉头以后,还是没有收到信号,重新回到第一步重来
							mode.step_mk = 0;
						}
					    if((rm_hw.effect != 0)||(up_hw.effect != 0)) //接收头收到信号
					    {
					        stop_rap();
						    mode.step_bp = 1;
					    }
						if((m != 0)&&((m <7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step = 0;
							mode.step_mk = 0;
						}
						break;
					//机器碰到障碍后退
					case 3:	
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 4;
					    }
						break;
                    //机器向右转90度
				    case 4:
					    if(do_action(2,angle90))
						{
						    mode.step_bp = 5;
						}
						break;
					//机器前进15厘米
					case 5:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //有碰撞信号，可能是右边的障碍物。
						{
						    stop_rap();
							mode.step_bp = 6;
						}
						if((rm_hw.effect == 0)&&(up_hw.effect == 0)) //接收头收不到信号
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break ;
					//机器后退到摆脱障碍
					case 6:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 7;
					    }
						break;
					//机器向左转动30度
					case 7:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 5;
						} 
						break;
					//机器向前进两米
					case 9:        
				        do_action(3,METE_TWO);	 
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if((rm_hw.effect == 0)&&(l_ring.length > angle90))
						{						//QZ:如果M_HW接收不到信号,且已走过angle90的脉冲,则停止,进入下一阶段
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 10;
						}
				   	    break;
						
					//前面的接收头没有收到信号，机器向左转动希望找到信号
				    case 10:
					    if(do_action(6,angle60))
						{
						    mode.step_bp = 11;
						}
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(rm_hw.effect != 0)
						{   
							DelayMs(200);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//前面的接收头没有收到信号，机器向右转动希望找到信号
					case 11:
					    if(do_action(1,angle360))
						{
						    mode.step_bp = 1;		//QZ:向右向左都有尝试，还是M_HW没有信号,则mode.step_bp=1,重新回到第一步
						}
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(rm_hw.effect != 0)
						{
							DelayMs(100);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//机器收到顶上信号，然后向右转120度，直到左边接收头收到信号
					case 12:	
				        if(do_action(2,angle120))
						{
							mode.step_bp = 13;
						}
						if(( s&0xf000) != 0)  
						{
						    stop_rap();		//QZ:L_HW收到任意信号
							mode.step_bp = 13;
						}
						if((s&0x0600) != 0)
						{
						    stop_rap();		//M_HW收到中红外或右红外
							mode.step_bp = 17;
						}
				   	    break;
					
					//机器前进希望找到中间或者右边信号
					case 13:
					    if(do_action(3,angle75))
						{
						    if((s & 0x0001) == 0)		
						    {							//QZ:向前走完,UP_HW没有收到左红外信号
						        stop_rap();
							    mode.step_bp = 16;
								break;
						    }
						}
					    if((m != 0)&&(m < 7))
						{
						    stop_rap();
						    mode.step_bp = 14;
						}
						if((s & 0x0606) != 0)			
						{								//QZ:M_HW和UP_HW收到中红外或右红外信号
						    stop_rap();
							mode.step_bp = 17;
						}
						break;	
					
					//机器找到了中间或者右边信号，机器再向前走15厘米
					case 14:   
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 15;
					    }
						break;
					case 15	:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 13;
						}
						break;
					case 16:
					    if(do_action(1,angle360))
						{
						    mode.step_bp = 0;
							mode.step = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0100) != 0)	
						{						//QZ:M_HW收到左红外信号
						    stop_rap();
							mode.step_bp = 12;
						}
						if((s & 0x0606) != 0)
						{						//QZ:M_HW或UP_HW收到中红外或右红外信号
						    stop_rap();
							mode.step_bp = 17;
						}
						break;
					case 17:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 19;
						}
						if((m != 0)&&(m<7))
						{
						    stop_rap();
							mode.step_bp = 18;
						}
						break;
					case 18:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 19;
					    }
						break;	
					//机器找到中间区域，向左转动，希望前面接收头能收到中间信号
					case 19:
					    if(do_action(1,angle360))
						{
						    mode.step = 0;
							mode.step_bp = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0500) != 0)
						{
						    stop_rap();
							mode.step_bp = 0;
							mode.step_mk = 3;
						}
						break;
					default :
					    mode.step_bp = 0;
			    }
			    return 1;
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//====================================================================================================================================================================== 
		    //找到右边的信号				    
			case 2:
				if(((up_hw.effect != 0) && ((up_hw.data & 0x80) == 0 ))
				 && (mode.step_bp < 12))	 //中间接收头收到顶上的信号
						{
								stop_rap();
							mode.step_bp = 12;
						}
				if(((s&0x0100) != 0) )
						{
								if((mode.step_bp < 12))  //收到右边或者中间信号
										{	   
												mode.step_mk = 1;	
										}
								else
										{
												if(mode.step_bp <17)
											{
													mode.step_bp = 17;
											}
										}	
						}
			    switch(mode.step_bp)
			    {
			        //向左旋转希望中间的接收头能收到左边信号
			        case 0:
				        if(do_action(1,angle360))
					    {
					        mode.step_bp = 1; //转完一圈后，中间没有收到信号
					    }
					    if(rm_hw.effect != 0) //中间接收头收到信号，机器下一步向前进
					    {                     
							DelayMs(100);
					        stop_rap();
							DelayMs(200);
						    mode.step_bp = 9;
					    }
						if((m != 0)&&((m <7)))  //有碰撞信号、第一次
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    return 1;

				    //转完一圈后前接收头不能收到信号，继续向前进1米
				    case 1:
				        if(do_action(3,METE))
						{		 
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						if((rm_hw.effect == 0)&&(up_hw.effect == 0)) //接收头收不到信号
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break;
				   	//没有收到信号掉头
					case 2:
					    if(do_action(5,angle720+angle120))
						{
							mode.step = 0;
							mode.step_mk = 0;
						}
					    if((rm_hw.effect != 0)||(up_hw.effect != 0)) //接收头收到信号
					    {
					        stop_rap();
						    mode.step_bp = 1;
					    }
						if((m != 0)&&((m <7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step = 0;
							mode.step_mk = 0;
						}
						break;
					//机器碰到障碍后退
					case 3:	
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 4;
					    }
						break;
                    //机器向右转90度
				    case 4:
					    if(do_action(1,angle90))
						{
						    mode.step_bp = 5;
						}
						break;
					//机器前进15厘米
					case 5:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //有碰撞信号，可能是右边的障碍物。
						{
						    stop_rap();
							mode.step_bp = 6;
						}
						if((rm_hw.effect == 0)&&(up_hw.effect == 0)) //接收头收不到信号
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break ;
					//机器后退到摆脱障碍
					case 6:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 7;
					    }
						break;
					//机器向左转动30度
					case 7:
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 5;
						} 
						break;
					//机器向前进两米
					case 9:        
				        do_action(3,METE_TWO);	 
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if((rm_hw.effect == 0)&&(l_ring.length > angle90))
						{
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 10;
						}
				   	    break;
						
					//前面的接收头没有收到信号，机器向左转动希望找到信号
				    case 10:
					    if(do_action(5,angle60))
						{
						    mode.step_bp = 11;
						}
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(rm_hw.effect != 0)
						{   
							DelayMs(200);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//前面的接收头没有收到信号，机器向右转动希望找到信号
					case 11:
					    if(do_action(2,angle360))
						{
						    mode.step_bp = 1;
						}
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(rm_hw.effect != 0)
						{
							DelayMs(100);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//机器收到顶上信号，然后向右转120度，直到左边接收头收到信号
					case 12:	
				        if(do_action(1,angle120))
						{
							mode.step_bp = 13;
						}
						if(( s&0x00f0) != 0) 	//QZ:R_HW收到信号 
						{
						    stop_rap();
							mode.step_bp = 13;
						}			
						if((s&0x0500) != 0)		//QZ M_HW收到中红外或左红外信号
						{
						    stop_rap();
							mode.step_bp = 17;
						}
				   	    break;
					
					//机器前进希望找到中间或者右边信号
					case 13:
					    if(do_action(3,angle75))
						{	
						    if((s & 0x0002) == 0)
						    {
						        stop_rap();
							    mode.step_bp = 16;
						    }
						}
					    if((m != 0)&&(m < 7))
						{
						    stop_rap();
						    mode.step_bp = 14;
						}
						if((s & 0x0505) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;	
					
					//机器找到了中间或者右边信号，机器再向前走15厘米
					case 14:   
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 15;
					    }
						break;
					case 15	:
					if(do_action(2,angle30))
							{
									mode.step_bp = 13;
							}
						break;
					case 16:
					 if(do_action(2,angle360))
							{
									mode.step_bp = 0;
								mode.step = 0;
								mode.step_mk = 0;
								mode.bump = 0;
							}
						if((s & 0x0200) != 0)	//QZ:M_HW是否收到右红外信号
							{
									stop_rap();
									mode.step_bp = 12;
							}
						if((s & 0x0505) != 0)		//QZ:因为之前有判断0x0200,此处判断M_HW和UP_HW是否收到中左红外信号
							{						//QZ:M_HW可能有右红外信号,即mode.step_bp==12,不影响此处判断
									stop_rap();
								mode.step_bp = 17;
							}
						break;
					case 17:
						if(do_action(3,angle60))
							{
									mode.step_bp = 19;
							}
						if((m != 0)&&(m<7))
							{
									stop_rap();
									mode.step_bp = 18;
							}
						break;
					case 18:
					    if(do_action(4,angle360))
								{
										giv_sys_err = m;
										Init_Err();
										break;
								}
					    if((m == 0)||(m > 7))  //没有碰撞信号
								{
										stop_rap();
										mode.step_bp = 19;
								}
						break;	
					//机器找到中间区域，向左转动，希望前面接收头能收到中间信号
					case 19:
					    if(do_action(2,angle360))
						{
						    mode.step = 0;
							mode.step_bp = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0600) != 0)
						{
						    stop_rap();
							mode.step_bp = 0;
							mode.step_mk = 3;
						}
						break;
					default :
					    mode.step_bp = 0;
			    }
			    return 1; 

//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
						
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
//======================================================================================================================================================================				
//======================================================================================================================================================================		
//======================================================================================================================================================================
			//找到了中间的信号
			case 3:	
				if((l_bump.key != 0)&&(r_bump.key != 10)&&(mode.step_bp != 10))  //有碰撞信号
						{
									stop_rap();
									piv_stime = giv_sys_time;
									mode.step_bp = 10;
						}
	         //   if(( l_rap.rap > 500) || (r_rap.rap > 500) )
	      if(( l_rap.rap > 1000) || (r_rap.rap > 1000) )
		        {
	                l_rap.rap = l_rap.rap/2;
	                r_rap.rap = r_rap.rap/2;
		        }
//				Enable_Beep();
			   	switch(mode.step_bp)
				{
				    //顶上接收头收到右边信号，向左转360度希望能找到信号
				    case 0:
								if((l_rap.sign == 0)&&(r_rap.sign == 0))
										{
				//							mode.step_bp = 1;
				//							enable_rap(FRONT,angle60 ,FRONT,angle180);							
											
											/////////////////////2015.8.6////////////////
											if((s & 0x0700) == 0) //中间接收头没收到信号
											{
												mode.step_bp = 5;	
											}
											////////////////////end2015.8.6//////////////
											else
											{
												mode.step_bp = 1;
												enable_rap(FRONT,angle60 ,FRONT,angle180);
											}	
										}
								if((s & 0x0200) != 0)  //收到右边信号
										{			 
												 stop_rap();
											 mode.step_bp = 1; //2
											 enable_rap(FRONT,angle60 ,FRONT,angle180);
										}													 
					    break;
				    //顶上接收头找到左边信号向右转
					case 1:
						if((l_rap.sign == 0)&&(r_rap.sign == 0))
								{
		//							mode.step_bp = 2;
		//							enable_rap(FRONT,angle180 ,FRONT,angle60);

									/////////////////////2015.8.6////////////////
									if((s & 0x0700) == 0) //中间接收头没收到信号
											{
												mode.step_bp = 5;	
											}
									////////////////////end2015.8.6//////////////
									else
											{
												mode.step_bp = 2;
												enable_rap(FRONT,angle180 ,FRONT,angle60);
											}	
								}
						if((s & 0x0100) != 0)  //收到左边信号
								{
									stop_rap();
									mode.step_bp = 2;
									enable_rap(FRONT,angle180 ,FRONT,angle60);	
		//							enable_rap(FRONT,r_ring.length ,FRONT,l_ring.length);
								}
						break;	
					//中间接收头找到了中间信号，机器向前前进	
				    case 2:	
						if((l_rap.sign == 0)&&(r_rap.sign == 0))
								{
		//							mode.step_bp = 3;							 
		//							enable_rap(FRONT,angle60 ,FRONT,angle180);
									
									/////////////////////2015.8.6////////////////
									if((s & 0x0700) == 0) //中间接收头没收到信号
									{
										mode.step_bp = 5;	
									}
							////////////////////end2015.8.6//////////////
									else
									{
										mode.step_bp = 3;							 
										enable_rap(FRONT,angle60 ,FRONT,angle180);
									}		
								}
						if((s & 0x0200) != 0)  //收到右边信号
								{			 
										 stop_rap();
									 
									 if(l_ring.length > angle45)
											 {
												mode.step_bp = 3;							 
												enable_rap(FRONT,angle60 ,FRONT,angle180);
											 }
									 else 
											 {
												mode.step_bp = 9;							 
												enable_rap(FRONT,r_ring.length*2/3 ,FRONT,l_ring.length*2/3);		
											 }
								}
						break;	
					//中间接收头收到右边信号，机器向左转，希望找到中间信号
				    case 3:
						if((l_rap.sign == 0)&&(r_rap.sign == 0))
								{
		//							mode.step_bp = 2;							 
		//							enable_rap(FRONT, angle180,FRONT,angle60);

									/////////////////////2015.8.6////////////////
									if((s & 0x0700) == 0) //中间接收头没收到信号
											{
												mode.step_bp = 5;	
											}
									////////////////////end2015.8.6//////////////
									else
											{
												mode.step_bp = 2;							 
												enable_rap(FRONT, angle180,FRONT,angle60);
											}
								}
						if((s & 0x0100) != 0)  //收到左边信号
								{
									stop_rap();
									if(r_ring.length > angle45)
											{
												mode.step_bp = 2;							 
												enable_rap(FRONT, angle180,FRONT,angle60);
											}
									else 
											{
												mode.step_bp = 9;							 
												enable_rap(FRONT,r_ring.length*2/3 ,FRONT,l_ring.length*2/3);		
											}
								}

					    break;
				    case 4:

						break;
					//中间接收头收到左边信号，机器向右转，希望找到中间信号	  
					case 5:
						if(do_action(2,angle120))
								{
									mode.step_bp = 6;
								}
						if((s & 0x0700) != 0) //中间接收头没收到信号
								{
									mode.step_bp = 0;
									mode.step_mk = 0;	
								}

						break;	
					case 6:
						if(do_action(1,angle240))
								{
									mode.step_bp = 0;
									mode.step_mk = 0;
									mode.step = 0;
								}
						if((s & 0x0700) != 0) //中间接收头收到信号
								{
									mode.step_bp = 0;
									mode.step_mk = 0;	
								}

						break;
					
					case 7:

						break;
					case 8:	

						break;				  
					case 9:
						if((l_rap.sign == 0)&&(r_rap.sign == 0))
								{
									mode.step_bp = 11;								
								}
					    break;
					case 10:
						//在发现碰撞信号时停下等待2秒	
						if((giv_sys_time - piv_stime) > 10000)	//10000
							{
									mode.step_mk = 4;
								mode.step_bp = 0;
							}
						break;
					case 11:	
					    do_action(3,angle180);
						/////////////////////2015.8.6////////////////
						if((s & 0x0100) != 0)  //收到左边信号
								{
									stop_rap();
									mode.step_bp = 2;
									enable_rap(FRONT,angle60 ,FRONT,angle20);	
								}
						if((s & 0x0200) != 0)  //收到右边信号
								{
									stop_rap();
									mode.step_bp = 3;
									enable_rap(FRONT,angle20 ,FRONT,angle60);	
								}
						///////////////////end2015.8.6////////////////
						break;
					default :
					    mode.step_bp = 0;
						stop_rap();
				}
				return 1;   //结束收到中间区域的程序
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
			//机器在中间区域没有对准电极时掉头往回走
			case 4:
			    switch(mode.step_bp)
				{
				    case 0:	
					    if(do_action(4,angle45))
						{
						    mode.step_bp = 1;
						}
						break;
					case 1:
					    if(do_action(1,angle180))
						{
						   mode.step_bp = 2;
						}
						break;
					case 2:
					    if(do_action(3,METE_H5))
						{
						   mode.step_bp = 4;
						}
						if((m != 0)&&(m < 7))
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						break;
					case 3:
					    if(do_action(4,angle45))
						{
						    mode.step_bp = 4;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 4;
						}
						break;
					case 4:
					    if(do_action(2,angle180))
						{
						   mode.step_bp = 0;
						   mode.step = 0;
						   mode.step_mk = 0;
						}
						break;
					default :
					    mode.step_bp = 0;
						stop_rap();

				}
				return 1;
				
				
				
				
				
				
				
				
			case 5:
			    if(do_action(3,120))
							{
														if((power.charge_dc == 1) || (power.charge_seat == 1))
														{
																Init_Chargeing();
																return 1;
														} 
														else
														{
															mode.step_mk = 4;
														}
							}  
					if(( l_rap.rap > 200) || (r_rap.rap > 200) )
							{
										l_rap.rap = 200;
										r_rap.rap = 200;
							}
//			    if((1 == l_bump.key)||(1 == l_bump.key))
//				{
				    if((l_ring.length > 30)|| (r_ring.length > 30)||((giv_sys_time-action.time) > 800))
									{
												stop_rap();
															if((power.charge_dc == 1)||(power.charge_seat == 1)||((giv_sys_time-action.time) > 10000))
															{
																	Init_Chargeing();
																	return 1;
															} 
															else
															{
																mode.step_mk = 4;
															}
									}
//				}
				return 1;
									
			default :
			    mode.step_mk= 0;
				return 0;
		}
 //return 1;
}
