////////////////////////头文件////////////////////////////////////
#include "AAA-Include.h"
////////////////////////////////////私有定义//////////////////////////////////
////////////const u16 M_PROTECTCURRENT =   466;//292;//中扫保护电流 500MA,计算公式  1A*0.47R*4096/3.3  
////////////const u16 M_STOPCURRENT =   788;//中扫停止电流 1350MA,计算公式  1A*0.47R*4096/3.3
////////////const u16 DUST_PROTECTCURRENT =   500; //灰尘风机电流 500MA						   
////////////const u16 WHEEL_PROTECTCURRENT =  292;//234; //左右轮电流 400MA

////////////const u16 BATTERY_PREOTECTTEMP = 815;  //电池保护温度  60度
////////////////////////////////////全局变量//////////////////////////////////
#define RING_FIX_OFFSET	500
#define	ROTATE_OFFSET	25

#define	PITCH_ANGLE_BIOS		550		//5.5度
#define	SPEEDUP_ANGLE_BIOS		200		//2度
#define	PITCHROLL_ANGLE_BIOS	500		//6度
#define	ROLL_ANGLE_BIOS			500		//5堵
#define	PITCH_TIME				10000	//500ms
#define	SPEEDUP_TIME			2000	//200ms

u8 sweep_level=0x01;//清扫吸力等级,//qz add 默认清扫吸力等级为标准
bool CHECK_STATUS_FLAG=false;
bool CHECK_STATUS_TIME=false;
const uint16 BATTERY_PREOTECTTEMP = 815;  //电池保护温度  60度
////////////////////////全局变量//////////////////////////////////

//qz add 20180316
ROTATE_SKID	Rotate_Skid;
FREE_SKID	Free_Skid;
#ifdef FREE_SKID_INDEP_CHECK
FREE_SKID 	Free_Skid_Indep;
#endif
#ifdef FREE_FIRST_BAD_CHECK		//qz add 20181011
FREE_SKID	Free_Skid_First;
#endif
#ifdef YBS_DIS_RESTORE
ROTATE_ANGLE rotate_angle;
#endif


MODE mode;
WORK_MODE gsv_work_mode;	
u8 giv_sys_err;         //系统的错误代码
int abnormal_gyrobios_cnt=0;

u8 read_sideearth(void);	
//动作   0:停止  1:原地左转  2原地右转 3前进   4后退   5旋转1  6：旋转2  。。。18旋转14	，19走螺旋线
/***********************************************************
在正常工作时的主路径：
前行1米，扫6圈，前行1米，左转90度，前行1.2米，右转90度，
前行1米，扫6圈，前行1米，左转90度，前行1.2米，右转90度，
前行1米，扫6圈，左转90度，前行1.2米，前行2米， 左转90度，
扫6圈，前行1米，左转90度，前行1.2米，右转90度，前行1米，
扫6圈，前行1米，左转90度，前行1.2米，右转90度，前行1米，
扫6圈，右转135度 结束，从头再开始
************************************************************/

/******************************************************
机器碰撞后的旋转的角度与碰撞次数的关系
*******************************************************/


///////////////////////私有变量////////////////////////////////////	
//u8 piv_display;         //系统显示电源，范围0--3； 
///////////////////////私有函数////////////////////////////////////
				

///////////////////////全局函数////////////////////////////////////
/*****************************************************************
功能：初始化路径
*****************************************************************/
void Init_Mode(void)
{
	u8 lastisdead=0;
	mode.mode=CEASE;
	mode.sub_mode=CEASE;
	mode.last_mode=CEASE;
	mode.last_sub_mode=CEASE;
	mode.step=0;
	mode.bump=0;
	mode.bump_time=giv_sys_time;
	mode.step_bp=0;
	mode.abnormity=0;
	mode.step_abn=0;
	mode.abn_time=giv_sys_time;
	mode.action=0;
	mode.All_Info_Abort=0;
	mode.Info_Abort=0;
	mode.time=giv_sys_time;
	mode.step_mk=0;
	mode.factory=false;							
	mode.factory_burnning=false;				
	lastisdead=Read_Last_Is_Dead();	
	Read_PwrSwitch_Status();
#ifdef SWITCHOFF_SUBMODE
	if(!power.switch_flag) 						//船型开关没有打开,进入SIWTCHOFF模式
		{
			Init_Switchoff();
		}
	else if(!lastisdead)						//如果没有充电标志，且上一次不是DEAD模式，那么进入CEASE模式
#endif
		{			
	//		Send_Voice(VOICE_SWEEPER_INIT);
	//		Send_Voice(VOICE_SWEEPER_INIT);
			Init_Cease();
			Open_Led(1,0,1);		//绿灯慢闪
			Send_Voice(VOICE_SWEEPER_OK);
			Open_Led(1, 0, 0);
		}
	else										//如果没有充电标志，且上一次是DEAD模式，那么这次还是直接进入DEAD模式，qz add 20181026
	//	if(lastisdead)								//qz add 20180823,如果上一次是DEAD模式，那么这次还是直接进入DEAD模式, qz mask 20181026
		{
#ifdef DEBUG_INIT
			TRACE("Last Mode is Dead!\r\n");
#endif
			Init_Dead();
		}
		
		__NOP();
		
	Init_Action();
}
//==================================================================================================
//==================================================================================================










//==================================================================================================
//==================================================================================================
///////////*****************************************************************
//////////功能: 运行工作模式
//////////*****************************************************************/
//////////void Action_Mode(void)
//////////{
//////////  switch (mode.mode)
//////////  {    
//////////      /*********************开机待机状态*******************/
//////////      case CEASE: 
//////////	      Do_Cease();   
//////////	      break;
//////////	  
//////////	  /*********************遥控器控制状态*****************/
//////////	  case TELE:
//////////	     // Do_Tele();
//////////	  break;
//////////	  
//////////	  /*********************找回充座状态*******************/
//////////	  case DOCKING:
//////////	     // Do_Docking();
//////////	  break;
//////////	  
//////////	  /*********************找回充座状态*******************/
//////////	  case CHARGEING:
//////////	      Do_Chargeing();
//////////	  break;
//////////	  
//////////	  /*********************找回充座状态*******************/
//////////	  case SLEEP:
//////////	      Do_Sleep();
//////////	  break;
//////////	  
//////////	  /*********************找回充座状态*******************/
//////////	  case ERR:
//////////	      //Do_Err();
//////////	  break;
//////////	  
//////////	  /*********************默认状态***********************/
//////////	  default :
//////////	      Init_Cease();	  
//////////  }
//////////}



/*****************************************************************
功能: 运行工作模式
*****************************************************************/
void Action_Mode(void)
{ 
	switch (mode.mode)
		{    
/*********************开机待机状态*******************/
			case CEASE: 
				switch(mode.sub_mode)
					{
						case CEASE:
							Do_Cease();
							break;
						case QUIT_CHARGING:
							Do_Quit_Chargeing();
							break;
						case ERR:
							Do_Err();
							break;
						case SHUTDOWN:		//qz add 20180901
						case DEAD:
							Do_Dead();
							break;
						case SLEEP:
							Do_Sleep_My();
							break;
						case SUBMODE_PAUSESWEEP:
							Do_PauseSweep();
							break;
						case SUBMODE_VIRTUAL_SLEEP:
							Do_VirtualSleep();
							break;
					}
			break;
	  
	  /*********************找回充座状态*******************/
			case DOCKING:
				switch(mode.sub_mode)
					{
						case DOCKING:
							Do_Docking_My();
							break;
						case YBS_SUB_RIGHT:
						case YBS_SUB_LEFT:
							Do_Docking_YBS();
							break;
					}
			break;
	  /*********************充电状态*******************/
			case CHARGEING:
				Do_Chargeing();
			break;
	  /*********************沿边状态*******************/
			case YBS:
				   YBS_YBS();        
			break;
	  /*********************速度控制状态*******************/
		/*********************自主清扫状态********************/
			case SWEEP:
				switch (mode.sub_mode)
					{
						case SWEEP_FIRST_INIT:
							Do_FirstInit_Sweep();
							break;
						case NORMAL_SWEEP:					//正常直行清扫
							Do_NormalSweep();
							break;
						case PASS2SWEEP:					//过度到清扫
							Do_Pass2Sweep();
							break;
//						case BACK2YBSSTART:
//							Do_Back2YbsStart();
//							break;
						case BACK_SWEEP:					//回扫直行清扫
							Do_BackSweep();
							break;
						case STOP_BACKSWEEP:
				//停止直行清扫
							//Do_Stop_BackSweep();							
							Do_Stop_BackSweep();
							break;
						case LEAK_SWEEP:					//漏扫直行清扫
							Do_LeakSweep();
							break;
						//case PASS2LEAKSWEEP:				//过度到漏扫直行清扫
							//Do_Pass2LeakSweep();
							//break;
						//case STOP_LEAKBACK:
							//Do_Stop_LeakBack();
							//break;
						case LEAK_BACKSWEEP:
							Do_Leak_BackSweep();
							break;
						case YBS_SUB_RIGHT:
						case YBS_SUB_LEFT:
							Sweep_YBS();
							break;
						case SUBMODE_SPOT:
							Do_Spot_My();
							break;
					}
			break;
			case SHIFT:
				switch(mode.sub_mode)
					{
						case SHIFTPOINT1:
							Do_Shift_Point1();
							break;
						case SHIFTPOINT2:
							Do_Shift_Point2();
							break;
						case YBS_SUB_LEFT:
						case YBS_SUB_RIGHT:
							Do_ShiftYBS();
							break;
					}
				break;
			case PASS2INIT:
				Do_Pass2Init();
				break;
			case EXIT:
				Do_ShiftExit_YBS();
				break;
			case TEST:
				Do_RunTest();
				break;
			case MODE_CTRL:
				switch(mode.sub_mode)
					{
						case SUBMODE_REMOTE_CTRL:
							Do_Remote_Move();
							break;
						case SUBMODE_APP_CTRL:
							Do_App_Ctrl();
							break;
					}
				break;
			case SPOT:
				//Do_Spot();
				Do_Spot_My();
				break;
	  /*********************默认状态***********************/
			default :
				Init_Cease();	  
			break;
  }
}


/**************************************************************************
功能：取反uv灯
**************************************************************************/
void Turn_UV(void)
{
    if(gsv_work_mode.uv == false)
	{
	    gsv_work_mode.uv = true;  
	}
	else
	{
	    gsv_work_mode.uv = false;
	}
	WriteWorkState();
}
 
/**************************************************** 
功能：改变机器行走速度
输入：无
输出：Wheel_State = 1;
返回：无
****************************************************/
void Change_Speed(void)
{
    if( gsv_work_mode.speed == 1)
	{
	    gsv_work_mode.speed = 2;
		Speed = LOWSPEED;//750;
		l_rap.rap /= (HIGHSPEED/LOWSPEED);//2;	   
		r_rap.rap /= (HIGHSPEED/LOWSPEED);//2;

	}
	else
	{
	    gsv_work_mode.speed = 1; 
		Speed = HIGHSPEED;//1500;
		l_rap.rap *= (HIGHSPEED/LOWSPEED);//2;	   
		r_rap.rap *= (HIGHSPEED/LOWSPEED);//2;
	}
	WriteWorkState();
}

/******************************************
读取碰撞信息
*******************************************/
//QZ注意:按照程序编写，调用read_bump()函数时，返回值可以和mode.bump不一样
//QZ返回的值肯定是当前碰撞信息,但mode.bump只有上次为0时，才与返回的值一样,且mode.step_bp才会重新置0
u32  read_bump(void)
{

#if 0			//ZDK屏蔽

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



	
     if (0 == l_bump.key)
     {
		 if((mode.bump == 0))	 //左撞
			 {
						mode.bump=5;//L_BUMP;
						mode.step_bp=0;
						stop_rap();
				}
			return 5;//L_BUMP;
	 }
     if (0 == r_bump.key)
     {
			if((mode.bump == 0))	 //右撞
					{
							mode.bump=6;//R_BUMP;
							mode.step_bp=0;
							stop_rap();
					}
			return 6;//R_BUMP;
	 }  

	 if(0==m_bump.key)
		 {
				if((mode.bump == 0))	 //前撞
						{
								mode.bump=90;//R_BUMP;
								mode.step_bp=0;
								stop_rap();
						}
				return 90;//R_BUMP;

	 }	
		 
#if 1		 
    if((w_l.sign==NEAR)&&(w_l.on==1))
    {
		if((mode.bump == 0))	    //左墙检靠近墙
     	{
				mode.bump=7;//W_L;
				mode.step_bp=0;
				stop_rap();
		 }	  
	  return 7;//W_L;
	}
    if((w_lm.sign==NEAR)&&(w_lm.on==1))
    {
         if((mode.bump == 0))		 //左中墙检靠近墙
         {
				mode.bump=8;//W_LM;
				mode.step_bp=0;
				stop_rap();
		 }
	  
	  return 8;//W_LM;
   }
  if((w_m.sign==NEAR)&&(w_m.on==1))
   {
   		if((mode.bump == 0))	   //中墙检靠近墙
     	{
				mode.bump=9;//W_M;
				mode.step_bp=0;
				stop_rap();
		 }
	  return 9;//W_M;
   }

  if((w_rm.sign==NEAR)&&(w_rm.on==1))
  {
   		if((mode.bump == 0))		//右中墙检靠近墙
     	{
				mode.bump=15;//W_RM;
				mode.step_bp=0;
				stop_rap();
		 }
	  return 15;//W_RM;
   }

  if((w_r.sign==NEAR)&&(w_r.on==1))
  {
  		if((mode.bump == 0))		//右墙检靠近墙		
     	{
				mode.bump=14;//W_R;
				mode.step_bp=0;
				stop_rap();
		 }	
	     return 14;//W_R;
  }


//	if(mode.action == 3)
//	{
//		if((Ultra.Distance[0] < 90) & (Ultra.Distance[0]  > 1))
//		{
//			mode.bump = 99;			//	超声停止
//			mode.step_bp=0;
//			stop_rap();
//			return 6;
//		}
//	}
	#endif
					
	return 0;


}




/****************************************************************
是否有保护的内容，有则处理并返回1，否则返回0
****************************************************************/
u8 Read_Protect(void)
{
	u8 data1,return_data=0;

#ifdef PITCH_SPEEDUP
	if(Gyro_Pitch_Speedup())
		mode.speed_up=true;
	else
		mode.speed_up=false;
#endif

	/////////////边刷检查//////////////////
#ifdef 	SB_FIX_CHECK
#if 0
	data1=SB_FIX_Check();
	if(data1)
		{
			if(mode.abnormity==0)
				{
					stop_rap();
					SideBrush.flag=false;					//暂时关闭检测,保护动作完成之前,不会再次判断进入
					if(data1==1)							//左缠绕
						mode.abnormity=ABNORMAL_LSB;
					else									//右缠绕及左右缠绕
						mode.abnormity=ABNORMAL_RSB;
					mode.step_abn=0;
					mode.Info_Abort=1;						//屏蔽与SLAM通信
					if(giv_sys_time-SideBrush.done_time<300000) 	//如果30s内再次进入边刷缠绕
					{
						SideBrush.disc_error_time++;			//不连续错误次数自加
					}
					else
						SideBrush.disc_error_time=0;
					if(SideBrush.disc_error_time>=5)				//不连续错误次数超过5次，即150s之内连续进入5次边刷缠绕
					{
						SideBrush.fail=0x01;						//直接报错，不再保护
						return 0;
					}
				}
			else
				{
					if(data1==1)							//左缠绕
						return_data=ABNORMAL_LSB;
					else									//右缠绕及左右缠绕
						return_data=ABNORMAL_RSB;
				}
		}
#else
	if(SB_OC_Check())
		{
			if(mode.abnormity==0)
				{
					stop_rap();
					SideBrush.flag=false;
					mode.abnormity=ABNORMAL_SB;
					mode.step_abn=0;
					if(giv_sys_time-SideBrush.done_time<300000) 	//如果30s内再次进入边刷缠绕
						{
							SideBrush.disc_error_time++;			//不连续错误次数自加
						}
					else
						SideBrush.disc_error_time=0;
					if(SideBrush.disc_error_time>=5)				//不连续错误次数超过5次，即150s之内连续进入5次边刷缠绕
						{
							SideBrush.fail=0x01;						//直接报错，不再保护
							return 0;
						}
#ifdef NEW_VOICE_IC
					Send_Voice(VOICE_ERROR_WARNING);
#endif
				}
			else
				return_data=ABNORMAL_SB;
		}
#endif
#endif

#ifdef	LIFT_CHECK
	/////////////离地检查/////////////////
	if(Lift_Check())		//1.5s检测时间
		{
			if((!l_lidi.key)&(!r_lidi.key))		//双轮同时离地,直接报错停止
				{
					l_lidi.fail=true;
					r_lidi.fail=true;
					return 0;
				}
			if(mode.abnormity==0)
				{
					stop_rap();
					mode.abnormity=ABNORMAL_LIFT;
					mode.step_abn=0;
					mode.Info_Abort=1;
				}
			return_data=ABNORMAL_LIFT;
		}
#endif

#ifdef ROTATE_SKID_CHECK
	data1=Check_Rotate_Skid();
	if(data1==1)
		{
			if(mode.abnormity==0)
				{
					stop_rap();
					mode.abnormity=ABNORMAL_ROTATE_SKID;
					mode.step_abn=0;
					mode.Info_Abort=1;
#ifdef NEW_VOICE_IC
					Send_Voice(VOICE_ERROR_WARNING);
#endif
				}
			return_data =ABNORMAL_ROTATE_SKID;
		}
	//else 
		//return_data=data1;
#endif

	///////////碰撞检查///////////
#ifdef BUMP_FIX_CHECK
	if(Bump_Fix_Check())	//3s检测时间
		{
			if(mode.abnormity<ABNORMAL_BUMP)
				{ 
					stop_rap();
					mode.abnormity=ABNORMAL_BUMP;
					mode.step_abn=0;
					mode.Info_Abort=1;
#ifdef NEW_VOICE_IC
					Send_Voice(VOICE_ERROR_WARNING);
#endif
				}
			return_data= ABNORMAL_BUMP;
		}
#endif

	//////////轮子检查//////////
#ifdef RING_FIX_CHECK
	if(Ring_Fix_Check())
		{
			if(mode.abnormity!=ABNORMAL_RING)
				{
					//stop_rap();
					mode.abnormity=ABNORMAL_RING;
					mode.step_abn=0;
					mode.Info_Abort=1;
#ifdef NEW_VOICE_IC
					Send_Voice(VOICE_ERROR_WARNING);
#endif
				}
			return_data= ABNORMAL_RING;
		}
#endif

#ifdef GYRO_PITCH_CHECK
	data1=Gyro_Pitch_Check();
	if(data1)
		{
			if(mode.abnormity<ABNORMAL_PITCH_RISE)
				{
					stop_rap();
					if(data1==1)
						{
							mode.abnormity=ABNORMAL_PITCH_RISE;							
#ifdef EFFICENT_DEBUG
							TRACE("Gyro Pitch rise up!\r\n");
#endif
						}
					else
						{
							mode.abnormity=ABNORMAL_PITCH_DOWN;							
#ifdef EFFICENT_DEBUG
							TRACE("Gyro Pitch down!\r\n");
#endif
						}
					mode.step_abn=0;
					Gyro_Data.pitch_fail_cnt=0;
				}
			else
				{
					if(data1==1)
						return_data=ABNORMAL_PITCH_RISE;
					if(data1==2)
						return_data=ABNORMAL_PITCH_DOWN;
				}
		}
#endif

#ifdef GYRO_PITCHROLL_CHECK
	if(Gyro_PitchRoll_Check())
		{
			if(mode.abnormity==0)
				{
					stop_rap();
					mode.abnormity=ABNORMAL_PITCHROLL;
					mode.step_abn=0;
					Gyro_Data.pitchroll_fail_cnt=0;
#ifdef EFFICENT_DEBUG
					TRACE("Gyro PitchRoll!\r\n");
#endif
				}
			return_data=ABNORMAL_PITCHROLL;
		}
#endif

#ifdef GYRO_BIOS_CHECK
	if((mode.mode==DOCKING)&((mode.sub_mode!=YBS_SUB_LEFT)&(mode.sub_mode!=YBS_SUB_RIGHT)))
		return return_data;
	data1=Gyro_Bios_Check();
	if(data1)
		{
			if(data1==1)
				{
					if((mode.abnormity)&(mode.abnormity!=ABNORMAL_GYROBIOS_L)&(mode.abnormity!=ABNORMAL_GYROBIOS_R))
						abnormal_gyrobios_cnt--;
				
					if((mode.abnormity==0)&(mode.sub_mode!=SUBMODE_SPOT))
						{
							mode.abnormity=ABNORMAL_GYROBIOS_L;
							mode.step_abn=0;
							abnormal_gyrobios_cnt=0;
						}
					
					if((mode.sub_mode==SUBMODE_SPOT)&(mode.step<9)&(mode.abnormity==0))
						{
							spot_gyrobios_cnt--;
						}						
					return_data=ABNORMAL_GYROBIOS_L;
				}
			else
				{
					if((mode.abnormity)&(mode.abnormity!=ABNORMAL_GYROBIOS_L)&(mode.abnormity!=ABNORMAL_GYROBIOS_R))
						abnormal_gyrobios_cnt++;
					
					if((mode.abnormity==0)&(mode.sub_mode!=SUBMODE_SPOT))
						{
							mode.abnormity=ABNORMAL_GYROBIOS_R;
							mode.step_abn=0;
							abnormal_gyrobios_cnt=0;
						}
					if((mode.sub_mode==SUBMODE_SPOT)&(mode.step<9)&(mode.abnormity==0))
						{
							spot_gyrobios_cnt++;
						}
					return_data=ABNORMAL_GYROBIOS_R;
				}
		}
#endif
	if(return_data)
		TRACE("return_data=%d\r\n",return_data);
	return return_data;
	
}

u8 Action_Protect_My(u8 abnoraml)
{
	u8 n,data1;
//	u8 m;
//	m=abnoraml;
	static short resume_anlge;
	static u8 turn_dir;
	static u32 l_state,r_state,l_ori,r_ori,l_length,r_length;
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	n=Dock_read_bump();		//只要碰撞,不要红外
	if((n!=0)&(mode.step_abn<7)&(mode.abnormity!=ABNORMAL_BUMP)&(mode.abnormity!=ABNORMAL_PITCH_RISE)&(mode.abnormity!=ABNORMAL_PITCH_DOWN)&(mode.abnormity!=ABNORMAL_PITCHROLL))
		{
			stop_rap();
			mode.step_abn=7;
			mode.abn_time=giv_sys_time;		//qz add 20181011
		}
	switch (mode.abnormity)
		{
			////////////左边刷被缠绕////////////////////////
			////////////左边刷被缠绕////////////////////////
			case ABNORMAL_LSB:
				switch(mode.step_abn)
					{
						case 0:
							Set_SideBrush_Pwm(0);				//先关掉边扫
							mode.abn_time=giv_sys_time;			//qz add 20181011
							mode.step_abn++;
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)		//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;							//速度约135mm/s
							if(do_action(1,70*Angle_1)) 		//左转,行走完成时间大约900ms
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							if(giv_sys_time-action.time>10000)	//时间大于2s
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 2:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;							//速度约270mm/s
							if(do_action(4,10*CM_PLUS))			//行走完成100mm,大约耗时370ms
								{
									stop_rap();					
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(giv_sys_time-action.time>20000)	//时间大于1s
								{
									stop_rap();					//qz add 20181011
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 3:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;							//速度约135mm/s
							if(do_action(2,60*Angle_1))			//右转，
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(giv_sys_time-action.time>10000)	//时间大于2s
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							break;
						case 4:							
							Set_SideBrush_Pwm(STANDARD_PWM);		//开启边刷
							mode.abn_time=giv_sys_time; //qz add 20181011
							mode.step_abn++;
							break;  
						case 5:
							if(giv_sys_time-mode.abn_time>SB_CHECK_TIME)	//观察1s qz modify 20181201 1s--->3s
							{
								data1=Read_SB_Status();
#ifdef EFFICENT_DEBUG
								TRACE("SB_data=%d\r\n",data1);
#endif
								if(data1)
									{
										SideBrush.error_time++;
										if(SideBrush.error_time>=2)
											{
												SideBrush.fail=0x01;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
												mode.Info_Abort=0;
												mode.abnormity=0;		//清零
												mode.step_abn=0;		//清零
											}
										else if(data1>=2)				//如果左边刷已OK,右边刷缠绕,切换动作至右边刷
											mode.abnormity=ABNORMAL_RSB;
										mode.step_abn=0;				//从第0步开始
									}
								else
									{
										SideBrush.error_time=0; 		//重新开始检测条件初始化
										SideBrush.fail=0;			//fail标志取消
										SideBrush.flag=true;			//重新开启检测
										SideBrush.check_step=0; 		//重新开始检测条件初始化
										SideBrush.done_time=giv_sys_time;	//记录保护动作完成时间
										mode.Info_Abort=0;				//开启SLAM通信
										mode.abnormity=0;
										mode.step_abn=0;
										mode.step=0;
										mode.step_mk=0;					//qz add 20180919
										Sweep_Level_Set(sweep_level);	//重新打开清扫等级
									}
							}
							break;
						case 7:											//缠绕的同时发生碰撞,先处理完碰撞
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							Set_BS_Level(0);							//关边刷
							if(do_action(4,4*CM_PLUS))					//后退			
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(n==0)									//后退后无碰撞
								{
									stop_rap();
									mode.bump=0;
									mode.step_abn=1;					//从step_abn=1重新开始
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							break;
						case 8:											//后退后还是有碰撞(因为此时被缠绕,后退不一定成功)
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;									//速度约135mm/s
							if(do_action(1,360*Angle_1))				//旋转一周,全程需要5.4s	
								{
									stop_rap();
									SideBrush.fail=0x01;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
									mode.Info_Abort=0;
									mode.abnormity=0;
									mode.step_abn=0;
								}
							if(n==0)							//无碰撞
								{
									stop_rap();
									mode.bump=0;
									mode.step_abn=1;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(giv_sys_time-action.time>65000)	//6.5s
								{
									stop_rap();
									SideBrush.fail=0x01;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
									mode.Info_Abort=0;
									mode.abnormity=0;
									mode.step_abn=0;
								}
							break;
					}
			break;
			
			////////////右边刷被缠绕////////////////////////
			////////////右边刷被缠绕////////////////////////
			case ABNORMAL_RSB:
				switch(mode.step_abn)
					{
						case 0:
							Set_SideBrush_Pwm(0);				//先关掉边扫
							mode.abn_time=giv_sys_time; 		//qz add 20181011
							mode.step_abn++;
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;							//速度约135mm/s
							if(do_action(2,70*Angle_1)) 		//行走完成时间大约900ms
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(giv_sys_time-action.time>10000)	//时间大于1s
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							break;
						case 2:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Set_SideBrush_Pwm(0);				//先关掉边扫
							Speed=HIGH_MOVE_SPEED;							//速度约270mm/s
							if(do_action(4,10*CM_PLUS))			//行走完成100mm,大约耗时370ms
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(giv_sys_time-action.time>20000)	//时间大于1s
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 3:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;							//速度约135mm/s
							if(do_action(1,60*Angle_1))			
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(giv_sys_time-action.time>10000)	//时间大于2s
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							break;
						case 4:
							Set_SideBrush_Pwm(STANDARD_PWM);		//开启边刷
							mode.abn_time=giv_sys_time; //qz add 20181011
							mode.step_abn++;
							break;  
						case 5:
							if(giv_sys_time-mode.abn_time>SB_CHECK_TIME)	//观察1s qz modify 20181201 1s--->3s
							{
								data1=Read_SB_Status();
#ifdef EFFICENT_DEBUG
								TRACE("SB_data=%d\r\n",data1);
#endif
								if(data1)
									{
										SideBrush.error_time++;
										if(SideBrush.error_time>=2)
											{
												SideBrush.fail=0x02;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
												mode.Info_Abort=0;
												mode.abnormity=0;
												mode.step_abn=0;
											}
										else if(data1==1)				//如果右边刷已OK,左边刷缠绕,切换动作至左边刷
											mode.abnormity=ABNORMAL_LSB;
										mode.step_abn=0;				//从第0步开始
									}
								else
									{
										SideBrush.error_time=0; 	//重新开始检测条件初始化
										SideBrush.fail=0x00;
										SideBrush.flag=true;		//重新开启检测
										SideBrush.check_step=0; 	//重新开始检测条件初始化
										SideBrush.done_time=giv_sys_time;	//记录保护动作完成时间
										mode.Info_Abort=0;
										mode.abnormity=0;
										mode.step_abn=0;
										mode.step=0;				//恢复模式步骤qz add 20180801
										mode.step_mk=0;				//qz add 20180919
										Sweep_Level_Set(sweep_level);	//重新打开清扫等级
									}
							}
							break;
						case 7:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
//							Set_BS_Level(0);
							if(do_action(4,4*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(n==0)
								{
									stop_rap();
									mode.bump=0;
									mode.step_abn=1;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							break;
						case 8:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;						//速度约135mm/s
							if(do_action(1,360*Angle_1))	//全程需要5.4s	
								{
									stop_rap();
									SideBrush.fail=0x02;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
									mode.Info_Abort=0;
									mode.abnormity=0;
									mode.step_abn=0;
									mode.step=0;			//恢复模式步骤qz add 20180801
									mode.step_mk=0;			//qz add 20180919
								}
							if(n==0)
								{
									stop_rap();
									mode.bump=0;
									mode.step_abn=1;
									mode.abn_time=giv_sys_time;	//qz add 20181011
								}
							if(giv_sys_time-action.time>65000)	//6.5s
								{
									stop_rap();
									SideBrush.fail=0x02;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
									mode.Info_Abort=0;
									mode.abnormity=0;
									mode.step_abn=0;
									mode.step=0;			//恢复模式步骤qz add 20180801
									mode.step_mk=0;			//qz add 20180919
								}
							break;
								
							
					}
				break;

			case ABNORMAL_SB:
				switch(mode.step_abn)
					{
						case 0:
							Set_Coordinate_Wall(now_gridx,now_gridy);
							Set_SideBrush_Pwm(0);				//先关掉边扫
							mode.abn_time=giv_sys_time; 		//qz add 20181011
							mode.step_abn++;
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Set_SideBrush_Pwm(0);				//先关掉边扫
							Speed=HIGH_MOVE_SPEED;							//速度约270mm/s
							if(do_action(4,10*CM_PLUS)) 		//行走完成100mm,大约耗时370ms
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(giv_sys_time-action.time>20000)	//时间大于1s
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 2:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;							//速度约135mm/s
							if(do_action(1,180*Angle_1)) 		
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(giv_sys_time-action.time>10000)	//时间大于2s
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 3:
							Set_SideBrush_Pwm(STANDARD_PWM);		//开启边刷
							mode.abn_time=giv_sys_time; //qz add 20181011
							mode.step_abn++;
							break;	
						case 4:
							if(giv_sys_time-mode.abn_time>SB_CHECK_TIME)	//观察1s qz modify 20181201 1s--->3s
							{
								data1=Read_SB_Status();
#ifdef EFFICENT_DEBUG
								TRACE("SB_data=%d\r\n",data1);
#endif
								if(data1)
									{
										SideBrush.error_time++;
										if(SideBrush.error_time>=2)
											{
												SideBrush.fail=0x02;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
												mode.Info_Abort=0;
												mode.abnormity=0;
												mode.step_abn=0;
											}
										else if(data1==1)				//如果右边刷已OK,左边刷缠绕,切换动作至左边刷
											mode.abnormity=ABNORMAL_LSB;
										mode.step_abn=0;				//从第0步开始
									}
								else
									{
										mode.step_abn=0xf0;
									}
							}
							break;
						case 7:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Set_Coordinate_Wall(now_gridx,now_gridy);
							Speed=HIGH_MOVE_SPEED;
//							Set_BS_Level(0);
							if(do_action(4,4*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(n==0)
								{
									stop_rap();
									mode.bump=0;
									mode.step_abn=1;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 8:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;						//速度约135mm/s
							if(do_action(1,360*Angle_1))	//全程需要5.4s	
								{
									stop_rap();
									SideBrush.fail=0x02;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
									mode.Info_Abort=0;
									mode.abnormity=0;
									mode.step_abn=0;
									mode.step=0;			//恢复模式步骤qz add 20180801
									mode.step_mk=0; 		//qz add 20180919
								}
							if(n==0)
								{
									stop_rap();
									mode.bump=0;
									mode.step_abn=1;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(giv_sys_time-action.time>65000)	//6.5s
								{
									stop_rap();
									SideBrush.fail=0x02;	//确认发生"边刷被缠绕"异常，将由Check_Status()函数检测,并进入ERR模式
									mode.Info_Abort=0;
									mode.abnormity=0;
									mode.step_abn=0;
									mode.step=0;			//恢复模式步骤qz add 20180801
									mode.step_mk=0; 		//qz add 20180919
								}
							break;

						case 0xf0:
							SideBrush.error_time=0; 	//重新开始检测条件初始化
							SideBrush.fail=0x00;
							SideBrush.flag=true;		//重新开启检测
							SideBrush.check_step=0; 	//重新开始检测条件初始化
							SideBrush.done_time=giv_sys_time;	//记录保护动作完成时间
							mode.Info_Abort=0;
							mode.abnormity=0;
							mode.step_abn=0;
							mode.step=0;				//恢复模式步骤qz add 20180801
							mode.step_mk=0; 			//qz add 20180919
							Sweep_Level_Set(sweep_level);	//重新打开清扫等级
							if((mode.sub_mode==YBS_SUB_LEFT)|(mode.sub_mode==YBS_SUB_RIGHT))
								break;
							switch(mode.mode)
								{
									case SWEEP:
									case PASS2INIT:
									case EXIT:
									case SHIFT:
										Area_Check(0);
										Init_Shift_Point1(0);
									break;
									default:
										break;
								}
								
					}
				break;

				////////////旋转打滑////////////
#ifdef ROTATE_SKID_CHECK
			case ABNORMAL_ROTATE_SKID:
				switch (mode.step_abn)
					{
						case 0:
							Speed=HIGH_MOVE_SPEED;
							if(do_action(4,10*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 1:
							Speed=TURN_SPEED;
							if(giv_sys_time-mode.abn_time<10000)
								return 0;
							if(do_action(2,360*Angle_1))
								{
									stop_rap();
									mode.step_abn=0xf0;
								}
							if(abnoraml==ABNORMAL_ROTATE_SKID)
								{
									stop_rap();
									mode.step_abn++;
								}
							break;
						case 2:
							Speed=HIGH_MOVE_SPEED;
							if(do_action(4,10*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 3:
							Speed=TURN_SPEED;
							if(giv_sys_time-mode.abn_time<10000)
								return 0;
							if(do_action(2,360*Angle_1))
								{
									stop_rap();
									mode.step_abn=0xf0;
								}
							if(abnoraml==ABNORMAL_ROTATE_SKID)
								{
									stop_rap();
									mode.step_abn++;
								}
							break;
						case 4:
							Rotate_Skid.fail=true;
							break;
#if 0
						case 4:
							Speed=TURN_SPEED;
							if(do_action(1,180*Angle_1))
								{
									stop_rap();
									mode.step_abn++;
								}
						case 5:
							Speed=TURN_SPEED;
							if(do_action(2,180*Angle_1))
								{
									stop_rap();
									mode.step_abn=0;
									Rotate_Skid.resuce_time++;
									if(Rotate_Skid.resuce_time>=3)
										{
											Rotate_Skid.resuce_time=0;
											if((n>0)&&(n!=ABNORMAL_ROTATE_SKID))
												{
													mode.abnormity=n;
													mode.step_abn=0;
													return 0;
												}
											else
												{
													Rotate_Skid.fail=true;
													return 0;
												}												
										}
								}
							break;
#endif
						case 7:
							Speed=BUMP_BACK_SPEED;
							if(do_action(4,4*CM_PLUS))
								{
									stop_rap();
									if(n==0)
										mode.step_abn++;
									else
										mode.step_abn=7;
								}
							break;
						case 8:
							Speed=TURN_SPEED;
							if(do_action(1,90*Angle_1))
								{
									stop_rap();
									mode.step_abn=0;
								}
							break;
						case 0xf0:
							Speed=TURN_SPEED;
							if(abnormal_gyrobios_cnt==0)
								{
									mode.step_abn++;
									return 0;
								}
							if(abnormal_gyrobios_cnt>0)
								{
									if(do_action(1,abnormal_gyrobios_cnt*360*Angle_1))
										{
											stop_rap();
											mode.step_abn++;
										}
								}
							else
								{
									if(do_action(2,abnormal_gyrobios_cnt*360*Angle_1))
										{
											stop_rap();
											mode.step_abn++;
										}
								}
							break;
						case 0xf1:
							abnormal_gyrobios_cnt=0;
							Rotate_Skid.fail=false;
							Rotate_Skid.resuce_time=0;
							mode.abnormity=0;
							mode.step_abn=0;
							mode.Info_Abort=0;
							mode.step=0;		//恢复模式步骤qz add 20180801
							mode.step_mk=0; 		//qz add 20180919
							switch(mode.mode)
								{
									case SWEEP:
									case PASS2INIT:
									case EXIT:
									case SHIFT:
										Area_Check(0);
										Init_Shift_Point1(0);
									break;
									default:
										break;
								}
							break;
					}
				break;
#endif
			////////////左轮离地////////////
			case 22:
			case 23:
				if(r_lidi.key==0)
					{
						mode.abnormity=ABNORMAL_LIFT;
						mode.step_abn=0;
						mode.abn_time=giv_sys_time; //qz add 20181011
						return 0;
					}
			case ABNORMAL_LIFT:
				switch (mode.step_abn)
					{
						case 0:	
							mode.abn_time=giv_sys_time; //qz add 20181011
							mode.step_abn++;
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(1,360*Angle_1))			//左轮不动向左转
								{
									stop_rap();
									if((l_lidi.key==0)|(r_lidi.key==0))	//仍然有离地情况
										{
											mode.step_abn++;
											mode.abn_time=giv_sys_time; //qz add 20181011
										}
									else
										{
											mode.step_abn=3;			//双轮着地，后退
											mode.abn_time=giv_sys_time; //qz add 20181011
										}
								}
							if((l_lidi.key)&(r_lidi.key))			//双轮着地，后退
								{
									stop_rap();
									mode.step_abn=3;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 2:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(2,360*Angle_1))			//左轮不动向右转
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if((l_lidi.key)&(r_lidi.key))			//双轮着地
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 3:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(4,10*CM_PLUS))				//10cm耗时约500ms
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(giv_sys_time-action.time>15000)
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 4:
							if(giv_sys_time-mode.abn_time>5000)
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(l_lidi.key&r_lidi.key)			//双轮着地
								{
									stop_rap();
									mode.step_abn=6;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 5:
							if((l_lidi.key==0)&(r_lidi.key==0))		//双轮离地
								{

									l_lidi.fail=true;
									r_lidi.fail=true;
									return 0;
								}
							else if((!l_lidi.key)&(r_lidi.key))		//左轮离地
								{
									l_lidi.fail=true;
									r_lidi.fail=false;
									return 0;
								}
							
							else if((l_lidi.key)&(!r_lidi.key))		//右轮离地
								{
									l_lidi.fail=false;
									r_lidi.fail=true;
									return 0;
								}
							else									//双轮着地
								{
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 6:			//已经在step_abn为4的时候延时，此刻无需延时
							Speed=HIGH_MOVE_SPEED;
							if(do_action(1,60*Angle_1))
								{
									stop_rap();
									mode.abnormity=0;
									mode.step_abn=0;
									l_lidi.fail=false;
									r_lidi.fail=false;
									mode.Info_Abort=0;
									mode.step=0;					//恢复模式步骤qz add 20180801
									mode.step_mk=0; 		//qz add 20180919
								}
							break;
						case 7:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(4,4*CM_PLUS))
								{
									stop_rap();
									if(n==0)
										{
											mode.step_abn=0;
											mode.abn_time=giv_sys_time; //qz add 20181011
										}
									else
										{
											mode.step_abn=7;
											mode.abn_time=giv_sys_time; //qz add 20181011
										}
								}
							break;
					}
				break;
			////////////碰撞卡住////////////
			case ABNORMAL_BUMP:
				switch (mode.step_abn)
					{
						case 0:
							mode.abn_time=giv_sys_time; //qz add 20181011
							mode.step_abn++;
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(2,360*Angle_1))			//旋转
								{
									stop_rap();
									mode.step_abn++;				
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(l_bump.key&r_bump.key)	//没有碰撞信号后
								{
									stop_rap();
									mode.step_abn=3;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(giv_sys_time-action.time>60000)		//超时
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 2: 									//旋转一周后任然有碰撞信号，判定异常
							l_bump.fail=true;
							return 0;
						case 3:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED; 							//前进
							if(do_action(3,10*CM_PLUS))
								{
									stop_rap();
									mode.step_abn=5;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(n)									//再次发生碰撞
								{
									stop_rap();
									Set_Coordinate_Wall(now_gridx,now_gridy);
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(giv_sys_time-action.time>10000)
								{
									stop_rap();
									mode.step_abn=5;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 4:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(4,4*CM_PLUS))			//碰撞后退
								{
									stop_rap();
									if(n)						//后退了还是有碰撞信号
										{
											l_bump.fail=true;	//判定异常
											mode.Info_Abort=0;
											mode.abnormity=0;
											mode.step_abn=0;
											return 0;
										}
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 5:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(2,90*Angle_1))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 6:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							if(do_action(3,10*CM_PLUS))
								{
									stop_rap();
									mode.step_abn=8;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							if(n)														//再次发生碰撞
								{
									stop_rap();
									Set_Coordinate_Wall(now_gridx,now_gridy);
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 7:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(4,4*CM_PLUS))			//碰撞后退
								{
									stop_rap();
									if(n)												//后退了还是有碰撞信号
										{
											l_bump.fail=true;				//判定异常
											mode.Info_Abort=0;
											mode.abnormity=0;
											mode.step_abn=0;
											return 0;
										}
									mode.step_abn++;
									mode.abn_time=giv_sys_time; //qz add 20181011
								}
							break;
						case 8:
							mode.step_abn=0xf0;
							break;
						case 0xf0:
							mode.abnormity=0;
							mode.step_abn=0;
							mode.Info_Abort=0;
							mode.step=0;		//恢复模式步骤qz add 20180801
							mode.step_mk=0;			//qz add 20180919
							mode.bump=0;			//qz add 20181210
							mode.step_bp=0;			//qz add 20181210
							mode.bump_flag=false;	//qz add 20181210
							switch(mode.mode)
								{
									case SWEEP:
									case PASS2INIT:
									case EXIT:
									case SHIFT:
										Area_Check(0);
										Init_Shift_Point1(0);
									break;
									default:
										break;
								}
							break;
					}
				break;
			
			////////轮子卡住/////////////
			case ABNORMAL_RING:
				switch (mode.step_abn)
					{
						case 0: 
							l_state=l_ring.state;
							r_state=r_ring.state;
							l_length=l_ring.all_length;
							r_length=r_ring.all_length;
							l_ori=l_rap.ori;
							r_ori=r_rap.ori;
							stop_rap();
							mode.step_abn++;
							mode.abn_time=giv_sys_time; //qz add 20181011
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)	//qz add 20181011
								return 0;
							if(!l_state&(!r_state))
								{
									mode.abnormity=0;
									mode.step_abn=0;
									mode.Info_Abort=0;
									return 0;
								}
							else
								mode.step_abn++;
							break;
						case 2:
							if(l_state)
								{
									if((l_ori<=FRONT))				//取反方向开始运转
										{
											l_rap.ori=BACK;
											enable_pwm(L_BACK,PWM_RING_MAX*4/5);
										}
									else
										{
											l_rap.ori=FRONT;
											enable_pwm(L_FRONT,PWM_RING_MAX*4/5);
										}
								}
							if(r_state)
								{
									if((r_ori<=FRONT))				//取反方向开始运转
									{
										r_rap.ori=BACK;
										enable_pwm(R_BACK,PWM_RING_MAX*4/5);
									}
									else
									{
										r_rap.ori=FRONT;
										enable_pwm(R_FRONT,PWM_RING_MAX*4/5);
									}
								}
							mode.abn_time=giv_sys_time; //qz add 20181011
							mode.step_abn++;
							break;
						case 3:
							if(giv_sys_time-mode.abn_time>10000)		//500ms
								{
									disable_pwm(L_FRONT);
									disable_pwm(L_BACK);
									disable_pwm(R_FRONT);
									disable_pwm(R_BACK);
									mode.abn_time=giv_sys_time; //qz add 20181011
									mode.step_abn++;
								}
							break;
						case 4:
							if(giv_sys_time-mode.abn_time>500)
								{
									if(l_state)
									{
									if((l_ori<=FRONT)&&(l_ring.all_length>=(l_length-RING_FIX_OFFSET)))	//刚才是倒转
										{
											l_rap.fail=true;
											return 0;
										}
									else if((l_ori==BACK)&&(l_ring.all_length<=(l_length+RING_FIX_OFFSET)))	//刚才是顺转
										{
											l_rap.fail=true;
											return 0;
										}
									}
									if(r_state)
									{
									if((r_ori<=FRONT)&&(r_ring.all_length>=(r_length-RING_FIX_OFFSET)))
										{
											r_rap.fail=true;
											return 0;
										}
									else if((r_ori==BACK)&&(r_ring.all_length<=(r_length+RING_FIX_OFFSET)))
										{
											r_rap.fail=true;
											return 0;
										}
									}
									mode.step_abn++;											
								}
							break;
						case 5:
							l_length=l_ring.all_length;
							r_length=r_ring.all_length;
							if(l_state)
								{
									if((l_ori<=FRONT))				//取正方向开始运转
										{
											l_rap.ori=FRONT;
											enable_pwm(L_FRONT,PWM_RING_MAX*4/5);
										}
									else
										{
											l_rap.ori=BACK;
											enable_pwm(L_BACK,PWM_RING_MAX*4/5);
										}
								}
							if(r_state)
								{
									if((r_ori<=FRONT))				//取正方向开始运转
									{
										r_rap.ori=FRONT;
										enable_pwm(R_FRONT,PWM_RING_MAX*4/5);
									}
									else
									{
										r_rap.ori=BACK;
										enable_pwm(R_BACK,PWM_RING_MAX*4/5);
									}
								}
							mode.step_abn++;
							mode.abn_time=giv_sys_time; //qz add 20181011
							break;
						case 6:
							if(giv_sys_time-mode.abn_time>10000)		//500ms
								{
									disable_pwm(L_FRONT);
									disable_pwm(L_BACK);
									disable_pwm(R_FRONT);
									disable_pwm(R_BACK);
									mode.abn_time=giv_sys_time;
									mode.step_abn++;
								}
							break;
						case 7:
							if(giv_sys_time-mode.abn_time>500)		//50ms
								{
									if(l_state)
									{
									if((l_ori<=FRONT)&&(l_ring.all_length<=(l_length+RING_FIX_OFFSET)))
										{
											l_rap.fail=true;
											return 0;
										}
									else if((l_ori==BACK)&&(l_ring.all_length>=(l_length-RING_FIX_OFFSET)))
										{
											l_rap.fail=true;
											return 0;
										}
									}
									if(r_state)
									{
									if((r_ori<=FRONT)&&(r_ring.all_length<=(r_length+RING_FIX_OFFSET)))
										{
											r_rap.fail=true;
											return 0;
										}
									else if((r_ori==BACK)&&(r_ring.all_length>=(r_length-RING_FIX_OFFSET)))
										{
											r_rap.fail=true;
											return 0;
										}
									}
									mode.step_abn++;											
								}
							break;
						case 8:
							mode.step_abn=0xf0;
							break;
						case 0xf0:
							l_rap.fail=false;
							l_ring.state=GOOD;
							l_ring.odds=0;
							r_rap.fail=false;
							r_ring.state=GOOD;
							l_ring.odds=0;
							mode.abnormity=0;
							mode.step_abn=0;
							mode.Info_Abort=0;
							mode.step=0;
							mode.step_mk=0;			//qz add 20180919
							switch(mode.mode)
								{
									case SWEEP:
									case PASS2INIT:
									case EXIT:
									case SHIFT:
										Area_Check(0);
										Init_Shift_Point1(0);
									break;
									default:
										break;
								}
							break;
					}
				break;
				
			case ABNORMAL_PITCH_RISE:
				switch(mode.step_abn)
					{
						case 0:
							stop_rap();
							Set_Coordinate_Wall(now_gridx,now_gridy);
							if(Gyro_Data.pitch_fail_cnt>3)
								{
									Gyro_Data.pitch_fail=true;
									return 0;
								}
							mode.abn_time=giv_sys_time;
							mode.step_abn++;
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(4,15*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 2:
							if(giv_sys_time-mode.abn_time>8000)			//延时800ms
								return 0;
							if(abs(Gyro_Data.pitch-Gyro_Data.first_pitch)>600)
								{
									if(Gyro_Data.pitch<Gyro_Data.first_pitch)
										{
											mode.abnormity=ABNORMAL_PITCH_DOWN;
										}
									mode.step_abn=0;
									Gyro_Data.pitch_fail_cnt++;
								}
							else
								{
									//mode.step_abn++;
									mode.step_abn=0xf0;
								}
							break;
						case 0xf0:
							mode.abnormity=0;
							mode.step_abn=0;
							mode.step=0;
							mode.Info_Abort=0;
							mode.step=0;
							mode.step_mk=0; 		//qz add 20180919
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;	//qz add 20181210
							Gyro_Data.pitch_fail_cnt=0;
							if((mode.sub_mode==YBS_SUB_LEFT)|(mode.sub_mode==YBS_SUB_RIGHT))
								break;
#ifdef EFFICENT_DEBUG
							TRACE("Gyro Pitch bump complete!\r\n");
#endif
							switch(mode.mode)
								{
									case SWEEP:
									case PASS2INIT:
									case EXIT:
									case SHIFT:
										Area_Check(0);
										Init_Shift_Point1(0);
									break;
									default:
										break;
								}							
							break;							
					}
				break;
			case ABNORMAL_PITCH_DOWN:
				switch(mode.step_abn)
					{
						case 0:
							stop_rap();
							Set_Coordinate_Wall(now_gridx,now_gridy);
							if(Gyro_Data.pitch_fail_cnt>3)
								{
									Gyro_Data.pitch_fail=true;
									return 0;
								}
							mode.abn_time=giv_sys_time;
							mode.step_abn++;
							
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(3,10*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 2:
							if(giv_sys_time-mode.abn_time>8000) 		//延时800ms
								return 0;
							if(abs(Gyro_Data.pitch-Gyro_Data.first_pitch)>600)
								{
									if(Gyro_Data.pitch>Gyro_Data.first_pitch)
										{
											mode.abnormity=ABNORMAL_PITCH_RISE;
										}
									mode.step_abn=0;
									Gyro_Data.pitch_fail_cnt++;
								}
							else
								{
									//mode.step_abn++;
									mode.step_abn=0xf0;
								}
							break;
						case 0xf0:
							mode.abnormity=0;
							mode.step_abn=0;
							mode.step=0;
							mode.Info_Abort=0;
							mode.step=0;
							mode.step_mk=0; 		//qz add 20180919
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;	//qz add 20181210
							Gyro_Data.pitch_fail_cnt=0;
							if((mode.sub_mode==YBS_SUB_LEFT)|(mode.sub_mode==YBS_SUB_RIGHT))
								break;
#ifdef EFFICENT_DEBUG
							TRACE("Gyro Pitch bump complete!\r\n");
#endif
							switch(mode.mode)
								{
									case SWEEP:
									case PASS2INIT:
									case EXIT:
									case SHIFT:
										Area_Check(0);
										Init_Shift_Point1(0);
									break;
									default:
										break;
								}							
							break;							
						
					}
				break;

			case ABNORMAL_PITCHROLL:
				switch(mode.step_abn)
					{
						case 0:
							stop_rap();
							Set_Coordinate_Wall(now_gridx,now_gridy);
							mode.abn_time=giv_sys_time;
							mode.step_abn++;							
							if(Gyro_Data.pitch>Gyro_Data.first_pitch)
								turn_dir=4;
							else
								turn_dir=3;
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<BUMP_TIME_DELAY)
								return 0;
							Speed=HIGH_MOVE_SPEED;
							if(do_action(turn_dir,15*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							if(n)
								{
									Set_Coordinate_Wall(now_gridx,now_gridy);
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 2:
							if(giv_sys_time-mode.abn_time>8000) 		//延时800ms
								return 0;
							if((abs(Gyro_Data.roll-Gyro_Data.first_roll))+\
								(abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>600)
								{
									mode.step_abn++;
									Gyro_Data.pitchroll_fail_cnt++;
									if(Gyro_Data.pitch>Gyro_Data.first_pitch)
										turn_dir=4;
									else
										turn_dir=3;
								}
							else
								{
									mode.step_abn=0xf0;
								}
							break;
						case 3:
							Speed=HIGH_MOVE_SPEED;
							if(do_action(turn_dir,15*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							if(n)
								{
									Set_Coordinate_Wall(now_gridx,now_gridy);
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 4:
							if(giv_sys_time-mode.abn_time>8000) 		//延时800ms
								return 0;
							if((abs(Gyro_Data.roll-Gyro_Data.first_roll))+\
								(abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>600)
								{
									mode.step_abn++;
									Gyro_Data.pitchroll_fail_cnt++;
								}
							else
								{
									mode.step_abn=0xf0;
								}
							break;
						case 5:
							Speed=HIGH_MOVE_SPEED;
							if(do_action(2,90*Angle_1))
								{
									stop_rap();
									mode.step_abn++;
									
									if(Gyro_Data.pitch>Gyro_Data.first_pitch)
										turn_dir=4;
									else
										turn_dir=3;
								}
							break;
						case 6:
							Speed=HIGH_MOVE_SPEED;
							if(do_action(turn_dir,15*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							if(n)
								{
									Set_Coordinate_Wall(now_gridx,now_gridy);
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 7:
							if(giv_sys_time-mode.abn_time>8000) 		//延时800ms
								return 0;
							if((abs(Gyro_Data.roll-Gyro_Data.first_roll))+\
								(abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>600)
								{
									mode.step_abn++;
									Gyro_Data.pitchroll_fail_cnt++;
								}
							else
								{
									mode.step_abn=0xf0;
								}
							break;
						case 8:
							Speed=HIGH_MOVE_SPEED;
							if(do_action(1,180*Angle_1))
								{
									stop_rap();
									mode.step_abn++;
									
									if(Gyro_Data.pitch>Gyro_Data.first_pitch)
										turn_dir=4;
									else
										turn_dir=3;
								}
							break;
						case 9:
							Speed=HIGH_MOVE_SPEED;
							if(do_action(turn_dir,15*CM_PLUS))
								{
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							if(n)
								{
									Set_Coordinate_Wall(now_gridx,now_gridy);
									stop_rap();
									mode.step_abn++;
									mode.abn_time=giv_sys_time;
								}
							break;
						case 10:
							if(giv_sys_time-mode.abn_time>8000) 		//延时800ms
								return 0;
							if((abs(Gyro_Data.roll-Gyro_Data.first_roll))+\
								(abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>600)
								{
									mode.step_abn++;
									Gyro_Data.pitchroll_fail=true;
								}
							else
								{
									//mode.step_abn++;
									mode.step_abn=0xf0;
								}
							break;
						case 0xf0:
							mode.abnormity=0;
							mode.step_abn=0;
							mode.step=0;
							mode.Info_Abort=0;
							mode.step=0;
							mode.step_mk=0; 		//qz add 20180919
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;	//qz add 20181210
							Gyro_Data.pitch_fail_cnt=0;
							if((mode.sub_mode==YBS_SUB_LEFT)|(mode.sub_mode==YBS_SUB_RIGHT))
								break;
#ifdef EFFICENT_DEBUG
							TRACE("Gyro Pitch bump complete!\r\n");
#endif
							switch(mode.mode)
								{
									case SWEEP:
									case PASS2INIT:
									case EXIT:
									case SHIFT:
										Area_Check(0);
										Init_Shift_Point1(0);
									break;
									default:
										break;
								}							
							break;							
					}
				break;

			case ABNORMAL_GYROBIOS_L:
			case ABNORMAL_GYROBIOS_R:
				switch(mode.step_abn)
					{
						case 0:
							stop_rap();
							mode.abn_time=giv_sys_time;
							mode.step_abn++;
							break;
						case 1:
							if(giv_sys_time-mode.abn_time<1000)
								return 0;
							resume_anlge=Gyro_Data.yaw;
							if(mode.abnormity==ABNORMAL_GYROBIOS_R)
								{
									turn_dir=1;
								}
							else
								{
									turn_dir=2;
								}
							mode.step_abn++;
							stop_rap();
							break;
						case 2:
							Speed=TURN_SPEED;
							if(do_action(turn_dir,380*Angle_1))
								{
									stop_rap();
									mode.step_abn++;
								}
							Init_Gyro_Bios();
							break;
						case 3:
							turn_dir=Get_TurnDir(resume_anlge);
							mode.step_abn++;
							break;
						case 4:
							Speed=TURN_SPEED;
							do_action(turn_dir,360*Angle_1);
							if(Judge_Yaw_Reach(resume_anlge,TURN_ANGLE_BIOS))
								{
									stop_rap();
									mode.step_abn++;
								}
							if((abnoraml==ABNORMAL_GYROBIOS_L)|(abnoraml==ABNORMAL_GYROBIOS_R))
								{
									stop_rap();
									mode.step_abn=0;
									mode.abnormity=abnoraml;
									return 0;
								}
							break;
						case 5:
							mode.abnormity=0;
							mode.step_abn=0;
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;	//qz add 20181210
							if((mode.sub_mode==YBS_SUB_LEFT)|(mode.sub_mode==YBS_SUB_RIGHT))
								{
									if(gyro_bios.island)
										mode.step=0x88;
									else
										mode.step=0;
									gyro_bios.island=false;
								}

							else if((mode.sub_mode==NORMAL_SWEEP)|(mode.sub_mode==BACK_SWEEP)|(mode.sub_mode==LEAK_SWEEP)|(mode.sub_mode==LEAK_BACKSWEEP))
								{
									mode.step=0;
								}
							break;
						case 7:
							if(giv_sys_time-mode.abn_time<1000)
								return 0;
							Speed=MID_MOVE_SPEED;
							if(do_action(2,2*CM_PLUS))
								{
									stop_rap();
									if(n)
										mode.step_abn=7;
									else
										mode.step_abn=0;
								}
							break;
					}
				break;
		}
	return 0;
}

/*****************************************************************
功能：对保护信号进行处理
*****************************************************************/   
#if 0
uint8 Action_Protect(void)
{
uint8 m;  
uint32 n;
static uint32 piv_abntime;
    m = Read_Protect();
	if(m == 1)
	{
	    return 1;
	}
	if(mode.abnormity != 0)
	{
	    n = read_bump();
	    if((((e_l.sign == FARN))
		 &&((e_m.sign == FARN)&&(e_r.sign == FARN)))
		 &&((l_lidi.key == 0)&&(l_lidi.key == 0)))  //在轮子离地并且地检离地时机器停机
		{
		    Init_Cease();
			return 1;    
		}
		if(( m == 0) && (mode.step_abn != 1))
		{
		    stop_rap();
		    mode.step_abn = 1;
			gbv_action_m_fangxiang = false;
		} 
		if((n != 0)&&(mode.step_abn != 1)&&(mode.step_abn < 7))
		{ 
		    stop_rap();
			mode.step_abn = 7;
		}
		if(mode.abnormity == 16)
		{
		    /*if(m_current > M_STOPCURRENT)
			{		   
				 giv_sys_err = mode.abnormity;
				 gbv_action_m_fangxiang = false;
				 Init_Err();
			     return 1;
			}	*/
	 	    switch (mode.step_abn)
		    {
		        case  0:		   
			        piv_abntime = giv_sys_time;
					gbv_action_m_fangxiang = true;
				    mode.step_abn = 3;
			        return 1;
			    case  1:
				    if(do_action(1, angle15 * (giv_sys_time % 10)))			//左转
			        {
				        mode.abnormity = 0;	   
						mode.step = 0;
					    return 0;					
				    }
				    if((giv_sys_time - action.time) > 100000)		//保护动作超过10s还处于step_abn为1状态,
				    {  				   
				        stop_rap();
				        mode.step_abn = 3;
				    }
				    return 1;
			    case 3:	   
				    if(do_action(1,angle45))					//左转
					{
				        mode.step_abn = 4; 
					    gbv_action_m_fangxiang = true;			//中扫向后
					} 
				    if((giv_sys_time - action.time) > 30000)	//3s
				    {  				   
				        stop_rap();
				        mode.step_abn = 4;	  
					    gbv_action_m_fangxiang = true;
				    }
				    break;

			    case 4:	  
				    if(do_action(3,angle360))					//前进
					{
				        mode.step_abn = 3; 
					} 
				    if((giv_sys_time - action.time) > 100000)	//10s
				    {  				     
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err(); 
				    }
				    break; 
				case 7:	
				    if(do_action(4,angle90))		  
				    {
					    mode.step_abn++;
			            break;
				    }
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 3;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;  
				case 8:	
				    if(do_action(1,angle90))		  
				    {
					    mode.step_abn++;
			            break;
				    } 
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 4;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;  
				case 9:	
				    if(do_action(4,angle90))		  
				    {
				        giv_sys_err = mode.abnormity;
					    Init_Err();
			            return 1;
				    }  
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 3;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;
				default :
				    mode.step_abn = 0;
					return 0;				   
		    }
			if((giv_sys_time - piv_abntime) > 150000)
			{
			    stop_rap();
			    giv_sys_err = mode.abnormity;
			    gbv_action_m_fangxiang = false;
				Init_Err();
			}
			return 1;
		}
		else
		{	
		    if(mode.abnormity == 21)
			{	
	 	        switch (mode.step_abn)
		    	{
		        	case  0:	
			        	piv_abntime = giv_sys_time;
				    	mode.step_abn = 2;
			        	return 1;
			    	case  1:		//case 1为m=0,即故障消失的情况
				    	if(do_action(1, angle15 * (giv_sys_time % 10)))			//左转
			        	{
				        	mode.abnormity = 0;
							mode.step = 0;
					    	return 0;					
				    	}
				    	if((giv_sys_time - action.time) > 100000)				//10s
				    	{  				   
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
				    	return 1;
					case 2:
			        	if(do_action(4,angle360))								//后退
		            	{
				        	mode.step_abn = 3;
			            	return 1;
			        	}
				    	if((giv_sys_time - action.time) > 100000)				//10s
				    	{  
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
						break;
			    	case 3:
				    	if(do_action(1,angle45))								//左转
				    	{
					    	mode.step_abn = 4;
				    	}
				    	if((giv_sys_time - action.time) > 30000)				//3s
				    	{  					
				        	stop_rap();
				        	mode.step_abn = 4;
				    	}
				    	break;

			    	case 4:	 		
			        	if(do_action(3,angle360))								//前进
				    	{
					    	mode.step_abn = 5;
				    	}
				    	if((giv_sys_time - action.time) > 100000)				//10s
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 5;
				    	}
				    	break;
			    	case 5:
				    	if(do_action(2,angle90))								//右转
						{
					   		mode.step_abn = 6;
						} 	 
				    	if((giv_sys_time - action.time) > 30000)				//3s
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 6;
				    	}
						break; 
					case 6:	
				    	if(do_action(4,angle180))								//后退		  
				    	{
				        	//frontwheelon = 0;
							mode.abnormity = 0;
							mode.step = 0;
			            	return 0;
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	//frontwheelon = 0;
							mode.abnormity = 0;
			            	return 0;
				    	}
				    	break;	 
					case 7:	
				    	if(do_action(4,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	}
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	break;  
					case 8:	
				    	if(do_action(1,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	} 
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 4;
						}
				    	break;  
					case 9:	
				    	if(do_action(4,angle90))		  
				    	{
				        	//frontwheelon = 0;
							mode.abnormity = 0;
							mode.step = 0;
			            	return 0;
				    	}  
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	break;
		    	}  
				if((giv_sys_time - piv_abntime) > 250000)
				{
			    	stop_rap();	
//				    frontwheelon = 0;
					mode.abnormity = 0;
					mode.step = 0;
			        return 0;
				}
				return 1;
			}
			else
			{  
	 	        switch (mode.step_abn)
		    	{
		        	case  0:	
			        	piv_abntime = giv_sys_time;
				    	mode.step_abn = 2;
			        	return 1;
			    	case  1:
				    	if(do_action(1, angle15 * (giv_sys_time % 10)))
			        	{
				        	mode.abnormity = 0;
							mode.step = 0;
					    	return 0;					
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				   
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
				    	return 1;
					case 2:
			        	if(do_action(4,angle60))
		            	{
				        	mode.step_abn = 3;
			            	return 1;
			        	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
						break;
			    	case 3:
				    	if(do_action(1,angle45))
				    	{
					    	mode.step_abn = 4;
				    	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					
				        	stop_rap();
				        	mode.step_abn = 4;
				    	}
				    	break;

			    	case 4:	 		
			        	if(do_action(3,angle60))
				    	{
					    	mode.step_abn = 5;
				    	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 5;
				    	}
				    	break;
			    	case 5:
				    	if(do_action(2,angle90))
						{
					   	mode.step_abn = 6;
						} 	 
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 6;
				    	}
						break; 
					case 6:	
				    	if(do_action(4,angle90))		  
				    	{
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
			            	return 1;
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;	 
					case 7:	
				    	if(do_action(4,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	}
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;  
					case 8:	
				    	if(do_action(1,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	} 
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 4;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;  
					case 9:	
				    	if(do_action(4,angle90))		  
				    	{
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
			            	return 1;
				    	}  
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;
		    	}  
				if((giv_sys_time - piv_abntime) > 150000)
				{
			    	stop_rap();
			    	giv_sys_err = mode.abnormity;
			    	gbv_action_m_fangxiang = false;
					Init_Err();
				}
				return 1;
			}
		}

	}
    mode.abnormity=0;
    return 0;
}
#endif
/**************************************
左转
输入参数    longth    所转的脉冲数
输出  无
***************************************/
void  left(u32 longth)
{
 enable_rap(BACK, longth,FRONT,longth);
		// unsigned int action;		//动作   0:停止  1:原地左转  2原地右转 3前进   4后退   5旋转1  6：旋转2  。。。18旋转14	，19走螺旋线
	mode.action = 1; 	

}
/**************************************
左转
输入参数    longth    所转的脉冲数
输出  无
***************************************/
void  right(u32 longth)
{
enable_rap(FRONT, longth,BACK,longth);
		// unsigned int action;		//动作   0:停止  1:原地左转  2原地右转 3前进   4后退   5旋转1  6：旋转2  。。。18旋转14	，19走螺旋线
	mode.action = 2; 	
}

 /**************************************
左转
输入参数    longth    所转的脉冲数
输出  无
***************************************/

void  back(u32 longth)
{
  enable_rap(BACK, longth,BACK,longth); 
	// unsigned int action;		//动作   0:停止  1:原地左转  2原地右转 3前进   4后退   5旋转1  6：旋转2  。。。18旋转14	，19走螺旋线
	mode.action = 4; 	        
}
/**************************************
左转
输入参数    longth    所转的脉冲数
输出  无
***************************************/
void  forward(u32 longth)
{
	enable_rap(FRONT, longth,FRONT,longth);
// unsigned int action;		//动作   0:停止  1:原地左转  2原地右转 3前进   4后退   5旋转1  6：旋转2  。。。18旋转14	，19走螺旋线
	mode.action = 3;
}
/*************************************************
清除路径有关的内存
*************************************************/
void clr_ram(void)
{
	mode.bump=0;
	mode.step=0;
	mode.step_bp=0;
	mode.abnormity=0;
	mode.step_abn=0;
	mode.step_mk=0;
	mode.time = giv_sys_time;
	mode.bump_flag=false;
	action.sign=0;
	stop_rap();
	disable_pwm(CHARGE_PWM);
	power.pwm = 0;
}




/*****************************************
根据动作代码执行指定动作
*****************************************/
//QZ:输出:0,本动作或者其他动作正在进行中;1,已停止
//QZ:dm=0,return; dm=1,左转; dm=2,右转; dm=3,前进; dm=4,后退; dm=5,左轮不动向左转; dm=6,右轮不动向右转;
//	 dm=7,右轮不动向左转; dm=8,左轮不动向右转 18:顺时针走螺旋形; 19:逆时针走螺旋形; 21 走螺旋线
u8 do_action(u8 dm,u32 length)
{
	u32 m;

	Enable_RingPWMCtrl();
	Open_Ring_Cnt();
	if(!action.sign)
		{
				if(length == 0)
					{return 1;}
					
				r_rap.State = 0x00;
				action.sign = 1;
				action.step = 0;
				action.time = giv_sys_time;
				action.length = length;
				
		}
		
	m = (giv_sys_time-action.time);
	if((m>600000)&&(dm<18))
		{
			 stop_rap();
			 return 1;
		}	 
	
	switch (dm)
		{
			case  0:
				return 1;
		//---------------------------------------------	
		//---------------------------------------------	
			case  1:							//左转
				if(!action.step)
					{
						left(action.length);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign)&&(!r_rap.sign))
					{
						action.sign=0;		 
						return 1;
					}
			return 0;
			case  2:							//右转
				if(!action.step)
					{
						right(action.length);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign)&&(!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}
			return 0;
			case  3:							//前进
				if(!action.step)
					 {
						forward(action.length);
						action.step=1;
						return 0;
					 }
				if((!l_rap.sign)&&(!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}
			return 0;
			case  4:							//后退
				if(!action.step)
					{
						back(action.length);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign)&&(!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}
			return 0;
			case  5:				//左轮不动,右轮向前,左转
				if(!action.step)
					{
						enable_rap(FRONT,0,FRONT,action.length);
						action.step=1;
						return 0;
					}
				if((!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}  
			return 0;
			case  6:			   //右轮不动,左轮向前,右转
				if(!action.step)
					{
						enable_rap(FRONT,action.length,FRONT,0);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign))
					{
						action.sign=0;
						return 1;
					}  
			return 0;	 
			case  7:				//右轮不动,左轮向后,左转
				if(!action.step)
					{									 
						enable_rap(BACK,action.length,BACK,0);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign))
					{
						action.sign=0;
						return 1;
					}  
			return 0;
			case  8:			   //左轮不动向右转，右转
				if(!action.step)
					{								 
						enable_rap(BACK,0,BACK,action.length);
						action.step=1;
						return 0;
					}
				if((!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}  
			return 0;
			case  18:
				if(louxuan( 0 ))	//顺时针走螺旋线	 
					{
						action.sign=0;       
						return 1;
					}
				else
			 return 0;
			case  19:
				if(louxuan( 1 ))	//逆时针走螺旋线	 
					{
						action.sign=0;       
						return 1;
					}
				else
			return 0;
			default :
				stop_rap();
		}
	return 1;
}

u8 do_action_my(u8 dm,u32 length,short tgt_yaw)
{
	u32 m;
	Enable_RingPWMCtrl();
	Open_Ring_Cnt();
	if(!action.sign)
		{
			if(length == 0)
				{return 1;}
				
			r_rap.State = 0x00;
			action.sign = 1;
			action.step = 0;
			action.time = giv_sys_time;
			action.length = length;
		}
		
	m = (giv_sys_time-action.time);
	if((m>600000)&&(dm<18))
		{
			 stop_rap();
			 return 1;
		}	 
	
	switch (dm)
		{
			case  0:
				return 1;
		//---------------------------------------------	
		//---------------------------------------------	
			case  1:							//左转
				if(!action.step)
					{
						left(action.length);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign)&&(!r_rap.sign))
					{
						action.sign=0;		 
						return 1;
					}
			return 0;
			case  2:							//右转
				if(!action.step)
					{
						right(action.length);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign)&&(!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}
			return 0;

			case  3:							//前进
				if(!action.step)
					{
						//forward(action.length);
						enable_rap_yaw(FRONT,action.length,FRONT,action.length,tgt_yaw);
						mode.action=3;
						action.step=1;
						return 0;
					}
					if((!l_rap.sign)&&(!r_rap.sign))
						{
							action.sign=0;
							return 1;
						}
			return 0;

			case  4:							//后退
				if(!action.step)
					{
						//back(action.length);
						enable_rap_yaw(BACK,action.length,BACK,action.length,tgt_yaw);
						mode.action=4;
						action.step=1;
						return 0;
					}
				if((!l_rap.sign)&&(!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}
			return 0;
			case  5:				//左轮不动,右轮向前,左转
				if(!action.step)
					{
						enable_rap(FRONT,0,FRONT,action.length);
						action.step=1;
						return 0;
					}
				if((!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}  
			return 0;
			case  6:			   //右轮不动,左轮向前,右转
				if(!action.step)
					{
						enable_rap(FRONT,action.length,FRONT,0);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign))
					{
						action.sign=0;
						return 1;
					}  
			return 0;	 
			case  7:				//右轮不动,左轮向后,左转
				if(!action.step)
					{									 
						enable_rap(BACK,action.length,BACK,0);
						action.step=1;
						return 0;
					}
				if((!l_rap.sign))
					{
						action.sign=0;
						return 1;
					}  
			return 0;
			case  8:			   //左轮不动向右转，右转
				if(!action.step)
					{								 
						enable_rap(BACK,0,BACK,action.length);
						action.step=1;
						return 0;
					}
				if((!r_rap.sign))
					{
						action.sign=0;
						return 1;
					}  
			return 0;
			case  18:
				if(louxuan( 0 ))	//顺时针走螺旋线	 
					{
						action.sign=0;       
						return 1;
					}
				else
			return 0;
			case  19:
				if(louxuan( 1 ))	//逆时针走螺旋线	 
					{
						action.sign=0;       
						return 1;
					}
				else
			return 0;
			default :
				stop_rap();
		}
	return 1;
}




/***************************************************
走螺旋线，圈数由前面给出， 此程序由   do_action()调用。
螺旋线不能大于20圈  
输入：f = 1,向左转圈；f = 0，向右转圈。
返回  如果完成了则返回1，否则返回0；
***************************************************/
u8 louxuan(  u8  f)
{
  switch (action.step)
  {
   case  0:
//       if( f != 0)
//		  {
//	     left(ANGLE_360);				 //原地转圈1圈
//		  }
//		  else
//		  {
//		   right(ANGLE_360);				 //原地转圈1圈
//		  }
		  action.step++;
		  return 0;
   case   1:
   case   2:
   case   3:
   case   4:
      if((!l_rap.sign)&&(!r_rap.sign))
		  {
		   if( f != 0)
		   {
////         enable_rap(BACK, (ANGLE_180 - action.step*ANGLE_45), FRONT, (ANGLE_180 + action.step*ANGLE_45));
         enable_rap(FRONT, (action.step*METE_0_2), FRONT, (action.step*METE*2));
			 }
			 else
			 {
////			   enable_rap(FRONT, (ANGLE_180+action.step*ANGLE_45), BACK, (ANGLE_180-action.step*ANGLE_45)); 
         enable_rap(FRONT, (action.step*METE*2), FRONT, (action.step*METE_0_2));
			 }
			 action.step++;
		  }
		  return 0;
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
   case   18:
   case   19:
   case   20:
   case   21:
   case   22:
   case   23:
   case   24:
   case   25:
   case   26:
   case   27:
   case   28:
   case   29:
   case   30:
   case   31:
   case   32:
   case   33:
   case   34:
   case   35:
   case   36:
   case   37:
   case   38:
   case   39:
   case   40:
      if((!l_rap.sign)&&(!r_rap.sign))
		  { 
         if( (action.step==(action.length*2-1))|| (action.step==40))
         {
          action.sign=0;
          return 1;
         }	
         if( f != 0)
         {
          enable_rap(FRONT, (action.step-4)*ANGLE_45, FRONT, (action.step-4)*ANGLE_45 + ANGLE_360);
         }
         else
         {
            enable_rap(FRONT,(action.step-4) * ANGLE_45+ ANGLE_360, FRONT, (action.step-4)*ANGLE_360);
         } 					 
         action.step++;
		  }
		  return 0;
  }
  return 1;
}
/***********************************************
程序名：read_earth
程序说明：读取地检信号
返回：在有地检信号时停止动作，将 mode.bump置为其
      地检的值。
***********************************************/
u8 read_earth(void)
{						   
				if(read_sideearth())
				  {
				    return 1;
				  } 
//				if(e_lm.sign == FARN)
//				  {
//				    stop_rap();
//						mode.step_bp = 0;
//						mode.bump = 2;
//				    return 1;
//				  }   
//				if(e_rm.sign == FARN)
//				  {
//				    stop_rap();
//						mode.step_bp = 0;
//						mode.bump = 3;
//				    return 1;
//				  } 
		return 0;		  
}
/***********************************************
程序名：read_earth
程序说明：读取地检信号
返回：在有地检信号时停止动作，将 mode.bump置为其
      地检的值。
***********************************************/
u8 read_sideearth(void)
{						   
		if(e_r.sign == FARN)
				  {
				    stop_rap();
						mode.step_bp = 0;
						mode.bump = 4;
				    return 1;
				  } 
					
		if(e_l.sign == FARN)
				  {
				    stop_rap();
						mode.step_bp = 0;
						mode.bump = 1;
				    return 1;
				  } 

		return 0;		  
}



//====================================================================================================================
//====================================================================================================================
//====================================================================================================================
/************************************************************
核对当前时间是否与预约时间一致，如果是则返回1；否则返回0
************************************************************/
u8 Check_PreengageTime(void)
{
#if 0
u16 piv_t;
u8  piv_hour;
u8  piv_minute;
static u8 piv_preengage;
    piv_minute = (u8)((Rtc_time % 3600) / 60);
    piv_hour = (u8)((Rtc_time % 86400) / 3600) ;  
    piv_t = (piv_hour << 8)	+ piv_minute;
	if(piv_t == PreengageTime)
	{
	    if(piv_preengage == 1)
		{
		    piv_preengage = 0;
	        if((((1<<(Rtc_time/86400))& PreengageDate) != 0))
	        {
		        return 1;
		    }
		}
	}
	else
	{
	    piv_preengage = 1;
	} 
	return 0;  
#endif

	#if 1	//qz add
	u8 week_day,piv_hour,piv_min;
	week_day=(Rtc_time)/86400;
    piv_hour = (u8)((Rtc_time % 86400) / 3600); 
    piv_min = (u8)((Rtc_time % 3600) / 60);
	if(week_day==0)
		week_day=(1<<6);			//周日为0100 0000b
	else
		week_day=1<<(week_day-1);
	for(int i=0;i<PREEN_LENGTH;i++)
		{
			if(Preen_Data[i].Flag)											//7个预约时间，每个的标记是否ENABLE,从第一个预约开始检查
				{															//预约时间标记有效

#if 0
					if(week_day&Preen_Data[i].Preen_Week_Num)				//检查预约周几时间是否到达
						{													//预约周几时间到达
							if((Preen_Data[i].Preen_Hour==piv_hour)&(Preen_Data[i].Preen_Min==piv_min))		//预约小时分钟时间是否到达
								{
									if(!Preen_Data[i].Cycle)
										Preen_Data[i].Flag=0;				//如果没有循环，则取消标记ENABLE，下一次不需要打扫
									Preen_Data[i].Hour_done=1;
									return 1;								//即刻中断此函数，返回1，开始准备打扫

								}
							else
								Preen_Data[i].Hour_done=0;
						}
#endif
					if((Preen_Data[i].Preen_Hour==piv_hour)&(Preen_Data[i].Preen_Min==piv_min))
						{
							if(!Preen_Data[i].Cycle)
								{
									Preen_Data[i].Flag=0;
									CleanPreenData();			//qz add 20180428:Flag被清零，表示单次的预约完成，Flag被更改，需要重新保存，
																//不保存的话，如果中途被关机了，重新开机时，读到的预约数据Flag仍然为1，时间到后，又会开始。
									return 1;
								}
							else if(week_day&Preen_Data[i].Preen_Week_Num)
								{
									Preen_Data[i].Hour_done=1;
									return 1;
								}
						}
					return 0;
				}
		}

	return 0;																//一个预约时间都没有到，返回0
	#endif
	
}
/************************************************************
初始化碰撞以及保护
************************************************************/
void Init_Action(void)
{
   e_l.sign = NEAR;
   e_m.sign =NEAR;
   e_r.sign = NEAR;
//   bump_value=0;
   w_l.sign = FARN;
   w_lm.sign = FARN;
   w_m.sign = FARN;
   w_r.sign = FARN;
   w_rm.sign = FARN;
   l_ring.state = GOOD;
   r_ring.state = GOOD;
//   Enable_RingPWMCtrl();
//   Open_Ring_Cnt();
}
//===============================================================================================






/*******************************************************
功能：检查中扫和风机的是否在
返回：在返回0，否则返回1
*******************************************************/
u8 CheckMDustMotor(void)
{
//u16 piv_dust,piv_m;
//u16 v;
//    piv_dust = account_current(DUST_CURRENT);
//	piv_m = account_current(ADC_MB_CURR);
//	Set_Dust_Motor();
//    pbv_state_dust_motor = true;
//	DelayMs(10);
//	v = account_current(DUST_CURRENT) ;
//	if((v < piv_dust)||(( v - piv_dust) < 10))
//	{
//	     DelayMs(10);
//	     v = account_current(DUST_CURRENT) ;
//		 if((v < piv_dust)||(( v - piv_dust) < 10))
//		 {
//	         giv_sys_err = 33;
//		     Init_Err();
//		     return 1;//默认1   屏蔽风机
//		 }
//	}
//  Reset_M_ZNMOS();
//	Set_M_ZPMOS();
//	pbv_state_m_motor = true;
//	gbv_action_m_fangxiang = false;
//	DelayMs(10);
//	v = account_current(ADC_MB_CURR) ;
//	if((v < piv_m)||(( v - piv_m) < 10))
//	{
//	     DelayMs(10);
//	     v = account_current(ADC_MB_CURR) ;
//		 if((v < piv_m)||(( v - piv_m) < 10))
//		 {
//	         giv_sys_err = 34;
//		     Init_Err();
//		     return 1;
//		 }
//	}
	return 0;
}

void Enable_Free_Skid_Check(void)
{

	Free_Skid.check_flag=true;
	//Free_Skid.check_start_time=giv_sys_time;
	//Free_Skid.length=w_ring.length;
	Free_Skid.check_step=0;
	Free_Skid.check_start_time=giv_sys_time;
}

void Disable_Free_Skid_Check(void)
{
	Free_Skid.check_flag=false;
	Free_Skid.check_step=0;
}
//qz add，万向轮打滑检测
u8 Check_Free_Sikd(void)
{
	u8 data=0;
#if 0
	if((Free_Skid.check_flag!=0)&&((giv_sys_time-Free_Skid.check_start_time)>=30000))		//qz add:20180320修改时间为3s
		{
			if(w_ring.length<=Free_Skid.length+5)
			{
				if(Free_Skid.check_step==0)						//万向轮打滑预检测，但没有接收到波形，停止检测
					{
						Free_Skid.check_start_time=giv_sys_time;			//重新加载时间，准备进入检测
						Free_Skid.length=w_ring.length; 	//重新加载脉冲，准备进入检测
						Free_Skid.check_step=0;					//重新加载步骤，准备重新进入检测
						return 0;
					}
				else if(Free_Skid.check_step==1)
					{
						Free_Skid.check_start_time=giv_sys_time;			//重新加载时间，准备进入检测
						Free_Skid.length=w_ring.length; 	//重新加载脉冲，准备进入检测
						Free_Skid.check_step=0; 				//万向轮打滑检测本轮FAIL,action_flag=0,准备重新下一轮检测
#ifdef EFFICENT_DEBUG

						TRACE("SKID OCCURE!!!\r\n");		//qz add 20180323:actiong_flag再本轮检测为打滑的情况下，表明之前是收到脉冲了，可以继续测试
#endif
						return 1;
					}
			}
			else 
			{
				if(Free_Skid.check_step==0)						//万向轮打滑预检测，接收到脉冲，准备开始万向轮检测
					{
						Free_Skid.check_step++;
						Free_Skid.check_start_time=giv_sys_time;			//重新加载时间，准备进入检测
						Free_Skid.length=w_ring.length;		//重新加载脉冲，准备进入检测
						return 0;
					}
				else if(Free_Skid.check_step==1)					//万向轮打滑检测	
					{
						Free_Skid.check_step=0;					//万向轮打滑检测本轮OK，action_flag=1，进入万向轮打滑预检测//qz add 20180323:actiong_flag再本轮检测为打滑的情况下，表明之前是收到脉冲了，可以继续测试
						Free_Skid.check_start_time=giv_sys_time;			//重新加载时间，准备进入检测
						Free_Skid.length=w_ring.length;		//重新加载脉冲，准备进入检测
						return 0;
					}
			}
		}
	return 0;
#endif

#if 1
#ifdef FREE_FIRST_BAD_CHECK		//qz add 20181011
	if((Free_Skid.check_flag)&(!Free_Skid_First.fail))
#else
	if(Free_Skid.check_flag)
#endif
		{
		switch(Free_Skid.check_step)
			{
			case 0:
				Free_Skid.check_start_time=giv_sys_time;
				Free_Skid.length=w_ring.length;
			  	Free_Skid.check_step++;
				break;
			case 1:
				if(giv_sys_time-Free_Skid.check_start_time>30000)
					{
					
						Free_Skid.check_step=0;		//准备开始重新检测
						if(w_ring.length<=(Free_Skid.length+3))
						{
							data=1;					//打滑
#ifdef EFFICENT_DEBUG
							TRACE("W_R.leth=%d\r\n",w_ring.length);
							TRACE("f_sk.leth=%d\r\n",Free_Skid.length);
							
							TRACE("Free Skid Occure!\r\n");
#endif
						}
					}
				break;
			}
		}
	return data;
#endif	
}


//qz add 20180605
//独立的一个检测万向轮的程序,
//原理：在1min之内没有检测到万向轮脉冲输出，判定为已卡住且无法脱困
//条件：应该在运动的时候进行检查，且如果1min之内都是转弯则要避开。
#ifdef FREE_SKID_INDEP_CHECK
bool Check_Free_Skid_Indep(void)
{
	if((mode.status==0)|(mode.status==4))	//机器非工作状态或者重点打扫无需检测
		{
			Free_Skid_Indep.check_step=0;
			return false;
		}
	if(mode.mode==DOCKING)					//机器处于回充模式时,无需检查
		{

			Free_Skid_Indep.check_step=0;
			return false;
		}

#ifdef FREE_FIRST_BAD_CHECK					//qz add 20181011
	if((Free_Skid_First.check_flag)|(Free_Skid_First.fail))			//当进行万向轮第一次故障检查或者检查为故障状态时，万向轮独立检查无需进行
		{
			Free_Skid_Indep.check_start_time=0;
			return false;
		}

#endif

		if(Free_Skid_Indep.check_start_time>200000)
			{
				if(w_ring.length<Free_Skid_Indep.length+3)
					Free_Skid_Indep.fail=true;
				else
					{
						Free_Skid_Indep.check_start_time=0;
						Free_Skid_Indep.length=w_ring.length;
						Free_Skid_Indep.fail=false;
					}
			}

	return (Free_Skid_Indep.fail);
}

#endif

#ifdef FREE_FIRST_BAD_CHECK
void Enable_Free_First_Check(void)
{
	Free_Skid_First.check_flag=true;
	Free_Skid_First.check_start_time=giv_sys_time;
	Free_Skid_First.fail=false;
	Free_Skid_First.check_step=0;
	Free_Skid_First.length=w_ring.length;
}

void Disable_Free_First_Check(void)
{
	Free_Skid_First.check_flag=false;
	Free_Skid_First.check_start_time=giv_sys_time;
	Free_Skid_First.fail=false;
	Free_Skid_First.check_step=0;
	Free_Skid_First.length=w_ring.length;
}

void Free_First_Check(void)
{
	if(Free_Skid_First.check_flag)
		{
			switch(Free_Skid_First.check_step)
				{
					case 0:
						Free_Skid_First.check_start_time=giv_sys_time;
						Free_Skid_First.length=w_ring.length;
#ifdef EFFICENT_DEBUG
						TRACE("First_fk.len=%d\r\n",Free_Skid_First.length);
#endif
						Free_Skid_First.check_step++;
						break;
					case 1:
						if(giv_sys_time-Free_Skid_First.check_start_time>600000)		//1min
							{

#ifdef EFFICENT_DEBUG
								TRACE("First_fk.len=%d\r\n",Free_Skid_First.length);
								TRACE("w_ring.len=%d\r\n",w_ring.length);
#endif
								if((Free_Skid_First.length+9)>=w_ring.length)			//9为经验值qz modify 20181110
									{
										Free_Skid_First.fail=true;		//万向轮状态BAD
#ifdef EFFICENT_DEBUG
										TRACE("Free Bad!\r\n");
#endif
									}
								else
									{
										Free_Skid_First.fail=false;		//万向轮状态BAD
#ifdef EFFICENT_DEBUG
										TRACE("Free Good!\r\n");
#endif
									}
								Free_Skid_First.check_flag=false;				//无论检测万向轮状况是好或坏，都停止再次检测
							}
						break;
				}
		}
}
#endif

//打开旋转打滑检测（依托陀螺仪）
//输入参数:dir，0,左转;1,右转
void Enable_Rotate_Skid_Check(u8 dir)
{
	Rotate_Skid.check_flag=true;
	Rotate_Skid.time_flag=false;
	Rotate_Skid.check_time=0;
	Rotate_Skid.start_time=giv_sys_time;
	Rotate_Skid.skid_time=giv_sys_time;
	Rotate_Skid.start_angle=Gyro_Data.yaw;
#ifdef ROTATE_SKID_DEBUG
	TRACE("a.s=%d\r\n",Rotate_Skid.start_angle);
#endif
	Rotate_Skid.l_lenght=l_ring.all_length;
	Rotate_Skid.l_last_length=Rotate_Skid.l_lenght;
	Rotate_Skid.r_length=r_ring.all_length;
	Rotate_Skid.r_last_length=Rotate_Skid.r_length;
	Rotate_Skid.rot_angle=0.0;
	Rotate_Skid.direction=dir;

	Rotate_Skid.l_speed=l_ring.real_speed;
	Rotate_Skid.r_speed=r_ring.real_speed;
	Rotate_Skid.l_last_speed=Rotate_Skid.l_speed;
	Rotate_Skid.r_last_speed=Rotate_Skid.r_speed;

	Rotate_Skid.l_sum_length=0;
	Rotate_Skid.r_sum_length=0;
}

void Disable_Rotate_Skid_Check(void)
{
	Rotate_Skid.check_flag=false;
}

u8 Check_Gyro_Tick(void)
{
	switch(Gyro_Data.check_step)
		{
			case 0:
				Gyro_Data.start_check_time=giv_sys_time;
				Gyro_Data.check_step++;
			break;
			case 1:
				if(giv_sys_time-Gyro_Data.start_check_time>200000)			//20s
					{
						Gyro_Data.check_step=0;
						//qz add 20181101
						if((!Gyro_Data.first_rst)&(!mode.status))
							{
								Gyro_Rst();
								Gyro_Data.first_rst=true;
								return 0;
							}
						//qz add end
						return 1;
					}
			break;
		}
	return 0;
}

int Check_Gyro_angle(u8 dir)
{
	int result;
//	if(Rotate_Skid.direction==0)		//向左旋转，角度偏移量为正值
	if(dir==1)
		{
			result=((int)(Rotate_Skid.start_angle-Rotate_Skid.end_angle)/100);	//如果两者都大于0，用起始角减终止角，因为向右为负，向左为正
			if(result<0)
				result+=360; 
		}
	else								//向右旋转，角度偏移量为负值
		{
			result=(int)((Rotate_Skid.end_angle-Rotate_Skid.start_angle)/100);
			if(result<0)
				result+=360;
			result=0-result;
		}
	return result;
}

u8 Gyro_Pitch_Check(void)
{
	if(mode.status==0)
		return 0;
#if 0
	switch (Gyro_Data.pitch_check_step)
		{
			case 0:
				if((abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>=PITCH_ANGLE_BIOS)
					{
						Gyro_Data.pitch_check_step++;
						Gyro_Data.pitch_check_time=giv_sys_time;
					}
				break;
			case 1:
				if(giv_sys_time-Gyro_Data.pitch_check_time>PITCH_TIME)			//500ms
					{
						Gyro_Data.pitch_check_step=0;
						if(Gyro_Data.pitch>Gyro_Data.first_pitch)			//抬头
							return 1;
						else												//抬尾
							return 2;
					}
				if((abs(Gyro_Data.pitch-Gyro_Data.first_pitch))<PITCH_ANGLE_BIOS)
					{
						Gyro_Data.pitch_check_step=0;
						return 0;
					}
				break;
		}
	return 0;
#else
	static u32 ok_cnt=0,fail_cnt=0;
	if(!gyro_check_time)
		return 0;
	gyro_check_time=false;
	switch(Gyro_Data.pitch_check_step)
		{
			case 0:
				if((abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>=PITCH_ANGLE_BIOS)
					{
						Gyro_Data.pitch_check_step++;
						Gyro_Data.pitch_check_time=giv_sys_time;
						ok_cnt=0;fail_cnt=0;
					}
				break;
			case 1:
				if(giv_sys_time-Gyro_Data.pitch_check_time>PITCH_TIME)
					{
						Gyro_Data.pitch_check_step++;
						return 0;
					}
				if((abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>=PITCH_ANGLE_BIOS)
					fail_cnt++;
				else
					ok_cnt++;
				break;
			case 2:
				Gyro_Data.pitch_check_step=0;
				if(fail_cnt+10>=ok_cnt)
					{
						if(Gyro_Data.pitch>Gyro_Data.first_pitch)			//抬头
							return 1;
						else												//抬尾
							return 2;
					}
				break;
		}
	return 0;
#endif
}


/*--------------------------------------------
当pitch角度100ms内与初始角度相差3度，
左右轮开始加速
---------------------------------------------*/
u8 Gyro_Pitch_Speedup(void)
{
	static u8 check_step=0;
	static u32 check_time=0;
	if(mode.status==0)
		return 0;

	if(!mode.speed_up)
		{
			switch(check_step)
				{
					case 0:
						if(((abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>=SPEEDUP_ANGLE_BIOS)\
							|((abs(Gyro_Data.roll-Gyro_Data.first_roll))>=SPEEDUP_ANGLE_BIOS))		//3度
							{
								check_step++;
								check_time=giv_sys_time;
							}
						return 0;
					case 1:
						if(giv_sys_time-check_time>SPEEDUP_TIME)//SPEEDUP_TIME)							//100ms
							{
								check_step=0;
								return 1;
							}
						if(((abs(Gyro_Data.pitch-Gyro_Data.first_pitch))<SPEEDUP_ANGLE_BIOS)\
							&((abs(Gyro_Data.roll-Gyro_Data.first_roll))<SPEEDUP_ANGLE_BIOS))
							{
								check_step=0;
								return 0;
							}
						return 0;
				}
		}
	else		//(mode.speed_up==1)
		{
			switch(check_step)
				{
					case 0:
						if(((abs(Gyro_Data.pitch-Gyro_Data.first_pitch))<SPEEDUP_ANGLE_BIOS)\
							&((abs(Gyro_Data.roll-Gyro_Data.first_roll))<SPEEDUP_ANGLE_BIOS))		//3度
							{
								check_step++;
								check_time=giv_sys_time;
							}
						return 1;
					case 1:
						if(giv_sys_time-check_time>SPEEDUP_TIME)							//100ms
							{	
								check_step=0;
								return 0;
							}
						if(((abs(Gyro_Data.pitch-Gyro_Data.first_pitch))>=SPEEDUP_ANGLE_BIOS)\
							|((abs(Gyro_Data.roll-Gyro_Data.first_roll))>=SPEEDUP_ANGLE_BIOS))
							{
								check_step=0;
								return 1;
							}
						return 1;
				}
		}
	return 0;
}

u8 Gyro_Roll_Check(void)
{
	if(mode.status==0)
		return 0;
	
	switch (Gyro_Data.roll_check_step)
		{
			case 0:
				if((abs(Gyro_Data.roll-Gyro_Data.first_roll))>ROLL_ANGLE_BIOS)
					{
						Gyro_Data.roll_check_step++;
						Gyro_Data.roll_check_time=giv_sys_time;
					}
				break;
			case 1:
				if(giv_sys_time-Gyro_Data.roll_check_time>600000)
					{
						Gyro_Data.roll_check_step=0;
						return 1;
					}
				if((abs(Gyro_Data.roll-Gyro_Data.first_roll))<ROLL_ANGLE_BIOS)
					{
						Gyro_Data.roll_check_step=0;
						return 0;
					}
				break;
		}
	return 0;
}

u8 Gyro_PitchRoll_Check(void)
{
	int pitch_dif,roll_dif;
	if(mode.status==0)
		return 0;
#if 0
	switch (Gyro_Data.pitchroll_check_step)
		{
			case 0:
				pitch_dif=abs(Gyro_Data.first_pitch-Gyro_Data.pitch);
				roll_dif=abs(Gyro_Data.first_roll-Gyro_Data.roll);
				if(pitch_dif+roll_dif>=PITCHROLL_ANGLE_BIOS)								//6度
					{
						Gyro_Data.pitchroll_check_step++;
						Gyro_Data.pitchroll_check_time=giv_sys_time;
					}
				break;
			case 1:
				pitch_dif=abs(Gyro_Data.first_pitch-Gyro_Data.pitch);
				roll_dif=abs(Gyro_Data.first_roll-Gyro_Data.roll);
				if(giv_sys_time-Gyro_Data.pitchroll_check_time>PITCH_TIME)	//500ms
					{
						Gyro_Data.pitchroll_check_step=0;
						return 1;
					}
				if(pitch_dif+roll_dif<PITCHROLL_ANGLE_BIOS)								//6度
					{
						Gyro_Data.pitchroll_check_step=0;
					}
				break;
		}
#else
	static u32 fail_cnt=0,ok_cnt=0;
	switch (Gyro_Data.pitchroll_check_step)
		{
			case 0:
				pitch_dif=abs(Gyro_Data.first_pitch-Gyro_Data.pitch);
				roll_dif=abs(Gyro_Data.first_roll-Gyro_Data.roll);
				if(pitch_dif+roll_dif>=PITCHROLL_ANGLE_BIOS)								//6度
					{
						Gyro_Data.pitchroll_check_step++;
						Gyro_Data.pitchroll_check_time=giv_sys_time;
						fail_cnt=0;ok_cnt=0;
					}
				break;
			case 1:
				if(giv_sys_time-Gyro_Data.pitchroll_check_time>PITCH_TIME)
					{
						Gyro_Data.pitchroll_check_step++;
						return 0;
					}
				pitch_dif=abs(Gyro_Data.first_pitch-Gyro_Data.pitch);
				roll_dif=abs(Gyro_Data.first_roll-Gyro_Data.roll);
				if(pitch_dif+roll_dif>=PITCHROLL_ANGLE_BIOS)
					fail_cnt++;
				else
					ok_cnt++;
				break;
			case 2:
				Gyro_Data.pitchroll_check_step=0;
				if(fail_cnt+10>=ok_cnt)
					return 1;
		}
#endif
	return 0;
}

u8 Check_Rotate_Skid(void)
{
	int rota_angle_change=0,gyro_angle_change=0;
	if(Rotate_Skid.check_flag)
		{
			if(Rotate_Skid.check_time<70 )
				{
					if(Rotate_Skid.time_flag)
						{
							Rotate_Skid.time_flag=false;
							//获取增量
							Rotate_Skid.l_sum_length+=(l_ring.all_length-Rotate_Skid.l_lenght);
							Rotate_Skid.l_lenght=l_ring.all_length;
							Rotate_Skid.r_sum_length+=(r_ring.all_length-Rotate_Skid.r_length);
							Rotate_Skid.r_length=r_ring.all_length;
							Rotate_Skid.check_time++;
						}
					return 0;
				}
			else
				{
					Rotate_Skid.end_angle=Gyro_Data.yaw;
					rota_angle_change=(int)(((float)(Rotate_Skid.r_sum_length-Rotate_Skid.l_sum_length))*PULSE_LENGTH*360/(2*3.141592*RING_RANGE));
					//因为有时候无法判断机器在左转还是右转，比如沿边的时候，
					//所以通过里程计计算的角度来判断方向
					u8 dir=0;
					if(rota_angle_change>=0)
						dir=1;
					else
						dir=0;
					//因为以最大速度300mm/s来跑，500ms最多跑73度，如果绝对值大于180度，判定方向计算错误，
					//需要纠正
					gyro_angle_change=Check_Gyro_angle(dir);
					if(abs(gyro_angle_change)>180)
						{
							if(gyro_angle_change>0)
								gyro_angle_change=360-gyro_angle_change;
							else
								gyro_angle_change=0-(gyro_angle_change+360);
						}

					while(abs(rota_angle_change)>360)
						{
							rota_angle_change=rota_angle_change/360;
						}
#ifdef ROTATE_SKID_DEBUG
					TRACE("a_c=%d\r\n",gyro_angle_change);
					TRACE("r_c=%d\r\n",rota_angle_change);
#endif
					Enable_Rotate_Skid_Check(0);
					#if 1
						if(((gyro_angle_change>0)&&(rota_angle_change<0))||((gyro_angle_change<0)&&(rota_angle_change>0)))
							{
								stop_rap();
								Rotate_Skid.skid_time=giv_sys_time;
								return 1;
							}
						else if((abs(gyro_angle_change-rota_angle_change))>ROTATE_OFFSET)
							{
//								stop_rap();
								Rotate_Skid.skid_time=giv_sys_time;
#ifdef ROTATE_SKID_DEBUG
								TRACE("Rotate Skid Occure!\r\n");
#endif
								return 1;
							}
						//else
							//return 2;
					#endif
				}
		}
	return 0;
}

#ifdef YBS_DIS_RESTORE
void Init_Rotate_Angle(void)
{
	rotate_angle.l_length=l_ring.all_length;
	rotate_angle.r_length=r_ring.all_length;
	rotate_angle.l_last_length=rotate_angle.l_length;
	rotate_angle.r_last_length=rotate_angle.r_length;
	rotate_angle.l_sum_length=0;
	rotate_angle.r_sum_length=0;
	rotate_angle.check_time=0;
	rotate_angle.start_time=giv_sys_time;
	rotate_angle.check_step=0;
}

void Enable_Rotate_Angle(void)
{
	rotate_angle.check_flag=true;
	Init_Rotate_Angle();
}
void Disable_Rotate_Angle(void)
{
	rotate_angle.check_flag=false;
}
void Check_Rotate_Angle(void)
{
	if(rotate_angle.check_flag)
		{
			switch(rotate_angle.check_step)
				{
					case 0:
						//Init_Rotate_Angle();
						//rotate_angle.rot_angle=0.0;
						rotate_angle.check_step++;
					break;
					case 1:
						if(rotate_angle.time_flag)
						{
							rotate_angle.time_flag=false;
							//获取增量
							rotate_angle.l_sum_length+=(l_ring.all_length-rotate_angle.l_length);
							rotate_angle.l_length=l_ring.all_length;
							rotate_angle.r_sum_length+=(r_ring.all_length-rotate_angle.r_length);
							rotate_angle.r_length=r_ring.all_length;
							rotate_angle.check_time++;
							if(rotate_angle.check_time>=70)	//700ms
								{
									rotate_angle.check_step++;
								}
						}
					break;
					case 2:
							//计算角度
						rotate_angle.rot_angle=(((float)(rotate_angle.r_sum_length-rotate_angle.l_sum_length))*PULSE_LENGTH*360/(2*3.141592*RING_RANGE));
						Init_Rotate_Angle();
					break;
				}
		}
}
#endif

//qz add在非休眠模式下检查SLAM心跳通信
//SLAM一分钟之内没有发送心跳，返回1，否则返回0
u8 SLAM_Tick_Check(void)
{
#if 0
	if(((giv_sys_time-Slam_Data.tick_time)>600000)&&(mode.mode!=SLEEP)&&(mode.mode!=DOCKING))
		{
			Slam_Data.tick_flag=0;
			return 1;
		}
	else
		return 0;
#endif

	//qz add 20180814
	if((mode.Info_Abort)|(mode.All_Info_Abort))
		{
			Slam_Data.tick_check_step=0;
			return 0;
		}
	//qz add end
	
	switch (Slam_Data.tick_check_step)
		{
  			case 0:
				Slam_Data.tick_time=giv_sys_time;
				Slam_Data.tick_check_step++;
				break;
			case 1:
				if(((giv_sys_time-Slam_Data.tick_time)>600000))
					{
						Slam_Data.tick_flag=false;
						Slam_Data.tick_check_step=0;
						return 1;
					}
				break;
		}
	return 0;
}

//u8 Retern_Wall_Dis(*WALL_DATA w_d)
//{
//	
//}

u8 Return_M_Dis(u16 data)

{
	#if 0
	u8 Distance=0;
	if(data < 0x20)
		{Distance = 200;}
		
	else if(data<0x40)
		{
			Distance=200-((data-0x20)*50/(0x40-0x20));
		}
	else if(data<0x50)
		{
			Distance=150-((data-0x40)*50/(0x50-0x40));
		}
	else if(data<0x80)
		{
			Distance=100-((data-0x60)*50/(0x80-0x60));
		}
	else if(data<0xC0)
		{
			Distance=50-((data-0x80)*10/(0xC0-0x80));
		}
	else if(data<0x1B0)
		{
			Distance=40-((data-0x1B0)*10/(0x1B0-0xC0));
		}
	else if(data<0x420)
		{
			Distance=30-((data-0x1B0)*10/(0x420-0x1B0));
		}
	else
		Distance=20;
	return(Distance);
	#endif


	u8 Distance=0;
	if(data < 0x20)
		{Distance = 200;}
		
	else if(data<0x40)
		{
			Distance=200-((data-0x20)*50/(0x40-0x20));
		}
	else if(data<0x50)
		{
			Distance=150-((data-0x40)*50/(0x50-0x40));
		}
	else if(data<0x80)
		{
			Distance=100-((data-0x60)*50/(0x80-0x60));
		}
	else if(data<0xC0)
		{
			Distance=50-((data-0x80)*10/(0xC0-0x80));
		}
	else if(data<0x1B0)
		{
			Distance=40-((data-0x1B0)*10/(0x1B0-0xC0));
		}
	else if(data<0x420)
		{
			Distance=30-((data-0x1B0)*10/(0x420-0x1B0));
		}
	else
		Distance=20;
	return(Distance);
}

u8 Return_LM_Dis(u16 data)

{
	u8 Distance=0;
	if(data < 0x50)
		{Distance = 200;}
		
	else if(data < 0x70)	//QZ:30<=DATA<40
		{
			Distance = 200 - (uint16_t)( ( (float)((data-0x50) * (200-120) / (0x70-0x50)) ) );	//QZ: 50<DISTANCE<=200
		}
	else if(data < 0x90)	//QZ:40<=DATA<63
		{
			Distance = 120 - (uint16_t)( ( (float)((data-0x70) * (120-100) / (0x90-0x70) )) 		);	//QZ: 70<DISTANCE<=100;
		}
	else if(data < 0xb0)	//QZ:63<=DATA<98
		{
			Distance = 100 - (uint16_t)( ( (float)((data-0x90) * (100-90) / (0xb0-0x90) )) 		);	//QZ:  58<DISTANCE<=70
		}
	else if(data < 0xc0)	//QZ:98<=DATA<167
		{
			Distance = 90 - (uint16_t)( ( (float)((data-0xb0) * (90-80) / (0xc0-0xb0) )) 		);	//QZ:  48<DISTANCE<=58
		}
	else if(data < 0xff)	//QZ:167<=DATA<220
		{
			Distance = 80 - (uint16_t)( ( (float)((data-0xc0) * (80-70) / (0xff-0xc0) )) 	);		//QZ:  45<DISTANCE<=48
		}
	else if(data < 0x140)	//QZ:220<=DATA<361
		{
			Distance = 70 - (uint16_t)( ( (float)((data-0xff) * (70-60) / (0x140-0xff) )) 	);		//QZ:  38<DISTANCE<=45
		}
	else if(data < 0x1b0)	//QZ:361<=DATA<470
		{
			Distance = 60 - (uint16_t)( ( (float)((data-0x140) * (60-50) / (0x1b0-0x140) ))	);		//QZ:  35<DISTANCE<=38
		}
	else if(data < 0x280)	//QZ:470<=DATA<830
		{
			Distance = 50 - (uint16_t)( ( (float)((data-0x1b0) * (50-60) / (0x280-0x1b0) )	) );	//QZ:  30<DISTANCE<=35
		}
	else if(data < 0x410)//QZ:830<=DATA<2050
		{
			Distance = 40 -  (uint16_t)( ( (float)((data-0x280) * (40-30)/ (0x410-0x280) ) )		);	//QZ:22<DISTANCE<=30
		}
	else if(data < 0x7a0)//QZ:830<=DATA<2050
		{
			Distance = 30 -  (uint16_t)( ( (float)((data-0x410) * (30-20)/ (0x7a0-0x410) ) )		);	//QZ:22<DISTANCE<=30
		}
	else if(data < 0xd60)//QZ:830<=DATA<2050
		{
			Distance = 20 -  (uint16_t)( ( (float)((data-0x7a0) * (20-10)/ (0xd60-0x7a0) ) )		);	//QZ:22<DISTANCE<=30
		}

	
	else		//QZ:DATA>=2025
		{
			Distance = 10; 
		}
		return Distance;
}

void Write_Mode_Backup(void)
{

	   #if 1	//qz add

//		   FLASH_Status Status;
		   if(!FLASH_GetWriteProtectionOptionByte())
		   FLASH_Unlock();

		   u32 addr=MODE_BACKUP_ADDR;
		   FLASH_ProgramHalfWord(addr,mode.mode);
		   FLASH_Lock();
	  #endif
}

u8 FAN_Skid_Check(void)
{
	if(Fan.flag)
		{
			switch (Fan.check_step)
				{
					case 0:
						Fan.check_time=giv_sys_time;
						Fan.last_length=Fan.length;
						Fan.check_step++;
					return 0;
					case 1:
						if(giv_sys_time-Fan.check_time>30000)
							{
								Fan.check_step=0;								
								if(Fan.length<=Fan.last_length)
									return 1;
								else
									return 0;									
							}
					return 0;	
				}
			
		}
	
	return 0;
}

//以下程序来自黄工风机调档
void Set_FJ_Level(u16 level)
{
  if(level==0)
  	{
  		FAN_PWR_CTL_0;
  		Fan.flag=false;
  	}
  else
  	{
  		FAN_PWR_CTL_1;
  		Fan.flag=true;
//  TIM_SetCompare4(TIM3,level);//标准  
		Set_Fan_Pwm(level);
  	}
}

void Set_BS_Level(u16 level)
{
	if(level==0)
		SideBrush.flag=false;
	else
		SideBrush.flag=true;
	Set_SideBrush_Pwm(level);
}

void Set_ZS_Level(u16 level)
{
//	TIM_SetCompare1(TIM3,level);
//	TIM_SetCompare2(TIM3,0);
	if(level==0)
		MidBrush.flag=false;
	else
		MidBrush.flag=true;
	Set_MidBrush_Pwm(level);
}

void Sweep_Level_Set(u16 sweep_level)
{
	if(mode.mode==DOCKING)
		sweep_level=SWEEP_LEVEL_DOCK;

	switch (sweep_level)
		{
			case SWEEP_LEVEL_STOP:
				Set_BS_Level(SWEEP_LEVEL_STOP);
				Set_ZS_Level(SWEEP_LEVEL_STOP);
				Set_FJ_Level(SWEEP_LEVEL_STOP);
			break;
			case SWEEP_LEVEL_STANDARD:
				Set_BS_Level(STANDARD_PWM);
				Set_ZS_Level(MB_STD_PWM);
				Set_FJ_Level(FAN_STD_PWM);
			break;
			case SWEEP_LEVEL_SILENCE:
				Set_BS_Level(SILENCE_PWM);
				Set_ZS_Level(SILENCE_PWM);
				Set_FJ_Level(SILENCE_PWM);
			break;
			case SWEEP_LEVEL_FORCE:
				Set_BS_Level(FORCE_PWM);
				Set_ZS_Level(MB_FORCE_PWM);
				Set_FJ_Level(FAN_FORCE_PWM);
			break;

			//qz add 20180515
			case SWEEP_LEVEL_DOCK:		//Dock_Mode:
				Set_BS_Level(DOCK_SWEEP_PWM);
				Set_ZS_Level(SWEEP_LEVEL_STOP);
				Set_FJ_Level(SWEEP_LEVEL_STOP);
			break;
			//qz add end
				
		}
}

u8 Lift_Check(void)
{
	u8 result=0;

	//qz add 20181210
	if((r_rap.ori==BACK)&(l_rap.ori==BACK))
		{
			l_lidi.check_step=0;
			r_lidi.check_step=0;
			return 0;
		}
	
	if(Key_Status_Check_Low(&l_lidi))
		result|=0x01;
	if(Key_Status_Check_Low(&r_lidi))
		result|=0x02;
	return result;
}

u8 Ring_Fix_Check(void)
{
	u8 result=0;
	if(l_ring.state==BAD)
		result|=0x01;
	if((r_ring.state==BAD))
		result|=0x02;
	return result;
}

u8 SB_OC_Check(void)
{
#if 0
	if((sb_current_1s*CURR_SB_CNT_mA)>SB_PROTECTCURRENT)
		return 1;
	else
		return 0;
#else
	if((sb_current_1s*CURR_SB_CNT_mA)>SB_PROTECTCURRENT)
		{
			switch(SideBrush.check_step)
				{
					case 0:
						SideBrush.check_time=giv_sys_time;
						SideBrush.check_step++;
					break;
					case 1:
						if(giv_sys_time-SideBrush.check_time>SB_CHECK_TIME)
							{
								SideBrush.check_step=0;
#ifdef EFFICENT_DEBUG
								TRACE("SB OC Ocurr!\r\n");
#endif
								return 1;
							}
					break;
				}
		}
	else

		{
			LSideBrush.check_step=0;
		}
#endif
	return 0;
}

u8 MB_OC_Check(void)
{
	if((m_current_1s*CURR_MB_CNT_mA)>M_PROTECTCURRENT)
		return 1;
	else
		return 0;		//qz add 20180515
}

u8 Ring_OC_Check(void)
{
	u8 result=0;
	if((l_current_1s*CURR_RING_CNT_mA)>WHEEL_PROTECTCURRENT)
		result|=0x01;
	if((r_current_1s*CURR_RING_CNT_mA)>WHEEL_PROTECTCURRENT)
		result|=0x02;
	return result;
}

u8 Fan_OC_Check(void)
{
#if 1
		if(Fan.flag)
			{
				switch(Fan.check_step)
					{
						case 0:
							if((dust_current_1s*CURR_FAN_CNT_mA)<=DUST_PROTECTCURRENT)
								{
									Fan.check_time=giv_sys_time;
									Fan.check_step++;
								}
							break;
						case 1:
							if((dust_current_1s*CURR_FAN_CNT_mA)>DUST_PROTECTCURRENT)
								{
									Fan.check_step=0;
									return 0;
								}
							if(giv_sys_time-Fan.check_time>100000)		//10s
								{
									Fan.check_step=0;
									return 1;
								}
							break;
					}
			}
		return 0;
#else
		if(dust_current_1s>DUST_PROTECTCURRENT)
			return 1;
		else
			return 0;
#endif
}

u8 Bump_Fix_Check(void)
{
#if 0
	if((m_bump.key==0)&(giv_sys_time-m_bump.time>30000))
		{
			return 1;
		}
	if((l_bump.key==0)&(giv_sys_time-l_bump.time>30000))
		return 1;
	if((r_bump.key==0)&(giv_sys_time-r_bump.time>30000))
		return 1;

	return 0;
#endif
	if(Key_Status_Check_Low(&l_bump))
		return 1;
	if(Key_Status_Check_Low(&r_bump))
		return 1;
	else
		return 0;		//qz add 20180515
}

u8 Dust_Box_Check(void)
{
	if(mode.mode==CHARGEING)		//qz add 20180902充电机器，不检查集尘盒
		return 0;
	if((Key_Status_Check_High(&dust_box))&(!dust_box.fail))
		{
			dust_box.fail=true;
			return 1;
		}	
	else
		return 0;		//qz add 20180515
}

//qz add 20180725
void Dust_Level_Check(void)
{
	static u8 high_times=0;
	static u32 check_time;
	if(dust_sta.key)
		{
			if(!high_times)
				check_time=giv_sys_time;
			high_times++;
			if(giv_sys_time-check_time>12000)
				{
					if(high_times>=3)
						Slam_Data.dust_level=3;
					high_times=0;
				}
		}
}

u8 Dust_Full_Check(void)
{
	if(dust_sta.key)
		{
			switch (dust_sta.check_step)
				{
					case 0:
						dust_sta.start_check_time=giv_sys_time;
						dust_sta.check_step++;
					case 1:
						if(giv_sys_time-dust_sta.start_check_time>3000000)		//5分钟
							{
								dust_sta.check_step=0;
								return 1;
							}
				}
		}
	else
		dust_sta.check_step=0;
	return 0;
}

u8 SB_FIX_Check(void)
{
	u8 data1=0;
	if(SideBrush.flag)
		{
		}
	return data1;
}

u8 Check_All_Cliff(void)
{
	static u8 step=0;
	static u32 check_time;
	switch (step)
		{
			case 0:
				if((e_l.sign==FARN)&(e_m.sign==FARN)&(e_r.sign==FARN))
					{
						step++;
						check_time=giv_sys_time;
					}
				break;
			case 1:
				if((e_l.sign==NEAR)|(e_m.sign==NEAR)|(e_r.sign==NEAR))
					{
						step=0;
					}
				else if(giv_sys_time-check_time>30000)		//qz modify 20181110 2s--->10s
					{
						step=0;
						return 1;
					}
				break;
		}
	return 0;
}


#ifdef WALL_EARTH_ERROR_CHECK
u8 Wall_Earth_Check_Error(WALL_DATA *wall)
{
	switch(wall->error_check_step)
		{
			case 0:
				if(wall->brightness>=(wall->darkness-3))
					{
						wall->error_check_step++;
						wall->error_check_time=giv_sys_time;
					}
				break;
			case 1:
				if(wall->brightness<=(wall->darkness-10))
					{
						wall->error_check_step=0;
						return 0;
					}
				if(giv_sys_time-wall->error_check_time>30000)
					{
						wall->error_check_step=0;
						return 1;
					}
				break;
		}
	return 0;
}

u8 Wall_Earth_Check(void)
{
	if((mode.mode==CEASE)|(mode.mode==CHARGEING)|(mode.mode==DOCKING))
		return 0;
	if(Wall_Earth_Check_Error(&w_l))
		return 1;
	if(Wall_Earth_Check_Error(&w_lm))
		return 1;
	if(Wall_Earth_Check_Error(&w_m))
		return 2;
	if(Wall_Earth_Check_Error(&w_rm))
		return 3;
	if(Wall_Earth_Check_Error(&w_r))
		return 3;

	if(Wall_Earth_Check_Error(&e_l))
		return 4;
	if(Wall_Earth_Check_Error(&e_m))
		return 5;
	if(Wall_Earth_Check_Error(&e_r))
		return 6;
	else
		return 0;
}
#endif

void Check_Status(void)
{
	u8 voice_addr=0,data1=0;
	if(CHECK_STATUS_FLAG&CHECK_STATUS_TIME)		//
		{
			CHECK_STATUS_TIME=false;

			////////风机卡住异常检测//////////////
#ifdef 		FAN_CHECK
#if 0
			if(FAN_Skid_Check())
#else
			if(Fan_OC_Check())
#endif
				{

					Fan.error_time++;
					if(Fan.error_time>=3)
						{
							error_code=SEND_ERROR_FANFIX;
							dis_err_code=DIS_ERROR_FAN_OC;
							voice_addr=VOICE_ERROR_FAN_OC;			//无需发送语音
							mode.err_code|=WIFI_ERR_FAN;
						}
				}
			
#endif

			/////////离地异常检测//////////////
#ifdef 		LIFT_CHECK
			//if(Lift_Check())
			if((l_lidi.fail)&(!r_lidi.fail))
				{
					error_code=SEND_ERROR_LEFTLIFT;
					dis_err_code=DIS_ERROR_LEFT_LIFT;
					voice_addr=VOICE_ERROR_L_LIFT;
				}
			else if((!l_lidi.fail)&(r_lidi.fail))
				{
					error_code=SEND_ERROR_RIGHTLIFT;
					dis_err_code=DIS_ERROR_RIGHT_LIFT;
					voice_addr=VOICE_ERROR_R_LIFT;
				}
			else if(l_lidi.fail&r_lidi.fail)
				{
					error_code=SEND_ERROR_DIPANLIFT;
					dis_err_code=DIS_ERROR_SWEEPER_LIFT;
					voice_addr=VOICE_ERROR_LIFT;
				}
#endif


#ifdef CLIFF_ENABLE
			if(Check_All_Cliff())
				{
					error_code=SEND_ERROR_DANGER;
					dis_err_code=DIS_ERROR_DANGER;
					voice_addr=VOICE_ERROR_DANGER;
				}
#endif


#ifdef 		SLAM_CHECK
			////////SLAM_TICK异常检测/////////
			if(SLAM_Tick_Check())
				{
					error_code=SEND_ERROR_SLAMCOM;
					dis_err_code=DIS_ERROR_SLAM_DOWN;
					voice_addr=VOICE_ERROR_SLAM_TICK;		//无需发送语音
				}
#endif

			//////驱动轮卡住异常检测///////////
#ifdef 		RING_FIX_CHECK
			//if(Ring_Fix_Check())
			if((l_rap.fail)&(!r_rap.fail))
				{
					error_code=SEND_ERROR_RINGFIX;
					dis_err_code=DIS_ERROR_LEFTRING_OC;
					voice_addr=VOICE_ERROR_L_RING;
					mode.err_code|=WIFI_ERR_LRING_OC;
				}
			else if((!l_rap.fail)&(r_rap.fail))
				{
					error_code=SEND_ERROR_RINGFIX;
					dis_err_code=DIS_ERROR_RIGHTRING_OC;
					voice_addr=VOICE_ERROR_R_RING;
					mode.err_code|=WIFI_ERR_RRING_OC;
				}
			if(l_rap.fail&r_rap.fail)
				{
					error_code=SEND_ERROR_RINGFIX;
					dis_err_code=DIS_ERROR_RIGHTRING_OC;
					voice_addr=VOICE_ERROR_R_RING;
					mode.err_code|=WIFI_ERR_LRING_OC;
					mode.err_code|=WIFI_ERR_RRING_OC;
				}
#endif

#ifdef 		RING_OC_CHECK
			//////驱动轮过电流异常检测//////////
			data1=Ring_OC_Check();
			if(data1)
				{
					error_code=SEND_ERROR_RINGOC;
					switch(data1)
						{
							case 1:
								error_code=SEND_ERROR_LEFTRINGOC;		//qz add 20180913
								dis_err_code=DIS_ERROR_LEFTRING_OC;
								voice_addr=VOICE_ERROR_L_RING;
								mode.err_code|=WIFI_ERR_LRING_OC;
								break;
							case 2:
								error_code=SEND_ERROR_RIGHTRINGOC;		//qz add 20180913
								dis_err_code=DIS_ERROR_RIGHTRING_OC;
								voice_addr=VOICE_ERROR_R_RING;
								mode.err_code|=WIFI_ERR_RRING_OC;
								break;
							case 3:
								error_code=SEND_ERROR_RIGHTRINGOC;		//qz add 20180913
								dis_err_code=DIS_ERROR_RIGHTRING_OC;
								voice_addr=VOICE_ERROR_R_RING;
								mode.err_code|=WIFI_ERR_LRING_OC;
								mode.err_code|=WIFI_ERR_RRING_OC;
								break;							
						}
				}
#endif

#ifdef 		MB_OC_CHECK
			//////中扫过流异常检测//////////////
			if(MB_OC_Check())
				{
					error_code=SEND_ERROR_MBOC;
					dis_err_code=DIS_ERROR_MB_OC;
					voice_addr=VOICE_ERROR_ZS_OC;
					mode.err_code|=WIFI_ERR_MB;
				}
#endif

#ifdef 		BUMP_FIX_CHECK
			//////碰撞开关卡住异常检测//////////
			//if(Bump_Fix_Check())
			if(l_bump.fail)
				{
					error_code=SEND_ERROR_BUMPFIX;
					dis_err_code=DIS_ERROR_BUMP_FIX;
					voice_addr=VOICE_ERROR_BUMP_FIX;
					mode.err_code|=WIFI_ERR_BUMP;
				}
#endif

			/////集尘盒存在异常检测////////////
#ifdef 		DUST_BOX_EXIST_CHECK
			u8 data1=0;
			data1=Dust_Box_Check();
			if(dust_box.fail)
				{
					error_code=SEND_ERROR_BOXNONE;
					dis_err_code=DIS_ERROR_BOX_NONE;
					voice_addr=VOICE_ERROR_DUSTBOX_NONE;
				}

#endif

			/////边刷卡住异常检测////////////
#ifdef 		SB_FIX_CHECK
			if(SideBrush.fail==1)
				{
					error_code=SEND_ERROR_LEFTSBOC;			//qz modify 20180913
					dis_err_code=DIS_ERROR_LEFTSB_OC;
					voice_addr=VOICE_ERROR_L_SB;
				}
			else if(SideBrush.fail==2)
				{
					error_code=SEND_ERROR_RIGHTSBOC;		//qz modify 20180913
					dis_err_code=DIS_ERROR_RIGHTSB_OC;
					voice_addr=VOICE_ERROR_R_SB;
				}
#endif

			/////单独万向轮打滑检查/////////
#ifdef		FREE_SKID_INDEP_CHECK
			if(Check_Free_Skid_Indep())
				{
					error_code=SEND_ERROR_SWEEPER_FIX;
					dis_err_code=DIS_ERROR_SWEEPER_FIX;
					voice_addr=VOICE_ERROR_SWEEP_FIX;
				}
#endif
			/////旋转打滑异常检测////////////
#ifdef 		ROTATE_SKID_CHECK
			if(Rotate_Skid.fail)
				{
					error_code=ERROR_RATATE_SKID;
					voice_addr=VOICE_ERROR_SWEEP_FIX;
					mode.err_code|=WIFI_ERR_OTHER;
				}
#endif
			/////惯导数据检测////////////////
#ifdef 		GYRO_TICK_CHECK
			if(Check_Gyro_Tick())
				{
					error_code=SEND_ERROR_GYRO;
					dis_err_code=DIS_ERROR_GYRO;
					voice_addr=VOICE_ERROR_GYRO;
					mode.err_code|=WIFI_ERR_OTHER;
				}
#endif

#ifdef 		GYRO_PITCH_CHECK
			if(Gyro_Data.pitch_fail)
				{
					error_code=SEND_ERROR_GYRO;
					dis_err_code=DIS_ERROR_GYRO;
					voice_addr=VOICE_ERROR_GYRO;
					mode.err_code|=WIFI_ERR_OTHER;
				}
#endif

#ifdef 		GYRO_ROLL_CHECK
			if(Gyro_Roll_Check())
				{
					error_code=SEND_ERROR_SWEEPER_FIX;
					dis_err_code=DIS_ERROR_SWEEPER_FIX;
#ifdef EFFICENT_DEBUG
					TRACE("Gyro_Roll_Check Fail!\r\n");
#endif
				}
#endif

#ifdef 		GYRO_PITCHROLL_CHECK
			if(Gyro_Data.pitchroll_fail)
				{
					error_code=SEND_ERROR_GYRO;
					dis_err_code=DIS_ERROR_GYRO;
					voice_addr=VOICE_ERROR_GYRO;
					mode.err_code|=WIFI_ERR_OTHER;
				}
#endif

#ifdef 		FREE_FIRST_BAD_CHECK
			Free_First_Check();
#endif

			//////墙地检红外异常检测/////////
#ifdef 		WALL_EARTH_ERROR_CHECK
			data1=Wall_Earth_Check();
			switch(data1)
				{
					case 1:
						error_code=SEND_ERROR_LEFTWALL;
						dis_err_code=DIS_ERROR_LEFTWALL;
					break;
					case 2:
						error_code=SEND_ERROR_MIDWALL;
						dis_err_code=DIS_ERROR_MIDWALL;
					break;
					case 3:
						error_code=SEND_ERROR_RIGHTWALL;
						dis_err_code=DIS_ERROR_RIGHTWALL;
					break;
					case 4:
						error_code=SEND_ERROR_LEFTEARTH;
						dis_err_code=DIS_ERROR_LEFTEARTH;
					break;
					case 5:
						error_code=SEND_ERROR_MIDEARTH;
						dis_err_code=DIS_ERROR_MIDEARTH;
					break;
					case 6:
						error_code=SEND_ERROR_RIGHTEARTH;
						dis_err_code=DIS_ERROR_RIGHTEARTH;
					break;
				}
#endif
			
			//////异常处理///////////////////
			if((dis_err_code==DIS_ERROR_SWEEPER_LIFT)|((dis_err_code<0XF000)&&(dis_err_code>0)))	//异常处理,异常全部进入ERR模式,双轮抬起进入状态需要进入异常模式
				{			
					stop_rap();
					if(voice_addr)
						Send_Voice(voice_addr);
					CHECK_STATUS_FLAG=false;
					Init_Err();
					
				}
		} 
}

void Init_Check_Status(void)
{
	//SLAM_TICK检测初始化
	Slam_Data.tick_check_step=0;

	//风机检测初始化
	Fan.check_step=0;
	Fan.error_time=0;		//qz add V2.3.7
	Fan.length=0;			//qz add 
	Fan.last_length=0;
	Fan.flag=0;
	Fan.step=0;

	//离地检测初始化
	l_lidi.check_step=0;
	r_lidi.check_step=0;
	l_lidi.fail=false;		//qz add 20180522
	r_lidi.fail=false;		//qz add 20180522

	//碰撞检测初始化
	l_bump.check_step=0;
	r_bump.check_step=0;
	lm_bump.check_step=0;
	rm_bump.check_step=0;
	l_bump.fail=false;

	//驱动轮检测初始化
	r_ring.state=GOOD;
	l_ring.state=GOOD;
	r_rap.fail=false;
	l_rap.fail=false;

	//集成盒检测初始化
	dust_box.fail=false;
	dust_box.check_step=0;
	dust_sta.check_step=0;
	dust_sta.fail=false;

	//边刷检测初始化
	SideBrush.fail=0;
	SideBrush.check_step=0;
	SideBrush.error_time=0;
	SideBrush.disc_error_time=0;
	SideBrush.done_time=giv_sys_time;

	//万向轮独立检测初始化
#ifdef FREE_SKID_INDEP_CHECK
	Free_Skid_Indep.fail=false;
	Free_Skid_Indep.check_step=0;
	Free_Skid_Indep.check_start_time=0;
	Free_Skid_Indep.length=w_ring.length;
	Free_Skid_Indep.check_flag=false;
#endif

#ifdef ROTATE_SKID_CHECK
	Rotate_Skid.fail=false;
	Rotate_Skid.resuce_time=0;
	Enable_Rotate_Skid_Check(0);
#endif

	//惯导数据检测初始化
#ifdef GYRO_TICK_CHECK
	Gyro_Data.check_step=0;
#endif

#ifdef WALL_EARTH_ERROR_CHECK
	w_l.error_check_step=0;
	w_lm.error_check_step=0;
	w_m.error_check_step=0;
	w_rm.error_check_step=0;
	w_r.error_check_step=0;
	e_l.error_check_step=0;
	e_m.error_check_step=0;
	e_r.error_check_step=0;
#endif

#ifdef GYRO_ROLL_CHECK
	Gyro_Data.roll_check_step=0;
	Gyro_Data.first_roll=Gyro_Data.roll;		//qz add 20180927

#endif
#ifdef GYRO_PITCH_CHECK
	Gyro_Data.first_pitch=Gyro_Data.pitch;
	Gyro_Data.pitch_fail=false;
	Gyro_Data.pitch_fail_cnt=0;
	Gyro_Data.pitch_check_step=0;
#endif
#ifdef GYRO_PITCHROLL_CHECK
	Gyro_Data.pitchroll_check_step=0;
	Gyro_Data.pitchroll_fail=false;
#endif
	Init_Gyro_Bios();

	error_code=0;	//qz add 20180522
	dis_err_code=0;
	mode.err_code=0;

#ifdef DEBUG_INIT
	TRACE("Check Status init OK!\r\n");
#endif
}

u8 Key_Status_Check_Low(KEY* key)					//对应于低电平有效key
{
	u8 result=0;
	switch (key->check_step)
		{
		case 0:
			if(!key->key)						//如果出现异常,
				{
					key->start_check_time=giv_sys_time;		//设定测试开始时间
					key->check_step++;			//准备进入测试
				}
		break;
		case 1:
			if((!key->key)&(giv_sys_time-key->start_check_time>key->MAX_CHECK_TIME))
				{
					result=1;					//异常时间持续超过设定时间，返回1
					key->check_step=0;			//准备重新进入检测条件
				}
			else if(key->key)					//设定时间内异常恢复
				key->check_step=0;				//重新进入检测
		break;
		}
	return result;
		
}

u8 Key_Status_Check_High(KEY* key)				//对应于高电平有效key
{

	u8 result=0;
	switch (key->check_step)
		{
		case 0:
			if(key->key)						//如果出现异常,
				{
					key->start_check_time=giv_sys_time;		//设定测试开始时间
					key->check_step++;			//准备进入测试
				}
		break;
		case 1:
			if((key->key)&(giv_sys_time-key->start_check_time>key->MAX_CHECK_TIME))
				{
					result=1;					//异常时间持续超过3s，返回1
					key->check_step=0;			//准备重新进入检测条件
				}
			else if(!key->key)					//3s时间内异常恢复
				key->check_step=0;				//重新进入检测
		break;
		}
	return result;
}

