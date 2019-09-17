////////////////////ͷ�ļ�/////////////////////////////////////
#include   "a7105.h"  
#include   "soft.h"
#include   "hardware.h"
#include   "flash.h"
#include   "systime.h"
#include   "lcdkey.h"
#include   "key.h"
////////////////////////˽�ж���//////////////////////////////////
const u8 ID_Tab[4]={'X','R',210,'A'}; //ID code
 u8  A7105Config[51]=	 //���üĴ���
{
	0x00, //д��ʱоƬ��λ������Ϊ״̬λ
	0x42, //MODE register,
	0x00, //CALIBRATION register,only read, not use on config
	0x08, //FIFO1 register,
	0x00, //FIFO2 register,
	0x00, //FIFO register,		for fifo read/write
	0x00, //IDDATA register,	for idcode
	0x00, //RCOSC1 register,
	0x00, //RCOSC2 register,
	0x00, //RCOSC3 register,
	0x00, //CKO register,
	0x01, //GPIO1 register
	0x21, //GPIO2 register,
	0x05, //CLOCK register,
	0x00, //DATARATE register,
	0x50, //PLL1 register,
	0x9E, //PLL2 register, 		RFbase 2400MHz
	0x4B, //PLL3 register,
	0x00, //PLL4 register,
	0x02, //PLL5 register,
	0x16, //TX1 register,
	0x2B, //TX2 register,
	0x12, //DELAY1 register,
	0x00, //DELAY2 register,
	0x62, //RX register,
	0x80, //RXGAIN1 register,
	0x80, //RXGAIN2 register,
	0x00, //RXGAIN3 register,
	0x0a, //RXGAIN4 register,
	0x32, //RSSI register,
	0xC3, //ADC register,
	0x07, //CODE1 register,
	0x16, //CODE2 register,
	0x00, //CODE3 register,
	0x00, //IFCAL1 register,
	0x00, //IFCAL2 register,    only read
	0x00, //VCOCCAL register,
	0x00, //VCOCAL1 register,
	0x3b, //VCOCAL2 register,
	0x00, //BATTERY register,
	0x17, //TXTEST register,
	0x47, //RXDEM1 register,
	0x80, //RXDEM2 register,
	0x03, //CPC register,
	0x01, //CRYSTAL register,
	0x45, //PLLTEST register,
	0x18, //VCOTEST1 register,
	0x00, //VCOTEST2 register,
	0x01, //IFAT register,
	0x0F, //RSCALE register,
	0x00  //FILTERTEST
};
////////////////////////ȫ�ֱ���//////////////////////////////////
YAOKONG gsv_yaokong;
bool gbv_yaokong_time;
bool gbv_a7105_reset;
///////////////////////˽�б���////////////////////////////////////
u32 Telecontrol_ID;//ң��ID��
///////////////////////ȫ�ֺ���////////////////////////////////////
u8 initRF(void);
///////////////////////˽�к���////////////////////////////////////
void init_a7105hardware(void);
void A7105_Reset(void);
void A7105_WriteReg(u8, u8);
u8 A7105_ReadReg(u8);
void ByteSend(u8 src);
u8 ByteRead(void);
void A7105_WriteID(void);
void A7105_Config(void);
void A7105_Cal(void);
void RxPacket(u8 a[]);
void StrobeCmd(u8);
void SetCH(u8);
void A7105_WriteFIFO(u8 p[], u8 byte);
void A7105_ReadFIFO(u8 a[],u8 number);
void A7105_ReadID(u8 *id);
void A7105_rx_en(void);
void set_A7105_SDIO_in(void);
void set_A7105_SDIO_out(void);
///////////////////////����ʵ��////////////////////////////////////
/****************************************************
���ܣ���ʼ����a7105оƬͨѶ��io��
���룺��
�������
��������A7105_GIO1��ʼ��Ϊ�������룻�� A7105_SCS��A7105_SCK��A7105_SDIO��ʼ��
      Ϊ���������
	  ��A7105_SCS A7105_SDIO�ߣ���A7105_SCK��
****************************************************/
void init_a7105hardware(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;   //GPIO �ṹ����
    //
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC, ENABLE); 
  
  GPIO_InitStructure.GPIO_Pin = A7105_GIO1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIOC, A7105_SDIO);
  GPIO_SetBits(GPIOC, A7105_SCS);
  GPIO_ResetBits(GPIOC, A7105_SCK);
  GPIO_InitStructure.GPIO_Pin = A7105_SCS | A7105_SCK | A7105_SDIO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
}
void init_a7105(void)
{
u8 a;
 A7105_Reset();
  A7105_WriteID();
  A7105_Config();
  //////////////////������У׼A7105//////////
  StrobeCmd(CMD_STBY);                      //0XA0  ���ʹ�������
  A7105_WriteReg(CALIBRATION_REG, 0x01);    //0X02 У׼ʹ��
  //DelayMs(10);
  delay(10000);
  a = A7105_ReadReg(CALIBRATION_REG);
  a &= 0x01;
  if(a!=0)return ;                       //У׼ʧ�ܣ�����0
  a = A7105_ReadReg(IFCAL1_REG);//0X22�Ĵ���
  a=a&0X10;								//У׼ʧ�ܣ�����0
  if (a)	return ;
  A7105_WriteReg(VCOCCAL_REG, 0x13); //0X25 VCO У׼	
  A7105_WriteReg(VCOCAL1_REG, 0x09);//0X26 VCO У׼	
  StrobeCmd(CMD_STBY);           //�������ģʽ 
  A7105_rx_en();
  gbv_a7105_reset = false;
}
/************************************************************
����: ��ʼ��7105ģ��
����: ��
���: ��
************************************************************/
u8 initRF(void)
{
u8 a;
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
	
  init_a7105hardware();	

  init_flash();	   	  ////��ʼ��flash
  Telecontrol_ID = 	read_flash();  //��flash�ж���ң������id��

  init_a7105();
  gsv_yaokong.key = 0;
  gsv_yaokong.sign = 0;
  //////////////////////����//////////////
  for(a=0; a<100;a++)
			{
					if(dui_Telecontrol_ID() == 1)
				{
					//Set_Beep();
					//DelayMs(100);
					delay(1000000);
					//Reset_Beep();
					delay(1000000);
						break;
				}
				//DelayMs(10);
				delay(30000);
			}
 ///////����A7105_GIO1��Ϊ�½����жϡ� 
 
        StrobeCmd(CMD_RFR);        //���RX FIFO
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
//  EXTI->IMR |= 0xe;
  
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  StrobeCmd(CMD_RX);         //�������ģʽ  
  return 1;
}
/************************************************************************
**  Reset_RF	��λоƬ
************************************************************************/
void A7105_Reset(void)
{
	A7105_WriteReg(MODE_REG, 0x00); //��Ĵ���0�ĸ�λд�ȸ�λоƬ
}
/************************************************************************
**  WriteID	   //доƬID�����оƬIDΪ4���ֽ�
************************************************************************/
void A7105_WriteID(void)
{
	u8 i;
	u8 addr;
	u8 id[4];
	addr = IDCODE_REG; //дID���� send address 0x06, bit cmd=0, r/w=0
	GPIO_ResetBits(GPIOC, A7105_SCS);//Ƭѡ
	ByteSend(addr);			  //��������
	for (i=0; i < 4; i++)
		ByteSend(ID_Tab[i]);  //����ID
    GPIO_SetBits(GPIOC, A7105_SCS);

	addr = IDCODE_REG|0x40;
	GPIO_ResetBits(GPIOC, A7105_SCS);
	A7105_ReadID(id);	   
	for (i=0; i < 4; i++)
	{
	    if(id[i] !=  ID_Tab[i])
		break;
	}

}
/************************************************************************
**  A7105_WriteReg
д�Ĵ��� �Ĵ�����ַ������
************************************************************************/
void A7105_WriteReg(u8 addr, u8 dataByte)
{
    u8 i;

	//A7105_SCS = 0;
	GPIO_ResetBits(GPIOC, A7105_SCS);
    addr |= 0x00; //bit cmd=0,r/w=0
    for(i = 0; i < 8; i++)
    {
        if(addr & 0x80)
		  {
		   GPIO_SetBits(GPIOC, A7105_SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, A7105_SDIO);
		  }
        delay(10);
	   GPIO_SetBits(GPIOC, A7105_SCK);
	   delay(10);
	   GPIO_ResetBits(GPIOC,A7105_SCK);
        addr = addr << 1;
    }
   delay(10);
    for(i = 0; i < 8; i++)
    {
        if(dataByte & 0x80)
		   {
		   GPIO_SetBits(GPIOC, A7105_SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, A7105_SDIO);
		  }
        delay(10);
	   GPIO_SetBits(GPIOC, A7105_SCK);
	   delay(10);
	   GPIO_ResetBits(GPIOC,A7105_SCK);
        dataByte = dataByte<< 1;
    }
	GPIO_SetBits(GPIOC, A7105_SCS);
}
/************************************************************************
**  A7105_ReadReg //���Ĵ���
************************************************************************/
u8 A7105_ReadReg(u8 addr)
{
    u8 i;
    u8 tmp=0;
	GPIO_ResetBits(GPIOC,A7105_SCS);
    addr |= 0x40; //bit cmd=0,r/w=1
    for(i = 0; i < 8; i++)
    {

        if(addr & 0x80)
          {
		   GPIO_SetBits(GPIOC, A7105_SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, A7105_SDIO);
		  }
		delay(10);
		 GPIO_SetBits(GPIOC, A7105_SCK);
		delay(10);
        GPIO_ResetBits(GPIOC, A7105_SCK);
        addr = addr << 1;
    }
    delay(10);
	set_A7105_SDIO_in();
    for(i = 0; i < 8; i++)
    {
        if(GPIO_ReadInputDataBit(GPIOC,A7105_SDIO))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;
	   GPIO_SetBits(GPIOC, A7105_SCK);
		delay(10);
	   GPIO_ResetBits(GPIOC, A7105_SCK);
	   delay(10);
    }
	set_A7105_SDIO_out();
    return tmp;
}
/************************************************************************
**  Byte Send	//д���� 8λ
************************************************************************/
void ByteSend(u8 src)
{
    u8 i;

    for(i = 0; i < 8; i++)
    {
        if(src & 0x80)
          {
		   GPIO_SetBits(GPIOC, A7105_SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, A7105_SDIO);
		  }
		delay(10);
		GPIO_SetBits(GPIOC, A7105_SCK);
		delay(10);
		GPIO_ResetBits(GPIOC, A7105_SCK);
        src = src << 1;
    }
}
/************************************************************************
**  ByteRead	������ 8λ
************************************************************************/
u8 ByteRead(void)
{
	u8 i,tmp;
    tmp = 0;
	set_A7105_SDIO_in() ;
    for(i = 0; i < 8; i++)
    {
        if(GPIO_ReadInputDataBit(GPIOC,A7105_SDIO))				 //������
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;
		GPIO_SetBits(GPIOC, A7105_SCK);
		delay(10);
		GPIO_ResetBits(GPIOC, A7105_SCK);
    }
	set_A7105_SDIO_out() ;
    return tmp;
}
/************************************************************************
**  Send4Bit	 ���ֽ�4λ����  �ֽڵĸ���λд����
************************************************************************/
void Send4Bit(u8 src)
{
    u8 i;

    for(i = 0; i < 4; i++)
    {
        if(src & 0x80)
          {
		   GPIO_SetBits(GPIOC, A7105_SDIO);
		  }	
        else
		  {
			GPIO_ResetBits(GPIOC, A7105_SDIO);
		  }
		delay(10);
		GPIO_SetBits(GPIOC, A7105_SCK);
		delay(10);
		GPIO_ResetBits(GPIOC, A7105_SCK);
        src = src << 1;
    }
}
/*********************************************************************
** SetCH
*********************************************************************/
void SetCH(u8 ch)
{
	A7105_WriteReg(PLL1_REG, ch);
}
/*********************************************************************
** A7105_WriteFIFO ����������FIFO�� �ֽڵ�ַ������
*********************************************************************/
void A7105_WriteFIFO(u8 p[], u8 byte)
{
	u8 i;
	u8 cmd;

	cmd = FIFO_REG; //send address 0x05, bit cmd=0, r/w=0 ��ַ
	GPIO_ResetBits(GPIOC, A7105_SCS);					  //Ƭѡ
	ByteSend(cmd);			      //д����
for(i=0; i <byte; i++)
		ByteSend(p[i]);	  //д64�����ݵ�FIFO��
	GPIO_SetBits(GPIOC, A7105_SCS);
	
}
/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCmd(u8 cmd)
{
	GPIO_ResetBits(GPIOC, A7105_SCS);					  //Ƭѡ
	Send4Bit(cmd);	 //����4λ
	GPIO_SetBits(GPIOC, A7105_SCS);
}
/*********************************************************************
** Rx Packet   ��FIFO�ж������ϲ��Ա�
*********************************************************************/
void A7105_ReadFIFO(u8 a[],u8 number)
{
    u8 i;
    u8 cmd;
	cmd = FIFO_REG | 0x40; //������ address 0x05, bit cmd=0, r/w=1
	GPIO_ResetBits(GPIOC, A7105_SCS);					  //Ƭѡ
	ByteSend(cmd);			//д����
	for(i=0; i <number; i++)
	{
	  a[i]  = ByteRead();	//����		
    }	
	GPIO_SetBits(GPIOC, A7105_SCS);
}
/********************************************************************
���ܣ���ȡоƬ��ID
�������оƬID���������������
********************************************************************/
void A7105_ReadID(u8 *id)
{
   u8 i;
   u8 cmd;
   cmd = IDCODE_REG | 0x40;
   GPIO_ResetBits(GPIOC, A7105_SCS);					  //Ƭѡ
   ByteSend(cmd);			//д����
   
   for(i=0; i<4; i++)
    {
	  id[i] = ByteRead();
	}	
  	GPIO_SetBits(GPIOC, A7105_SCS);
}
/*********************************************************************
** A7105_Config	  //����оƬ
*********************************************************************/
void A7105_Config(void)
{
	u8 i;

    //0x00 mode register, for reset
    //0x05 fifo data register
    //0x06 id code register
    //0x23 IF calibration II, only read
    //0x32 filter test register

    for (i=0x01; i<=0x04; i++)
	    A7105_WriteReg(i, A7105Config[i]);// д�Ĵ���

	for (i=0x07; i<=0x22; i++)
		A7105_WriteReg(i, A7105Config[i]);

	for (i=0x24; i<=0x31; i++)
		A7105_WriteReg(i, A7105Config[i]);
}

/***************************************************************
���ܣ�����ʹ��
****************************************************************/
void A7105_rx_en(void)
{
 SetCH(99);                 //Ƶ��2449.501MHz
 StrobeCmd(CMD_RFR);        //���RX FIFO
 StrobeCmd(CMD_RX);         //�������ģʽ 	
}
/****************************************************************
���ܣ���ң�����Խ�ң������id���ɹ�����1��ʧ�ܷ���0
****************************************************************/
u8 dui_Telecontrol_ID(void)
{
u8 A7105_rx_data[8],a;
u32 x;
if(GPIO_ReadInputDataBit(GPIOD,A7105_GIO1) == 0)
{
    RxPacket(A7105_rx_data);
	//����У��
	if(A7105_rx_data[0]==0x55)//�Ƕ�ң����ID��
	{	
	   a=(A7105_rx_data[0]+A7105_rx_data[1]+A7105_rx_data[2]+A7105_rx_data[3]+A7105_rx_data[4]);
	   if(A7105_rx_data[7]==a)//������ȷ 
	   {
	       x = (A7105_rx_data[1]<<24)+(A7105_rx_data[2]<<16)+(A7105_rx_data[3]<<8)+A7105_rx_data[4];
		   if(x != Telecontrol_ID)
		   {
		      Telecontrol_ID = x;
			  write_flash( Telecontrol_ID);
		   }
		   StrobeCmd(CMD_RFR);        //���RX FIFO
           StrobeCmd(CMD_RX);         //�������ģʽ 
		   return 1;
	   }
	}
}
StrobeCmd(CMD_RFR);        //���RX FIFO
StrobeCmd(CMD_RX);         //�������ģʽ 
return 0;
}
/*********************************************************************
** Rx Packet   ��FIFO�ж�������
*********************************************************************/
void RxPacket(u8 a[])
{
    u8 i;
    u8 cmd;
    
	cmd = FIFO_REG | 0x40; //������ address 0x05, bit cmd=0, r/w=1
	
	GPIO_ResetBits(GPIOC,A7105_SCS);
	ByteSend(cmd);			//д����
	for(i=0; i <8; i++)
	{
	  a[i]  = ByteRead();	//����		
    }
	GPIO_SetBits(GPIOC,A7105_SCS);
}
/**********************************************
ң�������ճ���
**********************************************/
void A7105_rx(void)
{
  if(gbv_yaokong_time == false)
	{
	    return;
	}
	gbv_yaokong_time = false;
	if(((giv_sys_time - gsv_yaokong.effect_time) > 5000) && (gsv_yaokong.effect == 1))
	{
	    gsv_yaokong.effect = 0;
		gsv_yaokong.sign = 0;
	}
	if(((giv_sys_time - gsv_yaokong.effect_time) > 50000) && (gsv_yaokong.effect == 0))
	{									
	    gbv_a7105_reset = true;	 
		gsv_yaokong.effect_time = giv_sys_time;
		EXTI_GenerateSWInterrupt(EXTI_Line0);
	} 
}
/*************************************************
���ܣ�����A7105_SDIOΪ����
*************************************************/
void set_A7105_SDIO_in(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;   //GPIO �ṹ����
   GPIO_InitStructure.GPIO_Pin =  A7105_SDIO;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*************************************************
���ܣ�����A7105_SDIOΪ���
*************************************************/
void set_A7105_SDIO_out(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;   //GPIO �ṹ����
   GPIO_InitStructure.GPIO_Pin =  A7105_SDIO;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
}
