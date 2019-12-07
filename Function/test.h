#ifndef SELF_TEST_H
#define	SELF_TEST_H
extern bool test_100ms_flag,test_500ms_flag,test_1s_flag;

#define	TEST_ERATH_TIME		20000
#define	TEST_WALL_TIME		20000
#define	TEST_CURR_TIME		50000

#define	TEST_EARTH_LOW_VALUE	0
#define	TEST_EARTH_UP_VALUE		4000
#define	TEST_WALL_LOW_VALUE		0
#define	TEST_WALL_UP_VALUE		4000

#define	TEST_VOLCAL_OK			(1<<0)
#define	TEST_BUMP_OK			(1<<1)
#define	TEST_TEMP_OK			(1<<2)
#define	TEST_GRYO_OK			(1<<3)
#define	TEST_DC_OK				(1<<4)
#define	TEST_SETA_OK			(1<<5)
#define	TEST_LHW_OK				(1<<6)
#define	TEST_RHW_OK				(1<<7)
#define TEST_LMHW_OK			(1<<8)
#define	TEST_RMHW_OK			(1<<9)
#define	TEST_LBHW_OK			(1<<10)
#define	TEST_RBHW_OK			(1<<11)
#define	TEST_VOL_OK				(1<<12)
#define	TEST_EL_OK				(1<<13)
#define	TEST_EM_OK				(1<<14)
#define	TEST_ER_OK				(1<<15)
#define	TEST_WL_OK				(1<<16)
#define	TEST_WLM_OK				(1<<17)
#define	TEST_WM_OK				(1<<18)
#define	TEST_WRM_OK				(1<<19)
#define	TEST_WR_OK				(1<<20)
#define	TEST_SBCURR_OK			(1<<21)
#define	TEST_MBCURR_OK			(1<<22)
#define	TEST_FANCURR_OK			(1<<23)
#define	TEST_LRING_OK			(1<<24)
#define	TEST_RRING_OK			(1<<25)

void Init_Test(u8 test_mode);
void Do_SelfTest(void);
u8 Parse_SelfTest_Stop(void);
void ChargeControl_Test(void);


#endif
