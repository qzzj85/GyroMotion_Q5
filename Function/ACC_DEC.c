
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================


//////////////////////////私有定义//////////////////////////////////
#define G_KP 80/100
#define	G_KI 10/100
#define G_KD 10/100
#define SPEED_MAX	50//100//800

#define	W_KP	50/100
#define	W_KI	10/100
#define	W_KD 	10/100
#define WALL_SPEED_MAX 200 	
//////////////////////////全局变量//////////////////////////////////
/////////////////////////全局函数////////////////////////////////////
/////////////////////////私有函数////////////////////////////////////

void  ACC_DEC_comm_rap(void);
void  ACC_DEC_Curve(void);
//=======================================================================
///////////////////////函数实体////////////////////////////////////

void ACC_DEC_Comm_rap_My(void);
void Gyro_Comm_rap(void);
void Wall_Comm_Rap(void);
//=================================================================================
//=================================================================================
//////功能  ：    每10ms 执行一次，自动根据车轮信息控制车轮
//////输入： 无
//////输出 ：无
//=================================================================================
//=================================================================================


uint32_t Acc_Dec_length;
//==================================================================================================================================================================
//==================================================================================================================================================================
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
//	int ec;
	float f_ec;
//		if(rap_time==false)
//			return;
//		rap_time=false;

	Check_Speed_My(&(l_ring.real_speed), &(r_ring.real_speed));

#if 1
	if(Deci_Sec)
		{
			Deci_Sec=false;
			if(l_rap.rap_run<l_rap.rap)
			{
				l_rap.rap_run+=300;
				if(l_rap.rap_run>=l_rap.rap)
					l_rap.rap_run=l_rap.rap;
			}
			else if(l_rap.rap_run>l_rap.rap)
			{
				l_rap.rap_run-=300;
				if(l_rap.rap_run<l_rap.rap)
					l_rap.rap_run=l_rap.rap;
			}	
			
			if(r_rap.rap_run<r_rap.rap)
			{
				r_rap.rap_run+=300;
				if(r_rap.rap_run>=r_rap.rap)
				r_rap.rap_run=r_rap.rap;
			}
			else if(r_rap.rap_run>r_rap.rap)
			{
				r_rap.rap_run-=300;
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

			if(r_rap.length<r_ring.length+10*CM_PLUS)
				{
					r_rap.rap=500;
				}
			
			if(r_rap.length  <=  (r_ring.length+10) )
				{
					r_rap.sign		= 0;
					r_rap.pwm			= 0;
					r_rap.rap		= 0;
					disable_pwm(R_BACK);
					disable_pwm(R_FRONT); 
					r_rap.rap_run=0;
				}				
		}

	if(l_rap.sign)
		{
			if(l_rap.ori == FRONT)
				{enable_pwm(L_FRONT,l_rap.pwm);}
			else
				{enable_pwm(L_BACK,l_rap.pwm);}

			if(l_rap.length<l_ring.length+10*CM_PLUS)
				{
					l_rap.rap=500;
				}
			
			if(l_rap.length  <=  (l_ring.length+10) )
				{
					l_rap.sign		= 0;
					l_rap.pwm			= 0;
					l_rap.rap		= 0;
					disable_pwm(L_FRONT);
					disable_pwm(L_BACK);
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
	if(r_rap.rap_run<r_rap.rap-SPEED_MAX)
		r_rap.rap_run=r_rap.rap-SPEED_MAX;
	
	l_rap.rap_run-=data1;
	if(l_rap.rap_run>l_rap.rap+SPEED_MAX)
		l_rap.rap_run=l_rap.rap+SPEED_MAX;
	if(l_rap.rap_run<l_rap.rap-SPEED_MAX)
		l_rap.rap_run=l_rap.rap-SPEED_MAX;
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



