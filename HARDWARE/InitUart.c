
//=====================头文件====================================
#include "AAA-Include.h"
//=====================私有定义====================================

//******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义******全局定义************
//=====================全局变量====================================
//struct	ULTRASONIC_DATA 		Ultra;

//=====================全局函数====================================



//******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义******私有定义************
//=====================私有变量====================================uint8——
//#define ULTRA_DATA_LENGTH			20
//uint16_t SPI_Send_data[ULTRA_DATA_LENGTH];


//=====================私有函数====================================
void ccDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void ccDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);

u8 ccSPI3_ReadWriteByte(u16 TxData);
//===============================================================================================================
//===============================================================================================================







//=========================================================================================================================
//======函数实体======函数实体======函数实体======函数实体======函数实体======函数实体======函数实体======函数实体======
//=========================================================================================================================












//============================================================================================
void URAT3_init(u32 bound){
    //GPIO端口设置
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		UART1.Trans_Busy = false;	//初始化串口1发送忙标志位
	
//		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART3，GPIOA时钟
		USART_DeInit(USART3);  //复位串口1
	 //USART1_TX   PB10
    GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PB11
    GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PA10


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate 		= bound;//一般设置为115200;
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity 			= USART_Parity_No ;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口
	
	
  //Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器


	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启空闲中断 UsART3
	USART_Cmd(USART3, ENABLE);                    //使能串口 
	USART_ClearFlag(USART3, USART_FLAG_TC);		    //解决第一个字节无法发送问题
}
//============================================================================================















//=====================================================================================================
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
//void ccSPI3_SetSpeed(uint8_t SPI_BaudRatePrescaler)
//{
//  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
//	SPI3->CR1&=0XFFC7;
//	SPI3->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
//	SPI_Cmd(SPI3,ENABLE); 

//} 
//=====================================================================================================















