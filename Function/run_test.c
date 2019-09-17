#include "AAA-include.h"

u32 test_bump_time=0;
void Init_RunTest(void)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	/******��ʼ����ʾ***********/
		
	/******��ʼ�����õ�ֵ********************/
	clr_ram();
//	ReInitAd();

	Enable_earth();
	Enable_wall();
	enable_hwincept();				//���������յ�Դ
	Enable_Speed(); 				//�����ٶȷ���
#if 0
	if(DOCK_SWEEP)
		Sweep_Level_Set(DOCK_SWEEP_LEVEL);
	else
		Sweep_Level_Set(sweep_suction);
#endif		 
	Init_Action();
	
	mode.mode = TEST;			
	mode.sub_mode = RUN_TEST;
	mode.step=0;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
	mode.All_Info_Abort=0;			//qz add 20180919
//		WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init RUNTEST Mode Complete!\r\n");
#endif
	//��ʼ����������
	CHECK_STATUS_FLAG=true; 	//qz add 20180725:���������������ʱ,�ӵ����������ֱ�ӽ����״̬,�����Ҫ�򿪼�⿪��
//	Init_Check_Status();//qz add 20180425
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//�������ּ��
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//������ջ���qz add 20180703
	
	if(dis_err_code==DIS_ERROR_DOCK_FAIL)		//qz add 20180710
		{
			dis_err_code=0;
			error_code=0;
		}

#ifdef UV
	if((mode.status)&(!SLAM_DOCK))		//qz add 20180902
		Set_UV();
	else
		Reset_UV();
#endif
	motion1.tgt_yaw=Gyro_Data.yaw;
	motion1.anti_tgt_yaw=Get_Reverse_Angle(Gyro_Data.yaw);

#if 0
	if(motion1.tgt_yaw==F_Angle_Const)
		{
			if(Read_Motion_YAcc())	//Ŀ��Ƕ�Ϊx����������ɨ�������ΪY������
				Set_LeftRight(0);			//���÷���Ϊ��ת�����ر�
			else							//Ŀ��Ƕ�Ϊx����������ɨ�������ΪY�Ḻ��
				Set_LeftRight(1);			//���÷���Ϊ��ת�����ر�
		}
	else
		{
			if(Read_Motion_YAcc())	//Ŀ��Ƕ�Ϊx�Ḻ������ɨ�������ΪY������
				Set_LeftRight(1);
			else							//Ŀ��Ƕ�Ϊx�Ḻ������ɨ�������ΪY�Ḻ��
				Set_LeftRight(0);
		}
#else
	Assign_LeftRight(0);
#endif
	grid.x_straight_start=grid.x;
	grid.y_straight_start=grid.y;
	
	TRACE("motion1.tgt_yaw=%d\r\n",motion1.tgt_yaw);
	TRACE("motion1.anti_tgt_yaw=%d\r\n",motion1.anti_tgt_yaw);
	TRACE("grid.x_straight_start=%d\r\n",grid.x_straight_start);
	TRACE("grid.y_straight_start=%d\r\n",grid.y_straight_start);

	delay_ms(500);
}

u8 Read_RunTest_Bump(void)
{
	u8 data1;
	
	data1=Parse_BumpValue();
	switch (data1)
	{
		case BUMP_ONLY_LEFT:						 //����ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.l_bump_flag=true;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_ONLY_LEFT;
		 case BUMP_ONLY_LEFTMID:					 //������ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.l_bump_flag=true;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_ONLY_LEFTMID;
		 case BUMP_LEFT_MID:						 //��������ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_LEFT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.l_bump_flag=true;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_LEFT_MID;
		 case BUMP_ONLY_RIGHT:						 //����ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.r_bump_flag=true;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_ONLY_RIGHT;
		 case BUMP_ONLY_RIGHTMID:					 //������ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.r_bump_flag=true;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_ONLY_RIGHTMID;
		 case BUMP_RIGHT_MID:						 //��������ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_RIGHT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.r_bump_flag=true;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_RIGHT_MID;
		 case BUMP_MID: 							 //����ײ
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 Slam_Data.m_bump_flag=true;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_MID;
	}
	return 0;
}

void RunTest_Bump_Action(void)
{
	u8 m;
	m=Read_RunTest_Bump();
	switch(mode.bump)
		{
			case BUMP_ONLY_LEFT:
			case BUMP_ONLY_LEFTMID:
			case BUMP_LEFT_MID:
			case BUMP_ONLY_RIGHT:
			case BUMP_ONLY_RIGHTMID:
			case BUMP_RIGHT_MID:
			case BUMP_MID:
				switch(mode.step_bp)
					{
						case 0:
							stop_rap();
							mode.bump_time=giv_sys_time;
							mode.step_bp++;
//							TRACE("tst_bp_time=%d\r\n",test_bump_time);
//							TRACE("bumpx=%d bumpy=%d\r\n",Gyro_Data.x_pos,Gyro_Data.y_pos);
						break;
						case 1:
							if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
								return;
							TRACE("ring_cnt=%d\r\n",ring_cnt_flag);
							TRACE("bump xpos=%d ypos=%d\r\n",Gyro_Data.x_pos,Gyro_Data.y_pos);
							TRACE("bump tst time=%d\r\n",test_bump_time);
							mode.step_bp++;
							break;
						case 2:
							Speed=1000;
							if(do_action(4,20*CM_PLUS))
								{
									stop_rap();
									mode.step_bp++;
									mode.bump_time=giv_sys_time;
								}
							break;
						case 3:
							if(giv_sys_time-mode.bump_time<BUMP_TIME_DELAY)
								return;
							mode.step_bp=0;
							mode.bump=0;
							mode.step=0;
							mode.bump_flag=false;
							TRACE("back xpos=%d ypos=%d\r\n",Gyro_Data.x_pos,Gyro_Data.y_pos);
							break;
					}
		}
}

void Do_RunTest(void)
{
	ACC_DEC_Curve();
	//YBS_Comm_Rap_My();
	RunTest_Bump_Action();
	if(mode.bump)
		return;
	switch(mode.step)
		{
			case 0:
				if(giv_sys_time-mode.time<200)
					return;
				Speed=1000;
				if(do_action_my(3,20*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
						mode.time=giv_sys_time;
					}
				break;
			case 1:
				break;
				if(giv_sys_time-mode.time<200)
					return;
				Speed=BUMP_BACK_SPEED;
				if(do_action_my(3,10*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step++;
						mode.time=giv_sys_time;
					}
				break;
			case 2:
				if(giv_sys_time-mode.time<200)
					return;
				Speed=BUMP_BACK_SPEED;
				if(do_action(4,BUMP_BACK_LENGTH*CM_PLUS))
					{
						stop_rap();
						mode.step++;
						mode.time=giv_sys_time;
					}
//			  do_action(3,FARAWAY*CM_PLUS);
				break;
			case 3:
				if(giv_sys_time-mode.time<200)
					return;
				Speed=BUMP_BACK_SPEED;
				if(do_action_my(3,10*CM_PLUS,motion1.tgt_yaw))
					{
						stop_rap();
						mode.step=0;
						mode.time=giv_sys_time;
					}
				break;
		}
}

