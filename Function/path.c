#include "AAA-include.h"

#define PATHPOINT_LEN (sizeof(struct POINT_GRID))
#define POINTGRID_LEN (sizeof(struct POINT_GRID))
#define	LEN_PATHPOINT (sizeof(struct PATH_POINT))

PATH_STARTEND startend_last,startend_now;		//·����������һ���յ���Ϣ����ε����յ���Ϣ
struct PATH_POINT *path_head;					
struct PATH_POINT *path_end;
POINT_GRID turn_grid;							//YBS����������·�������ת��
u32 path_length=0;								//·�����������ߵ����
u32 last_path_length=0;
//POINT_GRID last_grid[100];

void Init_LastPointGrid(POINT_GRID *p)
{
	for(int i=0;i<100;i++)
		{
			p->data=0;
			p->gridx=0x7f;
			p->gridy=0x7f;
			p++;
		}
}

/*---------------------------------------------------
function���Ƚ���һ�ε�PATH_POINT����һ���Ƿ�һ��
return:
		0:��һ��
		1:һ��
-----------------------------------------------------*/
u8 Compare_PointGrid(POINT_GRID *p,POINT_GRID *q)
{
	int i=0;
	if(last_path_length==0)
		{
			TRACE("last point grid is empty,return 0!!!\r\n");
			return 0;
		}
	if((path_length!=last_path_length))
		return 0;
	while(i<path_length)
		{
			if((p->data)==(q->data))
				{
					if(((p->gridx)==(q->gridx))&((p->gridy)==(q->gridy)))
						{
							p++;
							q++;
							i++;
						}
					else
						{
							return 0;
						}
				}
			else
				{
					return 0;
				}
		}
	return 1;
}

u8 Path_Repeat_Check(PATH_STARTEND *last,PATH_STARTEND *now)
{
	if(path_length==0)
		{
			TRACE("This is the first use path_point,no need compare!!\r\n");
			return 0;
		}
	TRACE("Enter in %s...\r\n",__func__);
	TRACE("last start grid.x=%d y=%d\r\n",last->start_grid.gridx,last->start_grid.gridy);
	TRACE("now start grid.x=%d y=%d\r\n",now->start_grid.gridx,now->start_grid.gridy);
	TRACE("last tgt grid.x=%d y=%d\r\n",last->tgt_grid.gridx,last->tgt_grid.gridy);
	TRACE("now tgt grid.x=%d y=%d\r\n",now->tgt_grid.gridx,now->tgt_grid.gridy);
	if((last->start_grid.gridx==now->start_grid.gridx)&(last->start_grid.gridy==now->start_grid.gridy))
		{
			if((last->tgt_grid.gridx==now->tgt_grid.gridx)&(last->tgt_grid.gridy==now->tgt_grid.gridy))
				return 1;
		}
	
	return 0;
}

void Load_LastPoinGrid(POINT_GRID *lastgrid,POINT_GRID *nowgrid)
{
	last_path_length=path_length;
	for(int i=0;i<path_length;i++)
		{
			*lastgrid=*nowgrid;
			lastgrid++;
			nowgrid++;
		}
}

u8 Init_PathPoint(void)
{
	path_head=(struct PATH_POINT*)malloc(LEN_PATHPOINT);
	if(path_head==NULL)
		{
			TRACE("path head malloc fail!!\r\n");
			return 1;
		}
	path_head->last_point=NULL;
	path_head->next_point=NULL;
	path_head->path_grid.data=0;
//	Init_LastPointGrid();
	return 0;
}

u8 Delete_All_PathPoint(void)
{
	struct PATH_POINT *p,*q;
	p=path_head;
//	TRACE("Enter in Delete All PathPoint!!\r\n");
	while(p->next_point!=NULL)			//��ȥ�����һ���ڵ�
		{
			p=p->next_point;
		}
	while(p->last_point!=NULL)			//���ж����һ���ڵ��Ƿ�path_head
		{
			q=p->last_point;			//�ҵ���һ���ڵ�
			free(p);					//�ͷ����һ���ڵ�
			p=q;						//ǰ����һ���ڵ㣬׼���ٴ��ͷ�
		}
	if(p!=path_head)
		{
			TRACE("Delete All PathPoint fail!\r\n");
			return 1;
		}
	p->next_point=NULL;					//����p��head_point������next����Ϊ��
	TRACE("Delete All PathPoint success!\r\n");
	return 0;
}

/*------------------------------------------
function:����PATHPOINT�ڵ�
output: 0--->���ӳɹ�
        1--->����ʧ��
--------------------------------------------*/
u8 Add_PathPoint(s8 gridx,s8 gridy,u8 data)
{
	struct PATH_POINT *p,*q;
	p=path_head;
	while(p->next_point!=NULL)			//��ȥ�����һ���ڵ�
		{
			p=p->next_point;
		}
	q=(struct PATH_POINT*)malloc(LEN_PATHPOINT);
	if(q==NULL)
		{
			TRACE("PATH POINT malloc fail in %s\r\n",__func__);
			return 1;
		}
	p->next_point=q;					//���һ���ڵ��nextָ��ָ���·��䵥Ԫ
	q->last_point=p;					//�·��䵥Ԫ��lastָ��ָ�����һ���ڵ�
	q->next_point=NULL;					//�·��䵥Ԫ��nextָ��ָ��NULL����ɺ��·��䵥Ԫ��Ϊ���һ���ڵ�
	q->path_grid.data=data;
	q->path_grid.gridx=gridx;
	q->path_grid.gridy=gridy;
	return 0;
}

struct PATH_POINT* Get_PathPoint(u8 data)
{
	struct PATH_POINT *p;
	//p=(struct PATH_POINT*)malloc(LEN_PATHPOINT);
	p=path_head;
	while(p->next_point!=NULL)
		{
			if(p->path_grid.data==data)
				break;
			else
				p=p->next_point;
		}
	if(p->path_grid.data==data)
		return p;
	//free(p);
	else
		return NULL;
}

u8 IS_Can_ReachPointX_NoWall(s8 now_gridx,s8 tgt_gridx,s8 now_gridy)
{
	s8 temp_gridx,temp_gridy;
	s8 temp_xmin,temp_xmax;
	temp_gridx=now_gridx;temp_gridy=now_gridy;

	TRACE("enter in %s\r\n",__func__);
	if(temp_gridx==tgt_gridx)
		return 1;

	while(temp_gridx!=tgt_gridx)											//Ŀ����뵱ǰ��X���겻һ��
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))				//��ǰ���Ƿ���ɨ��
				{
					if(temp_gridx>tgt_gridx)
						temp_gridx--;
					else
						temp_gridx++;
				}
			else															//��ǰ��û�б���ɨ��
				{
					temp_xmax=Return_MaxClean_GridX(temp_gridy,0);			//��ǰY���������X��ɨ��
					temp_xmin=Return_MinClean_GridX(temp_gridy,0);			//��ǰY����С��СX��ɨ��
					TRACE("girdy=%d Xmax Clean=%d Xmin Clean=%d\r\n",temp_gridy,temp_xmax,temp_xmin);
					for(temp_gridx=temp_xmin;temp_gridx<=temp_xmax;temp_gridx++)
						{
							TRACE("grid[%d][%d]=%d\r\n",temp_gridy,temp_gridx,Read_Coordinate_Clean(temp_gridx,temp_gridy));
						}
					break;
				}
		}

	if(temp_gridx==tgt_gridx)
		{
	
			TRACE("return 1 in %s\r\n",__func__);
			return 1;
		}
	else
		{
			TRACE("return 0 in %s\r\n",__func__);
			return 0;
		}
}

s8 Find_Adjoin_Gap_NoWall(s8 gridx1,s8 gridy1,s8 gridy2)
{
	s8 max_cleanx1,min_cleanx1,temp_gridx1;

	max_cleanx1=Return_MaxClean_GridX(gridy1,0);
	min_cleanx1=Return_MinClean_GridX(gridy1,0);
//	TRACE("enter in %s...\r\n",__func__);
//	TRACE("gridx1=%d gridy1=%d gridy2=%d\r\n",gridx1,gridy1,gridy2);
//	TRACE("gridy1 Xmax=%d Xmin=%d\r\n",max_cleanx1,min_cleanx1);
#if 0
	for(temp_gridx1=gridx1;temp_gridx1<max_cleanx1;temp_gridx1++)
		{
			if(Read_Coordinate_Clean(temp_gridx1,gridy1)&(Read_Coordinate_Clean(temp_gridx1+1,gridy1)))
				{
					if(Read_Coordinate_Clean(temp_gridx1,gridy2)&(Read_Coordinate_Clean(temp_gridx1+1,gridy2)))
						{
							//return temp_gridx1;
							TRACE("comm point x=%d\r\n",temp_gridx1);
							if(IS_Can_ReachPoint_X(gridx1, temp_gridx1, gridy1))
								return temp_gridx1;
						}
				}
		}
	TRACE("now&now+1 point to max checkout!!!\r\n");

	for(temp_gridx1=gridx1;temp_gridx1>min_cleanx1;temp_gridx1--)
		{
			if(Read_Coordinate_Clean(temp_gridx1,gridy1)&(Read_Coordinate_Clean(temp_gridx1-1,gridy1)))
				{
					if(Read_Coordinate_Clean(temp_gridx1,gridy2)&(Read_Coordinate_Clean(temp_gridx1-1,gridy2)))
						{
							//return temp_gridx1;
							if(IS_Can_ReachPoint_X(gridx1,temp_gridx1,gridy1))
								return temp_gridx1;
						}
				}
		}
	TRACE("now&now-1 point to min checkout!!!\r\n");
#endif

	for(temp_gridx1=gridx1;temp_gridx1<=max_cleanx1;temp_gridx1++)
		{
			if((Read_Coordinate_Clean(temp_gridx1,gridy1))&(Read_Coordinate_CleanNoWall(temp_gridx1,gridy2)))
				{
					//if(IS_Can_ReachPointX_NoWall(gridx1,temp_gridx1,gridy1))
					if(IS_Can_ReachPoint_X(gridx1,temp_gridx1,gridy1))
						return temp_gridx1;
				}
		}
	TRACE("now point to max checkout!!!\r\n");

	for(temp_gridx1=gridx1;temp_gridx1>=min_cleanx1;temp_gridx1--)
		{
			if((Read_Coordinate_Clean(temp_gridx1,gridy1))&(Read_Coordinate_CleanNoWall(temp_gridx1,gridy2)))
				{
					//if(IS_Can_ReachPointX_NoWall(gridx1,temp_gridx1,gridy1))
					if(IS_Can_ReachPoint_X(gridx1,temp_gridx1,gridy1))
						return temp_gridx1;
				}
		}
	TRACE("now point to min checkout!!!\r\n");
	TRACE("gridx1=%d gridy1=%d gridy2=%d\r\n",gridx1,gridy1,gridy2);
	TRACE("gridy1 can't to gridy2,No comman point!!!\r\n");
	TRACE("Out %s\r\n",__func__);
	return (0x7f);	
}

/*---------------------------------------------------------
function:��ͬһY��������ϣ��ж��ܷ�ӵ�ǰ�㵽��Ŀ��X�����
input:	now_gridx:��ǰ��X���꣬tgt_gridx:Ŀ���X���꣬now_gridy:��ǰ��Y����
output:	0:�޷����1:���Ե���
----------------------------------------------------------*/
u8 IS_Can_ReachPoint_X(s8 now_gridx,s8 tgt_gridx,s8 now_gridy)
{
	s8 temp_gridx,temp_gridy;
	s8 temp_xmin,temp_xmax;
	temp_gridx=now_gridx;temp_gridy=now_gridy;

//	TRACE("enter in %s\r\n",__func__);
	if(temp_gridx==tgt_gridx)
		return 1;

	while(temp_gridx!=tgt_gridx)											//Ŀ����뵱ǰ��X���겻һ��
		{
			if(Read_Coordinate_Clean(temp_gridx,temp_gridy))				//��ǰ���Ƿ���ɨ��
				{
					if(temp_gridx>tgt_gridx)
						temp_gridx--;
					else
						temp_gridx++;
				}
			else															//��ǰ��û�б���ɨ��
				{
					temp_xmax=Return_MaxClean_GridX(temp_gridy,0);			//��ǰY���������X��ɨ��
					temp_xmin=Return_MinClean_GridX(temp_gridy,0);			//��ǰY����С��СX��ɨ��
#if 0
					TRACE("girdy=%d Xmax Clean=%d Xmin Clean=%d\r\n",temp_gridy,temp_xmax,temp_xmin);
					for(temp_gridx=temp_xmin;temp_gridx<=temp_xmax;temp_gridx++)
						{
							TRACE("grid[%d][%d]=%d\r\n",temp_gridy,temp_gridx,Read_Coordinate_Clean(temp_gridx,temp_gridy));
						}
#endif
					break;
				}
		}

	if(temp_gridx==tgt_gridx)
		{
	
			//TRACE("return 1 in %s\r\n",__func__);
			return 1;
		}
	else
		{
			//TRACE("return 0 in %s\r\n",__func__);
			return 0;
		}
}

/*--------------------------------------------------------------
function:Ѱ���������ڵ�Y����㣬�Ƿ��й���ͨ�����Խ���
input:	gridx1:��ʼ��ѯ���X���꣬gridy1:��ʼ��ѯ���Y���꣬gridy2:��ʼ��ѯ�������Y����
output:����У����ع�����X�����;���û�з���0x7f
----------------------------------------------------------------*/
s8 Find_Adjoin_Gap(s8 gridx1,s8 gridy1,s8 gridy2)
{
	s8 max_cleanx1,min_cleanx1,temp_gridx1;

	max_cleanx1=Return_MaxClean_GridX(gridy1,0);
	min_cleanx1=Return_MinClean_GridX(gridy1,0);
	TRACE("enter in %s...\r\n",__func__);
	TRACE("gridx1=%d gridy1=%d gridy2=%d\r\n",gridx1,gridy1,gridy2);
	TRACE("gridy1 Xmax=%d Xmin=%d\r\n",max_cleanx1,min_cleanx1);

	for(temp_gridx1=gridx1;temp_gridx1<max_cleanx1;temp_gridx1++)
		{
			if(Read_Coordinate_Clean(temp_gridx1,gridy1)&(Read_Coordinate_Clean(temp_gridx1+1,gridy1)))
				{
					if(Read_Coordinate_Clean(temp_gridx1,gridy2)&(Read_Coordinate_Clean(temp_gridx1+1,gridy2)))
						{
							//return temp_gridx1;
							TRACE("comm point x=%d\r\n",temp_gridx1);
							if(IS_Can_ReachPoint_X(gridx1, temp_gridx1, gridy1))
								return temp_gridx1;
						}
				}
		}
	TRACE("now&now+1 point to max checkout!!!\r\n");

	for(temp_gridx1=gridx1;temp_gridx1>min_cleanx1;temp_gridx1--)
		{
			if(Read_Coordinate_Clean(temp_gridx1,gridy1)&(Read_Coordinate_Clean(temp_gridx1-1,gridy1)))
				{
					if(Read_Coordinate_Clean(temp_gridx1,gridy2)&(Read_Coordinate_Clean(temp_gridx1-1,gridy2)))
						{
							//return temp_gridx1;
							if(IS_Can_ReachPoint_X(gridx1,temp_gridx1,gridy1))
								return temp_gridx1;
						}
				}
		}
	TRACE("now&now-1 point to min checkout!!!\r\n");

	for(temp_gridx1=gridx1;temp_gridx1<=max_cleanx1;temp_gridx1++)
		{
			if((Read_Coordinate_Clean(temp_gridx1,gridy1))&(Read_Coordinate_Clean(temp_gridx1,gridy2)))
				{
					if(IS_Can_ReachPoint_X(gridx1,temp_gridx1,gridy1))
						return temp_gridx1;
				}
		}
	TRACE("now point to max checkout!!!\r\n");

	for(temp_gridx1=gridx1;temp_gridx1>=min_cleanx1;temp_gridx1--)
		{
			if((Read_Coordinate_Clean(temp_gridx1,gridy1))&(Read_Coordinate_Clean(temp_gridx1,gridy2)))
				{
					if(IS_Can_ReachPoint_X(gridx1,temp_gridx1,gridy1))
						return temp_gridx1;
				}
		}
	TRACE("now point to min checkout!!!\r\n");

	TRACE("gridy1 can't to gridy2,No comman point!!!\r\n");
	TRACE("Out %s\r\n",__func__);
	return (0x7f);	
}

//function:Ѱ�ҵ�ǰ�����ͬһX�����ϵĲ�ͬY����㣬�ܹ���ӽ�Ŀ����Y�����
//return:������ӽ�Ŀ����Y����
s8 Find_MaxApproch_Y(s8 tgt_gridy)
{
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	TRACE("Enter in %s...\r\n",__func__);
	
	if(now_gridy>tgt_gridy)
		now_gridy--;
	else if(now_gridy<tgt_gridy)
		now_gridy++;
		
	while(now_gridy!=tgt_gridy)
		{
			//if(Read_Coordinate_Clean(now_gridx,now_gridy))
			if(Read_Coordinate_CleanNoWall(now_gridx,now_gridy))
				{
					if(now_gridy>tgt_gridy)
						now_gridy--;
					else
						now_gridy++;
				}
			else
				{
					break;
				}
		}
	TRACE("cal result_girdy=%d tgt_girdy=%d\r\n",now_gridy,tgt_gridy);
	TRACE("Out %s\r\n",__func__);
	return now_gridy;
}

//function:Ѱ�ҵ�ǰ�����ͬһY�����ϵĲ�ͬX����㣬�ܹ���ӽ�Ŀ����X�����
//return:������ӽ�Ŀ����X����
s8 Find_MaxApproch_X(s8 tgt_gridx)
{
	s8 now_gridx,now_gridy;
	now_gridx=grid.x;now_gridy=grid.y;
	TRACE("Enter in %s...\r\n",__func__);
	
	if(now_gridx>tgt_gridx)
		now_gridx--;
	else if(now_gridx<tgt_gridx)
		now_gridx++;
	
	while(now_gridx!=tgt_gridx)
		{
			//if(Read_Coordinate_Clean(now_gridx,now_gridy))
			if(Read_Coordinate_CleanNoWall(now_gridx,now_gridy))
				{
					if(now_gridx>tgt_gridx)
						now_gridx--;
					else
						now_gridx++;
				}
			else
				{
					break;
				}
		}
	TRACE("cal result_girdx=%d tgt_girdx=%d\r\n",now_gridx,tgt_gridx);
	TRACE("Out %s\r\n",__func__);
	return now_gridx;
	
}

u8 Analysis_Directly(POINT_GRID *now_grid,POINT_GRID *tgt_grid)
{
	s8 temp_gridx,temp_gridy,tgt_gridx,tgt_gridy,now_gridx,now_gridy;
	now_gridx=now_grid->gridx;now_gridy=now_grid->gridy;
	tgt_gridx=tgt_grid->gridx;tgt_gridy=tgt_grid->gridy;
	TRACE("enter in %s...\r\n",__func__);
	TRACE("now_point.x=%d y=%d\r\n",now_gridx,now_gridy);
	TRACE("tgt_point.x=%d y=%d\r\n",tgt_gridx,tgt_gridy);

	if((now_gridy!=tgt_gridy)&(now_gridy<tgt_gridy))
		{
			temp_gridy=now_gridy+1;
		}
	else if((now_gridy!=tgt_gridy)&(now_gridy>tgt_gridy))
		{
			temp_gridy=now_gridy-1;
		}
	temp_gridx=now_gridx;
	//�ȴ�X����һ�¿�ʼ
	//X����һ�£��ӵ�ǰ��Y���굽Ŀ���Y��������е�
	while(temp_gridy!=tgt_gridy)			//�ӵ�ǰ�㵽Ŀ��������X������ͬ,Y���겻ͬ�㣬�ܵ������ӽ�Ŀ��Y����ĵ�
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))
				{
					if(temp_gridy>tgt_gridy)
						temp_gridy--;
					else
						temp_gridy++;
				}
			else
				{
					break;
				}
		}
	if(temp_gridy!=tgt_gridy)
		goto BEGIN_Y;
	//����ֱ�ӵ���Ŀ���Y���꣬�ټ����Ŀ��Y����һ�£��ӵ�ǰ��X���굽Ŀ���X��������е�
	while(temp_gridx!=tgt_gridx)			
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))
				{
					if(temp_gridx>tgt_gridx)
						temp_gridx--;
					else
						temp_gridx++;
				}
			else
				{
					break;
				}
		}
	if(temp_gridx==tgt_gridx)
		{
			now_grid->data=1;
			return 1;
		}

BEGIN_Y:
	//���ȴ�Y����һ�¿�ʼ
	//Y����һ�£��ӵ�ǰ��X���굽Ŀ���X��������е�
	if((now_gridx!=tgt_gridx)&(now_gridx<tgt_gridx))
		{
			temp_gridx=now_gridx+1;
		}
	else if((now_gridx!=tgt_gridx)&(now_gridx>tgt_gridx))
		{
			temp_gridx=now_gridx-1;
		}
	temp_gridy=now_gridy;
	while(temp_gridx!=tgt_gridx)			//�ӵ�ǰ�㵽Ŀ��������X������ͬ,Y���겻ͬ�㣬�ܵ������ӽ�Ŀ��Y����ĵ�
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))
				{
					if(temp_gridx>tgt_gridx)
						temp_gridx--;
					else
						temp_gridx++;
				}
			else
				{
					break;
				}
		}
	if(temp_gridx!=tgt_gridx)
		return 0;
	//����ֱ�ӵ���Ŀ���Y���꣬�ټ����Ŀ��Y����һ�£��ӵ�ǰ��X���굽Ŀ���X��������е�
	while(temp_gridy!=tgt_gridy)
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))
				{
					if(temp_gridy>tgt_gridy)
						temp_gridy--;
					else
						temp_gridy++;
				}
			else
				{
					break;
				}
		}
	if(temp_gridy==tgt_gridy)
		{
			now_grid->data=2;
			return 2;
		}
	else
		return 0;
}

//function:��X Y��������鿴�Ƿ��ܹ�ֱ�ӵ���
//return:
//0:�޷�ֱ�ӵ���
//1:��������Y��������X���򵽴�
//2:��������X��������Y���򵽴�
u8 Find_Directly_Way(s8 tgt_gridx,s8 tgt_gridy)
{
	u8 result=0;
	POINT_GRID now_grid,tgt_grid;
	TRACE("enter in %s...\r\n",__func__);
	now_grid.gridx=grid.x;now_grid.gridy=grid.y;
	tgt_grid.gridx=tgt_gridx;tgt_grid.gridy=tgt_gridy;

	result=Analysis_Directly(&now_grid, &tgt_grid);
	return result;
}

/*-----------------------------------------------------------------
function:�ӵ�ǰ�㵽Ŀ��㣬����ֻ�ܼ�ӵ���ʱ��Ѱ��·��
input: tgt_gridx:Ŀ���X����; tgt_gridy:Ŀ���Y����
output:	0:�޷����1�������������򵽴2�������ط����򵽴�
-------------------------------------------------------------------*/
u8 Find_Indirectly_Way(s8 tgt_gridx,s8 tgt_gridy)
{
	static bool dir_opposite=false;
	s8 now_gridx,now_gridy,temp_gridx,temp_gridy;
	now_gridx=grid.x;now_gridy=grid.y;

	TRACE("Enter %s ...\r\n",__func__);

	temp_gridx=now_gridx;temp_gridy=now_gridy;
	if(temp_gridy<tgt_gridy)						//����ǰ��Y����С��Ŀ���Y����ʱ						
		{
			dir_opposite=false;
			while(1)								//��ѭ��
				{
//					TRACE("temp_gridy=%d in %s\r\n",temp_gridy,__func__);
					while(temp_gridy!=tgt_gridy)	//�ӵ�ǰY���꿪ʼѰ��
						{
							TRACE("temp_gridx=%d in %s\r\n",temp_gridx,__func__);
							TRACE("temp_gridy=%d in %s\r\n",temp_gridy,__func__);
#if 0
							result1=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy+1);	//Ѱ������֮���Ƿ��й���ͨ��
							if(result1==0x7f)											
								return 0;													//û�з���0��׼��Ѱ���ر�
							if(IS_Can_ReachPoint_X(temp_gridx, result1, temp_gridy))		//�й���ͨ����X���꣬�жϵ�ǰ���Ƿ��ܴﵽ��X����
								temp_gridy++;												//���Ե�����ж���һ��Y����
							else															//�����Ե���������˴�ѭ��
								break;	
#else
							temp_gridx=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy+1);
							if(temp_gridx==0x7f)
								break;
							else
								temp_gridy++;
#endif
						}
					if(temp_gridy==tgt_gridy)												//�������ܹ�����Ŀ���Y����			
						{
							if(!dir_opposite)												//�ж��Ƿ���Ҫ�ӷ�����Ѱ��
								return 1;													//����Ҫ���򷵻�1
							else
								return 2;													//��Ҫ���򷵻�3
						}
																							//�ӵ�ǰ��temp_gridy�㲻�ܼ�ӵ���Ŀ���Y����׼��������һ������Ѱ��
					if(temp_gridy-1<grid.y_area_min)										
						return 0;
#if 0
					result1=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy-1);
					if(result1==0x7f)
						return 0;
					if(IS_Can_ReachPoint_X(temp_gridx, result1, temp_gridy))
						{
							dir_opposite=true;
							temp_gridy--;
							temp_gridx=result1;
						}
#else
					temp_gridx=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy-1);
					if(temp_gridx==0x7f)
						return 0;
					else
						{
							dir_opposite=true;
							temp_gridy--;
						}
#endif
					
				}
		}
	else											//����ǰ��Y�������Ŀ���Y����ʱ
		{
			dir_opposite=false;
			while(1)
				{
//					TRACE("temp_gridy=%d in %s\r\n",temp_gridy,__func__);
					while(temp_gridy!=tgt_gridy)
						{
							TRACE("temp_gridy=%d in %s\r\n",temp_gridy,__func__);
#if 0
							result1=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy-1);
							if(result1==0x7f)
								return 0;
							if(IS_Can_ReachPoint_X(temp_gridx, result1, temp_gridy))
								temp_gridy--;
							else
								break;
#else
							temp_gridx=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy-1);
							if(temp_gridx==0x7f)
								break;
							else
								temp_gridy--;
#endif
						}
					if(temp_gridy==tgt_gridy)
						{
							if(!dir_opposite)
								return 1;
							else
								return 2;
						}
			
					if(temp_gridy+1>grid.y_area_max)
						return 0;
#if 0
					result1=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy+1);
					if(result1==0x7f)
						return 0;
					if(IS_Can_ReachPoint_X(temp_gridx, result1, temp_gridy))
						{
							dir_opposite=true;
							temp_gridy++;
							temp_gridx=result1;
						}
#else
					temp_gridx=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy+1);
					if(temp_gridx==0x7f)
						return 0;
					else
						{
							dir_opposite=true;
							temp_gridy++;
						}
#endif
				}
		}
}

/*----------------------------------------------
function:������ǽ�ڵ�·��
output:  0--->û��·�����߽ڵ��ڴ��޷�����
         1--->���ҵ�·��
-------------------------------------------------*/
u8 Analysis_PathPoint_NoWall(POINT_GRID *now_grid,POINT_GRID *tgt_grid)
{
	POINT_GRID pointgrid[100];
	u8 point_num=0,i;
	s8 now_gridx,now_gridy,temp_gridx,temp_gridy,tgt_gridx,tgt_gridy;
	now_gridx=now_grid->gridx;now_gridy=now_grid->gridy;
	tgt_gridx=tgt_grid->gridx;tgt_gridy=tgt_grid->gridy;

	TRACE("Enter in %s...\r\n",__func__);
	TRACE("now_gridx=%d gridy=%d\r\n",now_gridx,now_gridy);
	TRACE("tgt_gridx=%d gridy=%d\r\n",tgt_gridx,tgt_gridy);
	Delete_All_PathPoint();
//	pointgrid=(POINT_GRID*)malloc(POINTGRID_LEN*100);
	if(now_gridy<tgt_gridy)
		{
			temp_gridx=now_gridx;
			for(temp_gridy=now_gridy;temp_gridy<tgt_gridy;temp_gridy++)
				{
					temp_gridx=Find_Adjoin_Gap_NoWall(temp_gridx,temp_gridy,temp_gridy+1);
					if(temp_gridx==0x7f)
						break;
					else
						{
							pointgrid[point_num].gridx=temp_gridx;
							pointgrid[point_num].gridy=temp_gridy+1;
							point_num++;
							if(point_num>99)		//Խ��
								{
									TRACE("Analysis PathPoint has more 100,return 0!!!\r\n");
									return 0;
								}
								
						}
				}
			if(temp_gridy==tgt_gridy)
				{
					if(IS_Can_ReachPoint_X(temp_gridx,tgt_gridx,temp_gridy))
						{
#if 0
							pointgrid[point_num].gridx=tgt_gridx;
							pointgrid[point_num].gridy=tgt_gridy;
							point_num++;
#endif
#if 0
							i=Compare_PointGrid(last_grid,pointgrid);
							if(i)
								{	
									TRACE("Last PointGrid is equ Now PointGrid,Return !!!\r\n");
									return 0;
								}							
							stop_rap();
							TRACE("Last PointGrid is not equ Now PointGrid,Return !!!\r\n");
							TRACE("Reset&Load last PointGrid!!!\r\n");
							Init_LastPointGrid();
							Load_LastPoinGrid(last_grid,pointgrid);
#else
							startend_now.start_grid.gridx=now_gridx;startend_now.start_grid.gridy=now_gridy;
							startend_now.tgt_grid.gridx=tgt_gridx;startend_now.tgt_grid.gridy=tgt_gridy;
							i=Path_Repeat_Check(&startend_last,&startend_now);
							if(i)
								{
									TRACE("The same PathPoint Start!!! No use PathPoint!!!\r\n");
									return 0;
								}
							startend_last.start_grid.gridx=startend_now.start_grid.gridx;startend_last.start_grid.gridy=startend_now.start_grid.gridy;
							startend_last.tgt_grid.gridx=startend_now.tgt_grid.gridx;startend_last.start_grid.gridy=startend_now.tgt_grid.gridy;
#endif
							path_length=point_num;
							for(i=0;i<point_num;i++)
								{
									if(Add_PathPoint(pointgrid[i].gridx,pointgrid[i].gridy,i+1))
										{
											TRACE("PATHPOINT malloc fail,return 0!!!\r\n");
											Delete_All_PathPoint();
											return 0;
										}
								}
							//����PATH_POINT�ڵ�
							return 1;
						}
					else
						return 0;
				}
			else
				{
					return 0;
				}
		}
	else if(now_gridy>tgt_gridy)
		{
			temp_gridx=now_gridx;
			for(temp_gridy=now_gridy;temp_gridy>tgt_gridy;temp_gridy--)
				{
					temp_gridx=Find_Adjoin_Gap_NoWall(temp_gridx,temp_gridy,temp_gridy-1);
					if(temp_gridx==0x7f)
						break;
					else
						{
							pointgrid[point_num].gridx=temp_gridx;
							pointgrid[point_num].gridy=temp_gridy-1;
							point_num++;
							if(point_num>99)		//Խ��
								{
									TRACE("Analysis PathPoint has more 100,return 0!!!\r\n");
									return 0;
								}
						}
				}
			if(temp_gridy==tgt_gridy)
				{
					if(IS_Can_ReachPoint_X(temp_gridx,tgt_gridx,tgt_gridy))
						{
#if 0
							pointgrid[point_num].gridx=tgt_gridx;
							pointgrid[point_num].gridy=tgt_gridy;
							point_num++;
#endif
#if 0
							i=Compare_PointGrid(last_grid,pointgrid);
							if(i)
								{	
									TRACE("Last PointGrid is equ Now PointGrid,Return !!!\r\n");
									return 0;
								}							
							stop_rap();
							TRACE("Last PointGrid is not equ Now PointGrid,Return !!!\r\n");
							TRACE("Reset&Load last PointGrid!!!\r\n");
							Init_LastPointGrid();
							Load_LastPoinGrid(last_grid,pointgrid);
#else
							startend_now.start_grid.gridx=now_gridx;startend_now.start_grid.gridy=now_gridy;
							startend_now.tgt_grid.gridx=tgt_gridx;startend_now.tgt_grid.gridy=tgt_gridy;
							i=Path_Repeat_Check(&startend_last,&startend_now);
							if(i)
								{
									TRACE("The same PathPoint Start!!! No use PathPoint!!!\r\n");
									return 0;
								}
							startend_last.start_grid.gridx=startend_now.start_grid.gridx;startend_last.start_grid.gridy=startend_now.start_grid.gridy;
							startend_last.tgt_grid.gridx=startend_now.tgt_grid.gridx;startend_last.start_grid.gridy=startend_now.tgt_grid.gridy;
#endif
							path_length=point_num;
							for(i=0;i<point_num;i++)
								{
									if(Add_PathPoint(pointgrid[i].gridx,pointgrid[i].gridy,i+1))
										{

											TRACE("PATHPOINT malloc fail,return 0!!!\r\n");
											return 0;
										}
								}
							//����PATH_POINT�ڵ�
							return 1;
						}
					else
						return 0;
				}
			else
				{
					return 0;
				}
		}
	return 0;
}

/*--------------------------------------------------
function:ʹ�ýڵ�ķ���Ѱ��·��
output:  0:û���ҵ�����·�������ڴ�ڵ��޷�����
		 1:���ҵ�����·��
----------------------------------------------------*/
u8 Find_PathPoint_Way(s8 tgt_gridx,s8 tgt_gridy)
{
	u8 result=0;
	POINT_GRID now_grid,tgt_grid;
	now_grid.gridx=grid.x;now_grid.gridy=grid.y;
	tgt_grid.gridx=tgt_gridx;tgt_grid.gridy=tgt_gridy;

	TRACE("Enter in %s...\r\n",__func__);
	result=Analysis_PathPoint_NoWall(&now_grid,&tgt_grid);
	TRACE("result=%d\r\n",result);
	TRACE("Out %s!\r\n",__func__);
	return result;
}

u8 Analysis_Directly_YBS(POINT_GRID *now_grid,POINT_GRID *tgt_grid)
{
	s8 temp_gridx,temp_gridy,tgt_gridx,tgt_gridy;
	temp_gridx=now_grid->gridx;temp_gridy=now_grid->gridy;
	tgt_gridx=tgt_grid->gridx;tgt_gridy=tgt_grid->gridy;

	//�ȴ�X����һ�¿�ʼ
	//X����һ�£��ӵ�ǰ��Y���굽Ŀ���Y��������е�
	while(temp_gridy!=tgt_gridy)			//�ӵ�ǰ�㵽Ŀ��������X������ͬ,Y���겻ͬ�㣬�ܵ������ӽ�Ŀ��Y����ĵ�
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))
				{
					if(temp_gridy>tgt_gridy)
						temp_gridy--;
					else
						temp_gridy++;
				}
			else
				{
					break;
				}
		}
	if(temp_gridy!=tgt_gridy)
		goto BEGIN_Y_YBS;
	
	//�˴�temp_gridy==tgt_gridy
	//����ֱ�ӵ���Ŀ���Y���꣬�ټ����Ŀ��Y����һ�£��ӵ�ǰ��X���굽Ŀ���X��������е�
	while(temp_gridx!=tgt_gridx)			
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))
				{
					if(temp_gridx>tgt_gridx)
						temp_gridx--;
					else
						temp_gridx++;
				}
			else
				{
					break;
				}
		}
	if(temp_gridx==tgt_gridx)
		{
			now_grid->data=1;
			return 1;
		}

BEGIN_Y_YBS:
	//���ȴ�Y����һ�¿�ʼ
	//Y����һ�£��ӵ�ǰ��X���굽Ŀ���X��������е�
	temp_gridx=now_grid->gridx;temp_gridy=now_grid->gridy;
	while(temp_gridx!=tgt_gridx)			//�ӵ�ǰ�㵽Ŀ��������X������ͬ,Y���겻ͬ�㣬�ܵ������ӽ�Ŀ��Y����ĵ�
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))
				{
					if(temp_gridx>tgt_gridx)
						temp_gridx--;
					else
						temp_gridx++;
				}
			else
				{
					break;
				}
		}
	if(temp_gridx!=tgt_gridx)
		return 0;
	//����ֱ�ӵ���Ŀ���Y���꣬�ټ����Ŀ��Y����һ�£��ӵ�ǰ��X���굽Ŀ���X��������е�
	while(temp_gridy!=tgt_gridy)
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))
				{
					if(temp_gridy>tgt_gridy)
						temp_gridy--;
					else
						temp_gridy++;
				}
			else
				{
					break;
				}
		}
	if(temp_gridy==tgt_gridy)
		{
			now_grid->data=2;
			return 2;
		}
	else
		return 0;
}


u8 Find_DirectlyWay_YBS(s8 tgt_gridx,s8 tgt_gridy)
{
	s8 temp_gridx,temp_gridy;
	u8 result=0;
	POINT_GRID now_grid,tgt_grid;
	tgt_grid.gridx=tgt_gridx;tgt_grid.gridy=tgt_gridy;
	temp_gridx=grid.x;temp_gridy=grid.y;
	
	now_grid.gridx=temp_gridx;now_grid.gridy=temp_gridy;
	result=Analysis_Directly_YBS(&now_grid,&tgt_grid);
	if(result)
		{
			turn_grid=now_grid;
			return 1;
		}

	now_grid.gridx=temp_gridx+1;now_grid.gridy=temp_gridy;
	result=Analysis_Directly_YBS(&now_grid,&tgt_grid);
	if(result)
		{
			turn_grid=now_grid;
			return 1;
		}
	
	now_grid.gridx=temp_gridx-1;now_grid.gridy=temp_gridy;
	result=Analysis_Directly_YBS(&now_grid,&tgt_grid);
	if(result)
		{
			turn_grid=now_grid;
			return 1;
		}
	
	now_grid.gridx=temp_gridx;now_grid.gridy=temp_gridy+1;
	result=Analysis_Directly_YBS(&now_grid,&tgt_grid);
	if(result)
		{
			turn_grid=now_grid;
			return 1;
		}

	now_grid.gridx=temp_gridx;now_grid.gridy=temp_gridy-1;
	result=Analysis_Directly_YBS(&now_grid,&tgt_grid);
	if(result)
		{
			turn_grid=now_grid;
			return 1;
		}

	return 0;
}

u8 Find_PathPoint_YBS(s8 tgt_gridx,s8 tgt_gridy)
{
	POINT_GRID tgt_grid,temp_grid;
	s8 now_gridx,now_gridy;
	u8 result=0;
	tgt_grid.gridx=tgt_gridx;tgt_grid.gridy=tgt_gridy;
	now_gridx=grid.x;now_gridy=grid.y;

	temp_grid.gridx=now_gridx;temp_grid.gridy=now_gridy;
	result=Analysis_PathPoint_NoWall(&temp_grid,&tgt_grid);
	if(result)
		{
			turn_grid=temp_grid;
			return 1;
		}
	
	temp_grid.gridx=now_gridx+1;temp_grid.gridy=now_gridy;
	result=Analysis_PathPoint_NoWall(&temp_grid,&tgt_grid);
	if(result)
		{
			turn_grid=temp_grid;
			return 1;
		}
	
	temp_grid.gridx=now_gridx-1;temp_grid.gridy=now_gridy;
	result=Analysis_PathPoint_NoWall(&temp_grid,&tgt_grid);
	if(result)
		{
			turn_grid=temp_grid;
			return 1;
		}
	
	temp_grid.gridx=now_gridx;temp_grid.gridy=now_gridy+1;
	result=Analysis_PathPoint_NoWall(&temp_grid,&tgt_grid);
	if(result)
		{
			turn_grid=temp_grid;
			return 1;
		}

	temp_grid.gridx=now_gridx;temp_grid.gridy=now_gridy-1;
	result=Analysis_PathPoint_NoWall(&temp_grid,&tgt_grid);
	if(result)
		{
			turn_grid=temp_grid;
			return 1;
		}

	return 0;
}


