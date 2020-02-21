#include "AAA-include.h"

#define NODE_LEN sizeof(struct AREA_NODE)

struct AREA_NODE						//����ڵ�ṹ�壬���ڱ��浱ǰ�������Ϣ����������ڵ��������
{
	bool clean;							//����
	bool area_ok;						//������ɨ�����ɱ�־
	bool ymax_ok;						//YMAX����������ɱ�־
	bool ymin_ok;						//YMIN����������ɱ�־
	bool xmax_ok;						//XMAX����������ɱ�־
	bool xmin_ok;						//XMIN����������ɱ�־
	u8 area_num;						//�����ɨ���
	u8 exit_area_num;					//�˳�������
	s8 exit_gridx1;						//������ڵ�1 X����			��������˳�ĳ����ʱʹ��
	s8 exit_gridy1;						//������ڵ�1 Y����
	s8 exit_gridx2;						//������ڵ�2 X����
	s8 exit_gridy2;						//������ڵ�2 Y����
	short exit_tgtyaw;
	s8 gridx_area_start;
	s8 gridy_area_start;
	u8 y_acc;
	u8 x_acc;
	struct AREA_NODE *last_node;
	struct AREA_NODE *next_node;
};

struct AREA_NODE *head_node;
struct AREA_NODE *curr_node;

/*-------------------------------------------
function:��������ڵ�NODE��ͷָ��head_node
output:
       0--->����ɹ�
       1--->����ʧ��
---------------------------------------------*/
u8 Creat_AreaNodeList(void)
{
	if(head_node!=NULL)
		{
#ifdef DEBUG_AREALIST
			TRACE("head_node is exist!!\r\n");
#endif
			if(head_node->last_node!=NULL)
				head_node->last_node=NULL;
			if(head_node->next_node!=NULL)
				{
					if(Del_All_AreaNode())
						{
#ifdef DEBUG_AREALIST
							TRACE("can't del all areanode!!\r\n");
#endif
							return 1;
						}
				}
			return 0;
		}
	head_node=(struct AREA_NODE*)malloc(NODE_LEN);
//	curr_node=(struct AREA_NODE*)malloc(NODE_LEN);
	if(head_node==NULL)
		{
#ifdef DEBUG_AREALIST
			TRACE("Area HEAD Node Dynamic memory allocation failure!!!\r\n");
#endif
			return 1;
		}
	head_node->area_num=0;
	head_node->next_node=NULL;
	head_node->last_node=NULL;
	head_node->clean=false;
	curr_node=head_node;
#ifdef DEBUG_AREALIST
	TRACE("Creat AreaNodeList Complete!!\r\n");
#endif
	return 0;
}

struct AREA_NODE* Return_CurrNode(void)
{
	struct AREA_NODE *p;
	while(p->next_node!=NULL)
		{
			p=p->next_node;
		}
	return p;
}

u8 Add_AreaNode_End(void)
{
	struct AREA_NODE *p,*q;
	p=(struct AREA_NODE*)malloc(NODE_LEN);
	if(p==NULL)
		{
#ifdef DEBUG_AREALIST
			TRACE("Area Node Dynamic memory allocation failure!!!\r\n");
#endif
			return 1;
		}
	
	q=head_node;
	while(q->next_node!=NULL)
		q=q->next_node;
		
	p->area_num=motion1.area_num;
	p->exit_area_num=motion1.exit_area_num;
	p->clean=false;
	p->gridx_area_start=grid.x_area_start;
	p->gridy_area_start=grid.y_area_start;
	p->y_acc=motion1.y_acc;
	p->x_acc=motion1.x_acc;
	p->area_ok=motion1.area_ok;
	p->ymax_ok=motion1.ymax_ok;
	p->ymin_ok=motion1.ymin_ok;
	p->xmax_ok=motion1.xmax_ok;
	p->xmin_ok=motion1.xmin_ok;
	
	//p->last_node=curr_node;
	q->next_node=p;
	p->last_node=q;
	p->next_node=NULL;
	
	curr_node=p;
#ifdef DEBUG_AREALIST
	TRACE("Add a AreaNode complete!!\r\n");
#endif
	return 0;
}

u8 Del_AreaNode_End(void)
{
	struct AREA_NODE *p;
	if(curr_node->area_num==0)
		{
#ifdef DEBUG_AREALIST
			TRACE("head node can't be del!!!\r\n");
#endif
			return 0;
		}
	p=curr_node;									//�ҵ���ǰ����ָ��
	curr_node=curr_node->last_node;					//��curr_node�ƶ�����ǰָ�����һ��ָ��
	free(p);										//ɾ��ָ��
	curr_node->next_node=NULL;						//��ǰָ�����һָ��ָ���
#ifdef DEBUG_AREALIST
	TRACE("Del a AreaNode complete!!\r\n");
#endif
	return 0;
}

u8 Del_All_AreaNode(void)
{
	struct AREA_NODE *p,*q;
	p=head_node;
	while(p->next_node!=NULL)			//��ȥ�����һ���ڵ�
		{
			p=p->next_node;
		}
	while(p->last_node!=NULL)			//���ж����һ���ڵ��Ƿ�path_head
		{
			q=p->last_node;				//�ҵ���һ���ڵ�
			free(p);					//�ͷ����һ���ڵ�
			p=q;						//ǰ����һ���ڵ㣬֮ǰ����һ���ڵ��Ѿ������һ���ڵ㣬׼���ٴ��ͷ�
			p->next_node=NULL;			//			
		}
	if(p!=head_node)
		{
#ifdef DEBUG_AREALIST
			TRACE("Delete All AreaNode fail!\r\n");
#endif
			return 1;
		}
	head_node->next_node=NULL; 				//����p��head_point������next����Ϊ��
#ifdef DEBUG_AREALIST
	TRACE("Delete All AreaNode success!\r\n");
#endif
	return 0;
}

void Logout_CurrNode(void)
{
	TRACE("curr_node.area_num=%d\r\n",curr_node->area_num);
}

void Load_CurrNode_Info(void)
{	
	if(curr_node->area_num==1)
		{
			curr_node->exit_gridx1=motion1.xpos_start;
			curr_node->exit_gridy1=motion1.ypos_start;
			curr_node->exit_gridx2=motion1.xpos_start;
			curr_node->exit_gridy2=motion1.ypos_start;
			curr_node->exit_tgtyaw=F_Angle_Const;
		}
	else
		{
			curr_node->exit_gridx1=check_point.new_x2;
			curr_node->exit_gridy1=check_point.new_y2;
			curr_node->exit_gridx2=check_point.new_x1;
			curr_node->exit_gridy2=check_point.new_y1;
			//temp_yaw=Get_Reverse_Angle(check_point.next_tgtyaw);
			//curr_node->exit_tgtyaw=temp_yaw;
			curr_node->exit_tgtyaw=check_point.next_tgtyaw;
		}
	
	curr_node->gridx_area_start=grid.x_area_start;
	curr_node->gridy_area_start=grid.y_area_start;
	curr_node->x_acc=motion1.x_acc;
	curr_node->y_acc=motion1.y_acc;
	curr_node->area_ok=motion1.area_ok;
	curr_node->ymax_ok=motion1.ymax_ok;
	curr_node->ymin_ok=motion1.ymin_ok;
	curr_node->xmax_ok=motion1.xmax_ok;
	curr_node->xmin_ok=motion1.xmin_ok;
	curr_node->exit_area_num=motion1.exit_area_num;
#ifdef DEBUG_AREALIST
	TRACE("curr_node.exit_gridx1=%d\r\n",curr_node->exit_gridx1);
	TRACE("curr_node.exit_gridy1=%d\r\n",curr_node->exit_gridy1);
	TRACE("curr_node.exit_gridx2=%d\r\n",curr_node->exit_gridx2);
	TRACE("curr_node.exit_gridy2=%d\r\n",curr_node->exit_gridy2);
	TRACE("curr_node.exit_tgtyaw=%d\r\n",curr_node->exit_tgtyaw);
	TRACE("curr_node.exit_area_num=%d\r\n",curr_node->exit_area_num);
#endif
}

void Get_CurrNode_Info(void)
{
	grid.x_area_start=curr_node->gridx_area_start;
	grid.y_area_start=curr_node->gridy_area_start;
	motion1.x_acc=curr_node->x_acc;
	motion1.y_acc=curr_node->y_acc;
	motion1.area_num=curr_node->area_num;
	motion1.area_ok=curr_node->area_ok;
	
	motion1.ymax_ok=curr_node->ymax_ok;
	motion1.ymin_ok=curr_node->ymin_ok;
	motion1.xmax_ok=curr_node->xmax_ok;
	motion1.xmin_ok=curr_node->xmin_ok;
	motion1.exit_area_num=curr_node->exit_area_num;
#ifdef DEBUG_AREALIST
	TRACE("Get CurrNode Info!!!\r\n");
	TRACE("CurrNode.area_num=%d\r\n",curr_node->area_num);
	TRACE("motion1.area_num=%d\r\n",motion1.area_num);
	TRACE("grid.x_area_start=%d\r\n",grid.x_area_start);
	TRACE("grid.y_area_start=%d\r\n",grid.y_area_start);
	TRACE("motion1.x_acc=%d\r\n",motion1.x_acc);
	TRACE("motion1.y_acc=%d\r\n",motion1.y_acc);
	TRACE("motion1 ymax check %d\r\n",motion1.ymax_ok);
	TRACE("motion1 ymin check %d\r\n",motion1.ymin_ok);
	TRACE("motion1 xmax check %d\r\n",motion1.xmax_ok);
	TRACE("motion1 xmin check %d\r\n",motion1.xmin_ok);
	TRACE("motion1 exit_area_num=%d\r\n",motion1.exit_area_num);
#endif
}

void Get_Node_Info(struct AREA_NODE *p)
{
	grid.x_area_start=p->gridx_area_start;
	grid.y_area_start=p->gridy_area_start;
	motion1.x_acc=p->x_acc;
	motion1.y_acc=p->y_acc;
	motion1.area_num=p->area_num;
	motion1.area_ok=p->area_ok;
	
	motion1.ymax_ok=p->ymax_ok;
	motion1.ymin_ok=p->ymin_ok;
	motion1.xmax_ok=p->xmax_ok;
	motion1.xmin_ok=p->xmin_ok;
	motion1.exit_area_num=p->exit_area_num;
}

u8 Out_CurrNode_Exit(void)
{
#ifdef DEBUG_AREALIST
	TRACE("curr_node.exit_gridx1=%d\r\n",curr_node->exit_gridx1);
	TRACE("curr_node.exit_gridy1=%d\r\n",curr_node->exit_gridy1);
	TRACE("curr_node.exit_gridx2=%d\r\n",curr_node->exit_gridx2);
	TRACE("curr_node.exit_gridy2=%d\r\n",curr_node->exit_gridy2);
#endif
	check_point.new_x1=curr_node->exit_gridx1;
	check_point.new_y1=curr_node->exit_gridy1;
	check_point.new_x2=curr_node->exit_gridx2;
	check_point.new_y2=curr_node->exit_gridy2;
	check_point.next_tgtyaw=curr_node->exit_tgtyaw;	
//	check_point.go_exit=true;
	motion1.exit_gridx1=curr_node->exit_gridx1;
	motion1.exit_gridy1=curr_node->exit_gridy1;
	motion1.exit_gridx2=curr_node->exit_gridx2;
	motion1.exit_gridy2=curr_node->exit_gridy2;

	Set_CheckPoint_NextAction(CHECK_GOEXIT);
	return 1;
}

void Init_CurrNode_Exit(void)
{
	curr_node->exit_gridx1=0;
	curr_node->exit_gridy1=0;
	curr_node->exit_gridx2=0;
	curr_node->exit_gridy2=0;
	curr_node->exit_tgtyaw=F_Angle_Const;
}

u8 Read_CurrNode_AreaNO(void)
{
	return (curr_node->area_num);
}

u8 Set_CurrNode_LeakInfo(bool area_ok)
{
	curr_node->area_ok=area_ok;
	return 0;
}

/*------------------------------------------------
function:����ǰ�����������־������curr_node
input:
      area_ok-- 0:��Ҫ��飬1:������
      num--  1:YMAX����
             2:XMAX����
             3:YMIN����
             4:XMIN����
--------------------------------------------------*/
u8 Set_CurrNode_NewAreaInfo(bool area_ok,u8 num)
{
	switch(num)
		{
			case DIR_YMAX:				//YMAX����
				curr_node->ymax_ok=area_ok;
				break;
			case DIR_XMAX:				//XMAX����
				curr_node->xmax_ok=area_ok;
				break;
			case DIR_YMIN:
				curr_node->ymin_ok=area_ok;
				break;
			case DIR_XMIN:
				curr_node->xmin_ok=area_ok;
				break;
			default:
				break;
		}
	return 0;
}

u8 Set_Curr_AllNewAreaOK(void)
{
	motion1.ymax_ok=true;
	motion1.ymin_ok=true;
	motion1.xmax_ok=true;
	motion1.xmin_ok=true;
	curr_node->ymax_ok=motion1.ymax_ok;
	curr_node->ymin_ok=motion1.ymin_ok;
	curr_node->xmax_ok=motion1.xmax_ok;
	curr_node->xmin_ok=motion1.xmin_ok;
	return 0;
}

u8 Analysis_AllNode_NewArea(void)
{
	struct AREA_NODE *p;
	p=head_node;
	while(p->next_node!=NULL)
		{
			p=p->next_node;

			Get_Node_Info(p);
			Cal_PosArea_Max();
			
			if(Analysis_Boundary_X_II(0)!=0x7f)
				return 1;
			if(Analysis_Boundary_X_II(1)!=0x7f)
				return 1;
			if(Analysis_Boundary_Y_II(0)!=0x7f)
				return 1;
			if(Analysis_Boundary_Y_II(1)!=0x7f)
				return 1;
		}
	return 0;	
}
