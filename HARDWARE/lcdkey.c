////////////////////////ͷ�ļ�////////////////////////////////////
#include "AAA-include.h"
////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
//qz add
//����TM1668 datasheet����0�ֽڣ���2�ֽڣ���4�ֽڣ���6�ֽڣ���8�ֽڣ���9�ֽڱ�����
//ʱ��λ�ɵ�0�ֽڣ���2�ֽڿ��ƣ�����λ�ɵ�4�ֽڡ���6�ֽڿ��ơ�����˸�ɵ�6�ֽڿ���
//������ʾ�ź��ɵ�8�ֽں͵�9�ֽ���ʾ��
u8 Dis_Data[10]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff} ; 	//��ʾ��������
u8 Dis_Data_Temp[10]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//u8 Dis_Data[10]={0};

bool Dis_On;            //��ʾ��־��
bool Dis_ErrOn ;    	//����ͼ����
bool Dis_UvOn ;			//uvͼ����
bool Dis_PrOn ;			//ԤԼ������
bool Dis_AuOn ;			//�Զ���ɨ��
bool Dis_SpOn ;			//�ֲ���ɨ��
bool Dis_HourOn ;		//Сʱ��
bool Dis_MinuteOn ;		//������
bool Dis_ColOn ;		//ʱ�����
bool Dis_WeekOn ;		//������
bool Dis_SpeedOn ;		//�ٶ���
bool Dis_PowerOn ;		//���ͼ����
bool Dis_ChargeOn ;		//���ͼ����
bool Dis_SpeedGo;       //����ͼ��ת��
bool Dis_YBS;			//qz add �ر�ͼ����
bool Dis_Docking;		//qz add:�س�ͼ����
bool Dis_Laji;			//qz add:������ͼ��
bool Dis_Guihua;		//qz add:�滮ͼ��
bool Dis_Zhongdian;		//qz add:�ص�ͼ��
bool Dis_Yuyue;			//qz add:ԤԼͼ��
bool Dis_SecOn;			//qz add:����˸��־
bool Dis_AllFlagOn;		//qz add:���б�־����
bool Dis_TestOn;		//qz add:�������ģʽʱ��ʱ��������ʾ
bool Dis_AllLedOn=false;//qz add 20180725
bool Dis_StatusOn;		//qz add 20180725��״̬��ʾ��(��8888LED)

bool Dis_GuihuaGlint;	//qz add:�滮��־��˸��SLEEPʱ��
bool Dis_ErrGlint ;		//����ͼ����˸
bool Dis_UvGlint ;		//uvͼ����˸
bool Dis_PrGlint ;		//ԤԼ������˸
bool Dis_AuGlint ;		//�Զ���ɨ��˸
bool Dis_SpGlint ;		//�ֲ���ɨ��˸
bool Dis_HourGlint ;	//Сʱ��˸
bool Dis_MinuteGlint ;	//������˸
bool Dis_ColGlint ;		//ʱ�����˸
bool Dis_WeekGlint ;	//������˸
bool Dis_SpeedGlint ;	//�ٶ���˸
bool Dis_PowerGlint ;	//���ͼ����˸
bool Dis_ChargeGlint ;	//���ͼ����˸
bool Dis_Test;			//������
bool Dis_TimeSet;		//qz add 20180817
bool Dis_H_M_Glint;	//false,Сʱ��˸,true,������˸	//qz add 20180817
bool Dis_TimeSet_Sec=false;	//qz add 20180817

u8 Dis_Preengage;	//ԤԼ����
u8 Dis_Hour; 		//��ʾ��Сʱ
u8 Dis_Minute;  		//��ʾ�ķ���
u8 Dis_Week;			//��ʾ����������  ��Χ 1����7
u8 Dis_Speed;		//��ʾ���ٶȣ���Χ1����3
u8 Dis_Power;		//��ʾ�ĵ�Դ���� ����Χ0����3
u8 Dis_Hour_Set;	//qz add 20180817
u8 Dis_Min_Set;		//qz add 20180817

bool dis_updata;        //��ʾ���±�־
u8 charge_glint;		//qz add 20180123

///////////////////////˽�б���////////////////////////////////////
//qz mask
//const u8 disnumber[16] = {0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,
//                             0xef,0xce,0xee,0xe3,0xa9,0x67,0xe9,0xe8};//���ֽ���

//qz add
const u8 disnumber[16]={0x77,0x12,0x5d,0x5b,0x3a,0x6b,0x6f,0x52,0x7f,0x7b,0x7e,0x2f,0x65,0x1f,0x6d,0x6c};//���ֽ���0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
const u8 disspeed[6] = {0x80,0x01,
                           0x61,0x11,
						   0xa1,0x11};  //�ٶ����ֽ���
//const u8 dispower[4] = {0x0,0x4,0x6,0x7}; 	
const u8 dispower[4]={0x20,0x30,0x38,0x3c};	//qz:��ص������ֽ���

bool Action_display;  // ��ǰ����ʾ״̬
///////////////////////ȫ�ֺ���////////////////////////////////////

//////////////////////�ֲ���������//////////////////
/*********************************************************
������void  AutoDisplay(void)
���룺��
�������
˵����������ʾ�Ĵ���������ʾ����ʾ��0.1�����һ��
      ��˸ʱ��Ƶ��Ϊ1�롣
	  ����ת������Ƶ��Ϊ0.6�롢0.4�롢0.2�����
*********************************************************/
void  AutoDisplay(void)
{
  static u8 number = 0,i=0;
	
  if(Dis_On == true)     //������ʾ
  {
	  if(dis_updata == true)   //���˸�����ʾ��
	  {
	  	dis_updata = false;
		if(Action_display == false)
		{
		    Action_display = true;
            lcdkey_bgon();     //�򿪱���
            //lcdkey_bgoff(); 				//zdkmd
	        lcdkey_dison();    //����ʾ
		}

		
		if( Dis_PowerOn  == false)//��ʾ���
			dis_disPower();
		else
			dis_Power();
		////////////����,���ͼ��,����ͼ����˸��ʾ��/////////////////
		////////////����,���ͼ��,����ͼ����˸��ʾ��/////////////////
		////////////����,���ͼ��,����ͼ����˸��ʾ��/////////////////
		number ++;			 
			// Dis_Data[7] |= 1; 
			//Dis_Data[2] |= 0X10;

		//qz add 20180703	�������Ҫ��ʾ��,���̹ر�
		if((!Dis_SecOn)|(Dis_TimeSet))
			Dis_Data[4]&=0x7F;		//�ر���LED��ʾ

		if(number == 20)   //��˸��1�붨ʱ	//dis_update��TIM2��ʱ�ж���50ms����һ�Σ�����20*50ms=1s��
			{
				 number =0;
				 if((Dis_SecOn)&(!Dis_ErrOn)&(!Dis_TimeSet))		//qz addd20180417,���쳣״̬�£���ʾdis_err_codeʱ��������ʾ��,qz add 20180818,��ң����ʱ������ʱҲ������˸����
					{
						Dis_Data[4]^=(1<<7);	//qz add ��˸��־ȡ��,��LED��˸
						if(Dis_Laji)			//������ʾ��־
						{
							Dis_Data[9]^=(1<<1);//������˸
						}
					}

				 //QZ ADD						//���ʱ����Щ���ͼ����Ҫ��˸
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


		//////////////ʱ��,�쳣����,����������ʾ��/////////////////
		//////////////ʱ��,�쳣����,����������ʾ��/////////////////
		//////////////ʱ��,�쳣����,����������ʾ��/////////////////
		//qz add 20180416
		if((mode.sub_mode!=SELF_TEST)&(Dis_StatusOn))
		{
		//if(!Dis_ErrOn)	//qz mask 20180710
		if((!dis_err_code)&(!Dis_TimeSet))
			{			
				if( Dis_HourOn == false)//��ʾСʱ
					dis_disHour();
				else
					dis_Hour();
				if( Dis_MinuteOn  == false)//��ʾ����
					dis_disMinute();
				else
					dis_Minute();		
			}
		else if(dis_err_code)			//��ʾerror_code
			{
				Dis_ErrCode(dis_err_code);
			}
		else if(Dis_TimeSet)			//ң��������ʱ����ʾ
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
				if((mode.test_item==TST_DOCK)&(mode.test_step!=0)&(mode.test_dis_data!=PASS))				//�س����������ʾ
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

		/////////////��־����ʾ��//////////////
		/////////////��־����ʾ��//////////////
		/////////////��־����ʾ��//////////////
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

		//qz add 20180801				//��ΪԤԼ����������ƣ����Բ�ģʽ�£��˵�������ʾ����״��
		if(mode.sub_mode==SELF_TEST)
		{
		//qz add end
		if(Dis_Yuyue==false)
			dis_disyuyue();
		else
			dis_yuyue();
		}
		//qz add 20180801
		else							//�ڷ��Բ�ģʽ�£�ͳͳ������ʾ����ƣ�DEADģʽ�����������
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

		////////////д�뻺��////////////
		////////////д�뻺��////////////
		//qz add
		Lcd_WriteCommand(0x40); //���ݵ�ַ�Զ�����
		if(Dis_AllLedOn)		//�ϻ�ģʽ�£���ȫ��
			Lcd_WriteDataAuto(0XC0,Dis_Data_Temp,10);
		else
			Lcd_WriteDataAuto(0XC0,Dis_Data,10);

		if((Dis_Hour>22)|(Dis_Hour<6))		//22�㵽0��֮��������ʾ
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
�������ƣ�delay(uint32 delaynumbe)
��������ʱ�ӳ���
���룺delaynumber   ��ʱʱ��ĳ���
�������
**********************************************************/
void delay(u32 delaynumber)
{
  u32 l;
  for(l=0; l<delaynumber; l++)
  ;
}
/***********************************************************
�������֣�void Init_Lcd(void)
��������ʼ��Һ����ʾ��оƬTM1723
���룺��
�������
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
  //ȫ���رսӿ�
  delay(100);
//  Reset_LCD_Power();
  delay(100);

	Lcd_WriteCommand(0x03);	//Ĭ��7λ10��
	Lcd_WriteCommand(0x40);	//���ݵ�ַ�Զ�����
	Lcd_WriteDataAuto(0xC0,Dis_Data,10);
	Lcd_WriteCommand(0x8B);	//����ʾ��������10/16

//  Set_LCD_Power();	//QZ Add,must be!

//  Lcd_WriteCommand(0x1c6);//����Ϊ��ͨģʽ
//  Lcd_WriteCommand(0X40);		//LCD1/2ƫѹѡ�2��������  QZ
//  Lcd_WriteCommand(2);			//��ϵͳ����  QZ
//  Lcd_WriteCommand(0X50);		//LCD1/2ƫѹѡ�4��������  QZ
//  Lcd_WriteCommand(0x30);//ϵͳʱ��ԴΪ�ڲ�rc����
//  clr_display();
  Action_display = false;
  
}
/*************************************************************
�������ƣ�lcdkey_write
��������LCD����оƬTM1723д��һ���ֽ�����
���룺data   Ҫд�������
�������
*************************************************************/
//����HT1621B datasheet��д����ģʽ����Ҫ�ȷ���������������"100"
//DAT��������WR�������ر�HT1621B����	//QZ
//TM1668Э�飬LSB���� 20171212


/**************************************************************
�������ƣ�cdkey_dison
����������ʾ
���룺��
�������
**************************************************************/
void  lcdkey_dison(void)
{
	Lcd_WriteCommand(0x8F);	//����ʾ��������14/16
}
/**************************************************************
�������ƣ�lcdkey_disoff
�������ر���ʾ
���룺��
�������
**************************************************************/
void  lcdkey_disoff(void)
{
  Lcd_WriteCommand(0x80);	//����ʾ
}
/**************************************************************
�������ƣ�cdkey_bgon
�������򿪱���
���룺��
�������
**************************************************************/
void  lcdkey_bgon(void)
{
//  Reset_LCD_BG();
}
/**************************************************************
�������ƣ�cdkey_bgoff
�������رձ���
���룺��
�������
**************************************************************/
void  lcdkey_bgoff(void)		//�رձ���
{
//  Set_LCD_BG();
}


/**************************************************************
�������ƣ�lcdkey_disdata
��������ָ��λ����ʾ����
���룺��
�����addr   д����ʵ��λ��,��ַ���ܴ���15
      str    Ҫд������ݵ�ַ
	  number Ҫд����ֽ���
**************************************************************/
//����HT1621B datasheet��д����ģʽ����Ҫ�ȷ���������������"101"
//DAT��������WR�������ر�HT1621B����	QZ
//д���ݵ�HT1621B��RAM�����ʽ:
// 1 0 1 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3
//����datasheet��HT1621B��32X4bit��RAM,ʹ������д����ģʽʱ,ÿд4Bit,addr��1,����u8��*strʱ
//һ���ֽ�װ��������RAM����,����ԭ��ͼ,RAM��ʼ��ַΪ12,��ֹ��ַΪ31,���Թ�20��RAM,str[]�������ܴ���10,number���ܴ���10
void  lcdkey_disdata(u8  addr, u8*  str, u8  number)
{
u8 l,m,n;
   Reset_LCD_Stb();		//CS0  QZ
   delay(1);  
   Reset_LCD_Clk();		//WR0  QZ	
   Set_LCD_Data();		//DAT1 QZ
   delay(1);  
   Set_LCD_Clk();		//WR1	QZ	����'1'
   delay(1); 
   Reset_LCD_Clk();		//WR0	QZ
   Reset_LCD_Data();	//DA0	QZ
   delay(1);  
   Set_LCD_Clk();		//WR1	QZ	����'0'
   delay(1); 
   Reset_LCD_Clk();		//WR0	QZ
   Set_LCD_Data();		//DA1	QZ
   delay(1);  
   Set_LCD_Clk();		//WR0	QZ	����'1'
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
	  addr *=2;					//addr�����ƶ�һλ,������6��
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
	       n /=2;					//n�����ƶ�һλ			
	       delay(1);
	       Set_LCD_Clk();		//WR1
	       delay(1);  
	   }
   }
   Set_LCD_Data();
   Set_LCD_Stb();
}
/************************************************************
��ʾԤԼͼ��
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
	 	if((Dis_Preengage&0x1) != 0)  //��ʾ����1
		{
		 	//Dis_Data[8] |= 0x44;
		 	Dis_Data[1] |= 0x44;
		}
	 	if((Dis_Preengage&0x2) != 0)  //��ʾ����2
		{
		 	//Dis_Data[8] |= 0x22;
		 	Dis_Data[1] |= 0x22;
		}
	 	if((Dis_Preengage&0x4) != 0)  //��ʾ����3
		{
		 	//Dis_Data[8] |= 0x9;
		 	Dis_Data[1] |= 0x90;
		}
	 	if((Dis_Preengage&0x8) != 0)  //��ʾ����4
		{
		 	//Dis_Data[8] |= 0x90;
		 	Dis_Data[1] |= 0x9;
		}
	 	if((Dis_Preengage&0x10) != 0)  //��ʾ����5
		{
		 	//Dis_Data[9] |= 0x11;
		 	Dis_Data[0] |= 0x11;
		}
	 	if((Dis_Preengage&0x20) != 0)  //��ʾ����6
		{
		 	//Dis_Data[9] |= 0x88;
		 	Dis_Data[0] |= 0x88;
		}
	 	if((Dis_Preengage&0x40) != 0)  //��ʾ����7
		{
		 	//Dis_Data[9] |= 0x22;
		 	Dis_Data[0] |= 0x22;
		}	  
	 	if((Dis_Preengage&0x80) != 0)  //ÿ��
		{
		 	//Dis_Data[9] |= 0x22;
		 	Dis_Data[0] |= 0x44;
		}
	}
  } 
  
}

/************************************************************
�ر�ԤԼͼ��
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
��ʾСʱ
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
�ر�Сʱ
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
��ʾ����
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
�رշ���
***********************************************************/
void dis_disMinute(void)
{
   // Dis_Data[3] &= 0x10;
	//Dis_Data[2] &= 0x10;
   Dis_Data[4]&=0x80;
   Dis_Data[6]&=0x80;
}
/***********************************************************
��ʾ����
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
�ر�����
***********************************************************/
void dis_disWeek(void)
{
    //Dis_Data[1] &= 0x10;
    Dis_Data[5] &= 0x10;
}
/***********************************************************
��ʾ�ٶ�ͼ��
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
�ر��ٶ�ͼ��
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
��ʾ��ص���
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
					case 1:					//С�ڵ���25%
						charge_glint=1;
					break;
					case 2:					//����25%��С�ڵ���50%
						charge_glint=2;
					break;
					case 3:					//����50%��С�ڵ���75%
						charge_glint=3;
					break;
					case 4:					//����75%��С��100%
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
					case 1:		//С�ڵ���25%
						Dis_Data[8]&=0xC3;
						Dis_Data[8]|=0xE3;
					break;
					case 2:		//����25%��С�ڵ���50%
						Dis_Data[8]&=0xC3;
						Dis_Data[8]|=0xF3;
					break;
					case 3:		//����50%��С�ڵ���75%
						Dis_Data[8]&=0xC3;
						Dis_Data[8]|=0xFB;
					break;
					case 4:		//����75%��С��90%
					case 5:		//���ڵ���90%��С�ڵ���100%
						Dis_Data[8]&=0xC3;
						Dis_Data[8]|=0xFF;
					break;
				}
		}
}
/***********************************************************
�رյ�ص���
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
���ܣ������ʾ����
***********************************************************/
void clr_display(void)
{
Dis_On 			= 	false;      //��ʾ��־��
Dis_ErrOn 		= 	false ;    	//����ͼ����
Dis_HourOn 		= 	false ;		//Сʱ��
Dis_MinuteOn 	= 	false ;		//������
Dis_SecOn		=	false;

Dis_PowerOn 	= 	false ;		//���ͼ����
Dis_PowerGlint	=	false;

Dis_ChargeOn 	= 	false ;		//���ͼ����
      

//qz add 20180703
Dis_AllFlagOn	=	false;
Dis_Laji		=	false;
Dis_GuihuaGlint	=	false;		
Dis_StatusOn	=	false;				//״̬����ʾ��
Clr_Alldisflag();


}
 /*****************************************
 �����ʾ����
 *****************************************/
 void Clr_DisplayData(void)
 {
	u8 i;//ѭ����������	  
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
	
	if(!Dis_H_M_Glint)					//ң��������ʱ�䣬Сʱ��˸
		{
			if(Dis_TimeSet_Sec)			//500msʱ��
			{
				for(i=0;i<7;i++)
					{
						if((Dis_Data0_temp&data1))
							{
								Dis_Data[0]^=(1<<i);			//��˸
							}
		
						if((Dis_Data2_temp&data1))
							{
								Dis_Data[2]^=(1<<i);			//��˸
							}
						data1<<=1;
					}
				Dis_TimeSet_Sec=false;				
			}
			Dis_Data[4]=Dis_Data4_temp;							//���ӳ���
			Dis_Data[6]=Dis_Data6_temp;							//���ӳ���
		}
	else								//ң��������ʱ�䣬������˸
		{
			if(Dis_TimeSet_Sec)			//500msʱ��
			{
				for(i=0;i<7;i++)
					{
						if((Dis_Data4_temp&data1))
							{
								Dis_Data[4]^=(1<<i);			//��˸
							}
		
						if((Dis_Data6_temp&data1))
							{
								Dis_Data[6]^=(1<<i);			//��˸
							}
						data1<<=1;
					}
				Dis_TimeSet_Sec=false;				
			}
			Dis_Data[0]=Dis_Data0_temp;							//Сʱ����
			Dis_Data[2]=Dis_Data2_temp;							//Сʱ����
		}
}

