#include "AAA-include.h"

u32 test_bump_time=0;
void Init_RunTest(void)
{
	mode.last_mode=mode.mode;		//qz add 20180205
	mode.last_sub_mode=mode.sub_mode;
	/******初始化显示***********/
		
	/******初始化设置的值********************/
	clr_ram();
//	ReInitAd();

	Enable_earth();
	Enable_wall();
	enable_hwincept();				//允许红外接收电源
	Enable_Speed(); 				//允许速度发送
	Init_Action();
	
	mode.mode = TEST;			
	mode.sub_mode = RUN_TEST;
	mode.step=0;
	mode.time=giv_sys_time;
	mode.bump = 0;
	mode.step_bp = 0;
	mode.bump_flag=0;
	mode.Info_Abort=0;				//qz add 20180919
//		WriteWorkState();

	w_l.on=0;
	w_r.on=0;
	w_rm.on=0;
	w_lm.on=0;
#ifdef DEBUG_Enter_Mode
	TRACE("Init RUNTEST Mode Complete!\r\n");
#endif
#ifdef FREE_SKID_CHECK
	Enable_Free_Skid_Check();		//打开万向轮检测
#endif
	memset(USART1_RX_BUF,0,sizeof(USART1_RX_BUF)/sizeof(USART1_RX_BUF[0])); 	//清除接收缓存qz add 20180703
	

	motion1.tgt_yaw=Gyro_Data.yaw;
	motion1.anti_tgt_yaw=Get_Reverse_Angle(Gyro_Data.yaw);

#if 0
	if(motion1.tgt_yaw==F_Angle_Const)
		{
			if(Read_Motion_YAcc())	//目标角度为x轴正向，且清扫间隔方向为Y轴正向
				Set_LeftRight(0);			//设置方向为左转及左沿边
			else							//目标角度为x轴正向，且清扫间隔方向为Y轴负向
				Set_LeftRight(1);			//设置方向为右转及右沿边
		}
	else
		{
			if(Read_Motion_YAcc())	//目标角度为x轴负向，且清扫间隔方向为Y轴正向
				Set_LeftRight(1);
			else							//目标角度为x轴负向，且清扫间隔方向为Y轴负向
				Set_LeftRight(0);
		}
#else
	Assign_LeftRight();
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
		case BUMP_ONLY_LEFT:						 //左碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_ONLY_LEFT;
		 case BUMP_ONLY_LEFTMID:					 //左中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {
					 stop_rap();
					 mode.bump=BUMP_ONLY_LEFTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_ONLY_LEFTMID;
		 case BUMP_LEFT_MID:						 //左左中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_LEFT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_LEFT_MID;
		 case BUMP_ONLY_RIGHT:						 //右碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHT;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_ONLY_RIGHT;
		 case BUMP_ONLY_RIGHTMID:					 //右中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_ONLY_RIGHTMID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_ONLY_RIGHTMID;
		 case BUMP_RIGHT_MID:						 //右右中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_RIGHT_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
					 mode.bump_time=giv_sys_time;
					 test_bump_time++;
				 }
			 return BUMP_RIGHT_MID;
		 case BUMP_MID: 							 //中碰撞
			 if((!mode.bump_flag)&(mode.bump==0))
				 {							 
					 stop_rap();
					 mode.bump=BUMP_MID;
					 mode.bump_flag=true;
					 mode.step_bp=0;
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
							break;
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
			switch(mode.test_step)
				{
					case 0:
						if(giv_sys_time-mode.time<200)
							return;
						Speed=TOP_MOVE_SPEED;
						//if(do_action(3,80*CM_PLUS))
						if(do_action_my(3,80*CM_PLUS,motion1.tgt_yaw))

						//if(do_action(2,3600*Angle_1))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
						break;
					case 1:
						if(giv_sys_time-mode.time<200)
							return;
						Speed=TURN_SPEED;
						do_action(2,360*Angle_1);
						if(Judge_Yaw_Reach(8700,TURN_ANGLE_BIOS,1))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
						break;
					case 2:
					
						if(giv_sys_time-mode.time<200)
							return;
						Speed=MID_MOVE_SPEED;
						if(do_action_my(3,20*CM_PLUS,9000))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
						break;
					case 3:
						if(giv_sys_time-mode.time<200)
							return;
						Speed=TURN_SPEED;
						do_action(2,360*Angle_1);
						if(Judge_Yaw_Reach(17700,TURN_ANGLE_BIOS,1))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
						break;
					case 4:
						if(giv_sys_time-mode.time<200)
							return;
						Speed=TOP_MOVE_SPEED;
						if(do_action_my(3,80*CM_PLUS,18000))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
						break;
					case 5:
						if(giv_sys_time-mode.time<200)
							return;
						Speed=TURN_SPEED;
						do_action(1,360*Angle_1);
						if(Judge_Yaw_Reach(9300,TURN_ANGLE_BIOS,1))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
						break;
					case 6:
						if(giv_sys_time-mode.time<200)
							return;
						Speed=MID_MOVE_SPEED;
						if(do_action_my(3,20*CM_PLUS,9000))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
						break;
					case 7:
						if(giv_sys_time-mode.time<200)
							return;
						Speed=TURN_SPEED;
						do_action(1,360*Angle_1);
						if(Judge_Yaw_Reach(3,TURN_ANGLE_BIOS,1))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
						break;
					case 8:
						if(giv_sys_time-mode.time<200)
							return;
						Speed=TOP_MOVE_SPEED;
						if(do_action_my(3,80*CM_PLUS,0))
							{
								stop_rap();
#ifdef TEST_ONESTEP
								mode.step=1;
#else
								mode.test_step++;
#endif
								mode.time=giv_sys_time;
							}
					case 9:
						break;
				}
			break;
			case 1:
				break;
		}
}

//计算打扫区域
//X_ACC: 0:按X轴正负清扫;1:X轴都只朝一个方向清扫
//Y_ACC: 0:按Y轴负方向清扫;1:按Y轴正方向清扫;2:按双方向清扫
void Cal_PosArea_Max_Test(void)
{
//	short xmax,xmin,ymax,ymin;
	short temp_pos;

	TRACE("Enter in %s...\r\n",__func__);
	TRACE("motion1.xacc=%d yacc=%d\r\n",motion1.x_acc,motion1.y_acc);

	motion1.xpos_start_area=Return_GridXPos_Point(grid.x_area_start);
	motion1.ypos_start_area=Return_GridYPos_Point(grid.y_area_start);

	switch(motion1.x_acc)
		{
			case 0:				//沿X轴负方向(B_Angle_Const)清扫
				grid.x_area_max=grid.x_area_start;
				if(grid.x_area_max>GRID_MAX)
					grid.x_area_max=GRID_MAX;
				grid.x_area_min=grid.x_area_start-20;
				if(grid.x_area_min<GRID_MIN)
					grid.x_area_min=GRID_MIN;
				break;
			case 1:				//沿X轴正方向(F_Angle_Const)清扫
				grid.x_area_max=grid.x_area_start+5;
				if(grid.x_area_max>GRID_MAX)
					grid.x_area_max=GRID_MAX;
				grid.x_area_min=grid.x_area_start;
				if(grid.x_area_min<GRID_MIN)
					grid.x_area_min=GRID_MIN;
				break;
			case 2:				//沿X轴双方向清扫
				grid.x_area_max=grid.x_area_start+10;
				if(grid.x_area_max>GRID_MAX)
					grid.x_area_max=GRID_MAX;
				grid.x_area_min=grid.x_area_start-10;
				if(grid.x_area_min<GRID_MIN)
					grid.x_area_min=GRID_MIN;
			default:
				break;
		}
	temp_pos=Return_GridXPos_Point(grid.x_area_max);
	temp_pos+=8;
	if(temp_pos>motion1.xpos_start+RANGE_MAX)
		temp_pos=motion1.xpos_start+RANGE_MAX;
	motion1.xpos_max_area=temp_pos;
	
	temp_pos=Return_GridXPos_Point(grid.x_area_min);
	temp_pos-=8;
	if(temp_pos<motion1.xpos_start-RANGE_MIN)
		temp_pos=motion1.xpos_start-RANGE_MIN;
	motion1.xpos_min_area=temp_pos;

	switch (motion1.y_acc)
		{
			case 0:				//沿Y轴负方向(L_Angle_Const)清扫
				grid.y_area_max=grid.y_area_start;
				if(grid.y_area_max>GRID_MAX)
					grid.y_area_max=GRID_MAX;
				grid.y_area_min=grid.y_area_start-20;
				if(grid.y_area_min<GRID_MIN)
					grid.y_area_min=GRID_MIN;
				break;
			case 1:				//沿Y轴正方向(R_Angle_Const)清扫			
				grid.y_area_max=grid.y_area_start+5;
				if(grid.y_area_max>GRID_MAX)
					grid.y_area_max=GRID_MAX;
				grid.y_area_min=grid.y_area_start;
				if(grid.y_area_min<GRID_MIN)
					grid.y_area_min=GRID_MIN;
				break;
			case 2:				//沿Y轴双方向清扫
			default:
				grid.y_area_max=grid.y_area_start+10;
				if(grid.y_area_max>GRID_MAX)
					grid.y_area_max=GRID_MAX;
				grid.y_area_min=grid.y_area_start-10;
				if(grid.y_area_min<GRID_MIN)
					grid.y_area_min=GRID_MIN;				
				break;				
		}
	
	temp_pos=Return_GridYPos_Point(grid.y_area_min);
	temp_pos-=8;
	if(temp_pos<motion1.ypos_start-RANGE_MAX)
		temp_pos=motion1.ypos_start-RANGE_MAX;
	motion1.ypos_min_area=temp_pos;
	temp_pos=Return_GridYPos_Point(grid.y_area_max);
	temp_pos+=8;
	if(temp_pos>motion1.ypos_start+RANGE_MAX)
		temp_pos=motion1.ypos_start+RANGE_MAX;
	motion1.ypos_max_area=temp_pos;
	
	TRACE("Grid.x_sweep_start=%d\r\n",grid.x_area_start);
	TRACE("Gird.y_sweep_start=%d\r\n",grid.y_area_start);
	TRACE("Grid.x_area_max=%d\r\n",grid.x_area_max);
	TRACE("Grid.x_area_min=%d\r\n",grid.x_area_min);
	TRACE("Grid.y_area_max=%d\r\n",grid.y_area_max);
	TRACE("Grid.y_area_min=%d\r\n",grid.y_area_min);
	TRACE("motion1.xpos_max_area=%d\r\n",motion1.xpos_max_area);
	TRACE("motion1.xpos_min_area=%d\r\n",motion1.xpos_min_area);
	TRACE("motion1.ypos_max_area=%d\r\n",motion1.ypos_max_area);
	TRACE("motion1.ypos_min_area=%d\r\n",motion1.ypos_min_area);
}

void Init_SweepTest(void)
{
	delay_ms(100);
	Init_Coordinate();

	delay_ms(100);
	Get_Const_Angle();

	Init_PathPoint();
	Init_BackHead();
	Delete_All_PathPoint();
	Set_Motion_BackSweep(0);								//qz add 20190328
	motion1.xpos_start=Gyro_Data.x_pos; 			//qz add 20190307
	motion1.ypos_start=Gyro_Data.y_pos; 			//qz add 20190307
	motion1.sweep_time=0;
	motion1.continue_checkstep=0;
	mode.status=1;
	TRACE("Enter Init_First_Sweep!\r\n");
	TRACE("F_Angle_Const=%d\r\n",F_Angle_Const);
	TRACE("B_Angle_Const=%d\r\n",B_Angle_Const);
	TRACE("L_Angle_Const=%d\r\n",L_Angle_Const);
	TRACE("R_Angle_Const=%d\r\n",R_Angle_Const);
	TRACE("LF_Angle_Const=%d\r\n",LF_Angle_Const);
	TRACE("LB_Angle_Const=%d\r\n",LB_Angle_Const);
	TRACE("RF_Angle_Const=%d\r\n",RF_Angle_Const);
	TRACE("RB_Angle_Const=%d\r\n",RB_Angle_Const);
	TRACE("Range max=%d\r\n",motion1.xpos_start+RANGE_MAX);
	TRACE("Range min=%d\r\n",motion1.xpos_start-RANGE_MIN);
	TRACE("GRID_MAX=%d\r\n",GRID_MAX);
	TRACE("GRID_MIN=%d\r\n",GRID_MIN);
	Cal_Grid_Pos();
	if(Creat_AreaNodeList())
		{
			stop_rap();
			error_code=ERROR_NODEMALLOC;
			Init_Err();
			return;
		}

	Send_Voice(VOICE_SWEEP_START);
	Open_Led(1,0,1);

	TRACE("Enter Init_Test_Sweep!\r\n");
	short tgt_yaw=Gyro_Data.yaw;

	Set_Motion_BackSweep(0);								//qz add 20190328
	Set_Motion_Acc(1,1);
	motion1.sweep_time++;
	motion1.exit_area_num=motion1.area_num;
	motion1.area_num=motion1.sweep_time;
	motion1.continue_checkstep=0;
	motion1.area_ok=false;
	motion1.ymax_ok=false;
	motion1.ymin_ok=false;
	motion1.xmax_ok=false;
	motion1.xmin_ok=false;
	
	grid.x_area_start=grid.x;
	grid.y_area_start=grid.y;

	Cal_PosArea_Max_Test();
	if(Add_AreaNode_End())
		{
			stop_rap();
			error_code=ERROR_NODEMALLOC;
			Init_Err();
			return;
		}
	Load_CurrNode_Info();
	Init_NormalSweep(tgt_yaw);		
	switch(motion1.y_acc)
		{
			case 0:
				Set_Motion_YDir(-1);
				break;
			case 1:
				Set_Motion_YDir(1);
				break;
			case 2:
				Set_Motion_YDir(0);
				break;
		}
	if(motion1.x_acc==2)
		motion1.repeat_sweep=true;		//第一次清扫，如果是X轴是双方向清扫，则设置重复扫一次，单方向的，则没有必要
}
