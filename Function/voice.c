#include "AAA-include.h"
#include "stdlib.h"

#define	VOICELIST_START 		1
#define VOICE_LEN sizeof(struct node)


u32 voice_time=0;
u8 voice_level;
bool voice_ok=true;

struct node
{
	u8 data;
	struct node* next;
};

struct node *voice_head;


u8 Init_Voice_Head(void)
{	
	if(voice_head!=NULL)
		{
			TRACE("voice head is exist!!\r\n");
			if(voice_head->next!=NULL)
				{
					Del_All_VoiceNode();
				}
			return 0;
		}
	voice_head=(struct node*)malloc(VOICE_LEN);
	if(voice_head==NULL)
		{
			TRACE("voice head malloc fail!!\r\n");
			return 1;
		}
	TRACE("voice head init ok!!\r\n");
	voice_head->next=NULL;
	return 0;
}

void Send_Voice(u8 data)
{
#ifdef NEW_VOICE_IC
	if(data>13)
		return;
#endif

#ifdef VOICE_LIST						//ʹ������LIST��������
	if(!voice_ok)
		return;
#ifndef VOICELIST_START		//������ӵ�������������list�����
	struct node *p,*q;
	p=(struct node*)malloc(VOICE_LEN);
	
	if(p==NULL)
		return;
	
	q=voice_head;
	while(q->next!=NULL)
		q=q->next;
	q->next=p;
	p->data=data;
	p->next=NULL;
#else						//������ӵ�������������list����ǰ��voice_head���棩
	struct node *p=NULL;
	p=(struct node*)malloc(VOICE_LEN);

	if(p==NULL)
		return;

	p->next=voice_head->next;
	voice_head->next=p;
	p->data=data;
#endif
#else						//��ʹ��LIST��ֱ�ӷ�����
	V_CLK_1;
	delay_us(100);
	V_CLK_0;
	delay_us(100);
	for(int i=0;i<data;i++)
		{
			delay_100us(1);
			V_DAT_1;
			delay_100us(1);
			V_DAT_0;	
		}
#endif
}

void Del_Node(void)
{
	if(voice_head->next==NULL)
		return;
#ifndef VOICELIST_START		//���µ�������List���������ɾ����һ��������Ϣ��voice_head���棩
	struct node *p=voice_head;
	p=p->next;
	voice_head->next=p->next;
	free(p);
#else						//���µ�������list��ǰ��������ɾ�����һ��������Ϣ��
	struct node *p=voice_head,*q;
	while(p->next!=NULL)
		{
			q=p;
			p=p->next;
		}
	q->next=NULL;
	free(p);
	
#endif
}

void Del_All_VoiceNode(void)
{
	if(voice_head->next==NULL)
		return;
	struct node *p,*q;
	while(voice_head->next!=NULL)
		{
			p=voice_head;
			while(p->next!=NULL)
				{
					q=p;
					p=p->next;
				}
			free(p);
			q->next=NULL;
		}
	TRACE("Del all voicenode success!!!\r\n");	
}

#ifdef ONE_LINE_VOICE
u8 Voice_Driver(u8 data)
{
	u8 temp_addr=data;
	u8 i=0;
	V_CLK_0;
//	delay_100us(50);
	if(giv_sys_time-voice_time<50)		//delay 5ms
		return 0;
	for(i=0;i<8;i++)
		{
			if(temp_addr&0x01)
				{
					V_CLK_1;
					delay_100us(6);
					V_CLK_0;
					delay_100us(2);
				}
			else
				{
					V_CLK_1;
					delay_100us(2);
					V_CLK_0;
					delay_100us(6);
				}
			temp_addr>>=1;
		}
	V_CLK_1;
	return 1;
}
#endif

u8 Voice_Driver(u8 data)
{
	u8 temp_addr=data;
#ifdef  NEW_VOICE_IC
	if (temp_addr >	13)  
		return  1;
		// temp_addr = 9; //������ 
		V_CLK_1;					//����ʱ��
		if(giv_sys_time-voice_time<50)
			return 0;
		V_CLK_0;			
		for(int i=0;i<temp_addr;i++)
			{
				delay_100us(3);
				V_DAT_1;
				delay_100us(3);
				V_DAT_0;	
			}
	return 1;							 
#else
	V_DAT_1;
	V_CLK_0;					//����ʱ��
	if(giv_sys_time-voice_time<50)
		return 0;
 	for(int i=0;i<8;i++)
		{
			V_CLK_0;			//����ʱ��

			if(temp_addr&0x01)	//׼������
				V_DAT_1;
			else
				V_DAT_0;
			temp_addr>>=1;

			delay_100us(3);		//��ʱ300us
			V_CLK_1;			//����ʱ��
			delay_100us(3);		//��ʱ300us
				
		}

//	V_CLK_0;
//	delay_100us(3);
	V_CLK_1;
	V_DAT_1;
	return 1;
#endif	
}

void Voice_Handle(void)
{
	static u8 step=0;
	if(voice_head->next==NULL)
		return;
	struct node *p;
#if 0
	p=voice_head->next;
#else
	p=voice_head;
	while(p->next!=NULL)
		p=p->next;
#endif


	switch (step)
		{
		case 0:
			voice_time=giv_sys_time;
			step++;
#ifdef USE_VOICE_BUSY
			step=2;
#endif
		if((p->data==VOICE_KEY_PRESS)|(p->data==VOICE_CHARGING_START)|(p->data==VOICE_STOP_CHARGE)|(p->data==VOICE_VOLUME_TEST))			//��������,��ϵ�ǰ����,���̷���
			step=3;
		if((p->data==VOICE_VOLUME_TEST))
			step=3;
		break;
		case 1:
			if(Voice_Driver(0xF3))				//����0xF3
				{
					step++;
					voice_time=giv_sys_time;
				}
		break;
		case 2:
			if(giv_sys_time-voice_time>20)		//��ʱ2ms
				{
#ifdef USE_VOICE_BUSY
					if(!ISVOICEBUSY)
						return;
#endif
					step++;
					voice_time=giv_sys_time;
				}
		break;
		case 3:
			if(Voice_Driver(p->data))			//����������ַ
				{
					step++;
					voice_time=giv_sys_time;
				}
		break;
		case 4:
			if(giv_sys_time-voice_time>40)		//��ʱ4ms
				{
					step=8;
					voice_time=giv_sys_time;
				}
		break;
#if 1
		case 5:
			if(Voice_Driver(0xF8))				//����0xF3
				{
					step++;
					voice_time=giv_sys_time;
				}
		break;
		case 6:
			if(giv_sys_time-voice_time>20)		//��ʱ2ms
				{
					step++;
					voice_time=giv_sys_time;
				}
		break;
		case 7:
			if(Voice_Driver(100))				//����1000ms
				{
					step++;
					voice_time=giv_sys_time;					
				}
		break;
#endif
		case 8:
#ifdef USE_VOICE_BUSY
			Del_Node();
			voice_time=giv_sys_time;
			step=0;
#else
			if((p->data==VOICE_KEY_PRESS)|(p->data==VOICE_STOP_CHARGE)|(p->data==VOICE_CHARGING_START)\
				|(p->data==VOICE_VOLUME_TEST))		//��������,����ɾ������,������ʱ
				{
					Del_Node();					
					voice_time=giv_sys_time;
					step=0;
				}
			else
				{								//����������,ɾ����ַ����ʱ
					Del_Node();
					step++;
				}
#endif
		break;
		case 9:
			if(giv_sys_time-voice_time>15000)		//10ms
				{
					step=0;
					voice_time=giv_sys_time;
				}
			break;
		}
		return;
}


void voice_usart(u8 data)
{
	u8 addr=0;
	switch (data)
		{
			case 0x01:
				addr=VOICE_SWEEPER_OK;
				Slam_Data.slam_ok=true;
				if(mode.mode!=CHARGEING)
					Open_Led(1,0,0);
			break;
			case 0x02:
				addr=VOICE_WIFI_OK;			//�������ӳɹ�
				Slam_Data.wifi_ok=true;
			break;
			case 0x03:
				addr=(VOICE_WIFI_FAIL);		//��������ʧ��
				Slam_Data.wifi_ok=false;
			break;
			case 0x04:			//�滮��ɨ
			case 0x05:			//�ر�
			case 0x06:			//����
			case 0x07:			//�ص�
				addr=(VOICE_SWEEP_START);

			//��ֹ�ڳ��������������ʱ���������缫�Ƚ���CEASE״̬�ˣ�
			//��ʱ�ͻ��ڳ������ת��������
			//qz add 20180814
			#if 1
			if((mode.mode==CEASE)&(mode.sub_mode==CEASE)&(mode.last_mode==CHARGEING)&(mode.last_sub_mode==SEAT_CHARGING)&(!dc_nobat_run))
				{
					if(giv_sys_time-mode.init_mode_time<100000)
						{
							Init_Quit_Charging(0);
						}
				}
			#endif
			//qz add end
			
			break;
			case 0x08:			//ԤԼ
				return;
			case 0x09:			//ֹͣ��ɨ
				addr=VOICE_SWEEP_STOP;
			break;
			case 0x0A:			//��ʼ�س�
				addr=VOICE_DOCK_START;
			break;
			case 0x0B:			//ֹͣ�س�
				addr=VOICE_DOCK_STOP;
			break;
			case 0x0C:			//�س�ʧ��
				addr=VOICE_DOCK_FAIL;
			break;
			case 0x0D:			//��������
				addr=VOICE_I_AM_HERE;
				Open_Led(0,50000,1);
			break;
			case 0x0E:			//������ʼ
				addr=VOICE_UPGRADE_START;
			break;
			case 0x0F:			//�����ɹ�
				addr=VOICE_UPGRADE_OK;
			break;
			case 0x10:			//����ʧ��
				addr=VOICE_UPGRADE_FAIL;
			break;
			case 0x11:			//������ֵ�̶�
				addr=VOICE_ERROR_BIRD_FAIL;
				error_code=ERROR_BIRD;				//qz add V2.3.9
				
			break;
			case 0x12:			//����ͷ��,����ͨѶ
				addr=VOICE_ERROR_BIRD_FAIL;
				error_code=ERROR_BIRDCOM;			//qz add V2.3.9
			break;
			case 0x13:			//��Ƶ���쳣
				addr=VOICE_ERROR_VIDEO_FAIL;
				error_code=ERROR_VIDEO;				//qz add V2.3.9
			break;
			case 0x14:			//gyro���쳣
				addr=VOICE_ERROR_GYRO;
				error_code=ERROR_GYRO;				//qz add V2.3.9
			break;
			case 0x15:			//���̷��������쳣
				addr=VOICE_ERROR_DIPAN_TICK;
				error_code=ERROR_MOTION_TICK;			//qz add V2.3.9
			break;
		}

#ifdef SLEEP_SUBMODE
		if((mode.mode==CEASE)&(mode.sub_mode==SLEEP))
			{
				switch(data)
					{
						case 0x01:
						case 0x04:			//�滮��ɨ
						case 0x05:			//�ر�
						case 0x06:			//����
						case 0x07:			//�ص�
						case 0x08:			//ԤԼ
						case 0x0A:			//��ʼ�س�
						case 0x0D:			//��������
						case 0x0E:			//������ʼ
						case 0x0F:			//�����ɹ�
							Init_Cease();
						break;
						default:
							break;
					}
			}
#endif
	if((addr==VOICE_DOCK_START)&(Battery.bat_per<20))
		{
			Send_Voice(VOICE_POWER_LOW);
		}
	if(addr)
		Send_Voice(addr);

}


void delay_100us(u32 delay_time)
{
	u32 time=giv_sys_time;
	while(giv_sys_time-time<delay_time);
}

void Init_Voice(void)
{
	if(Init_Voice_Head())
		{
			voice_ok=false;
			return;
		}
	voice_ok=true;
	voice_level=Read_Voice_Level();
#ifdef DEBUG_INIT
	TRACE("voice_level=0x%x\r\n",voice_level);
#endif
	if((voice_level<VOICE_LEVEL_1)|(voice_level>VOICE_LEVEL_16))
		{
			voice_level=VOICE_LEVEL_13;
#ifdef DEBUG_INIT
	TRACE("voice_level =%d is not good,rewrite!\r\n",voice_level);
#endif
		}
	Set_Voice_Level(voice_level);
#ifdef DEBUG_INIT
	TRACE("Voice init OK!\r\n");
#endif
}

void Set_Voice_Level(u8 data)
{
	voice_time=giv_sys_time;
	while(!Voice_Driver(data));
	Write_Voice_Level(data);
}

void Write_Voice_Level(u8 data)
{
#ifndef DS1307
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(VOICE_BACKUP_BKP, (u16)(data));
	PWR_BackupAccessCmd(DISABLE);
#else
	if(DS1307_Writeu8data(VALUE_BACKUP_ADDR, data))
		{
#ifdef DEBUG_Enter_Mode
			TRACE("Write DS1307 fail!\r\n");
#endif
		}
#endif
}

u8 Read_Voice_Level(void)
{
	u8 data=0;
#ifndef DS1307
	data=BKP_ReadBackupRegister(VOICE_BACKUP_BKP);
#else

	if(DS1307_Readu8data(VALUE_BACKUP_ADDR, &data))
		{
#ifdef DEBUG_Enter_Mode
			TRACE("Write DS1307 fail!\r\n");
#endif
		}
#endif
	return data;
}

