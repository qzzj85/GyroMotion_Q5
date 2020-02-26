#include "AAA-include.h"
#include "stdlib.h"

#define VOICE_LEN sizeof(struct node)

static uint32_t voice_busy_time=0;
static bool voice_busy=FALSE;

uint8_t Check_Voice_Busy(void)
{
	if(voice_busy)
		{
			if(giv_sys_time-voice_busy_time>10000)
				{

					voice_busy=FALSE;
					return 0;
				}
			else
				{
					return 1;
				}
		}
	else
		return 0;
}


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
#ifdef DEBUG_VOICE
			TRACE("voice head is exist!!\r\n");
#endif
			if(voice_head->next!=NULL)
				{
					Del_All_VoiceNode();
				}
			return 0;
		}
	voice_head=(struct node*)malloc(VOICE_LEN);
	if(voice_head==NULL)
		{
#ifdef DEBUG_VOICE
			TRACE("voice head malloc fail!!\r\n");
#endif
			return 1;
		}
#ifdef DEBUG_VOICE
	TRACE("voice head init ok!!\r\n");
#endif
	voice_head->next=NULL;
	return 0;
}


void Del_Node(void)
{
	if(voice_head->next==NULL)
		return;
	struct node *p=voice_head,*q;
	while(p->next!=NULL)
		{
			q=p;
			p=p->next;
		}
	q->next=NULL;
	free(p);
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
#ifdef DEBUG_VOICE
	TRACE("Del all voicenode success!!!\r\n");	
#endif
}



void Voice_Handle(void)
{
	static u8 step=0;

	if(Check_Voice_Busy())
		return;
	
	if(voice_head->next==NULL)
		return;
	struct node *p;
	p=voice_head;
	while(p->next!=NULL)
		p=p->next;

	
	static uint8_t bit_cnt=0;
	static u32 voice_delay_time=0;
	switch(step)
		{
			case 0:
				V_CLK_0;
				V_DAT_0;
				voice_delay_time=giv_sys_time;
				step++;
				bit_cnt=p->data;
				if(bit_cnt==0)
					Del_Node();
				return;
			case 1:
				V_CLK_1;
				voice_delay_time=giv_sys_time;
				step++;
				return;
			case 2:
				if(giv_sys_time-voice_delay_time<3)
					return;
				V_CLK_0;
				voice_delay_time=giv_sys_time;
				step++;
				return;
			case 3:
				if(giv_sys_time-voice_delay_time<3)
					return;
				bit_cnt--;
				V_DAT_1;
				voice_delay_time=giv_sys_time;
				step++;
				return;
			case 4:
				if(giv_sys_time-voice_delay_time<3)
					return;
				V_DAT_0;
				if(!bit_cnt)
					step++;
				else
					step=3;
				voice_delay_time=giv_sys_time;
				return;
			case 5:
				if(giv_sys_time-voice_delay_time<3)
					return;
				V_CLK_0;
				V_DAT_0;
				bit_cnt=0;
				step=0;
				break;
		}
	if(p->data==VOICE_WIFI_OK)
		wifi_ok=true;

	voice_busy=TRUE;
	voice_busy_time=giv_sys_time;
	Del_Node();
	return;
}

void Init_Voice(void)
{
	if(Init_Voice_Head())
		{
			voice_ok=false;
			return;
		}
}

void delay_100us(u32 delay_time)
{
	u32 time=giv_sys_time;
	while(giv_sys_time-time<delay_time);
}
void Send_Voice(u8 data)
{
#ifdef DEBUG_VOICE
	TRACE("vd=%d\r\n",data);
#endif

	if(data>13)
		return;

	if(!voice_ok)
		return;
	struct node *p=NULL;
	p=(struct node*)malloc(VOICE_LEN);

	if(p==NULL)
		return;

	p->next=voice_head->next;
	voice_head->next=p;
	p->data=data;
}


