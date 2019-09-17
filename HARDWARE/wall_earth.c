////////////////////////ͷ�ļ�////////////////////////////////////
#include "AAA-include.h"
////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���////////////////////////////////// 
WALL_DATA  e_l,e_r,e_m,w_l,w_lm,w_r,w_rm,w_m;//e_lm,e_rm,
#define W_MM_ENABLE 1

#ifdef W_MM_ENABLE
WALL_DATA w_mm;
#endif

bool wall_earth_time;
bool action_wall_time;	//qz add 20180717
bool WallSensitivity;   //ǽ���ź������ȣ�Ϊfalseʱ�����ȵͣ�Ϊtureʱ�����ȸ�
///////////////////////˽�б���////////////////////////////////////	  
bool action_wall;     //������ǽ���־
bool action_earth;    //������ؼ��־
bool check_earth_time=false;	//qz add 20180814
bool check_wall_time=false;


//////////////////////������//////////////////////////////////////
/****************************************************************
���ܣ���ʼ��ǽ��͵ؼ������
���룺��
�������
****************************************************************/
void init_wallearth(void)
{
  action_wall = false;             //
  action_earth = false;            //��ֹ��ȡ�ؼ�ǽ�������
  
  /*************���õؼ��ǽ���Ĭ��״̬************/
  e_l.sign=NEAR;
  e_r.sign=NEAR;
  e_m.sign=NEAR;
  w_l.sign=FARN;
  w_lm.sign=FARN;
  w_r.sign=FARN;
  w_rm.sign=FARN;
  w_m.sign=FARN;

#ifdef W_MM_ENABLE
  w_mm.sign=FARN;	//QZ ADD
  w_mm.on=1;
  w_mm.high_sign=FARN;
#endif
  
  w_l.on = 1;
  w_lm.on = 1;
  w_r.on = 1;
  w_rm.on = 1;
  w_m.on = 1;
  					 
  w_l.high_sign=FARN;
  w_lm.high_sign=FARN;
  w_r.high_sign=FARN;
  w_rm.high_sign=FARN;
  w_m.high_sign=FARN;
  
////////////  /*************���õؼ��ǽ����ż���ѹֵ*/


	
  e_l.low_door		= E_LOWDOOR;
  e_m.low_door		= E_LOWDOOR;
  e_r.low_door		= E_LOWDOOR;

	w_l.low_door=600;	//QZ:��������
	w_lm.low_door=600;//200;
	w_rm.low_door=600;//300;
	w_m.low_door=600;//300;
	w_r.low_door=600;

#ifdef W_MM_ENABLE
	w_mm.low_door=0x100;
	w_mm.high_door=0x50;
#endif

	w_l.high_door=300;	//qz add 20180919
	w_m.high_door=300;
	w_r.high_door=300;
	w_rm.high_door=300;
	w_lm.high_door=300;

	w_l.bias=0xFFFF;
	w_lm.bias=0xFFFF;
	w_m.bias=0xFFFF;
	w_rm.bias=0xFFFF;
	w_r.bias=0xFFFF;

	//qz add 20180504
	w_l.check_time=15;
	w_r.check_time=15;
	w_m.check_time=15;
	w_mm.check_time=15;
	w_lm.check_time=15;
	w_rm.check_time=15;

	e_l.check_time=3;
	e_m.check_time=3;
	e_r.check_time=3;
	//qz add end

  WallSensitivity = false;	

#ifdef DEBUG_INIT
	TRACE("Wall Earth init OK!\r\n");
#endif
}
/*****************************************
����: ��ʱ��ȡ�ؼ��ǽ�������,1ms��һ��
����: ��
�������
*****************************************/
void read_wallearth(void)
{
/*****************************************

��ʱ����
	
*****************************************/	 
	static bool ligth = false;
	if( wall_earth_time == false)
	{
		return;
	}
	wall_earth_time = false;
//-----------------------------
/*****************************************

��ȡ���ź�
	
*****************************************/	
	if(ligth == false)		  //Ϩ�Ƶ��ź�
			{
				if(action_earth != false)							//	���Ҫ��ؼ��
						{
							e_l.darkness	= Grand_Sensor_AD(E_L);
							e_m.darkness = Grand_Sensor_AD(E_M);
							e_r.darkness	= Grand_Sensor_AD(E_R);
							Set_Earth_Send();
						}
						
				if(action_wall != false)						//	���Ҫ��ǿ���
						{

							w_l.darkness	= Wall_account_current(W_L);   
							w_lm.darkness = Wall_account_current(W_LM);  
							w_m.darkness	= Wall_account_current(W_M);  
							w_rm.darkness = Wall_account_current(W_RM);  
							w_r.darkness	= Wall_account_current(W_R);
							Set_Wall_Send();
						} 
				ligth = true;
			}
	else
/*****************************************

��ȡ���ź�
	
*****************************************/	   
			{
				ligth = false;			   
				if(action_earth != false)													//	���Ҫ��ؼ��
						{
							e_l.brightness	= Grand_Sensor_AD(E_L);
							e_m.brightness = Grand_Sensor_AD(E_M);
							e_r.brightness	= Grand_Sensor_AD(E_R);
							check_earth( &e_l,		e_l.difference);   
							check_earth( &e_m,		e_m.difference); 
							check_earth( &e_r,		e_r.difference); 
						}
						
		//		uint16 Wall_account_current (u8 channel)
				if(action_wall != false)													//	���Ҫ��ǿ���
						{
										
							w_l.brightness	= Wall_account_current(W_L);
//							w_l.brightness	+= e_r.difference / 30; 		//	��ΪADת��	//qz mask 20180202
							
							w_lm.brightness = Wall_account_current(W_LM);  
							w_m.brightness	= Wall_account_current(W_M);  
							w_rm.brightness = Wall_account_current(W_RM);  
							w_r.brightness	= Wall_account_current(W_R);
							
							
							check_Wall	(&w_l,		w_l.difference);	
							check_Wall	(&w_lm, 	w_lm.difference); 
							check_Wall	(&w_m,		w_m.difference); 
							check_Wall	(&w_rm, 	w_rm.difference); 
							check_Wall	(&w_r,		w_r.difference);	
							
							check_near	(&w_l); 
							check_near	(&w_rm); 
							check_near	(&w_r); 
							check_near	(&w_lm);
							check_near	(&w_m); 

							
							w_r.dis=Return_Distance((w_r.difference>w_r.bias)?(w_r.difference-w_r.bias):0);
							if(w_r.dis>=w_r.max_dis)
								w_r.max_dis=w_r.dis;
							w_rm.dis=Return_Distance((w_rm.difference>w_rm.bias)?(w_rm.difference-w_rm.bias):0);
							if(w_rm.dis>=w_rm.max_dis)
								w_rm.max_dis=w_rm.dis;
							w_m.dis=Return_Distance((w_m.difference>w_m.bias)?(w_m.difference-w_m.bias):0);
							if(w_m.dis>=w_m.max_dis)
								w_m.max_dis=w_m.dis;
							w_lm.dis=Return_Distance((w_lm.difference>w_lm.bias)?(w_lm.difference-w_lm.bias):0);
							if(w_lm.dis>=w_lm.max_dis)
								w_lm.max_dis=w_lm.dis;
							w_l.dis=Return_Distance((w_l.difference>w_l.bias)?(w_l.difference-w_l.bias):0);
							if(w_l.dis>=w_l.max_dis)
								w_l.max_dis=w_l.dis;


							
						}
				Reset_Wall_Send();				//�رշ���
				Reset_Earth_Send(); 			//�رշ���
			}			
}



void read_wallearth_my(void)
{
/*****************************************

��ʱ����
  
*****************************************/   
	static bool light_earth = false;
	static bool light_wall=false;
	if( wall_earth_time == false)
    {
      return;
    }
  	wall_earth_time = false;
//-----------------------------
	if(action_earth)
		{
			if(!light_earth)
			{
				e_l.darkness 	= Grand_Sensor_AD(E_L);
				e_m.darkness = Grand_Sensor_AD(E_M);
				e_r.darkness 	= Grand_Sensor_AD(E_R);
				Set_Earth_Send();
				light_earth=true;
			}
			else
			{
				e_l.brightness 	= Grand_Sensor_AD(E_L);
				e_m.brightness = Grand_Sensor_AD(E_M);
				e_r.brightness 	= Grand_Sensor_AD(E_R);
				check_earth( &e_l,		e_l.difference);   
				check_earth( &e_m,		e_m.difference); 
				check_earth( &e_r,		e_r.difference); 
				light_earth=false;
				Reset_Earth_Send();
			}
		}

	if(action_wall)
		{
			if(!light_wall)
				{
					w_l.darkness 	= Wall_account_current(W_L);   
					w_lm.darkness = Wall_account_current(W_LM);  
					w_m.darkness 	= Wall_account_current(W_M);  
					w_rm.darkness = Wall_account_current(W_RM);  
					w_r.darkness 	= Wall_account_current(W_R);
					Set_Wall_Send();
					light_wall=true;
				}
			else
				{
					w_l.brightness	= Wall_account_current(W_L);
					w_lm.brightness = Wall_account_current(W_LM);  
					w_m.brightness	= Wall_account_current(W_M);  
					w_rm.brightness = Wall_account_current(W_RM);  
					w_r.brightness	= Wall_account_current(W_R);
					
					check_Wall	(&w_l,		w_l.difference);	
					check_Wall	(&w_lm, 	w_lm.difference); 
					check_Wall	(&w_m,		w_m.difference); 
					check_Wall	(&w_rm, 	w_rm.difference); 
					check_Wall	(&w_r,		w_r.difference);	
					
					check_near	(&w_l); 
					check_near	(&w_rm); 
					check_near	(&w_r); 
					check_near	(&w_lm);
					check_near	(&w_m); 	
					
					w_r.dis=Return_Distance((w_r.difference>w_r.bias)?(w_r.difference-w_r.bias):0);
					if(w_r.dis>=w_r.max_dis)
						w_r.max_dis=w_r.dis;
					w_rm.dis=Return_Distance((w_rm.difference>w_rm.bias)?(w_rm.difference-w_rm.bias):0);
					if(w_rm.dis>=w_rm.max_dis)
						w_rm.max_dis=w_rm.dis;
					w_m.dis=Return_Distance((w_m.difference>w_m.bias)?(w_m.difference-w_m.bias):0);
					if(w_m.dis>=w_m.max_dis)
						w_m.max_dis=w_m.dis;
					w_lm.dis=Return_Distance((w_lm.difference>w_lm.bias)?(w_lm.difference-w_lm.bias):0);
					if(w_lm.dis>=w_lm.max_dis)
						w_lm.max_dis=w_lm.dis;
					w_l.dis=Return_Distance((w_l.difference>w_l.bias)?(w_l.difference-w_l.bias):0);
					if(w_l.dis>=w_l.max_dis)
						w_l.max_dis=w_l.dis;

					if(action_wall_time)
						{
							light_wall=false;
							action_wall_time=false;
							Reset_Wall_Send();
						}
					}
							
		} 
}
/****************************************************
�������� check_earth
˵�����жϵؼ��ź�
���룺   WALL_DATA *earth         �ؼ�����ָ��
�����   �ı�  	earth  �ڵ�����
���أ�   ��	 
****************************************************/
void  check_earth( WALL_DATA *earth,u16 difference)
{
u16	in16;
//u8	in8;
u32	hehe;
#if 0		//shftemp			
	if(earth->darkness > (earth->brightness + difference/30))
			{
////////////					earth->difference = earth->darkness - (earth->brightness + difference/30);
				in16 = earth->darkness - (earth->brightness + difference/30);
#else
	if(earth->darkness > (earth->brightness ))
			{
////////////					earth->difference = earth->darkness - (earth->brightness + difference/30);
				in16 = earth->darkness - (earth->brightness );

#endif
			}
	else
			{
////////////					earth->difference =  0;
				in16 = 0;
			}
			
			
	//	in16 = earth->darkness - earth->brightness;
#if 0			
		earth->dif[0] = earth->dif[1];
		earth->dif[1] = earth->dif[2];
		earth->dif[2] = earth->dif[3];
		earth->dif[3] = earth->dif[4];
		earth->dif[4] = earth->dif[5];
		earth->dif[5] = earth->dif[6];
		earth->dif[6] = earth->dif[7];
		earth->dif[7] = in16;
			
		hehe = earth->dif[0] + earth->dif[1] + earth->dif[2] + earth->dif[3] + earth->dif[4] + earth->dif[5] + earth->dif[6] + earth->dif[7];
			
		earth->difference =  hehe / 8;
#else
		earth->dif[0]=earth->dif[1];
		earth->dif[1]=earth->dif[2];
		earth->dif[2]=earth->dif[3];
		earth->dif[3]=earth->dif[4];
		earth->dif[4]=in16;
		hehe=earth->dif[0]+earth->dif[1]+earth->dif[2]+earth->dif[3]+earth->dif[4];
		earth->difference=hehe/5;
#endif			
			
	if( earth->difference > earth->low_door) //����ʱ�źŴ����趨ֵ
			{
					earth->near++;
					earth->farn = 0;
					if(earth->near > earth->check_time)
						{
								earth->sign = NEAR;
								earth->near = 15;
						}
			}  
	else                           //С���趨ֵ
			{
					earth->near = 0;
					earth->farn++;
					if(earth->farn > earth->check_time)
						{
								earth->sign = FARN;
								earth->farn = 15;
						}
			} 
}



void  check_Wall( WALL_DATA *earth,u16 difference)
{
u16	in16;
//u8	in8;
u32	hehe;
  
/*****************************************

���ֵ
  
*****************************************/   
  
#if  0		//shftemp			
	if(earth->darkness > (earth->brightness + difference/30))
			{
////////////					earth->difference = earth->darkness - (earth->brightness + difference/30);
				in16 = earth->darkness - (earth->brightness + difference/30);
#else
	if(earth->darkness > (earth->brightness ))
			{
////////////					earth->difference = earth->darkness - (earth->brightness + difference/30);
				in16 = earth->darkness - (earth->brightness );

#endif	
		//		if(earth->darkness > (earth->brightness + difference/30))
			//			{
			////////////					earth->difference = earth->darkness - (earth->brightness + difference/30);
			//				in16 = earth->darkness - (earth->brightness + difference/30);

			}
	else
			{
////////////					earth->difference =  0;
				in16 = 0;
			}
			
			
	//	in16 = earth->darkness - earth->brightness;
		if(earth->bias>in16)
			earth->bias=in16;
/*****************************************

��ƽ��
  
*****************************************/ 			
		earth->dif[0] = earth->dif[1];
		earth->dif[1] = earth->dif[2];
		earth->dif[2] = earth->dif[3];
		earth->dif[3] = earth->dif[4];
		earth->dif[4] = earth->dif[5];
		earth->dif[5] = earth->dif[6];
		earth->dif[6] = earth->dif[7];
		earth->dif[7] = in16;
			
		hehe = earth->dif[0] + earth->dif[1] + earth->dif[2] + earth->dif[3] + earth->dif[4] + earth->dif[5] + earth->dif[6] + earth->dif[7];
			
		earth->difference =  hehe / 8;

		//qz add
//		if(earth->bias>earth->difference)
//			earth->bias=earth->difference;
		//qz add end
			
/*****************************************

�Ƚ�
  
*****************************************/ 			
	if( earth->difference > earth->low_door) //����ʱ�źŴ����趨ֵ
			{
					earth->near++;
					earth->farn = 0;
					if(earth->near > 10)
						{
								earth->sign = NEAR;
								earth->near = 10;
								
						}
			}  
	else                           //С���趨ֵ
			{
					earth->near = 0;
					earth->farn++;
					if(earth->farn > 10)
						{
								earth->sign = FARN;
								earth->farn = 10;
						}
			}

}

/****************************************************
�������� check_earth
˵�����жϵؼ��ź�
���룺   WALL_DATA *earth         �ؼ�����ָ��
�����   �ı�  	earth  �ڵ�����
���أ�   ��	 
****************************************************/
void  check_near( WALL_DATA *earth)
{
	if( earth->difference > earth->high_door) //����ʱ�źŴ����趨ֵ
		{
			earth->high_near++;
			earth->high_farn = 0;
			if(earth->high_near > 8)
					{
							earth->high_sign = NEAR;
							earth->high_near = 10;
					}
    }  
	else                           //С���趨ֵ
		{
			earth->high_near = 0;
			earth->high_farn++;
			if(earth->high_farn > 8)
					{
							earth->high_sign = FARN;
							earth->high_farn = 10;
							earth->on = 1;
					}
		} 
}
/****************************************************
���ܣ���������ؼ�
���룺��
�������
****************************************************/
void Enable_earth(void)
{
action_earth = true;
} 
/****************************************************
���ܣ��������ǽ��
���룺��
�������
****************************************************/
void Enable_wall(void)
{
action_wall = true;
}
/****************************************************
���ܣ���ֹ�����ؼ�
���룺��
�������
****************************************************/
void Disable_earth(void)
{
  action_earth = false;
  e_l.sign=NEAR;
  e_r.sign=NEAR;
  e_m.sign=NEAR;
}
/****************************************************
���ܣ���ֹ����ǽ��
���룺��
�������
****************************************************/
void Disable_wall(void)
{
action_wall = false;
  w_l.sign=FARN;
  w_lm.sign=FARN;
  w_r.sign=FARN;
  w_rm.sign=FARN;
  w_m.sign=FARN;

    				 
  w_l.high_sign=FARN;
  w_lm.high_sign=FARN;
  w_r.high_sign=FARN;
  w_rm.high_sign=FARN;
  w_m.high_sign=FARN;

#ifdef W_MM_ENABLE
w_mm.sign=FARN;
w_mm.high_sign=FARN;
#endif

}





/****************************************************
���ܣ���ǽ�������������Ϊ��
���룺��
�������
****************************************************/
void Wall_SensitivityLow(void)		//SWEEPʹ��
{
    if(WallSensitivity != false)
	{
	    WallSensitivity = false;	
	
		
////////	������û�д���
//////w_l.low_door		= 650;
//////w_lm.low_door		= 400;
//////w_m.low_door		= 900;
//////w_rm.low_door 	= 6000;
//////w_r.low_door 		= 650;
//////	
		
		
		
		
		
	//LD-002
	w_l.low_door		= 650;
  w_lm.low_door		= 700;
  w_m.low_door		= 200;
  w_rm.low_door 	= 900;
  w_r.low_door 		= 650;
		

	}
}  
/****************************************************
���ܣ���ǽ�������������Ϊ��
���룺��
�������
****************************************************/
void Wall_SensitivityHigh(void)	//SWEEPʹ��
{
    if(WallSensitivity == false)
	{
	    WallSensitivity = true;	


				
		
	//LD-002
	w_l.low_door		= 650;
  w_lm.low_door		= 700;
  w_m.low_door		= 200;
  w_rm.low_door 	= 900;
  w_r.low_door 		= 650;
	
	
	}

}







//void Wall_Sensitivity_YanQiang(void)
//{
//	if(WallSensitivity == false)
//		{
//				WallSensitivity = true;	

//				w_l.high_door		= 0x02E0;
//				w_lm.high_door	= 0x0200;
//				w_m.high_door		= 0x00F0;
//				w_rm.high_door 	= 0x0200;
//				w_r.high_door 	= 0x02E0;
//				
//				w_l.low_door		= 0x0200;
//				w_lm.low_door		= 0x0100;
//				w_m.low_door		= 0x0040;
//				w_rm.low_door 	= 0x0100;
//				w_r.low_door 		= 0x0200;
//		}
//} 

//qz add 20180814
//��ǽ������ؼ�������
//�ؼ����2ms�ڼ��һ��
void Read_Earth_My(void)
{
	static bool light_earth=false;
	if(!check_earth_time)
		return;
	check_earth_time=false;
	if(action_earth)
		{
			if(!light_earth)
			{
				e_l.darkness 	= Grand_Sensor_AD(E_L);
				e_m.darkness = Grand_Sensor_AD(E_M);
				e_r.darkness 	= Grand_Sensor_AD(E_R);
				Set_Earth_Send();
				light_earth=true;
			}
			else
			{
				e_l.brightness 	= Grand_Sensor_AD(E_L);
				e_m.brightness = Grand_Sensor_AD(E_M);
				e_r.brightness 	= Grand_Sensor_AD(E_R);
				check_earth( &e_l,		e_l.difference);   
				check_earth( &e_m,		e_m.difference); 
				check_earth( &e_r,		e_r.difference); 
				light_earth=false;
				Reset_Earth_Send();
			}
		}
}

//qz add 20180814
//��ǽ������ؼ�������
//ǽ�����5ms�ڼ��һ��
void Read_Wall_My(void)
{
	static bool ligth_wall=false;

	if(!wall_earth_time)
		return;
	wall_earth_time=false;
	if(action_wall)
		{
			if(!ligth_wall)
				{
					w_l.darkness 	= Wall_account_current(W_L);   
					w_lm.darkness = Wall_account_current(W_LM);  
					w_m.darkness 	= Wall_account_current(W_M);  
					w_rm.darkness = Wall_account_current(W_RM);  
					w_r.darkness 	= Wall_account_current(W_R);
					Set_Wall_Send();
					ligth_wall=true;
				}
			else
				{
					w_l.brightness	= Wall_account_current(W_L);
					w_lm.brightness = Wall_account_current(W_LM);  
					w_m.brightness	= Wall_account_current(W_M);  
					w_rm.brightness = Wall_account_current(W_RM);  
					w_r.brightness	= Wall_account_current(W_R);
					
					check_Wall	(&w_l,		w_l.difference);	
					check_Wall	(&w_lm, 	w_lm.difference); 
					check_Wall	(&w_m,		w_m.difference); 
					check_Wall	(&w_rm, 	w_rm.difference); 
					check_Wall	(&w_r,		w_r.difference);	
					
					check_near	(&w_l); 
					check_near	(&w_rm); 
					check_near	(&w_r); 
					check_near	(&w_lm);
					check_near	(&w_m); 	
					
					w_r.dis=Return_Distance((w_r.difference>w_r.bias)?(w_r.difference-w_r.bias):0);
					if(w_r.dis>=w_r.max_dis)
						w_r.max_dis=w_r.dis;
					w_rm.dis=Return_Distance((w_rm.difference>w_rm.bias)?(w_rm.difference-w_rm.bias):0);
					if(w_rm.dis>=w_rm.max_dis)
						w_rm.max_dis=w_rm.dis;
					w_m.dis=Return_Distance((w_m.difference>w_m.bias)?(w_m.difference-w_m.bias):0);
					if(w_m.dis>=w_m.max_dis)
						w_m.max_dis=w_m.dis;
					w_lm.dis=Return_Distance((w_lm.difference>w_lm.bias)?(w_lm.difference-w_lm.bias):0);
					if(w_lm.dis>=w_lm.max_dis)
						w_lm.max_dis=w_lm.dis;
					w_l.dis=Return_Distance((w_l.difference>w_l.bias)?(w_l.difference-w_l.bias):0);
					if(w_l.dis>=w_l.max_dis)
						w_l.max_dis=w_l.dis;

					if(action_wall_time)
						{
							ligth_wall=false;
							action_wall_time=false;
							Reset_Wall_Send();
						}
					}
							
		} 
}
