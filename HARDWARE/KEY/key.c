#include "key.h"
#include "sys.h" 
#include "delay.h"
			
uint16_t cont_key_count=0;

//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTA,PORTEʱ��

//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_8;//KEY0-KEY1
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA7,8

//	//��ʼ�� WK_UP-->GPIOA.0	  ��������
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.9
	
	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTEʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//PA5:����    PA6:��    PA7:��
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA7,8

    //PB0:����
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.9

}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY_UP!!
//u8 KEY_Scan(u8 mode)
//{	 
//	static u8 key_up=1;//�������ɿ���־
//	if(mode)key_up=1;  //֧������		  
//	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
//	{
//		delay_ms(10);//ȥ���� 
//		key_up=0;
//		if(KEY0==0)return KEY0_PRES;
//		else if(KEY1==0)return KEY1_PRES;
//		else if(WK_UP==1)return WKUP_PRES;
//	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
// 	return 0;// �ް�������
//}

//	KEY_NEXT=2,
//	KEY_BACK,
//	KEY_PREVIOUS,
//	KEY_ENTER,
//	KEY_ADD,
//	KEY_SUB,
	
/*
�������ܣ���ȡ��ֵ
��������
����ֵ�����ؼ�ֵ����������(2,3,4,5),�ް�������(0)
*/
u8 Get_KEY_Value(void)
{
	static u8 Flag=0;
	if((!KEY_UP||!KEY_DOWN||!KEY_LEFT||!KEY_RIGHT)&&(Flag==0))
	{
		Flag=1;
		delay_ms(15);//��������
//		if(KEY_UP==0)return 2; //��
//		if(KEY_LEFT==0)return 3; //����
//		if(KEY_DOWN==0)return 4; //��
//		if(KEY_RIGHT==0)return 5; // ����
		
		if(KEY_UP==0)return 4; //��
		if(KEY_LEFT==0)return 5; //����
		if(KEY_DOWN==0)return 2; //��
		if(KEY_RIGHT==0)return 3; // ����
		
	}

	
	if((KEY_UP&&KEY_DOWN&&KEY_LEFT&&KEY_RIGHT)&&(Flag==1)){Flag=0;cont_key_count=0;}//����Ƿ��ɿ�
	
	
	if((!KEY_UP||!KEY_DOWN||!KEY_LEFT||!KEY_RIGHT)&&(Flag==1))
	{
		//printf("Get_KEY_Value:%d\n",cont_key_count);
		//�ڴ˴��Ŀ�ӿ�����ܣ�ÿ��Ӽ�����������ÿ����Ļˢ�´�������Ϊ�Ӽ�����GUI_Refresh�����н��еģ���������Ӽ�����Ҫ�ö�ʱ��ʵ��
		cont_key_count++;
		if(cont_key_count>8){
			if(KEY_UP==0)return 4; //���
			if(KEY_DOWN==0)return 2; //���
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
		delay_ms(15);//��������
//		if(KEY_UP==0)return 2; //��
//		if(KEY_LEFT==0)return 3; //����
//		if(KEY_DOWN==0)return 4; //��
//		if(KEY_RIGHT==0)return 5; // ����
		
		if(KEY_UP==0)return 4; //��
		if(KEY_LEFT==0)return 5; //����
		if(KEY_DOWN==0)return 2; //��
		if(KEY_RIGHT==0)return 3; // ����
		if(KEY_LEFT==0 && KEY_RIGHT==0)return 8;
		
	}

	
	if((KEY_UP&&KEY_DOWN&&KEY_LEFT&&KEY_RIGHT)&&Flag==1){Flag=0;cont_key_count=0;}//����Ƿ��ɿ�
	
	
	return 0;
}

