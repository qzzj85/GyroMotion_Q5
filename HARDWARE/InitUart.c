
//=====================ͷ�ļ�====================================
#include "AAA-Include.h"
//=====================˽�ж���====================================

//******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���******ȫ�ֶ���************
//=====================ȫ�ֱ���====================================
//struct	ULTRASONIC_DATA 		Ultra;

//=====================ȫ�ֺ���====================================



//******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���******˽�ж���************
//=====================˽�б���====================================uint8����
//#define ULTRA_DATA_LENGTH			20
//uint16_t SPI_Send_data[ULTRA_DATA_LENGTH];


//=====================˽�к���====================================
void ccDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void ccDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);

u8 ccSPI3_ReadWriteByte(u16 TxData);
//===============================================================================================================
//===============================================================================================================







//=========================================================================================================================
//======����ʵ��======����ʵ��======����ʵ��======����ʵ��======����ʵ��======����ʵ��======����ʵ��======����ʵ��======
//=========================================================================================================================












//============================================================================================
void URAT3_init(u32 bound){
    //GPIO�˿�����
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		UART1.Trans_Busy = false;	//��ʼ������1����æ��־λ
	
//		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART3��GPIOAʱ��
		USART_DeInit(USART3);  //��λ����1
	 //USART1_TX   PB10
    GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PB11
    GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PA10


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate 		= bound;//һ������Ϊ115200;
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity 			= USART_Parity_No ;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART3, &USART_InitStructure); //��ʼ������
	
	
  //Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���


	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//���������ж� UsART3
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
	USART_ClearFlag(USART3, USART_FLAG_TC);		    //�����һ���ֽ��޷���������
}
//============================================================================================















//=====================================================================================================
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
//void ccSPI3_SetSpeed(uint8_t SPI_BaudRatePrescaler)
//{
//  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
//	SPI3->CR1&=0XFFC7;
//	SPI3->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
//	SPI_Cmd(SPI3,ENABLE); 

//} 
//=====================================================================================================















