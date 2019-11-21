#ifndef AREA_TREE_H
#define	AREA_TREE_H



//void Del_Tree(struct AREA_TREE *p);
void Del_All_Tree(void);
int Init_AreaTree(void);
int Add_AreaTree(void);
void Back_To_ParentTree(void);
void Get_Curr_AreaTree_Info(void);
u8 Out_Curr_AreaTree_Exit(void);
u8 Read_Curr_AreaTree_NO(void);
u8 Set_Curr_AreaTree_LeakInfo(bool area_ok);
u8 Set_Curr_AreaTree_NewInfo(bool area_ok,u8 num);
u8 Set_Curr_AreaTree_AllNewOK(void);

#endif
