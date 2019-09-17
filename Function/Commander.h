//===========================================================================================================
#include "stm32f10x_conf.h"
//===========================================================================================================






#define COMMAND_RUNNING_FINISH_0x00			0x00
#define COMMAND_RUNNING_0x11						0x11


#define COMMAND_REPORT_MASTER_0x22									0x22
#define COMMAND_REPORT_MASTER_FINISH_0x00						0x00




extern uint8_t Command_Running_Flag;



void Commander_Yes_Sir(void);

void Init_Commander(void);



