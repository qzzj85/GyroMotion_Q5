#include "AAA-include.h"

#define PATHPOINT_LEN (sizeof(struct POINT_GRID))
#define POINTGRID_LEN (sizeof(struct POINT_GRID))
#define	LEN_PATHPOINT (sizeof(struct PATH_POINT))

PATH_STARTEND startend_last,startend_now;		//路径分析的上一起终点信息，这次的起终点信息
struct PATH_POINT *path_head;					
struct PATH_POINT *path_end;
POINT_GRID turn_grid;							//YBS分析到可行路径后的中转点
u32 path_length=0;								//路径分析的行走点个数
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
function：比较这一次的PATH_POINT和上一次是否一致
return:
		0:不一致
		1:一致
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
	while(p->next_point!=NULL)			//先去到最后一个节点
		{
			p=p->next_point;
		}
	while(p->last_point!=NULL)			//再判断最后一个节点是否path_head
		{
			q=p->last_point;			//找到上一个节点
			free(p);					//释放最后一个节点
			p=q;						//前往上一个节点，准备再次释放
		}
	if(p!=path_head)
		{
			TRACE("Delete All PathPoint fail!\r\n");
			return 1;
		}
	p->next_point=NULL;					//现在p是head_point，将其next设置为空
	TRACE("Delete All PathPoint success!\r\n");
	return 0;
}

/*------------------------------------------
function:增加PATHPOINT节点
output: 0--->增加成功
        1--->增加失败
--------------------------------------------*/
u8 Add_PathPoint(s8 gridx,s8 gridy,u8 data)
{
	struct PATH_POINT *p,*q;
	p=path_head;
	while(p->next_point!=NULL)			//先去到最后一个节点
		{
			p=p->next_point;
		}
	q=(struct PATH_POINT*)malloc(LEN_PATHPOINT);
	if(q==NULL)
		{
			TRACE("PATH POINT malloc fail in %s\r\n",__func__);
			return 1;
		}
	p->next_point=q;					//最后一个节点的next指针指向新分配单元
	q->last_point=p;					//新分配单元的last指针指向最后一个节点
	q->next_point=NULL;					//新分配单元的next指针指向NULL，完成后，新分配单元成为最后一个节点
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

	while(temp_gridx!=tgt_gridx)											//目标点与当前点X坐标不一致
		{
			if(Read_Coordinate_CleanNoWall(temp_gridx,temp_gridy))				//当前点是否被清扫过
				{
					if(temp_gridx>tgt_gridx)
						temp_gridx--;
					else
						temp_gridx++;
				}
			else															//当前点没有被清扫过
				{
					temp_xmax=Return_MaxClean_GridX(temp_gridy,0);			//当前Y坐标下最大X清扫点
					temp_xmin=Return_MinClean_GridX(temp_gridy,0);			//当前Y坐标小最小X清扫点
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
function:在同一Y坐标基础上，判断能否从当前点到达目标X坐标点
input:	now_gridx:当前点X坐标，tgt_gridx:目标点X坐标，now_gridy:当前的Y坐标
output:	0:无法到达，1:可以到达
----------------------------------------------------------*/
u8 IS_Can_ReachPoint_X(s8 now_gridx,s8 tgt_gridx,s8 now_gridy)
{
	s8 temp_gridx,temp_gridy;
	s8 temp_xmin,temp_xmax;
	temp_gridx=now_gridx;temp_gridy=now_gridy;

//	TRACE("enter in %s\r\n",__func__);
	if(temp_gridx==tgt_gridx)
		return 1;

	while(temp_gridx!=tgt_gridx)											//目标点与当前点X坐标不一致
		{
			if(Read_Coordinate_Clean(temp_gridx,temp_gridy))				//当前点是否被清扫过
				{
					if(temp_gridx>tgt_gridx)
						temp_gridx--;
					else
						temp_gridx++;
				}
			else															//当前点没有被清扫过
				{
					temp_xmax=Return_MaxClean_GridX(temp_gridy,0);			//当前Y坐标下最大X清扫点
					temp_xmin=Return_MinClean_GridX(temp_gridy,0);			//当前Y坐标小最小X清扫点
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
function:寻找两个相邻的Y坐标点，是否有公共通道可以进入
input:	gridx1:开始查询点的X坐标，gridy1:开始查询点的Y坐标，gridy2:开始查询点的相邻Y坐标
output:如果有，返回公共的X坐标点;如果没有返回0x7f
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

//function:寻找当前点的在同一X坐标上的不同Y坐标点，能够最接近目标点的Y坐标点
//return:返回最接近目标点的Y坐标
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

//function:寻找当前点的在同一Y坐标上的不同X坐标点，能够最接近目标点的X坐标点
//return:返回最接近目标点的X坐标
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
	//先从X坐标一致开始
	//X坐标一致，从当前点Y坐标到目标点Y坐标的所有点
	while(temp_gridy!=tgt_gridy)			//从当前点到目标点的所有X坐标相同,Y坐标不同点，能到达的最接近目标Y坐标的点
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
	//可以直接到达目标点Y坐标，再检查以目标Y坐标一致，从当前点X坐标到目标点X坐标的所有点
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
	//再先从Y坐标一致开始
	//Y坐标一致，从当前点X坐标到目标点X坐标的所有点
	if((now_gridx!=tgt_gridx)&(now_gridx<tgt_gridx))
		{
			temp_gridx=now_gridx+1;
		}
	else if((now_gridx!=tgt_gridx)&(now_gridx>tgt_gridx))
		{
			temp_gridx=now_gridx-1;
		}
	temp_gridy=now_gridy;
	while(temp_gridx!=tgt_gridx)			//从当前点到目标点的所有X坐标相同,Y坐标不同点，能到达的最接近目标Y坐标的点
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
	//可以直接到达目标点Y坐标，再检查以目标Y坐标一致，从当前点X坐标到目标点X坐标的所有点
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

//function:从X Y两个方向查看是否能够直接到达
//return:
//0:无法直接到达
//1:可以先走Y方向，再走X方向到达
//2:可以先走X方向，再走Y方向到达
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
function:从当前点到目标点，两点只能间接到达时，寻找路线
input: tgt_gridx:目标点X坐标; tgt_gridy:目标点Y坐标
output:	0:无法到达，1：可以正常方向到达，2：可以沿反方向到达
-------------------------------------------------------------------*/
u8 Find_Indirectly_Way(s8 tgt_gridx,s8 tgt_gridy)
{
	static bool dir_opposite=false;
	s8 now_gridx,now_gridy,temp_gridx,temp_gridy;
	now_gridx=grid.x;now_gridy=grid.y;

	TRACE("Enter %s ...\r\n",__func__);

	temp_gridx=now_gridx;temp_gridy=now_gridy;
	if(temp_gridy<tgt_gridy)						//当当前点Y坐标小于目标点Y坐标时						
		{
			dir_opposite=false;
			while(1)								//死循环
				{
//					TRACE("temp_gridy=%d in %s\r\n",temp_gridy,__func__);
					while(temp_gridy!=tgt_gridy)	//从当前Y坐标开始寻找
						{
							TRACE("temp_gridx=%d in %s\r\n",temp_gridx,__func__);
							TRACE("temp_gridy=%d in %s\r\n",temp_gridy,__func__);
#if 0
							result1=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy+1);	//寻找两点之间是否有公共通道
							if(result1==0x7f)											
								return 0;													//没有返回0，准备寻求沿边
							if(IS_Can_ReachPoint_X(temp_gridx, result1, temp_gridy))		//有公共通道的X坐标，判断当前点是否能达到该X坐标
								temp_gridy++;												//可以到达，则判断下一个Y坐标
							else															//不可以到达，则跳出此次循环
								break;	
#else
							temp_gridx=Find_Adjoin_Gap(temp_gridx,temp_gridy,temp_gridy+1);
							if(temp_gridx==0x7f)
								break;
							else
								temp_gridy++;
#endif
						}
					if(temp_gridy==tgt_gridy)												//如果最后能够到达目标点Y坐标			
						{
							if(!dir_opposite)												//判断是否需要从反方向寻找
								return 1;													//不需要，则返回1
							else
								return 2;													//需要，则返回3
						}
																							//从当前的temp_gridy点不能间接到达目标点Y，则准备从另外一个方向寻找
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
	else											//当当前点Y坐标大于目标点Y坐标时
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
function:分析无墙节点路径
output:  0--->没有路径或者节点内存无法分配
         1--->已找到路径
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
							if(point_num>99)		//越界
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
							//加入PATH_POINT节点
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
							if(point_num>99)		//越界
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
							//加入PATH_POINT节点
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
function:使用节点的方法寻找路径
output:  0:没有找到合适路径或者内存节点无法分配
		 1:已找到合适路径
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

	//先从X坐标一致开始
	//X坐标一致，从当前点Y坐标到目标点Y坐标的所有点
	while(temp_gridy!=tgt_gridy)			//从当前点到目标点的所有X坐标相同,Y坐标不同点，能到达的最接近目标Y坐标的点
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
	
	//此处temp_gridy==tgt_gridy
	//可以直接到达目标点Y坐标，再检查以目标Y坐标一致，从当前点X坐标到目标点X坐标的所有点
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
	//再先从Y坐标一致开始
	//Y坐标一致，从当前点X坐标到目标点X坐标的所有点
	temp_gridx=now_grid->gridx;temp_gridy=now_grid->gridy;
	while(temp_gridx!=tgt_gridx)			//从当前点到目标点的所有X坐标相同,Y坐标不同点，能到达的最接近目标Y坐标的点
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
	//可以直接到达目标点Y坐标，再检查以目标Y坐标一致，从当前点X坐标到目标点X坐标的所有点
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


