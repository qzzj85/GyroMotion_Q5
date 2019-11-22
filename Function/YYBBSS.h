#ifndef YYBBSS_H_
#define YYBBSS_H_


#define TURN_SPEED 		MID_MOVE_SPEED//500//1000
//qz add 20180314
#define		CLIFF_BACK_LENGTH		5	//cm


#define     YBS_DISTANCE_CONST          30		//	毫米
#define		YBS_DISTANCE_LONG			160


//qz add 20180319
#define 	YBS_MAIN_LOWSPEED_H			1000//1500//2500
#define 	YBS_MAIN_LOWSPEED_L			200//175//300
#define 	YBS_SLAVE_LOWSPEED_H			800//1300//2200
#define 	YBS_SLAVE_LOWSPEED_L			700//1100//1900

//#define 	YBS_Straight_FAST			
#ifdef		YBS_Straight_FAST
	#define 	YBS_MAIN_HIGHSPD_H			2000//3500
	#define		YBS_MAIN_HIGHSPD_L			300//500
	#define		YBS_SLAVE_HIGHSPD_H			1900//3200
	#define		YBS_SLAVE_HIGHSPD_L			1700//2900
#endif

#define     YBS_FORWARD_SPEED    			700//1200   
#define     YBS_LOST_CORNER_FORWARD_SPEED   MID_MOVE_SPEED//2400 
#define     YBS_LOST_CORNER_TURN_SPD	   	1000//2200//2000//3500 
#define		YBS_LOST_CORNER_TURN_SPD_R		290//600;//400//750
#define     YBS_KP           10000//10000 	//10000
#define     YBS_KI           30  
#define     YBS_KD           50 

#define		YBS_FKP			0.5//0.05//1//0.2	
#define		YBS_FKI			0.2//0.03//0.1
#define		YBS_FKD			0.0125//0.05//0.0125


//20180814在冠唯现场做出调整
								//5#					//7#					//8#		
								//白		//灰							//白			//灰				//纸箱		墙
			//>200				//0x0F					//0x13		0x0D						//0x19		
#define		VALUE200	180//15//30//0x30	//0X30					//			0x32		0x32			//0x32		0x28
#define		VALUE190	190
#define		VALUE180	220
#define		VALUE170	240
#define		VALUE160	260
#define		VALUE150	300//60//0x50	//0X50					//0x50		0x50		0x4A			//0x4B		0x45
#define		VALUE140	360
#define		VALUE130	400
#define		VALUE120	430//90//0x60	//0X70					//0x70		0x71		0x60			//0x60		0x5A
#define		VALUE110	530
#define		VALUE100	590//130//0x80	//0X90					//0x90		0x100		0x80			//0x7C		0x82
#define		VALUE90		690//160//0x100	//0XB0					//0xB0		0XB8		0x96			//0x90		0x96
#define		VALUE80		860//200//0xB0	//0XE0					//0xD0		0xEA		0xB0			//0XB0		0xBE
#define		VALUE70		1090//250//0XE0	//0x110					//0x100		0x120		0xD9			//0xD0		0XE6
#define		VALUE60		1260//350//0x110	//0X160					//0x150		0x16A		0X110			//0x110		0x12C	
#define		VALUE50		1720//500//0x150	//0X1C0					//0x1C0		0x1F0		0X160			//0x150		0X190
#define		VALUE40		2060//700//0x1E0	//0X260					//0x290		0x2C0		0X1D0			//0x1C0		0X212
#define		VALUE30		2120//900//0x280	//0X340					//0x3B0		0x3D0		0X260			//0x250		0X2F8
#define		VALUE20		2140//1100//0x360	//0X460					//0x520		0x530		0X350			//0x350		0X3E8
#define		VALUE10		2150//1200//0x420	//0X620					//0x680		0x630		0X420			//0x440		0X4D8
#define		VALUE05		2160//0X450	//												0X440			//0x475		0X4F6

#define CONST_DIS 			60

extern bool  	YBS_Check_Flag;
extern bool  	YBS_Conor_Check_Flag; 
extern bool		YBS_check_base;
extern bool		YBS2leftright;	//0变换为左沿边，1变换为右沿边


extern u32 bump_time[2];

extern s16 lost_flag_angle;
extern u16 lost_turn_angle;
extern u32 YBS_Straight_Time;
extern u16 YBS_Wall_Distance;
extern int Wall_lost_Start_Pos;
extern u32 lost_turn_time;

extern u8 YBS_DISTANCE;
extern u32 rightbumptime;
extern u8 rightbumpcnt;			
extern bool	ybs_first_call;
extern bool YBS_rap_time;
uint16_t  Return_Distance(uint16_t data);
void  	YBS_Left_Distance_Check(void);
void  	YBS_Left_Check_corner(void);
void 	YBS_YBS(void);
void 	Init_Right_YBS(u8 direct_first);
void 	Init_Left_YBS(u8 direct_first);

u8 		YBS_YaoKong_Manage(void);	
u32 	YBS_read_bump1(u8 out_enable);
void 	YBS_Distance_Check(uint8_t Left_Right);
void  	YBS_Check_corner(void);
void  	YBS_comm_rap(void);
void  	YBS_enable_rap(u8 Left_or_Right,	u16 YBS_Speed,	u32 max_length);
void 	YBS_stop(void);
void 	YBS_Left_Bump(u8 out_enable);
void 	YBS_Wall_Count(void);
void 	Enable_Wall_Count(void);
void YBS_Right_Bump(u8 out_enable);
u8 Parse_ContinueInYBS(s8 now_gridx,s8 now_gridy);
u8 AngleRepeat_InYBS(void);
u8 Init_YBS_Exchange(u8 next_ybs_mode);
void Do_YBS_Exchange(void);
u8 Analysis_NeedBack_YBS(s8 ygrid_abort);
u8 Check_OutRange_Clean_YBS(u8 data);
u32 Check_OutofRange_YBS(u8 out_enable);

#endif


