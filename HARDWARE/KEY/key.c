#include "key.h"
#include "sys.h" 
#include "delay.h"
			
uint16_t cont_key_count=0;

//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTE时钟

//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_8;//KEY0-KEY1
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA7,8

//	//初始化 WK_UP-->GPIOA.0	  下拉输入
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.9
	
	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTE时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//PA5:设置    PA6:加    PA7:减
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA7,8

    //PB0:返回
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.9

}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY_UP!!
//u8 KEY_Scan(u8 mode)
//{	 
//	static u8 key_up=1;//按键按松开标志
//	if(mode)key_up=1;  //支持连按		  
//	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
//	{
//		delay_ms(10);//去抖动 
//		key_up=0;
//		if(KEY0==0)return KEY0_PRES;
//		else if(KEY1==0)return KEY1_PRES;
//		else if(WK_UP==1)return WKUP_PRES;
//	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
// 	return 0;// 无按键按下
//}

//	KEY_NEXT=2,
//	KEY_BACK,
//	KEY_PREVIOUS,
//	KEY_ENTER,
//	KEY_ADD,
//	KEY_SUB,
	
/*
函数功能：获取键值
参数：无
返回值：返回键值，按键按下(2,3,4,5),无按键按下(0)
*/
u8 Get_KEY_Value(void)
{
	static u8 Flag=0;
	if((!KEY_UP||!KEY_DOWN||!KEY_LEFT||!KEY_RIGHT)&&(Flag==0))
	{
		Flag=1;
		delay_ms(15);//按键消抖
//		if(KEY_UP==0)return 2; //加
//		if(KEY_LEFT==0)return 3; //设置
//		if(KEY_DOWN==0)return 4; //减
//		if(KEY_RIGHT==0)return 5; // 返回
		
		if(KEY_UP==0)return 4; //加
		if(KEY_LEFT==0)return 5; //设置
		if(KEY_DOWN==0)return 2; //减
		if(KEY_RIGHT==0)return 3; // 返回
		
	}

	
	if((KEY_UP&&KEY_DOWN&&KEY_LEFT&&KEY_RIGHT)&&(Flag==1)){Flag=0;cont_key_count=0;}//检测是否松开
	
	
	if((!KEY_UP||!KEY_DOWN||!KEY_LEFT||!KEY_RIGHT)&&(Flag==1))
	{
		//printf("Get_KEY_Value:%d\n",cont_key_count);
		//在此处的快加快减功能，每秒加减的数不超过每秒屏幕刷新次数，因为加减是在GUI_Refresh函数中进行的，如果想更快加减，需要用定时器实现
		cont_key_count++;
		if(cont_key_count>8){
			if(KEY_UP==0)return 4; //快加
			if(KEY_DOWN==0)return 2; //快减
		}
	}
	return 0;
}

u8 Test_Get_KEY_Value(void)
{
	static u8 Flag=0;
	if((!KEY_UP||!KEY_DOWN||!KEY_LEFT||!KEY_RIGHT)&&Flag==0)
	{
		Flag=1;
		delay_ms(15);//按键消抖
//		if(KEY_UP==0)return 2; //加
//		if(KEY_LEFT==0)return 3; //设置
//		if(KEY_DOWN==0)return 4; //减
//		if(KEY_RIGHT==0)return 5; // 返回
		
		if(KEY_UP==0)return 4; //加
		if(KEY_LEFT==0)return 5; //设置
		if(KEY_DOWN==0)return 2; //减
		if(KEY_RIGHT==0)return 3; // 返回
		if(KEY_LEFT==0 && KEY_RIGHT==0)return 8;
		
	}

	
	if((KEY_UP&&KEY_DOWN&&KEY_LEFT&&KEY_RIGHT)&&Flag==1){Flag=0;cont_key_count=0;}//检测是否松开
	
	
	return 0;
}

