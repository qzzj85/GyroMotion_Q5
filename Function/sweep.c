////////////////////////ͷ�ļ�////////////////////////////////////
#include "function.h"
#include "hardware.h"
#include "io_driver.h"
#include "key.h"
#include "lcdkey.h"
#include "power.h"
#include "ring.h"
#include "test.h"
#include "A7105.h"
#include "wall_earth.h"
#include "hwincept.h"
#include "ad.h"
#include "pwm.h"
#include "systime.h"
#include "rtc.h"
#include "flash.h"
#include "cease.h" 
#include "sweep.h" 
#include "docking.h"
#include "spot.h"
#include "tele.h"
#include "chargeing.h"
#include "sleep.h"
#include "err.h"

////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////
void Init_Sweep(void);
void Do_Sweep(void);		   
u8 Sweep_YaoKong_Manage(void) ;
///////////////////////˽�к���////////////////////////////////////	
///////////////////////����ʵ��////////////////////////////////////
/*****************************************************************
���ܣ���ʼ���Զ���ɨ
*****************************************************************/
void Init_Sweep(void)
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
	Dis_AuGlint = true;
	
	Dis_SpeedGo = true; 
	/******��ʼ�����õ�ֵ********************/
	gsv_work_mode.work_mode = SWEEP;	 //����ģʽΪ��ɨ
	mode.mode = SWEEP;
	clr_ram();
	
    Enable_Sweep();

	Enable_earth();
	Enable_wall();
//	enable_hwincept();//���������յ�Դ
//	Reset_Speed_Send();  //������ⷢ��  
	gbv_action_m_fangxiang = false; 	//false:��ǰ
	Display_Real();
	Init_Action();
	mode.step = 0xfe;
	WriteWorkState();
//	ReInitAd();
}
/******************************************************************
���ܣ�����ɨʱ�Ĺ���
******************************************************************/
void Do_Sweep(void)
{
 u8 y = 0;//����ײ�ź�ʱ y = 1;����Ϊ0
// u8 z = 0;//��ִ������ɨʱ z = 1,����Ϊ0

   //NVIC_GenerateSystemReset();
  //////��ص�ѹ��ʱ�����Զ��س�
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
	 return;
  }
  //���ʼ����ʱ�ж��������Ƿ���أ������˳�ɨ��ģʽ
  if((giv_sys_time - mode.time) < 5000)
  {
//      if((lidi.key == 0)||(lidi.key == 0))	//qz:MASK
	  if((l_lidi.key==0)||(r_lidi.key==0))	//QZ:ADD
	  {
	      Init_Cease();
		  return ;
	  }
	  
  }
////////////  //�����ɨ����
////////////  if((giv_sys_time - mode.time) < 100)
////////////  { 
////////////      if(CheckMDustMotor())
////////////	  {
////////////	      return;
////////////	  }
////////////  }
  //ϵͳ�д���
  if(giv_sys_err != 0)
  {
	  Init_Err();
	  return ;
  }
  ///ң������������		     
//  if(Sweep_YaoKong_Manage() == 1)
//  {
//      return;
//  }

  ///������������������е�����һ��������
////  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
////  {
////    key1.sign = 1;
////	  key2.sign = 1;
////	  key3.sign = 1;
////	  Init_Cease();	 
////	  Enable_Beep();
////	  return;
////  }
  
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
  Display_Real();
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&rm_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  ////������һЩ��Ҫ�������ж�////////////
  Io_Driver();	 

  ////�ڿ���ǽ��ʱ��������
  if((w_l.high_sign == NEAR)||(w_lm.high_sign == NEAR)||(w_m.high_sign == NEAR)||
     (w_r.high_sign == NEAR)||(w_rm.high_sign == NEAR))
			{
					if((l_rap.rap > 1000)||(r_rap.rap > 1000))
						{
								l_rap.rap = (l_rap.rap*2)/3;
								r_rap.rap = (r_rap.rap*2)/3;
						}
			}
  else
			{
					if((l_rap.rap < Speed)&&(r_rap.rap < Speed)&&(mode.bump == 0))
							{
									l_rap.rap = (l_rap.rap*3)/2;
									r_rap.rap = (r_rap.rap*3)/2;
							}
			}
  //�ڻس���������ǽ�������������Ϊ��
  if((up_hw.sign != 0) && ((up_hw.data&0x8f)==0))
			{
					Wall_SensitivityHigh();
					mode.angle_wall = 0;
			}
  else
			{
					Wall_SensitivityLow();
			}

  ////��ң����ʱ����ֹͣ����ң�ط����˶�
//  if(YaoKongFangXiang())
//		{
//				return;
//		}
  ////�Ƿ�����Ҫ����������
  if(Action_Protect())
		{
			 return;
		}
  
  if((mode.fangxiang != 0)&&(mode.fangxiang != 1)&&(mode.fangxiang != 2))
		{
				mode.fangxiang = 0;
		}
  /////����������ɨ�ص�·��
//  if(z == 0)
  {
      if(action_bump())
				{
						y=1;
						return;
				} 
  }
///////////////////////////////////������������ɨ��ʱ��·��////////////////////////////
  ////�й���ײ����ײ����·��ֻ����ֱ�ߺ�Z���ζ���
  if(1 == y)	
				{ 
						y = 0;
					if((mode.step==0xfe) || (mode.step==0xff))
						{
								mode.step = 0;
						}
					do
						{
								mode.step++;
							if(22 < mode.step)
							{
									mode.step = 0;
							}
						}
						while(3 != zhulujing[mode.step]);
				} 
  	  
  if(mode.step==0xfe)	  //���ʼ��ʱ��
			{
					if(!do_action(3,METE))
					{
							return;
					}
							mode.step++;
			}
  if(mode.step ==0xff)	 //��ͷ
			{
					if(!do_action(1,angle180))
					{
							return;
					}
					mode.step++;
			}

  //�����������������Z���ζ���������ǽ����ǽ�� 
 /* if((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step])||(21 == zhulujing[mode.step]))
  {
	  mode.ybs_sign = 1;
	  mode.angle_wall=angle720;	  
	  mode.long_wall = 0;
  }
  else
  {
      mode.ybs_sign = 0;
  }*/
  if((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step]))
			{
				mode.sum_bump = 0;
				mode.fangxiang = 0;
			}
  if(((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step])||(mode.sum_bump > 10)))
			{
				mode.ybs_sign = 1;
				mode.angle_wall=angle720;	  
				mode.long_wall = 0;
			}
  else
			{
				mode.ybs_sign = 0;
				Enable_wall();
			}
  ////����Z����ʱ������ײ���� 
//  if(21 == zhulujing[mode.step])  
//  {
//      z = 1;
//      if(read_earth())
//      {
//          z = 0;
//          return ;
//      }
//  }
//  else
//  {
//      z = 0;
//  }
	if(do_action(zhulujing[mode.step],zhulujinglong[mode.step]))
			 {
					 mode.step++;
					 if(mode.step > 22)
					 {
							 mode.step=0;
					 }
			 }
}
/******************************************************************
����ɨʱ��ң��������
******************************************************************/
#if 0
u8 Sweep_YaoKong_Manage(void) 
{
	if(gsv_yaokong.sign == 1) /////��ң������������
				{
						gsv_yaokong.sign = 0;
					switch(	gsv_yaokong.key)
					{
							case YAOKONG_ESC:
						case YAOKONG_AUTO_THU:
								Init_Cease();
							Enable_Beep();
								return 1;
						case YAOKONG_SPOT_FRI:	
								Init_Spot();
							Enable_Beep();
								return 1;
						case YAOKONG_DOCKING_SUN: 
								Init_Docking();
							Enable_Beep();
								return 1;
								case YAOKONG_V_SAT:
									Change_Speed();
							Enable_Beep();
									return 0;
							//////����uv����uvָʾȡ��
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
/***************************************************
���ܣ�����ң������ǰ�����Ұ���֮�󣬻�������ң�����ķ����˶�
      ��Ҫ���������ײ
***************************************************/
#if 0
u8 YaoKongFangXiang(void)
{
static u8 piv_yaokong = 0;
static u8 piv_pengzhuang = 0;
u32 m;
    //����ң�ص�ʱ������ײ�źţ���������
	if(piv_yaokong == 1)	//����ң�ض���״̬QZ
				{
						m = read_bump();
							if(mode.bump != 0)        //��������ײ
							{
								if((m != 0))//��ײ�źŲ��Ǻ�����ײ�ź�
								{
									piv_pengzhuang = 1;			//ң����ײ״̬��1 QZ
									if(do_action(4,angle240))	//����һ�ξ���  do_action����1��ʾ����ִ�����  QZ
									{
										mode.step = 0;	
										mode.bump = 0;			//mode.bump���㣬QZ
										mode.step_yaokong = 0;	//mode.step_yaokong���㣬QZ
										piv_pengzhuang = 0;
										piv_yaokong = 0;
										return 0; 	//���˶���ִ����ɣ�return 0  QZ
									}
									return 1;	//���˶���δ��ɣ�return 1  QZ
								}
								if(piv_pengzhuang != 0)
								{
									stop_rap();
									piv_pengzhuang = 0;
									mode.step = 0;
									mode.bump = 0;
									mode.step_yaokong = 0;
									piv_yaokong = 0;
									return 0;
								}
							}
				}
	if((gsv_yaokong.effect == 1)||(gsv_yaokong.sign == 1))	//��ң�ذ����ź�����  QZ
				{
						gsv_yaokong.sign = 0;
						switch(gsv_yaokong.key)
					{
								case YAOKONG_LEFT:	//LEFTң�ذ���  QZ
								if(mode.step_yaokong != 1)
							{		   
									stop_rap();
									mode.step_yaokong = 1;
							}
							break;
								case YAOKONG_RIGHT:	   
								if(mode.step_yaokong != 2)
							{		   
									stop_rap();
									mode.step_yaokong = 2;
							}
							break;
								case YAOKONG_FRONT:		 
								if(mode.step_yaokong != 4)
							{		   
									stop_rap();
									mode.step_yaokong = 4;
							}
							break;
								case YAOKONG_BACK:	   
								if(mode.step_yaokong != 3)
							{		   
									stop_rap();
									mode.step_yaokong = 3;
							}
							break;
						default :		//���ఴ��,��������return 0 piv_yaokong=0 ֱ�ӷ��� QZ
								piv_yaokong = 0;
							return 0;
					}
					if(piv_yaokong == 0)	//LEFT/RIGHT/FRONT/BACK���������֮ǰ����ң��״̬����ֹͣ����������ң��״̬piv_yaokong=1  QZ
					{
							stop_rap();
						piv_yaokong = 1;
					}
				}
	else 
				{		//��ң�ذ����źţ����ǻ���֮ǰ����ң��״̬��QZ
						if((piv_yaokong != 0)&&(mode.step_yaokong != 5))
					{
						stop_rap();
						mode.step_yaokong = 5;
					}
				}
				
				
				
	if(piv_yaokong != 0)
				{
						switch(mode.step_yaokong)
						{
							case 0:
								break;
							case 1:
								do_action(1,angle360);
									break;
							case 2:
								do_action(2,angle360);
								break;
							case 3:
						 //   do_action(4,METE_TWO);
								do_action(4,(uint32_t)(2 * METER_PLUS));
								break;
							case 4:
					//	    do_action(3,METE_TWO);
								do_action(3,(uint32_t)(2 * METER_PLUS));
									break;
							case 5:
								if(do_action(3,angle120))
									{
												piv_yaokong = 0;
										mode.bump = 0;
											mode.step_yaokong = 0;
										mode.step = 0;
								}
						}
				}
	return piv_yaokong;
}
#endif





