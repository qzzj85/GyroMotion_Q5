#ifndef BACK_H
#define BACK_H

struct BACK_INFO
{
	bool area_ok;				//当前区域清扫检查是否完成，如果清扫检查完毕，以后不再检查漏扫
	bool repeat_sweep;			//重复扫标志，true:重复扫，false：无需要，沿边出现空旷区域时重复扫
	u8 back_sweep;				//回扫标志，true:回扫,false:正常扫
	u8 leftright;				//左右沿边标志，0：左沿边，1：右沿边
	u8 num;
	s8 ydir;					//Y轴清扫时的增长方向(0:负方向,1:正方向),因为y_acc有0,1,2三个值，2代表双方向清扫，无法再用y_acc表示目前正在清扫的方向
	s8 gridy_abort;
	s8 gridx_abort;
	short tgt_yaw;				//直线清扫时目标角度
	short anti_tgt_yaw;			//直线清扫时目标反角度

	struct BACK_INFO* last_info;
	struct BACK_INFO* next_info;
};


#define BACK_INFO_LEN (sizeof(struct BACK_INFO))


void Get_Curr_BackInfo(void);
u8 Add_BackInfo(void);
u8 Init_BackHead(void);
u8 Del_All_BackInfo(void);

#endif
