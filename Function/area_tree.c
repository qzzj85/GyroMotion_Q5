#include "AAA-include.h"

#define	TREE_LEN sizeof(struct AREA_TREE)

struct AREA_TREE
{
	bool clean;							//区域
	bool area_ok;						//区域清扫检查完成标志
	bool ymax_ok;						//YMAX新区域检查完成标志
	bool ymin_ok;						//YMIN新区域检查完成标志
	bool xmax_ok;						//XMAX新区域检查完成标志
	bool xmin_ok;						//XMIN新区域检查完成标志
	u8 area_num;						//区域打扫编号
	u8 exit_area_num;					//退出区域编号
	s8 exit_gridx1;						//区域入口点1 X坐标			当进入或退出某区域时使用
	s8 exit_gridy1;						//区域入口点1 Y坐标
	s8 exit_gridx2;						//区域入口点2 X坐标
	s8 exit_gridy2;						//区域入口点2 Y坐标
	short exit_tgtyaw;
	s8 gridx_area_start;
	s8 gridy_area_start;
	u8 y_acc;
	u8 x_acc;

	struct AREA_TREE *parent;
	struct AREA_TREE *l_brother;
	struct AREA_TREE *r_brother;
    struct AREA_TREE *final_child;
	struct AREA_TREE *first_child;
};


struct AREA_TREE *root_tree;
struct AREA_TREE *curr_tree;

void Del_Tree(struct AREA_TREE *p)
{
	struct AREA_TREE *q,*last_brother,*father;	
//	printf("Enter in %s...\r\n",__func__);
	if(p->first_child==NULL)
		{
			if(p==root_tree)
				return;
			father=p->parent;
			if(p->l_brother==NULL)
			{
				free(p);
				father->final_child=NULL;
				father->first_child=NULL;
				return;
			}
			else
			{
				last_brother=p->l_brother;
				free(p);
				last_brother->r_brother=NULL;
				father->final_child=last_brother;
				return;
			}
			
		}

	while(p->r_brother!=NULL)
	{
		p=p->r_brother;
	}

	while(p->first_child!=NULL)
	{
		//q=p;
		//while(q->next_child!=NULL)
		//	q=q->next_child;
		q=p->final_child;
		Del_Tree(q);
	}
	Del_Tree(p);		
}

void Del_All_Tree(void)
{
#ifdef DEBUG_TREE
	TRACE("Enter in %s...\r\n",__func__);
#endif
	if(root_tree->first_child==NULL)
		return;
	struct AREA_TREE *p;
	while(root_tree->first_child!=NULL)
		{
			p=root_tree->final_child;
			Del_Tree(p);	
		}
	curr_tree=root_tree;
#ifdef DEBUG_TREE
	TRACE("Del All Tree success!!\r\n");
	TRACE("Out %s\r\n",__func__);	
#endif
}

int Init_AreaTree(void)
{
#ifdef DEBUG_TREE
	TRACE("Enter in %s...\r\n",__func__);
#endif
	if(root_tree==NULL)
	{
#ifdef DEBUG_TREE
		TRACE("root_tree is not exist!\r\n");
#endif
		root_tree=(struct AREA_TREE*)malloc(TREE_LEN);
		if(root_tree==NULL)
			{
				return 1;
			}
		root_tree->area_num=0;
		root_tree->exit_area_num=0;
		root_tree->exit_gridx1=0;
		root_tree->exit_gridy1=0;
		root_tree->exit_gridx2=0;
		root_tree->exit_gridy2=0;
		root_tree->exit_tgtyaw=0;
		root_tree->gridx_area_start=0;
		root_tree->gridy_area_start=0;
		root_tree->parent=NULL;
		root_tree->final_child=NULL;
		root_tree->first_child=NULL;
		root_tree->l_brother=NULL;
		root_tree->r_brother=NULL;
		curr_tree=root_tree;
	}
	else
	{
		curr_tree=root_tree;
#ifdef DEBUG_TREE
		TRACE("root_tree is exist!!\r\n");
#endif
		if(root_tree->first_child==NULL)
		{
#ifdef DEBUG_TREE
			TRACE("but root tree no leaf!\r\n");
#endif
			return 0;
		}
	    Del_All_Tree();
	}
#ifdef DEBUG_TREE
	TRACE("Out %s\r\n",__func__);
#endif
	return 0;
};

int Add_AreaTree(void)
{
	struct AREA_TREE *p=NULL,*now_currtree=NULL,*now_finalchild=NULL,*now_firstchild=NULL;
	now_currtree=curr_tree;
	now_finalchild=curr_tree->final_child;
	now_firstchild=curr_tree->first_child;
	p=(struct AREA_TREE*)malloc(TREE_LEN);
	if(p==NULL)
	{
#ifdef DEBUG_TREE
		TRACE("%s malloc fail!!\r\n",__func__);
#endif
		return 1;
	}
	
	p->first_child=NULL;
	p->final_child=NULL;
	p->parent=curr_tree;
	if(now_firstchild==NULL)
	{
		//now_currtree->first_child=p;
		//now_currtree->final_child=p;
		now_firstchild=p;
		now_finalchild=p;
		p->l_brother=NULL;
		p->r_brother=NULL;
	}
	else
	{
		//now_currtree->final_child->r_brother=p;
		//p->l_brother=now_currtree->final_child;
		//now_currtree->final_child=p;
		
		now_finalchild->r_brother=p;
		now_currtree->final_child=p;
		p->l_brother=now_finalchild;
		p->r_brother=NULL;
		
	}
	p->clean=true;
	p->area_ok=motion1.area_ok;
	p->ymax_ok=motion1.ymax_ok;
	p->ymin_ok=motion1.ymin_ok;
	p->xmax_ok=motion1.xmax_ok;
	p->xmin_ok=motion1.xmin_ok;
	p->area_num=motion1.area_num;
	p->exit_area_num=motion1.exit_area_num;
	p->gridx_area_start=grid.x_area_start;
	p->gridy_area_start=grid.y_area_start;
	p->y_acc=motion1.y_acc;
	p->x_acc=motion1.x_acc;

	if(p->area_num==1)
		{
			p->exit_gridx1=motion1.xpos_start;
			p->exit_gridy1=motion1.ypos_start;
			p->exit_gridx2=motion1.xpos_start;
			p->exit_gridy2=motion1.ypos_start;
			p->exit_tgtyaw=F_Angle_Const;
		}
	else
		{
			p->exit_gridx1=check_point.new_x2;
			p->exit_gridy1=check_point.new_y2;
			p->exit_gridx2=check_point.new_x1;
			p->exit_gridy2=check_point.new_y1;
			p->exit_tgtyaw=check_point.next_tgtyaw;
		}
#ifdef DEBUG_TREE
	TRACE("curr_tree.area_num=%d\r\n",p->area_num);
	TRACE("curr_tree.exit_gridx1=%d\r\n",p->exit_gridx1);
	TRACE("curr_tree.exit_gridy1=%d\r\n",p->exit_gridy1);
	TRACE("curr_tree.exit_gridx2=%d\r\n",p->exit_gridx2);
	TRACE("curr_tree.exit_gridy2=%d\r\n",p->exit_gridy2);
	TRACE("curr_tree.exit_tgtyaw=%d\r\n",p->exit_tgtyaw);
	TRACE("curr_tree.exit_area_num=%d\r\n",p->exit_area_num);
#endif
	curr_tree=p;
	return 0;
}

void Get_Curr_AreaTree_Info(void)
{
	grid.x_area_start=curr_tree->gridx_area_start;
	grid.y_area_start=curr_tree->gridy_area_start;
	motion1.x_acc=curr_tree->x_acc;
	motion1.y_acc=curr_tree->y_acc;
	motion1.area_num=curr_tree->area_num;
	motion1.area_ok=curr_tree->area_ok;
	
	motion1.ymax_ok=curr_tree->ymax_ok;
	motion1.ymin_ok=curr_tree->ymin_ok;
	motion1.xmax_ok=curr_tree->xmax_ok;
	motion1.xmin_ok=curr_tree->xmin_ok;
	motion1.exit_area_num=curr_tree->exit_area_num;
#ifdef DEBUG_TREE
	TRACE("Get CurrTree Info!!!\r\n");
	TRACE("CurrTree.area_num=%d\r\n",curr_tree->area_num);
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

u8 Out_Curr_AreaTree_Exit(void)
{
#ifdef DEBUG_TREE
	TRACE("curr_tree.exit_gridx1=%d\r\n",curr_tree->exit_gridx1);
	TRACE("curr_tree.exit_gridy1=%d\r\n",curr_tree->exit_gridy1);
	TRACE("curr_tree.exit_gridx2=%d\r\n",curr_tree->exit_gridx2);
	TRACE("curr_tree.exit_gridy2=%d\r\n",curr_tree->exit_gridy2);
#endif
	check_point.new_x1=curr_tree->exit_gridx1;
	check_point.new_y1=curr_tree->exit_gridy1;
	check_point.new_x2=curr_tree->exit_gridx2;
	check_point.new_y2=curr_tree->exit_gridy2;
	check_point.next_tgtyaw=curr_tree->exit_tgtyaw;	
//	check_point.go_exit=true;
	motion1.exit_gridx1=curr_tree->exit_gridx1;
	motion1.exit_gridy1=curr_tree->exit_gridy1;
	motion1.exit_gridx2=curr_tree->exit_gridx2;
	motion1.exit_gridy2=curr_tree->exit_gridy2;

	Set_CheckPoint_NextAction(CHECK_GOEXIT);
	return 1;
}

u8 Read_Curr_AreaTree_NO(void)
{
	return (curr_tree->area_num);
}

u8 Set_Curr_AreaTree_LeakInfo(bool area_ok)
{
	curr_tree->area_ok=area_ok;
	return 0;
}

/*------------------------------------------------
function:将当前的新区域检查标志，存入curr_node
input:
      area_ok-- 0:需要检查，1:检查完毕
      num--  1:YMAX区域
             2:XMAX区域
             3:YMIN区域
             4:XMIN区域
--------------------------------------------------*/
u8 Set_Curr_AreaTree_NewInfo(bool area_ok,u8 num)
{
	switch(num)
		{
			case DIR_YMAX:				//YMAX区域
				curr_tree->ymax_ok=area_ok;
				break;
			case DIR_XMAX:				//XMAX区域
				curr_tree->xmax_ok=area_ok;
				break;
			case DIR_YMIN:
				curr_tree->ymin_ok=area_ok;
				break;
			case DIR_XMIN:
				curr_tree->xmin_ok=area_ok;
				break;
			default:
				break;
		}
	return 0;
}

u8 Set_Curr_AreaTree_AllNewOK(void)
{
	motion1.ymax_ok=true;
	motion1.ymin_ok=true;
	motion1.xmax_ok=true;
	motion1.xmin_ok=true;
	curr_tree->ymax_ok=motion1.ymax_ok;
	curr_tree->ymin_ok=motion1.ymin_ok;
	curr_tree->xmax_ok=motion1.xmax_ok;
	curr_tree->xmin_ok=motion1.xmin_ok;
	return 0;
}


void Back_To_ParentTree(void)
{
	struct AREA_TREE *father;
	if(curr_tree->parent==NULL)
		return;
	father=curr_tree->parent;
	curr_tree=father;
}





