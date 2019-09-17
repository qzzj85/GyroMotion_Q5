////////////////////////头文件////////////////////////////////////
#include "AAA-include.h"

static u8 piv_out;	//机器是否向外展开，1为向外展开，0为向里缩小
static u8 piv_left; //机器是否向左转，1为向左转，0为向右转
static u8 piv_done;   //qz add，机器完成定点为1,未完成为0，20180125

////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////
void Do_Spot(void);
void Init_Spot(void);
///////////////////////私有函数////////////////////////////////////
u8 Spot_YaoKong_Manage(void);	
u32  read_bump1(void);
///////////////////////函数实体////////////////////////////////////
/*****************************************************************
功能：初始化定点清扫的程序
*****************************************************************/
void Init_Spot(void)
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
	Dis_SpGlint = true;
	/******初始化设置的值********************/
	mode.sub_mode = SPOT;	 //工作模式为清扫
	mode.mode = SPOT;
	clr_ram();
	

	Enable_earth();
	//Disable_wall();
	if(DOCK_PREPARE)
		{
			Disable_wall();
			Reset_Wall_Send();
			DOCK_PREPARE=false;
			Speed=1000;
		}
	else
		{
			Enable_wall();
			Set_Wall_Send();
			Speed=4000;
		}
//	enable_hwincept();//允许红外接收电源
//	Reset_Speed_Send();  //允许红外发送  
	 
	Display_Real();
	Init_Action();
//	ReInitAd();
	////////////////清除墙检数据
    w_l.sign=FARN;
    w_lm.sign=FARN;
    w_r.sign=FARN;
    w_rm.sign=FARN;
    w_m.sign=FARN;
	WriteWorkState();

	piv_left=1;
	piv_done=0;
	piv_out=1;

}

/*****************************************************************
功能：定点清扫程序
*****************************************************************/
void Do_Spot(void)
{
//  static u8 piv_out;	//机器是否向外展开，1为向外展开，0为向里缩小
//  static u8 piv_left; //机器是否向左转，1为向左转，0为向右转
//  static u8 piv_done;   //qz add，机器完成定点为1,未完成为0，20180125

  u32 m ;
  s32 l_length,r_length;	//qz add 20180125
  static u8 bump_turn;
  		
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
						}
					return;
			}
			
  //在最开始启动时判断左右轮是否离地，否则退出扫地模式
  if((giv_sys_time - mode.time) < 5000)
		{
//				if((lidi.key == 0)||(lidi.key == 0))	////QZ:MASK
				if((l_lidi.key==0)||(r_lidi.key==0))	//QZ:add
					{
							Init_Cease();
							return ;
					}
		}
		
////////////  if((giv_sys_time - mode.time) < 100)
////////////		{
////////////			if(CheckMDustMotor())
////////////					{
////////////							return;
////////////					}
////////////		}
  ///遥控器按键管理		     
//  if(Spot_YaoKong_Manage() == 1)
//		{
//				return;
//		}
  ///在面板上有三个按键中的任意一个按下了
////  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
////		{
////      key1.sign = 1;
////			key2.sign = 1;
////			key3.sign = 1;
////			Init_Cease();	 
////			Enable_Beep();
////			return;
////		}
    
    
    
  //////显示显示屏的内容
  Display_Real();
 
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&rm_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  ////下面是一些需要驱动的行动////////////

	//系统有错误
	if(giv_sys_err != 0)
	{
	    Init_Err();
		return ;
	}
  
  ////有遥控器时机器停止按照遥控方向运动
//  if(YaoKongFangXiang())
//  {
//      return;
//  }
  ////是否有需要保护的内容
	
	#if 0
  m=read_bump1();
	switch(mode.bump)	 //bump碰撞的标志
    {
	    case 1://E_L		   
	        switch  (mode.step_bp)	 //step_bp碰撞时的步骤标志
		    {
			    case  0:
	                if(do_action(7,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return ; 
		            }
				    if((m==1))
				    {
				     return ;
				    }		   
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(7,angle30))
				    {
				        mode.step_bp++;
				    }
				    return ;	 
			    case  2:
			        if(do_action(8,angle15))
				    {
				        mode.step_bp++;
				    }	  
				    read_earth();
				    return ;
		        case  3:		
	                if(do_action(2,angle30))
		            {
		                mode.bump = 0;
		            }
				    read_earth();	
					return;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  ;
	    case 4://E_R: 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(8,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return ; 
		            }
				    if((m==4))
				    {
				         return ;
				    }		   
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(8,angle30))
				    {
				        mode.step_bp++;
				    }
				    return ;	 
			    case  2:
			        if(do_action(7,angle15))
				    {
				        mode.step_bp++;
				    }	
				    read_earth();
				    return ;
		        case  3:		
	                if(do_action(1,angle30))
		            {
		                mode.bump = 0;
		            }
				    read_earth();	
		            return;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  ;	 
				
			case 2://E_LM 
	    case 3://E_RM: 	 
	    case 5://L_BUMP:
	    case 6://R_BUMP:	
	    case 7://W_L:
	    case 8://W_LM:	
	    case 9://W_M:		 		    
	    case 10://up_hw			 	
	    case 11://l_hw	 	
	    case 12://rm_hw
	    case 13://r_hw	
	    case 14://W_R:	
	    case 15://W_RM:	
            switch(mode.step_bp)
								{
                case 0:
											if(do_action(4,angle360))
												{
													giv_sys_err = mode.bump;
													Init_Err();
													return ; 
												}		
											read_sideearth();
											if(m == 0)
												{
													stop_rap();
													mode.step_bp = 1;
												}
										return ;
							case 1:
								if(do_action(1,30*Angle_1))
								//if(do_action(1,angle30))
									{
											mode.bump = 0;
									}		 
								read_earth();	
								return;
									
	            default :
	                mode.bump = 0;
							}
			return ;
	    default :
	        mode.bump = 0;
	        break;
	} //结束  switch(mode.bump)	
	#endif

	#if 1		//qz add 20180125
	m=read_bump1();
	switch(mode.bump)
		{

			case 5:
			case 6:
			case 8:
			case 9:
			case 15:
				switch(mode.step_bp)
					{

						case 0:
							Speed=2400;
							if(do_action(4,5*CM_PLUS))
								{

									stop_rap();
									mode.step_bp++;
									return;
								}
							return;
						case 1:
							if(piv_left)
								bump_turn=2;
							else
								bump_turn=1;
							mode.step_bp++;
							return;
						case 2:
							Speed=2400;
							if(do_action(bump_turn,180*Angle_1))
								{
									stop_rap();
									if(m==0)
										{
											mode.bump=0;
											mode.step_bp=0;
											if(piv_left)
												piv_left=0;
											else
												piv_left=1;
											Speed=4000;
										}
									
								}
						
					}
		}
		#endif
		
	
	
	
	
	
  switch (mode.step)	//step路径执行的步骤
		{
			case 0:
				#if 0	//qz mask 20180125
				left(360*Angle_1);
//				left(angle360);//原地转圈1圈
				mode.step++;
				piv_out = 1;
				piv_left = 1;
				break;
				#endif
			
				#if 1
				if(!piv_done)
					{
						left(360*Angle_1);
						mode.step++;
						piv_out=1;
						piv_left=1;
					}
				else
					{
						if((!r_rap.sign)&&(!l_rap.sign))
							{
								if(do_action(1,(360*Angle_1)))
									{
										piv_done=0;					//qz add 准备下一次定点
										stop_rap();
//										Init_Cease();
//										Init_Commander();
										return;
									}
								return;
							}
					}
					break;
				#endif
			
			case 1:
			case 2:
			case 3:
			case 4:
				if((!l_rap.sign)&&(!r_rap.sign))
						{   
								action.step++;
//								if(piv_out == 0)
//										{
//												Init_Cease();
//												return;
//										}
								if(piv_left != 0)
										{
											//qz add
											l_length=(Angle_1*180)-mode.step*(Angle_1*45);
											if(l_length<0)
												l_length=0;
											r_length=(Angle_1*180)+mode.step*(Angle_1*45);
											if(r_length<0)
												r_length=0;
											//qz add end
												
//											enable_rap(BACK,(angle180-mode.step*angle45), FRONT,(angle180+mode.step*angle45));		//qz mask
											enable_rap(BACK,l_length, FRONT,r_length);												//qz add
										}
								else
										{
											//qz add
											l_length=(Angle_1*180)+mode.step*(Angle_1*45);
											if(l_length<0)
												l_length=0;
											r_length=(Angle_1*180)-mode.step*(Angle_1*45);
											if(r_length<0)
												r_length=0;
											//qz add end
											
//											enable_rap(FRONT,(angle180+mode.step*angle45),BACK,(angle180-mode.step*angle45) ); 
											enable_rap(BACK,l_length, FRONT,r_length);
										}
								
										//qz add
								if(piv_out!=0)
									mode.step++;
								else
									{
										mode.step--;
										piv_done=1;
									}
								
						}
				break;
						
						
				case   5:
				case   6:
				case   7:
				case   8:
				case   9:
				case   10:
				case   11:
				case   12:
				case   13:
				case   14:
				case   15:
				case   16:
				case   17:
					if((!l_rap.sign)&&(!r_rap.sign))
							{
								action.step++;
								if(piv_left != 0)
										{
												enable_rap(FRONT, (mode.step-4)*(45*Angle_1), FRONT, (mode.step-4)*(45*Angle_1)+360*(Angle_1));		//qz add									
//												enable_rap(FRONT, (mode.step-4)*angle45, FRONT, (mode.step-4)*angle45+angle360);					//qz mask 20180124
										}
								else
										{
												enable_rap(FRONT, (mode.step-4)*(45*Angle_1)+360*(Angle_1), FRONT, (mode.step-4)*(45*Angle_1));					//qz add
//												enable_rap(FRONT, (mode.step-4)*angle45+angle360, FRONT, (mode.step-4)*angle45);					//qz mask
										}
								if(mode.step == 17)
										{
												piv_out = 0;
										}
								if(piv_out != 0)
										{
												mode.step++;
										}
								else 
										{
												mode.step--;
										}
							}
				break; 
		}
}
/******************************************
读取碰撞信息
*******************************************/
u32  read_bump1(void)
{
if((e_l.sign == FARN))
   {
	  if((mode.bump > 4) || (mode.bump == 0))	    //左地检悬空
       { 
	      mode.bump = 1;// E_L;
	      mode.step_bp=0;
	      stop_rap();
	    }
	 return 1;//E_L;
   }   
//if((e_lm.sign==FARN))
//   {
//	  if((mode.bump > 4) || (mode.bump == 0))	    
//     {
//      mode.bump= 2;//E_LM;
//	  mode.step_bp=0;
//	  stop_rap();
//	 }
//	  return 2;//E_LM;
//    }
//if((e_rm.sign==FARN))
//  {
//	  if((mode.bump > 4) || (mode.bump == 0))	    
//     { 
//      mode.bump=3;//E_RM;
//	  mode.step_bp=0;
//	  stop_rap();
//	 }
//	  return 3;//E_RM;
//  }	 						
  if((e_r.sign==FARN))
   {
	  if((mode.bump > 4) || (mode.bump == 0))	   
     {	
      mode.bump=4;//E_R;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 4;//E_R;
   }
  if (0 == l_bump.key)		//qz modify:李工的机器为下降沿 20180125
    {
	 if((mode.bump == 0))	 //左前撞
     {
      mode.bump=5;//L_BUMP;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 5;//L_BUMP;
 	}
  if (0 == r_bump.key)		//qz modify:李工的机器为下降沿 20180125
    {
	if((mode.bump == 0))	 //右前撞
     {
      mode.bump=6;//R_BUMP;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 6;//R_BUMP;
	}      
  if((w_lm.sign==NEAR))
  {
   if((mode.bump == 0))		 //左中墙检靠近墙
     {
      mode.bump=8;//W_LM;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  
	  return 8;//W_LM;
   }
  if((w_m.sign==NEAR))
   {
   if((mode.bump == 0))	   //中墙检靠近墙
     {
      mode.bump=9;//W_M;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 9;//W_M;
   }	   
  if((w_rm.sign==NEAR))
  {
   if((mode.bump == 0))		//右中墙检靠近墙
     {
      mode.bump=15;//W_RM;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 15;//W_RM;
   }

return 0;
}
