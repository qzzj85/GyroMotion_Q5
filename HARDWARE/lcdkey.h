#ifndef __LCDKEY_H
#define __LCDKEY_H
/////////////////*****INCLUDE *************************
#include "stm32f10x.h"

///////////��������ʾ�����ܽŵĶ���////////////////////////////
#define  LCD_STB  		 GPIO_Pin_12	//LCDָ�����   PC NEW	
#define  LCD_CLK  	 	 GPIO_Pin_3		//LCDʱ��       PD NEW
#define  LCD_DATA	  	 GPIO_Pin_3		//LCD����       PB NEW

#define  LCD_CS        GPIO_Pin_0  //PC0123
#define  LCD_WR        GPIO_Pin_1
#define  LCD_DAT       GPIO_Pin_2
#define  LCD_BL_ON     GPIO_Pin_3

#define	 LCD_POW_ON		GPIO_Pin_10				//PD

////////////////ȫ�ֱ�������//////////////////////////
extern bool Dis_On;            //��ʾ��־��
extern bool Dis_ErrOn ;    	//����ͼ����
extern bool Dis_UvOn ;			//uvͼ����
extern bool Dis_PrOn ;			//ԤԼ������
extern bool Dis_AuOn ;			//�Զ���ɨ��
extern bool Dis_SpOn ;			//�ֲ���ɨ��
extern bool Dis_HourOn ;		//Сʱ��
extern bool Dis_MinuteOn ;		//������
extern bool Dis_ColOn ;		//ʱ�����
extern bool Dis_WeekOn ;		//������
extern bool Dis_SpeedOn ;		//�ٶ���
extern bool Dis_PowerOn ;		//���ͼ����
extern bool Dis_ChargeOn ;		//���ͼ����

extern bool Dis_SpeedGo;       //����ͼ��ת��

extern bool Dis_ErrGlint ;		//����ͼ����˸
extern bool Dis_UvGlint ;		//uvͼ����˸
extern bool Dis_PrGlint ;		//ԤԼ������˸
extern bool Dis_AuGlint ;		//�Զ���ɨ��˸
extern bool Dis_SpGlint ;		//�ֲ���ɨ��˸
extern bool Dis_HourGlint ;	//Сʱ��˸
extern bool Dis_MinuteGlint ;	//������˸
extern bool Dis_ColGlint ;		//ʱ�����˸
extern bool Dis_WeekGlint ;	//������˸
extern bool Dis_SpeedGlint ;	//�ٶ���˸
extern bool Dis_PowerGlint ;	//���ͼ����˸
extern bool Dis_ChargeGlint ;	//���ͼ����˸
extern bool Dis_YBS;			//qz add �ر�ͼ����
extern bool Dis_Docking;		//qz add:�س�ͼ����
extern bool Dis_Laji;			//qz add:������ͼ��
extern bool Dis_Guihua;			//qz add:�滮ͼ��
extern bool Dis_Zhongdian;		//qz add:�ص�ͼ��
extern bool Dis_Yuyue;			//qz add:ԤԼͼ��
extern bool Dis_SecOn;	
extern bool Dis_AllFlagOn;
extern bool Dis_StatusOn;		//qz add 20180725��״̬��ʾ��(��8888LED)
extern bool Dis_GuihuaGlint;	//qz add
extern u8 Dis_Preengage;	//ԤԼ����
extern u8 Dis_Hour; 		//��ʾ��Сʱ
extern u8 Dis_Minute;  		//��ʾ�ķ���
extern u8 Dis_Week;			//��ʾ����������  ��Χ 1����7
extern u8 Dis_Speed;		//��ʾ���ٶȣ���Χ1����3
extern u8 Dis_Power;		//��ʾ�ĵ�Դ���� ����Χ0����3

extern bool dis_updata;        //��ʾ���±�־
extern bool Dis_AllLedOn;
extern bool Dis_Test;
extern bool Dis_TimeSet;
extern bool Dis_H_M_Glint;
extern bool Dis_TimeSet_Sec;

extern u8 Dis_Hour_Set;
extern u8 Dis_Min_Set;
extern u8 Dis_Data[10] ; 	//��ʾ��������
/////////////////***FUC*******************************
void Init_Lcd(void);	//��ʼ��оƬ��IO��
u8 lcdkey_read(void);     // ��һ����
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
 void dis_Preengage (void);//����
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
