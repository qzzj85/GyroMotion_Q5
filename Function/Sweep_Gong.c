////////////////////////头文件////////////////////////////////////
#include "AAA-Include.h"
#include "math.h"

u8 Gong_Lj[]={3,2,3,1,3,2,3,1};	//QZ
float Gong_Ljl[]={METER_PLUS,angle180,METER_PLUS,angle180,METER_PLUS,angle180,METER_PLUS,angle180};	//QZ
u8 Wall_Close_Flag=0; //QZ
u32 GONG_LENGTH=METER_PLUS;
u8 YBS_GONG_FLAG=0;
Gong S_Gong;

#define L_BUMP_MASK 0X0200
#define R_BUMP_MASK 0X0100
#define E_L_MASK    0X0080
#define E_M_MASK    0X0040
#define E_R_MASK    0X0020
#define W_L_MASK 	0X0010
#define W_LM_MASK 	0X0008
#define W_M_MASK    0X0004
#define W_RM_MASK	0X0002
#define W_R_MASK 	0X0001
#define PI			3.141592

////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
///////////////////////私有变量////////////////////////////////////
///////////////////////全局函数////////////////////////////////////
///////////////////////私有函数////////////////////////////////////	
///////////////////////函数实体////////////////////////////////////
/*****************************************************************
功能：初始化自动清扫
*****************************************************************/
//QZ ADD
// bit9 l_bump;
// bit8 r_bump;
// bit7 e_l;
// bit6 e_m;
// bit5 e_r;
// bit4 w_l;
// bit3 w_lm;
// bit2 w_m;
// bit1 w_rm;
// bit0 w_r;
// 0无碰撞，1有碰撞
u8 Read_Bump_Gong(void)
{
	u16 result=0;
	
#if 0			//ZDK屏蔽
	
	   if((e_l.sign == FARN))
	   {
		  if((mode.bump > 4) || (mode.bump == 0))		//左地检悬空
		   { 
						mode.bump = 1;// E_L;
						mode.step_bp=0;
						stop_rap();
			}
			return 1;//E_L;
	   }   
	   if((e_lm.sign==FARN))
	   {
				if((mode.bump > 4) || (mode.bump == 0)) 	
					 {
							mode.bump= 2;//E_LM;
							mode.step_bp=0;
							stop_rap();
					 }
				return 2;//E_LM;
		}
	  if((e_rm.sign==FARN))
	  {
		  if((mode.bump > 4) || (mode.bump == 0))		
				 { 
					mode.bump=3;//E_RM;
					mode.step_bp=0;
					stop_rap();
				 }
		  return 3;//E_RM;
	  }  
		
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
	   
#endif	 		
		  if (1 == l_bump.key)
		 {
			 if((mode.bump == 0))	 //左前撞
				 {
							mode.bump=5;//L_BUMP;
							mode.step_bp=0;
							stop_rap();
					}
			return 5;//L_BUMP;
//				result|=L_BUMP_MASK;
		 }
		 
	   if (1 == r_bump.key)
		 {
				if((mode.bump == 0))	 //右前撞
						{
								mode.bump=6;//R_BUMP;
								mode.step_bp=0;
								stop_rap();
						}
				return 6;//R_BUMP;
//				result|=R_BUMP_MASK;
		 }
		
		if((w_l.sign==NEAR)&&(w_l.on==1))
		{
			if((mode.bump == 0))		//左墙检靠近墙
			{
					mode.bump=7;//W_L;
					mode.step_bp=0;
					if(S_Gong.turn_step==0)
						stop_rap();
			 }	  
		  return 7;//W_L;
//		  result|=W_L_MASK;
		}

		 if((w_lm.sign==NEAR)&&(w_lm.on==1))
		 {
			  if((mode.bump == 0))		  //左中墙检靠近墙
			  {
					 mode.bump=8;//W_LM;
					 mode.step_bp=0;
					 stop_rap();
			  }
		   
		   return 8;//W_LM;
//		   result|=W_LM_MASK;
		}

		 
		 if((w_m.sign==NEAR)&&(w_m.on==1))
		  {
			   if((mode.bump == 0)) 	  //中墙检靠近墙
			   {
					   mode.bump=9;//W_M;
					   mode.step_bp=0;
					   stop_rap();
				}
			 return 9;//W_M;
//			 result|=W_M_MASK;
		  }
		 
//		 if((rm_hw.effect)||(dummy_wall.effect))
//		 {
//				 if(//((rm_hw.data & 0x8f) == 0)|| 
//					 (rm_hw.data==0xec) || (rm_hw.data==0xCC) || 
//				 (rm_hw.data==0xdc) || (dummy_wall.effect))
//			 {
//						 if((mode.bump == 0))
//					 {
//							 mode.bump=10;//INFRARED;
//							 mode.step_bp=0;
//							 stop_rap();
//					 }
//					 //return 10;//INFRARED;
//			 }
//		 } 
		 
			 
			 
	
	  if((w_rm.sign==NEAR)&&(w_rm.on==1))
	  {
			if((mode.bump == 0))		//右中墙检靠近墙
			{
					mode.bump=15;//W_RM;
					mode.step_bp=0;
					stop_rap();
			 }
		  return 15;//W_RM;
//			result|=W_RM_MASK;
	   }
	
	  if((w_r.sign==NEAR)&&(w_r.on==1))
	  {
			if((mode.bump == 0))		//右墙检靠近墙		
			{
					mode.bump=14;//W_R;
					mode.step_bp=0;
					if(S_Gong.turn_step==0)
						stop_rap();
			 }	
			 return 14;//W_R;
//			 result|=W_R_MASK;
	  }
	
	
//		if(mode.action == 3)
//		{
//			if((Ultra.Distance[0] < 90) & (Ultra.Distance[0]  > 1))
//			{
//				mode.bump = 99; 		//	超声停止
//				mode.step_bp=0;
//				stop_rap();
//				return 6;
//			}
//		}
						
		return result;
	
	
}

void Init_S_Gong(void)
{
	S_Gong.angle=0;
	S_Gong.bump_time=0;
	S_Gong.length_h=400;
	S_Gong.length_h_rest=0;
	S_Gong.length_w=400;
	S_Gong.length_w_rest=0;
	S_Gong.status=0;
	S_Gong.time=1;	//0代表左转开始，1代表右转开始
	S_Gong.turn_dir=0;
	S_Gong.status=0;
	S_Gong.turn_step=0;
	S_Gong.first_time_value=S_Gong.time;
}

void Init_Sweep_Gong(void)
{					 
    /******初始化显示***********/
//    clr_display();
//    Dis_On = true;            //显示标志。
//    Dis_PrOn = true; 			//预约天数亮
//    Dis_HourOn = true; 		//小时亮
//    Dis_MinuteOn = true; 		//分钟亮
//    Dis_ColOn = true; 		//时间点亮
//    Dis_WeekOn = true; 		//星期亮
//    Dis_SpeedOn = true; 		//速度亮
//    Dis_PowerOn = true; 		//电池图标亮
//    Dis_ChargeOn = false ;		//充电图标亮
//	Dis_ColGlint = true ;	    //显示时间点为闪烁
//	Dis_AuGlint = true;
//	
//	Dis_SpeedGo = true; 
	/******初始化设置的值********************/
	gsv_work_mode.work_mode = SWEEP_GONG;	 //工作模式为清扫
	mode.mode = SWEEP_GONG;
	clr_ram();

	Init_S_Gong();
	
    Enable_Sweep();

	Enable_earth();
	Enable_wall();
//	enable_hwincept();//允许红外接收电源
//	Reset_Speed_Send();  //允许红外发送  
	gbv_action_m_fangxiang = false; 	//false:向前
	Display_Real();
	Init_Action();
	mode.step = 0x70;
	WriteWorkState();
//	ReInitAd();
}

void wall_diff_check(void)
{
	static u8 flag=0;
	if((w_l.difference>=0x90||w_r.difference>=0x100||w_m.difference>=0x90||w_lm.difference>=0x90||w_rm.difference>=0x90)&&flag==0)
	{
		r_rap.rap/=2;
		l_rap.rap/=2;
		flag=1;
	}
	if(w_l.difference>=0x200||w_r.difference>=0x200||w_m.difference>=0x200||w_lm.difference>=0x200||w_rm.difference>=0x200)
	{
		flag=0;
		stop_rap();
	}
}
/******************************************************************
功能：在清扫时的工作
******************************************************************/
u8 YBS_Gong(u32 length)
{
	u32 length_w=length;
	if(do_action(3,length_w))
		{
			stop_rap();
			return 0;
		}
	yq_pwm();
	return 1;
 }
	//QZ ADD:
	//定点弓形清扫，以边长为距离，比如1MX1M的弓形清扫
u8 Sweep_Gong_Spot(u32 length_w,u32 length_h)
{
	u32 cycle,time_delay;
//	static u8 time=0;
//	static u8 turn_dir=0;
//	S_Gong.turn_step=0;
	cycle=length_h/(25);
	
	length_w=(u32)(length_w*CM_PLUS);
//	length_h=(u32)(length_h*CM_PLUS);
	S_Gong.length_w_rest=length_w;
	S_Gong.length_h_rest=length_h;
	if (cycle==0)
		return 1;
	if(S_Gong.time%2==0)
	{
		Speed=2400;
		S_Gong.status=0;
		S_Gong.angle=0;
//		if((l_rap.sign==0)&&(r_rap.sign==0))
//		{
			if(do_action(3,length_w))
			{
				if(S_Gong.time==2*cycle)
				{
					stop_rap();
					S_Gong.time=0;
					cycle=0;
					return 1;
				}
				else
					S_Gong.time++;
//				time_delay=giv_sys_time;
//				while(giv_sys_time-time_delay<10000);
			}

//		}
		return 0;
	}
	if(S_Gong.time%2!=0)
	{	
		S_Gong.status=1;
//		if((l_rap.sign==0)&&(r_rap.sign==0))
//		{
			if(S_Gong.turn_dir==0)
			{
//				Speed=1200;
				switch (S_Gong.turn_step)
				{
					case 0:
					if(do_action(2,angle90))	//向右转90度
					{
						stop_rap();
						S_Gong.turn_step++;
//						time_delay=giv_sys_time;
//						while(giv_sys_time-time_delay<10000);
						S_Gong.angle+=angle90;
					}
					return 0;
					break;
//					case 1:
//						if(do_action(3,(u32)(25*CM_PLUS)))	//前进25公分
//						{
//							stop_rap();
//							S_Gong.turn_step++;
//							time_delay=giv_sys_time;
//							while(giv_sys_time-time_delay<10000);
//						}
//						return 0;
//						break;
					case 1:
						if(do_action(6,angle90_D))	//向右转90度
							{
								stop_rap();
								S_Gong.turn_step=0;
								S_Gong.turn_dir=1;
								S_Gong.time++;
//								time_delay=giv_sys_time;
//								while(giv_sys_time-time_delay<10000);
								S_Gong.angle=0;
							}
						return 0;
						break;
					}		
			}
			else
			{
//				Speed=1200;
				switch (S_Gong.turn_step)
				{
					case 0:
					if(do_action(1,angle90))	//向左转90度
					{
						stop_rap();
						S_Gong.turn_step++;
//						time_delay=giv_sys_time;
//						while(giv_sys_time-time_delay<10000);
						S_Gong.angle+=angle90;
					}
					return 0;
					break;
//					case 1:
//						if(do_action(3,(u32)(25*CM_PLUS)))
//						{
//							stop_rap();
//							S_Gong.turn_step++;
//							time_delay=giv_sys_time;
//							while(giv_sys_time-time_delay<10000);
//						}
//						return 0;
//						break;
					case 1:
						if(do_action(5,angle90_D))	//向左转90度
							{
								stop_rap();
								S_Gong.turn_step=0;
								S_Gong.turn_dir=0;
								S_Gong.time++;
//								time_delay=giv_sys_time;
//								while(giv_sys_time-time_delay<10000);
								S_Gong.angle=0;
							}
						return 0;
						break;
					}		
			}
//		}
	}
	return 0;
}

//S_Gong.time为0时,从左转开始;为1时,从右转开始
u8 Sweep_Gong_Spot_1(u32 length_w,u32 length_h)
{
	u32 cycle,time_delay;
//	static u8 time=0;
//	static u8 turn_dir=0;
//	S_Gong.turn_step=0;
	cycle=length_h/(25);
	
	length_w=(u32)(length_w*CM_PLUS);
//	length_h=(u32)(length_h*CM_PLUS);
	S_Gong.length_w=length_w;
	S_Gong.length_h=length_h;
	if (cycle==0)
		return 1;
	switch (S_Gong.turn_step)
		{
			case 0:
				Speed=2400;
				if(do_action(3,length_w))
					{
						if(S_Gong.time==cycle+S_Gong.first_time_value)	//
						{
							stop_rap();
							S_Gong.time=0;
							cycle=0;
							return 1;
						}
						else
							S_Gong.turn_step++;
					}
				return 0;
			case 1:
				Speed=1200;
				if(do_action(S_Gong.time%2+1,angle90))
					{
						stop_rap();
						S_Gong.turn_step++;
						S_Gong.angle+=angle90;
					}
				return 0;
			case 2:
				if(do_action(S_Gong.time%2+5,angle90_D))
					{
						stop_rap();
						S_Gong.turn_step=0;
						S_Gong.angle=0;
						S_Gong.time++;
						S_Gong.bump_time=0;
					}
				return 0;
			default:
				break;
		}
}


u8 Action_Bump_Gong(void)
{
	u32 m;
	u8 result=0;
	
	m=read_bump();
	switch (mode.bump)
	{
		case 9:
			switch (mode.step_bp)
				{
					case 0:

						if(do_action(4,5*CM_PLUS))

							{
								stop_rap();
								mode.step_bp++;
								return 1;
							}
						return 1;
						break;
					case 1:
//						stop_rap();
						S_Gong.bump_time++;
						S_Gong.length_w_rest-=r_ring.length;
						Speed=1200;
						if(S_Gong.turn_dir==0)
							{
								if(do_action(2,angle90))	//向右转
								{
									stop_rap();
									mode.step_bp++;
									S_Gong.angle+=angle90;
//									return 1;
								}
//								if((m!=0)&&(m&W_L_MASK))
//								{

//									S_Gong.bump_time++;
//									stop_rap();
//									S_Gong.angle=r_ring.length;
//									mode.bump=7;
//									mode.step_bp=0;
//									return 1;
//								}
//								return 1;
							}
						else
							{
								if(do_action(1,angle90))	//向左转  
								{
									stop_rap();
									mode.step_bp++;
									S_Gong.angle+=angle90;
//									return 1;
								}
//								if((m!=0)&&(m&W_R_MASK))
//								{
//							
//									S_Gong.bump_time++;
//									stop_rap();
//									S_Gong.angle=r_ring.length;
//									mode.bump=14;
//									mode.step_bp=0;
//									return 1;
//								}
							}
						if(S_Gong.angle>=angle180)
							{
								if((m!=9))

									{
										stop_rap();
										S_Gong.time+=2;
										S_Gong.angle=0;
										S_Gong.turn_step=0;
										S_Gong.turn_dir=!S_Gong.turn_dir;
										mode.bump=0;
										mode.step_bp=0;
										return 0;								
									}
							}
						return 1;
						break;
					case 2:
						if(S_Gong.turn_dir==0)
							{
								if(do_action(6,angle90_D))	//右轮不动，右转
								{
									stop_rap();
									S_Gong.time+=2;
									S_Gong.angle=0;
									S_Gong.turn_step=0;
									S_Gong.turn_dir=!S_Gong.turn_dir;
									mode.bump=0;
									mode.step_bp=0;
									return 0;								
								}
							}
						else
							{
								if(do_action(5,angle90_D))	//左轮不动，左转
								{
									stop_rap();
									S_Gong.time+=2;
									S_Gong.angle=0;									
									S_Gong.turn_step=0;
									S_Gong.turn_dir=!S_Gong.turn_dir;
									mode.bump=0;
									mode.step_bp=0;
									return 0;								
								}
							}
						if((m!=0)&&(m!=7)&&(m!=14))
							{
								S_Gong.bump_time++;
								stop_rap();
								S_Gong.length_w_rest-=r_ring.length;
								mode.bump=m;
								mode.step_bp=0;
								return 1;								
							}
//						if(m==9)
//							{
//								//遇到拐角
//								stop_rap();
//								mode.step_bp=0;
//								
//							}
						return 1;							
					default:
						return 1;
				}
		break;
		case 8:
			S_Gong.angle+=r_ring.length;
			switch (mode.step_bp)
				{
//					case 10:
//					if(do_action(4,5*CM_PLUS))
//						{
//							stop_rap();
//							mode.step_bp++;
//							return 1;
//						}
//					return 1;
					case 0:
						if(S_Gong.turn_dir==0)	//需要右转
							{
								if(S_Gong.angle<angle90)
									{
										if(do_action(2,angle90))
											{
												stop_rap();
												S_Gong.angle+=angle90;
												mode.step_bp++;
												return 1;
											}
										return 1;
									}
								else
									{
										if(do_action(2,angle60))
											{

												if(m!=9)
													{
													stop_rap();
													S_Gong.time+=2;
													S_Gong.angle=0;													
													S_Gong.turn_step=0;
													S_Gong.turn_dir=!S_Gong.turn_dir;
													mode.bump=0;
													mode.step_bp=0;
													return 0;								
													}
											}
										return 1;
									}
							}
						else			//turn_dir==1,需要左转
							{
								if(S_Gong.angle<angle90)
									{

										if(do_action(1,angle180))
											{
												stop_rap();
												S_Gong.time+=2;
												S_Gong.angle=0;												
												S_Gong.turn_step=0;
												S_Gong.turn_dir=!S_Gong.turn_dir;
												mode.bump=0;
												mode.step_bp=0;
												return 0;
											}
										return 1;
									}
								else
									{
									if(do_action(1,45*Angle_1))
										{

											if(m!=9)
												{
												stop_rap();
												S_Gong.time+=2;
												S_Gong.angle=0;
												S_Gong.turn_step=0;												
												S_Gong.turn_dir=!S_Gong.turn_dir;
												mode.bump=0;
												mode.step_bp=0;
												return 0;								
												}
										}
										return 1;
									}
							}
					case 1:
						if(do_action(6,angle90_D))
							{
								if(m!=9)
									{
										stop_rap();
										S_Gong.angle=0;
										S_Gong.time+=2;
										S_Gong.turn_step=0;
										S_Gong.turn_dir=!S_Gong.turn_dir;
										mode.bump=0;
										mode.step_bp=0;
										return 0;
									}
							}
						return 1;
					default:
						break;
				}
		case 15:
			S_Gong.angle+=r_ring.length;
			switch (mode.step_bp)
				{
					case 0:
						if(do_action(4,5*CM_PLUS))
							{
								stop_rap();
								mode.step_bp++;
								return 1;
							}
						return 1;
					case 1:
						if(S_Gong.turn_dir==0)
							{
								if(S_Gong.angle<angle90)
									{
										if(do_action(2,angle180))
											{
												if(m!=9)
													{
														S_Gong.angle=0;
														S_Gong.time+=2;
														S_Gong.turn_step=0;
														S_Gong.turn_dir=!S_Gong.turn_dir;
														mode.bump=0;
														mode.step_bp=0;
														return 0;
													}
											}
										return 1;
									}
								return 1;
							}
						else
							{
								if(S_Gong.angle<angle90)
									{
										if(do_action(1,angle90))
											{
												S_Gong.angle+=angle90;
												mode.step_bp++;
												return 1;
											}
										return 1;
									}
								else
									{
										if(do_action(1,angle60))
											{
												
											}
										}
										
							}
				}
			
			
		
		default:
			break;
			
	return 1;
	}
}

u8 Action_Bump_Gong_1(void)
{
	u32 m;
	u8 result=0;
	static float degree=0;
	
	m=Read_Bump_Gong();

//	printf("Mode.Bump=%d\r\n",mode.bump);
//	printf("m=%d\r\n",m);
//	printf("S_Gong.turn_step=%d\r\n",S_Gong.turn_step);
	
	switch (mode.bump)
	{
		case 9:
//			switch (mode.step_bp)
//				{
//					case 0:
//						Speed=1200;
//						if(do_action(4,5*CM_PLUS))

//							{
//								stop_rap();
//								mode.step_bp++;
//								return 1;
//							}
//						return 1;
//						break;
//					case 1:
//						S_Gong.bump_time++;
//						Speed=1200;
//						if(do_action(S_Gong.time%2+1,angle90))
//							{
//								stop_rap();
//								mode.step_bp++;
//								S_Gong.angle+=90;
//								S_Gong.turn_step+=2;
//								if(S_Gong.turn_step>=3)
//									S_Gong.turn_step=2;
//								if(S_Gong.angle>=180)
//									{
//										S_Gong.time++;
//										S_Gong.turn_step=0;
//									}
//								mode.bump=0;
//								mode.step_bp=0;
//								return 0;
//							}
//						return 1;
//				}
//		break;		
		case 5:
		case 6:
//			switch (mode.step_bp)
//				{
//					case 0:
//						if(S_Gong.turn_step==2)
//							S_Gong.angle+=r_ring.length;
//						mode.step_bp++;
//						return 1;
//					case 1:
//						Speed=1200;
//							if(do_action(4,5*CM_PLUS))
//								{
//									stop_rap();
//									mode.step_bp++;
//								}
//							return 1;
//					case 2:
//						if(S_Gong.turn_step==0)
//							{
//								if(do_action(S_Gong.time%2+1,angle90))
//									{
//										stop_rap();
//										S_Gong.angle+=angle90;
//										S_Gong.turn_step+=2;
////										S_Gong.time++;
//										mode.bump=0;
//										mode.step_bp=0;
//										return 0;
//									}
//								return 1;
//							}
//						if(S_Gong.turn_step==2)
//							{
//								if(do_action(S_Gong.time%2+1,(angle90-S_Gong.angle%angle90)))
//									{
//										stop_rap();
//										S_Gong.angle=0;
//										S_Gong.turn_step=0;
//										S_Gong.time++;
//										mode.bump=0;
//										mode.step_bp=0;
//										return 0;
//									}
//								return 1;
//							}
//					default:
//						return 1;
//				}
		case 8:
		case 15:
			switch (mode.step_bp)
				{
					case 0:
						if(S_Gong.turn_step==2)
						{
//							if(r_ring.length>l_ring.length)
//								S_Gong.angle+=r_ring.length;
								
//							else
//								S_Gong.angle+=l_ring.length;
//							S_Gong.angle+=r_ring.length;
							S_Gong.temp_length=(r_ring.length>l_ring.length)?r_ring.length:l_ring.length;
						}
						mode.step_bp++;
						return 1;
					case 1:
						Speed=1200;
							if(do_action(4,4*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							return 1;
					case 2:
						if(S_Gong.turn_step<2)
							{
								if(do_action(S_Gong.time%2+1,angle90))
									{
										stop_rap();
										S_Gong.angle+=angle90;
										S_Gong.turn_step=2;
//										S_Gong.time++;
										mode.bump=0;
										mode.step_bp=0;
										return 0;
									}
								return 1;
							}
						if(S_Gong.turn_step==2)
							{
//								if(do_action(S_Gong.time%2+1,(angle90-S_Gong.angle%angle90)))
								if(do_action(S_Gong.time%2+1,(u32)(angle90*((float)(angle90_D-S_Gong.temp_length)\
									/(float)(angle90_D)))))
									{
										stop_rap();
										S_Gong.angle=0;
										S_Gong.turn_step=0;
										S_Gong.time++;
										S_Gong.bump_time++;
										mode.bump=0;
										mode.step_bp=0;
										return 0;
									}
								return 1;
							}
					default:
						return 1;
				}
		case 7:
//			Speed=1200;
//			if(S_Gong.turn_step==0)
//				{
//					if(((S_Gong.time%2+1)==1)||((S_Gong.time%2+1)==5))
//						{
//							switch (mode.step_bp)
//								{
//								case 0:
//									if(do_action(4,5*CM_PLUS))
//										{
//											mode.step_bp++;
//											return 1;
//										}
//									return 1;
//								case 1:
//									S_Gong.turn_step++;
//									mode.bump=0;
//									mode.step_bp=0;
//									return 0;
//								}
//						}
//					else if(((S_Gong.time%2+1)==2)||((S_Gong.time%2+1)==6))
//						{
//							switch (mode.step_bp)
//								{
//									case 0:
//										S_Gong.length_w_rest=S_Gong.length_w-r_ring.length;
//										if(S_Gong.length_w_rest<50)	//小于50个脉冲，无需做辅助动作，直接进入下一动作
//											{
//												S_Gong.turn_step++;
//												mode.bump=0;
//												mode.step_bp=0;
//												return 0;
//											}
//										else
//											{
//												mode.step_bp++;
//												return 1;
//											}
//									case 1:
//										if(do_action(6,angle360))
//											{
//												stop_rap();
//												while(1);
//											}
//										if(m==0)
//											{
//												stop_rap();
//												S_Gong.temp_length=r_ring.length;
//												S_Gong.angle+=l_ring.length;
//												mode.step_bp++;
//												return 1;												
//											}
//										return 1;
//									case 2:
//										if(do_action(5,angle360))
//											{
//												stop_rap();
//												while(1);
//											}
//										if(m==7)
//											{
//												stop_rap();
//												S_Gong.angle-=r_ring.length/2;
//												degree=(float)(S_Gong.angle/angle90)*90;
//												S_Gong.length_w_rest=(u32)(S_Gong.length_w_rest*cos(degree*PI/180));
//												mode.step_bp++;
//												return 1;
//											}									
//										return 1;
//									case 3:
//										Speed=2400;
//										if(!YBS_Gong(S_Gong.length_w_rest))
//											{
//													stop_rap();
//												mode.bump=0;
//												mode.step_bp=0;
//												S_Gong.length_h_rest=0;
//												S_Gong.turn_step++;
//												return 0;
//											}
//												
// 										if((m!=7))
//											{
//												stop_rap();
//												mode.bump=m;
//												mode.step_bp=0;
//												return 1;
//											}
//										
//								}
//						}
//									
//							
//				}
//			else 
//				{
//					mode.bump=0;
//					mode.step_bp=0;
//					return 0;
//				}
//			return 1;
		case 14:
			mode.bump=0;
			mode.step_bp=0;
			return 0;
		
		default:
			return 0;
			break;
			
	}
}

u8 Do_Sweep_Gong(void)
{
 u8 y = 0;//有碰撞信号时 y = 1;否则为0
 u8 m;
// u8 z = 0;//在执行来回扫时 z = 1,否则为0
u8 num[20];	//QZ
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
	 return 1;
  }
  //在最开始启动时判断左右轮是否离地，否则退出扫地模式
  if((giv_sys_time - mode.time) < 5000)
  {
    if((l_lidi.key==0)||(r_lidi.key==0))  
//		if((lidi.key == 0)||(lidi.key == 0))
	  {
	      Init_Cease();
		  return 1;
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
	  return 1 ;
  }
  ///遥控器按键管理		     
  if(Sweep_YaoKong_Manage() == 1)
  {
      return 1;
  }

  ///在面板上有三个按键中的任意一个按下了
////  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
////  {
////      key1.sign = 1;
////	  key2.sign = 1;
////	  key3.sign = 1;
////	  Init_Cease();	 
////	  Enable_Beep();
////	  return 1;
////  }
  
  //hzh   20171204
	if(key1.sign == 0)		
	{
	   key1.sign = 1;  
	   return 1;
	}			 
	if(key2.sign == 0)	
	{
	   key2.sign = 1; 	   
	   Init_Cease();
	   return 1;
	}
	if(key3.sign == 0)	
	{
	   key3.sign = 1;  
	   Init_Docking();
     return 1;
	}
  
  
  //////显示显示屏的内容
  Display_Real();
//  comm_rap();
  ACC_DEC_Curve();
//  clr_hw_effect(&l_hw);
//  clr_hw_effect(&lm_hw);
//  clr_hw_effect(&r_hw);	
//  clr_hw_effect(&rm_hw); 	
//  clr_hw_effect(&dummy_wall); 
  ////下面是一些需要驱动的行动////////////
  Io_Driver();	 
//  Dis_Power = Get_Dispower();	

//  do_action(22,100,100);

  ////在靠近墙壁时机器减速
//  if((w_l.high_sign == NEAR)||(w_lm.high_sign == NEAR)||(w_m.high_sign == NEAR)||
//     (w_r.high_sign == NEAR)||(w_rm.high_sign == NEAR))
//		if((w_lm.sign==NEAR)||(w_m.sign==NEAR)||(w_rm.sign==NEAR))
//			{
//					if((l_rap.rap > 1000)||(r_rap.rap > 1000))
//						{
//								l_rap.rap = (l_rap.rap*2)/3;
//								r_rap.rap = (r_rap.rap*2)/3;
//						}
//												
//					Wall_Close_Flag=1;					
////					stop_rap();
//			}
//  else
//			{
//					if((l_rap.rap < Speed)&&(r_rap.rap < Speed)&&(mode.bump == 0))
//							{
//									l_rap.rap = (l_rap.rap*3)/2;
//									r_rap.rap = (r_rap.rap*3)/2;
//							}
//			}

  ACC_DEC_Curve();
	if(Action_Bump_Gong_1())
		return 1;
	Sweep_Gong_Spot_1(400,400);
  if(S_Gong.bump_time>6)
  	{
  		stop_rap();
		while(1);
  	}
}
