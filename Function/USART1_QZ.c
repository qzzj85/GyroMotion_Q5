#include "usart1_qz.h"
#include <stdio.h>	
#include "AAA-Include.h"

void Usart_Configuration(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure; //GPIO??????
	 USART_InitTypeDef USART_InitStructure;//USART??????
	 USART_ClockInitTypeDef USART_ClockInitStructure;
	 //????1,GPIOA,AFIO??
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1,ENABLE);
	 /* Configure USART1 Tx (PA9) as alternate function push-pull */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//PA9????50MHz
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //????
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 /* Configure USART1 Rx (PA10) as input floating */
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//????
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	 USART_InitStructure.USART_BaudRate =115200; //???115200
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8???
	 USART_InitStructure.USART_StopBits = USART_StopBits_1; //1????
	 USART_InitStructure.USART_Parity = USART_Parity_No; //????
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //???????
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //???????
	
	 USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	 USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;//????????
	 USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;//???????? ?????
	 USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;//?????????????SCLK??
	
	 USART_ClockInit(USART1, &USART_ClockInitStructure);
	 USART_Init(USART1, &USART_InitStructure);	//??????
	 USART_Cmd(USART1, ENABLE); //????1	
	 //USART1->DR=USART1->DR&0XBF;

    #if DEBUG
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//????
    #else
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//????
		#endif
}

//??????,??printf??,??????use MicroLIB	  
/***************************START*********************/
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
	{ 
		int handle; 

	}; 

//FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	USART1->DR = (u8) ch;      
	while((USART1->SR&0X40)==0);//????,??????   
	return ch;
}
#endif
/***************************END*********************/

/**********************************************************
** ???:USART1_Putc
** ????: ??1?????
** ????: c
** ????: ?
***********************************************************/
void USART1_Putc(unsigned char c)
{
    USART1->DR = (u8)c; //???????????????  
	while((USART1->SR&0X40)==0); //??????  
}
/**********************************************************
** ???:USART1_Puts
** ????: ??1??????
** ????: ??str
** ????: ?
***********************************************************/
void USART1_Puts(char * str)
{
    while(*str)
    {
	    USART_GetFlagStatus(USART1, USART_FLAG_TC);
		USART1->DR= *str++;
		while((USART1->SR&0X40)==0);//??????  
    }
}
/**********************************************************
** ???:UART_Send_Enter
** ????: ??1??????
** ????: ?
** ????: ?
***********************************************************/
void UART_Send_Enter(void)
{
	USART1_Putc(0x0d);
	USART1_Putc(0x0a);
}
/**********************************************************
** ???:UART_Send_Str
** ????: ??1??????,???????
** ????: ??s
** ????: ?
***********************************************************/
void UART_Send_Str(char *s)
{
 
 	for(;*s;s++)
 	{
	 	if(*s=='\n') 
	  		UART_Send_Enter();
	 	else
	  		USART1_Putc(*s);
 	}
}
