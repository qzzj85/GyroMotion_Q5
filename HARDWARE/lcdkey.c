////////////////////////头文件////////////////////////////////////
#include "AAA-include.h"
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量//////////////////////////////////
//qz add
//根据TM1668 datasheet，第0字节，第2字节，第4字节，第6字节，第8字节，第9字节被启用
//时两位由第0字节，第2字节控制，分两位由第4字节、第6字节控制。秒闪烁由第6字节控制
//其他显示信号由第8字节和第9字节显示。
u8 Dis_Data[10]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff} ; 	//显示的数据亮
u8 Dis_Data_Temp[10]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//u8 Dis_Data[10]={0};

bool Dis_On;            //显示标志。
bool Dis_ErrOn ;    	//错误图标亮
bool Dis_UvOn ;			//uv图标亮
bool Dis_PrOn ;			//预约天数亮
bool Dis_AuOn ;			//自动清扫亮
bool Dis_SpOn ;			//局部清扫亮
bool Dis_HourOn ;		//小时亮
bool Dis_MinuteOn ;		//分钟亮
bool Dis_ColOn ;		//时间点亮
bool Dis_WeekOn ;		//星期亮
bool Dis_SpeedOn ;		//速度亮
bool Dis_PowerOn ;		//电池图标亮
bool Dis_ChargeOn ;		//充电图标亮
bool Dis_SpeedGo;       //轮子图标转动
bool Dis_YBS;			//qz add 沿边图标亮
bool Dis_Docking;		//qz add:回充图标亮
bool Dis_Laji;			//qz add:垃圾满图标
bool Dis_Guihua;		//qz add:规划图标
bool Dis_Zhongdian;		//qz add:重点图标
bool Dis_Yuyue;			//qz add:预约图标
bool Dis_SecOn;			//qz add:秒闪烁标志
bool Dis_AllFlagOn;		//qz add:所有标志开关
bool Dis_TestOn;		//qz add:进入测试模式时，时间无需显示
bool Dis_AllLedOn=false;//qz add 20180725
bool Dis_StatusOn;		//qz add 20180725，状态显示区(即8888LED)

bool Dis_GuihuaGlint;	//qz add:规划标志闪烁（SLEEP时）
bool Dis_ErrGlint ;		//错误图标闪烁
bool Dis_UvGlint ;		//uv图标闪烁
bool Dis_PrGlint ;		//预约天数闪烁
bool Dis_AuGlint ;		//自动清扫闪烁
bool Dis_SpGlint ;		//局部清扫闪烁
bool Dis_HourGlint ;	//小时闪烁
bool Dis_MinuteGlint ;	//分钟闪烁
bool Dis_ColGlint ;		//时间点闪烁
bool Dis_WeekGlint ;	//星期闪烁
bool Dis_SpeedGlint ;	//速度闪烁
bool Dis_PowerGlint ;	//电池图标闪烁
bool Dis_ChargeGlint ;	//充电图标闪烁
bool Dis_Test;			//用于外
bool Dis_TimeSet;		//qz add 20180817
bool Dis_H_M_Glint;	//false,小时闪烁,true,分钟闪烁	//qz add 20180817
bool Dis_TimeSet_Sec=false;	//qz add 20180817

u8 Dis_Preengage;	//预约内容
u8 Dis_Hour; 		//显示的小时
u8 Dis_Minute;  		//显示的分钟
u8 Dis_Week;			//显示的星期内容  范围 1——7
u8 Dis_Speed;		//显示的速度，范围1——3
u8 Dis_Power;		//显示的电源电量 ，范围0——3
u8 Dis_Hour_Set;	//qz add 20180817
u8 Dis_Min_Set;		//qz add 20180817

bool dis_updata;        //显示更新标志
u8 charge_glint;		//qz add 20180123

///////////////////////私有变量////////////////////////////////////
//qz mask
//const u8 disnumber[16] = {0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,
//                             0xef,0xce,0xee,0xe3,0xa9,0x67,0xe9,0xe8};//数字解码

//qz add
const u8 disnumber[16]={0x77,0x12,0x5d,0x5b,0x3a,0x6b,0x6f,0x52,0x7f,0x7b,0x7e,0x2f,0x65,0x1f,0x6d,0x6c};//数字解码0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
const u8 disspeed[6] = {0x80,0x01,
                           0x61,0x11,
						   0xa1,0x11};  //速度数字解码
//const u8 dispower[4] = {0x0,0x4,0x6,0x7}; 	
const u8 dispower[4]={0x20,0x30,0x38,0x3c};	//qz:电池电量数字解码

bool Action_display;  // 当前的显示状态
///////////////////////全局函数////////////////////////////////////

//////////////////////局部函数定义//////////////////
/*********************************************************
函数：void  AutoDisplay(void)
输入：无
输出：无
说明：根据显示寄存器内容显示，显示屏0.1秒更新一次
      闪烁时的频率为1秒。
	  轮盘转动更新频率为0.6秒、0.4秒、0.2秒更新
*********************************************************/
void  AutoDisplay(void)
{
  static u8 number = 0,i=0;
	
  if(Dis_On == true)     //允许显示
  {
	  if(dis_updata == true)   //到了更新显示屏
	  {
	  	dis_updata = false;
		if(Action_display == false)
		{
		    Action_display = true;
            lcdkey_bgon();     //打开背光
            //lcdkey_bgoff(); 				//zdkmd
	        lcdkey_dison();    //打开显示
		}

		
		if( Dis_PowerOn  == false)//显示电池
			dis_disPower();
		else
			dis_Power();
		////////////秒针,电池图标,垃圾图标闪烁显示区/////////////////
		////////////秒针,电池图标,垃圾图标闪烁显示区/////////////////
		////////////秒针,电池图标,垃圾图标闪烁显示区/////////////////
		number ++;			 
			// Dis_Data[7] |= 1; 
			//Dis_Data[2] |= 0X10;

		//qz add 20180703	如果不需要显示秒,即刻关闭
		if((!Dis_SecOn)|(Dis_TimeSet))
			Dis_Data[4]&=0x7F;		//关闭秒LED显示

		if(number == 20)   //闪烁的1秒定时	//dis_update在TIM2定时中断中50ms更新一次，所以20*50ms=1s。
			{
				 number =0;
				 if((Dis_SecOn)&(!Dis_ErrOn)&(!Dis_TimeSet))		//qz addd20180417,在异常状态下，显示dis_err_code时，不用显示秒,qz add 20180818,在遥控器时间设置时也无需闪烁秒针
					{
						Dis_Data[4]^=(1<<7);	//qz add 闪烁标志取反,秒LED闪烁
						if(Dis_Laji)			//垃圾显示标志
						{
							Dis_Data[9]^=(1<<1);//垃圾闪烁
						}
					}

				 //QZ ADD						//充电时，有些电池图标需要闪烁
					switch(charge_glint)
					 {
					 #if 1
						 case 0:
							 break;
						 case 1:
						 	i++;
							if(i>4)
								i=0;
							Dis_Glint_Power_Acc(i);
						 break;
						 case 2:
						 	i++;
							if(i>4)
								i=0;
							Dis_Glint_Power_Acc(i);
						 break;
						 case 3:
							 i++;
							 if(i>4)
								 i=0;
							 Dis_Glint_Power_Acc(i);
						 break;
						 case 4:
							 i++;
							 if(i>4)
								 i=0;
							 Dis_Glint_Power_Acc(i);
						 break;
						 case 5:
						 break;
						#endif
					 }

					if(Dis_GuihuaGlint)
						{
							Dis_Data[9]^=(0x01);
						}
					 //QZ ADD END

			}


		//////////////时间,异常代码,测试数据显示区/////////////////
		//////////////时间,异常代码,测试数据显示区/////////////////
		//////////////时间,异常代码,测试数据显示区/////////////////
		//qz add 20180416
		if((mode.sub_mode!=SELF_TEST)&(Dis_StatusOn))
		{
		//if(!Dis_ErrOn)	//qz mask 20180710
		if((!dis_err_code)&(!Dis_TimeSet))
			{			
				if( Dis_HourOn == false)//显示小时
					dis_disHour();
				else
					dis_Hour();
				if( Dis_MinuteOn  == false)//显示分钟
					dis_disMinute();
				else
					dis_Minute();		
			}
		else if(dis_err_code)			//显示error_code
			{
				Dis_ErrCode(dis_err_code);
			}
		else if(Dis_TimeSet)			//遥控器设置时间显示
			{
				Dis_HourMin_Set(0);
			}

#if 0
			if(mode.mode==YBS)
				{
					Dis_Test_Data(YBS_DISTANCE);
				}
#endif
		}

		if((mode.mode==CEASE)&(mode.sub_mode==SELF_TEST))
			{
				Dis_Data[0]=0;
				Dis_Data[2]=0;
				Dis_Data[4]=0;
				Dis_Data[6]=0;	
				if((mode.test_item==TST_DOCK)&(mode.test_step!=0)&(mode.test_dis_data!=PASS))				//回充测试另外显示
					{
						Dis_Dock_Data();
						
					}
				else
				{
				switch(mode.test_dis_data)
					{
						case PASS:
							Dis_Data[0]|=0x7c;	//'P'
							Dis_Data[2]|=0x7e;	//'A'
							Dis_Data[4]|=0x6b;	//'S'
							Dis_Data[6]|=0x6b;
							break;
						case FAIL:
							Dis_Data[0]|=0x6c;
							Dis_Data[2]|=0x7e;
							Dis_Data[4]|=0x24;
							Dis_Data[6]|=0x25;
							break;
						case NONE:
							break;
						case MAIN_MENU:
							Dis_Data[0]|=0x08;
							Dis_Data[2]|=0x08;
							Dis_Data[4]|=0x08;
							Dis_Data[6]|=0x08;
							break;
						case FULL:
							Dis_Data[0]|=0x6c;
							Dis_Data[2]|=0x37;
							Dis_Data[4]|=0x25;
							Dis_Data[6]|=0x25;
							break;
						default:
							Dis_Test_Data(mode.test_dis_data);
							break;
					}
				}
			}

		/////////////标志灯显示区//////////////
		/////////////标志灯显示区//////////////
		/////////////标志灯显示区//////////////
		if((Dis_AllFlagOn)&(mode.sub_mode!=SELF_TEST))		//qz mask 20180522
		{
		
			Clr_Alldisflag();
			switch (Slam_Data.dipan_req_pre)
			{
				case DIPAN_REQ_NONE:	//qz add 20180522
					break;
				case DIPAN_REQ_SWEEP:
					Dis_Guihua=true;
				break;
				case DIPAN_REQ_YBS:
					Dis_YBS=true;
				break;
				case DIPAN_REQ_SPOT:
					Dis_Zhongdian=true;
				break;
				case DIPAN_REQ_DOCK:
					Dis_Docking=true;
				break;
				//qz add 20180420
				//case DIPAN_REQ_PREEN:
					//Dis_Yuyue=true;
//				break;
				//qz add end
				default:
				break;
			}
		}


		#if 1
		//qz add
		if(Dis_YBS==false)
			dis_disybs();
		else
			dis_ybs();
		if(Dis_Docking==false)
			dis_disdocking();
		else
			dis_docking();

		if(!Dis_GuihuaGlint)
		{
			if(Dis_Guihua==false)
			dis_disguihua();
			else
			dis_guihua();
		}
		if(Dis_Zhongdian==false)
			dis_diszhongdian();
		else
			dis_zhongdian();

		//qz add 20180801				//因为预约灯用于网络灯，在自测模式下，此灯用于显示其他状况
		if(mode.sub_mode==SELF_TEST)
		{
		//qz add end
		if(Dis_Yuyue==false)
			dis_disyuyue();
		else
			dis_yuyue();
		}
		//qz add 20180801
		else							//在非自测模式下，统统用来显示网络灯，DEAD模式不会进入这里
			{
				if(Slam_Data.wifi_ok)
					dis_yuyue();
				else
					dis_disyuyue();
			}
		//qz add end
		if(!Dis_Laji)
			dis_dislaji();
		else
			dis_laji();

		#endif

		////////////写入缓存////////////
		////////////写入缓存////////////
		//qz add
		Lcd_WriteCommand(0x40); //数据地址自动增加
		if(Dis_AllLedOn)		//老化模式下，灯全亮
			Lcd_WriteDataAuto(0XC0,Dis_Data_Temp,10);
		else
			Lcd_WriteDataAuto(0XC0,Dis_Data,10);

		if((Dis_Hour>22)|(Dis_Hour<6))		//22点到0点之间消光显示
			Lcd_WriteCommand(0x8a);
		else
			Lcd_WriteCommand(0x8f);
		//qz add end
	  } 
  }



  else
  {
      if(Action_display != false)
	  {
	      Action_display = false;
          lcdkey_bgoff();
	      lcdkey_disoff();
	  }

  }

 
}
/*********************************************************
程序名称：delay(uint32 delaynumbe)
描述：延时子程序
输入：delaynumber   延时时间的长短
输出：无
**********************************************************/
void delay(u32 delaynumber)
{
  u32 l;
  for(l=0; l<delaynumber; l++)
  ;
}
/***********************************************************
程序名字：void Init_Lcd(void)
描述：初始化液晶显示器芯片TM1723
输入：无
输出：无
***********************************************************/

void Lcd_Write(u8 data)
{
	u8 temp=0x01;
	for(u8 i=0;i<8;i++)
		{
			delay(1);
			Reset_LCD_Clk();
			delay(1);
			if((data)&temp)
				Set_LCD_Data();
			else
				Reset_LCD_Data();
			delay(1);
			Set_LCD_Clk();

			temp=temp<<1;
			
		}
}
void Lcd_WriteDataAuto(u8 addr,u8* data_addr,u8 num)
{
	Set_LCD_Stb();
	Set_LCD_Clk();
	Set_LCD_Data();

	Reset_LCD_Stb();
	delay(10);

	Lcd_Write(addr);
	for(int i=0;i<num;i++)
		Lcd_Write(*data_addr++);
	
	Set_LCD_Stb();
	Set_LCD_Clk();
	Set_LCD_Data();
	
}

void Lcd_WriteCommand(u16 command)
{
	Set_LCD_Stb();
	Set_LCD_Clk();
	Set_LCD_Data();

	Reset_LCD_Stb();
	delay(10);
	Lcd_Write((u8)command);
   Set_LCD_Data();
   Set_LCD_Stb();
	 Set_LCD_Clk();
}

void Init_Lcd(void)
{
//	u8 dis_data_temp[10]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//  Set_LCD_Power();
  Set_LCD_Stb();
  Set_LCD_BG();
  Set_LCD_Data();
  Set_LCD_Clk();
  //全部关闭接口
  delay(100);
//  Reset_LCD_Power();
  delay(100);

	Lcd_WriteCommand(0x03);	//默认7位10段
	Lcd_WriteCommand(0x40);	//数据地址自动增加
	Lcd_WriteDataAuto(0xC0,Dis_Data,10);
	Lcd_WriteCommand(0x8B);	//开显示，脉冲宽度10/16

//  Set_LCD_Power();	//QZ Add,must be!

//  Lcd_WriteCommand(0x1c6);//设置为普通模式
//  Lcd_WriteCommand(0X40);		//LCD1/2偏压选项，2个公共口  QZ
//  Lcd_WriteCommand(2);			//打开系统振荡器  QZ
//  Lcd_WriteCommand(0X50);		//LCD1/2偏压选项，4个公共口  QZ
//  Lcd_WriteCommand(0x30);//系统时钟源为内部rc振荡器
//  clr_display();
  Action_display = false;
  
}
/*************************************************************
程序名称：lcdkey_write
描述：向LCD控制芯片TM1723写入一个字节数据
输入：data   要写入的内容
输出：无
*************************************************************/
//根据HT1621B datasheet，写命令模式，需要先发命令控制码二进制"100"
//DAT的数据在WR的上升沿被HT1621B锁存	//QZ
//TM1668协议，LSB传送 20171212


/**************************************************************
程序名称：cdkey_dison
描述：打开显示
输入：无
输出：无
**************************************************************/
void  lcdkey_dison(void)
{
	Lcd_WriteCommand(0x8F);	//开显示，脉冲宽度14/16
}
/**************************************************************
程序名称：lcdkey_disoff
描述：关闭显示
输入：无
输出：无
**************************************************************/
void  lcdkey_disoff(void)
{
  Lcd_WriteCommand(0x80);	//关显示
}
/**************************************************************
程序名称：cdkey_bgon
描述：打开背光
输入：无
输出：无
**************************************************************/
void  lcdkey_bgon(void)
{
//  Reset_LCD_BG();
}
/**************************************************************
程序名称：cdkey_bgoff
描述：关闭背光
输入：无
输出：无
**************************************************************/
void  lcdkey_bgoff(void)		//关闭背光
{
//  Set_LCD_BG();
}


/**************************************************************
程序名称：lcdkey_disdata
描述：在指定位置显示数据
输入：无
输出：addr   写入现实区位置,地址不能大于15
      str    要写入的数据地址
	  number 要写入的字节数
**************************************************************/
//根据HT1621B datasheet，写命令模式，需要先发命令控制码二进制"101"
//DAT的数据在WR的上升沿被HT1621B锁存	QZ
//写数据到HT1621B的RAM命令格式:
// 1 0 1 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3
//根据datasheet，HT1621B有32X4bit个RAM,使用连续写数据模式时,每写4Bit,addr加1,采用u8的*str时
//一个字节装载了两个RAM数据,根据原理图,RAM起始地址为12,终止地址为31,所以共20个RAM,str[]数量不能大于10,number不能大于10
void  lcdkey_disdata(u8  addr, u8*  str, u8  number)
{
u8 l,m,n;
   Reset_LCD_Stb();		//CS0  QZ
   delay(1);  
   Reset_LCD_Clk();		//WR0  QZ	
   Set_LCD_Data();		//DAT1 QZ
   delay(1);  
   Set_LCD_Clk();		//WR1	QZ	发送'1'
   delay(1); 
   Reset_LCD_Clk();		//WR0	QZ
   Reset_LCD_Data();	//DA0	QZ
   delay(1);  
   Set_LCD_Clk();		//WR1	QZ	发送'0'
   delay(1); 
   Reset_LCD_Clk();		//WR0	QZ
   Set_LCD_Data();		//DA1	QZ
   delay(1);  
   Set_LCD_Clk();		//WR0	QZ	发送'1'
   delay(1); 
   for(l=0; l<6; l++)
   {
	  Reset_LCD_Clk();			//WR0 
	  if((addr & 0x20) != 0)	
	   {
	     Set_LCD_Data();		//DA1
	   }
	  else
	   {
	     Reset_LCD_Data();		//DA0
	   } 
	  addr *=2;					//addr向左移动一位,共发送6次
	  delay(1);
	  Set_LCD_Clk();			//WR1
	  delay(1);  
   }
   for(l = 0;l<number ;l++)
   {
       n = str[l];
       for(m=0; m<8; m++)
	   {
	       Reset_LCD_Clk();		//WR0
	       if((n & 0x1) != 0)
	       {
	           Set_LCD_Data();	//DA1
	       }
	       else
	       {
	           Reset_LCD_Data();	//DA0
	       } 
	       n /=2;					//n向右移动一位			
	       delay(1);
	       Set_LCD_Clk();		//WR1
	       delay(1);  
	   }
   }
   Set_LCD_Data();
   Set_LCD_Stb();
}
/************************************************************
显示预约图标
************************************************************/
void dis_Preengage (void)
{
  //Dis_Data[9] = 0;
  //Dis_Data[8] = 0;
  //Dis_Data[7] |= 0x8;
  Dis_Data[0] = 0;
  Dis_Data[1] = 0;
  Dis_Data[2] |= 0x80;
  if(Dis_Preengage != 0)
  {
	{
	 	if((Dis_Preengage&0x1) != 0)  //显示星期1
		{
		 	//Dis_Data[8] |= 0x44;
		 	Dis_Data[1] |= 0x44;
		}
	 	if((Dis_Preengage&0x2) != 0)  //显示星期2
		{
		 	//Dis_Data[8] |= 0x22;
		 	Dis_Data[1] |= 0x22;
		}
	 	if((Dis_Preengage&0x4) != 0)  //显示星期3
		{
		 	//Dis_Data[8] |= 0x9;
		 	Dis_Data[1] |= 0x90;
		}
	 	if((Dis_Preengage&0x8) != 0)  //显示星期4
		{
		 	//Dis_Data[8] |= 0x90;
		 	Dis_Data[1] |= 0x9;
		}
	 	if((Dis_Preengage&0x10) != 0)  //显示星期5
		{
		 	//Dis_Data[9] |= 0x11;
		 	Dis_Data[0] |= 0x11;
		}
	 	if((Dis_Preengage&0x20) != 0)  //显示星期6
		{
		 	//Dis_Data[9] |= 0x88;
		 	Dis_Data[0] |= 0x88;
		}
	 	if((Dis_Preengage&0x40) != 0)  //显示星期7
		{
		 	//Dis_Data[9] |= 0x22;
		 	Dis_Data[0] |= 0x22;
		}	  
	 	if((Dis_Preengage&0x80) != 0)  //每天
		{
		 	//Dis_Data[9] |= 0x22;
		 	Dis_Data[0] |= 0x44;
		}
	}
  } 
  
}

/************************************************************
关闭预约图标
************************************************************/
void dis_disPreengage(void)
{
	  //Dis_Data[7] &= 0xf7;
	  //Dis_Data[8] &= 0x0;
	  //Dis_Data[9] &= 0x0;
	  Dis_Data[2] &= 0x7f;
	  Dis_Data[1] &= 0x0;
	  Dis_Data[0] &= 0x0;
}
/***********************************************************
显示小时
***********************************************************/
void dis_Hour(void)
{
 u8 h,l;
/*    if(Dis_Hour > 23)
	Dis_Hour = 0;*/
 	h = Dis_Hour /10;//
 	l = Dis_Hour % 10;//
    //Dis_Data[5] &= 0x10;
	//Dis_Data[4] &= 0x10;
	//Dis_Data[5] |= disnumber[h];
	//Dis_Data[4] |= disnumber[l];

	//qz mask
//	Dis_Data[9] &= 0x10;
//	Dis_Data[8] &= 0x10;
	//qz mask end

	//qz add
	Dis_Data[0]&=0x80;
	Dis_Data[2]&=0x80;
	//qz add end

	//qz mask	
//	Dis_Data[9] |= disnumber[h];
//	Dis_Data[8] |= disnumber[l];
	//qz mask end
	Dis_Data[0] |= disnumber[h];
	Dis_Data[2] |= disnumber[l];
	//qz add
	
}
/***********************************************************
关闭小时
***********************************************************/
void dis_disHour(void)
{
    //Dis_Data[5] &= 0x10;
	//Dis_Data[4] &= 0x10;

	//qz mask
//    Dis_Data[9] &= 0x10;
//	Dis_Data[8] &= 0x10;
	//qz mask end

	//qz add
	Dis_Data[0]&=0x80;
	Dis_Data[2]&=0x80;
	//qz add end
}
/***********************************************************
显示分钟
***********************************************************/
void dis_Minute(void)
{
 u8 h,l;
    if(Dis_Minute > 99)
	Dis_Minute = 0;
 
 	h = Dis_Minute / 10;//
 	l = Dis_Minute % 10;//
    //Dis_Data[3] &= 0x10;
	//Dis_Data[2] &= 0x10;
	//Dis_Data[3] |= disnumber[h];
	//Dis_Data[2] |= disnumber[l];
	Dis_Data[4]&=0x80;
	Dis_Data[6]&=0x80;
	Dis_Data[4] |= disnumber[h];
	Dis_Data[6] |= disnumber[l];
}
/***********************************************************
关闭分钟
***********************************************************/
void dis_disMinute(void)
{
   // Dis_Data[3] &= 0x10;
	//Dis_Data[2] &= 0x10;
   Dis_Data[4]&=0x80;
   Dis_Data[6]&=0x80;
}
/***********************************************************
显示星期
***********************************************************/
void dis_Week(void)
{  
    if((Dis_Week > 6))
	Dis_Week = 0;
	//Dis_Data[1] &= 0x10;
	//Dis_Data[1] |= disnumber[Dis_Week+1];
	Dis_Data[5] &= 0x10;
	Dis_Data[5] |= disnumber[Dis_Week+1];
}
/***********************************************************
关闭星期
***********************************************************/
void dis_disWeek(void)
{
    //Dis_Data[1] &= 0x10;
    Dis_Data[5] &= 0x10;
}
/***********************************************************
显示速度图标
***********************************************************/
void dis_Speed(void)
{
	//Dis_Data[0] &= 0xee;
	//Dis_Data[6] &= 0xee;
	//Dis_Data[7] &= 0xf9;
	Dis_Data[4] &= 0xee;
	Dis_Data[3] &= 0xee;
	Dis_Data[2] &= 0x9f;
	if((Dis_Speed > 3) || (Dis_Speed == 0))
	Dis_Speed = 1;
	if(Dis_Speed == 1)
	{
	    //Dis_Data[0] |= 0x10;
		//Dis_Data[6] |= 0x1;
	    Dis_Data[4] |= 0x10;
		Dis_Data[3] |= 0x10;
	}
	else
	{
	   if(Dis_Speed == 2)
	   {
	       //Dis_Data[0] |= 1;
		   //Dis_Data[6] |= 0x11;
		   //Dis_Data[7] |= 0x6;
	       Dis_Data[4] |= 1;
		   Dis_Data[3] |= 0x11;
		   Dis_Data[2] |= 0x60;
	   }
	   else
	   {
	       //Dis_Data[0] |= 0x11;
		   //Dis_Data[6] |= 0x11;
		   //Dis_Data[7] |= 0x2;
	       Dis_Data[4] |= 0x11;
		   Dis_Data[3] |= 0x11;
		   Dis_Data[2] |= 0x20;
	   }
	}
}
/***********************************************************
关闭速度图标
***********************************************************/
void dis_disSpeed(void)
{
	//Dis_Data[0] &= 0x0;
	//Dis_Data[6] &= 0x0;
	//Dis_Data[7] &= 0xf9;
	Dis_Data[4] = 0x0;
	Dis_Data[3] = 0x0;
	Dis_Data[2] &= 0x9f;
}
/***********************************************************
显示电池电量
***********************************************************/
void dis_Power(void)
{
	#if 0 	//qz mask 20180123
	//Dis_Data[7] &= 0x8f;
	if((Dis_Power > 3))
	Dis_Power = 0;
	//Dis_Data[7] |= dispower[Dis_Power];
	//qz mask
//	Dis_Data[2] &= 0xf8;
//	Dis_Data[2] |= dispower[Dis_Power];
	//qz mask end

	//qz add
	Dis_Data[8]&=0xc3;
	Dis_Data[8]|=dispower[Dis_Power];
	#endif
	if(Dis_Power>5)
		Dis_Power=5;
	//qz add
	//if((mode.mode==CHARGEING))		//qz mask 20180522
	if(Dis_PowerGlint)					//qz add 20180522
		{
			switch (Dis_Power)
				{
					case 1:					//小于等于25%
						charge_glint=1;
					break;
					case 2:					//大于25%，小于等于50%
						charge_glint=2;
					break;
					case 3:					//大于50%，小于等于75%
						charge_glint=3;
					break;
					case 4:					//大于75%，小于100%
						charge_glint=4;
					break;
					case 5:
						charge_glint=5;
					break;
						
				}
		}
	else
		{
			charge_glint=0;
			switch(Dis_Power)
				{
					case 1:		//小于等于25%
						Dis_Data[8]&=0xC3;
						Dis_Data[8]|=0xE3;
					break;
					case 2:		//大于25%，小于等于50%
						Dis_Data[8]&=0xC3;
						Dis_Data[8]|=0xF3;
					break;
					case 3:		//大于50%，小于等于75%
						Dis_Data[8]&=0xC3;
						Dis_Data[8]|=0xFB;
					break;
					case 4:		//大于75%，小于90%
					case 5:		//大于等于90%，小于等于100%
						Dis_Data[8]&=0xC3;
						Dis_Data[8]|=0xFF;
					break;
				}
		}
}
/***********************************************************
关闭电池电量
***********************************************************/
void dis_disPower(void)
{
	//Dis_Data[7] &= 0x8f;
	Dis_Data[8] &= 0xc3;	//qz modify
}

//qz add
void dis_disybs(void)
{
	Dis_Data[8]&=0x7f;
}

void dis_ybs(void)
{
	Dis_Data[8]|=0x80;
}

void dis_disdocking(void)
{
	Dis_Data[8]&=0xfe;
}

void dis_docking(void)
{
	Dis_Data[8]|=0x01;
}

void dis_guihua(void)
{
	Dis_Data[9]|=0x01;
}

void dis_disguihua(void)
{
	Dis_Data[9]&=0xfe;
}

void dis_zhongdian(void)
{
	Dis_Data[8]|=0x40;
}

void dis_diszhongdian(void)
{
	Dis_Data[8]&=0xbf;
}

void dis_yuyue(void)
{
	Dis_Data[8]|=0x02;
}

void dis_disyuyue(void)
{
	Dis_Data[8]&=0xfd;
}

void dis_dislaji(void)
{
	Dis_Data[9]&=0xfd;
}

void dis_laji(void)
{
	Dis_Data[9]|=0x02;
}
//qz add end
/***********************************************************
功能：清除显示内容
***********************************************************/
void clr_display(void)
{
Dis_On 			= 	false;      //显示标志。
Dis_ErrOn 		= 	false ;    	//错误图标亮
Dis_HourOn 		= 	false ;		//小时亮
Dis_MinuteOn 	= 	false ;		//分钟亮
Dis_SecOn		=	false;

Dis_PowerOn 	= 	false ;		//电池图标亮
Dis_PowerGlint	=	false;

Dis_ChargeOn 	= 	false ;		//充电图标亮
      

//qz add 20180703
Dis_AllFlagOn	=	false;
Dis_Laji		=	false;
Dis_GuihuaGlint	=	false;		
Dis_StatusOn	=	false;				//状态区显示亮
Clr_Alldisflag();


}
 /*****************************************
 清除显示数据
 *****************************************/
 void Clr_DisplayData(void)
 {
	u8 i;//循环计数变量	  
	for(i=0; i<10; i++)
    {
	    Dis_Data[i] = 0;
	}
 }


//qz add
 void Clr_Alldisflag(void)
 {
 	Dis_YBS=false;
	Dis_Guihua=false;
	Dis_Zhongdian=false;
	Dis_Docking=false;
//	Dis_Yuyue=false;		//qz mask 20180522
 		
 }

void Dis_ErrCode(u16 error_code)
{
	u16 mask=0xf000,shift=12,i=0,j=0;
	Dis_ErrOn=true;
	for(i=0;i<4;i++)
		{
			Dis_Data[j]=disnumber[(error_code&mask)>>shift];
			j+=2;
			mask>>=4;
			shift-=4;
		}
}

void Dis_MainTest_Interface(void)
{
	u8 i=0;
	while(i<7)
		{
			Dis_Data[i]=0x00;
			Dis_Data[i]=0x08;
			i+=2;
		}
}

void Dis_Test_Data(u16 data)
{
	
	u8 data1,data2,data3;
	if(data>9999)
		data=data%10000;
	
	Dis_Data[0]=0;
	data1=data/1000;
	if(data1)
		Dis_Data[0]=disnumber[data1];
	data=data%1000;
	
	Dis_Data[2]=0;
	data2=data/100;
	if((data1!=0)|((data1==0)&(data2!=0)))
		Dis_Data[2]=disnumber[data/100];
	data=data%100;
	
	Dis_Data[4]=0;
	data3=data/10;
	if((data1!=0)|((data1==0)&(data2!=0))|((data1==0)&(data2==0)&(data3!=0)))
		Dis_Data[4]=disnumber[data/10];
	data=data%10;
	
	Dis_Data[6]=0;
	Dis_Data[6]=disnumber[data];
	
}

void Dis_Glint_Power_Acc(u8 i)
{
	switch(i)
	   {
		   case 0:
			   Dis_Data[8]&=0xc3;
		   break;
		   case 1:
			   Dis_Data[8]&=0xc3;
			   Dis_Data[8]|=0xe3;
		   break;
		   case 2:
			   Dis_Data[8]&=0xc3;
			   Dis_Data[8]|=0xf3;
		   break;
		   case 3:
			   Dis_Data[8]&=0xc3;
			   Dis_Data[8]|=0xfb;
		   break;
		   case 4:
			   Dis_Data[8]&=0xc3;
			   Dis_Data[8]|=0xff;
		   break;
	   }
}

void Dis_Dock_Data(void)
{
	u32 i,data1;
	Dis_Data[0]=0;
	Dis_Data[2]=0;
	Dis_Data[4]=0;
	Dis_Data[6]=0;
	data1=0x80000;
	for(i=0;i<7;)
		{
			if(mode.test_dis_data&data1)
				Dis_Data[i]|=0x01;
			data1=data1>>1;
			i+=2;
		}
	mode.test_dis_data=mode.test_dis_data&0x0FFFF;
	data1=0x8000;
	i=0;
	while(i<7)
	{
	if(mode.test_dis_data&data1)
		Dis_Data[i]|=0X24;
	data1>>=1;
	if(mode.test_dis_data&data1)
		Dis_Data[i]|=0X08;
	data1>>=1;
	if(mode.test_dis_data&data1)
		Dis_Data[i]|=0X12;
	data1>>=1;
	if(mode.test_dis_data&data1)
		Dis_Data[i]|=0X40;
	data1>>=1;
	i+=2;
	}
}

void Dis_HourMin_Set(u8 hour_min)
{
	u8 h,l,i=0,data1=0x01;
	static u8 Hour_Set_Temp=0,Min_Set_Temp=0,Dis_Data0_temp,Dis_Data2_temp,Dis_Data4_temp,Dis_Data6_temp;
	/*	  if(Dis_Hour > 23)
		Dis_Hour = 0;*/
		
	if(Hour_Set_Temp!=Dis_Hour_Set)
		{
			Hour_Set_Temp=Dis_Hour_Set;
			h = Hour_Set_Temp /10;
			l = Hour_Set_Temp % 10;
			Dis_Data[0]=disnumber[h];
			Dis_Data[2]=disnumber[l];
			Dis_Data0_temp=Dis_Data[0];
			Dis_Data2_temp=Dis_Data[2];
		}

	if(Min_Set_Temp!=Dis_Min_Set)
		{
			Min_Set_Temp=Dis_Min_Set;
			h = Min_Set_Temp/10;
			l = Min_Set_Temp%10;
			Dis_Data[4]=disnumber[h];
			Dis_Data[6]=disnumber[l];
			Dis_Data4_temp=Dis_Data[4];
			Dis_Data6_temp=Dis_Data[6];
		}
	
	if(!Dis_H_M_Glint)					//遥控器设置时间，小时闪烁
		{
			if(Dis_TimeSet_Sec)			//500ms时间
			{
				for(i=0;i<7;i++)
					{
						if((Dis_Data0_temp&data1))
							{
								Dis_Data[0]^=(1<<i);			//闪烁
							}
		
						if((Dis_Data2_temp&data1))
							{
								Dis_Data[2]^=(1<<i);			//闪烁
							}
						data1<<=1;
					}
				Dis_TimeSet_Sec=false;				
			}
			Dis_Data[4]=Dis_Data4_temp;							//分钟常亮
			Dis_Data[6]=Dis_Data6_temp;							//分钟常亮
		}
	else								//遥控器设置时间，分钟闪烁
		{
			if(Dis_TimeSet_Sec)			//500ms时间
			{
				for(i=0;i<7;i++)
					{
						if((Dis_Data4_temp&data1))
							{
								Dis_Data[4]^=(1<<i);			//闪烁
							}
		
						if((Dis_Data6_temp&data1))
							{
								Dis_Data[6]^=(1<<i);			//闪烁
							}
						data1<<=1;
					}
				Dis_TimeSet_Sec=false;				
			}
			Dis_Data[0]=Dis_Data0_temp;							//小时常亮
			Dis_Data[2]=Dis_Data2_temp;							//小时常亮
		}
}

