#include "AAA-include.h"
#include "stm32f10x_i2c.h"
#include "delay.h"

void Init_DS1307(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	I2C_InitTypeDef		I2C_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin=DSRTC_CLK|DSRTC_DAT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(DSRTC_GPIO,&GPIO_InitStructure);

	I2C_DeInit(I2C2);
	I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1=DSRTC_SLAVE_ADDR;
	I2C_InitStructure.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed=100000;
	I2C_Init(I2C2, &I2C_InitStructure);
	
	I2C_Cmd(I2C2, ENABLE);
}

void DS1307_GPIO_Init(void)
{
	RCC->APB2ENR|=1<<3;
	GPIOB->CRH&=0XFFFF00FF;
	GPIOB->CRH|=0X00003300;

	DSCLK_1;//GPIOB->BSRR|=1<<10;
	DSDAT_1;//GPIOB->BSRR|=1<<11;
	Set_DS1307_On();

	DS1307toRtcTime();
}

void DS1307_GenerateStart(void)
{
	delay_us(5);
	DS1307_DAOut();
	DSCLK_1;//GPIOB->BSRR|=1<<10;
	DSDAT_1;//GPIOB->BSRR|=1<<11;
	delay_us(1);
	DSDAT_0;//GPIOB->BRR|=1<<11;
	delay_us(5);
	DSCLK_0;//GPIOB->BRR|=1<<10;
}

void DS1307_GenerateStop(void)
{

	DS1307_DAOut();
	DSCLK_0;//GPIOB->BRR|=1<<10;
	DSDAT_0;//GPIOB->BRR|=1<<11;
	delay_us(1);
	DSCLK_1;//GPIOB->BSRR|=1<<10;
	delay_us(5);
	DSDAT_1;//GPIOB->BSRR|=1<<11;
}

u8 DS1307_WaiteAck(void)
{
	u8 err_time=0;
	DS1307_DAIn();
	DSCLK_1;//GPIOB->BSRR|=1<<10;
	while((GPIOB->IDR)&(0X0800))
		{
			err_time++;
			if(err_time>250)
				{
					DS1307_GenerateStop();
					return 1;
				}
		}
	DSCLK_0;//GPIOB->BRR|=1<<10;
	return 0;
}

void DS1307_GenerateAck(void)
{
	DSCLK_0;//GPIOB->BRR|=1<<10;
	DS1307_DAOut();

	DSDAT_0;//GPIOB->BRR|=1<<11;
	delay_us(1);
	DSCLK_1;//GPIOB->BSRR|=1<<10;
	delay_us(1);

	DSCLK_0;//GPIOB->BRR|=1<<10;
}

void DS1307_GenerateNoAck(void)
{
	DSCLK_0;//GPIOB->BRR|=1<<10;
	DS1307_DAOut();

	DSDAT_1;//GPIOB->BSRR|=1<<11;//DSDAT_1;
	delay_us(1);
	DSCLK_1;//GPIOB->BSRR|=1<<10;//DSCLK_1;
	delay_us(1);
	DSCLK_0;//GPIOB->BRR|=1<<10;
}

void DS1307_SendByte_GPIO(u8 data)
{
    u8 t;   
	DS1307_DAOut(); 	    
	DSCLK_0;//GPIOB->BRR|=1<<10;
    delay_us(5);
    for(t=0;t<8;t++)
    {              
		if((data&0x80)>>7)
			DSDAT_1//GPIOB->BSRR|=1<<11;//DSDAT_1;
		else
			DSDAT_0//GPIOB->BRR|=1<<11;
		data<<=1; 	  
		DSCLK_1;//GPIOB->BSRR|=1<<10;//DSCLK_1;
		delay_us(1);
		DSCLK_0;//GPIOB->BRR|=1<<10;
		delay_us(5);
    }	 
}

u8 DS1307_ReadByte_GPIO(u8 ack)
{
	unsigned char i,receive=0;
	DS1307_DAIn();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
		DSCLK_0;//GPIOB->BRR|=1<<10;
		delay_us(5);
		DSCLK_1;//GPIOB->BSRR|=1<<10;//DSCLK_1;
        receive<<=1;
        if((GPIOB->IDR)&(0X0800))
			receive++;		
		delay_us(1);
    }					 
    if (!ack)
        DS1307_GenerateNoAck();//发送nACK
    else
        DS1307_GenerateAck(); //发送ACK   
    return receive;
}


void Set_DS1307_On(void)
{
	u8 data=0;
	if(DS1307_Readu8data(0x00,&(data)))
		{
#ifdef DEBUG_INIT
			TRACE("Read DS1307 fail!\r\n");
#endif
			return;
		}
	if(data&0x80)
	{
#ifdef DEBUG_INIT
		TRACE("DS1307 need Turn on!\r\n");
#endif
		DS1307_Writeu8data(0x00, 0x00);	
	}
#ifdef DEBUG_INIT
	TRACE("DS1307 Init OK!\r\n");
#endif
}

u8 BCDtoHEX(u8 data)
{
	u8 temp;
	temp=(data&0xf0)>>4;
	data&=0x0f;
	temp=temp*10+data;
	return temp;
}

u8 HEXtoBCD(u8 data)
{
	u8 temp;
	temp=(data/10)<<4;
	temp&=0xf0;
	data=data%10;
	data&=0x0f;
	temp|=data;
	return temp;
}


u8 DS1307_Writeu8data(u8 addr,u8 data)
{
#ifndef DS1307_IIC_GPIO
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C2, DSRTC_SLAVE_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C2,data);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C2, ENABLE);
#else
	u8 temp_data=0;
	DS1307_GenerateStart();

	temp_data=DSSLAVE_W_ADDR;		//写模式
	DS1307_SendByte_GPIO(temp_data);
	if(DS1307_WaiteAck())
		return 1;
	DS1307_SendByte_GPIO(addr); 	//写地址
	if(DS1307_WaiteAck())
		return 1;
	DS1307_SendByte_GPIO(data); 	//写数据
	if(DS1307_WaiteAck())
		return 1;
	DS1307_GenerateStop();
	return 0;
#endif
}


u8 DS1307_Readu8data(u8 addr,u8 *pdata)
{
#ifndef DS1307_IIC_GPIO
	u8 data=0;
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C2, DSRTC_SLAVE_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2, DSRTC_SLAVE_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
	data=I2C_ReceiveData(I2C2);

	I2C_AcknowledgeConfig(I2C2, DISABLE);
	I2C_GenerateSTOP(I2C2, ENABLE);

	I2C_AcknowledgeConfig(I2C2, ENABLE);
	return data;
#else
	DS1307_GenerateStart();

	DS1307_SendByte_GPIO(DSSLAVE_W_ADDR);
	if(DS1307_WaiteAck())
		return 1;
	DS1307_SendByte_GPIO(addr); 	//写地址

	DS1307_GenerateStart(); 		//重新开始
	DS1307_SendByte_GPIO(DSSLAVE_R_ADDR);		//读模式
	if(DS1307_WaiteAck())
		return 1;

	*pdata=DS1307_ReadByte_GPIO(0); //读数据
	DS1307_GenerateStop();
	return 0;
#endif
}

u16 DS1307_Readu16data(u8 addr)
{
	u16 data=0;
	u8 data1,data2;
	
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2, DSRTC_SLAVE_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2, DSRTC_SLAVE_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
	data1=I2C_ReceiveData(I2C2);
	I2C_AcknowledgeConfig(I2C2, ENABLE);

	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
	data2=I2C_ReceiveData(I2C2);
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	I2C_GenerateSTOP(I2C2, ENABLE);

	I2C_AcknowledgeConfig(I2C2, ENABLE);
	data=(data1<<8)|data2;
	return data;
}

void DS1307_Writeu16data(u8 addr,u16 data)
{
	u8 data1,data2;
	data1=data>>8;
	data2=data2&0x00ff;
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2, DSRTC_SLAVE_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C2,data1);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C2,data2);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C2, ENABLE);
}

u8 DS1307_Readu32data(u8 addr,u32 *pdata)
{
	u32 temp_data=0;
	u8 data=0,i=0;
	
	DS1307_GenerateStart();

	DS1307_SendByte_GPIO(DSSLAVE_W_ADDR);
	if(DS1307_WaiteAck())
		return 1;
	DS1307_SendByte_GPIO(addr);		//写地址

	DS1307_GenerateStart();			//重新开始
	DS1307_SendByte_GPIO(DSSLAVE_R_ADDR);		//读模式
	if(DS1307_WaiteAck())
		return 1;
	
	i=0;
	while(1)
		{
			if(i==3)
				{
					data=DS1307_ReadByte_GPIO(0);
					temp_data|=data;
					break;
				}
			data=DS1307_ReadByte_GPIO(1);
			temp_data|=data;
			temp_data<<=8;
			i++;
		}
	DS1307_GenerateStop();
	*pdata=temp_data;
	return 0;
}

u8 DS1307_Writeu32data(u8 addr,u32 data)
{
	u8 temp_data=0,i=0;
	int j=24;
	DS1307_GenerateStart();

	DS1307_SendByte_GPIO(DSSLAVE_W_ADDR);
	if(DS1307_WaiteAck())
		return 1;
	DS1307_SendByte_GPIO(addr);		//写地址
	if(DS1307_WaiteAck())
		return 1;

	while(i<4)
		{
			temp_data=data>>j;
			DS1307_SendByte_GPIO(temp_data);
			if(DS1307_WaiteAck())
				return 1;
			i++;
			j-=8;
		}
	#if 0
	temp_data=data>>24;
	DS1307_SendByte_GPIO(temp_data);
	if(DS1307_WaiteAck())
		return 1;
	temp_data=(data)>>16;
	DS1307_SendByte_GPIO(temp_data);
	if(DS1307_WaiteAck())
		return 1;
	temp_data=(data)>>8;
	DS1307_SendByte_GPIO(temp_data);
	if(DS1307_WaiteAck())
		return 1;
	temp_data=data;
	DS1307_SendByte_GPIO(temp_data);
	if(DS1307_WaiteAck())
		return 1;
	#endif
	DS1307_GenerateStop();
	return 0;
}

u8 DS1307toRtcTime(void)
{
	u8 data[7],i=0,temp_data=0;
	u8 sec,min,hour,day;
#ifndef DS1307_IIC_GPIO
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

	//设置读取地址0x00
	I2C_Send7bitAddress(I2C2, DSRTC_SLAVE_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,0x00);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	//设置读模式
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2, DSRTC_SLAVE_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(1)
		{
			if(I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED))
				{
					data[i]=I2C_ReceiveData(I2C2);
					i++;
					if(i==7)
						{

							I2C_AcknowledgeConfig(I2C2, DISABLE);
							I2C_GenerateSTOP(I2C2, ENABLE);
							break;
						}
				}
		}
	I2C_AcknowledgeConfig(I2C2, ENABLE);
#else
	DS1307_GenerateStart();

	temp_data=DSSLAVE_W_ADDR;			//写模式
	DS1307_SendByte_GPIO(temp_data);
	if(DS1307_WaiteAck())
		return 1;
	DS1307_SendByte_GPIO(0x00); 	//写地址

	DS1307_GenerateStart(); 		//重新开始
	temp_data=DSSLAVE_R_ADDR;
	DS1307_SendByte_GPIO(temp_data); 	//读模式
	if(DS1307_WaiteAck())
		return 1;
	i=0;
	while(1)
		{
			if(i==6)
				{
					data[i]=DS1307_ReadByte_GPIO(0);
					break;
				}
			data[i]=DS1307_ReadByte_GPIO(1);
			i++;
		}
	DS1307_GenerateStop();
#endif
	sec=BCDtoHEX(data[0]);
	min=BCDtoHEX(data[1]);
	hour=BCDtoHEX(data[2]);
	day=BCDtoHEX(data[3]);
	if(day==7)
		day=0;
	Rtc_time=day*86400+hour*3600+min*60+sec;
#ifdef DEBUG_INIT
	TRACE("day=%d hour=%d min=%d sec=%d\r\n",day,hour,min,sec);
#endif
	return 0;
}

u8 DS1307_Write_Time(u8 *pdata)
{
	u8 temp_data=0,i=0;
#ifndef DS1307_IIC_GPIO
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C2,DSRTC_SLAVE_ADDR,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C2,0x00);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	while(i<7)
		{
			I2C_SendData(I2C2,*pdata);
			while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
			pdata++;
			i++;
		}
	I2C_GenerateSTOP(I2C2,ENABLE);
	return 0;
#else
	DS1307_GenerateStart();
	DS1307_GenerateStart();

	temp_data=DSSLAVE_W_ADDR;		//写模式
	DS1307_SendByte_GPIO(temp_data);
	if(DS1307_WaiteAck())
		return 1;
	DS1307_SendByte_GPIO(0x00); 	//写地址
	if(DS1307_WaiteAck())
		return 1;

	while(i<7)
		{
			DS1307_SendByte_GPIO(*pdata);
			if(DS1307_WaiteAck())
				return 1;
			pdata++;
			i++;
		}
	return 0;
#endif

}

void RtcTimetoDS1307(void)
{
	u8 data[7];
	u8 hour,min,sec,weekday;
	u32 temp_time=0;

	temp_time=Rtc_time;
	weekday=temp_time/86400;
	if(weekday==0)			//周日
		weekday=7;
	hour=(temp_time%86400)/3600;
	min=((temp_time%86400)%3600)/60;
	sec=((temp_time%86400)%3600)%60;
	
	data[0]=HEXtoBCD(sec);
	data[0]&=0x7f;

	data[1]=HEXtoBCD(min);
	data[2]=HEXtoBCD(hour);
	data[3]=HEXtoBCD(weekday);
	data[4]=HEXtoBCD(Unix_Time.Day);
	data[5]=HEXtoBCD(Unix_Time.Month);
	data[6]=HEXtoBCD(Unix_Time.Year%100);

	DS1307_Write_Time(data);

}

u8 DS1307_Write_Backup(u8 addr,u8 data)
{
	if(DS1307_Writeu8data(addr, data))
		return 1;
	return 0;
}

u8 DS1307_Read_Backup(u8 addr,u8 *data)
{
	if(DS1307_Readu8data(addr, data))
		return 1;
	return 0;
}

u8 DS1307_WriteBat_Backup(u8 addr,u32 data)
{
	if(DS1307_Writeu32data(addr,data))
		return 1;
	return 0;
}

u8 DS1307_ReadBat_Backup(u8 addr,u32 *data)
{
	if(DS1307_Readu32data(addr,data))
		return 1;
	return 0;
}

u8 DS1307_Data_Reinit(void)
{
	DS1307_Write_Backup(FLAG_BACKUP_ADDR, 0x5A);			//重置备份标志
	Rtc_time=0;												//重置时间qz add 20181110
	RtcTimetoDS1307();										//重置时间qz add 20181110
	DS1307_Write_Backup(MODE_BACKUP_ADDR, 0X01);			//重置主模式，默认CEASE
	DS1307_Write_Backup(SUBMODE_BACKUP_ADDR, 0X01);			//重置子模式，默认CEASE
	DS1307_Write_Backup(VALUE_BACKUP_ADDR, 0XEC);			//重置音量大小，默认14级
	DS1307_Write_Backup(BAT_RECAL_ADDR, 0);					//重置电池RECAL标志			//qz add 20181101
	DS1307_WriteBat_Backup(BAT_CAP_ADDR,0x0);				//重置电量，默认0	
	DS1307_WriteBat_Backup(BAT_FDCAP_ADDR,0X0);				//重置放电量，默认0
	return 0;
}

	
