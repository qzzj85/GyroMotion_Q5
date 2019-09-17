////////////////////////ͷ�ļ�////////////////////////////////////
#include "AAA-include.h"

static u8 piv_out;	//�����Ƿ�����չ����1Ϊ����չ����0Ϊ������С
static u8 piv_left; //�����Ƿ�����ת��1Ϊ����ת��0Ϊ����ת
static u8 piv_done;   //qz add��������ɶ���Ϊ1,δ���Ϊ0��20180125

////////////////////////˽�ж���//////////////////////////////////
////////////////////////ȫ�ֱ���//////////////////////////////////
///////////////////////˽�б���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////
void Do_Spot(void);
void Init_Spot(void);
///////////////////////˽�к���////////////////////////////////////
u8 Spot_YaoKong_Manage(void);	
u32  read_bump1(void);
///////////////////////����ʵ��////////////////////////////////////
/*****************************************************************
���ܣ���ʼ��������ɨ�ĳ���
*****************************************************************/
void Init_Spot(void)
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
	Dis_SpGlint = true;
	/******��ʼ�����õ�ֵ********************/
	mode.sub_mode = SPOT;	 //����ģʽΪ��ɨ
	mode.mode = SPOT;
	clr_ram();
	

	Enable_earth();
	//Disable_wall();
	if(DOCK_PREPARE)
		{
			Disable_wall();
			Reset_Wall_Send();
			DOCK_PREPARE=false;
			Speed=1000;
		}
	else
		{
			Enable_wall();
			Set_Wall_Send();
			Speed=4000;
		}
//	enable_hwincept();//���������յ�Դ
//	Reset_Speed_Send();  //������ⷢ��  
	 
	Display_Real();
	Init_Action();
//	ReInitAd();
	////////////////���ǽ������
    w_l.sign=FARN;
    w_lm.sign=FARN;
    w_r.sign=FARN;
    w_rm.sign=FARN;
    w_m.sign=FARN;
	WriteWorkState();

	piv_left=1;
	piv_done=0;
	piv_out=1;

}

/*****************************************************************
���ܣ�������ɨ����
*****************************************************************/
void Do_Spot(void)
{
//  static u8 piv_out;	//�����Ƿ�����չ����1Ϊ����չ����0Ϊ������С
//  static u8 piv_left; //�����Ƿ�����ת��1Ϊ����ת��0Ϊ����ת
//  static u8 piv_done;   //qz add��������ɶ���Ϊ1,δ���Ϊ0��20180125

  u32 m ;
  s32 l_length,r_length;	//qz add 20180125
  static u8 bump_turn;
  		
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
						}
					return;
			}
			
  //���ʼ����ʱ�ж��������Ƿ���أ������˳�ɨ��ģʽ
  if((giv_sys_time - mode.time) < 5000)
		{
//				if((lidi.key == 0)||(lidi.key == 0))	////QZ:MASK
				if((l_lidi.key==0)||(r_lidi.key==0))	//QZ:add
					{
							Init_Cease();
							return ;
					}
		}
		
////////////  if((giv_sys_time - mode.time) < 100)
////////////		{
////////////			if(CheckMDustMotor())
////////////					{
////////////							return;
////////////					}
////////////		}
  ///ң������������		     
//  if(Spot_YaoKong_Manage() == 1)
//		{
//				return;
//		}
  ///������������������е�����һ��������
////  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
////		{
////      key1.sign = 1;
////			key2.sign = 1;
////			key3.sign = 1;
////			Init_Cease();	 
////			Enable_Beep();
////			return;
////		}
    
    
    
  //////��ʾ��ʾ��������
  Display_Real();
 
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&rm_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  ////������һЩ��Ҫ�������ж�////////////

	//ϵͳ�д���
	if(giv_sys_err != 0)
	{
	    Init_Err();
		return ;
	}
  
  ////��ң����ʱ����ֹͣ����ң�ط����˶�
//  if(YaoKongFangXiang())
//  {
//      return;
//  }
  ////�Ƿ�����Ҫ����������
	
	#if 0
  m=read_bump1();
	switch(mode.bump)	 //bump��ײ�ı�־
    {
	    case 1://E_L		   
	        switch  (mode.step_bp)	 //step_bp��ײʱ�Ĳ����־
		    {
			    case  0:
	                if(do_action(7,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return ; 
		            }
				    if((m==1))
				    {
				     return ;
				    }		   
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(7,angle30))
				    {
				        mode.step_bp++;
				    }
				    return ;	 
			    case  2:
			        if(do_action(8,angle15))
				    {
				        mode.step_bp++;
				    }	  
				    read_earth();
				    return ;
		        case  3:		
	                if(do_action(2,angle30))
		            {
		                mode.bump = 0;
		            }
				    read_earth();	
					return;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  ;
	    case 4://E_R: 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(8,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return ; 
		            }
				    if((m==4))
				    {
				         return ;
				    }		   
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(8,angle30))
				    {
				        mode.step_bp++;
				    }
				    return ;	 
			    case  2:
			        if(do_action(7,angle15))
				    {
				        mode.step_bp++;
				    }	
				    read_earth();
				    return ;
		        case  3:		
	                if(do_action(1,angle30))
		            {
		                mode.bump = 0;
		            }
				    read_earth();	
		            return;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  ;	 
				
			case 2://E_LM 
	    case 3://E_RM: 	 
	    case 5://L_BUMP:
	    case 6://R_BUMP:	
	    case 7://W_L:
	    case 8://W_LM:	
	    case 9://W_M:		 		    
	    case 10://up_hw			 	
	    case 11://l_hw	 	
	    case 12://rm_hw
	    case 13://r_hw	
	    case 14://W_R:	
	    case 15://W_RM:	
            switch(mode.step_bp)
								{
                case 0:
											if(do_action(4,angle360))
												{
													giv_sys_err = mode.bump;
													Init_Err();
													return ; 
												}		
											read_sideearth();
											if(m == 0)
												{
													stop_rap();
													mode.step_bp = 1;
												}
										return ;
							case 1:
								if(do_action(1,30*Angle_1))
								//if(do_action(1,angle30))
									{
											mode.bump = 0;
									}		 
								read_earth();	
								return;
									
	            default :
	                mode.bump = 0;
							}
			return ;
	    default :
	        mode.bump = 0;
	        break;
	} //����  switch(mode.bump)	
	#endif

	#if 1		//qz add 20180125
	m=read_bump1();
	switch(mode.bump)
		{

			case 5:
			case 6:
			case 8:
			case 9:
			case 15:
				switch(mode.step_bp)
					{

						case 0:
							Speed=2400;
							if(do_action(4,5*CM_PLUS))
								{

									stop_rap();
									mode.step_bp++;
									return;
								}
							return;
						case 1:
							if(piv_left)
								bump_turn=2;
							else
								bump_turn=1;
							mode.step_bp++;
							return;
						case 2:
							Speed=2400;
							if(do_action(bump_turn,180*Angle_1))
								{
									stop_rap();
									if(m==0)
										{
											mode.bump=0;
											mode.step_bp=0;
											if(piv_left)
												piv_left=0;
											else
												piv_left=1;
											Speed=4000;
										}
									
								}
						
					}
		}
		#endif
		
	
	
	
	
	
  switch (mode.step)	//step·��ִ�еĲ���
		{
			case 0:
				#if 0	//qz mask 20180125
				left(360*Angle_1);
//				left(angle360);//ԭ��תȦ1Ȧ
				mode.step++;
				piv_out = 1;
				piv_left = 1;
				break;
				#endif
			
				#if 1
				if(!piv_done)
					{
						left(360*Angle_1);
						mode.step++;
						piv_out=1;
						piv_left=1;
					}
				else
					{
						if((!r_rap.sign)&&(!l_rap.sign))
							{
								if(do_action(1,(360*Angle_1)))
									{
										piv_done=0;					//qz add ׼����һ�ζ���
										stop_rap();
//										Init_Cease();
//										Init_Commander();
										return;
									}
								return;
							}
					}
					break;
				#endif
			
			case 1:
			case 2:
			case 3:
			case 4:
				if((!l_rap.sign)&&(!r_rap.sign))
						{   
								action.step++;
//								if(piv_out == 0)
//										{
//												Init_Cease();
//												return;
//										}
								if(piv_left != 0)
										{
											//qz add
											l_length=(Angle_1*180)-mode.step*(Angle_1*45);
											if(l_length<0)
												l_length=0;
											r_length=(Angle_1*180)+mode.step*(Angle_1*45);
											if(r_length<0)
												r_length=0;
											//qz add end
												
//											enable_rap(BACK,(angle180-mode.step*angle45), FRONT,(angle180+mode.step*angle45));		//qz mask
											enable_rap(BACK,l_length, FRONT,r_length);												//qz add
										}
								else
										{
											//qz add
											l_length=(Angle_1*180)+mode.step*(Angle_1*45);
											if(l_length<0)
												l_length=0;
											r_length=(Angle_1*180)-mode.step*(Angle_1*45);
											if(r_length<0)
												r_length=0;
											//qz add end
											
//											enable_rap(FRONT,(angle180+mode.step*angle45),BACK,(angle180-mode.step*angle45) ); 
											enable_rap(BACK,l_length, FRONT,r_length);
										}
								
										//qz add
								if(piv_out!=0)
									mode.step++;
								else
									{
										mode.step--;
										piv_done=1;
									}
								
						}
				break;
						
						
				case   5:
				case   6:
				case   7:
				case   8:
				case   9:
				case   10:
				case   11:
				case   12:
				case   13:
				case   14:
				case   15:
				case   16:
				case   17:
					if((!l_rap.sign)&&(!r_rap.sign))
							{
								action.step++;
								if(piv_left != 0)
										{
												enable_rap(FRONT, (mode.step-4)*(45*Angle_1), FRONT, (mode.step-4)*(45*Angle_1)+360*(Angle_1));		//qz add									
//												enable_rap(FRONT, (mode.step-4)*angle45, FRONT, (mode.step-4)*angle45+angle360);					//qz mask 20180124
										}
								else
										{
												enable_rap(FRONT, (mode.step-4)*(45*Angle_1)+360*(Angle_1), FRONT, (mode.step-4)*(45*Angle_1));					//qz add
//												enable_rap(FRONT, (mode.step-4)*angle45+angle360, FRONT, (mode.step-4)*angle45);					//qz mask
										}
								if(mode.step == 17)
										{
												piv_out = 0;
										}
								if(piv_out != 0)
										{
												mode.step++;
										}
								else 
										{
												mode.step--;
										}
							}
				break; 
		}
}
/******************************************
��ȡ��ײ��Ϣ
*******************************************/
u32  read_bump1(void)
{
if((e_l.sign == FARN))
   {
	  if((mode.bump > 4) || (mode.bump == 0))	    //��ؼ�����
       { 
	      mode.bump = 1;// E_L;
	      mode.step_bp=0;
	      stop_rap();
	    }
	 return 1;//E_L;
   }   
//if((e_lm.sign==FARN))
//   {
//	  if((mode.bump > 4) || (mode.bump == 0))	    
//     {
//      mode.bump= 2;//E_LM;
//	  mode.step_bp=0;
//	  stop_rap();
//	 }
//	  return 2;//E_LM;
//    }
//if((e_rm.sign==FARN))
//  {
//	  if((mode.bump > 4) || (mode.bump == 0))	    
//     { 
//      mode.bump=3;//E_RM;
//	  mode.step_bp=0;
//	  stop_rap();
//	 }
//	  return 3;//E_RM;
//  }	 						
  if((e_r.sign==FARN))
   {
	  if((mode.bump > 4) || (mode.bump == 0))	   
     {	
      mode.bump=4;//E_R;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 4;//E_R;
   }
  if (0 == l_bump.key)		//qz modify:��Ļ���Ϊ�½��� 20180125
    {
	 if((mode.bump == 0))	 //��ǰײ
     {
      mode.bump=5;//L_BUMP;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 5;//L_BUMP;
 	}
  if (0 == r_bump.key)		//qz modify:��Ļ���Ϊ�½��� 20180125
    {
	if((mode.bump == 0))	 //��ǰײ
     {
      mode.bump=6;//R_BUMP;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 6;//R_BUMP;
	}      
  if((w_lm.sign==NEAR))
  {
   if((mode.bump == 0))		 //����ǽ�쿿��ǽ
     {
      mode.bump=8;//W_LM;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  
	  return 8;//W_LM;
   }
  if((w_m.sign==NEAR))
   {
   if((mode.bump == 0))	   //��ǽ�쿿��ǽ
     {
      mode.bump=9;//W_M;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 9;//W_M;
   }	   
  if((w_rm.sign==NEAR))
  {
   if((mode.bump == 0))		//����ǽ�쿿��ǽ
     {
      mode.bump=15;//W_RM;
	  mode.step_bp=0;
	  stop_rap();
	 }
	  return 15;//W_RM;
   }

return 0;
}
