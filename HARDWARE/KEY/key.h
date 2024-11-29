#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "stm32f10x.h"
 


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define WK_UP PAin(0)	//PA0  WK_UP

//#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//读取按键0
//#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//读取按键1
//#define WK_UP   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//读取按键3(WK_UP) 

// 

//#define KEY0_PRES 	1	//KEY0按下
//#define KEY1_PRES	  2	//KEY1按下
//#define WKUP_PRES   3	//KEY_UP按下(即WK_UP/KEY_UP)


//void KEY_Init(void);//IO初始化
//u8 KEY_Scan(u8);  	//按键扫描函数	

//加
#define KEY_UP 		PAin(6)
//减
#define KEY_DOWN 	PAin(7)
//设置
#define KEY_LEFT 	PAin(5)
//返回
#define KEY_RIGHT 	PBin(0)


void KEY_Init(void);
u8 Get_KEY_Value(void);

#endif
