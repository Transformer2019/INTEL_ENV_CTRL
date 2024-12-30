#include "timer.h"
#include "usart1.h"
#include "gui.h"
#include "ntc.h"
#include "delay.h"
#include "mb_hook.h"
   	  
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

//��ʱ�ж������Ƿ��������Լ��Ƿ�ע��
volatile uint16_t TIM3_Counter_10s=0;
volatile u8 TIM3_flag=0;

volatile uint16_t TIM4_Counter_10s=0;
volatile u8 TIM4_flag=0;

volatile uint16_t TIM5_Counter_10s=0;
volatile u8 TIM5_flag=0;

volatile u8 TIM1_Counter=0;
volatile u8 TIM1_flag=0;

volatile uint8_t Heartbeat_Counter_1s=0;
volatile u8 Heartbeat_flag=0;


//�¶�����
//volatile float temp1_list[11]={0};
//volatile float temp2_list[11]={0};
//volatile float temp3_list[11]={0};
//volatile float temp_avg_list[11]={0};


volatile float temperature1 = 99.0;
volatile float temperature2 = 99.0;
volatile float temperature3 = 99.0;
volatile float average_temp = 99.0;

void TIM1_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 499; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������2000Ϊ200ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIMx����
							 
}


void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 9999; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}

void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 9999; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������1000Ϊ100ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM2�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
							 
}

//��ʱ��2�жϷ������

float get_temperature(Temp_Choose_flag flag) {
	
    switch(flag) {
        case temperature1_flag: return temperature1;
        case temperature2_flag: return temperature2;
        case temperature3_flag: return temperature3;
        case average_temp_flag: return average_temp;
        default: return -1.0f;  // ����������һ�����Ϸ����¶�ֵ
    }
}


//��ֵ�˲��㷨
float middleValueFilter(float value_buf[], int N)
{
    float temp_buf[N],temp;
	float avg_temp = 0.0;
    int i,j,k;
    for(i = 0; i < N; ++i)
    {
        temp_buf[i] = value_buf[i];
    }
    for(j = 0; j < N-1; ++j)
    {
        for(k = 0; k < N-j-1; ++k)
        {
            //��С��������ð�ݷ�����
            if(temp_buf[k] > temp_buf[k+1])
            {
                temp = temp_buf[k];
                temp_buf[k] = temp_buf[k+1];
                temp_buf[k+1] = temp;
            }
        }
    }
	
	for(j = 1; j < N-1; ++j){
		avg_temp += temp_buf[j];
	}
	return avg_temp/8;
    //return temp_buf[(N-1)/2];
}

void list_change(float value_buff[], u8 channel, int N){
	int j;
	for(j = 0 ; j < N-1; ++j){
		value_buff[j] = value_buff[j+1];
		//printf("value_buff[%d]: %.2f degrees Celsius\r\n", j, value_buff[j]);
	}
	value_buff[j]=Read_Temperature(channel);
	//printf("value_buff[%d]: %.2f degrees Celsius\r\n", j, value_buff[j]);
} 


void list_change_end(float value_buff[], float temp_end, int N){
	int j;
	for(j = 0 ; j < N-1; ++j){
		value_buff[j] = value_buff[j+1];
		//printf("value_buff[%d]: %.2f degrees Celsius\r\n", j, value_buff[j]);
	}
	value_buff[j]=temp_end;

}

void TIM2_IRQHandler(void)   //TIM2�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{   
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ
		//���Ͳɼ�Ƶ��
		if(TIM4_Counter_10s>60)
		{
		  TIM4_flag=1;
		}
		TIM4_Counter_10s++;
		
		//�����ò���д��flashƵ��
		if(TIM5_Counter_10s>6)
		{
		  TIM5_flag=1;
		}
		TIM5_Counter_10s++;
		
		//��������Ƶ��
		if(Heartbeat_Counter_1s>10)//һ���һ���жϣ��ۼ�8����־��1�����жϴ���������Ϣ
		{
		  Heartbeat_flag=1;
		}
		Heartbeat_Counter_1s++;

		//printf("send_NH3:%d\n",send_NH3);
		
		uint8_t relay_No_temp[11] = {0};
		float max_temp[11] = {0};
		float min_temp[11] = {0};		
		Temp_Choose_flag temp_choose_flag[11] = {temperature1_flag};
		float  temp_choose[11] = {0};
		uint8_t startup_mode[11] = {0};
		
		uint8_t max_nh3[11] = {0};
		uint8_t min_nh3[11] = {0};	


		
//		temperature1= middleValueFilter(10,ADC_Channel_2)-temp1_correct;
//		temperature2= middleValueFilter(10,ADC_Channel_3)-temp2_correct;
//		temperature3= middleValueFilter(10,ADC_Channel_4)-temp3_correct;
//		if(temperature1<0 || temperature1>90.0)temperature1=99.0;
//		if(temperature2<0 || temperature2>90.0)temperature2=99.0;
//		if(temperature3<0 || temperature3>90.0)temperature3=99.0;
////		printf("Temperature: %.2f degrees Celsius\r\n", temperature1);	
////    printf("Temperature: %.2f degrees Celsius\r\n", temperature2);	
////    printf("Temperature: %.2f degrees Celsius\r\n", temperature3);	
////		
////	    temperature1 = Read_Temperature(0);//
////		temperature2 = Read_Temperature(1)+17.3;//
////		temperature3 = Read_Temperature(2)+17.3;//
//		if((((temperature1>=0 && temperature1<=90) ? 1 :0)+ ((temperature2>=0 && temperature2<=90) ? 1 :0) + ((temperature3>=0 && temperature3<=90) ? 1 :0)) == 0){average_temp=0;}else{
//			average_temp = (((temperature1>=0 && temperature1<=90) ? temperature1 :0)+ ((temperature2>=0 && temperature2<=90) ? temperature2 :0) + ((temperature3>=0 && temperature3<=90) ? temperature3 :0)) / (((temperature1>=0 && temperature1<=90) ? 1 :0)+ ((temperature2>=0 && temperature2<=90) ? 1 :0) + ((temperature3>=0 && temperature3<=90) ? 1 :0));	
//		}
		
		
		uint8_t j = 0;
		//�˴��Ƿ�����ж��Ƿ���Ҫforѭ��
		//�¿�
		for(int i=0; i<10; i++){
			if(relay_structure[i].relay_mode == 1){
				relay_No_temp[j] = relay_structure[i].relayNo;
				max_temp[j] = relay_structure[i].temp_control.max_temp;
				min_temp[j] = relay_structure[i].temp_control.min_temp;
				temp_choose_flag[j] = relay_structure[i].temp_control.temp_choose_flag;
				//temp_choose[j] = relay_structure[i].temp_control.temp_choose;
				temp_choose[j] = get_temperature(temp_choose_flag[j]);
				startup_mode[j] = relay_structure[i].temp_control.startup_mode;
				
				max_nh3[j] = relay_structure[i].temp_control.max_nh3;
				min_nh3[j] = relay_structure[i].temp_control.min_nh3;
				j++;
			}
		}
		for (int i = 0; i < j; i++){

//			if(startup_mode[i] == 0){
//				//������
//				if((temp_choose[i] >= max_temp[i] && temp_choose[i]<=90) || (send_NH3!=999 && (send_NH3/10)>max_nh3[i])){
//					relay_structure[relay_No_temp[i]-1].on_off = 1;
////					printf("relay_No_temp[%d]:%d\n",i,relay_No_temp[i]);
////					printf("relay_structure[relay_No_temp[%d]-1].on_off:%d",i,relay_structure[relay_No_temp[i]-1].on_off);
//				}else if( (temp_choose[i] <= min_temp[i] && (send_NH3/10)<min_nh3[i]) || ((uint16_t)temp_choose[i] == 99 && (send_NH3/10)<min_nh3[i]) ){
//					relay_structure[relay_No_temp[i]-1].on_off = 0;
//				}
//			}else{
//				//������.
//				if((temp_choose[i] <= min_temp[i] && temp_choose[i]>0) || (send_NH3!=999 && (send_NH3/10)>max_nh3[i])){
//					relay_structure[relay_No_temp[i]-1].on_off = 1;
//				}else if( (temp_choose[i] >= max_temp[i] && (send_NH3/10)<min_nh3[i]) || ((uint16_t)temp_choose[i] == 99 && (send_NH3/10)<min_nh3[i]) ){
//					relay_structure[relay_No_temp[i]-1].on_off = 0;
//				}
//			}
			
			if(startup_mode[i] == 0){
				//������
				if(send_NH3 == 999){
					if(temp_choose[i] >= max_temp[i] && temp_choose[i]<=90){
						relay_structure[relay_No_temp[i]-1].on_off = 1;
	//					printf("relay_No_temp[%d]:%d\n",i,relay_No_temp[i]);
	//					printf("relay_structure[relay_No_temp[%d]-1].on_off:%d",i,relay_structure[relay_No_temp[i]-1].on_off);
					}else if(temp_choose[i] <= min_temp[i]){
						relay_structure[relay_No_temp[i]-1].on_off = 0;
					}
				}else{
					if((temp_choose[i] >= max_temp[i] && temp_choose[i]<=90) || send_NH3>max_nh3[i]){
						relay_structure[relay_No_temp[i]-1].on_off = 1;
	//					printf("relay_No_temp[%d]:%d\n",i,relay_No_temp[i]);
	//					printf("relay_structure[relay_No_temp[%d]-1].on_off:%d",i,relay_structure[relay_No_temp[i]-1].on_off);
					}else if( (temp_choose[i] <= min_temp[i] && send_NH3<min_nh3[i]) || ((uint16_t)temp_choose[i] == 99 && send_NH3<min_nh3[i]) ){
						relay_structure[relay_No_temp[i]-1].on_off = 0;
					}
				}

			}else{
				//������
				if(send_NH3 == 999){
					if(temp_choose[i] <= min_temp[i] && temp_choose[i]>0){
						relay_structure[relay_No_temp[i]-1].on_off = 1;
					}else if(temp_choose[i] >= max_temp[i]){
						relay_structure[relay_No_temp[i]-1].on_off = 0;
					}
				}else{
					if((temp_choose[i] <= min_temp[i] && temp_choose[i]>0) || send_NH3>max_nh3[i]){
						relay_structure[relay_No_temp[i]-1].on_off = 1;
					}else if( (temp_choose[i] >= max_temp[i] && send_NH3<min_nh3[i]) || ((uint16_t)temp_choose[i] == 99 && send_NH3<min_nh3[i]) ){
						relay_structure[relay_No_temp[i]-1].on_off = 0;
					}
				}

			}
			
			
		}
	}

}


//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{   
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
			
			// ��ʱ������
			static uint32_t led_counters[11] = {0};
			static uint32_t led_counters_t[11] = {0};

			
			//ʱ�ؾֲ�������Ϊʲô���ó�ȫ�ֱ����͸�ֵ�����ˣ���������������������������������������������������������������
//			static uint8_t led_states[] = {0}; // 0: ��, 1: ��
			
			//���±��������óɾ�̬�ֲ�������ÿ���±��Ӧ�̶���һ�������
			static uint16_t relay_on_times[11] = {0};
			static uint16_t relay_off_times[11] = {0};
			uint8_t  relay_No[11] = {0};
			//uint8_t  change_flag[] = {0};
			
			if(TIM3_Counter_10s>3)//һ���һ���жϣ��ۼ�8����־��1�����жϴ���
			{
			  TIM3_flag=1;
			}
			TIM3_Counter_10s++;
			//printf("TIM3-------\r\n");
			uint8_t j = 0;
			
			//�˴��Ƿ�����ж��Ƿ���Ҫforѭ��
			//ʱ��
			for(int ii=0; ii<10; ii++){
				if(relay_structure[ii].relay_mode == 2){
					relay_No[j] = relay_structure[ii].relayNo;
					relay_on_times[relay_No[j]-1] = relay_structure[ii].time_control.time_open*60;
					relay_off_times[relay_No[j]-1] = relay_structure[ii].time_control.time_stop*60;
					//change_flag[j] = relay_structure[ii].time_control.change_flag;
					j++;			
				}
			}
			
			
			for (int i = 0; i < j; i++)
			{

				//���ͬһ��ģʽ��ֵ���Ķ������¿�ʼ
//				if(change_flag[i] == 1){
//					led_counters[i] = 0;
//					relay_structure[relay_No[i]-1].on_off = 0;
//					change_flag[i] = 0;
//				}
				
				led_counters[relay_No[i]-1]++;
				if (relay_structure[relay_No[i]-1].on_off == 0 && led_counters[relay_No[i]-1] >= relay_off_times[relay_No[i]-1])
				{
					//led_states[i] = 1;
					relay_structure[relay_No[i]-1].on_off = 1;
					led_counters[relay_No[i]-1] = 0;
					//printf("relay_No:%d/n", relay_No[i]);

				}
				else if (relay_structure[relay_No[i]-1].on_off == 1 && led_counters[relay_No[i]-1] >= relay_on_times[relay_No[i]-1])
				{
					//led_states[i] = 0;
					relay_structure[relay_No[i]-1].on_off = 0;
					led_counters[relay_No[i]-1] = 0;
							
				}
				//printf("led_counters[%d]: %u\n", i, led_counters[i]);
			}
			
			//��ʱ�ؾֲ�����
			static uint32_t flag_t[11] = {0};
			static uint16_t relay_on_times_t[11] = {0};
			static uint16_t relay_off_times_t[11] = {0};
			uint8_t  relay_No_t[11] = {0};
			static uint8_t  change_flag_t[11] = {0};

			static float max_temp_t[11] = {0};
			static float min_temp_t[11] = {0};		
			static Temp_Choose_flag temp_choose_flag_t[11] = {temperature1_flag};
			static float  temp_choose_t[11] = {0};
			static uint8_t startup_mode_t[11] = {0};
			
			//��ʱ��
			uint8_t t = 0;
		    for(int i=0; i<10; i++){
				if(relay_structure[i].relay_mode == 3){
					//�¿ز���
					relay_No_t[t] = relay_structure[i].relayNo;
					max_temp_t[relay_No_t[t]-1] = relay_structure[i].temp_control.max_temp;
					min_temp_t[relay_No_t[t]-1] = relay_structure[i].temp_control.min_temp;
					temp_choose_flag_t[relay_No_t[t]-1] = relay_structure[i].temp_control.temp_choose_flag;
					//temp_choose[j] = relay_structure[i].temp_control.temp_choose;
					temp_choose_t[relay_No_t[t]-1] = get_temperature(temp_choose_flag_t[relay_No_t[t]-1]);
					startup_mode_t[relay_No_t[t]-1] = relay_structure[i].temp_control.startup_mode;
					//ʱ�ز���

					relay_on_times_t[relay_No_t[t]-1] = relay_structure[i].time_control.time_open*60;
					relay_off_times_t[relay_No_t[t]-1] = relay_structure[i].time_control.time_stop*60;
					
					//change_flag_t[relay_No_t[t]-1] = relay_structure[i].time_control.change_flag;
					
					t++;
				}
			}
			
		    for (int i = 0; i < t; i++)
			{
				if (led_counters_t[relay_No_t[i]-1] <= relay_off_times_t[relay_No_t[i]-1] && change_flag_t[relay_No_t[i]-1] == 0){
					led_counters_t[relay_No_t[i]-1]++;
					//printf("111\n");
					
				    if(startup_mode_t[relay_No_t[i]-1] == 0){
						//������
//						if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] < 90){
//							relay_structure[relay_No_t[i]-1].on_off = 1;
//						}else if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[t]-1]){
//							relay_structure[relay_No_t[i]-1].on_off = 0;
//						}
						if((uint16_t)temp_choose_t[relay_No_t[i]-1] == 99){
							relay_structure[relay_No_t[i]-1].on_off = 0;
							//printf("222\n");
						}else{
							if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] < 90){
								relay_structure[relay_No_t[i]-1].on_off = 1;
							}else if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[t]-1]){
								relay_structure[relay_No_t[i]-1].on_off = 0;
							}
							//printf("333\n");
						}	
					}else{
						//������.
//						if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1]>0){
//							relay_structure[relay_No_t[i]-1].on_off = 1;
//						}else if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[t]-1]){
//							relay_structure[relay_No_t[i]-1].on_off = 0;
//						}
						if((uint16_t)temp_choose_t[relay_No_t[i]-1] == 99){
							relay_structure[relay_No_t[i]-1].on_off = 0;
						}else{
							if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1]>0){
								relay_structure[relay_No_t[i]-1].on_off = 1;
							}else if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[t]-1]){
								relay_structure[relay_No_t[i]-1].on_off = 0;
							}
						}					

			        }
					
				}else if(led_counters_t[relay_No_t[i]-1] > relay_off_times_t[relay_No_t[i]-1] && change_flag_t[relay_No_t[i]-1] == 0){
					change_flag_t[relay_No_t[i]-1] = 1;
					led_counters_t[relay_No_t[i]-1] = 0;
					//printf("555\n");
				}
				
				if (led_counters_t[relay_No_t[i]-1] <= relay_on_times_t[relay_No_t[i]-1] && change_flag_t[relay_No_t[i]-1] == 1){
					led_counters_t[relay_No_t[i]-1]++;
					
					if(startup_mode_t[relay_No_t[i]-1] == 0){
						//������
//						if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] < 90){
//							relay_structure[relay_No_t[i]-1].on_off = 1;
//						}else if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[t]-1]){
//							relay_structure[relay_No_t[i]-1].on_off = 0;
//						}
						//printf("666\n");
						if((uint16_t)temp_choose_t[relay_No_t[i]-1] == 99){
							relay_structure[relay_No_t[i]-1].on_off = 1;
							//printf("777\n");
						}else{
							if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] < 90){
								relay_structure[relay_No_t[i]-1].on_off = 1;
							}else if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[t]-1]){
								relay_structure[relay_No_t[i]-1].on_off = 0;
							}
						}	
					}else{
						//������.
//						if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1]>0){
//							relay_structure[relay_No_t[i]-1].on_off = 1;
//						}else if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[t]-1]){
//							relay_structure[relay_No_t[i]-1].on_off = 0;
//						}
						if((uint16_t)temp_choose_t[relay_No_t[i]-1] == 99){
							relay_structure[relay_No_t[i]-1].on_off = 1;
							//printf("888\n");
						}else{
							if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1]>0){
								relay_structure[relay_No_t[i]-1].on_off = 1;
							}else if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[t]-1]){
								relay_structure[relay_No_t[i]-1].on_off = 0;
							}
						}	
						
			        }
					
				}else if(led_counters_t[relay_No_t[i]-1] > relay_on_times_t[relay_No_t[i]-1] && change_flag_t[relay_No_t[i]-1] == 1){
					change_flag_t[relay_No_t[i]-1] = 0;
					led_counters_t[relay_No_t[i]-1] = 0;
					//printf("999\n");
				}
				
			}
		}
			

}


void TIM1_UP_IRQHandler(void){
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //���TIM4�����жϱ�־
		
		if(TIM1_Counter>100)
		{
		  //TIM1_flag=1;
		  TIM1_Counter=0;
		}
		TIM1_Counter++;
		
//		static float temp1_list_buff[10]={99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0};
//		static float temp2_list_buff[10]={99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0};
//		static float temp3_list_buff[10]={99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0};
//		static float temp_avg_list_buff[10]={99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0};
		
		static float temp1_list_buff[10]={0.0};
		static float temp2_list_buff[10]={0.0};
		static float temp3_list_buff[10]={0.0};
		
		static float temp1_list[10]={99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0};
		static float temp2_list[10]={99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0};
		static float temp3_list[10]={99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0,99.0};

		
		float temperature1_buff = 0.0;
		list_change(temp1_list_buff,ADC_Channel_2,10);
		if(TIM1_Counter%10 == 0){
			float temperature1_buff = middleValueFilter(temp1_list_buff,10);
			list_change_end(temp1_list,temperature1_buff,10);
			if((int)temp1_list[0]==99 || (int)temp1_list[1]==99 || (int)temp1_list[2]==99 || 
			(int)temp1_list[3]==99 || (int)temp1_list[4]==99 || (int)temp1_list[5]==99 ||
			(int)temp1_list[6]==99 || (int)temp1_list[7]==99 || (int)temp1_list[8]==99 ||
			(int)temp1_list[9]==99){
				temperature1 = 99.0;
			}else{
				temperature1 = middleValueFilter(temp1_list,10)-temp1_correct;
			}
		}
//		if(TIM1_Counter==100){
//			float temperature1 = middleValueFilter(temp1_list_buff,10)-temp1_correct;
//			list_change_end(temp1_list,temperature1_buff,10);
//		}	
		if(temperature1<0 || temperature1>90.0)temperature1=99.0;
		
		

//		list_change(temp1_list,ADC_Channel_2,10);
//		if((int)temp1_list[0]==99 || (int)temp1_list[1]==99 || (int)temp1_list[2]==99 || 
//			(int)temp1_list[3]==99 || (int)temp1_list[4]==99 || (int)temp1_list[5]==99 ||
//			(int)temp1_list[6]==99 || (int)temp1_list[7]==99 || (int)temp1_list[8]==99 ||
//			(int)temp1_list[9]==99){
//				temperature1 = 99.0;
//			}else{
//				temperature1 = middleValueFilter(temp1_list,10)-temp1_correct;
//			}
//		if(temperature1<0 || temperature1>90.0)temperature1=99.0;
//	    printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[0]);
//		printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[1]);
//		printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[2]);
//		printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[3]);
//		printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[4]);
//		printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[5]);
//		printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[6]);
//		printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[7]);
//		printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[8]);
//				printf("temp1_list: %.2f degrees Celsius\r\n", temp1_list[9]);
		
		
		float temperature2_buff = 0.0;
		list_change(temp2_list_buff,ADC_Channel_3,10);
		if(TIM1_Counter%10 == 0){
			float temperature2_buff = middleValueFilter(temp2_list_buff,10);
			list_change_end(temp2_list,temperature2_buff,10);
			if((int)temp2_list[0]==99 || (int)temp2_list[1]==99 || (int)temp2_list[2]==99 || 
			(int)temp2_list[3]==99 || (int)temp2_list[4]==99 || (int)temp2_list[5]==99 ||
			(int)temp2_list[6]==99 || (int)temp2_list[7]==99 || (int)temp2_list[8]==99 ||
			(int)temp2_list[9]==99){
				temperature2 = 99.0;
			}else{
				temperature2 = middleValueFilter(temp2_list,10)-temp2_correct;
			}
		}
		
//		list_change(temp2_list,ADC_Channel_3,10);
//		if((int)temp2_list[0]==99 || (int)temp2_list[1]==99 || (int)temp2_list[2]==99 || 
//			(int)temp2_list[3]==99 || (int)temp2_list[4]==99 || (int)temp2_list[5]==99 ||
//			(int)temp2_list[6]==99 || (int)temp2_list[7]==99 || (int)temp2_list[8]==99 ||
//			(int)temp2_list[9]==99){
//				temperature2 = 99.0;
//			}else{
//				temperature2= middleValueFilter(temp2_list,10)-temp2_correct;
//			}
		if(temperature2<0 || temperature2>90.0)temperature2=99.0;

		
		float temperature3_buff = 0.0;
		list_change(temp3_list_buff,ADC_Channel_4,10);
		if(TIM1_Counter%10 == 0){
			float temperature3_buff = middleValueFilter(temp3_list_buff,10);
			list_change_end(temp3_list,temperature3_buff,10);
			if((int)temp3_list[0]==99 || (int)temp3_list[1]==99 || (int)temp3_list[2]==99 || 
			(int)temp3_list[3]==99 || (int)temp3_list[4]==99 || (int)temp3_list[5]==99 ||
			(int)temp3_list[6]==99 || (int)temp3_list[7]==99 || (int)temp3_list[8]==99 ||
			(int)temp3_list[9]==99){
				temperature3 = 99.0;
			}else{
				temperature3 = middleValueFilter(temp3_list,10)-temp3_correct;
			}
		}
		
		
//		list_change(temp3_list,ADC_Channel_4,10);
//		if((int)temp3_list[0]==99 || (int)temp3_list[1]==99 || (int)temp3_list[2]==99 || 
//			(int)temp3_list[3]==99 || (int)temp3_list[4]==99 || (int)temp3_list[5]==99 ||
//			(int)temp3_list[6]==99 || (int)temp3_list[7]==99 || (int)temp3_list[8]==99 ||
//			(int)temp3_list[9]==99){
//				temperature3 = 99.0;
//			}else{
//				temperature3= middleValueFilter(temp3_list,10)-temp3_correct;
//			}
		if(temperature3<0 || temperature3>90.0)temperature3=99.0;			
	
		
//		printf("Temperature1: %.2f degrees Celsius\r\n", temperature1);	
//		printf("Temperature2: %.2f degrees Celsius\r\n", temperature2);	
//		printf("Temperature3: %.2f degrees Celsius\r\n", temperature3);	
		
		if((((temperature1>=0 && temperature1<=90) ? 1 :0)+ ((temperature2>=0 && temperature2<=90) ? 1 :0) + ((temperature3>=0 && temperature3<=90) ? 1 :0)) == 0){average_temp=0;}else{
			average_temp = (((temperature1>=0 && temperature1<=90) ? temperature1 :0)+ ((temperature2>=0 && temperature2<=90) ? temperature2 :0) + ((temperature3>=0 && temperature3<=90) ? temperature3 :0)) / (((temperature1>=0 && temperature1<=90) ? 1 :0)+ ((temperature2>=0 && temperature2<=90) ? 1 :0) + ((temperature3>=0 && temperature3<=90) ? 1 :0));	
		}
		
	}
}

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
//void TIM3_PWM_Init(u16 arr,u16 psc)
//{  
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM_OCInitStructure;
//	

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
//	
////	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
// 
//   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOA.5
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
// 
//   //��ʼ��TIM3
//	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
//	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
//	
//	//��ʼ��TIM3 Channel2 PWMģʽ	 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
//	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3��ͨ��2

//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
// 
//	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
//	

//}
