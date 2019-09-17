#ifndef DS1307_H
#define DS1307_H

#define DS1307_IIC_GPIO			1
#define DSRTC_GPIO				GPIOB
#define DSRTC_CLK				GPIO_Pin_10		//PB
#define DSRTC_DAT				GPIO_Pin_11
#define DSCLK_1					{GPIOB->BSRR|=1<<10;}//GPIO_SetBits(DSRTC_GPIO,DSRTC_CLK)
#define DSCLK_0					{GPIOB->BRR|=1<<10;}//GPIO_ResetBits(DSRTC_GPIO,DSRTC_CLK)
#define DSDAT_1					{GPIOB->BSRR|=1<<11;}//GPIO_SetBits(DSRTC_GPIO,DSRTC_DAT)
#define DSDAT_0					{GPIOB->BRR|=1<<11;}//GPIO_ResetBits(DSRTC_GPIO,DSRTC_DAT)

#define DSRTC_SLAVE_ADDR		0XD0
#define DSSLAVE_W_ADDR			0XD0
#define DSSLAVE_R_ADDR			0XD1
#define WIRTE_MODE				0X00
#define	READ_MODE				0X01
#define READ_DSDAT				GPIO_ReadInputDataBit(DSRTC_GPIO,DSRTC_DAT)
#define FLAG_BACKUP_ADDR		0x08
#define MODE_BACKUP_ADDR		0X09
#define SUBMODE_BACKUP_ADDR		0X0A
#define VALUE_BACKUP_ADDR		0X0B
#define BAT_RECAL_ADDR			0X0C
#define BAT_CAP_ADDR			0X10
#define BAT_FDCAP_ADDR			0X14

#define DS1307_DAOut()			{GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00003000;}
#define DS1307_DAIn()			{GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=0X00008000;}

void Init_DS1307(void);
void DS1307_GPIO_Init(void);
void DS1307_Writeu16data(u8 addr,u16 data);
u8 DS1307_Writeu8data(u8 addr,u8 data);
u8 DS1307_Readu8data(u8 addr,u8 *pdata);
u16 DS1307_Readu16data(u8 addr);
u8 DS1307_Writeu32data(u8 addr,u32 data);
u8 DS1307_Readu32data(u8 addr,u32 *pdata);
void Set_DS1307_On(void);
u8 BCDtoHEX(u8 data);
u8 HEXtoBCD(u8 data);
u8 DS1307toRtcTime(void);
void RtcTimetoDS1307(void);
u8 DS1307_Read_Backup(u8 addr,u8 *data);
u8 DS1307_Write_Backup(u8 addr,u8 data);
u8 DS1307_WriteBat_Backup(u8 addr,u32 data);
u8 DS1307_ReadBat_Backup(u8 addr,u32 *data);
u8 DS1307_Data_Reinit(void);
#endif
