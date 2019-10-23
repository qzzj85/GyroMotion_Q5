#ifndef __REMOTE_H
#define	__REMOTE_H

////////////冠唯遥控编码/////////////
#define		REMOTE_SLEEP			0XAF
#define		REMOTE_DOCK				0XAC
#define		REMOTE_GUIHUA			0XB0
#define		REMOTE_FORWOARD			0XBE
#define		REMOTE_LEFT				0XBA
#define		REMOTE_OK				0X8A
#define		REMOTE_RIGHT			0XB8
#define		REMOTE_BACK				0XBC
#define		REMOTE_YBS				0XB2
#define		REMOTE_SPOT				0XB5
#define		REMOTE_TIME				0XB4
#define		REMOTE_SET				0XB3
#define		REMOTE_PREEN			0X2A
#define		REMOTE_START			0X85
#define		REMOTE_FAN				0XB5

typedef enum
{
	REMOTE_KEY_NONE=0,
	REMOTE_KEY_SLEEP,
	REMOTE_KEY_DOCK,
	REMOTE_KEY_GUIHUA,
	REMOTE_KEY_FORWORD,
	REMOTE_KEY_LEFT,
	REMOTE_KEY_OK,
	REMOTE_KEY_RIGHT,
	REMOTE_KEY_BACK,
	REMOTE_KEY_YBS,
	REMOTE_KEY_SPOT,
	REMOTE_KEY_TIME,
	REMOTE_KEY_SET,
	REMOTE_KEY_PREEEN,
	REMOTE_KEY_START,
	REMOTE_KEY_FAN,
}REMOTE_KEY;


typedef struct
{
	bool effect;
	u32 effect_time;
	u32 key_time;				//收到遥控按键时的时间
	REMOTE_KEY remote_key;		//表示收到的哪个遥控按键
	RECE_IR rece_ir;			//标记哪个接收头收到遥控信息
}REMOTE_INFO;

extern REMOTE_INFO remote_info;
extern u32 voice_time;

void Init_Remote_Info(void);
void Clr_Remote_Info(void);
void Remote_Handle(void);
void Init_Remote_Move(void);
void Do_Remote_Move(void);

#endif



