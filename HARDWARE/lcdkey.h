#ifndef __LCDKEY_H
#define __LCDKEY_H
/////////////////*****INCLUDE *************************
#include "stm32f10x.h"

///////////下面是显示驱动管脚的定义////////////////////////////
#define  LCD_STB  		 GPIO_Pin_12	//LCD指令控制   PC NEW	
#define  LCD_CLK  	 	 GPIO_Pin_3		//LCD时钟       PD NEW
#define  LCD_DATA	  	 GPIO_Pin_3		//LCD数据       PB NEW

#define  LCD_CS        GPIO_Pin_0  //PC0123
#define  LCD_WR        GPIO_Pin_1
#define  LCD_DAT       GPIO_Pin_2
#define  LCD_BL_ON     GPIO_Pin_3

#define	 LCD_POW_ON		GPIO_Pin_10				//PD

////////////////全局变量定义//////////////////////////
extern bool Dis_On;            //显示标志。
extern bool Dis_ErrOn ;    	//错误图标亮
extern bool Dis_UvOn ;			//uv图标亮
extern bool Dis_PrOn ;			//预约天数亮
extern bool Dis_AuOn ;			//自动清扫亮
extern bool Dis_SpOn ;			//局部清扫亮
extern bool Dis_HourOn ;		//小时亮
extern bool Dis_MinuteOn ;		//分钟亮
extern bool Dis_ColOn ;		//时间点亮
extern bool Dis_WeekOn ;		//星期亮
extern bool Dis_SpeedOn ;		//速度亮
extern bool Dis_PowerOn ;		//电池图标亮
extern bool Dis_ChargeOn ;		//充电图标亮

extern bool Dis_SpeedGo;       //轮子图标转动

extern bool Dis_ErrGlint ;		//错误图标闪烁
extern bool Dis_UvGlint ;		//uv图标闪烁
extern bool Dis_PrGlint ;		//预约天数闪烁
extern bool Dis_AuGlint ;		//自动清扫闪烁
extern bool Dis_SpGlint ;		//局部清扫闪烁
extern bool Dis_HourGlint ;	//小时闪烁
extern bool Dis_MinuteGlint ;	//分钟闪烁
extern bool Dis_ColGlint ;		//时间点闪烁
extern bool Dis_WeekGlint ;	//星期闪烁
extern bool Dis_SpeedGlint ;	//速度闪烁
extern bool Dis_PowerGlint ;	//电池图标闪烁
extern bool Dis_ChargeGlint ;	//充电图标闪烁
extern bool Dis_YBS;			//qz add 沿边图标亮
extern bool Dis_Docking;		//qz add:回充图标亮
extern bool Dis_Laji;			//qz add:垃圾满图标
extern bool Dis_Guihua;			//qz add:规划图标
extern bool Dis_Zhongdian;		//qz add:重点图标
extern bool Dis_Yuyue;			//qz add:预约图标
extern bool Dis_SecOn;	
extern bool Dis_AllFlagOn;
extern bool Dis_StatusOn;		//qz add 20180725，状态显示区(即8888LED)
extern bool Dis_GuihuaGlint;	//qz add
extern u8 Dis_Preengage;	//预约内容
extern u8 Dis_Hour; 		//显示的小时
extern u8 Dis_Minute;  		//显示的分钟
extern u8 Dis_Week;			//显示的星期内容  范围 1——7
extern u8 Dis_Speed;		//显示的速度，范围1——3
extern u8 Dis_Power;		//显示的电源电量 ，范围0——3

extern bool dis_updata;        //显示更新标志
extern bool Dis_AllLedOn;
extern bool Dis_Test;
extern bool Dis_TimeSet;
extern bool Dis_H_M_Glint;
extern bool Dis_TimeSet_Sec;

extern u8 Dis_Hour_Set;
extern u8 Dis_Min_Set;
extern u8 Dis_Data[10] ; 	//显示的数据亮
/////////////////***FUC*******************************
void Init_Lcd(void);	//初始化芯片的IO脚
u8 lcdkey_read(void);     // 读一个数
void Lcd_WriteCommand(u16 command);
void  lcdkey_disdata(u8  addr, u8*  str, u8  number);
void  lcdkey_bgoff(void);
void  lcdkey_bgon(void);
void  lcdkey_disoff(void);
void  lcdkey_dison(void);
void  AutoDisplay(void);
void clr_display(void);
void delay(u32 delaynumber);
 void Clr_DisplayData(void);
 void Lcd_Write(u8 data);
 void Lcd_WriteDataAuto(u8 addr,u8* data_addr,u8 num);
 void Lcd_WriteCommand(u16 command);

 //qz add
 void dis_disybs(void);
 void dis_ybs(void);
 void dis_disdocking(void);
 void dis_docking(void);
 void dis_zhongdian(void);
 void dis_diszhongdian(void);
 void dis_guihua(void);
void dis_disguihua(void);
void dis_yuyue(void);
void dis_disyuyue(void);

void Clr_Alldisflag(void);
 void dis_Preengage (void);//；；
 void dis_disPreengage(void);//
 void dis_Hour(void);
 void dis_disHour(void);
 void dis_Minute(void);
 void dis_disMinute(void);
 void dis_Week(void);
 void dis_disWeek(void);
 void dis_Speed(void);
 void dis_disSpeed(void);
 void dis_Power(void);
 void dis_disPower(void);
 void dis_dislaji(void);
 void dis_laji(void);
 
 void Dis_ErrCode(u16 error_code);
 void Dis_MainTest_Interface(void);
 void Dis_Test_Data(u16 data);
 void Dis_Test_Interface(void);
 void Dis_Glint_Power_Acc(u8 i);
 void Dis_Dock_Data(void);
 void Dis_HourMin_Set(u8 hour_min);
 //qz add end
#endif /* __MAIN_H */
