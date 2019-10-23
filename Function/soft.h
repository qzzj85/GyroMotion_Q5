  
#ifndef  __SOFT_H
#define  __SOFT_H
#include "stm32f10x.h"

#define     TUYA_WIFI     // 自己控制程序上面的修改         
#define     TUYA_ADD      // 对tuya原来的sdk做修改 
           // 在tuya的sdk 里面wifi.h 单独也需要定义以上内容 


//#define 	USE_HSE					1
#define		USE_LSE					1
#define 	CPU_FREQ_64MHz 			1
#define 	FLOAT_PID 				1
//#define		FREE_SKID_ACTION		1					//万向轮打滑动作处理
//#define		ROTATE_SKID_ACTION		1				//旋转打滑动作处理
#define		ROTATE_SKID_DEBUG		1					//旋转打滑DEBUG
#define		SKID_REPORT_TIME		1
#define 	HW_INT_REC 				1					//红外中断接收

#define		DEBUG					1
#ifdef 		DEBUG
#define 	QZ_DEBUG_DOCK 			1
#define 	DOCK_DEBUG				1
#define 	PREEN_DEBUG				1
#define		YBS_DEBUG				1
#define		REMOTE_DEBUG			1
#define 	DEBUG_Enter_Mode		1
#define     DEBUG_INIT				1
#define		EFFICENT_DEBUG			1
#define		TEST_DEBUG				1
//#define	DEBUG_SPEED				1
#define		DEBUG_CHARGE			1
#define		DEBUG_POWER				1
#endif

#if 0
//#define	FREE_SKID_CHECK			1					//万向轮径向打滑检测，用于上报导航板
//#define	FREE_SKID_INDEP_CHECK	1					//独立万向轮检测，用于卡住
//#define 	ROTATE_SKID_CHECK		1					//旋转打滑检测
#define		FAN_CHECK				1					//风机检查
//#define	LIFT_CHECK				1					//离地检查
//#define 	SLAM_CHECK				1					//SLAMTICK检查
#define		BUMP_FIX_CHECK			1					//碰撞锁定检测
#define		RING_FIX_CHECK			1					//驱动轮锁定检查
//#define 	RING_OC_CHECK			1					//驱动轮过流检查
#define		MB_OC_CHECK				1					//中扫过电流检查
#define		DUST_BOX_EXIST_CHECK	1					//集尘盒检查
#define		SB_FIX_CHECK			1					//边扫缠绕检查
#define		GYRO_TICK_CHECK			1					//惯导数据检查,5s内未接到收据,判定错误
//#define	WALL_EARTH_ERROR_CHECK	1					//墙地检异常检测(比如没插)
#endif
#define		GYRO_BIOS_CHECK			1
#define		CLIFF_ENABLE			1					//地检悬崖检查

#define 	SLAM_CTL_FAN			1
#define 	YBS_BLACK				1					//沿边时黑色墙体处理
#define		PWRCAP_CAL				1					//电流计电量计算
//#define 	OBSTACLE_CURRENT_CHECK	1					//过障时,电流会增加,过去不处理
#define	 	YBS_AVOID_SEAT			1				//沿边充电座避障
#define		YBS_SEAT_REPROT			1					//延边时发现充电座上报标志
#define		YBS_DIS_RESTORE			1					//沿边固定距离增大以后自动回复
//define	COMMANDER_SEAT_REPORT	1					//命令模式下发现充电座上报标志
#define		SWITCHOFF_SUBMODE		1					//船型开关未打开模式
//#define		SLEEP_SUBMODE			1				//底盘休眠模式
//#define		DOCK_NEAR				1				//充电座近信号
//#define		WALL_DARK_FIX			1				//墙检暗数据固定
#define		WALL_DARK_CAL			1					//墙检暗数据校正,500ms打开一次
#define		DOCK_GOBACK_STRAIGHT	1					//回充首次收到左右信号后直走
#define		YBS_START_RIGHT_ANGLE	1					//沿边起始动作直行1m,右转90度,直行5m
#define		DC_NOBAT_RUN			1					//无电池,DC直插的直行
#define		FREE_SKID_INDEP_ACC		1					//使用直行时间累加的方法判断独立万向轮打滑
#define		NEW_PWR_CAL				1					//使用2600MAH初始化电池及电量计算方法
//#define		FINDSEAT_SKID_CHECK		1					//用于回充时骑上充电座但是又不动的情况
#define		COMMAND_CLIFF_ACTION	1					//新悬崖规避动作
#define		REMOTE					1				//使用遥控器功能
#define		REPEAT_DOCK				1					//再接续回充功能
#define		SHUTDOWN_MODE			1					//关机功能
#define		INTERFERENCE_ACTION		1					//红外信号抗干扰
//#define		UV						1				//UV灯功能
#define 	REYBS2DOCK				1					//小回充信号丢失再次请求沿边
#define		DUMMY_WALL				1					//虚拟墙，沿边模式下能够接受非00速度指令，进入速度模式
//#define		NARROW_PASS				1				//沿边窄通道处理,主要是与左碰撞相关。
#define		GYRO_ROLL_CHECK			1					//惯导俯仰角检测（阈值5度）
#define		FREE_FIRST_BAD_CHECK	1					//万向轮故障检测，用于每次“开始清扫”后的第一次Free_Skid_Indep_Check
//#define 	DS1307					1					//使用DS1307 RTC及后备寄存器
#define  	USE_VOICE_BUSY			1					//使用语音芯片BUSY引脚
#define		USE_BITBAND				1					//GPIO读写使用BITBAND操作
#define		STOP_WEEKUP				1					//可以从STOP模式中唤醒
#define		SPEED_EXTI				1					//速度使用外部中断来计数
//#define		MHW_CAPTURE				1
//#define		GYRO_PITCH_CHECK		1				//惯导俯仰角检测
//#define		GYRO_PITCHROLL_CHECK	1
//#define		PITCH_SPEEDUP			1			
#define		RING_PWM_CTL			1					//轮子PWM切断控制
#define		GYRO_CAL				1				//惯导校准代码
//#define		GYRO_COMPENSATION		1					//惯导角度补偿
//#define		MILE_COMPENSATION		1					//里程计补偿
//#define		STOP_SPD_CNT			1
#define		TEST_ONESTEP			1
#define		EARTH_IN_TIM2			1
//#define		OUT_8MHZ				1
#define		YIS055					1

#define 	MAIN_VERISON 			1
#define 	SUB_VERSION				3
#define		CORRECT_VERSION			7

#define 	PREEN_DATA_ADDR  		0X0807F800			//7组预约时间存储地址，最后一个页
#define		BAT_REINIT_ADDR			0x0807FFFC			//最后一个字节


/////////////上报导航版错误代码//////////////////////
#define		ERROR_DOCK_FAIL			0xF001
#define		ERROR_DC_EXIST			0xF003
#define		ERROR_BIRD				0xE011
#define		ERROR_BIRD_COMM			0xE012
#define		ERROR_VIDEO				0xE021
#define		ERROR_GYRO				0xE031
#define		ERROR_SLAM_TICK			0xE041
#define		ERROR_SLAM_FAIL			0XE042
#define		ERROR_LIFT				0xF003//0xE051
#define		ERROR_DUSTBOX_NONE		0xE001
#define		ERROR_FAN_FIX			0xE102
#define		ERROR_BUMP_FIX			0xE002
#define		ERROR_IR_FIX			0xE121
#define		ERROR_IR_FAIL			0xE122
#define		ERROR_RING_FIX			0xE131
#define		ERROR_RING_OC			0xE132
#define		ERROR_SWEEP_FIX			0xE133
#define		ERROR_MB_OC				0xE141
#define		ERROR_SB_OC				0xE151
#define		ERROR_SB_FIX			0xE152
#define		ERROR_BAT_TEMP			0xE091

////////////显示错误代码//////////////////////
#define		DIS_ERROR_BOX_NONE			0XE000
#define		DIS_ERROR_DANGER			0XE001
#define		DIS_ERROR_BUMP_FIX			0XE002
#define		DIS_ERROR_LEFT_LIFT			0XE004
#define		DIS_ERROR_RIGHT_LIFT		0XE005
#define		DIS_ERROR_LEFTRING_OC		0XE006
#define		DIS_ERROR_RIGHTRING_OC		0XE007
#define		DIS_ERROR_LEFTSB_OC			0XE008
#define		DIS_ERROR_RIGHTSB_OC		0XE009
#define		DIS_ERROR_MB_OC				0XE010
#define		DIS_ERROR_GYRO				0XE011
#define		DIS_ERROR_LEFTWALL			0XE012
#define		DIS_ERROR_MIDWALL			0XE013
#define		DIS_ERROR_RIGHTWALL			0XE014
#define		DIS_ERROR_LEFTEARTH			0XE015
#define		DIS_ERROR_MIDEARTH			0XE016
#define		DIS_ERROR_RIGHTEARTH		0XE017
#define		DIS_ERROR_FAN_OC			0XE018
#define		DIS_ERROR_SLAM_DOWN			0XE019
#define		DIS_ERROR_DIPAN_TICK		0XE020
#define		DIS_ERROR_BAT_DOWN			0XE021
#define		DIS_ERROR_VIDEO_DOWN		0XE022
#define		DIS_ERROR_BIRD_DOWN			0XE023
#define		DIS_ERROR_SWEEPER_FIX		0XE024
#define		DIS_ERROR_SWEEPER_LIFT		0XF001
#define		DIS_ERROR_DOCK_FAIL			0XF002
#define		DIS_ERROR_DC_EXSIT			0XF003


#define		ERROR_RATATE_SKID		0xE200
#define		ERROR_FREE_SKID_INDEP	0xE201

////////////底盘上报导航板请求///////////////
#define 	DIPAN_REQ_NONE			0
#define		DIPAN_REQ_SWEEP			1
#define 	DIPAN_REQ_DOCK			2
#define 	DIPAN_REQ_WIFI			3
#define 	DIPAN_REQ_YBS			4
#define 	DIPAN_REQ_SPOT			5
#define 	DIPAN_REQ_PREEN			6
#define		DIPAN_REQ_TURNOFFSLAM	7
#define		DIPAN_REQ_BURNNING		8
#define		DIPAN_REQ_FORWORD		9
#define		DIPAN_REQ_BACK			0X0A
#define		DIPAN_REQ_LEFT			0X0B
#define		DIPAN_REQ_RIGHT			0X0C
#define		DIPAN_REQ_STOP			0X0D
#define		DIPAN_REQ_SLEEP			0X0E
#define		DIPAN_REQ_DOCKYBS		0X0F	//底盘进入小回充以后又丢失信号，请求再次进入沿边
#define		DIPAN_REQ_FACTORY		0X10

#define		SWEEP_METHOD_GUIHUA		0
#define		SWEEP_METHOD_YBS		1

//QZ:dm=0,return; dm=1,左转; dm=2,右转; dm=3,前进; dm=4,后退; dm=5,左轮不动向左转; dm=6,右轮不动向右转;
//	 dm=7,右轮不动向左转; dm=8,左轮不动向右转 18:顺时针走螺旋形; 19:逆时针走螺旋形; 21 走螺旋线
///////////////////下面是do_action动作码////////////////////////
#define  TURN_LEFT  				1
#define  TURN_RIGHT  				2
#define  FORWARD     				3
#define  BACKWARD    				4
#define  TURN_LEFT_STAY_L_WHEEL   	5
#define  TURN_RIGHT_STAY_R_WHEEL  	6
#define  TURN_LEFT_STAY_R_WHEEL   	7
#define  TURN_RIGHT_STAY_L_WHEEL  	8

#define  HELIX_CLK_WISE           18//螺旋线顺时针
#define  HELIX_ANTI_CLK_WISE      19//螺旋线逆时针

#define  BUMP_WALL_LEFT              7
#define  BUMP_WALL_LEFT_MID          8
#define  BUMP_WALL_MID               9
#define  BUMP_WALL_RIGHT_MID         15
#define  BUMP_WALL_RIGHT             14

////////////下面是PWM驱动脚使用的软定义/////////////////////////
#define   L_FRONT     0x1   //左轮向前
#define   L_BACK      0x2   //左轮向后
#define   R_FRONT     0x3   //右轮向前
#define   R_BACK      0x4   //右轮向后
#define   CHARGE_PWM  0x5   //充电控制


/////////////下面是红外接收的数据定义////////////////////////////
#define  L_CHARGE         	0xe0		
#define  M_CHARGE         	0xb0
#define  R_CHARGE         	0xd0
#define  UP_CHARGE         	0x70
#define  L_UP_CHARGE        0x60 
#define  M_UP_CHARGE        0x30 
#define  R_UP_CHARGE  			0x50
#define  L_M_CHARGE         0xa0
#define  L_R_CHARGE         0xc0
#define  R_M_CHARGE         0x90 
#define  L_M_UP_CHARGE      0x20   
#define  L_R_UP_CHARGE      0x40   
#define  R_M_UP_CHARGE   		0x10
#define  L_M_R_CHARGE 			0x80
#define  L_M_R_UP_CHARGE 		0x00
#define  V_DUMMYWALL        0xdc 
#define  UP__DUMMYWALL      0xec    
#define  V_UP_DUMMYWALL     0xcc

#define  B_HW_MASK			0x0040
#define	 L_HW_MASK			0X0100
#define	 R_HW_MASK			0X0400
    

/////////////下面是遥控器的按键定义//////////////////////////////
#define  YAOKONG_ESC           	10	 //退出、取消、关闭电源
#define  YAOKONG_TIMINE_MON	   	6	   //定时、星期一
#define  YAOKONG_SETTIME_TUE   	9	   //设定时间、星期二
#define  YAOKONG_UV_WED		   		3	   //UV、星期三
#define  YAOKONG_AUTO_THU	   		1	   //定点清扫、星期四
#define  YAOKONG_SPOT_FRI	   		4	   //自动清扫、星期五
#define  YAOKONG_V_SAT		   		8	   //速度设定、星期六
#define  YAOKONG_DOCKING_SUN   	5	   //回充、星期天
#define  YAOKONG_ED			   			2	   //每天
#define  YAOKONG_LEFT		   			12	 //向左
#define  YAOKONG_RIGHT		   		13	 //向右
#define  YAOKONG_FRONT		   		14	 //向前
#define  YAOKONG_BACK		   			11	 //向后
#define  YAOKONG_OK			   			7	   //确定
                         
///////下面是地检和墙检的状态定义///////////////////////////

#define     NEAR            1                              //近
#define     FARN            0                              //远


#define    LEFT             1            	//方向是向左
#define    RIGHT            2            	//方向是向右
#define 	 STOP 			0X03

#define     FRONT           0X01    			// 向前
#define     BACK            0X02 					// 向后

#define     BAD             0X01 					//车轮不能动
#define     GOOD            0X00 					//车轮正常

#define     LI_ANGLE        0X90         //内墙角
#define     WAI_ANGLE       0XA0         //外墙角
#define     U_ANGLE         0XB0         //U型墙角

#define    POWER_DOWN       0X003        //显示电池没电
#define    POWER_TEMP       0x005        //显示电池温度过高
#define    POWER_NTC        0X006        //电池没有NTC
#define    POWER_CHARGE     0X007        //电池正在充电
#define    CHARGE_ERR       0X009        //充电错误

/////////////////工作模式/////////////////////////
#if 0
#define     CEASE            0X00                //待机模式
#define     SWEEP            0X01                //扫地模式
#define     SPOT             0X02                //定点清扫
#define     TELE             0X03                //遥控模式
#define     DOCKING          0X04                //自动回充模式
#define     CHARGEING        0X05				 //充电模式
#define     SLEEP            0X06                //睡眠模式 
#define     ERR              0X07				 //出错模式
#define 	SWEEP_GONG       0X80				 //工型清扫
#endif
//qz add 自有协议
//////////////////////主模式///////////////////////////////
#define CEASE 				0x01
#define	MODE_REMOTE			0X02
#define YBS 				0x03
#define SPOT 				0x06
#define LUOXUAN 			0X06
//#define SLAM_DOCK_MODE	0x04	//qz 20180417:大回充
#define DOCKING 			0X04		//qz 20180416:小回充
#define	SWEEP				0X07		//清扫及清扫子模式
#define	SHIFT				0X08		//转移及转移子模式
#define EXIT				0X09		//退出区域模式
#define CHARGEING 			0x0D
#define SLEEP 				0x0E
#define TEST				0x0F
#define PASS2INIT			0X10
#define ERR 				0xFF

//////////////////////子模式/////////////////////////////////
#define QUIT_CHARGING 		0XFE		//退出充电座子模式，主模式：CEASE
#define	SELF_TEST			0xFD		//自测子模式,主模式：CEASE
#define	BURNIN_TEST			0xFC		//老化子模式,主模式：TEST
#define	DEAD				0xFB		//低功耗子模式,主模式：CEASE
#define	SWITCHOFF			0XFA		//开关未打开子模式,主模式：CHARGE
#define	YBS_SUB_RIGHT		0XF9
#define	YBS_SUB_LEFT		0XF8
#define	DC_CHARGING			0XF7		//DC直充充电模式，主模式：CHARGING
#define	SEAT_CHARGING		0XF6		//充电座充电模式, 主模式: CHARGING
#define SHUTDOWN			0XF5		//关机模式，主模式：CEASE
#define FACTORY_TEST		0XF4		//厂测子模式,主模式：CEASE
#define	YBS_SUB_EXCHANGE 	0XF3

#define	NORMAL_SWEEP		0XF3
#define	PASS2SWEEP			0XF2
#define	BACK2YBSSTART		0XF1
#define	BACK_SWEEP			0XF0
#define	STOP_BACKSWEEP 		0XEF
#define	SWEEP_FIRST_INIT	0XED

#define SHIFTPOINT1			0XD0
#define	SHIFTPOINT2			0XD1
#define LEAK_SWEEP			0XD2
#define PASS2LEAKSWEEP  	0XD3
#define SHIFT_LEFT_YBS  	0XD4
#define SHIFT_RIGHT_YBS 	0XD5
#define	LEAK_BACKSWEEP		0XD6
#define	STOP_LEAKBACK		0XD7

#define	RUN_TEST			0xC0

#define	SUBMODE_REMOTE_MOVE			0XB0
//#define     COMMANDER_x10           0X10         //  指令模式

//#define     YBS           0X20    	     //  沿墙模式
//#define     YBS_YBS_x21           0X21        
#define     YBS_YBS_x22_FAST      0X22
#define     YBS_YBS_x23_FAST      0X23

////////////////下面是速度的定义//////////////////
#define     HIGHSPEED      4000//2400//1200  			//	  2400       //	高速的速度
#define     LOWSPEED       2000//1200//600  			//	1200          //	低速的速度

#define			MaPan_XiFen				2				//码盘细分数量   1 或者 2

//李工的样机
//轮子直径70mm
//传动比 81
//脉冲数 20（转一圈20个脉冲）
//左右轮的距离235mm
//马达转速：12000±10%rpm


///////*************************************************************
//////左右轮直径：68mm	

//////-------->>>>>>>>轮子的周长是:3.14*68 = 213.52mm		//zdk

//////传动比  61.6
//////脉冲数  16
//////一个脉冲的长度是68*3.14/61.6/16=0.2167mm
//////两轮距离  267.6mm。

//////-------->>>>>>>>自转一圈每个轮子走的长度:3.14*267.6 = 840.264mm		//zdk

//////毛刷   左边82mm  右边  75mm ；  平均毛刷150mm


//////经测，轮子转一周的r_ring.length = 0x7D0 = 2000		//zdk
//////所以，一米的距离的脉冲数就是: 1000mm/213.52mm  * 2000 = 4.6834 * 2000 = 9366.8	//zdk
//////所以，一个脉冲的长度就是213.52 / 2000 = 0.10676mm		//zdk
//////所以，机器转动一圈每个轮子所走的脉冲数是:840.264 / 0.10676 = 7870.588 约等于7871 //zdk


////////QZ ADD,凤瑞样机
//////一个脉冲的长度是68*3.141592/61.6/16/2=0.108374mm
//////两轮距离267.6mm
//////原地旋转自传一圈每个轮子走的长度：3.141592*267.6=840.690019mm
//////所需脉冲：840.690019/0.108374=7757.303587
//////转90度脉冲：1939.325897
//////转180度脉冲：3878.651793

//////以一点为圆心自转一圈每个轮子走的长度：267.6*2*3.141592=1681.380038mm
//////所需脉冲：1681.380038/0.108374=15514.607175
//////转90度脉冲：3878.651793

////////QZ ADD END
//////*************************************************************/


//hzh
#define    PULSES_PER_MM    14.74
#define    METE             (int)PULSES_PER_MM*1000+750        //1米的脉冲数   调好了            

#define    METE_0_1         (int)PULSES_PER_MM*110   //已调好
#define    METE_0_2         (int)PULSES_PER_MM*220   //已调好
#define    METE_0_3         (int)PULSES_PER_MM*323   //已调好
#define    METE_0_4         (int)PULSES_PER_MM*432   //已调好
#define    METE_0_5         (int)PULSES_PER_MM*530   //已调好 
//其他数据看做下去有需要再调，在c文件中修改宏，否则修改h文件会全部重编译


/************************************************************************************/
//hzh 
#define  ANGLE_5      (int)PULSES_PER_MM * 2.0497 *9    //3.14*235/360   已调好  
#define  ANGLE_10     (int)PULSES_PER_MM * 2.0497 *14   //已调好
#define  ANGLE_15     (int)PULSES_PER_MM * 2.0497 *19   //已调好 
#define  ANGLE_30     (int)PULSES_PER_MM * 2.0497 *34   //hzh   已调好
#define  ANGLE_45     (int)PULSES_PER_MM * 2.0497 *50   //hzh   已调好   
#define  ANGLE_60     (int)PULSES_PER_MM * 2.0497 *66   //hzh   已调好
#define  ANGLE_90     (int)PULSES_PER_MM * 2.0497 *94   //hzh   已调好
#define  ANGLE_180    (int)PULSES_PER_MM * 2.0497 *190  //hzh   已调好
#define  ANGLE_360    (int)PULSES_PER_MM * 2.0497 *379  //hzh  已调好                
#define  ANGLE_720    (int)PULSES_PER_MM * 2.0497 *763  //hzh  已调好
/************************************************************************************/  




#define     SPEED           1000        // 定义为车轮的最大速度   
#define     FARAWAY         0XFFFFFFFF   //定义为无穷远
#define     LENGTH		    243			 //车轮的宽度  单位是毫米

////////////#define     METE            4615         //1米的脉冲数
////////////#define     METE_H5         2308         //0.5米的距离
////////////#define     METE_H6         2769         //0.6米的距离
////////////#define     METE_H7         3231         //0.7米的距离
////////////#define     METE_H8         3692         //0.8米的距离
////////////#define     METE_H9         4154         //0.9米的距离
////////////#define     METE_TWO		5130         //2米的脉冲数
////////////#define     METE_0			5592		 //1.1米的脉冲数
////////////#define     METE_1          6053         //1.2米的脉冲数	
////////////#define     METE_2			6515		 //1.3米的脉冲数
////////////#define     METE_3			6976		 //1.4米的脉冲数
////////////#define     METE_4			7438		 //1.5米的脉冲数
////////////#define     METE_5			7899		 //1.6米的脉冲数
////////////#define     METE_6			8361		 //1.7米的脉冲数
////////////#define     METE_7			8822		 //1.8米的脉冲数
////////////#define     METE_8			9384		 //1.9米的脉冲数
////////////#define     METE_THREE		13845		 //3米的脉冲数

#define F1  38




//#define     METE            4615*MaPan_XiFen *F1/50        //1米的脉冲数
#define     METE_H5         2308*MaPan_XiFen *F1/50         //0.5米的距离
#define     METE_H6         2769*MaPan_XiFen  *F1/50         //0.6米的距离
#define     METE_H7         3231*MaPan_XiFen   *F1/50        //0.7米的距离
#define     METE_H8         3692*MaPan_XiFen    *F1/50       //0.8米的距离
#define     METE_H9         4154*MaPan_XiFen    *F1/50       //0.9米的距离
#define     METE_TWO				5130*MaPan_XiFen   *F1/50         //2米的脉冲数
#define     METE_0					5592*MaPan_XiFen *F1/50  		 //1.1米的脉冲数
#define     METE_1          6053*MaPan_XiFen     *F1/50       //1.2米的脉冲数	
#define     METE_2					6515*MaPan_XiFen	 *F1/50  	 //1.3米的脉冲数
#define     METE_3					6976*MaPan_XiFen	 *F1/50  	 //1.4米的脉冲数
#define     METE_4					7438*MaPan_XiFen	 *F1/50  	 //1.5米的脉冲数
#define     METE_5					7899*MaPan_XiFen	 *F1/50  	 //1.6米的脉冲数
#define     METE_6					8361*MaPan_XiFen	 *F1/50  	 //1.7米的脉冲数
#define     METE_7					8822*MaPan_XiFen	 *F1/50  	 //1.8米的脉冲数
#define     METE_8					9384*MaPan_XiFen	 *F1/50  	 //1.9米的脉冲数
#define     METE_THREE			13845*MaPan_XiFen	 *F1/50  	 //3米的脉冲数

#define     METE_SIX		METE_THREE*2	 *F1/50  	 //6米的脉冲数



////////////////////////////////////定义PID算法的系数///////////
#define     KP              40//20     //8
#define     KI              10    //30
#define     KD              80    //150

#define 	F_KP			0.02//0.1//0.5//0.02//0.025
#define		F_KI			0.1	//1.5	//0.1
#define		F_KD			0.0125	//0.2//0.0125//0.0125

#define		D_KP			0.01
#define		D_KI			0.05
#define		D_KD			0.0125

/*************************************************************
左右轮直径：68mm
传动比  61.6
脉冲数  16  加倍
一个脉冲的长度是68*3.14/61.6/32=0.10831980mm。
每毫米脉冲数量：9.23192206819
每米脉冲数量：9231.92206819
两轮距离  267.6mm。
毛刷   左边82mm  右边  75mm ；  平均毛刷150mm

*************************************************************/

/************************************************************
QZ ADD
李工的样机
轮子直径70mm
传动比 81
脉冲数 20（转一圈20个脉冲）
左右轮的距离235mm
马达转速：12000±10%rpm

轮子转动一圈脉冲数：81*20*2=3240
一个脉冲长度：70*3.141592/3240=0.067874mm
行走1mm脉冲：14.733182
行走1cm脉冲：147.331821
行走1M脉冲：14733.182073
两轮间距：235mm
旋转1度脉冲：235*3.141592/360/0.067874=30.214242
旋转1周脉冲：235*3.141592/0.067874=10877.12703
旋转45度脉冲：235*3.141592/360*45/0.067874=1359.64089~~1360
旋转180度脉冲:30.214242*180=5438.56356~~5440

经过实测：左右轮最大速度在350mm/s左右，设置最大速度为300mm/s，即4420个脉冲
***************************************************************/

/*************************************************************
QZ
周工样机
左右轮直径：68mm
传动比:57.23
码盘：16

轮子转一圈的脉冲数：16*57.23*2=1831.36
一个脉冲的长度：68*3.141592/1831.36=0.11665mm
行走1mm脉冲：8.572653
行走1cm脉冲85.726532
行走1M：8572.653236
两轮间距：220mm
旋转1度脉冲：220*3.141592/360/0.11665=16.458309
***************************************************************/

/*************************************************************
QZ
Q5样机
左右轮周长：194.68mm
传动比:49.9
轮厚：14.8
码盘：8
轮子转一圈的脉冲数：8*49.9*2=798.4
一个脉冲的长度：194.68/798.4=0.243838mm
行走1mm脉冲：4.1010835
行走1cm脉冲：41.010835
行走1M：4101.083506
两轮间距：216mm
旋转1度脉冲：216*3.141592/360/0.243838=7.730359
摩擦系数：5.9461	根据k=(speed^2)/(2gl),g为质量，l为刹车长度，speed为速度
机器质量：2300g
根据速度计算摩擦距离：l=speed^2/6.11413/2/2365，所以速度
***************************************************************/

#define   DIS_XiShu 	1.0255
#define   XISHU_1		1.133787	//QZ ADD

#define  F2  44
//#define   MM_PLUS  			9.23192206819 * DIS_XiShu * F2/50 * XISHU_1	//QZ ADD XISHU_1	9.44586946440 原来为8.3312557512
//#define   CM_PLUS  			92.3192206819 * DIS_XiShu * F2/50 * XISHU_1	//QZ ADD XISHU_1
//#define   METER_PLUS  		9231.92206819 * DIS_XiShu * F2/50 * XISHU_1	//QZ ADD XISHU_1
#define PULSE_LENGTH	0.243838//0.1219188		//QZ ADD：计算速度时，脉冲数无需除2
#define MM_PLUS 		4.1010835//8.202180		//QZ ADD
#define CM_PLUS 		41.010835//82.021805		//QZ ADD
#define METER_PLUS 		4101.083506//8202.180468	//QZ ADD
#define MAX_SPEED 		1600 			//qz 定义最大速度,约305mm/s
#define	RING_RANGE		216				//两轮间距216mm
#define	LENGTH_CAL		6858		//速度的平方直接除以LENGTH_CAL，为矫正长度。LENGTH_CAL=5.9461*2*2365*PULSE_LENGTH

#define 	Angle_1 		7.730359//15.460746
#define   	angle60  		Angle_1*60
#define   	angle75  		Angle_1*75
#define   	angle90			Angle_1*90
#define   	angle90_D		Angle_1*180
#define   	angle180		Angle_1*180
#define   	angle360      	Angle_1*360				//QZ ADD:李工样机
#define   	angle720  		angle360*2			//qz add:

#define		SPEED250		1000	//250mm/s
#define		SPEED200		820		//200mm/s
#define		SPEED150		600		//150mm/s
#define		SPEED100		400		//100ms/s
#define		SPEED50			200		//50mm/s

#define		TOP_MOVE_SPEED		SPEED250
#define		FAST_MOVE_SPEED		SPEED200
#define		HIGH_MOVE_SPEED		SPEED150
#define		MID_MOVE_SPEED		SPEED100
#define		LOW_MOVE_SPEED		SPEED50

#define		REVOLUTION_SPEED_HIGH	1000
#define		REVOLUTION_SPEED_LOW	400


typedef struct
{
//	 char	ComTxBuf[256];	  //通信缓冲区
//	 char	ComRxBuf[500];	  //通信缓冲区
	u32	RevLength;			//DMA接收数据长度		
	u32	TxdDataNum;			//发送数据个数	u32	TxdDataNum;		//发送数据个数

	volatile bool	Trans_Busy;		//通信发送忙 0=不忙 1=忙
	unsigned short	TimeOut_Flag;	//通信超时时间
	unsigned short	TimeOut;			//通信超时时间
	u32  	Com_State;		//通信内容处理标识  0=无通信处理  5=需要发送扫描(SCAN)命令 6=等待扫描(SCAN)命令反馈 0xff=通信处理识别
	bool Status;    //QZ
	volatile bool Rece_Busy;	//QZ
	volatile bool Rece_Done;	//QZ
}UART;

typedef struct 			  //按键的结构
{
     unsigned char key;		 //键值  ，当前状态
	 unsigned char name;
     unsigned char number;     //去抖次数
     unsigned char (*read_start)(); //读取得状态函数
     unsigned char sign;	 //标志  表示该按键有效，没有被处理
		 unsigned char flag; 
	u32 start_check_time;	//开始检测时间
	u32 MAX_CHECK_TIME;
	u8 check_step;
	bool fail;				//qz add 20180522
	bool press;				//qz add 20180801
	bool long_press;		//qz add 20180605 3s
	bool long_long_press;	//qz add 20180605 6s
}KEY;

typedef struct 			  //遥控器
{
    unsigned int effect_time;   //按键时间
	unsigned char old_key;        //旧按键
    unsigned char effect;         //按键有效时间
    unsigned char key;		      //键值
    unsigned char sign;	          //有按键 
	unsigned char test;			  //测试的遥控器标志  0表示自己的遥控器；1表示信号是由编码为FFFF的遥控器发出的信号
}YAOKONG;

typedef struct 			  //光敏电阻值的结构
{
 bool uv;		 
 unsigned char work_mode;	 
 unsigned char speed;
}WORK_MODE;
typedef struct
{
 unsigned int time;//采样垃圾检测的时间    
 unsigned char  odds;  //判断垃圾的标准
 unsigned int samp;  //在一段时间内垃圾脉冲的个数。
 unsigned char  step;  //垃圾检测的过程步骤
 unsigned char  sign;  //垃圾检测的标志  0表示垃圾少，1表示垃圾多
}MUCK;

typedef struct             //墙检地检的数据结构
{
  unsigned short brightness;	 //亮发射管时采样的值
  unsigned short darkness;       //暗发射管时采样的值
  unsigned char  near;	     //近地次数
  unsigned char  farn;	         //远地次数
  unsigned short difference   ;  //测量的反射值
  unsigned int low_door;       //门限值	 
  unsigned int high_door;       //门限值
  unsigned char  sign;	         //本灯的标志
  unsigned char  high_sign;	         //本灯的标志	
  unsigned char  high_near;	     //近地次数
  unsigned char  high_farn;	         //远地次数
  unsigned char  on;             //增加墙检有效标志。0为无效，1为有效
  u16	dif[8];	
  u8 name;	//QZ ADD;
  bool flag;
  u8 max_dis;
  u8 dis;

  unsigned short bias;
  u8 check_time;			//qz add 20180504
  u8  error_check_step;
  u32 error_check_time;
}WALL_DATA;

typedef enum
{
	RECE_NONE=0,
	RECE_LEFT,
	RECE_MID,
	RECE_RIGHT,
	RECE_BACK,
}RECE_IR;

typedef struct             //墙检地检的数据结构	//红外数据结构?  QZ
{
	unsigned 		char  	(*read_io)();
	unsigned 		char   	data;	   //红外接收的数据
	unsigned 		int  		time;	   //接收开始的时间
	unsigned 		char   	tempdata; //临时接收的数据
	unsigned 		char   	tempbit ;	   //采样的当前位的值
	unsigned 		char   	bitnumber;	   //红外接收位
	unsigned 		char   	start;  //红外信号数据开始标志
	volatile    unsigned int   	sign;	   //红外接收到的标志
	volatile    unsigned int  	effect_time;  //红外有效时间
	volatile    unsigned int  	effect;       //红外有效

	//hzh
	//其实sign并没有用到
	volatile	  unsigned int	  effectTop;
	volatile	unsigned int	  signTop;
	volatile	unsigned int	  effect_timeTop;

	volatile	unsigned int	effectSignal;//用于前接收头
	volatile	unsigned int	  effect_timeSignal;
	unsigned	char			Cnt_fallRise;

	volatile	  unsigned int	  effectLeft;
	volatile	  unsigned int	  effect_timeLeft;


	volatile	  unsigned int	  effectRight;
	volatile	  unsigned int	  effect_timeRight;

	volatile	  unsigned int	  effectMid;
	volatile	  unsigned int	  effect_timeMid;

	volatile	  unsigned int	  effectTopReal;
	volatile	  unsigned int	  effect_timeTopReal;

	volatile	  unsigned int	  effectNear;
	volatile	  unsigned int	  effect_timeNear;

	volatile	  unsigned int    effectMidLeft;
	volatile    unsigned int    effect_timeMidLeft;

	volatile    unsigned int    effectMidRight;
	volatile    unsigned int    effect_timeMidRight;

	u16 flag;

	RECE_IR	rece_ir;		//qz add 20180817

	u8 rece_cnt;
}INFR_DATA;
typedef struct             //车轮速度的数据结构
{ 
  int  ek[3];       //最近的三次系统速度偏差
  float	f_ek[3];
  volatile    unsigned int   length;	   //车轮所走的脉冲数
//  unsigned int	 length;
 unsigned int speed_length;//qz add 20180203
 unsigned int last_speed_length;//qz add 20180203
 unsigned int real_speed;  //qz add 20180203
  volatile    unsigned int   dq_time;       //轮子当前时间，在下降沿的时间


	int all_length_Bufer[4];
	volatile 		int all_length;
//  volatile    unsigned short   dq_captime;    //轮子捕获的定时器计数，在下降沿的时间    
//  unsigned int falling_time;             //当前测到的下降沿时间
//  unsigned short falling_captime;          //当前测到的下降沿时间
  unsigned int rising_time;              //当前测到的上升沿时间
//  unsigned short rising_captime;           //当前测到的上升沿时间
//  unsigned int rising_dqtime;            //确定测到的上升沿时间
//  unsigned short rising_dqcaptime;         //确定测到的上升沿时间
  unsigned int  old_time;               //调整周期前的时间
//  unsigned short  old_captime;
  unsigned char  state;       //车轮状态    BAD;车轮不动   GOOD车轮正常
  unsigned int  odds;        //车轮停下转动的时间次数

  u8 check_step;
  int stop_buf[10];
  int stop_spd;
  int cal_length;
}RING;

typedef struct	  //车轮的速度信息
{
unsigned char master;     //主动轮标志   ；1为主动轮，0为从动轮
unsigned char old_time;  //测速的起始时间
unsigned char ori   ;    //车轮的前后方向  
//unsigned char real_ori;	
//unsigned short pwm   ;    // 车轮的pwm值	//qz mask
int pwm;		//qz add
int rap  ;     //车轮的速度，表示为每秒的脉冲数
int rap_run;
unsigned int length  ;     //车轮应当走过的距离
unsigned int all_pulse;
unsigned int  time   ;     // 测量速度的时间
unsigned int  pulse  ;     // 测量速度用的脉冲数
int real_speed;		//轮子实时速度
unsigned char  sign   ;     //标志	   0:车轮停止   1：车轮启动，方式为到点停车     2，阶段到点    3，车轮启动，到点不停
	
	
uint8_t  			State;//0x00:  0x05:半径转弯  0x10:加速  0x20:匀速前进  0x30:减速  0x60:  0x70:  0x80
//float					AccDec;
//float			Desired_Speed;
//float			Max_Speed;

short			AccDec;
short			Desired_Speed;
u16				Max_Speed;

unsigned int 	ACC_Position;	
unsigned int 	DEC_Position;	
	
bool fail;	
	
}RAP	 ;

//QZ ADD
typedef struct
{
	unsigned int length_w;			//行走宽度
	unsigned int length_w_rest;
	unsigned int length_h;			//行走高度
	unsigned int length_h_rest;
	unsigned char time;				//动作次数
	unsigned int angle;				//机器角度
	unsigned char bump_time;		//碰撞次数
	unsigned char turn_step;		//转弯步骤	
	unsigned char status;			//0:表示正在直线,1:表示正在转弯
	bool turn_dir;			//将要转弯方向,0:表示将要右转,1:表示将要左转
	u8 first_time_value;
	u32 temp_length;
	u32 real_length;
}Gong;

typedef struct
{
	bool island;			//孤岛
	bool check_flag;
	
	u8 check_step;
	s8 result;
	s8 island_gridx;
	s8 island_gridy;
	
	short check_angle;
	int acc_angle;
}GYRO_BIOS;
//QZ ADD END

typedef struct
{
	short pwm_l; //左轮PWM
	short pwm_r;	//右轮PWM
	int length_l;		//左轮需要行走的里程数
	int length_r;		//右轮需要行走的里程数
	unsigned int speed;
	unsigned char mode;		//轮子任务的模式   1速度行驶  2 里程行驶
	unsigned char sign;
}MOTO_TASK;

typedef  struct
{
bool low2dock;				 //电池电量低的回充请求，发送一次
bool reinit;				//重新初始化标志,true:需要重新初始化
unsigned int temp   ;	     //电池温度 
unsigned int voltage;		 //电池电压   
unsigned int current;      //充电电流
unsigned char  charge_dc ;	     //外部DC座输入信号
unsigned char  charge_seat ;		 //外部充电座输入信号
 short pwm;         //充电PWM脉冲
unsigned char  err;         //电池错误  0 没有错误，1没有NTC，2 NTC短路，3 没有电池， 4 电池短路
unsigned char step;         //充电步骤  0 小电流  1 大电流  2维持电流
unsigned int time;        //充电开始时间
u32 step_time;			  //qz add
} POWER;

typedef  struct
{
unsigned char data;          //开关信号的数据		  0表示没有动作，1表示其已经动作
unsigned int time;         //读取开关信号的时间
}  SWITCH;

typedef  struct
{
unsigned int current;	       //当前采样的电流值
unsigned int old_current;    //前一次的采样电流
unsigned int sum_current;    //电流的和
unsigned int linshi_current; //临时存放的采样电流值
unsigned int temp;	       //电流的标准值
unsigned int time;		   //采样时间
unsigned int typo;		   //采样超标次数
unsigned char  sign;		   //电流标志         0表示未超标，1表示超标
unsigned char sign_l;
}CURRENT;

typedef  struct
{
unsigned int * music;	    //需要播放的音乐段
unsigned int temp;	    //电流的标准值
unsigned int time;		//音乐播放的时间
unsigned int typo;		//音乐播放的起始位置
volatile  unsigned char  sign;		//信号标志         0表示不唱，1表示唱
}mySOUND;

typedef struct	    //机器坐标参数
{
unsigned int x;		    //机器的x坐标
unsigned int y;			//机器的y坐标
unsigned int angle;		//机器的角度
}POSITION;


typedef struct		//机器系统的工作状态
{
	bool 		speed_up;
	bool 		factory;				//厂测状态qz add 20181024
	bool 		factory_burnning;		//厂测跑机状态qz add 20181024
	bool 		bump_flag; 				   //qz add for bleamn

	u8 			factory_tst_item;		//qz add 20181107
	u8 		 	mode ;	   //机器的运行模式  0：静止模式；1：扫地模式；2：自动回充模式；3；遥控模式	4:出错模式
	u8  		sub_mode;	//子模式，0x01:cease,0xfe:quit_charge,0xff：err
	u8  		step;		//路径执行的步骤
	u8 			step_abn;    //异常时的步骤
	u8			step_bp;	//碰撞时的步骤标志
	u8			step_mk;	//多垃圾清扫步骤
	u8	 		last_bump;
	u8 			Info_Abort;			//qz add中止接收指令，比如碰撞后退或退出充电座时,20180919修正为只速度屏蔽
	u8 			All_Info_Abort;		//qz add 20180919，屏蔽所有指令
	u8 			last_mode;			//qz add上一次的模式
	u8 			last_sub_mode;
	u8 			test_step;	
	u8 			sweep_method;
	
	u8 			status;		//qz add 20180422，扫地机状态，SLAM下发“开始清扫后置1,停止清扫后清0，进入异常状态后，也会清0”
                //底盘会用这个status来判断预约时间到了以后，需不需要打开预约灯
                /*status,0:停止中,机器处于非工作状态,
                         1:规划打扫
                         2:沿边打扫
                         3:区域打扫
                         4:重点打扫

                         5:预约打扫
                         6:回充*/
	u32 		action;		//动作   0:停止  1:原地左转  2原地右转 3前进   4后退   5旋转1  6：旋转2  。。。18旋转14	，19走螺旋线	
	u32 		time;       //动作的起始时间
	u32 		init_mode_time;	//qz add 20180814
	u32  		abnormity;  //异常号码  0表示没有异常 //QZ:16:中扫电流过大,19:左轮不动,20:右轮不动,22:左轮离地,23:右轮离地,31:右轮电流,32:左轮电流
	u32 		bump ;      //碰撞的标志 //QZ:1:左地检,2:左中地检,3:右中地检,4:右地检,5:左碰撞,6:右碰撞,7:左墙检靠近墙,
 									//8:左中墙检靠近墙,9:中墙检,10:顶红外或虚拟,14:右墙检,15:右中墙检,90:双碰撞,

	u32 		last_outbump;
	u32 		bump_time;
	u32 		abn_time;			//qz add 20181011

}MODE;

typedef struct
{
  unsigned char step;	  //步骤		//0 没有正在执行的步骤，1，正在执行某动作
  unsigned char step_tmp;
  unsigned char sign;	  //开始标志   0表示没有开始，1表示已经开始
  unsigned int time ;   //动作开始的时间
  unsigned int length ;  //长度或者所转圈数
  u8 action_flag;		//1:前进,0:停止,2:转弯或其他		//qz add 20180123
  u32 skid_time;		//打滑检测开始时间   qz add 20180123
}ACTION;

typedef	 union		 //显示数据
{
 unsigned int data;
 unsigned char  dis_data[4];
}DIS_DATA;

typedef struct
{
  unsigned char sign;
  unsigned char step;
volatile  unsigned char start;
  unsigned int time;
} DUST;

typedef struct					//坐标格信息结构体
{
	bool clean;					//该坐标是否清扫标志
	bool wall;					//该坐标是否墙壁，碰撞标志
	short clr_xpos;
	short clr_ypos;
	u32  cnt;
}COORDINATE;

typedef struct 					//清扫结构体
{
	bool 	leakon;				//机器当前是NORAML清扫还是LEAK清扫，false:正常清扫，true:漏扫清扫
	bool 	area_ok;				//当前区域清扫检查是否完成，如果清扫检查完毕，以后不再检查漏扫
	bool 	ymax_ok;						//YMAX新区域检查完成标志
	bool 	ymin_ok;						//YMIN新区域检查完成标志
	bool 	xmax_ok;						//XMAX新区域检查完成标志
	bool 	xmin_ok;						//XMIN新区域检查完成标志
	bool 	repeat_sweep;			//重复扫标志，true:重复扫，false：无需要，沿边出现空旷区域时重复扫
	bool 	pathpoint_ok;
	bool	start_seat;
	bool	force_dock;
	
	u8 		back_sweep;				//回扫标志，true:回扫,false:正常扫
	u8 		leftright;				//左右沿边标志，0：左沿边，1：右沿边
	u8 		y_acc;					//正常清扫时Y轴增长方向，0：负方向，1：正方向，2：双方向
	u8 		x_acc;					//正常清扫时X轴增长方向，0：负方向，1：正方向，2：双方向
	s8 		y_dir;					//Y轴清扫时的增长方向(0:负方向,1:正方向),因为y_acc有0,1,2三个值，2代表双方向清扫，无法再用y_acc表示目前正在清扫的方向
	u8  	continue_checkstep;		//沿边时检测是否绕过障碍的步骤
	u8 		sweep_time;				//区域打扫次数，用于新开辟区域时的编号增加，从#1开始
	u8 		area_num;				//当前区域编号
	u8 		exit_area_num;			//退出区域编号
	s8		first_leak_y;

	short 	xpos_ybs_start;		//弓形直线行走切换为沿边时起始点x坐标
	short 	xpos_start;			//起始点x坐标
	short 	ypos_ybs_start;		//弓形直线行走切换为沿边时起始点y坐标
	short 	ypos_start;			//起始点y坐标
	short 	tgt_yaw;				//直线清扫时目标角度
	short 	anti_tgt_yaw;			//直线清扫时目标反角度
	short 	xpos_abort;			//被打断(如回扫)时的x坐标
	short 	ypos_abort;			//被打断(如回扫)时的y坐标
	short 	tgtyaw_abort;
	short 	anti_tgtyaw_abort;
	short 	xpos_start_area;		//区域打扫（4X4）X起始坐标
	short 	ypos_start_area;		//区域打扫（4X4）Y起始坐标
	short 	xpos_max_area;		//区域打扫（4X4）能到达的最大X坐标
	short 	xpos_min_area;		//区域打扫（4X4）能到达的最小X坐标
	short 	ypos_max_area;		//区域打扫（4X4）能到达的最大Y坐标
	short 	ypos_min_area;		//区域打扫（4X4）能到达的最小Y坐标
	
	u32 	worktime;			//区域内工作时间
	u32 	worktime_max;		//区域允许最大工作时间	
}MOTION;

typedef struct					//坐标格坐标结构体
{
	volatile s8 x;						//当前X轴坐标
	volatile s8 y;						//当前Y轴坐标
	s8 x_abort;
	s8 y_abort;
	s8 y_back_last;				//结束回扫时的Y坐标
	s8 x_area_max;				//区域内X能达到的最大坐标格
	s8 x_area_min;				//区域内X能达到的最小坐标格
	s8 y_area_max;				//区域内Y能达到的最大坐标格
	s8 y_area_min;				//区域内Y能达到的最小坐标格
	s8 x_straight_start;		//每次开始直行清扫时的X坐标
	s8 y_straight_start;		//每次开始直行清扫时的Y坐标
	s8 x_area_start;			//区域打扫时起始X坐标	
	s8 y_area_start;			//区域打扫时起始Y坐标
	s8 x_ybs_start;
	s8 y_ybs_start;
	s8 y_start;
	s8 x_start;
}GRID;

typedef struct					//补扫检查信息结构体
{
	bool use_pathpoint;			//使用PATHPOINT走行方式
	bool yacc_anti;				//在motion1.interval_pos的前提下，当前漏扫时需要清扫的间隔方向,false:正常清扫方向，true:回扫方向
	u8 ydir;				//下一漏扫区域的Y轴清扫方向	0:Y轴负方向清扫，1:Y轴正方向清扫
	u8 next_yacc;				//新增清扫区域的Y轴增长方向
	u8 next_xacc;				//新增清扫区域的X轴增长方向
	u8 next_action;				//下一次的动作：0:正常清扫；1:漏扫清扫；2:去新增区域；3:返回入口点
	short next_tgtyaw;			//新增清扫区域的起始角度
	
	s8 max_cleanx1;				//检查点X轴方向最大清扫坐标
	s8 min_cleanx1;				//检查点X轴方向最小清扫坐标
	s8 max_cleanx2;				//相邻检查点X轴方向最大清扫坐标
	s8 min_cleanx2;				//相邻检查点X轴方向最小清扫坐标
	s8 y1;						//检查点Y轴坐标
	s8 y2;						//相邻检查点Y轴坐标
	s8 new_x1;					//可进入补扫点X坐标
	s8 new_y1;					//可进入补扫点Y坐标
	s8 new_x2;					//补扫点X坐标
	s8 new_y2;					//补扫点Y坐标

	u8 ybs_dir;					//可以预先判定的沿边方向，0:没有预先判定方向，1:左沿边，2:右沿边
	s8 backup_grid;				//可以进入的备份点
	u8 new_area_dir;			//下一区域next_area的方向，使用ymax/ymin/xmax/xmin来表示新区域相对于当前区域的方向	
} CHECK_POINT;

typedef struct 					//包含点坐标的结构体
{
	s8 gridx;					//当前点X坐标
	s8 gridy;					//当前点Y坐标
	u8 data;					//数据
	u8 area_dir;				//所在区域的方位（比如左上，右下等）
}POINT_GRID;

typedef struct					//包含路径分析起点和终点坐标的结构体
{
	POINT_GRID start_grid;		//起点坐标
	POINT_GRID tgt_grid;		//终点坐标
}PATH_STARTEND;

struct PATH_POINT				//用于路径分析的每个行走点信息
{
	POINT_GRID path_grid;		//行走点坐标信息
	struct PATH_POINT *last_point;	//上一个行走点指针
	struct PATH_POINT *next_point;	//下一个行走点指针
};


typedef struct
{
	bool first_rst;			//qz add 20181101:如果第一次检测到惯导数据异常，尝试复位1次
	u8 check_step;

	u8 roll_check_step;		//qz add 20180927
	u32 roll_check_time;	//qz add 20180927

	u8 pitch_check_step;
	u32 pitch_check_time;

	u8 pitchroll_check_step;
	u32 pitchroll_check_time;

	u32 start_check_time;
	u32 count_times;
	volatile short x_pos;		//qz modify 20190307 :add volatile
	volatile short y_pos;
	volatile short yaw;
	short tgt_yaw;
	volatile short pitch;
	volatile short roll;
	short first_roll;
	short first_pitch;
	volatile short x_acc;
	volatile short y_acc;
	volatile short z_acc;
	int g_ek[3];		//qz add 20180111
	u8  straight;		//qz add 20180111  0:无任何意义,1:需要做直线行驶的补偿,2:需要做旋转角度的判定
}GYRO_DATA;

extern bool log_show;


typedef struct
{
	u8 		Preen_Week_Num;
	u8 		Preen_Hour;
	u8 		Preen_Min;
	u8		Cycle;
	u8	 	Flag;
	u8 		Hour_done;
}PREEN_DATA;

typedef struct
{
	u16 	Year;
	u16 	Month;
	u16 	Day;
	u16 	Hour;
	u16 	Min;
	u16 	Sec;
	u16 	Week_Day;	//星期几，周日为0，周六为6,以此类推
}UNIX_TIME;

typedef struct
{
	bool 	ctrl_stop;
	u8 		user_mode;
	bool 	ctrl_change;
	bool 	ctrl_ack_flag;
	u8		ctrl_sta;
	bool 	ctrl_dock_req;
	s8		app_bat;
	s8		app_last_bat;
}APP_DATA;

typedef struct
{
	u8 gridx;
	u8 gridy;
	u8 data;
}WIFI_DATA;


typedef struct
{
	bool 	no_msg;				//不需要向导航板发送信息标志,false:需要上传,true:不需要上传
	bool 	l_bump_flag;
	bool 	r_bump_flag;
	bool 	m_bump_flag;
	bool 	slam_ok;			//导航板启动完成，陀螺仪，小鸟等都已启动，可以工作
	bool  	tick_flag;			//自己获取数据。SLAM发送心跳通信标志，0：没有接收到心跳，1：接收到心跳
	bool	sleep_flag;			//休眠标志，由底盘通知SLAM
	bool 	wifi_ok;			//网络连接成功标志。
	bool 	ir_flag;
	bool	preen_off;			//由底盘遥控器发起的取消预约，true：为发起取消
	bool	preen_on;			//由底盘遥控器发起的设置预约，true：为发起设置
	bool	restart_flag;		//底盘重启标志，开机/重启后为false,10s后为true;
	u8 		skid_flag;			//打滑标志
	u8 		ack;				//上传数据。收到某些SLAM命令后，需要回馈ACK
	u8 		dust_level;			//上传数据。收到充电座红外信号时上传
	u8  	tick_check_step;	//心跳检测步骤
	u8 		dipan_req;
	u8		dipan_req_pre;
	s8 		bat;				//电池百分比
	s8  	last_bat;
	u8 		preen_hour;			//由底盘遥控器发起的预约小时
	u8		preen_min;			//由底盘遥控器发起的预约分钟
	u32 	skid_report_time; 	//打滑上报标志时间，如果检测到打滑，1s之类上报打滑标志，1s后清零
	u32 	tick_time;			//自己获取数据。心跳通信时间，超过一定时间没有收到，会报错
}SLAM_DATA;

typedef struct
{
	bool    fail;
	bool 	check_flag;		//启动旋转打滑检测标志
	bool 	time_flag;		//40ms计时标志
	u8		direction;		//旋转方向，0：左转，1，右转
	u8  	check_time;		//获得脉冲增量次数
	u8 		resuce_time;	//自救次数
	u32 	start_time;		//启动旋转打滑检测时的时间
	u32 	skid_time;
	int 	l_lenght;		//
	int 	l_last_length;
	int		r_length;
	int 	r_last_length;
	int 	l_sum_length;
	int		r_sum_length;
	short 	start_angle;
	short	end_angle;
	float 	rot_angle;		//由脉冲增量得到的旋转角度量

	u32		l_speed;
	u32		l_last_speed;
	u32		r_speed;
	u32		r_last_speed;
	
}ROTATE_SKID;

typedef struct
{
	bool check_flag;
	bool time_flag;
	u8 	direction;
	u8  check_time;
	u8  check_step;
	u32 start_time;
	int l_length;
	int r_length;
	int l_last_length;
	int r_last_length;
	int l_sum_length;
	int r_sum_length;
	float rot_angle;	
}ROTATE_ANGLE;

typedef struct
{
	bool 	fail;
	bool 	check_flag;
	u8		check_step;
	u32 	check_start_time;
	u32 	length;
	
}FREE_SKID;

typedef struct
{
	bool 	use;
	bool 	flag;					//进行异常判断标志,true:需要判断,false:无需判断
	u8	 	fail;					//错误标志,true：保护动作已做完，无法挽回，判定异常，false：无错误或正在保护
	u8 		step;					//用于风机读取脉冲用步骤
	u8 		check_step;				//
	u8 		error_time;				//错误次数
	u8		disc_error_time;		//间断的错误次数
	u32	 	length;
	u32		check_time;
	u32		last_length;
	u32		done_time;				//保护动作完成时间
	
}BRUSH_DATA;

typedef struct
{
	u32 	piv_current_sum;
	u32		piv_voltage_sum;
	u32		piv_current_count;
	u32 	piv_voltage_count;
	float	real_current;
	float 	real_voltage;
	float   ek[3];
	
}CHARGE_DATA;

typedef struct
{
	bool 	reinit;
	bool 	BatteryChargeForbid;
	u8 		bat_recal;
	u32		BatteryCapability;
	u32 	BatteryFDCap;
	u32		UsePowerTime;
	u32 	start_time;
}BATTERY;

typedef struct
{
	u8		oc_data;
}FACTORY_DATA;

typedef struct sweep_node
{
	short xpos;
	short ypos;
	u32 sweep_time;
	struct sweep_node *next;
}SWEEP_BUMP_NODE;

#endif





