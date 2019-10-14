#ifndef PATH_H
#define PATH_H

#define	AREA_UPLEFT				1
#define	AREA_UPRIGHT			2
#define	AREA_DOWNLEFT			3
#define	AREA_DOWNRIGHT			4

#define	BOUNDARY_RIGHTDOWN		1
#define	BOUNDARY_RIGHTUP		2
#define	BOUNDARY_UPRIGHT		3
#define	BOUNDARY_UPLEFT			4
#define	BOUNDARY_LEFTUP			5
#define	BOUNDARY_LEFTDOWN		6
#define	BOUNDARY_DOWNLEFT		7
#define	BOUNDARY_DOWNRIGHT		8

extern POINT_GRID turn_grid;
extern u32 path_length;

u8 Init_PathPoint(void);
u8 Delete_All_PathPoint(void);
struct PATH_POINT* Get_PathPoint(u8 data);
s8 Find_Adjoin_Gap(s8 gridx1,s8 gridy1,s8 gridy2);
s8 Find_MaxApproch_Y(s8 tgt_gridy);
s8 Find_MaxApproch_X(s8 tgt_gridx);
u8 Find_Directly_Way(s8 tgt_gridx,s8 tgt_gridy);
u8 Find_Indirectly_Way(s8 tgt_gridx,s8 tgt_gridy);
u8 Find_PathPoint_Way(s8 tgt_gridx,s8 tgt_gridy);
u8 Find_DirectlyWay_YBS(s8 tgt_gridx,s8 tgt_gridy);
u8 Find_PathPoint_YBS(s8 tgt_gridx,s8 tgt_gridy);
u8 Find_DirectlyWay_YBS(s8 tgt_gridx,s8 tgt_gridy);
u8 Find_PathPoint_YBS(s8 tgt_gridx,s8 tgt_gridy);

u8 Add_PathPoint(s8 gridx,s8 gridy,u8 data);

#endif
