#ifndef  __FLASH_H
#define  __FLASH_H

#include "stm32f10x.h"

u8 init_flash(void);
u32 read_flash(void);
u8 write_flash(u32 data);
#endif
