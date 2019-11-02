//////////////////////////ͷ�ļ�////////////////////////////////////	
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
//#include "systime.h"
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
//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================
////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////	  
void Init_Tele(void); 
void Do_Tele(void);
///////////////////////˽�к���////////////////////////////////////	
///////////////////////����ʵ��////////////////////////////////////
/******************************************************************
���ܣ���ʼ��ң�س���
******************************************************************/
void Init_Tele(void)
{
    /******��ʼ����ʾ***********/
    clr_display();
    Dis_On = true;            //��ʾ��־��
    Dis_PrOn = true; 			//ԤԼ������
    Dis_HourOn = true; 		//Сʱ��
    Dis_MinuteOn = true; 		//������
    Dis_ColOn = true; 		//ʱ�����
    Dis_WeekOn = true; 		//������
    Dis_SpeedOn = true; 		//�ٶ���
    Dis_PowerOn = true; 		//���ͼ����
    Dis_ChargeOn = false ;		//���ͼ����
		Dis_ColGlint = true ;	    //��ʾʱ���Ϊ��˸
		Dis_SpeedGo = true; 
		
		/******��ʼ�����õ�ֵ********************/
//////		mode.mode = TELE;
//////		clr_ram();
//////	
//////	ReInitAd();

////////    Enable_Sweep();

//////	Enable_earth();
//////	Enable_wall();
//////	enable_hwincept();				//���������յ�Դ

//////		Reset_Speed_Send();  			//������ⷢ�� 
//////		gbv_action_m_fangxiang = false;  
////////	 
//////	Display_Real();
//////	Init_Action();
//////	WriteWorkState();
//////		init_key();
		
	mode.mode = TELE;
	mode.bump = 0;
	mode.step = 0;	
	mode.action = 0;
	
	clr_ram();
//	ReInitAd();

	Enable_Sweep();

	Enable_earth();
	Enable_wall();
//	enable_hwincept();		//���������յ�Դ
//	Reset_Speed_Send();  	//������ⷢ�� 
	gbv_action_m_fangxiang = false;  
	 
	Display_Real();
	Init_Action();
	WriteWorkState();

init_wallearth();

	r_rap.State = 0x00;
}










/*******************************************************************
ң����ң�ع�������
*******************************************************************/
void Do_Tele(void)
{
//  u32 m;

  //////��ص�ѹ��ʱ�����Զ��س�
  if(battery_voltage < LOWPOWER)
		{
				if(battery_voltage < NOPOWER)
					{
							Enable_Beep_number(8); 
							giv_sys_err = 18;
							Init_Err();
					}
				else
					{				
							Enable_Beep_number(6); 
							Init_Docking() ;
					}
				return;
		}

		
		
  //���ʼ����ʱ�ж��������Ƿ���أ������˳�ɨ��ģʽ
  if((giv_sys_time - mode.time) < 5000)
			{
//					if((lidi.key == 0)||(lidi.key == 0))	//QZ:MASK
				if((l_lidi.key==0)||(r_lidi.key==0))		//QZ:ADD
				{
						Init_Cease();
					return ;
				}
				
			}
  if((giv_sys_time - mode.time) < 100)
			{
////////////					if(CheckMDustMotor())
////////////				{
////////////						return;
////////////				}
				if(gsv_yaokong.key == YAOKONG_BACK)
					{
						mode.step = 3;
					}
			}
			
			
  ///������������������е�����һ��������
////  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
////			{
////					key1.sign = 1;
////					key2.sign = 1;
////					key3.sign = 1;
////					Init_Cease();	 
////					Enable_Beep();
////					return;
////			}
  
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
  
  
  
  //////��ʾ��ʾ��������
  //Display_Real();
  Io_Driver();	 
  //Dis_Power = Get_Dispower();



	//ϵͳ�д���
	if(giv_sys_err != 0)
			{
				Init_Err();
				return ;
			}
  if(gsv_yaokong.effect == 1) /////��ң������������
				{
					gsv_yaokong.sign = 0;
					switch(	gsv_yaokong.key)
					{
							case YAOKONG_ESC:
								Init_Cease();	
							Enable_Beep();
								return ;
						case YAOKONG_AUTO_THU:
								Init_Sweep();
							Enable_Beep();	  
								return ;
						case YAOKONG_SPOT_FRI:	   
								Init_Spot();
							Enable_Beep();
								return ;
						case YAOKONG_DOCKING_SUN:  
								Init_Docking();
							Enable_Beep();
								return ;
								case YAOKONG_V_SAT:	
									Change_Speed();
							Enable_Beep();
							break;
							case YAOKONG_UV_WED:	
									Turn_UV();
									Enable_Sweep();
									Enable_Beep();
							break ;
								
							case YAOKONG_LEFT:
									if(mode.step != 1)
											{		   
													stop_rap();
													mode.step = 1;
											}
									break;
											
							case YAOKONG_RIGHT:	   
									if(mode.step != 2)
											{		   
													stop_rap();
													mode.step = 2;
											}
									break;
											
							case YAOKONG_FRONT:		 
									if(mode.step != 0)
										{		   
												stop_rap();
												mode.step = 0;
										}
										break;
										
							case YAOKONG_BACK:	   
									if(mode.step != 3)
											{		   
													stop_rap();
													mode.step = 3;
											}
									break;
					}
				 }
   else
			 {
		////////////       if((mode.step == 3))
		////////////	   {
		////////////	     Init_Cease();
		////////////		   return;
		////////////	   }
		////////////	   else
		////////////	   {
		////////////	      if(mode.step != 0)
		////////////		  {
		////////////		      stop_rap();
		////////////		      mode.step = 0;
		////////////		  }
		////////////	   }
		////////////		 
				 
				 
				if((mode.step == 3))		//	û��ң�ذ������ں����˶�״̬   
						 {
								Init_Cease();
								return;
						 }
		////			 else if((mode.step == 0))
		////				 {
		////						why_Stop = 0x87;
		////						Init_Cease();
		////						return;
		////				 }
					 else			
						 {
								if(mode.step == 3)
									{
											stop_rap();
											mode.step = 0xF0;
									}
						 }
						
			 }
			 	 

  //�����ֵ���
//  comm_rap();
	ACC_DEC_Curve();			  
			 
  //��ң�صĹ��������ϰ��ź�
  //////m=read_bump();
	read_bump();		//	��ȡ��ײ��Ϣ
	switch(mode.bump)
			{
					case 0:		
						break;
					//----------------------
					default:
							if(do_action(4,500))
									{
										Init_Cease();	
									}
						return;
									
									
									
									
					case 5:	
							if(do_action(4,300))
									{
										Init_Cease();	
									}
							if (l_bump.key == 0)
								{
						//			Init_Cease();
								}
						return;
					case 6:	
							if(do_action(4,300))
									{
										Init_Cease();	
									}
							if (r_bump.key == 0)
								{
							//		Init_Cease();
								}
						return;

					case 99:	//	����ֹͣ
							Enable_Beep_number(3);
							Init_Cease();	
						return;
		}
		
		
		
		
	Speed = 2400;

		
		
		
   //�Ը��������Ķ���
   switch (mode.step)
		 {
				case 0:
						if(do_action(3,(uint32_t)(1 * METER_PLUS)) == 1)
								{
										mode.step = 0xF0;
								}
								

								
								
						break;
				case 1:				   
//////						if(do_action(1,	(uint32_t)(45 * Angle_1)) == 1)
//////								{
//////										mode.step = 0xF0;
//////								}
								
									   
						if(do_action(7,	(uint32_t)(360 * DuLun_Angle_1)) == 1)
								{
										mode.step = 0xF0;
								}
						break;
				case 2:					
//////						if(do_action(2,	(uint32_t)(45 * Angle_1)) == 1)
//////								{
//////										mode.step = 0xF0;
//////								}	   
				
						if(do_action(8,	(uint32_t)(360 * DuLun_Angle_1)) == 1)
								{
										mode.step = 0xF0;
								}
						break;
				case 3:			  
						if(do_action(4,(uint32_t)(1 * METER_PLUS)) == 1)
								{
										mode.step = 0xF0;
								}
							
		
	////					flo1 = GGPM01_Angle - Start_Aangle;
	  
	  
////						if(flo1 > 0.2)
////								{
////										mode.step = mode.step;
////////										mode.step = 0xF0;
////////										Init_Cease();	
////									Start_Aangle = GGPM01_Angle;
////									Enable_Beep_number(1);
////								}
////	
////						if(flo1 < -0.2)
////								{
////										mode.step = mode.step;
////									Start_Aangle = GGPM01_Angle;
////									Enable_Beep_number(1);
////								}
								
						break;
				
				case 0xF0:		//	�ȴ�	  
						mode.step = 0xF0;
						break;
				
				
				
				default:
					mode.step = 0;	      	   	    
		 } 

}































