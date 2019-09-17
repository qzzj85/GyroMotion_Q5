////////////////////////头文件////////////////////////////////////
#include "flash.h"
///////////////////////私有变量////////////////////////////////////
const u32  code_addr = 0x08036ff0;	 //flash的地址
////////////////////////全局变量//////////////////////////////////
////////////////////////私有定义//////////////////////////////////
///////////////////////全局函数////////////////////////////////////
///////////////////////私有函数////////////////////////////////////
///////////////////////函数实体////////////////////////////////////
/**************************************************************
功能：初始化flash的内容
**************************************************************/
u8 init_flash(void)
{
  u32 addr;
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP|FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);
  if(*(vu16*)code_addr != 0xaa55)
  {
   addr = code_addr&0xfffffc00;
   FLASH_ErasePage(addr);
   FLASH_ProgramHalfWord(code_addr,0xaa55);
   FLASH_ProgramHalfWord(code_addr+2,0x00);			  
   FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP|FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);
   if( *(u32 *)code_addr != 0xaa55)
   return 1;
  }
  return 0;
}

/**************************************************************
功能：在flash中写入一个16位的数 
**************************************************************/
u8 write_flash(u32 data)
{
 u32 addr;
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP|FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);
   addr = code_addr&0xfffffc00;
   FLASH_ErasePage(addr);
   FLASH_ProgramHalfWord(code_addr,0xaa55);
   FLASH_ProgramWord(code_addr+4,data);							
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP|FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);
  if( *(u32 *)code_addr != (0xaa55+(data<<16)))
   return 1;
   return 0;
}
/**************************************************************
功能：在flash中读出一个16位的数 
**************************************************************/
u32 read_flash(void)
{
  return *(vu32*)(code_addr+4);
}
