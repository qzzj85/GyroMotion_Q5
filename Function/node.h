#ifndef NODE_H
#define NODE_H

extern struct AREA_NODE *head_node;
extern struct AREA_NODE *curr_node;

u8 Creat_AreaNodeList(void);
struct AREA_NODE* Return_CurrNode(void);
u8 Add_AreaNode_End(void);
u8 Del_AreaNode_End(void);
u8 Read_CurrNode_AreaNO(void);
void Load_CurrNode_Info(void);
void Get_CurrNode_Info(void);
u8 Out_CurrNode_Exit(void);
u8 Set_CurrNode_LeakInfo(bool area_ok);
u8 Set_CurrNode_NewAreaInfo(bool area_ok,u8 num);
u8 Del_All_AreaNode(void);
u8 Set_Curr_AllNewAreaOK(void);

#endif
