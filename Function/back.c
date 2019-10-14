#include "AAA-include.h"


struct BACK_INFO *back_head,*curr_back;

u8 Init_BackHead(void)
{
	if(back_head!=NULL)
		{
			TRACE("back head is exist!!\r\n");
			if(back_head->last_info!=NULL);
				back_head->last_info=NULL;
			if(back_head->next_info!=NULL)
				{
					if(Del_All_BackInfo())
						{
							TRACE("Del all back info fail !!!\r\n");
							return 1;
						}
				}
			return 0;
		}
	
	back_head=(struct BACK_INFO*)malloc(BACK_INFO_LEN);
	back_head->num=0;
	back_head->last_info=NULL;
	back_head->next_info=NULL;
	curr_back=back_head;
	TRACE("back head malloc success!!\r\n");
	return 0;
}

u8 Add_BackInfo(void)
{
	struct BACK_INFO *p,*q;
	p=(struct BACK_INFO*)malloc(BACK_INFO_LEN
);
	if(p==NULL)
		{
			TRACE("BackInfo malloc fail!!!\r\n");
			return 1;
		}
	p->tgt_yaw=motion1.tgt_yaw;
	p->anti_tgt_yaw=motion1.anti_tgt_yaw;
	p->back_sweep=motion1.back_sweep;
	p->leftright=motion1.leftright;
	p->ydir=motion1.y_dir;
	p->repeat_sweep=motion1.repeat_sweep;
	p->gridy_abort=grid.y;
	p->gridx_abort=grid.x;

	q=back_head;
	while(q->next_info!=NULL)
		{
			q=q->next_info;
		}
	q->next_info=p;
	p->last_info=q;
	p->next_info=NULL;
	curr_back=p;

	TRACE("BacInfo Add success!!!\r\n");
	return 0;
}

u8 Delete_CurrBackInfo(void)
{
	struct BACK_INFO *p,*q;
	p=back_head;
	while(p->next_info!=NULL)
		{
			p=p->next_info;
		}
	if(p==back_head)
		{
			return 0;
		}

	q=p->last_info;
	q->next_info=NULL;
	free(p);
	curr_back=q;
	TRACE("delete curr back success!!!\r\n");
	return 0;
}

void Get_Curr_BackInfo(void)
{
	motion1.tgt_yaw=curr_back->tgt_yaw;
	motion1.anti_tgt_yaw=curr_back->anti_tgt_yaw;
	motion1.y_dir=curr_back->ydir;
	motion1.leftright=curr_back->leftright;
	motion1.back_sweep=curr_back->back_sweep;
	motion1.repeat_sweep=curr_back->repeat_sweep;
	grid.y_abort=curr_back->gridy_abort;
	grid.x_abort=curr_back->gridx_abort;
	Delete_CurrBackInfo();
}

u8 Del_All_BackInfo(void)
{
	struct BACK_INFO *p,*q;
	p=back_head;
//	TRACE("Enter in Delete All PathPoint!!\r\n");
	while(p->next_info!=NULL)			//��ȥ�����һ���ڵ�
		{
			p=p->next_info;
		}
	while(p->last_info!=NULL)			//���ж����һ���ڵ��Ƿ�path_head
		{
			q=p->last_info; 			//�ҵ���һ���ڵ�
			free(p);					//�ͷ����һ���ڵ�
			p=q;						//ǰ����һ���ڵ㣬֮ǰ����һ���ڵ��Ѿ������һ���ڵ㣬׼���ٴ��ͷ�
			p->next_info=NULL;			//			
		}
	if(p!=back_head)
		{
			TRACE("Delete All BackInfo fail!\r\n");
			return 1;
		}
	back_head->next_info=NULL;				//����p��head_point������next����Ϊ��
	TRACE("Delete All BackInfo success!\r\n");
	return 0;
}

