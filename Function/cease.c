////////////////////////ͷ�ļ�////////////////////////////////////

#include "AAA-Include.h"
////////////////////////˽�ж���//////////////////////////////////
#define WAIT_TIME 3000000
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
u8 piv_yaokongstate = 0;	   //������ʱ������״̬,0û�����ã�1����
                               //��ǰʱ�䣬2����ԤԼʱ��

u8 piv_dis_hour;			   //��ʾ��Сʱ��

u8 piv_dis_minute;		   //��ʾ�ķ�����

u8 piv_dis_week;			   //��ʾ��Сʱ��

u8 piv_dis_preengage;	   //��ʾ��ԤԼ����

u8 piv_set_content;         //���õ�����	0Ϊû�����ã�1Ϊ����������2Ϊ����Сʱ��3Ϊ���÷���
                               //4Ϊ��������


///////////////////////����ʵ��////////////////////////////////////
/*****************************************************************
���ܣ��ڴ���ʱ�Ķ���
*****************************************************************/

void Cease_Bump_Action(void)
                               	{
     u8 m=Read_Only_Collide();
	 switch(mode.bump)
	 	{
	 		case BUMP_ONLY_LEFT:
		 		switch(mode.step_bp)
		 			{
		 				case 0:
							Speed=BUMP_BACK_SPEED;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 1:
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							mode.step=0;
		 			}
				break;
			case BUMP_ONLY_RIGHT:
				switch(mode.step_bp)
					{
						case 0:
							Speed=BUMP_BACK_SPEED;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 1:
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							mode.step=0;
					}
				break;
			case BUMP_MID:
				switch(mode.step_bp)
					{
						case 0:
							Speed=BUMP_BACK_SPEED;
							if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
								}
							break;
						case 1:
							mode.bump=0;
							mode.step_bp=0;
							mode.bump_flag=false;
							mode.step=0;
					}
				break;
			default:
				mode.bump=0;
				mode.step_bp=0;
				mode.bump_flag=false;
				break;
	 	}
}

void Do_Cease(void)
{
    /**************�����ң��������*/
#ifdef DC_NOBAT_RUN
  if(((power.charge_dc)|(power.charge_seat))&(!dc_nobat_run))
#else
  if(power.charge_dc|power.charge_seat)
#endif
		{
			 //mode.last_mode=mode.mode;			//qz add 20180201
			 //Init_Chargeing(CHARGEING);

			 //qz add 20180901
			 if(power.charge_seat)
			 	Init_Chargeing(SEAT_CHARGING);
			 if(power.charge_dc)
			 	Init_Chargeing(DC_CHARGING);
			 //qz add end
			 
			 return;
		}
	/////////������ԤԼʱ�䵽����������ɨ��ģʽ/////////////////////

	//�ƻ�����charge��sleep�½���ԤԼʱ���飬���һ��flag��Ȼ����charge��cease�¼�����flag
	#if 0
	if(Check_PreengageTime())
			{
//				 if(Slam_Data.tick_flag)			//�жϵ������Ƿ����������ڹ���״̬	//qz mask 20180420
				 	{
						//qz add 20180420
						Slam_Data.dipan_req_pre=DIPAN_REQ_PREEN;
						//qz add end
						Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
				 	}
				 return;
			}
	#endif
	//ϵͳ�д���
	if(giv_sys_err != 0)
			{
				Init_Err();
				return ;
			}
#ifdef SLEEP_SUBMODE
    /**********���������3������û�в���,��������˯��ģʽ**********/
	if(((giv_sys_time-mode.time)>10000*60)&(!mode.factory))			//qz mask 20181110
	//if(((giv_sys_time-mode.sleep_time)>1800000)&(!mode.factory))		//qz add 20181110
		{
			//Init_Sleep();
			//Init_ShutDown();
			Init_VirtualSleep();
			return;
		}
#endif
	clr_all_hw_effect();
	ACC_DEC_Curve();
//qz mask 20181215
//	find_home=ReadHwSign_My();			//qz mask 20181215
#ifdef REPEAT_DOCK	//qz add 20180901
	if((giv_sys_time-mode.time>40000)&(mode.last_mode==CHARGEING)&(mode.last_sub_mode==SEAT_CHARGING)&(!dc_nobat_run)&(mode.step==0))
		{
			Enable_Speed();
			stop_rap();
			mode.step++;
			mode.Info_Abort=1;
		}

#if 0
	u8 abnormal=Read_Protect();
	Action_Protect_My(abnormal);
	if(mode.abnormity)
		return;

	Cease_Bump_Action();		
	if(mode.bump)
		return;
#endif

	switch(mode.step)
		{
			case 0:
				break;
			case 1:
				Speed=2000;
				if(do_action(4,15*CM_PLUS))
					{
						stop_rap();
						mode.step++;
						mode.Info_Abort=0;
					}
				break;
			case 2:
				Slam_Data.dipan_req_pre=DIPAN_REQ_DOCK;
				Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
				mode.step++;
				break;
			case 3:
				Send_Voice(VOICE_DOCK_START);
				motion1.start_seat=true;
				Init_Docking();
				break;
		}
#endif
}







/******************************************************************
�ڴ���״̬����ʾ��ʼ��
******************************************************************/
void Init_Cease(void)
{ 
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	
	/******��ʼ�����õ�ֵ********************/
	piv_set_content = 0;			//��ʼ����Ϊû������
	piv_yaokongstate = 0;

	/*******��ʼ������Ŀ���***************/
	stop_rap(); //�ر�������
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);
	//Disable_earth();				//�رյؼ�
	Disable_wall();					//�ر�ǽ��
	Enable_earth();
	//Enable_wall();
	enable_hwincept();				//�򿪻س������յ�Դ
	Enable_Speed();					//����״̬���ٶȼ��򿪣���Ϊ�˷�ֹ����CEASEʱ�ر��ٶȼ��ᵼ�¹��������޷����㡣
		
	/****���û����Ĺ���ģʽ******/   
	mode.mode = CEASE; 
	mode.Info_Abort=0;				//��SLAMͨ�ţ�qz add 20180418
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.sub_mode=CEASE;			//QZ ADD
	mode.step=0x00;					//qz add
	mode.status=0;					//qz add 20180625
	mode.time=giv_sys_time;			//qz add 20180703
	mode.init_mode_time=giv_sys_time;	//qz add 20180814
	motion1.start_seat=false;
	motion1.force_dock=false;
		
	WriteWorkState();
	Disable_Free_Skid_Check();		//�ر������ּ��
	//��ʼ����������
	CHECK_STATUS_FLAG=true;			//ʹ���쳣���
	Init_Check_Status();//qz add 20180425

	Slam_Data.dipan_req_pre=1;		//qz mask 20180522��Ĭ��Ϊ�滮��ɨ

#ifdef DEBUG_Enter_Mode
	TRACE("Init Cease Mode Complete!Prepare enter to Cease!\r\n");
#endif
	SLAM_DOCK=false;
	DOCK_SWEEP=false;				//qz add 20180803

#ifdef UV
	Reset_UV();						//�ر�UV
#endif 
	REYBS_TIME=0;					//qz add 20180910,С�س����������رߴ�����0
	Open_Led(1,0,0);
	
}

/*****************************************************************
���ܣ�����ң�����������
******************************************************************/
#if 0
void Cease_YaoKong_Manage(void)
{
	if(gsv_yaokong.sign == 1) /////��ң������������
			{
				gsv_yaokong.sign = 0; 
	//Enable_Beep();
				switch(piv_yaokongstate)
						{
								//����ǰû�а�������ʱ�����
								case 0:
									Cease_No_Key();
								break;

								//ԭ�������趨ʱ���
							case 1:
									Cease_SetTime();
									break;

							//ԭ�������趨ԤԼ��
							case 2:
									Cease_Timing();
									break;
							default :
							piv_yaokongstate = 0;

						}
				giv_cease_time = giv_sys_time;     
			}
}
#endif
/*******************************************************
���ܣ���û��Ԥ�Ȱ��°���ʱ�Ĵ������
*******************************************************/
#if 0
void Cease_No_Key(void)
{
u32 liv_time;
	
    switch(	gsv_yaokong.key)
					{
						//////���µ�Դ������ȡ����
					case YAOKONG_ESC:
							liv_time = giv_sys_time;
							////////�ȴ���Դ��������//////////////////
							do
								{
										A7105_rx();		//
										Beep_Driver();	
										IWDG_ReloadCounter();			   
										if((giv_sys_time - liv_time) > 30000)  //����5��
												{
														PreengageDate = 0;        //���ԭ����ԤԼ����	  
														PreengageTime = 0;
														WritePreengage();	
														Enable_Beep();
														break;			                         
												}
								}
							while(( gsv_yaokong.key == YAOKONG_ESC) &&(gsv_yaokong.effect != 0)) ;	
							//??????��A7105_rx()�У�500ms��û�з�Ӧ.effect�����㣬�Ų���3��,����EXIT0�жϺ����У�����ϴμ�ֵ�������ͬ���Ὣ.effect��1 QZ
							////�жϵ�Դ���Ƿ���ʱ�䳬��3��//////
							if((giv_sys_time - liv_time) < 30000)
									{
											Init_Sleep();        //����˯��ģʽ
									}
							break;	
						
						///////����ԤԼ��������ԤԼ����	 
							case YAOKONG_TIMINE_MON:
								Cease_Lode_Preengage();//��ԤԼ����װ����ʱ������
								piv_set_content = 1;
								piv_yaokongstate = 2;
								break;

						///////����ʱ�䰴������ʱ���趨
							case YAOKONG_SETTIME_TUE:	
								Cease_Lode_Time();
								piv_set_content = 4;
								piv_yaokongstate = 1;
								break;

						//////����uv����uvָʾȡ��   		//		�ر���   �ӱ�
							case YAOKONG_UV_WED:
					//			Turn_UV();
							
									Enable_Beep_number(4);
									Init_Right_YBS();
	
								break;

						/////���¶�����ɨ���붨����ɨģʽ		//�Զ���ɨ??  QZ
							case YAOKONG_AUTO_THU:
								Init_Sweep();;
								break;

						////�����Զ���ɨ��///////////////		//������ɨ??  QZ
							case YAOKONG_SPOT_FRI:
								Init_Spot();
								break;

							case YAOKONG_V_SAT:
								Change_Speed();
								break;

							case YAOKONG_DOCKING_SUN:  
								Init_Docking();
								break;
							
							case YAOKONG_LEFT:
							case YAOKONG_RIGHT:
							case YAOKONG_FRONT:
							case YAOKONG_BACK:
								Init_Tele();
								break;
							
							
							
						/*
							case YAOKONG_OK:
							if( gsv_work_mode.work_mode == SPOT)
							{
									Init_Spot();
							}
							else
							{
									Init_Sweep();
							}
								break;	
						*/
							
							
					}
}
#endif
/*************************************************************
װ��ʱ��
*************************************************************/
void Cease_Lode_Time(void)
{	
    piv_dis_preengage = (u8)PreengageDate;	//ԤԼ����	
    piv_dis_week = (u8)((Rtc_time / 86400)) ;
    piv_dis_minute = (u8)((Rtc_time % 3600) / 60);
    piv_dis_hour = (u8)((Rtc_time % 86400) / 3600) ;

}
/*************************************************************
����ʱ��
*************************************************************/
void  Cease_Save_Time(void)
{
    Rtc_time = 	(piv_dis_hour*3600) + (piv_dis_minute*60) + ((piv_dis_week)*86400);
	WriteRtcTime();
}
/*************************************************************
װ��ʱ��
*************************************************************/
void Cease_Lode_Preengage(void)
{	
    piv_dis_preengage = (u8)PreengageDate;	//ԤԼ����	
    piv_dis_week 			= (u8)(Rtc_time / 86400 );
    piv_dis_minute 		= (u8)(PreengageTime % 256);
    piv_dis_hour 			= (u8)(PreengageTime / 256);

}
/*************************************************************
����ʱ��
*************************************************************/
void  Cease_Save_Preengage(void)
{
    PreengageDate = piv_dis_preengage ;
		PreengageTime = piv_dis_minute + (piv_dis_hour<<8);
		WritePreenData();
}

/*************************************************************
�����ö�ʱԤԼ����
*************************************************************/
#if 0
void Cease_Timing(void)
{	 
u32 liv_time;	 //�����Ŀ�ʼ����ʱ��
u8 liv_number;	 
u8 liv_key_down_time;  //��������ʱ����ʱ��
    switch(	gsv_yaokong.key)
	{
		//////���µ�Դ������ȡ����
	    case YAOKONG_ESC:
			 liv_time = giv_sys_time;
		    ////////�ȴ���Դ��������//////////////////
		    do
			{
			    A7105_rx();		//	
				Beep_Driver();	 
				IWDG_ReloadCounter(); 
		        if((giv_sys_time - liv_time) > 30000)  //����5��
			    {
			        PreengageDate = 0;        //���ԭ����ԤԼ����	
				    PreengageTime = 0;
				    WritePreengage();         //
					Enable_Beep();
			    }
			}
			while(( gsv_yaokong.key == YAOKONG_ESC) &&(gsv_yaokong.effect != 0)) ;

		    ////�жϵ�Դ���Ƿ���ʱ�䳬��5��//////
			Init_Cease();
		    break;	
		
		///////����ԤԼ��������ԤԼ����	 
	    case YAOKONG_V_SAT:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x01;	//�൱�����λȡ�� QZ  
			}
		    break;

		///////����ʱ�䰴������ʱ���趨
//	    case YAOKONG_FRONT:	
//		    if(	piv_set_content == 1)
//			{
//			    piv_dis_preengage ^= 0x02;    
//			}
//		    break;

		//////����uv����uvָʾȡ��
//	    case YAOKONG_BACK:
//		    if(	piv_set_content == 1)
//			{
//			    piv_dis_preengage ^= 0x04;  
//			}
//		    break;

		/////���¶�����ɨ���붨����ɨģʽ
	    case YAOKONG_SETTIME_TUE:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x08;   
			}
		    break;

		////�����Զ���ɨ��///////////////
	    case YAOKONG_AUTO_THU:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x10;   
			}
		    break;

	    case YAOKONG_SPOT_FRI:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x20; 
			}
		    break;

	    case YAOKONG_DOCKING_SUN:  
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x40; 
			}
		    break;

	    case YAOKONG_UV_WED:	   
		    if( piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x80;    
			}
		    break;

	    case YAOKONG_LEFT:		 
		    piv_set_content--; 
		    if(piv_set_content == 0)
			{
			   piv_set_content = 3;
			}
			break;
	    case YAOKONG_RIGHT:	  
		    piv_set_content++; 
		    if(piv_set_content == 4)
			{
			   piv_set_content = 1;
			}
			break;
	    case YAOKONG_FRONT:
			if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x02;    
			}
			else
			{
			    liv_time = giv_sys_time; //���濪ʼ���°�����ʱ��
				liv_number = 0;
			    do
				{
				    A7105_rx();		// ��ȡ����	  
					Beep_Driver();
					liv_key_down_time = (u8)((giv_sys_time - liv_time)/10000) ;
				    //����ʱ�����3�벢�ҵ�����ʱ��
					if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
					{
					   liv_number =  liv_key_down_time;
					   Cease_Big_Add_Time();
					}			 
					Cease_display_set();
					AutoDisplay();
					IWDG_ReloadCounter();
				}
				while(( gsv_yaokong.key == YAOKONG_FRONT) &&(gsv_yaokong.effect != 0)) ;	
			    Cease_Add_Time();
			}
			break;
	    case YAOKONG_BACK:
			if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x04;  
			}
			else
			{
			    liv_time = giv_sys_time; //���濪ʼ���°�����ʱ��
				liv_number = 0;
			    do
				{
				    A7105_rx();		// ��ȡ����	  
					Beep_Driver();
					liv_key_down_time = (u8)((giv_sys_time - liv_time)/10000) ;
				    //����ʱ�����3�벢�ҵ�����ʱ��
					if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
					{
					   liv_number =  liv_key_down_time;
					   Cease_Big_Subb_Time();
					}			 
					Cease_display_set();
					AutoDisplay();
					IWDG_ReloadCounter();
				}
				while(( gsv_yaokong.key == YAOKONG_BACK) &&(gsv_yaokong.effect != 0)) ;
			    Cease_Subb_Time();
			}
			break;

	    case YAOKONG_OK:
		    Cease_Save_Preengage();	 
			Init_Cease();
		    break;
	}

}
#endif

/*************************************************************
������ʱ�����
*************************************************************/
#if 0
void Cease_SetTime(void)
{
u32 liv_time;	 //�����Ŀ�ʼ����ʱ��
u8 liv_number;	 
u8 liv_key_down_time;  //��������ʱ����ʱ��
    switch(	gsv_yaokong.key)
	{
		//////���µ�Դ������ȡ����
	    case YAOKONG_ESC:				   
			Init_Cease();
		    break;
		///////����ԤԼ��������ԤԼ����	 
	    case YAOKONG_V_SAT:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 0;   
			}
		    break;

		///////����ʱ�䰴������ʱ���趨
	    case YAOKONG_FRONT:	
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 1;  
			}
			else
			{
				liv_time = giv_sys_time; //���濪ʼ���°�����ʱ��
				liv_number = 0;
			    do
				{
				    A7105_rx();		// ��ȡ����	
					Beep_Driver();
					liv_key_down_time = (u8)((giv_sys_time - liv_time)/10000) ;
				    //����ʱ�����3�벢�ҵ�����ʱ��
					if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
					{
					   liv_number =  liv_key_down_time;
					   Cease_Big_Add_Time();
					}			 
					Cease_display_set();
					AutoDisplay();
					IWDG_ReloadCounter();
				}
				while(( gsv_yaokong.key == YAOKONG_FRONT) &&(gsv_yaokong.effect != 0)) ;	
			    Cease_Add_Time();
			}

		    break;

		//////����uv����uvָʾȡ��
	    case YAOKONG_BACK:
				if(	piv_set_content == 4)
						{
								piv_dis_week = 2;  
						}
				else
						{
							liv_time = giv_sys_time; //���濪ʼ���°�����ʱ��
							liv_number = 0;
								do
							{
									A7105_rx();		// ��ȡ����	 
								Beep_Driver();
								liv_key_down_time = (u8)((giv_sys_time - liv_time)/10000) ;
									//����ʱ�����3�벢�ҵ�����ʱ��
								if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
								{
									 liv_number =  liv_key_down_time;
									 Cease_Big_Subb_Time();
								}			 
								Cease_display_set();
								AutoDisplay();
								IWDG_ReloadCounter();
							}
							while(( gsv_yaokong.key == YAOKONG_BACK) &&(gsv_yaokong.effect != 0)) ;
								Cease_Subb_Time();
						}
		    break;

		/////���¶�����ɨ���붨����ɨģʽ
	    case YAOKONG_SETTIME_TUE:
		    if(	piv_set_content == 4)
					{
							piv_dis_week = 3;  
					}
		    break;

		////�����Զ���ɨ��///////////////
	    case YAOKONG_AUTO_THU:
		    if(	piv_set_content == 4)
					{
							piv_dis_week = 4; 
					}
		    break;

	    case YAOKONG_SPOT_FRI:
		    if(	piv_set_content == 4)
					{
							piv_dis_week = 5;
					}
		    break;

	    case YAOKONG_DOCKING_SUN:  
		    if(	piv_set_content == 4)
					{
							piv_dis_week = 6;
					}
		    break;
	
	    case YAOKONG_LEFT:	   
		    piv_set_content--;
				if(piv_set_content == 1)
					{
							piv_set_content = 4;
					}
			break;
	    case YAOKONG_RIGHT:
		    piv_set_content++;
				if(piv_set_content == 5)
					{
							piv_set_content = 2;
					}	 
			break;

	    case YAOKONG_OK:
		    Cease_Save_Time();	  	   
				Init_Cease();
		    break;
	}
}
#endif
/*******************************************************************************
��������ʱ��
*******************************************************************************/
void Cease_Big_Add_Time(void)
{
    if(piv_set_content == 3)
    {
	    piv_dis_minute = (piv_dis_minute+3)%60;
	}
	else if(piv_set_content == 2)
	     {
	         piv_dis_hour = (piv_dis_hour + 3) % 24;
	     }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+3)%7;
			  }	  */
}

/*******************************************************************************
��������ʱ��
*******************************************************************************/
void Cease_Add_Time(void)
{
    if(piv_set_content == 3)
	{
		piv_dis_minute = (piv_dis_minute+1)%60;
	}
	else if(piv_set_content == 2)
		 {
		     piv_dis_hour = (piv_dis_hour + 1) % 24;
		 }
		 /*else if(piv_set_content == 4)
			 {
				 piv_dis_week = (piv_dis_week+1)%7;
			 }	 */
}

/*******************************************************************************
��������ʱ��
*******************************************************************************/
void Cease_Big_Subb_Time(void)
{
    if(piv_set_content == 3)
    {
	    piv_dis_minute = (piv_dis_minute+57)%60;
	}
	else if(piv_set_content == 2)
		 {
			 piv_dis_hour = (piv_dis_hour + 21) % 24;
		 }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+4)%7;
		      }*/
}

/*******************************************************************************
��һ����ʱ��
*******************************************************************************/
void Cease_Subb_Time(void)
{
    if(piv_set_content == 3)
	{
	    piv_dis_minute = (piv_dis_minute+59)%60;
	}
	else if(piv_set_content == 2)
		 {
		     piv_dis_hour = (piv_dis_hour + 23) % 24;
		 }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+6)%7;
			  }	*/
}

void Init_VirtualSleep(void)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	
	/******��ʼ�����õ�ֵ********************/
	piv_set_content = 0;			//��ʼ����Ϊû������
	piv_yaokongstate = 0;

	/*******��ʼ������Ŀ���***************/
	stop_rap(); //�ر�������
	Init_Sweep_Pwm(PWM_SWEEP_MAX,PWM_SWEEP_PRESCALER);
	Sweep_Level_Set(SWEEP_LEVEL_STOP);
	Disable_earth();				//�رյؼ�
	Disable_wall();					//�ر�ǽ��
	//Enable_earth();
	//Enable_wall();
	enable_hwincept();				//�򿪻س������յ�Դ
	Enable_Speed(); 				//����״̬���ٶȼ��򿪣���Ϊ�˷�ֹ����CEASEʱ�ر��ٶȼ��ᵼ�¹��������޷����㡣
		
	/****���û����Ĺ���ģʽ******/   
	mode.mode = CEASE; 
	mode.Info_Abort=0;				//��SLAMͨ�ţ�qz add 20180418
	mode.All_Info_Abort=0;			//qz add 20180919
	mode.sub_mode=SUBMODE_VIRTUAL_SLEEP;			//QZ ADD
	mode.step=0x00; 				//qz add
	mode.status=0;					//qz add 20180625
	mode.time=giv_sys_time; 		//qz add 20180703
	mode.init_mode_time=giv_sys_time;	//qz add 20180814
		
	WriteWorkState();
	Disable_Free_Skid_Check();		//�ر������ּ��
	//��ʼ����������
	CHECK_STATUS_FLAG=false; 		//ʹ���쳣���
	Init_Check_Status();//qz add 20180425

	Slam_Data.dipan_req_pre=1;		//qz mask 20180522��Ĭ��Ϊ�滮��ɨ

#ifdef DEBUG_Enter_Mode
	TRACE("Init Cease Mode Complete!Prepare enter to Cease!\r\n");
#endif
	SLAM_DOCK=false;
	DOCK_SWEEP=false;				//qz add 20180803

#ifdef UV
	Reset_UV(); 					//�ر�UV
#endif 
	REYBS_TIME=0;					//qz add 20180910,С�س����������رߴ�����0
	Close_Led();
}

void Do_VirtualSleep(void)
{
	/**************�����ң��������*/
#ifdef DC_NOBAT_RUN
	if(((power.charge_dc)|(power.charge_seat))&(!dc_nobat_run))
#else
	if(power.charge_dc|power.charge_seat)
#endif
		{
			 //mode.last_mode=mode.mode;			//qz add 20180201
			 //Init_Chargeing(CHARGEING);

			 //qz add 20180901
			 if(power.charge_seat)
				Init_Chargeing(SEAT_CHARGING);
			 if(power.charge_dc)
				Init_Chargeing(DC_CHARGING);
			 //qz add end
			 
			 return;
		}
	/////////������ԤԼʱ�䵽����������ɨ��ģʽ/////////////////////

	//�ƻ�����charge��sleep�½���ԤԼʱ���飬���һ��flag��Ȼ����charge��cease�¼�����flag
#if 0
	if(Check_PreengageTime())
			{
	//				 if(Slam_Data.tick_flag)			//�жϵ������Ƿ����������ڹ���״̬	//qz mask 20180420
					{
						//qz add 20180420
						Slam_Data.dipan_req_pre=DIPAN_REQ_PREEN;
						//qz add end
						Slam_Data.dipan_req=Slam_Data.dipan_req_pre;
					}
				 return;
			}
#endif
	//ϵͳ�д���
	if(giv_sys_err != 0)
			{
				Init_Err();
				return ;
			}
#ifdef SLEEP_SUBMODE
	/**********���������3������û�в���,��������˯��ģʽ**********/
	if(((giv_sys_time-mode.time)>10000*600)&(!mode.factory))			//qz mask 20181110
	//if(((giv_sys_time-mode.sleep_time)>1800000)&(!mode.factory))		//qz add 20181110
		{
			Init_Sleep();
			//Init_ShutDown();
			return;
		}
#endif
	clr_all_hw_effect();				//qz mask 20181215
	//	find_home=ReadHwSign_My();			//qz mask 20181215
}
