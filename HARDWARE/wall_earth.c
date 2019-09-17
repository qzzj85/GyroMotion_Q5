////////////////////////头文件////////////////////////////////////
#include "AAA-include.h"
////////////////////////私有定义//////////////////////////////////
////////////////////////全局变量////////////////////////////////// 
WALL_DATA  e_l,e_r,e_m,w_l,w_lm,w_r,w_rm,w_m;//e_lm,e_rm,
#define W_MM_ENABLE 1

#ifdef W_MM_ENABLE
WALL_DATA w_mm;
#endif

bool wall_earth_time;
bool action_wall_time;	//qz add 20180717
bool WallSensitivity;   //墙检信号灵敏度，为false时灵敏度低，为ture时灵敏度高
///////////////////////私有变量////////////////////////////////////	  
bool action_wall;     //允许检测墙检标志
bool action_earth;    //允许检测地检标志
bool check_earth_time=false;	//qz add 20180814
bool check_wall_time=false;


//////////////////////函数体//////////////////////////////////////
/****************************************************************
功能：初始化墙检和地检的数据
输入：无
输出：无
****************************************************************/
void init_wallearth(void)
{
  action_wall = false;             //
  action_earth = false;            //禁止读取地检墙检的数据
  
  /*************设置地检和墙检的默认状态************/
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
  
////////////  /*************设置地检和墙检的门槛电压值*/


	
  e_l.low_door		= E_LOWDOOR;
  e_m.low_door		= E_LOWDOOR;
  e_r.low_door		= E_LOWDOOR;

	w_l.low_door=600;	//QZ:样机调试
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
功能: 定时读取地检和墙检的数据,1ms读一次
输入: 无
输出：无
*****************************************/
void read_wallearth(void)
{
/*****************************************

定时处理
	
*****************************************/	 
	static bool ligth = false;
	if( wall_earth_time == false)
	{
		return;
	}
	wall_earth_time = false;
//-----------------------------
/*****************************************

读取暗信号
	
*****************************************/	
	if(ligth == false)		  //熄灯的信号
			{
				if(action_earth != false)							//	如果要求地检测
						{
							e_l.darkness	= Grand_Sensor_AD(E_L);
							e_m.darkness = Grand_Sensor_AD(E_M);
							e_r.darkness	= Grand_Sensor_AD(E_R);
							Set_Earth_Send();
						}
						
				if(action_wall != false)						//	如果要求强检测
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

读取亮信号
	
*****************************************/	   
			{
				ligth = false;			   
				if(action_earth != false)													//	如果要求地检测
						{
							e_l.brightness	= Grand_Sensor_AD(E_L);
							e_m.brightness = Grand_Sensor_AD(E_M);
							e_r.brightness	= Grand_Sensor_AD(E_R);
							check_earth( &e_l,		e_l.difference);   
							check_earth( &e_m,		e_m.difference); 
							check_earth( &e_r,		e_r.difference); 
						}
						
		//		uint16 Wall_account_current (u8 channel)
				if(action_wall != false)													//	如果要求强检测
						{
										
							w_l.brightness	= Wall_account_current(W_L);
//							w_l.brightness	+= e_r.difference / 30; 		//	因为AD转换	//qz mask 20180202
							
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
				Reset_Wall_Send();				//关闭发射
				Reset_Earth_Send(); 			//关闭发射
			}			
}



void read_wallearth_my(void)
{
/*****************************************

定时处理
  
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
函数名： check_earth
说明：判断地检信号
输入：   WALL_DATA *earth         地检数据指针
输出：   改变  	earth  内的数据
返回：   无	 
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
			
	if( earth->difference > earth->low_door) //测试时信号大于设定值
			{
					earth->near++;
					earth->farn = 0;
					if(earth->near > earth->check_time)
						{
								earth->sign = NEAR;
								earth->near = 15;
						}
			}  
	else                           //小于设定值
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

算差值
  
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

求平均
  
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

比较
  
*****************************************/ 			
	if( earth->difference > earth->low_door) //测试时信号大于设定值
			{
					earth->near++;
					earth->farn = 0;
					if(earth->near > 10)
						{
								earth->sign = NEAR;
								earth->near = 10;
								
						}
			}  
	else                           //小于设定值
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
函数名： check_earth
说明：判断地检信号
输入：   WALL_DATA *earth         地检数据指针
输出：   改变  	earth  内的数据
返回：   无	 
****************************************************/
void  check_near( WALL_DATA *earth)
{
	if( earth->difference > earth->high_door) //测试时信号大于设定值
		{
			earth->high_near++;
			earth->high_farn = 0;
			if(earth->high_near > 8)
					{
							earth->high_sign = NEAR;
							earth->high_near = 10;
					}
    }  
	else                           //小于设定值
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
功能：允许测量地检
输入：无
输出：无
****************************************************/
void Enable_earth(void)
{
action_earth = true;
} 
/****************************************************
功能：允许测量墙检
输入：无
输出：无
****************************************************/
void Enable_wall(void)
{
action_wall = true;
}
/****************************************************
功能：禁止测量地检
输入：无
输出：无
****************************************************/
void Disable_earth(void)
{
  action_earth = false;
  e_l.sign=NEAR;
  e_r.sign=NEAR;
  e_m.sign=NEAR;
}
/****************************************************
功能：禁止测量墙检
输入：无
输出：无
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
功能：将墙检的灵敏度设置为高
输入：无
输出：无
****************************************************/
void Wall_SensitivityLow(void)		//SWEEP使用
{
    if(WallSensitivity != false)
	{
	    WallSensitivity = false;	
	
		
////////	空气。没有窗户
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
功能：将墙检的灵敏度设置为高
输入：无
输出：无
****************************************************/
void Wall_SensitivityHigh(void)	//SWEEP使用
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
//将墙检检测与地检检测区分
//地检会在2ms内检测一次
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
//将墙检检测与地检检测区分
//墙检会在5ms内检测一次
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
