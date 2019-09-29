
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================


//=================================================================================
//=================================================================================
//////功能  ：    每10ms 执行一次，自动根据车轮信息控制车轮
//////输入： 无
//////输出 ：无
//=================================================================================
//=================================================================================

bool gyro_comm_flag=true;
bool spd_acc_flag=true;

uint32_t Acc_Dec_length;
//==================================================================================================================================================================
//==================================================================================================================================================================

void Check_Speed_My(unsigned int *speed_l,unsigned int *speed_r)
{
	static u32 speed_check_time=0;
	u32 t=0;int l_length_temp=0,r_length_temp=0;
	t=giv_sys_time-speed_check_time;
#ifdef DEBUG_SPEED
	TRACE("t=%d\r\n",t);
#endif
	speed_check_time=giv_sys_time;

#ifdef DEBUG_SPEED
	TRACE("l.leng=%d l.last_length=%d\r\n",l_ring.speed_length,l_ring.last_speed_length);
	TRACE("r.leng=%d r.last_length=%d\r\n",r_ring.speed_length,r_ring.last_speed_length);
#endif
	l_length_temp=l_ring.speed_length-l_ring.last_speed_length;
	l_ring.last_speed_length=l_ring.speed_length;
	r_length_temp=r_ring.speed_length-r_ring.last_speed_length;
	r_ring.last_speed_length=r_ring.speed_length;

#ifdef STOP_SPD_CNT
	if(l_length_temp)
		{
			l_ring.stop_buf[0]=l_ring.stop_buf[1];
			l_ring.stop_buf[1]=l_ring.stop_buf[2];
			l_ring.stop_buf[2]=l_ring.stop_buf[3];
			l_ring.stop_buf[3]=l_ring.stop_buf[4];
			l_ring.stop_buf[4]=l_ring.stop_buf[5];
			l_ring.stop_buf[5]=l_ring.stop_buf[6];
			l_ring.stop_buf[6]=l_ring.stop_buf[7];
			l_ring.stop_buf[7]=l_ring.stop_buf[8];
			l_ring.stop_buf[8]=l_ring.stop_buf[9];
			l_ring.stop_buf[9]=l_length_temp;
		}

	if(r_length_temp)
		{
			r_ring.stop_buf[0]=r_ring.stop_buf[1];
			r_ring.stop_buf[1]=r_ring.stop_buf[2];
			r_ring.stop_buf[2]=r_ring.stop_buf[3];
			r_ring.stop_buf[3]=r_ring.stop_buf[4];
			r_ring.stop_buf[4]=r_ring.stop_buf[5];
			r_ring.stop_buf[5]=r_ring.stop_buf[6];
			r_ring.stop_buf[6]=r_ring.stop_buf[7];
			r_ring.stop_buf[7]=r_ring.stop_buf[8];
			r_ring.stop_buf[8]=r_ring.stop_buf[9];
			r_ring.stop_buf[9]=l_length_temp;
		}
#endif
		
//qz add 20180703
	if((l_rap.sign)&(l_length_temp==0))
		l_ring.odds++;
	else
		l_ring.odds=0;

	if((r_rap.sign)&(r_length_temp==0))
		r_ring.odds++;
	else
		r_ring.odds=0;
//qz add end

	*speed_l=l_length_temp*10000/t;
	*speed_r=r_length_temp*10000/t;


	if(r_rap.sign==0)
		*speed_r=0;
	if(l_rap.sign==0)
		*speed_l=0;

	//qz add 20180417
	#if 0
	if((r_rap.sign)&(*speed_r<=0))
		r_ring.odds++;
	else if(((r_rap.sign)&(*speed_r>0))||(!r_rap.sign))		//qz modify 20180515:增加!r_rap.sign条件
		r_ring.odds=0;
	
	if((l_rap.sign)&(*speed_l<=0))
		l_ring.odds++;
	else if(((l_rap.sign)&(*speed_l>0))||(!l_rap.sign))		//qz modify 20180515:增加!l_rap.sign条件
		l_ring.odds=0;
	#endif

	if(r_ring.odds>2000)		//约3s qz modify 20180515
		{
#if 1
			TRACE("l_rap=%d r_rap=%d\r\n",l_rap.rap,r_rap.rap);
			TRACE("l_pwm=%d r_pwm=%d\r\n",l_rap.pwm,r_rap.pwm);
#endif
			r_ring.state=BAD;
		}
	if(l_ring.odds>2000)
		{
			l_ring.state=BAD;
#if 1
			TRACE("l_rap=%d r_rap=%d\r\n",l_rap.rap,r_rap.rap);
			TRACE("l_pwm=%d r_pwm=%d\r\n",l_rap.pwm,r_rap.pwm);
#endif
		}
	//qz add end

}

void  ACC_DEC_Curve(void)
{
//u32 	t,l;
//signed long ec;

	if(rap_time == false)
		{return;}
	rap_time = false;
		
	ACC_DEC_Comm_rap_My();
	//ACC_DEC_Comm_rap_My_II();
}
//==================================================================================================================================================================
//==================================================================================================================================================================
void ACC_DEC_Comm_rap_My (void)
{
	float f_ec;

	Check_Speed_My(&(l_ring.real_speed), &(r_ring.real_speed));

#if 1
	if(spd_acc_flag)
		{
			spd_acc_flag=false;
			if(l_rap.rap_run<l_rap.rap)
			{
				l_rap.rap_run+=200;
				if(l_rap.rap_run>=l_rap.rap)
					l_rap.rap_run=l_rap.rap;
			}
			else if(l_rap.rap_run>l_rap.rap)
			{
				l_rap.rap_run-=200;
				if(l_rap.rap_run<l_rap.rap)
					l_rap.rap_run=l_rap.rap;
			}	
			
			if(r_rap.rap_run<r_rap.rap)
			{
				r_rap.rap_run+=200;
				if(r_rap.rap_run>=r_rap.rap)
				r_rap.rap_run=r_rap.rap;
			}
			else if(r_rap.rap_run>r_rap.rap)
			{
				r_rap.rap_run-=200;
				if(r_rap.rap_run<r_rap.rap)
					r_rap.rap_run=r_rap.rap;
			}	
		}
#else
	l_rap.rap_run=l_rap.rap;
	r_rap.rap_run=r_rap.rap;
#endif

	if(Gyro_Data.straight)
		Gyro_Comm_rap();
/////////////////////////////////右轮/////////////////////////// 
	if(r_rap.sign)
		{
			r_ring.f_ek[2]=r_ring.f_ek[1];
			r_ring.f_ek[1]=r_ring.f_ek[0];
			r_ring.f_ek[0]=(float)(r_rap.rap_run)-(float)(r_ring.real_speed);
			//f_ec = ((r_ring.f_ek[0]-r_ring.f_ek[1])*F_KP + r_ring.f_ek[0]*F_KI +(r_ring.f_ek[0]+r_ring.f_ek[2]-2*r_ring.f_ek[1])*F_KD);
			f_ec = ((r_ring.f_ek[0]-r_ring.f_ek[1])*YBS_FKP + r_ring.f_ek[0]*YBS_FKI +(r_ring.f_ek[0]+r_ring.f_ek[2]-2*r_ring.f_ek[1])*YBS_FKD);
			r_rap.pwm=(int)(r_rap.pwm+f_ec);
			if(r_rap.pwm>PWM_RING_MAX)
				r_rap.pwm=PWM_RING_MAX;
			else if(r_rap.pwm<=0)
				r_rap.pwm=0;
								
		}

/////////////左轮脉冲所走时间////////////////////	
	if(l_rap.sign)
		{
			l_ring.f_ek[2]=l_ring.f_ek[1];
			l_ring.f_ek[1]=l_ring.f_ek[0];
			l_ring.f_ek[0]=(float)(l_rap.rap_run)-(float)(l_ring.real_speed);
			//f_ec = ((l_ring.f_ek[0]-l_ring.f_ek[1])*F_KP + l_ring.f_ek[0]*F_KI +(l_ring.f_ek[0]+l_ring.f_ek[2]-2*l_ring.f_ek[1])*F_KD);
			f_ec = ((l_ring.f_ek[0]-l_ring.f_ek[1])*YBS_FKP + l_ring.f_ek[0]*YBS_FKI +(l_ring.f_ek[0]+l_ring.f_ek[2]-2*l_ring.f_ek[1])*YBS_FKD);
			l_rap.pwm=(int)(l_rap.pwm+f_ec);
			if(l_rap.pwm>PWM_RING_MAX)
				l_rap.pwm=PWM_RING_MAX;
			else if(l_rap.pwm<=0)
				l_rap.pwm=0;		
	}
	
	if(r_rap.sign)
		{
			if(r_rap.ori == FRONT)
				{enable_pwm(R_FRONT,r_rap.pwm);}
			else
				{enable_pwm(R_BACK,r_rap.pwm);}
#if 0
			if((r_rap.length<r_ring.length+10*CM_PLUS)&(r_rap.rap>500))
				{
					r_rap.rap=500;
				}
#endif
			if(r_rap.length  <=  (r_ring.length+10) )
				{
					disable_pwm(R_BACK);
					disable_pwm(R_FRONT); 
					if((l_rap.sign)&(l_rap.length==r_rap.length))
						{
#ifdef MILE_COMPENSATION
#ifdef STOP_SPD_CNT
							l_ring.stop_spd=0;
							for(int i=0;i<10;i++)
								{
									l_ring.stop_spd+=l_ring.stop_buf[i];
									l_ring.stop_buf[i]=0;
								}					
							l_ring.stop_spd=l_ring.stop_spd*10;
#else
							//l_ring.stop_spd=l_rap.rap_run;
							l_ring.stop_spd=l_rap.rap;
#endif
							l_ring.cal_length=l_ring.stop_spd*l_ring.stop_spd/LENGTH_CAL;
							TRACE("length_cal_data=%d\r\n",l_ring.cal_length);
							if(l_rap.ori==FRONT)
								l_ring.all_length+=l_ring.cal_length;
							else if(l_rap.ori==BACK)
								l_ring.all_length-=l_ring.cal_length;
#endif					
							l_rap.sign		= 0;
							l_rap.pwm			= 0;
							l_rap.rap		= 0;
							l_rap.rap_run=0;
						}
#ifdef MILE_COMPENSATION
#ifdef STOP_SPD_CNT
					r_ring.stop_spd=0;
					for(int i=0;i<10;i++)
						{
							r_ring.stop_spd+=r_ring.stop_buf[i];
							r_ring.stop_buf[i]=0;
						}
					r_ring.stop_spd=r_ring.stop_spd*10;
#else
					//r_ring.stop_spd=r_rap.rap_run;
					r_ring.stop_spd=r_rap.rap;
#endif
					r_ring.cal_length=r_ring.stop_spd*r_ring.stop_spd/LENGTH_CAL;
					TRACE("length_cal_data=%d\r\n",r_ring.cal_length);
					if(r_rap.ori==FRONT)
						r_ring.all_length+=r_ring.cal_length;
					else if(r_rap.ori==BACK)
						r_ring.all_length-=r_ring.cal_length;
#endif					
					r_rap.sign		= 0;
					r_rap.pwm			= 0;
					r_rap.rap		= 0;
					r_rap.rap_run=0;
				}	
		}

	if(l_rap.sign)
		{
			if(l_rap.ori == FRONT)
				{enable_pwm(L_FRONT,l_rap.pwm);}
			else
				{enable_pwm(L_BACK,l_rap.pwm);}
#if 0
			if((l_rap.length<l_ring.length+10*CM_PLUS)&(l_rap.rap>500))
				{
					l_rap.rap=500;
				}
#endif
			if(l_rap.length  <=  (l_ring.length+10) )
				{
					disable_pwm(L_FRONT);
					disable_pwm(L_BACK);
					if((r_rap.sign)&(r_rap.length==l_rap.length))
						{
							disable_pwm(R_FRONT);
							disable_pwm(R_BACK);
#ifdef MILE_COMPENSATION
#ifdef STOP_SPD_CNT
							r_ring.stop_spd=0;
							for(int i=0;i<10;i++)
								{
									r_ring.stop_spd+=r_ring.stop_buf[i];
									r_ring.stop_buf[i]=0;
								}
							r_ring.stop_spd=r_ring.stop_spd*10;
#else
							//r_ring.stop_spd=r_rap.rap_run;
							r_ring.stop_spd=r_rap.rap;
#endif
							r_ring.cal_length=r_ring.stop_spd*r_ring.stop_spd/LENGTH_CAL;
							TRACE("length_cal_data=%d\r\n",r_ring.cal_length);
							if(r_rap.ori==FRONT)
								r_ring.all_length+=r_ring.cal_length;
							else if(r_rap.ori==BACK)
								r_ring.all_length-=r_ring.cal_length;
#endif					
							r_rap.sign		= 0;
							r_rap.pwm			= 0;
							r_rap.rap		= 0;
							r_rap.rap_run=0;
						}
#ifdef MILE_COMPENSATION
#ifdef STOP_SPD_CNT
					l_ring.stop_spd=0;
					for(int i=0;i<10;i++)
						{
							l_ring.stop_spd+=l_ring.stop_buf[i];
							l_ring.stop_buf[i]=0;
						}					
					l_ring.stop_spd=l_ring.stop_spd*10;
#else
					//l_ring.stop_spd=l_rap.rap_run;
					l_ring.stop_spd=l_rap.rap;
#endif
					l_ring.cal_length=l_ring.stop_spd*l_ring.stop_spd/LENGTH_CAL;
					TRACE("length_cal_data=%d\r\n",l_ring.cal_length);
					if(l_rap.ori==FRONT)
						l_ring.all_length+=l_ring.cal_length;
					else if(l_rap.ori==BACK)
						l_ring.all_length-=l_ring.cal_length;
#endif					
					l_rap.sign		= 0;
					l_rap.pwm			= 0;
					l_rap.rap		= 0;
					l_rap.rap_run=0;
				}
		}
}



//==================================================================================================================================================================
//==================================================================================================================================================================

int Cal_Gyro_Angle(void)
{
	short temp_yaw,temp_tgt;
	int temp_angle;
	temp_yaw=Gyro_Data.yaw;
	temp_tgt=Gyro_Data.tgt_yaw;

#if 1
	if((temp_tgt>0)&(temp_yaw<0))
		{
			temp_angle=temp_yaw-temp_tgt;
			if(abs(temp_angle)>180*100)
				{
					temp_angle=temp_angle+360*100;		//右转为正
				}
		}
	else if((temp_tgt<0)&(temp_yaw>0))
		{
			temp_angle=temp_yaw-temp_tgt;
			if(abs(temp_angle)>180*100)
				{
					temp_angle=temp_angle-360*100;
				}
		}
	else
		{
			temp_angle=temp_yaw-temp_tgt;
		}
#else
	temp_angle=temp_yaw-Gyro_Data.tgt_yaw;
	if(abs(temp_angle)>DEGREE_180)
		{
			if(temp_angle>0)
				temp_angle=temp_angle-DEGREE_360;
			else
				temp_angle=temp_angle+DEGREE_360;
		}
#endif
	return temp_angle;
}

void Gyro_Comm_rap(void)
{
	int data1=0;

//	if(!gyro_comm_flag)
//		return;
	gyro_comm_flag=false;
	Gyro_Data.g_ek[2]=Gyro_Data.g_ek[1];
	Gyro_Data.g_ek[1]=Gyro_Data.g_ek[0];
	Gyro_Data.g_ek[0]=Cal_Gyro_Angle();

	data1=(Gyro_Data.g_ek[0]-Gyro_Data.g_ek[1])*G_KP+Gyro_Data.g_ek[0]*G_KI+(Gyro_Data.g_ek[0]+Gyro_Data.g_ek[2]-2*Gyro_Data.g_ek[1])*G_KD;
//	TRACE("data1=%d\r\n",data1);

#if 1
	if(data1>10)
		data1=10;
	if(data1<-10)
		data1=-10;
#endif

	r_rap.rap_run+=data1;
	if(r_rap.rap_run>r_rap.rap+SPEED_MAX)
		r_rap.rap_run=r_rap.rap+SPEED_MAX;
	if(r_rap.rap_run<100)
		r_rap.rap_run=100;
	//if(r_rap.rap_run<r_rap.rap-SPEED_MAX)
		//r_rap.rap_run=r_rap.rap-SPEED_MAX;
	
	l_rap.rap_run-=data1;
	if(l_rap.rap_run>l_rap.rap+SPEED_MAX)
		l_rap.rap_run=l_rap.rap+SPEED_MAX;
	if(l_rap.rap_run<100)
		l_rap.rap_run=100;
	//if(l_rap.rap_run<l_rap.rap-SPEED_MAX)
		//l_rap.rap_run=l_rap.rap-SPEED_MAX;
}

void Wall_Comm_Rap(void)
{
	static int wall_ek[3];
	int data1=0;
	
	if(mode.sub_mode==YBS_SUB_LEFT)
		{
			wall_ek[2]=wall_ek[1];
			wall_ek[1]=wall_ek[0];
			wall_ek[0]=w_l.dis-YBS_DISTANCE;

			data1=(wall_ek[0]-wall_ek[1])*G_KP+wall_ek[0]*G_KI+(wall_ek[0]+wall_ek[2]-2*wall_ek[1])*G_KD;
#if 1
				if(data1>50)
					data1=50;
				if(data1<-50)
					data1=-50;
#endif
			
				r_rap.rap_run+=data1;
				if(r_rap.rap_run>r_rap.rap+WALL_SPEED_MAX)
					r_rap.rap_run=r_rap.rap+WALL_SPEED_MAX;
				if(r_rap.rap_run<r_rap.rap-WALL_SPEED_MAX)
					r_rap.rap_run=r_rap.rap-WALL_SPEED_MAX;
				
				l_rap.rap_run-=data1;
				if(l_rap.rap_run>l_rap.rap+WALL_SPEED_MAX)
					l_rap.rap_run=l_rap.rap+WALL_SPEED_MAX;
				if(l_rap.rap_run<l_rap.rap-WALL_SPEED_MAX)
					l_rap.rap_run=l_rap.rap-WALL_SPEED_MAX;
		}
	else if(mode.sub_mode==YBS_SUB_RIGHT)
		{
			wall_ek[2]=wall_ek[1];
			wall_ek[1]=wall_ek[0];
			wall_ek[0]=w_r.dis-YBS_DISTANCE;

			data1=(wall_ek[0]-wall_ek[1])*W_KP+wall_ek[0]*W_KI+(wall_ek[0]+wall_ek[2]-2*wall_ek[1])*W_KD;
#if 1
				if(data1>50)
					data1=50;
				if(data1<-50)
					data1=-50;
#endif
			l_rap.rap_run+=data1;
			if(l_rap.rap_run>l_rap.rap+WALL_SPEED_MAX)
				l_rap.rap_run=l_rap.rap+WALL_SPEED_MAX;
			if(l_rap.rap_run<l_rap.rap-WALL_SPEED_MAX)
				l_rap.rap_run=l_rap.rap-WALL_SPEED_MAX;
			
			r_rap.rap_run-=data1;
			if(r_rap.rap_run>r_rap.rap+WALL_SPEED_MAX)
				r_rap.rap_run=r_rap.rap+WALL_SPEED_MAX;
			if(r_rap.rap_run<r_rap.rap-WALL_SPEED_MAX)
				r_rap.rap_run=r_rap.rap-WALL_SPEED_MAX;
		}
}
//==================================================================================================================================================================
//==================================================================================================================================================================



