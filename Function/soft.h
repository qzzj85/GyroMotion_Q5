  
#ifndef  __SOFT_H
#define  __SOFT_H
#include "stm32f10x.h"

#define     TUYA_WIFI     // �Լ����Ƴ���������޸�         
#define     TUYA_ADD      // ��tuyaԭ����sdk���޸� 
           // ��tuya��sdk ����wifi.h ����Ҳ��Ҫ������������ 


//#define 	USE_HSE					1
#define		USE_LSE					1
#define 	CPU_FREQ_64MHz 			1
#define 	FLOAT_PID 				1
//#define		FREE_SKID_ACTION		1					//�����ִ򻬶�������
//#define		ROTATE_SKID_ACTION		1				//��ת�򻬶�������
#define		ROTATE_SKID_DEBUG		1					//��ת��DEBUG
#define		SKID_REPORT_TIME		1
#define 	HW_INT_REC 				1					//�����жϽ���

#define		DEBUG					1
#ifdef 		DEBUG
#define 	QZ_DEBUG_DOCK 			1
#define 	DOCK_DEBUG				1
#define 	PREEN_DEBUG				1
#define		YBS_DEBUG				1
#define		REMOTE_DEBUG			1
#define 	DEBUG_Enter_Mode		1
#define     DEBUG_INIT				1
#define		EFFICENT_DEBUG			1
#define		TEST_DEBUG				1
//#define	DEBUG_SPEED				1
#define		DEBUG_CHARGE			1
#define		DEBUG_POWER				1
#endif

#if 0
//#define	FREE_SKID_CHECK			1					//�����־���򻬼�⣬�����ϱ�������
//#define	FREE_SKID_INDEP_CHECK	1					//���������ּ�⣬���ڿ�ס
//#define 	ROTATE_SKID_CHECK		1					//��ת�򻬼��
#define		FAN_CHECK				1					//������
//#define	LIFT_CHECK				1					//��ؼ��
//#define 	SLAM_CHECK				1					//SLAMTICK���
#define		BUMP_FIX_CHECK			1					//��ײ�������
#define		RING_FIX_CHECK			1					//�������������
//#define 	RING_OC_CHECK			1					//�����ֹ������
#define		MB_OC_CHECK				1					//��ɨ���������
#define		DUST_BOX_EXIST_CHECK	1					//�����м��
#define		SB_FIX_CHECK			1					//��ɨ���Ƽ��
#define		GYRO_TICK_CHECK			1					//�ߵ����ݼ��,5s��δ�ӵ��վ�,�ж�����
//#define	WALL_EARTH_ERROR_CHECK	1					//ǽ�ؼ��쳣���(����û��)
#endif
#define		GYRO_BIOS_CHECK			1
#define		CLIFF_ENABLE			1					//�ؼ����¼��

#define 	SLAM_CTL_FAN			1
#define 	YBS_BLACK				1					//�ر�ʱ��ɫǽ�崦��
#define		PWRCAP_CAL				1					//�����Ƶ�������
//#define 	OBSTACLE_CURRENT_CHECK	1					//����ʱ,����������,��ȥ������
#define	 	YBS_AVOID_SEAT			1				//�ر߳��������
#define		YBS_SEAT_REPROT			1					//�ӱ�ʱ���ֳ�����ϱ���־
#define		YBS_DIS_RESTORE			1					//�ر߹̶����������Ժ��Զ��ظ�
//define	COMMANDER_SEAT_REPORT	1					//����ģʽ�·��ֳ�����ϱ���־
#define		SWITCHOFF_SUBMODE		1					//���Ϳ���δ��ģʽ
//#define		SLEEP_SUBMODE			1				//��������ģʽ
//#define		DOCK_NEAR				1				//��������ź�
//#define		WALL_DARK_FIX			1				//ǽ�찵���ݹ̶�
#define		WALL_DARK_CAL			1					//ǽ�찵����У��,500ms��һ��
#define		DOCK_GOBACK_STRAIGHT	1					//�س��״��յ������źź�ֱ��
#define		YBS_START_RIGHT_ANGLE	1					//�ر���ʼ����ֱ��1m,��ת90��,ֱ��5m
#define		DC_NOBAT_RUN			1					//�޵��,DCֱ���ֱ��
#define		FREE_SKID_INDEP_ACC		1					//ʹ��ֱ��ʱ���ۼӵķ����ж϶��������ִ�
#define		NEW_PWR_CAL				1					//ʹ��2600MAH��ʼ����ؼ��������㷽��
//#define		FINDSEAT_SKID_CHECK		1					//���ڻس�ʱ���ϳ���������ֲ��������
#define		COMMAND_CLIFF_ACTION	1					//�����¹�ܶ���
#define		REMOTE					1				//ʹ��ң��������
#define		REPEAT_DOCK				1					//�ٽ����س书��
#define		SHUTDOWN_MODE			1					//�ػ�����
#define		INTERFERENCE_ACTION		1					//�����źſ�����
//#define		UV						1				//UV�ƹ���
#define 	REYBS2DOCK				1					//С�س��źŶ�ʧ�ٴ������ر�
#define		DUMMY_WALL				1					//����ǽ���ر�ģʽ���ܹ����ܷ�00�ٶ�ָ������ٶ�ģʽ
//#define		NARROW_PASS				1				//�ر�խͨ������,��Ҫ��������ײ��ء�
#define		GYRO_ROLL_CHECK			1					//�ߵ������Ǽ�⣨��ֵ5�ȣ�
#define		FREE_FIRST_BAD_CHECK	1					//�����ֹ��ϼ�⣬����ÿ�Ρ���ʼ��ɨ����ĵ�һ��Free_Skid_Indep_Check
//#define 	DS1307					1					//ʹ��DS1307 RTC���󱸼Ĵ���
#define  	USE_VOICE_BUSY			1					//ʹ������оƬBUSY����
#define		USE_BITBAND				1					//GPIO��дʹ��BITBAND����
#define		STOP_WEEKUP				1					//���Դ�STOPģʽ�л���
#define		SPEED_EXTI				1					//�ٶ�ʹ���ⲿ�ж�������
//#define		MHW_CAPTURE				1
//#define		GYRO_PITCH_CHECK		1				//�ߵ������Ǽ��
//#define		GYRO_PITCHROLL_CHECK	1
//#define		PITCH_SPEEDUP			1			
#define		RING_PWM_CTL			1					//����PWM�жϿ���
#define		GYRO_CAL				1				//�ߵ�У׼����
//#define		GYRO_COMPENSATION		1					//�ߵ��ǶȲ���
//#define		MILE_COMPENSATION		1					//��̼Ʋ���
//#define		STOP_SPD_CNT			1
#define		TEST_ONESTEP			1
#define		EARTH_IN_TIM2			1
//#define		OUT_8MHZ				1
#define		YIS055					1

#define 	MAIN_VERISON 			1
#define 	SUB_VERSION				3
#define		CORRECT_VERSION			7

#define 	PREEN_DATA_ADDR  		0X0807F800			//7��ԤԼʱ��洢��ַ�����һ��ҳ
#define		BAT_REINIT_ADDR			0x0807FFFC			//���һ���ֽ�


/////////////�ϱ�������������//////////////////////
#define		ERROR_DOCK_FAIL			0xF001
#define		ERROR_DC_EXIST			0xF003
#define		ERROR_BIRD				0xE011
#define		ERROR_BIRD_COMM			0xE012
#define		ERROR_VIDEO				0xE021
#define		ERROR_GYRO				0xE031
#define		ERROR_SLAM_TICK			0xE041
#define		ERROR_SLAM_FAIL			0XE042
#define		ERROR_LIFT				0xF003//0xE051
#define		ERROR_DUSTBOX_NONE		0xE001
#define		ERROR_FAN_FIX			0xE102
#define		ERROR_BUMP_FIX			0xE002
#define		ERROR_IR_FIX			0xE121
#define		ERROR_IR_FAIL			0xE122
#define		ERROR_RING_FIX			0xE131
#define		ERROR_RING_OC			0xE132
#define		ERROR_SWEEP_FIX			0xE133
#define		ERROR_MB_OC				0xE141
#define		ERROR_SB_OC				0xE151
#define		ERROR_SB_FIX			0xE152
#define		ERROR_BAT_TEMP			0xE091

////////////��ʾ�������//////////////////////
#define		DIS_ERROR_BOX_NONE			0XE000
#define		DIS_ERROR_DANGER			0XE001
#define		DIS_ERROR_BUMP_FIX			0XE002
#define		DIS_ERROR_LEFT_LIFT			0XE004
#define		DIS_ERROR_RIGHT_LIFT		0XE005
#define		DIS_ERROR_LEFTRING_OC		0XE006
#define		DIS_ERROR_RIGHTRING_OC		0XE007
#define		DIS_ERROR_LEFTSB_OC			0XE008
#define		DIS_ERROR_RIGHTSB_OC		0XE009
#define		DIS_ERROR_MB_OC				0XE010
#define		DIS_ERROR_GYRO				0XE011
#define		DIS_ERROR_LEFTWALL			0XE012
#define		DIS_ERROR_MIDWALL			0XE013
#define		DIS_ERROR_RIGHTWALL			0XE014
#define		DIS_ERROR_LEFTEARTH			0XE015
#define		DIS_ERROR_MIDEARTH			0XE016
#define		DIS_ERROR_RIGHTEARTH		0XE017
#define		DIS_ERROR_FAN_OC			0XE018
#define		DIS_ERROR_SLAM_DOWN			0XE019
#define		DIS_ERROR_DIPAN_TICK		0XE020
#define		DIS_ERROR_BAT_DOWN			0XE021
#define		DIS_ERROR_VIDEO_DOWN		0XE022
#define		DIS_ERROR_BIRD_DOWN			0XE023
#define		DIS_ERROR_SWEEPER_FIX		0XE024
#define		DIS_ERROR_SWEEPER_LIFT		0XF001
#define		DIS_ERROR_DOCK_FAIL			0XF002
#define		DIS_ERROR_DC_EXSIT			0XF003


#define		ERROR_RATATE_SKID		0xE200
#define		ERROR_FREE_SKID_INDEP	0xE201

////////////�����ϱ�����������///////////////
#define 	DIPAN_REQ_NONE			0
#define		DIPAN_REQ_SWEEP			1
#define 	DIPAN_REQ_DOCK			2
#define 	DIPAN_REQ_WIFI			3
#define 	DIPAN_REQ_YBS			4
#define 	DIPAN_REQ_SPOT			5
#define 	DIPAN_REQ_PREEN			6
#define		DIPAN_REQ_TURNOFFSLAM	7
#define		DIPAN_REQ_BURNNING		8
#define		DIPAN_REQ_FORWORD		9
#define		DIPAN_REQ_BACK			0X0A
#define		DIPAN_REQ_LEFT			0X0B
#define		DIPAN_REQ_RIGHT			0X0C
#define		DIPAN_REQ_STOP			0X0D
#define		DIPAN_REQ_SLEEP			0X0E
#define		DIPAN_REQ_DOCKYBS		0X0F	//���̽���С�س��Ժ��ֶ�ʧ�źţ������ٴν����ر�
#define		DIPAN_REQ_FACTORY		0X10

#define		SWEEP_METHOD_GUIHUA		0
#define		SWEEP_METHOD_YBS		1

//QZ:dm=0,return; dm=1,��ת; dm=2,��ת; dm=3,ǰ��; dm=4,����; dm=5,���ֲ�������ת; dm=6,���ֲ�������ת;
//	 dm=7,���ֲ�������ת; dm=8,���ֲ�������ת 18:˳ʱ����������; 19:��ʱ����������; 21 ��������
///////////////////������do_action������////////////////////////
#define  TURN_LEFT  				1
#define  TURN_RIGHT  				2
#define  FORWARD     				3
#define  BACKWARD    				4
#define  TURN_LEFT_STAY_L_WHEEL   	5
#define  TURN_RIGHT_STAY_R_WHEEL  	6
#define  TURN_LEFT_STAY_R_WHEEL   	7
#define  TURN_RIGHT_STAY_L_WHEEL  	8

#define  HELIX_CLK_WISE           18//������˳ʱ��
#define  HELIX_ANTI_CLK_WISE      19//��������ʱ��

#define  BUMP_WALL_LEFT              7
#define  BUMP_WALL_LEFT_MID          8
#define  BUMP_WALL_MID               9
#define  BUMP_WALL_RIGHT_MID         15
#define  BUMP_WALL_RIGHT             14

////////////������PWM������ʹ�õ�����/////////////////////////
#define   L_FRONT     0x1   //������ǰ
#define   L_BACK      0x2   //�������
#define   R_FRONT     0x3   //������ǰ
#define   R_BACK      0x4   //�������
#define   CHARGE_PWM  0x5   //������


/////////////�����Ǻ�����յ����ݶ���////////////////////////////
#define  L_CHARGE         	0xe0		
#define  M_CHARGE         	0xb0
#define  R_CHARGE         	0xd0
#define  UP_CHARGE         	0x70
#define  L_UP_CHARGE        0x60 
#define  M_UP_CHARGE        0x30 
#define  R_UP_CHARGE  			0x50
#define  L_M_CHARGE         0xa0
#define  L_R_CHARGE         0xc0
#define  R_M_CHARGE         0x90 
#define  L_M_UP_CHARGE      0x20   
#define  L_R_UP_CHARGE      0x40   
#define  R_M_UP_CHARGE   		0x10
#define  L_M_R_CHARGE 			0x80
#define  L_M_R_UP_CHARGE 		0x00
#define  V_DUMMYWALL        0xdc 
#define  UP__DUMMYWALL      0xec    
#define  V_UP_DUMMYWALL     0xcc

#define  B_HW_MASK			0x0040
#define	 L_HW_MASK			0X0100
#define	 R_HW_MASK			0X0400
    

/////////////������ң�����İ�������//////////////////////////////
#define  YAOKONG_ESC           	10	 //�˳���ȡ�����رյ�Դ
#define  YAOKONG_TIMINE_MON	   	6	   //��ʱ������һ
#define  YAOKONG_SETTIME_TUE   	9	   //�趨ʱ�䡢���ڶ�
#define  YAOKONG_UV_WED		   		3	   //UV��������
#define  YAOKONG_AUTO_THU	   		1	   //������ɨ��������
#define  YAOKONG_SPOT_FRI	   		4	   //�Զ���ɨ��������
#define  YAOKONG_V_SAT		   		8	   //�ٶ��趨��������
#define  YAOKONG_DOCKING_SUN   	5	   //�س䡢������
#define  YAOKONG_ED			   			2	   //ÿ��
#define  YAOKONG_LEFT		   			12	 //����
#define  YAOKONG_RIGHT		   		13	 //����
#define  YAOKONG_FRONT		   		14	 //��ǰ
#define  YAOKONG_BACK		   			11	 //���
#define  YAOKONG_OK			   			7	   //ȷ��
                         
///////�����ǵؼ��ǽ���״̬����///////////////////////////

#define     NEAR            1                              //��
#define     FARN            0                              //Զ


#define    LEFT             1            	//����������
#define    RIGHT            2            	//����������
#define 	 STOP 			0X03

#define     FRONT           0X01    			// ��ǰ
#define     BACK            0X02 					// ���

#define     BAD             0X01 					//���ֲ��ܶ�
#define     GOOD            0X00 					//��������

#define     LI_ANGLE        0X90         //��ǽ��
#define     WAI_ANGLE       0XA0         //��ǽ��
#define     U_ANGLE         0XB0         //U��ǽ��

#define    POWER_DOWN       0X003        //��ʾ���û��
#define    POWER_TEMP       0x005        //��ʾ����¶ȹ���
#define    POWER_NTC        0X006        //���û��NTC
#define    POWER_CHARGE     0X007        //������ڳ��
#define    CHARGE_ERR       0X009        //������

/////////////////����ģʽ/////////////////////////
#if 0
#define     CEASE            0X00                //����ģʽ
#define     SWEEP            0X01                //ɨ��ģʽ
#define     SPOT             0X02                //������ɨ
#define     TELE             0X03                //ң��ģʽ
#define     DOCKING          0X04                //�Զ��س�ģʽ
#define     CHARGEING        0X05				 //���ģʽ
#define     SLEEP            0X06                //˯��ģʽ 
#define     ERR              0X07				 //����ģʽ
#define 	SWEEP_GONG       0X80				 //������ɨ
#endif
//qz add ����Э��
//////////////////////��ģʽ///////////////////////////////
#define CEASE 				0x01
#define	MODE_REMOTE			0X02
#define YBS 				0x03
#define SPOT 				0x06
#define LUOXUAN 			0X06
//#define SLAM_DOCK_MODE	0x04	//qz 20180417:��س�
#define DOCKING 			0X04		//qz 20180416:С�س�
#define	SWEEP				0X07		//��ɨ����ɨ��ģʽ
#define	SHIFT				0X08		//ת�Ƽ�ת����ģʽ
#define EXIT				0X09		//�˳�����ģʽ
#define CHARGEING 			0x0D
#define SLEEP 				0x0E
#define TEST				0x0F
#define PASS2INIT			0X10
#define ERR 				0xFF

//////////////////////��ģʽ/////////////////////////////////
#define QUIT_CHARGING 		0XFE		//�˳��������ģʽ����ģʽ��CEASE
#define	SELF_TEST			0xFD		//�Բ���ģʽ,��ģʽ��CEASE
#define	BURNIN_TEST			0xFC		//�ϻ���ģʽ,��ģʽ��TEST
#define	DEAD				0xFB		//�͹�����ģʽ,��ģʽ��CEASE
#define	SWITCHOFF			0XFA		//����δ����ģʽ,��ģʽ��CHARGE
#define	YBS_SUB_RIGHT		0XF9
#define	YBS_SUB_LEFT		0XF8
#define	DC_CHARGING			0XF7		//DCֱ����ģʽ����ģʽ��CHARGING
#define	SEAT_CHARGING		0XF6		//��������ģʽ, ��ģʽ: CHARGING
#define SHUTDOWN			0XF5		//�ػ�ģʽ����ģʽ��CEASE
#define FACTORY_TEST		0XF4		//������ģʽ,��ģʽ��CEASE
#define	YBS_SUB_EXCHANGE 	0XF3

#define	NORMAL_SWEEP		0XF3
#define	PASS2SWEEP			0XF2
#define	BACK2YBSSTART		0XF1
#define	BACK_SWEEP			0XF0
#define	STOP_BACKSWEEP 		0XEF
#define	SWEEP_FIRST_INIT	0XED

#define SHIFTPOINT1			0XD0
#define	SHIFTPOINT2			0XD1
#define LEAK_SWEEP			0XD2
#define PASS2LEAKSWEEP  	0XD3
#define SHIFT_LEFT_YBS  	0XD4
#define SHIFT_RIGHT_YBS 	0XD5
#define	LEAK_BACKSWEEP		0XD6
#define	STOP_LEAKBACK		0XD7

#define	RUN_TEST			0xC0

#define	SUBMODE_REMOTE_MOVE			0XB0
//#define     COMMANDER_x10           0X10         //  ָ��ģʽ

//#define     YBS           0X20    	     //  ��ǽģʽ
//#define     YBS_YBS_x21           0X21        
#define     YBS_YBS_x22_FAST      0X22
#define     YBS_YBS_x23_FAST      0X23

////////////////�������ٶȵĶ���//////////////////
#define     HIGHSPEED      4000//2400//1200  			//	  2400       //	���ٵ��ٶ�
#define     LOWSPEED       2000//1200//600  			//	1200          //	���ٵ��ٶ�

#define			MaPan_XiFen				2				//����ϸ������   1 ���� 2

//�������
//����ֱ��70mm
//������ 81
//������ 20��תһȦ20�����壩
//�����ֵľ���235mm
//���ת�٣�12000��10%rpm


///////*************************************************************
//////������ֱ����68mm	

//////-------->>>>>>>>���ӵ��ܳ���:3.14*68 = 213.52mm		//zdk

//////������  61.6
//////������  16
//////һ������ĳ�����68*3.14/61.6/16=0.2167mm
//////���־���  267.6mm��

//////-------->>>>>>>>��תһȦÿ�������ߵĳ���:3.14*267.6 = 840.264mm		//zdk

//////ëˢ   ���82mm  �ұ�  75mm ��  ƽ��ëˢ150mm


//////���⣬����תһ�ܵ�r_ring.length = 0x7D0 = 2000		//zdk
//////���ԣ�һ�׵ľ��������������: 1000mm/213.52mm  * 2000 = 4.6834 * 2000 = 9366.8	//zdk
//////���ԣ�һ������ĳ��Ⱦ���213.52 / 2000 = 0.10676mm		//zdk
//////���ԣ�����ת��һȦÿ���������ߵ���������:840.264 / 0.10676 = 7870.588 Լ����7871 //zdk


////////QZ ADD,��������
//////һ������ĳ�����68*3.141592/61.6/16/2=0.108374mm
//////���־���267.6mm
//////ԭ����ת�Դ�һȦÿ�������ߵĳ��ȣ�3.141592*267.6=840.690019mm
//////�������壺840.690019/0.108374=7757.303587
//////ת90�����壺1939.325897
//////ת180�����壺3878.651793

//////��һ��ΪԲ����תһȦÿ�������ߵĳ��ȣ�267.6*2*3.141592=1681.380038mm
//////�������壺1681.380038/0.108374=15514.607175
//////ת90�����壺3878.651793

////////QZ ADD END
//////*************************************************************/


//hzh
#define    PULSES_PER_MM    14.74
#define    METE             (int)PULSES_PER_MM*1000+750        //1�׵�������   ������            

#define    METE_0_1         (int)PULSES_PER_MM*110   //�ѵ���
#define    METE_0_2         (int)PULSES_PER_MM*220   //�ѵ���
#define    METE_0_3         (int)PULSES_PER_MM*323   //�ѵ���
#define    METE_0_4         (int)PULSES_PER_MM*432   //�ѵ���
#define    METE_0_5         (int)PULSES_PER_MM*530   //�ѵ��� 
//�������ݿ�����ȥ����Ҫ�ٵ�����c�ļ����޸ĺ꣬�����޸�h�ļ���ȫ���ر���


/************************************************************************************/
//hzh 
#define  ANGLE_5      (int)PULSES_PER_MM * 2.0497 *9    //3.14*235/360   �ѵ���  
#define  ANGLE_10     (int)PULSES_PER_MM * 2.0497 *14   //�ѵ���
#define  ANGLE_15     (int)PULSES_PER_MM * 2.0497 *19   //�ѵ��� 
#define  ANGLE_30     (int)PULSES_PER_MM * 2.0497 *34   //hzh   �ѵ���
#define  ANGLE_45     (int)PULSES_PER_MM * 2.0497 *50   //hzh   �ѵ���   
#define  ANGLE_60     (int)PULSES_PER_MM * 2.0497 *66   //hzh   �ѵ���
#define  ANGLE_90     (int)PULSES_PER_MM * 2.0497 *94   //hzh   �ѵ���
#define  ANGLE_180    (int)PULSES_PER_MM * 2.0497 *190  //hzh   �ѵ���
#define  ANGLE_360    (int)PULSES_PER_MM * 2.0497 *379  //hzh  �ѵ���                
#define  ANGLE_720    (int)PULSES_PER_MM * 2.0497 *763  //hzh  �ѵ���
/************************************************************************************/  




#define     SPEED           1000        // ����Ϊ���ֵ�����ٶ�   
#define     FARAWAY         0XFFFFFFFF   //����Ϊ����Զ
#define     LENGTH		    243			 //���ֵĿ��  ��λ�Ǻ���

////////////#define     METE            4615         //1�׵�������
////////////#define     METE_H5         2308         //0.5�׵ľ���
////////////#define     METE_H6         2769         //0.6�׵ľ���
////////////#define     METE_H7         3231         //0.7�׵ľ���
////////////#define     METE_H8         3692         //0.8�׵ľ���
////////////#define     METE_H9         4154         //0.9�׵ľ���
////////////#define     METE_TWO		5130         //2�׵�������
////////////#define     METE_0			5592		 //1.1�׵�������
////////////#define     METE_1          6053         //1.2�׵�������	
////////////#define     METE_2			6515		 //1.3�׵�������
////////////#define     METE_3			6976		 //1.4�׵�������
////////////#define     METE_4			7438		 //1.5�׵�������
////////////#define     METE_5			7899		 //1.6�׵�������
////////////#define     METE_6			8361		 //1.7�׵�������
////////////#define     METE_7			8822		 //1.8�׵�������
////////////#define     METE_8			9384		 //1.9�׵�������
////////////#define     METE_THREE		13845		 //3�׵�������

#define F1  38




//#define     METE            4615*MaPan_XiFen *F1/50        //1�׵�������
#define     METE_H5         2308*MaPan_XiFen *F1/50         //0.5�׵ľ���
#define     METE_H6         2769*MaPan_XiFen  *F1/50         //0.6�׵ľ���
#define     METE_H7         3231*MaPan_XiFen   *F1/50        //0.7�׵ľ���
#define     METE_H8         3692*MaPan_XiFen    *F1/50       //0.8�׵ľ���
#define     METE_H9         4154*MaPan_XiFen    *F1/50       //0.9�׵ľ���
#define     METE_TWO				5130*MaPan_XiFen   *F1/50         //2�׵�������
#define     METE_0					5592*MaPan_XiFen *F1/50  		 //1.1�׵�������
#define     METE_1          6053*MaPan_XiFen     *F1/50       //1.2�׵�������	
#define     METE_2					6515*MaPan_XiFen	 *F1/50  	 //1.3�׵�������
#define     METE_3					6976*MaPan_XiFen	 *F1/50  	 //1.4�׵�������
#define     METE_4					7438*MaPan_XiFen	 *F1/50  	 //1.5�׵�������
#define     METE_5					7899*MaPan_XiFen	 *F1/50  	 //1.6�׵�������
#define     METE_6					8361*MaPan_XiFen	 *F1/50  	 //1.7�׵�������
#define     METE_7					8822*MaPan_XiFen	 *F1/50  	 //1.8�׵�������
#define     METE_8					9384*MaPan_XiFen	 *F1/50  	 //1.9�׵�������
#define     METE_THREE			13845*MaPan_XiFen	 *F1/50  	 //3�׵�������

#define     METE_SIX		METE_THREE*2	 *F1/50  	 //6�׵�������



////////////////////////////////////����PID�㷨��ϵ��///////////
#define     KP              40//20     //8
#define     KI              10    //30
#define     KD              80    //150

#define 	F_KP			0.02//0.1//0.5//0.02//0.025
#define		F_KI			0.1	//1.5	//0.1
#define		F_KD			0.0125	//0.2//0.0125//0.0125

#define		D_KP			0.01
#define		D_KI			0.05
#define		D_KD			0.0125

/*************************************************************
������ֱ����68mm
������  61.6
������  16  �ӱ�
һ������ĳ�����68*3.14/61.6/32=0.10831980mm��
ÿ��������������9.23192206819
ÿ������������9231.92206819
���־���  267.6mm��
ëˢ   ���82mm  �ұ�  75mm ��  ƽ��ëˢ150mm

*************************************************************/

/************************************************************
QZ ADD
�������
����ֱ��70mm
������ 81
������ 20��תһȦ20�����壩
�����ֵľ���235mm
���ת�٣�12000��10%rpm

����ת��һȦ��������81*20*2=3240
һ�����峤�ȣ�70*3.141592/3240=0.067874mm
����1mm���壺14.733182
����1cm���壺147.331821
����1M���壺14733.182073
���ּ�ࣺ235mm
��ת1�����壺235*3.141592/360/0.067874=30.214242
��ת1�����壺235*3.141592/0.067874=10877.12703
��ת45�����壺235*3.141592/360*45/0.067874=1359.64089~~1360
��ת180������:30.214242*180=5438.56356~~5440

����ʵ�⣺����������ٶ���350mm/s���ң���������ٶ�Ϊ300mm/s����4420������
***************************************************************/

/*************************************************************
QZ
�ܹ�����
������ֱ����68mm
������:57.23
���̣�16

����תһȦ����������16*57.23*2=1831.36
һ������ĳ��ȣ�68*3.141592/1831.36=0.11665mm
����1mm���壺8.572653
����1cm����85.726532
����1M��8572.653236
���ּ�ࣺ220mm
��ת1�����壺220*3.141592/360/0.11665=16.458309
***************************************************************/

/*************************************************************
QZ
Q5����
�������ܳ���194.68mm
������:49.9
�ֺ�14.8
���̣�8
����תһȦ����������8*49.9*2=798.4
һ������ĳ��ȣ�194.68/798.4=0.243838mm
����1mm���壺4.1010835
����1cm���壺41.010835
����1M��4101.083506
���ּ�ࣺ216mm
��ת1�����壺216*3.141592/360/0.243838=7.730359
Ħ��ϵ����5.9461	����k=(speed^2)/(2gl),gΪ������lΪɲ�����ȣ�speedΪ�ٶ�
����������2300g
�����ٶȼ���Ħ�����룺l=speed^2/6.11413/2/2365�������ٶ�
***************************************************************/

#define   DIS_XiShu 	1.0255
#define   XISHU_1		1.133787	//QZ ADD

#define  F2  44
//#define   MM_PLUS  			9.23192206819 * DIS_XiShu * F2/50 * XISHU_1	//QZ ADD XISHU_1	9.44586946440 ԭ��Ϊ8.3312557512
//#define   CM_PLUS  			92.3192206819 * DIS_XiShu * F2/50 * XISHU_1	//QZ ADD XISHU_1
//#define   METER_PLUS  		9231.92206819 * DIS_XiShu * F2/50 * XISHU_1	//QZ ADD XISHU_1
#define PULSE_LENGTH	0.243838//0.1219188		//QZ ADD�������ٶ�ʱ�������������2
#define MM_PLUS 		4.1010835//8.202180		//QZ ADD
#define CM_PLUS 		41.010835//82.021805		//QZ ADD
#define METER_PLUS 		4101.083506//8202.180468	//QZ ADD
#define MAX_SPEED 		1600 			//qz ��������ٶ�,Լ305mm/s
#define	RING_RANGE		216				//���ּ��216mm
#define	LENGTH_CAL		6858		//�ٶȵ�ƽ��ֱ�ӳ���LENGTH_CAL��Ϊ�������ȡ�LENGTH_CAL=5.9461*2*2365*PULSE_LENGTH

#define 	Angle_1 		7.730359//15.460746
#define   	angle60  		Angle_1*60
#define   	angle75  		Angle_1*75
#define   	angle90			Angle_1*90
#define   	angle90_D		Angle_1*180
#define   	angle180		Angle_1*180
#define   	angle360      	Angle_1*360				//QZ ADD:�����
#define   	angle720  		angle360*2			//qz add:

#define		SPEED250		1000	//250mm/s
#define		SPEED200		820		//200mm/s
#define		SPEED150		600		//150mm/s
#define		SPEED100		400		//100ms/s
#define		SPEED50			200		//50mm/s

#define		TOP_MOVE_SPEED		SPEED250
#define		FAST_MOVE_SPEED		SPEED200
#define		HIGH_MOVE_SPEED		SPEED150
#define		MID_MOVE_SPEED		SPEED100
#define		LOW_MOVE_SPEED		SPEED50

#define		REVOLUTION_SPEED_HIGH	1000
#define		REVOLUTION_SPEED_LOW	400


typedef struct
{
//	 char	ComTxBuf[256];	  //ͨ�Ż�����
//	 char	ComRxBuf[500];	  //ͨ�Ż�����
	u32	RevLength;			//DMA�������ݳ���		
	u32	TxdDataNum;			//�������ݸ���	u32	TxdDataNum;		//�������ݸ���

	volatile bool	Trans_Busy;		//ͨ�ŷ���æ 0=��æ 1=æ
	unsigned short	TimeOut_Flag;	//ͨ�ų�ʱʱ��
	unsigned short	TimeOut;			//ͨ�ų�ʱʱ��
	u32  	Com_State;		//ͨ�����ݴ����ʶ  0=��ͨ�Ŵ���  5=��Ҫ����ɨ��(SCAN)���� 6=�ȴ�ɨ��(SCAN)����� 0xff=ͨ�Ŵ���ʶ��
	bool Status;    //QZ
	volatile bool Rece_Busy;	//QZ
	volatile bool Rece_Done;	//QZ
}UART;

typedef struct 			  //�����Ľṹ
{
     unsigned char key;		 //��ֵ  ����ǰ״̬
	 unsigned char name;
     unsigned char number;     //ȥ������
     unsigned char (*read_start)(); //��ȡ��״̬����
     unsigned char sign;	 //��־  ��ʾ�ð�����Ч��û�б�����
		 unsigned char flag; 
	u32 start_check_time;	//��ʼ���ʱ��
	u32 MAX_CHECK_TIME;
	u8 check_step;
	bool fail;				//qz add 20180522
	bool press;				//qz add 20180801
	bool long_press;		//qz add 20180605 3s
	bool long_long_press;	//qz add 20180605 6s
}KEY;

typedef struct 			  //ң����
{
    unsigned int effect_time;   //����ʱ��
	unsigned char old_key;        //�ɰ���
    unsigned char effect;         //������Чʱ��
    unsigned char key;		      //��ֵ
    unsigned char sign;	          //�а��� 
	unsigned char test;			  //���Ե�ң������־  0��ʾ�Լ���ң������1��ʾ�ź����ɱ���ΪFFFF��ң�����������ź�
}YAOKONG;

typedef struct 			  //��������ֵ�Ľṹ
{
 bool uv;		 
 unsigned char work_mode;	 
 unsigned char speed;
}WORK_MODE;
typedef struct
{
 unsigned int time;//������������ʱ��    
 unsigned char  odds;  //�ж������ı�׼
 unsigned int samp;  //��һ��ʱ������������ĸ�����
 unsigned char  step;  //�������Ĺ��̲���
 unsigned char  sign;  //�������ı�־  0��ʾ�����٣�1��ʾ������
}MUCK;

typedef struct             //ǽ��ؼ�����ݽṹ
{
  unsigned short brightness;	 //�������ʱ������ֵ
  unsigned short darkness;       //�������ʱ������ֵ
  unsigned char  near;	     //���ش���
  unsigned char  farn;	         //Զ�ش���
  unsigned short difference   ;  //�����ķ���ֵ
  unsigned int low_door;       //����ֵ	 
  unsigned int high_door;       //����ֵ
  unsigned char  sign;	         //���Ƶı�־
  unsigned char  high_sign;	         //���Ƶı�־	
  unsigned char  high_near;	     //���ش���
  unsigned char  high_farn;	         //Զ�ش���
  unsigned char  on;             //����ǽ����Ч��־��0Ϊ��Ч��1Ϊ��Ч
  u16	dif[8];	
  u8 name;	//QZ ADD;
  bool flag;
  u8 max_dis;
  u8 dis;

  unsigned short bias;
  u8 check_time;			//qz add 20180504
  u8  error_check_step;
  u32 error_check_time;
}WALL_DATA;

typedef enum
{
	RECE_NONE=0,
	RECE_LEFT,
	RECE_MID,
	RECE_RIGHT,
	RECE_BACK,
}RECE_IR;

typedef struct             //ǽ��ؼ�����ݽṹ	//�������ݽṹ?  QZ
{
	unsigned 		char  	(*read_io)();
	unsigned 		char   	data;	   //������յ�����
	unsigned 		int  		time;	   //���տ�ʼ��ʱ��
	unsigned 		char   	tempdata; //��ʱ���յ�����
	unsigned 		char   	tempbit ;	   //�����ĵ�ǰλ��ֵ
	unsigned 		char   	bitnumber;	   //�������λ
	unsigned 		char   	start;  //�����ź����ݿ�ʼ��־
	volatile    unsigned int   	sign;	   //������յ��ı�־
	volatile    unsigned int  	effect_time;  //������Чʱ��
	volatile    unsigned int  	effect;       //������Ч

	//hzh
	//��ʵsign��û���õ�
	volatile	  unsigned int	  effectTop;
	volatile	unsigned int	  signTop;
	volatile	unsigned int	  effect_timeTop;

	volatile	unsigned int	effectSignal;//����ǰ����ͷ
	volatile	unsigned int	  effect_timeSignal;
	unsigned	char			Cnt_fallRise;

	volatile	  unsigned int	  effectLeft;
	volatile	  unsigned int	  effect_timeLeft;


	volatile	  unsigned int	  effectRight;
	volatile	  unsigned int	  effect_timeRight;

	volatile	  unsigned int	  effectMid;
	volatile	  unsigned int	  effect_timeMid;

	volatile	  unsigned int	  effectTopReal;
	volatile	  unsigned int	  effect_timeTopReal;

	volatile	  unsigned int	  effectNear;
	volatile	  unsigned int	  effect_timeNear;

	volatile	  unsigned int    effectMidLeft;
	volatile    unsigned int    effect_timeMidLeft;

	volatile    unsigned int    effectMidRight;
	volatile    unsigned int    effect_timeMidRight;

	u16 flag;

	RECE_IR	rece_ir;		//qz add 20180817

	u8 rece_cnt;
}INFR_DATA;
typedef struct             //�����ٶȵ����ݽṹ
{ 
  int  ek[3];       //���������ϵͳ�ٶ�ƫ��
  float	f_ek[3];
  volatile    unsigned int   length;	   //�������ߵ�������
//  unsigned int	 length;
 unsigned int speed_length;//qz add 20180203
 unsigned int last_speed_length;//qz add 20180203
 unsigned int real_speed;  //qz add 20180203
  volatile    unsigned int   dq_time;       //���ӵ�ǰʱ�䣬���½��ص�ʱ��


	int all_length_Bufer[4];
	volatile 		int all_length;
//  volatile    unsigned short   dq_captime;    //���Ӳ���Ķ�ʱ�����������½��ص�ʱ��    
//  unsigned int falling_time;             //��ǰ�⵽���½���ʱ��
//  unsigned short falling_captime;          //��ǰ�⵽���½���ʱ��
  unsigned int rising_time;              //��ǰ�⵽��������ʱ��
//  unsigned short rising_captime;           //��ǰ�⵽��������ʱ��
//  unsigned int rising_dqtime;            //ȷ���⵽��������ʱ��
//  unsigned short rising_dqcaptime;         //ȷ���⵽��������ʱ��
  unsigned int  old_time;               //��������ǰ��ʱ��
//  unsigned short  old_captime;
  unsigned char  state;       //����״̬    BAD;���ֲ���   GOOD��������
  unsigned int  odds;        //����ͣ��ת����ʱ�����

  u8 check_step;
  int stop_buf[10];
  int stop_spd;
  int cal_length;
}RING;

typedef struct	  //���ֵ��ٶ���Ϣ
{
unsigned char master;     //�����ֱ�־   ��1Ϊ�����֣�0Ϊ�Ӷ���
unsigned char old_time;  //���ٵ���ʼʱ��
unsigned char ori   ;    //���ֵ�ǰ����  
//unsigned char real_ori;	
//unsigned short pwm   ;    // ���ֵ�pwmֵ	//qz mask
int pwm;		//qz add
int rap  ;     //���ֵ��ٶȣ���ʾΪÿ���������
int rap_run;
unsigned int length  ;     //����Ӧ���߹��ľ���
unsigned int all_pulse;
unsigned int  time   ;     // �����ٶȵ�ʱ��
unsigned int  pulse  ;     // �����ٶ��õ�������
int real_speed;		//����ʵʱ�ٶ�
unsigned char  sign   ;     //��־	   0:����ֹͣ   1��������������ʽΪ����ͣ��     2���׶ε���    3���������������㲻ͣ
	
	
uint8_t  			State;//0x00:  0x05:�뾶ת��  0x10:����  0x20:����ǰ��  0x30:����  0x60:  0x70:  0x80
//float					AccDec;
//float			Desired_Speed;
//float			Max_Speed;

short			AccDec;
short			Desired_Speed;
u16				Max_Speed;

unsigned int 	ACC_Position;	
unsigned int 	DEC_Position;	
	
bool fail;	
	
}RAP	 ;

//QZ ADD
typedef struct
{
	unsigned int length_w;			//���߿��
	unsigned int length_w_rest;
	unsigned int length_h;			//���߸߶�
	unsigned int length_h_rest;
	unsigned char time;				//��������
	unsigned int angle;				//�����Ƕ�
	unsigned char bump_time;		//��ײ����
	unsigned char turn_step;		//ת�䲽��	
	unsigned char status;			//0:��ʾ����ֱ��,1:��ʾ����ת��
	bool turn_dir;			//��Ҫת�䷽��,0:��ʾ��Ҫ��ת,1:��ʾ��Ҫ��ת
	u8 first_time_value;
	u32 temp_length;
	u32 real_length;
}Gong;

typedef struct
{
	bool island;			//�µ�
	bool check_flag;
	
	u8 check_step;
	s8 result;
	s8 island_gridx;
	s8 island_gridy;
	
	short check_angle;
	int acc_angle;
}GYRO_BIOS;
//QZ ADD END

typedef struct
{
	short pwm_l; //����PWM
	short pwm_r;	//����PWM
	int length_l;		//������Ҫ���ߵ������
	int length_r;		//������Ҫ���ߵ������
	unsigned int speed;
	unsigned char mode;		//���������ģʽ   1�ٶ���ʻ  2 �����ʻ
	unsigned char sign;
}MOTO_TASK;

typedef  struct
{
bool low2dock;				 //��ص����͵Ļس����󣬷���һ��
bool reinit;				//���³�ʼ����־,true:��Ҫ���³�ʼ��
unsigned int temp   ;	     //����¶� 
unsigned int voltage;		 //��ص�ѹ   
unsigned int current;      //������
unsigned char  charge_dc ;	     //�ⲿDC�������ź�
unsigned char  charge_seat ;		 //�ⲿ����������ź�
 short pwm;         //���PWM����
unsigned char  err;         //��ش���  0 û�д���1û��NTC��2 NTC��·��3 û�е�أ� 4 ��ض�·
unsigned char step;         //��粽��  0 С����  1 �����  2ά�ֵ���
unsigned int time;        //��翪ʼʱ��
u32 step_time;			  //qz add
} POWER;

typedef  struct
{
unsigned char data;          //�����źŵ�����		  0��ʾû�ж�����1��ʾ���Ѿ�����
unsigned int time;         //��ȡ�����źŵ�ʱ��
}  SWITCH;

typedef  struct
{
unsigned int current;	       //��ǰ�����ĵ���ֵ
unsigned int old_current;    //ǰһ�εĲ�������
unsigned int sum_current;    //�����ĺ�
unsigned int linshi_current; //��ʱ��ŵĲ�������ֵ
unsigned int temp;	       //�����ı�׼ֵ
unsigned int time;		   //����ʱ��
unsigned int typo;		   //�����������
unsigned char  sign;		   //������־         0��ʾδ���꣬1��ʾ����
unsigned char sign_l;
}CURRENT;

typedef  struct
{
unsigned int * music;	    //��Ҫ���ŵ����ֶ�
unsigned int temp;	    //�����ı�׼ֵ
unsigned int time;		//���ֲ��ŵ�ʱ��
unsigned int typo;		//���ֲ��ŵ���ʼλ��
volatile  unsigned char  sign;		//�źű�־         0��ʾ������1��ʾ��
}mySOUND;

typedef struct	    //�����������
{
unsigned int x;		    //������x����
unsigned int y;			//������y����
unsigned int angle;		//�����ĽǶ�
}POSITION;


typedef struct		//����ϵͳ�Ĺ���״̬
{
	bool 		speed_up;
	bool 		factory;				//����״̬qz add 20181024
	bool 		factory_burnning;		//�����ܻ�״̬qz add 20181024
	bool 		bump_flag; 				   //qz add for bleamn

	u8 			factory_tst_item;		//qz add 20181107
	u8 		 	mode ;	   //����������ģʽ  0����ֹģʽ��1��ɨ��ģʽ��2���Զ��س�ģʽ��3��ң��ģʽ	4:����ģʽ
	u8  		sub_mode;	//��ģʽ��0x01:cease,0xfe:quit_charge,0xff��err
	u8  		step;		//·��ִ�еĲ���
	u8 			step_abn;    //�쳣ʱ�Ĳ���
	u8			step_bp;	//��ײʱ�Ĳ����־
	u8			step_mk;	//��������ɨ����
	u8	 		last_bump;
	u8 			Info_Abort;			//qz add��ֹ����ָ�������ײ���˻��˳������ʱ,20180919����Ϊֻ�ٶ�����
	u8 			All_Info_Abort;		//qz add 20180919����������ָ��
	u8 			last_mode;			//qz add��һ�ε�ģʽ
	u8 			last_sub_mode;
	u8 			test_step;	
	u8 			sweep_method;
	
	u8 			status;		//qz add 20180422��ɨ�ػ�״̬��SLAM�·�����ʼ��ɨ����1,ֹͣ��ɨ����0�������쳣״̬��Ҳ����0��
                //���̻������status���ж�ԤԼʱ�䵽���Ժ��費��Ҫ��ԤԼ��
                /*status,0:ֹͣ��,�������ڷǹ���״̬,
                         1:�滮��ɨ
                         2:�رߴ�ɨ
                         3:�����ɨ
                         4:�ص��ɨ

                         5:ԤԼ��ɨ
                         6:�س�*/
	u32 		action;		//����   0:ֹͣ  1:ԭ����ת  2ԭ����ת 3ǰ��   4����   5��ת1  6����ת2  ������18��ת14	��19��������	
	u32 		time;       //��������ʼʱ��
	u32 		init_mode_time;	//qz add 20180814
	u32  		abnormity;  //�쳣����  0��ʾû���쳣 //QZ:16:��ɨ��������,19:���ֲ���,20:���ֲ���,22:�������,23:�������,31:���ֵ���,32:���ֵ���
	u32 		bump ;      //��ײ�ı�־ //QZ:1:��ؼ�,2:���еؼ�,3:���еؼ�,4:�ҵؼ�,5:����ײ,6:����ײ,7:��ǽ�쿿��ǽ,
 									//8:����ǽ�쿿��ǽ,9:��ǽ��,10:�����������,14:��ǽ��,15:����ǽ��,90:˫��ײ,

	u32 		last_outbump;
	u32 		bump_time;
	u32 		abn_time;			//qz add 20181011

}MODE;

typedef struct
{
  unsigned char step;	  //����		//0 û������ִ�еĲ��裬1������ִ��ĳ����
  unsigned char step_tmp;
  unsigned char sign;	  //��ʼ��־   0��ʾû�п�ʼ��1��ʾ�Ѿ���ʼ
  unsigned int time ;   //������ʼ��ʱ��
  unsigned int length ;  //���Ȼ�����תȦ��
  u8 action_flag;		//1:ǰ��,0:ֹͣ,2:ת�������		//qz add 20180123
  u32 skid_time;		//�򻬼�⿪ʼʱ��   qz add 20180123
}ACTION;

typedef	 union		 //��ʾ����
{
 unsigned int data;
 unsigned char  dis_data[4];
}DIS_DATA;

typedef struct
{
  unsigned char sign;
  unsigned char step;
volatile  unsigned char start;
  unsigned int time;
} DUST;

typedef struct					//�������Ϣ�ṹ��
{
	bool clean;					//�������Ƿ���ɨ��־
	bool wall;					//�������Ƿ�ǽ�ڣ���ײ��־
	short clr_xpos;
	short clr_ypos;
	u32  cnt;
}COORDINATE;

typedef struct 					//��ɨ�ṹ��
{
	bool 	leakon;				//������ǰ��NORAML��ɨ����LEAK��ɨ��false:������ɨ��true:©ɨ��ɨ
	bool 	area_ok;				//��ǰ������ɨ����Ƿ���ɣ������ɨ�����ϣ��Ժ��ټ��©ɨ
	bool 	ymax_ok;						//YMAX����������ɱ�־
	bool 	ymin_ok;						//YMIN����������ɱ�־
	bool 	xmax_ok;						//XMAX����������ɱ�־
	bool 	xmin_ok;						//XMIN����������ɱ�־
	bool 	repeat_sweep;			//�ظ�ɨ��־��true:�ظ�ɨ��false������Ҫ���ر߳��ֿտ�����ʱ�ظ�ɨ
	bool 	pathpoint_ok;
	bool	start_seat;
	bool	force_dock;
	
	u8 		back_sweep;				//��ɨ��־��true:��ɨ,false:����ɨ
	u8 		leftright;				//�����ر߱�־��0�����رߣ�1�����ر�
	u8 		y_acc;					//������ɨʱY����������0��������1��������2��˫����
	u8 		x_acc;					//������ɨʱX����������0��������1��������2��˫����
	s8 		y_dir;					//Y����ɨʱ����������(0:������,1:������),��Ϊy_acc��0,1,2����ֵ��2����˫������ɨ���޷�����y_acc��ʾĿǰ������ɨ�ķ���
	u8  	continue_checkstep;		//�ر�ʱ����Ƿ��ƹ��ϰ��Ĳ���
	u8 		sweep_time;				//�����ɨ�����������¿�������ʱ�ı�����ӣ���#1��ʼ
	u8 		area_num;				//��ǰ������
	u8 		exit_area_num;			//�˳�������
	s8		first_leak_y;

	short 	xpos_ybs_start;		//����ֱ�������л�Ϊ�ر�ʱ��ʼ��x����
	short 	xpos_start;			//��ʼ��x����
	short 	ypos_ybs_start;		//����ֱ�������л�Ϊ�ر�ʱ��ʼ��y����
	short 	ypos_start;			//��ʼ��y����
	short 	tgt_yaw;				//ֱ����ɨʱĿ��Ƕ�
	short 	anti_tgt_yaw;			//ֱ����ɨʱĿ�귴�Ƕ�
	short 	xpos_abort;			//�����(���ɨ)ʱ��x����
	short 	ypos_abort;			//�����(���ɨ)ʱ��y����
	short 	tgtyaw_abort;
	short 	anti_tgtyaw_abort;
	short 	xpos_start_area;		//�����ɨ��4X4��X��ʼ����
	short 	ypos_start_area;		//�����ɨ��4X4��Y��ʼ����
	short 	xpos_max_area;		//�����ɨ��4X4���ܵ�������X����
	short 	xpos_min_area;		//�����ɨ��4X4���ܵ������СX����
	short 	ypos_max_area;		//�����ɨ��4X4���ܵ�������Y����
	short 	ypos_min_area;		//�����ɨ��4X4���ܵ������СY����
	
	u32 	worktime;			//�����ڹ���ʱ��
	u32 	worktime_max;		//�������������ʱ��	
}MOTION;

typedef struct					//���������ṹ��
{
	volatile s8 x;						//��ǰX������
	volatile s8 y;						//��ǰY������
	s8 x_abort;
	s8 y_abort;
	s8 y_back_last;				//������ɨʱ��Y����
	s8 x_area_max;				//������X�ܴﵽ����������
	s8 x_area_min;				//������X�ܴﵽ����С�����
	s8 y_area_max;				//������Y�ܴﵽ����������
	s8 y_area_min;				//������Y�ܴﵽ����С�����
	s8 x_straight_start;		//ÿ�ο�ʼֱ����ɨʱ��X����
	s8 y_straight_start;		//ÿ�ο�ʼֱ����ɨʱ��Y����
	s8 x_area_start;			//�����ɨʱ��ʼX����	
	s8 y_area_start;			//�����ɨʱ��ʼY����
	s8 x_ybs_start;
	s8 y_ybs_start;
	s8 y_start;
	s8 x_start;
}GRID;

typedef struct					//��ɨ�����Ϣ�ṹ��
{
	bool use_pathpoint;			//ʹ��PATHPOINT���з�ʽ
	bool yacc_anti;				//��motion1.interval_pos��ǰ���£���ǰ©ɨʱ��Ҫ��ɨ�ļ������,false:������ɨ����true:��ɨ����
	u8 ydir;				//��һ©ɨ�����Y����ɨ����	0:Y�Ḻ������ɨ��1:Y����������ɨ
	u8 next_yacc;				//������ɨ�����Y����������
	u8 next_xacc;				//������ɨ�����X����������
	u8 next_action;				//��һ�εĶ�����0:������ɨ��1:©ɨ��ɨ��2:ȥ��������3:������ڵ�
	short next_tgtyaw;			//������ɨ�������ʼ�Ƕ�
	
	s8 max_cleanx1;				//����X�᷽�������ɨ����
	s8 min_cleanx1;				//����X�᷽����С��ɨ����
	s8 max_cleanx2;				//���ڼ���X�᷽�������ɨ����
	s8 min_cleanx2;				//���ڼ���X�᷽����С��ɨ����
	s8 y1;						//����Y������
	s8 y2;						//���ڼ���Y������
	s8 new_x1;					//�ɽ��벹ɨ��X����
	s8 new_y1;					//�ɽ��벹ɨ��Y����
	s8 new_x2;					//��ɨ��X����
	s8 new_y2;					//��ɨ��Y����

	u8 ybs_dir;					//����Ԥ���ж����ر߷���0:û��Ԥ���ж�����1:���رߣ�2:���ر�
	s8 backup_grid;				//���Խ���ı��ݵ�
	u8 new_area_dir;			//��һ����next_area�ķ���ʹ��ymax/ymin/xmax/xmin����ʾ����������ڵ�ǰ����ķ���	
} CHECK_POINT;

typedef struct 					//����������Ľṹ��
{
	s8 gridx;					//��ǰ��X����
	s8 gridy;					//��ǰ��Y����
	u8 data;					//����
	u8 area_dir;				//��������ķ�λ���������ϣ����µȣ�
}POINT_GRID;

typedef struct					//����·�����������յ�����Ľṹ��
{
	POINT_GRID start_grid;		//�������
	POINT_GRID tgt_grid;		//�յ�����
}PATH_STARTEND;

struct PATH_POINT				//����·��������ÿ�����ߵ���Ϣ
{
	POINT_GRID path_grid;		//���ߵ�������Ϣ
	struct PATH_POINT *last_point;	//��һ�����ߵ�ָ��
	struct PATH_POINT *next_point;	//��һ�����ߵ�ָ��
};


typedef struct
{
	bool first_rst;			//qz add 20181101:�����һ�μ�⵽�ߵ������쳣�����Ը�λ1��
	u8 check_step;

	u8 roll_check_step;		//qz add 20180927
	u32 roll_check_time;	//qz add 20180927

	u8 pitch_check_step;
	u32 pitch_check_time;

	u8 pitchroll_check_step;
	u32 pitchroll_check_time;

	u32 start_check_time;
	u32 count_times;
	volatile short x_pos;		//qz modify 20190307 :add volatile
	volatile short y_pos;
	volatile short yaw;
	short tgt_yaw;
	volatile short pitch;
	volatile short roll;
	short first_roll;
	short first_pitch;
	volatile short x_acc;
	volatile short y_acc;
	volatile short z_acc;
	int g_ek[3];		//qz add 20180111
	u8  straight;		//qz add 20180111  0:���κ�����,1:��Ҫ��ֱ����ʻ�Ĳ���,2:��Ҫ����ת�Ƕȵ��ж�
}GYRO_DATA;

extern bool log_show;


typedef struct
{
	u8 		Preen_Week_Num;
	u8 		Preen_Hour;
	u8 		Preen_Min;
	u8		Cycle;
	u8	 	Flag;
	u8 		Hour_done;
}PREEN_DATA;

typedef struct
{
	u16 	Year;
	u16 	Month;
	u16 	Day;
	u16 	Hour;
	u16 	Min;
	u16 	Sec;
	u16 	Week_Day;	//���ڼ�������Ϊ0������Ϊ6,�Դ�����
}UNIX_TIME;

typedef struct
{
	bool 	ctrl_stop;
	u8 		user_mode;
	bool 	ctrl_change;
	bool 	ctrl_ack_flag;
	u8		ctrl_sta;
	bool 	ctrl_dock_req;
	s8		app_bat;
	s8		app_last_bat;
}APP_DATA;

typedef struct
{
	u8 gridx;
	u8 gridy;
	u8 data;
}WIFI_DATA;


typedef struct
{
	bool 	no_msg;				//����Ҫ�򵼺��巢����Ϣ��־,false:��Ҫ�ϴ�,true:����Ҫ�ϴ�
	bool 	l_bump_flag;
	bool 	r_bump_flag;
	bool 	m_bump_flag;
	bool 	slam_ok;			//������������ɣ������ǣ�С��ȶ������������Թ���
	bool  	tick_flag;			//�Լ���ȡ���ݡ�SLAM��������ͨ�ű�־��0��û�н��յ�������1�����յ�����
	bool	sleep_flag;			//���߱�־���ɵ���֪ͨSLAM
	bool 	wifi_ok;			//�������ӳɹ���־��
	bool 	ir_flag;
	bool	preen_off;			//�ɵ���ң���������ȡ��ԤԼ��true��Ϊ����ȡ��
	bool	preen_on;			//�ɵ���ң�������������ԤԼ��true��Ϊ��������
	bool	restart_flag;		//����������־������/������Ϊfalse,10s��Ϊtrue;
	u8 		skid_flag;			//�򻬱�־
	u8 		ack;				//�ϴ����ݡ��յ�ĳЩSLAM�������Ҫ����ACK
	u8 		dust_level;			//�ϴ����ݡ��յ�����������ź�ʱ�ϴ�
	u8  	tick_check_step;	//������ⲽ��
	u8 		dipan_req;
	u8		dipan_req_pre;
	s8 		bat;				//��ذٷֱ�
	s8  	last_bat;
	u8 		preen_hour;			//�ɵ���ң���������ԤԼСʱ
	u8		preen_min;			//�ɵ���ң���������ԤԼ����
	u32 	skid_report_time; 	//���ϱ���־ʱ�䣬�����⵽�򻬣�1s֮���ϱ��򻬱�־��1s������
	u32 	tick_time;			//�Լ���ȡ���ݡ�����ͨ��ʱ�䣬����һ��ʱ��û���յ����ᱨ��
}SLAM_DATA;

typedef struct
{
	bool    fail;
	bool 	check_flag;		//������ת�򻬼���־
	bool 	time_flag;		//40ms��ʱ��־
	u8		direction;		//��ת����0����ת��1����ת
	u8  	check_time;		//���������������
	u8 		resuce_time;	//�Ծȴ���
	u32 	start_time;		//������ת�򻬼��ʱ��ʱ��
	u32 	skid_time;
	int 	l_lenght;		//
	int 	l_last_length;
	int		r_length;
	int 	r_last_length;
	int 	l_sum_length;
	int		r_sum_length;
	short 	start_angle;
	short	end_angle;
	float 	rot_angle;		//�����������õ�����ת�Ƕ���

	u32		l_speed;
	u32		l_last_speed;
	u32		r_speed;
	u32		r_last_speed;
	
}ROTATE_SKID;

typedef struct
{
	bool check_flag;
	bool time_flag;
	u8 	direction;
	u8  check_time;
	u8  check_step;
	u32 start_time;
	int l_length;
	int r_length;
	int l_last_length;
	int r_last_length;
	int l_sum_length;
	int r_sum_length;
	float rot_angle;	
}ROTATE_ANGLE;

typedef struct
{
	bool 	fail;
	bool 	check_flag;
	u8		check_step;
	u32 	check_start_time;
	u32 	length;
	
}FREE_SKID;

typedef struct
{
	bool 	use;
	bool 	flag;					//�����쳣�жϱ�־,true:��Ҫ�ж�,false:�����ж�
	u8	 	fail;					//�����־,true���������������꣬�޷���أ��ж��쳣��false���޴�������ڱ���
	u8 		step;					//���ڷ����ȡ�����ò���
	u8 		check_step;				//
	u8 		error_time;				//�������
	u8		disc_error_time;		//��ϵĴ������
	u32	 	length;
	u32		check_time;
	u32		last_length;
	u32		done_time;				//�����������ʱ��
	
}BRUSH_DATA;

typedef struct
{
	u32 	piv_current_sum;
	u32		piv_voltage_sum;
	u32		piv_current_count;
	u32 	piv_voltage_count;
	float	real_current;
	float 	real_voltage;
	float   ek[3];
	
}CHARGE_DATA;

typedef struct
{
	bool 	reinit;
	bool 	BatteryChargeForbid;
	u8 		bat_recal;
	u32		BatteryCapability;
	u32 	BatteryFDCap;
	u32		UsePowerTime;
	u32 	start_time;
}BATTERY;

typedef struct
{
	u8		oc_data;
}FACTORY_DATA;

typedef struct sweep_node
{
	short xpos;
	short ypos;
	u32 sweep_time;
	struct sweep_node *next;
}SWEEP_BUMP_NODE;

#endif





