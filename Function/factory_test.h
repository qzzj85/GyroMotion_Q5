#ifndef FACTORY_TEST_H
#define FACTORY_TEST_H

#define 	FAC_TST_RING		1
#define		FAC_TST_LRINGOC		2
#define		FAC_TST_RRINGOC		3
#define 	FAC_TST_MBON		4
#define		FAC_TST_MBOFF		5
#define		FAC_TST_MBOC		6
#define		FAC_TST_SBON		7
#define		FAC_TST_SBOFF		8
#define		FAC_TST_SBOC		9
#define		FAC_TST_FANON		10
#define		FAC_TST_FANOFF		11
#define		FAC_TST_FANOC		12
#define		FAC_TST_LEDON		13
#define		FAC_TST_LEDOFF		14
#define		FAC_TST_VOICE		15
#define		FAC_TST_UVON		16
#define		FAC_TST_UVOFF		17
#define		FAC_TST_CHARGE		18
#define		FAC_TST_CHARGEOFF	19
#define		FAC_TST_LSBOC		20
#define		FAC_TST_RSBOC		21
#define		FAC_TST_BURNNING	22

extern FACTORY_DATA		factory_data;
void 		Init_FactoryTest(void);
void 		Do_FactoryTest(void);
#endif
