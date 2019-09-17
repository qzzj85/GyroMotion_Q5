////////////////////////ͷ�ļ�////////////////////////////////////
#include "flash.h"
///////////////////////˽�б���////////////////////////////////////
const u32  code_addr = 0x08036ff0;	 //flash�ĵ�ַ
////////////////////////ȫ�ֱ���//////////////////////////////////
////////////////////////˽�ж���//////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////
///////////////////////˽�к���////////////////////////////////////
///////////////////////����ʵ��////////////////////////////////////
/**************************************************************
���ܣ���ʼ��flash������
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
���ܣ���flash��д��һ��16λ���� 
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
���ܣ���flash�ж���һ��16λ���� 
**************************************************************/
u32 read_flash(void)
{
  return *(vu32*)(code_addr+4);
}
