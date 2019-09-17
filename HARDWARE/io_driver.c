//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================


#include "io_driver.h"
#include "hardware.h"
#include "systime.h"




////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
bool gbv_action_uv;	/*����uv�Ƶ�״̬�����ⲿģ���ȡ�͸ı�*/
bool gbv_action_s_motor;/*���ñ�ɨ����״̬�����ⲿģ���ȡ�͸ı�*/
bool gbv_action_m_motor;/*������ɨ����״̬�����ⲿģ���ȡ�͸ı�*/ 
bool gbv_action_m_fangxiang;//��ɨ�����˶�����false ��ʾ��ǰ��true��ʾ���
bool gbv_action_dust_motor;	/*���ûҳ������״̬�����ⲿģ���ȡ�͸ı�*/

////////////////////////˽�б���//////////////////////////////////	 
bool pbv_state_uv;/*ָʾ��ǰuv�Ƶ�״̬*/
bool pbv_state_m_motor;/*ָʾ��ǰ��ɨ����״̬*/
bool pbv_state_m_fangxiang;/*ָʾ��ǰ��ɨ����״̬*/

bool 	pbv_state_s_motor;/*ָʾ��ǰ��ɨ����״̬*/
bool 	pbv_state_dust_motor;/*ָʾ��ǰ�ҳ������״̬*/
u32 	piv_beep_time;     /*���������ʱ��*/

bool pbv_beep_on;        ///��������ı�־
u8 piv_beep_number;   //������������
///////////////////////ȫ�ֺ���////////////////////////////////////
///////////////////////˽�к���////////////////////////////////////
///////////////////////����ʵ��////////////////////////////////////
/******************************************************************
���ܣ���ʼ����ɨ�������ҳ��з����������ɨ�����������ɨ��������
      �����������
���룺��
�������
******************************************************************/
void Init_Io_Driver(void)
	{  
			gbv_action_uv = false;
			gbv_action_s_motor = false;
			gbv_action_m_motor = false;
			gbv_action_dust_motor = false;
			gbv_action_m_fangxiang = false;

			Reset_UV();
			pbv_state_uv = false;
			//Reset_S_Motor();
//			stop_bs(); //hzh mask
			pbv_state_s_motor = false;
			Reset_Dust_Motor();
			pbv_state_dust_motor = false;

////////////Reset_M_FPMOS();
////////////Reset_M_ZPMOS();
////////////Set_M_FNMOS();
////////////Set_M_ZNMOS();
			Reset_M_FNMOS();		//	2017-2-10  �޸����
	
	
			pbv_state_m_motor = false;
			pbv_state_m_fangxiang = false;
			//Reset_Beep();    //�رշ�����
			//pbv_beep_on = false;
	}
/*****************************************************************
���ܣ�������ɨ�����ɨ�����
���룺��
�������
˵��������Ҫ������ɨʱ�Ϳ���ѭ������
*****************************************************************/
void Io_Driver(void)
{
    /*********************���������*********************/
    if((gbv_action_uv == true) && (pbv_state_uv == false))
				{
							Set_UV();
							pbv_state_uv = true;
				}
		else if((gbv_action_uv == false) && (pbv_state_uv == true))
				 {
							Reset_UV();
							pbv_state_uv = false;
				 }
	 
    /*********************������ɨ*********************/
    if((gbv_action_s_motor == true) && (pbv_state_s_motor == false))
				{
						//Set_S_Motor();
//						bian_sao_front();
						pbv_state_s_motor = true;
				}
		else if((gbv_action_s_motor == false) && (pbv_state_s_motor == true))
				 {
							 //Reset_S_Motor();
//							 stop_bs(); hzh  mask
							 pbv_state_s_motor = false;
				 }
	
//////////    /*********************������ɨ*********************/
//////////    if((gbv_action_m_motor == true))
//////////    {
//////////	     if(gbv_action_m_fangxiang != pbv_state_m_fangxiang)
//////////					 {
//////////									 Reset_M_FPMOS();
//////////									 Reset_M_ZPMOS();
//////////							 Set_M_FNMOS();
//////////							 Set_M_ZNMOS();
//////////						 DelayMs(1);
//////////						 pbv_state_m_fangxiang = gbv_action_m_fangxiang;
//////////						 pbv_state_m_motor = false;
//////////					 }
//////////			 if(pbv_state_m_motor == false)
//////////			 {
//////////					 if(gbv_action_m_fangxiang == false)
//////////				 {
//////////									 Reset_M_ZNMOS();
//////////							 Set_M_ZPMOS();
//////////				 }
//////////				 else
//////////				 {
//////////									 Reset_M_FNMOS();
//////////							 Set_M_FPMOS();
//////////				 }
//////////							 pbv_state_m_motor = true;
//////////			 }
//////////			}
//////////		else if((gbv_action_m_motor == false) && (pbv_state_m_motor == true))
//////////				 {
//////////							 Reset_M_FPMOS();
//////////							 Reset_M_ZPMOS();
//////////					 Set_M_FNMOS();
//////////					 Set_M_ZNMOS();
//////////							 pbv_state_m_motor = false;
//////////				 }

			//	2017-2-10�޸����		 
			if((gbv_action_m_motor == true))
				{
						Set_M_FNMOS();							//	2017-2-10�޸����
						pbv_state_m_motor = true;
				}
			else if((gbv_action_m_motor == false) && (pbv_state_m_motor == true))
				{
						Reset_M_FNMOS();						//	2017-2-10�޸����
						pbv_state_m_motor = false;
				}
				
				
				
		
    /*********************�����ҳ����*********************/
			if((gbv_action_dust_motor == true) && (pbv_state_dust_motor == false))
					{
							Set_Dust_Motor();
							pbv_state_dust_motor = true;
					}
			else if((gbv_action_dust_motor == false) && (pbv_state_dust_motor == true))
				 {
							 Reset_Dust_Motor();
							 pbv_state_dust_motor = false;
				 }
} 
/**********************************************************
ʹ�ܷ�������0.2��
**********************************************************/
void Enable_Beep_Five(void)
{
   //Set_Beep();
   piv_beep_time = giv_sys_time;
   pbv_beep_on = true;
   piv_beep_number = 10;
}
/**********************************************************
ʹ�ܷ�������0.2��
**********************************************************/
void Enable_Beep(void)
{
   //Set_Beep();
   piv_beep_time = giv_sys_time;
   pbv_beep_on = true;
   piv_beep_number = 1;
}	  
/**********************************************************
ʹ�ܷ�������0.2��
**********************************************************/
void Enable_Beep_number(u8 number)
{
   //Set_Beep();
   piv_beep_time = giv_sys_time;
   pbv_beep_on = true;
   piv_beep_number = number;
}
/**********************************************************
������������
**********************************************************/
void Beep_Driver(void)
{
    if((pbv_beep_on != false)&&(piv_beep_number!=0))
				{
			//		if((giv_sys_time - piv_beep_time) > 2000)
					if((giv_sys_time - piv_beep_time) > 400)
							{
									//Reset_Beep();
									if((giv_sys_time - piv_beep_time) > (400+400))
											{
													piv_beep_time = giv_sys_time;  
													piv_beep_number --;
											}
									if(piv_beep_number == 0)
											pbv_beep_on = false;
							}
					else
								{
										//Set_Beep();
								}
				}
}

