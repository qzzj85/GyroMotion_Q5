#ifndef BACK_H
#define BACK_H

struct BACK_INFO
{
	bool area_ok;				//��ǰ������ɨ����Ƿ���ɣ������ɨ�����ϣ��Ժ��ټ��©ɨ
	bool repeat_sweep;			//�ظ�ɨ��־��true:�ظ�ɨ��false������Ҫ���ر߳��ֿտ�����ʱ�ظ�ɨ
	u8 back_sweep;				//��ɨ��־��true:��ɨ,false:����ɨ
	u8 leftright;				//�����ر߱�־��0�����رߣ�1�����ر�
	u8 num;
	s8 ydir;					//Y����ɨʱ����������(0:������,1:������),��Ϊy_acc��0,1,2����ֵ��2����˫������ɨ���޷�����y_acc��ʾĿǰ������ɨ�ķ���
	s8 gridy_abort;
	s8 gridx_abort;
	short tgt_yaw;				//ֱ����ɨʱĿ��Ƕ�
	short anti_tgt_yaw;			//ֱ����ɨʱĿ�귴�Ƕ�

	struct BACK_INFO* last_info;
	struct BACK_INFO* next_info;
};


#define BACK_INFO_LEN (sizeof(struct BACK_INFO))


void Get_Curr_BackInfo(void);
u8 Add_BackInfo(void);
u8 Init_BackHead(void);
u8 Del_All_BackInfo(void);

#endif
