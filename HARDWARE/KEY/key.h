#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "stm32f10x.h"
 


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define WK_UP PAin(0)	//PA0  WK_UP

//#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//��ȡ����0
//#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//��ȡ����1
//#define WK_UP   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//��ȡ����3(WK_UP) 

// 

//#define KEY0_PRES 	1	//KEY0����
//#define KEY1_PRES	  2	//KEY1����
//#define WKUP_PRES   3	//KEY_UP����(��WK_UP/KEY_UP)


//void KEY_Init(void);//IO��ʼ��
//u8 KEY_Scan(u8);  	//����ɨ�躯��	

//��
#define KEY_UP 		PAin(6)
//��
#define KEY_DOWN 	PAin(7)
//����
#define KEY_LEFT 	PAin(5)
//����
#define KEY_RIGHT 	PBin(0)


void KEY_Init(void);
u8 Get_KEY_Value(void);

#endif
