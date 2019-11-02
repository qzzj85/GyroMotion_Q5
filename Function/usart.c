
//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================
#define NORMAL_LENGTH 		0x4D
#define FACTURE_LENGTH		0X41

//QZ ADD
u8 		RELOCATE_FLAG		=1;					//qz add���ض�λ��־λ
bool 	STANDBY_FLAG		=false;				//������־λ��false:������,true:������
u8 		IR_RECE_FLAG		=0;					//�յ����̨�źű�־λ	
bool	SLAM_DOCK			=false;				//SLAM�·��س�ָ�����س�
bool 	DOCK_SWEEP			=false;				//�س��ˢ�ٶȸı��־
u16 	error_code			=0;					//���̼����쳣���룬���ֽڣ�
u16		dis_err_code		=0;					//��Ҫ��ʾ���쳣���룬�������̼��ĺ�SLAM�·���
u32 	dis_err_time;				

u16 Leap_Year_Day[2]={365,366};
u8 Year_Month_Day[2][12]={
{31,28,31,30,31,30,31,31,30,31,30,31},
{31,29,31,30,31,30,31,31,30,31,30,31}
};
u32 unix_sec=0;


u8 USART1_TX_BUF[USART1_TX_SIZE];
u8 USART1_RX_BUF[USART1_RX_SIZE];


u8 USART2_TX_BUF[USART2_TX_SIZE];
u8 USART2_RX_BUF[USART2_RX_SIZE];
u8 USART3_TX_BUF[USART3_TX_SIZE];
u8 USART3_RX_BUF[USART3_RX_SIZE];	//����С������


GYRO_DATA		Gyro_Data;
UNIX_TIME		Unix_Time;
PREEN_DATA		Preen_Data[PREEN_LENGTH];
SLAM_DATA		Slam_Data;

//qz add end

MOTO_TASK moto_task;									//����״̬�ṹ
bool Mileage_report_time=false,sensor_report_time=false;


UART UART1;
UART UART2;
UART UART3;
MOTION motion1;

short old_yaw=0;
char cnt_180=0;

void Init_SLAM_DATA(void)
{
	Slam_Data.tick_time			=	giv_sys_time;//SLAM����ʱ�䣬�������������ʧ��60�أ�1min����
												//����ǿ�Ƹ�slam�ϵ磬�ϵ�ʱ��1s������3�أ�����
												//����ʱ�����û�н��յ���������֪ͨSLAM���߱�־����
	Slam_Data.tick_check_step	=	0;
	Slam_Data.skid_flag			=	0;			//�򻬱�־
	Slam_Data.tick_flag			=	false;			//����ͨ�Ž��ձ�־��ʼ��Ϊ0

	Slam_Data.dipan_req			=	0;			//��������ԭʼ����
	Slam_Data.dipan_req_pre		=	0;			//��������Ԥ�����ݣ��Զ���ɨģʽ,AutoDispaly()�������ھ����Ǹ���־��ʾ,qz modify 1-->0 20180522
	Slam_Data.bat				=	0;
	Slam_Data.last_bat			=	0;
	Slam_Data.no_msg			=	false;		//��Ҫ�ϴ�
	Slam_Data.sleep_flag		=	false;
	Slam_Data.dust_level		=	0;			//�۶ȵȼ���0��OK��3���۶ȴ�
	Slam_Data.wifi_ok			=	false;
	Slam_Data.ir_flag			=	false;
	Slam_Data.preen_hour		=	0;
	Slam_Data.preen_min			=	0;
	Slam_Data.preen_off		=	false;
	Slam_Data.restart_flag		=	false;
}

void Init_Gyro_Data(void)
{
	Gyro_Data.first_rst=false;
	Gyro_Data.check_step=0;
	Gyro_Data.start_check_time=giv_sys_time;
	Gyro_Data.first_roll=Gyro_Data.roll;		//qz add 20180927
	Gyro_Data.count_times=0;
	Gyro_Data.g_ek[0]=0;
	Gyro_Data.g_ek[1]=0;
	Gyro_Data.g_ek[2]=0;
}

u8 Leap_Year_Check(u16 year)
{
	if(((year%4==0)&&(year%100!=0))||(year%400==0))
		return 1;
	else
		return 0;
}

u8 Get_WeekDay(UNIX_TIME *Unix_Time)
{
	char century_code, year_code, month_code, day_code;
	int week = 0;

	century_code = year_code = month_code = day_code = 0;

	if (Unix_Time->Month == 1 || Unix_Time->Month == 2) 
	{
		century_code = (Unix_Time->Year - 1) / 100;
		year_code = (Unix_Time->Year - 1) % 100;
		month_code = Unix_Time->Month + 12;
		day_code = Unix_Time->Day;
	} 
	else 
	{
		century_code = Unix_Time->Year / 100;
		year_code = Unix_Time->Year % 100;
		month_code = Unix_Time->Month;
		day_code = Unix_Time->Day;
	}

	week = year_code + year_code / 4 + century_code / 4 - 2 * century_code + 26 * ( month_code + 1 ) / 10 + day_code - 1;
	week = week > 0 ? (week % 7) : ((week % 7) + 7);

	return week;
}

void UnixTimetoTime(u32 Unix_Sec,UNIX_TIME *Unix_Time)
{
	u32 year=0,day=0,sec=0,n=0,i=0;

	Unix_Sec+=8*3600;
	day=Unix_Sec/86400;
	sec=Unix_Sec%86400;

	while(day>366)
		{
			day-=Leap_Year_Day[Leap_Year_Check(year+1970)];
			year++;
		}
	if(((day==365)&&(!Leap_Year_Check(year+1970)))||((day==366)&&(Leap_Year_Check(year+1970))))
		{
			year++;
			day=0;
		}
	Unix_Time->Year=1970+year;
	n=Leap_Year_Check(Unix_Time->Year);
	for(i=0;i<12;i++)

		{
			if(day>=Year_Month_Day[n][i])
				day-=Year_Month_Day[n][i];
			else
				break;
		}
	Unix_Time->Month=i+1;
	Unix_Time->Day=day+1;
	Unix_Time->Hour=sec/3600;
	Unix_Time->Min=sec%3600/60;
	Unix_Time->Sec=sec%60;
	Unix_Time->Week_Day=Get_WeekDay(Unix_Time);

	if(Unix_Time->Sec>60)
		Unix_Time->Sec=0;
	if(Unix_Time->Min>60)
		Unix_Time->Min=0;
	if(Unix_Time->Hour>24)
		Unix_Time->Hour=0;
	if(Unix_Time->Day>31)
		Unix_Time->Day=0;
	if(Unix_Time->Month>12)
		Unix_Time->Month=0;
	if(Unix_Time->Week_Day>7)
		Unix_Time->Week_Day=0;
	
}

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
/***************************START*********************/
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	USART1->DR = (u8) ch;      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	return ch;
}
#endif 
/***************************END*********************/

void u3_printf(char* fmt,...)  
{  
	while(DMA1_Channel2->CNDTR!=0);	//�ȴ�ͨ��2������� 
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	DMA_USART3_TX_Length(strlen((const char*)USART3_TX_BUF));
}

void u2_printf(char* fmt,...)  
{  
	while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��2������� 
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	DMA_USART2_TX_Length(strlen((const char*)USART2_TX_BUF));
}
			
//LRC ���У��
u8 LRC8( u8 *ptr,u32 length)
{
	u8 value=0;
	
	value = *ptr;
	ptr++;
	value ^= *ptr;
	length -= 2;
	while(length--)
			{
					ptr++;
					value ^= *ptr;
			}
	
	return value;
}


void Mile_Gyro_Report(void)
{
	int temp_32;
	u16 temp_u16=0,count=0;
	u8 temp_u8=0;
	//if(Mileage_report_time&(UART3.Trans_Busy==false))
	if(Mileage_report_time&(DMA_UART3_TX_Channel->CNDTR==0))
		{
			Mileage_report_time=false;
#if 0
			USART3_TX_BUF[0] = 0xA5;
			USART3_TX_BUF[1] = 0x00;
			USART3_TX_BUF[2] = 0x12;
			USART3_TX_BUF[3] = 0x04;
			USART3_TX_BUF[4] += 1;
	
			step_step=l_ring.all_length;
			USART3_TX_BUF[5]	= step_step >> 24;
			USART3_TX_BUF[6]	= step_step >> 16;
			USART3_TX_BUF[7]	= step_step >> 8;
			USART3_TX_BUF[8]	= step_step;
	
			step_step=r_ring.all_length;
			USART3_TX_BUF[9]	= step_step >> 24;
			USART3_TX_BUF[10]	= step_step >> 16;
			USART3_TX_BUF[11]	= step_step >> 8;
			USART3_TX_BUF[12]	= step_step;
	
			USART3_TX_BUF[13] = (giv_sys_time/10) >> 24;
			USART3_TX_BUF[14] = (giv_sys_time/10) >> 16;
			USART3_TX_BUF[15] = (giv_sys_time/10) >> 8;
			USART3_TX_BUF[16] = (giv_sys_time/10) ;
			
//			USART3_TX_BUF[17]=LRC8((unsigned char*)&USART3_TX_BUF[1],16);
//			USART3_TX_BUF[18]=0x5a;
//			UART3.TxdDataNum=19;
			
			USART3_TX_BUF[17]=0;//100;
			USART3_TX_BUF[18]=0;//90;//36;
			USART3_TX_BUF[19]=LRC8(&(USART3_TX_BUF[1]),18);
			USART3_TX_BUF[20]=0x5a;
			UART3.TxdDataNum=21;
			DMA_USART3_TX_Length(UART3.TxdDataNum);
			UART3.Trans_Busy=true;
#endif
		
			/*-----------֡ͷ---------*/
			USART3_TX_BUF[count++]=0xA5;	//֡ͷ						0
			USART3_TX_BUF[count++]=0x01;	//Command					1

			/*---------����-----------*/
			USART3_TX_BUF[count++]=0x00;
			USART3_TX_BUF[count++]=NORMAL_LENGTH;	//data_cnt						2
			
			/*---------ʱ���--------*/
//			time_temp=giv_sys_time;
			USART3_TX_BUF[count++] = (time_stemp/10) >> 24;//			3
			USART3_TX_BUF[count++] = (time_stemp/10) >> 16;//			4
			USART3_TX_BUF[count++] = (time_stemp/10) >> 8;//				5
			USART3_TX_BUF[count++] = (time_stemp/10);// 				6

			/*---------�쳣��--------*/
			USART3_TX_BUF[count++] = error_code>>8;
			USART3_TX_BUF[count++] = error_code;
//			if(error_code)				//qz mask V2.3.8
//				error_code=0;


			/*---------����ģʽ-------*/
			//�ڴ�س��£���Ȼ����Ӧ������ģʽ�ϱ���qz add 20180419
			temp_u8=mode.mode;
			if(mode.mode==CHARGEING)					//qz add :���״̬�ϱ�0x01���ȴ�����ģʽ
				temp_u8=0x01;
			else if((mode.mode==CEASE)&(mode.sub_mode==QUIT_CHARGING))
				temp_u8=0x05;
			else
				temp_u8=mode.mode;
			USART3_TX_BUF[count++]=temp_u8; 					//7

			/*-------����������--------*/
			temp_32=l_ring.all_length/2;
			USART3_TX_BUF[count++]=temp_32>>24; 					//16
			USART3_TX_BUF[count++]=temp_32>>16; 						//17
			USART3_TX_BUF[count++]=temp_32>>8;
			USART3_TX_BUF[count++]=temp_32;
			/*-------����������---------*/
			temp_32=r_ring.all_length/2;
			USART3_TX_BUF[count++]=temp_32>>24; 					//16
			USART3_TX_BUF[count++]=temp_32>>16; 						//17
			USART3_TX_BUF[count++]=temp_32>>8;
			USART3_TX_BUF[count++]=temp_32;

			/*-----������������--------*/
			temp_32=w_ring.length;
			USART3_TX_BUF[count++]=temp_32>>24; 					//16
			USART3_TX_BUF[count++]=temp_32>>16; 						//17
			USART3_TX_BUF[count++]=temp_32>>8;
			USART3_TX_BUF[count++]=temp_32;
			
			/*-------��ײ״̬-----------*/
			temp_u8=0x0;
			if(Slam_Data.m_bump_flag)
				{
					temp_u8=0x03;
					Slam_Data.m_bump_flag=false;
				}
			else if(Slam_Data.l_bump_flag)
				{
					temp_u8=0x01;
					Slam_Data.l_bump_flag=false;
				}
			else if(Slam_Data.r_bump_flag)
				{
					temp_u8=0x02;
					Slam_Data.r_bump_flag=false;
				}
			else
				temp_u8=0x00;
			USART3_TX_BUF[count++]=temp_u8; 						//24
			
			/*-------�������--------*/
			temp_u16=(u16)w_l.dis;
			USART3_TX_BUF[count++]=temp_u16>>8;
			USART3_TX_BUF[count++]=temp_u16;
			/*------���к�����-------*/
			temp_u16=(u16)w_lm.dis;
			USART3_TX_BUF[count++]=temp_u16>>8; 						//10
			USART3_TX_BUF[count++]=temp_u16;		//RIGHT 			//11
			/*-------�к�����--------*/
			temp_u16=(u16)w_m.dis;
			USART3_TX_BUF[count++]=temp_u16>>8;
			USART3_TX_BUF[count++]=temp_u16;
			/*------���к�����-------*/
			temp_u16=(u16)w_rm.dis;
			USART3_TX_BUF[count++]=temp_u16>>8;
			USART3_TX_BUF[count++]=temp_u16;
			/*--------�Һ�����-------*/
			temp_u16=(u16)w_r.dis;
			USART3_TX_BUF[count++]=temp_u16>>8; 					//8
			USART3_TX_BUF[count++]=temp_u16;							//9
			/*-------�������----------*/
			temp_u16=0X00;
			USART3_TX_BUF[count++]=temp_u16>>8; 						//10
			USART3_TX_BUF[count++]=temp_u16;		//RIGHT 			//11
			/*-------�г������----------*/
			temp_u16=0X00;
			USART3_TX_BUF[count++]=temp_u16>>8; 						//10
			USART3_TX_BUF[count++]=temp_u16;		//RIGHT 			//11
			/*-------�ҳ������----------*/
			temp_u16=0X00;
			USART3_TX_BUF[count++]=temp_u16>>8; 						//10
			USART3_TX_BUF[count++]=temp_u16;		//RIGHT 			//11

			/*-------���ռ�֪/�ض�λ-----------*/
			//temp_u8=RELOCATE_FLAG<<2;
			temp_u8=0x00;
#ifdef	CLIFF_ENABLE
			if(e_l.sign==FARN)
				temp_u8|=0x10;
			if(e_m.sign==FARN)
				temp_u8|=0x08;
			if(e_r.sign==FARN)
				temp_u8|=0x04;
#endif
			USART3_TX_BUF[count++]=temp_u8;

			/*-------��ؼ��------------*/
			temp_u8=0;
#ifdef LIFT_CHECK
			temp_u8=(~((r_lidi.key<<1)|(l_lidi.key)))&0x03;
#endif
			USART3_TX_BUF[count++]=temp_u8;

			/*-------���״̬-----------*/
			temp_u8=0;
			if(mode.mode==CHARGEING)
				temp_u8=power.charge_dc|power.charge_seat;
#ifdef DC_NOBAT_RUN
			if(dc_nobat_run)
				temp_u8=0;
#endif
			USART3_TX_BUF[count++]=temp_u8; 			//28

			/*-------�����ϱ�-----------*/
			temp_u8=Battery.bat_per;
			USART3_TX_BUF[count++]=temp_u8;
						
			/*--------��״��----------*/
			USART3_TX_BUF[count++]=Slam_Data.skid_flag; 						//29	
			//qz add 20180323���򻬱�־��Ϊ0�������ϱ�1s
#ifdef SKID_REPORT_TIME
			if((Slam_Data.skid_flag)&&(giv_sys_time-Slam_Data.skid_report_time>10000))
#endif
				Slam_Data.skid_flag=0;
			
			/*-------��������-----------*/
			USART3_TX_BUF[count++]=Slam_Data.dipan_req;
			//qz add 20180625,
			if(Slam_Data.dipan_req==DIPAN_REQ_TURNOFFSLAM)			//��������ػ�
				Slam_Data.no_msg=true;								//�����ϴ���Ϣ��־λΪ��
			//qz add end
#if 0			
			if((Slam_Data.dipan_req==DIPAN_REQ_FORWORD)|(Slam_Data.dipan_req==DIPAN_REQ_BACK)|
				(Slam_Data.dipan_req==DIPAN_REQ_LEFT)|(Slam_Data.dipan_req==DIPAN_REQ_RIGHT))
				{
					if(giv_sys_time-remote_info.key_time>500)
						Slam_Data.dipan_req=0;
				}
			//if(Slam_Data.dipan_req)
			else
#endif
				Slam_Data.dipan_req=0x00;
			
			/*-------�ߵ��Ƕ�YAW-----------*/
				temp_u8=0x00;
				
			USART3_TX_BUF[count++]=Gyro_Data.yaw>>8;				//38
			USART3_TX_BUF[count++]=Gyro_Data.yaw;					//39
			/*-------�ߵ��Ƕ�pitch---------*/
			USART3_TX_BUF[count++]=Gyro_Data.pitch>>8;				//40
			USART3_TX_BUF[count++]=Gyro_Data.pitch; 				//41
			/*-------�ߵ��Ƕ�roll---------*/
			USART3_TX_BUF[count++]=Gyro_Data.roll>>8;				//42
			USART3_TX_BUF[count++]=Gyro_Data.roll;					//43
			
			/*----�ߵ��Ǽ��ٶ�(x-axis)-----*/
			USART3_TX_BUF[count++]=Gyro_Data.x_acc>>8;				//44
			USART3_TX_BUF[count++]=Gyro_Data.x_acc; 				//45
			
			/*----�ߵ��Ǽ��ٶ�(y-axis)-----*/
			USART3_TX_BUF[count++]=Gyro_Data.y_acc>>8;				//46
			USART3_TX_BUF[count++]=Gyro_Data.y_acc; 				//47

			/*----�ߵ��Ǽ��ٶ�(z-axis)-----*/
			USART3_TX_BUF[count++]=Gyro_Data.z_acc>>8;				//48
			USART3_TX_BUF[count++]=Gyro_Data.z_acc; 				//49

			/*-------ACK----------------*/
			USART3_TX_BUF[count++]=Slam_Data.ack;	//30
			if(Slam_Data.ack)
				Slam_Data.ack=0;

			/*------���汾��-----------*/
			USART3_TX_BUF[count++]=MAIN_VERISON;

			/*------���汾��----------*/
			USART3_TX_BUF[count++]=SUB_VERSION;

			/*------�����汾��--------*/
			USART3_TX_BUF[count++]=CORRECT_VERSION;

			/*------�۶ȵȼ�----------*/
			USART3_TX_BUF[count++]=Slam_Data.dust_level;
			if(Slam_Data.dust_level)
				Slam_Data.dust_level=0;
			
#ifdef YBS_SEAT_REPROT
			/*------������ź�--------*/
			USART3_TX_BUF[count++]=Slam_Data.ir_flag;
			if(Slam_Data.ir_flag)
				Slam_Data.ir_flag=false;
			temp_u8=0;

			/*------�����ֽ�----------*/
#else
			temp_u8=0;
			USART3_TX_BUF[count++]=temp_u8;
#endif


			/*------ԤԼģʽ������----*/
			USART3_TX_BUF[count++]=0x11;				//Ĭ�ϣ��Զ���ɨ����׼����

			/*------ԤԼ�Ƿ��ظ��Ϳ���-----*/
			if((Slam_Data.preen_on)&(!Slam_Data.preen_off))
				{
					USART3_TX_BUF[count++]=0x80;
					Slam_Data.preen_on=false;
				}
			else
					USART3_TX_BUF[count++]=0x00;

			/*------ԤԼСʱ----------*/
			USART3_TX_BUF[count++]=Slam_Data.preen_hour;
			if(Slam_Data.preen_hour)
				Slam_Data.preen_hour=0;

			/*------ԤԼ����----------*/
			USART3_TX_BUF[count++]=Slam_Data.preen_min;
			if(Slam_Data.preen_min)
				Slam_Data.preen_min=0;

			/*------ȡ��ԤԼ----------*/
			USART3_TX_BUF[count++]=Slam_Data.preen_off;
			if(Slam_Data.preen_off)
				Slam_Data.preen_off=false;

			/*------������־----------*/			//qz add 20180902
			if((!Slam_Data.restart_flag)&(giv_sys_time-mode.time>100000))
				Slam_Data.restart_flag=true;
			temp_u8=Slam_Data.restart_flag;
			USART3_TX_BUF[count++]=temp_u8;

			/*------�����ϱ�----------*/
			USART3_TX_BUF[count++]=voice_level;

			/*------�����ֽ�----------*/
			USART3_TX_BUF[count++]=temp_u8;
			USART3_TX_BUF[count++]=temp_u8;
			USART3_TX_BUF[count++]=temp_u8;
			USART3_TX_BUF[count++]=temp_u8;
			USART3_TX_BUF[count++]=temp_u8;
			USART3_TX_BUF[count++]=temp_u8;
			USART3_TX_BUF[count++]=temp_u8;
			USART3_TX_BUF[count++]=temp_u8;

			/*------֡��������--------*/
			temp_u16=count-2;
			USART3_TX_BUF[2]=temp_u16>>8;
			USART3_TX_BUF[3]=temp_u16;

			/*------У���ֽ�----------*/
			temp_u8=LRC8(&(USART3_TX_BUF[2]),temp_u16);
			USART3_TX_BUF[count++]=temp_u8; 						//50

			/*------֡β--------------*/
			USART3_TX_BUF[count++]=0x5A;
			UART3.TxdDataNum = count;	
			
			DMA_USART3_TX_Length(UART3.TxdDataNum);
			UART3.Trans_Busy = true;
	
		}
}

bool command_check2(unsigned char p,unsigned char length)
{
	if(USART2_RX_BUF[(p+length+2)%128] == 0x5a)
		{return true;}
	else
		{return false;}
}

#ifdef   TUYA_WIFI
void Uart1_send(u8 *temp1 ,unsigned short count1)
{
   u8 i= 0;
   
   for (i=0;i<count1;i++)
   	    USART1_TX_BUF[i] = *temp1++;
   
   UART1.TxdDataNum = count1;	

   UART_TX_DMA_Enable();
   UART1.Trans_Busy = true;
   while (UART1.Trans_Busy == 1) ;
}
#endif

void Uart1_Comunication(void)
{
#if 1   // ����һ���ֽھ��жϷ�ʽ 
    wifi_uart_service();
#endif //tuya
}
//============================================================================================

void Uart3_Communication(void)
{
	u16 cnt_checksum=0,rece_checksum=0,num=0;
	unsigned char COMBUF3[USART3_RX_SIZE];
//	short temp_x=0,temp_y=0;

	if(UART3.Rece_Done==false)
		return;
	UART3.Rece_Done=false;
	
#ifdef YIS055
	if((USART3_RX_BUF[0]==0x59)&&(USART3_RX_BUF[1]==0x49)&(USART3_RX_BUF[2]==0X53))
#else
	if((USART3_RX_BUF[0]==0XAA)&&(USART3_RX_BUF[1]==0XAA))
#endif

	{
		memcpy(COMBUF3,USART3_RX_BUF,USART3_RX_SIZE);
#ifdef YIS055
		num=3;
#else
		num=2;
#endif

#ifdef YIS055
		while(num<USART3_RX_SIZE-2)
#else
		while(num<USART3_RX_SIZE-1)
#endif
		{
			cnt_checksum+=COMBUF3[num];
			num++;
		}
		
#ifdef YIS055
		rece_checksum=(COMBUF3[USART3_RX_SIZE-2]<<8)+COMBUF3[USART3_RX_SIZE-1];
#else
		rece_checksum=COMBUF3[USART3_RX_SIZE-1];
#endif

#ifdef YIS055
		if(cnt_checksum==rece_checksum)
#else
		if((cnt_checksum&0x00ff)==rece_checksum)
#endif
			{
			
#ifdef YIS055
				Gyro_Data.yaw=(COMBUF3[7]<<8)+COMBUF3[8];
#else
				Gyro_Data.yaw=(COMBUF3[4]<<8)+COMBUF3[3];
#endif

#ifdef GYRO_COMPENSATION
				long	yaw_temp ;
				yaw_temp = Gyro_Data.yaw -old_yaw;
				old_yaw = Gyro_Data.yaw;
			
				if (yaw_temp < -30000)	  // ������ ����180��ʱ��,	old_yaw Ϊ�� ,�µ�yawΪ��   , ��ô��Ҫ���µı��Ϊ����ֵ
					cnt_180 -=1;
				if (yaw_temp >	30000)	  //������ת���� 180�� 
					cnt_180 +=1;
				if(cnt_180 <0) 
					{
						if (Gyro_Data.yaw< -18000) 
						Gyro_Data.yaw =	Gyro_Data.yaw + 36000;
					}
				if(cnt_180 >0) 
					{
						if (Gyro_Data.yaw>  18000)  
						Gyro_Data.yaw =	Gyro_Data.yaw - 36000;
					}	
				Gyro_Data.yaw=((int)(Gyro_Data.yaw))*(36000+289)/36000;
				//Gyro_Data.yaw=((int)(Gyro_Data.yaw))*1.007;
				if (Gyro_Data.yaw> 18000) 
				 Gyro_Data.yaw	=  Gyro_Data.yaw - 36000;
				if (Gyro_Data.yaw< -18000) 
				 Gyro_Data.yaw =  Gyro_Data.yaw + 36000;
				
#endif
//				Gyro_Data.yaw=((int)(Gyro_Data.yaw))*(36000-385)/36000;
				#if 0
				Gyro_Data.pitch=(COMBUF3[6]<<8)+COMBUF3[5];
				Gyro_Data.roll=(COMBUF3[8]<<8)+COMBUF3[7];
				#else
//				temp_x=(COMBUF3[6]<<8)+COMBUF3[5];
//				temp_y=(COMBUF3[8]<<8)+COMBUF3[7];
				//Gyro_Data.x_pos=(short)((float)(temp_x)*1.724138);
				//Gyro_Data.y_pos=(short)((float)(temp_y)*1.724138);
				#endif
				Gyro_Data.x_acc=(COMBUF3[10]<<8)+COMBUF3[9];
				Gyro_Data.y_acc=(COMBUF3[12]<<8)+COMBUF3[11];
				Gyro_Data.z_acc=(COMBUF3[14]<<8)+COMBUF3[13];
				Gyro_Data.start_check_time=giv_sys_time;
			}
	}

	memset(USART3_RX_BUF,0,USART3_RX_SIZE);
	memset(COMBUF3,0,USART3_RX_SIZE);
	DMA_Cmd(DMA1_Channel3,ENABLE);
}

void Uart3_Communication_II(void)
{
	u16 cnt_checksum=0,rece_checksum=0,num=0;
	unsigned char COMBUF3[USART3_RX_SIZE];
//	short temp_x=0,temp_y=0;

	memcpy(COMBUF3,USART3_RX_BUF,USART3_RX_SIZE);
	
	DMA_Cmd(DMA1_Channel3,ENABLE);
#ifdef YIS055
	if((COMBUF3[0]==0x59)&&(COMBUF3[1]==0x49)&(COMBUF3[2]==0X53))
#else
	if((COMBUF3[0]==0XAA)&&(COMBUF3[1]==0XAA))
#endif

	{
#ifdef YIS055
		num=3;
#else
		num=2;
#endif

#ifdef YIS055
		while(num<USART3_RX_SIZE-2)
#else
		while(num<USART3_RX_SIZE-1)
#endif
		{
			cnt_checksum+=COMBUF3[num];
			num++;
		}
		
#ifdef YIS055
		rece_checksum=(COMBUF3[USART3_RX_SIZE-2]<<8)+COMBUF3[USART3_RX_SIZE-1];
#else
		rece_checksum=COMBUF3[USART3_RX_SIZE-1];
#endif

#ifdef YIS055
		if(cnt_checksum==rece_checksum)
#else
		if((cnt_checksum&0x00ff)==rece_checksum)
#endif
			{
			
#ifdef YIS055
				Gyro_Data.yaw=(COMBUF3[7]<<8)+COMBUF3[8];
#else
				Gyro_Data.yaw=(COMBUF3[4]<<8)+COMBUF3[3];
#endif

#ifdef GYRO_COMPENSATION
				long	yaw_temp ;
				yaw_temp = Gyro_Data.yaw -old_yaw;
				old_yaw = Gyro_Data.yaw;
			
				if (yaw_temp < -30000)	  // ������ ����180��ʱ��,	old_yaw Ϊ�� ,�µ�yawΪ��   , ��ô��Ҫ���µı��Ϊ����ֵ
					cnt_180 -=1;
				if (yaw_temp >	30000)	  //������ת���� 180�� 
					cnt_180 +=1;
				if(cnt_180 <0) 
					{
						if (Gyro_Data.yaw< -18000) 
						Gyro_Data.yaw =	Gyro_Data.yaw + 36000;
					}
				if(cnt_180 >0) 
					{
						if (Gyro_Data.yaw>  18000)  
						Gyro_Data.yaw =	Gyro_Data.yaw - 36000;
					}	
				Gyro_Data.yaw=((int)(Gyro_Data.yaw))*(36000+289)/36000;
				//Gyro_Data.yaw=((int)(Gyro_Data.yaw))*1.007;
				if (Gyro_Data.yaw> 18000) 
				 Gyro_Data.yaw	=  Gyro_Data.yaw - 36000;
				if (Gyro_Data.yaw< -18000) 
				 Gyro_Data.yaw =  Gyro_Data.yaw + 36000;
				
#endif
//				Gyro_Data.yaw=((int)(Gyro_Data.yaw))*(36000-385)/36000;
				#if 0
				Gyro_Data.pitch=(COMBUF3[6]<<8)+COMBUF3[5];
				Gyro_Data.roll=(COMBUF3[8]<<8)+COMBUF3[7];
				#else
//				temp_x=(COMBUF3[6]<<8)+COMBUF3[5];
//				temp_y=(COMBUF3[8]<<8)+COMBUF3[7];
				//Gyro_Data.x_pos=(short)((float)(temp_x)*1.724138);
				//Gyro_Data.y_pos=(short)((float)(temp_y)*1.724138);
				#endif
				Gyro_Data.x_acc=(COMBUF3[10]<<8)+COMBUF3[9];
				Gyro_Data.y_acc=(COMBUF3[12]<<8)+COMBUF3[11];
				Gyro_Data.z_acc=(COMBUF3[14]<<8)+COMBUF3[13];
				Gyro_Data.start_check_time=giv_sys_time;
			}
	}

	//memset(USART3_RX_BUF,0,USART3_RX_SIZE);
	memset(COMBUF3,0,USART3_RX_SIZE);
	//DMA_Cmd(DMA1_Channel3,ENABLE);
}

//============================================================================================
//��ʼ��IO ����1 
//bound:������
void URAT1_init(u32 bound)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	//mask
	
	UART1.Trans_Busy = false;	//��ʼ������1����æ��־λ
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No	;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������

#ifndef TUYA_WIFI
	USART_Cmd(USART1, ENABLE);					  //ʹ�ܴ��� 
	USART_ClearFlag(USART1, USART_FLAG_TC); 		//�����һ���ֽ��޷���������	
#else
	//mask
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//mask
#ifdef DMA_IRQ // DMA  + �����жϷ�ʽ 
	USART_Cmd(USART1,ENABLE);
	
	USART_ClearITPendingBit(USART1,USART_IT_IDLE);		//mask
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);		//mask
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);		//mask
	USART_ITConfig(USART1,USART_IT_TC,DISABLE); 		//mask
	USART_ClearFlag(USART1, USART_FLAG_TC); 		//�����һ���ֽ��޷���������
#else // ��ͨ�ж�ģʽ 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(USART1,USART_FLAG_TC);
    USART_Cmd(USART1, ENABLE); 
#endif
#endif	
}
//============================================================================================



//============================================================================================
/*DMA  UART1������������ */
//DataLength�������С
void DMA_Uart1Tx_Config(u16 DataLength)
{
	DMA_InitTypeDef DMA_InitStructure;
//#ifndef TUYA_WIFI	
	NVIC_InitTypeDef NVIC_InitStructure;
//#endif
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����ʱ��

	/* UART Trans DMA����*/	
  	DMA_DeInit(DMA_UART1_Tx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (u32)(&USART1->DR);  			//DMA�����ַ  UART1���ͼĴ���
	DMA_InitStructure.DMA_MemoryBaseAddr 			= (uint32_t)USART1_TX_BUF;  	//DMA�ڴ��ַ  UART1���ͻ�����  //QZ:����ԭ��ΪUSART1_RX_BUF
	DMA_InitStructure.DMA_DIR 								= DMA_DIR_PeripheralDST;  		//���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize 					= DataLength;  							//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;  	//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc 					= DMA_MemoryInc_Enable;  		//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte; 		//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode 								= DMA_Mode_Normal;  				//��������������ģʽ
	DMA_InitStructure.DMA_Priority 						= DMA_Priority_Medium; 			//DMAͨ�� xӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M 								= DMA_M2M_Disable;  				//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_UART1_Tx, &DMA_InitStructure);  				//����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
  	DMA_ITConfig(DMA_UART1_Tx, DMA_IT_TC, ENABLE);		  //ʹ�ܷ�������ж�
  	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);		  //ʹ��USART1��������
	DMA_Cmd(DMA_UART1_Tx,DISABLE);						  				//ʧЧDMA����
//#ifndef TUYA_WIFI

	NVIC_InitStructure.NVIC_IRQChannel	=	DMA1_Channel4_IRQn;		   				//DMA_UART1_Tx
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority					= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//#endif	
}
//============================================================================================





//============================================================================================
/*DMA  UART1������������ */
//DataLength�������С
void DMA_Uart1Rx_Config(unsigned short DataLength)
{
	DMA_InitTypeDef DMA_InitStructure;

/* UART Rev DMA����*/	
  	DMA_DeInit(DMA_UART1_Rx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (u32)(&USART1->DR);  			//DMA�����ַ  UART1���ͼĴ���
	DMA_InitStructure.DMA_MemoryBaseAddr 			= (u32)(USART1_RX_BUF);  		//DMA�ڴ��ַ  UART1���ջ�����
	DMA_InitStructure.DMA_DIR 								= DMA_DIR_PeripheralSRC;  		//���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_InitStructure.DMA_BufferSize 					= DataLength;  						//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;  	//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc 					= DMA_MemoryInc_Enable;  		//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte; 		//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode 								= DMA_Mode_Circular;  			//��������������ģʽ
	DMA_InitStructure.DMA_Priority 						= DMA_Priority_Medium; 			//DMAͨ�� xӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M 								= DMA_M2M_Disable;  			//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_UART1_Rx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
	
  	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);		  //ʹ��USART1��������
	DMA_Cmd(DMA_UART1_Rx,ENABLE);
}
//============================================================================================ 



//============================================================================================
//����һ��DMA����
void UART_TX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA_UART1_Tx, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ�� 
 	DMA_SetCurrDataCounter(DMA1_Channel4,UART1.TxdDataNum);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_UART1_Tx, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	  
//============================================================================================

void USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	//UART2.RevLength=128;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//ʹ��GPIOA,Gʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

#if 1
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
#endif

#if 0
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_Init(USART2,&USART_InitStructure);

#if 0
	USART_ClearITPendingBit(USART2,USART_IT_IDLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART2,USART_IT_TC,DISABLE);
#endif
	USART_Cmd(USART2,ENABLE);
	USART_ClearFlag(USART2, USART_FLAG_TC);		    //�����һ���ֽ��޷���������	
}

void USART2_DMA_TX_Init(u16 BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA_UART2_TX);	//USART2_TX DMA1 Channel7

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART2_TX_BUF;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize=BufferSize;//UART2.TxdDataNum;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA_UART2_TX,&DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	DMA_ClearFlag(DMA1_FLAG_TE7|DMA1_FLAG_GL7|DMA1_FLAG_HT7|DMA1_FLAG_TC7);
	DMA_ITConfig(DMA_UART2_TX,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA_UART2_TX,ENABLE);	
}


void USART2_DMA_RX_Init(u16 BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA_UART2_RX);	//USART2 RX DMA_Channel_6

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART2_RX_BUF;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize=BufferSize;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular; //QZ:DMA receive data in circular mode
	DMA_InitStructure.DMA_Priority=DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA_UART2_RX,&DMA_InitStructure);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	DMA_Cmd(DMA_UART2_RX,ENABLE);
}

void USART2_TX_DMA_Enable(void)
{
	DMA_Cmd(DMA_UART2_TX,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel7, UART2.TxdDataNum);
	DMA_Cmd(DMA_UART2_TX,ENABLE);
}

void DMA_USART2_TX_Length(u32 length)
{
	DMA_Cmd(DMA_UART2_TX, DISABLE);		//�ر�DMA,��ֹ�������������
	DMA_UART2_TX->CNDTR = length;			//��װ��
	DMA_Cmd(DMA_UART2_TX,ENABLE); 		//ʹ�ܴ���1��DMA����  
}

//============================================================================================
//USART3
void USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	//mask
	
//	UART3.RevLength=500;//452;	//QZ:LittelBird has 452 bytes in every packet

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��GPIOA,Gʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);

#if 0
	GPIO_InitStructure.GPIO_Pin=GYRO_RST_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
#endif

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_Init(USART3,&USART_InitStructure);
	
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);

	//mask
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//mask

	USART_Cmd(USART3,ENABLE);

	USART_ClearITPendingBit(USART3,USART_IT_IDLE);		//mask
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);		//mask
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);		//mask
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);			//mask
	USART_ClearFlag(USART3, USART_FLAG_TC);		    //�����һ���ֽ��޷���������
}

void USART3_DMA_TX_Init(u16 BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA_UART3_TX_Channel);

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART3->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART3_TX_BUF;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize=BufferSize;//UART3.TxdDataNum;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA_UART3_TX_Channel,&DMA_InitStructure);
	
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
	DMA_ClearFlag(DMA1_FLAG_TE2|DMA1_FLAG_GL2|DMA1_FLAG_HT2|DMA1_FLAG_TC2);
	DMA_Cmd(DMA_UART3_TX_Channel,ENABLE);	
}

void USART3_DMA_RX_Init(u16 BufferSize)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_DeInit(DMA_UART3_RX);

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)(&USART3->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr=(u32)USART3_RX_BUF;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize=BufferSize;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;	//QZ:DMA receive data in circular mode
	DMA_InitStructure.DMA_Priority=DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable; 
	DMA_Init(DMA_UART3_RX,&DMA_InitStructure);

	DMA_Cmd(DMA_UART3_RX,ENABLE);
}

void DMA_USART3_TX_Length(u32 length)
{
	DMA_Cmd(DMA_UART3_TX_Channel, DISABLE);		//�ر�DMA,��ֹ�������������
	DMA_UART3_TX_Channel->CNDTR = length;			//��װ��
	DMA_Cmd(DMA_UART3_TX_Channel,ENABLE); 		//ʹ�ܴ���1��DMA����  
}


