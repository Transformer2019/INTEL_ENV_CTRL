#include "timer.h"
#include "usart1.h"
#include "gui.h"
#include "ntc.h"
#include "delay.h"
#include "mb_hook.h"
#include "GP8201S.h"
#include "flash.h"
#include "74HC595.h"

#include "key.h"
	  
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

//定时判断网络是否正常，以及是否注册
volatile uint16_t TIM3_Counter_10s=0;
volatile u8 TIM3_flag=0;

volatile uint32_t TIM4_Counter_10s=1;
volatile u8 TIM4_flag=0;

volatile uint16_t TIM5_Counter_10s=0;
volatile u8 TIM5_flag=0;

volatile uint32_t TIM6_Counter=0;
volatile u8 TIM6_flag=0;

volatile uint32_t Heartbeat_Counter_1s=0;
volatile u8 Heartbeat_flag=0;


volatile uint32_t TIM7_Counter=0;

volatile uint32_t TIM8_Counter_test=0;

//volatile u8 relay_Control[2] = {0x00, 0x00}; //控制继电器

//快加键使用的计数器
//volatile uint16_t TIM3_Add_Counter=0;

//判断是否连接服务器
volatile uint32_t MQTT_CON_Counter=0;
volatile u8 MQTT_CON_flag=0;

//ui风机转速计数器
volatile uint32_t relay_speed_ui_count=0;

//温度数组
//volatile float temp1_list[11]={0};
//volatile float temp2_list[11]={0};
//volatile float temp3_list[11]={0};
//volatile float temp_avg_list[11]={0};


volatile float temperature1 = 99.0;
volatile float temperature2 = 99.0;
volatile float temperature3 = 99.0;
volatile float average_temp = 99.0;




//修改函数，参数包括第几个风机以及开还是关
void update_relay_control(u8 index, u8 on_or_off){
    // 更新 relay_Control[1] (控制继电器 0-6)
	
	switch(index){
		case 0:
			if(on_or_off){relay_Control[1] = relay_Control[1] | 0x80;}else{relay_Control[1] = relay_Control[1] & ~0x80;}
			break;
		case 1:
			if(on_or_off){relay_Control[1] = relay_Control[1] | 0x40;}else{relay_Control[1] = relay_Control[1] & ~0x40;}
			break;
		case 2:
			if(on_or_off){relay_Control[1] = relay_Control[1] | 0x20;}else{relay_Control[1] = relay_Control[1] & ~0x20;}
			break;
		case 3:
			if(on_or_off){relay_Control[1] = relay_Control[1] | 0x10;}else{relay_Control[1] = relay_Control[1] & ~0x10;}
			break;
		case 4:
			if(on_or_off){relay_Control[1] = relay_Control[1] | 0x08;}else{relay_Control[1] = relay_Control[1] & ~0x08;}
			break;
		case 5:
			if(on_or_off){relay_Control[1] = relay_Control[1] | 0x04;}else{relay_Control[1] = relay_Control[1] & ~0x04;}
			break;
		case 6:
			if(on_or_off){relay_Control[1] = relay_Control[1] | 0x02;}else{relay_Control[1] = relay_Control[1] & ~0x02;}
			break;
		case 7:
			if(on_or_off){relay_Control[0] = relay_Control[0] | 0x80;}else{relay_Control[0] = relay_Control[0] & ~0x80;}
			break;
		case 8:
			if(on_or_off){relay_Control[0] = relay_Control[0] | 0x40;}else{relay_Control[0] = relay_Control[0] & ~0x40;}
			break;
		case 9:
			if(on_or_off){relay_Control[0] = relay_Control[0] | 0x20;}else{relay_Control[0] = relay_Control[0] & ~0x20;}
			break;
		default:break;
	}
}


void TIM1_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 199; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到500为50ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设
							 
}


void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 9999; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}

void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = 9999; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到1000为100ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //使能指定的TIM2中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
							 
}

//定时器2中断服务程序

float get_temperature(Temp_Choose_flag flag) {
	
    switch(flag) {
        case temperature1_flag: return temperature1;
        case temperature2_flag: return temperature2;
        case temperature3_flag: return temperature3;
        case average_temp_flag: return average_temp;
        default: return -1.0f;  // 错误处理，返回一个不合法的温度值
    }
}


//中值滤波算法
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
            //从小到大排序，冒泡法排序
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

void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{   
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
		
		//发送采集频率
		if(TIM4_Counter_10s>60)
		{
		  TIM4_flag=1;
		}
		TIM4_Counter_10s++;
		

#if 1	//将配置参数写入flash	
		
		//		if(TIM7_Counter>65500){
//			TIM7_Counter=0;
//			//向flash里写入需要存储的数据
//			uint16_t *pData = (uint16_t *)relay_structure;
//			
//			uint8_t warn_temp[18]={NH3_warn_flag,warn_temp1_flag,warn_temp2_flag,warn_temp3_flag,warn_temp485_flag,warn_rh_flag,
//				NH3_max,limit_temp1_maxvalue,limit_temp1_minvalue,limit_temp2_maxvalue,limit_temp2_minvalue,limit_temp3_maxvalue,limit_temp3_minvalue,limit_temp485_maxvalue,
//                limit_temp485_minvalue,limit_rh_maxvalue,limit_rh_minvalue,0xff
//			};			
//			uint16_t *pData_alarm = (uint16_t *)warn_temp;
//			
//			//uint16_t *pData_hz = (uint16_t *)(&hz_control_cur);
//			Hz_Control *hz_control_cur=&hz_control;
//			uint16_t *pData_hz = (uint16_t *)hz_control_cur;
//			
//			WriteFlashData(0, pData, 5*sizeof(Relay_Structure),pData_alarm,10,pData_hz,(sizeof(hz_control)%2==0)?(sizeof(hz_control)/2):(sizeof(hz_control)/2+1));
//			//WriteFlashData(0, pData, 5*sizeof(Relay_Structure));

//		}
//		TIM7_Counter++;
		
	if(TIM7_Counter>2){
			TIM7_Counter=0;
	    //向flash里写入需要存储的数据
			//向flash里写入风机控制数据
			if(relay_change_falg){
				relay_change_falg=0;
				uint16_t *pData = (uint16_t *)relay_structure;
				WriteFlashData_relay(0, pData, 5*sizeof(Relay_Structure));
				memcpy(relay_change_buffer, relay_structure, sizeof(relay_structure));
			}
			
			
			
			//向flash里写入报警控制数据
			if(warn_change_falg){
				warn_change_falg=0;
				uint8_t warn_temp[18]={NH3_warn_flag,warn_temp1_flag,warn_temp2_flag,warn_temp3_flag,warn_temp485_flag,warn_rh_flag,
				NH3_max,limit_temp1_maxvalue,limit_temp1_minvalue,limit_temp2_maxvalue,limit_temp2_minvalue,limit_temp3_maxvalue,limit_temp3_minvalue,limit_temp485_maxvalue,
								limit_temp485_minvalue,limit_rh_maxvalue,limit_rh_minvalue,0xff
				};	
				uint16_t *pData_alarm = (uint16_t *)warn_temp;
				WriteFlashData_alarm(0,pData_alarm,10);
				
				warn_change_buffer.warn_temp1_flag = warn_temp1_flag;
				warn_change_buffer.warn_temp2_flag = warn_temp2_flag;
				warn_change_buffer.warn_temp485_flag = warn_temp485_flag;
				warn_change_buffer.warn_rh_flag = warn_rh_flag;
				
				warn_change_buffer.limit_temp1_maxvalue = limit_temp1_maxvalue;
				warn_change_buffer.limit_temp1_minvalue = limit_temp1_minvalue;
				warn_change_buffer.limit_temp2_maxvalue = limit_temp2_maxvalue;
				warn_change_buffer.limit_temp2_minvalue = limit_temp2_minvalue;
				warn_change_buffer.limit_temp485_maxvalue = limit_temp485_maxvalue;
				warn_change_buffer.limit_temp485_minvalue = limit_temp485_minvalue;
				warn_change_buffer.limit_rh_maxvalue = limit_rh_maxvalue;
				warn_change_buffer.limit_rh_minvalue = limit_rh_minvalue;
				
				warn_change_buffer.NH3_warn_flag = NH3_warn_flag;
				warn_change_buffer.NH3_max = NH3_max;
			}
	
			
			
			//向flash里写入变频控制数据
			if(hz_change_falg){
				hz_change_falg=0;
				Hz_Control *hz_control_cur=&hz_control;
				uint16_t *pData_hz = (uint16_t *)hz_control_cur;
				WriteFlashData_hz(0,pData_hz,(sizeof(hz_control)%2==0)?(sizeof(hz_control)/2):(sizeof(hz_control)/2+1));
				memcpy(&hz_change_buffer,&hz_control,sizeof(Hz_Control));
			}

			//WriteFlashData(0, pData, 5*sizeof(Relay_Structure),pData_alarm,10,pData_hz,(sizeof(hz_control)%2==0)?(sizeof(hz_control)/2):(sizeof(hz_control)/2+1));
		
	}
	TIM7_Counter++;
#endif	
		
		//获取是否联网频率
		if(TIM5_Counter_10s>60)
		{
			TIM5_flag=1;
		}
		TIM5_Counter_10s++;
		
		//获取网络时间频率
		if(TIM6_Counter>21600)
		{
			TIM6_flag=1;
		}
		TIM6_Counter++;
		
		
		//发送心跳频率
		if(Heartbeat_Counter_1s>4)//一秒进一次中断，累计8秒后标志置1，再中断处理，发布消息
		{
		  Heartbeat_flag=1;
		}
		Heartbeat_Counter_1s++;

		//printf("send_NH3:%d\n",send_NH3);
		



		
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
		

		
		//变频控制开始
		switch(hz_control.temp_choose){
			case 1:Hz_temp_choose = temperature1;break;
			case 2:Hz_temp_choose = temperature2;break;
			case 3:Hz_temp_choose = temperature3;break;
			case 4:Hz_temp_choose = average_temp;break;
			//case 5:Hz_temp_choose = send_TEMP/10;break;
			default:break;
		}
		u16 data_v;
		//printf("Hz_temp_choose:%.2f\n",Hz_temp_choose);
		if(Hz_temp_choose<=90){
			if(Hz_temp_choose<=hz_control.min_temp){
				data_v = hz_control.voltage_low*4095/10;
				out_voltage=hz_control.voltage_low*5.0;
			}else if(Hz_temp_choose>=hz_control.max_temp){
				data_v = hz_control.voltage_high*4095/10;
				out_voltage=hz_control.voltage_high*5.0;
			}else{
				float data_v_t = ((Hz_temp_choose-hz_control.min_temp)/(hz_control.max_temp-hz_control.min_temp)*(hz_control.voltage_high-hz_control.voltage_low))+hz_control.voltage_low;
				data_v = data_v_t*4095/10;
				out_voltage=data_v_t*5.0;
				//printf("out_voltage:%d\n",out_voltage);
			}
		}else{
			data_v=0;
			out_voltage=data_v;
		}
		
		u16 v_low = data_v<<4 | 0x00;
		u16 v_high = data_v>>4 | 0x00;
		GP8201S_Write(0,0,v_low,v_high);
		//变频控制结束
		
		
		uint8_t j = 0;
		uint8_t relay_No_temp[11] = {0};
		float max_temp[11] = {0};
		float min_temp[11] = {0};		
		Temp_Choose_flag temp_choose_flag[11] = {temperature1_flag};
		float  temp_choose[11] = {0};
		uint8_t startup_mode[11] = {0};
		
		uint8_t max_nh3[11] = {0};
		uint8_t min_nh3[11] = {0};	
		//此处是否可以判断是否需要for循环
		//温控
		for(int i=0; i<ROAD_COUNT; i++){
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
//				//冷启动
//				if((temp_choose[i] >= max_temp[i] && temp_choose[i]<=90) || (send_NH3!=999 && (send_NH3/10)>max_nh3[i])){
//					relay_structure[relay_No_temp[i]-1].on_off = 1;
////					printf("relay_No_temp[%d]:%d\n",i,relay_No_temp[i]);
////					printf("relay_structure[relay_No_temp[%d]-1].on_off:%d",i,relay_structure[relay_No_temp[i]-1].on_off);
//				}else if( (temp_choose[i] <= min_temp[i] && (send_NH3/10)<min_nh3[i]) || ((uint16_t)temp_choose[i] == 99 && (send_NH3/10)<min_nh3[i]) ){
//					relay_structure[relay_No_temp[i]-1].on_off = 0;
//				}
//			}else{
//				//热启动.
//				if((temp_choose[i] <= min_temp[i] && temp_choose[i]>0) || (send_NH3!=999 && (send_NH3/10)>max_nh3[i])){
//					relay_structure[relay_No_temp[i]-1].on_off = 1;
//				}else if( (temp_choose[i] >= max_temp[i] && (send_NH3/10)<min_nh3[i]) || ((uint16_t)temp_choose[i] == 99 && (send_NH3/10)<min_nh3[i]) ){
//					relay_structure[relay_No_temp[i]-1].on_off = 0;
//				}
//			}
			
			if(startup_mode[i] == 0){
				//冷启动
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
				//热启动
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


//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{   
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
			


			//顺序间隔开启风机（2025.06.18修改）延迟启动，顺序启动----------开始
			static uint32_t current_time = 0;

			//static uint8_t relay_on_off_temp[10]={0};

			static uint8_t on_flag=0;
			static uint8_t relay_no[20]={9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9};
			static uint8_t relay_off_on_count = 0;
			static uint8_t current_relay = 0;
			static uint8_t last_state[10] = {0};
				
			
			//for(int i=0;i<9;i++)relay_structure[i].on_off=1;
			
			uint8_t relay_index = 0;
			while(relay_index < ROAD_COUNT)
			{

				if(relay_structure[relay_index].on_off==0)last_state[relay_index]=0;
				if(last_state[relay_index]==0 && relay_structure[relay_index].on_off==1){
					relay_no[relay_off_on_count]=relay_index;
					relay_off_on_count++;
					last_state[relay_index]=1;
					//printf("relay_index:%d\n",relay_index);		
				}
				
				relay_index++;
				
			}
			
			if(relay_no[0] != 9 && relay_structure[relay_no[0]].on_off)
				current_time++;
			else {
				if(relay_no[0] != 9){
					for (int i = 0; i < relay_off_on_count; i++) {
						relay_no[i] = relay_no[i + 1];
					}
					current_time=0;
					relay_off_on_count--;
				}
			}
			
			if(current_time>2){
					// 更新控制寄存器---只发送一位
					update_relay_control(relay_no[0], relay_structure[relay_no[0]].on_off);
					// 发送数据到硬件
					HC595_Send_Multi_Byte(relay_Control, 2);
					current_time=0;
					for (int i = 0; i < relay_off_on_count; i++) {
						relay_no[i] = relay_no[i + 1];
					}
					relay_off_on_count--;
			}

			
			//关闭风机时，不间隔时间
			//static u8 count_595 = 0;
			//if(count_595%8==0){
				//风机
				for(int i =0; i<ROAD_COUNT; i++){
					if(relay_structure[i].on_off==0)update_relay_control(i,0);
				}
				//报警继电器
				if(warn_flag){relay_Control[0] = relay_Control[0] | 0x10;}else{relay_Control[0] = relay_Control[0] & ~0x10;}
				HC595_Send_Multi_Byte(relay_Control, 2);
			//}
			//if(++count_595>=32)count_595=0;
				
			//顺序间隔开启风机（2025.06.18修改）延迟启动，顺序启动----------结束
			
			
			//开机测试代码
			static u8 test_code=0;
			if(MQTT_CON_Counter<10)
			{
				if(KEY_LEFT==0 && KEY_RIGHT==0)test_code=8;
//				printf("KEY_LEFT:%d/n",KEY_LEFT);
//				printf("KEY_RIGHT:%d/n",KEY_RIGHT);
//				printf("KEY_UP:%d/n",KEY_UP);
//				printf("KEY_DOWN:%d/n",KEY_DOWN);
//				printf("test_code:%d\n",test_code);
			}
			
			if(test_code==8 && MQTT_CON_Counter>12){
//				printf("TIM8_Counter_test:%d\n",TIM8_Counter_test);
//				for(int i=0; i<8; i++){
//					printf("relay_structure[%d].on_off:%d\n",i,relay_structure[i].on_off);
//				}		

				if(TIM8_Counter_test==0){relay_structure[0].relay_mode=0;relay_structure[0].no_Ctrl.all_open_or_close=1;}
				if(TIM8_Counter_test==5){relay_structure[0].relay_mode=0;relay_structure[0].no_Ctrl.all_open_or_close=0;}
				if(TIM8_Counter_test==10){relay_structure[1].relay_mode=0;relay_structure[1].no_Ctrl.all_open_or_close=1;}
				if(TIM8_Counter_test==15){relay_structure[1].relay_mode=0;relay_structure[1].no_Ctrl.all_open_or_close=0;}
				if(TIM8_Counter_test==20){relay_structure[2].relay_mode=0;relay_structure[2].no_Ctrl.all_open_or_close=1;}
				if(TIM8_Counter_test==25){relay_structure[2].relay_mode=0;relay_structure[2].no_Ctrl.all_open_or_close=0;}
				if(TIM8_Counter_test==30){relay_structure[3].relay_mode=0;relay_structure[3].no_Ctrl.all_open_or_close=1;}
				if(TIM8_Counter_test==35){relay_structure[3].relay_mode=0;relay_structure[3].no_Ctrl.all_open_or_close=0;}
				if(TIM8_Counter_test==40){relay_structure[4].relay_mode=0;relay_structure[4].no_Ctrl.all_open_or_close=1;}
				if(TIM8_Counter_test==45){relay_structure[4].relay_mode=0;relay_structure[4].no_Ctrl.all_open_or_close=0;}
				if(TIM8_Counter_test==50){relay_structure[5].relay_mode=0;relay_structure[5].no_Ctrl.all_open_or_close=1;}
				if(TIM8_Counter_test==55){relay_structure[5].relay_mode=0;relay_structure[5].no_Ctrl.all_open_or_close=0;}
				if(TIM8_Counter_test==60){relay_structure[6].relay_mode=0;relay_structure[6].no_Ctrl.all_open_or_close=1;}
				if(TIM8_Counter_test==65){relay_structure[6].relay_mode=0;relay_structure[6].no_Ctrl.all_open_or_close=0;}
				if(TIM8_Counter_test==70){relay_structure[7].relay_mode=0;relay_structure[7].no_Ctrl.all_open_or_close=1;}
				if(TIM8_Counter_test==75){relay_structure[7].relay_mode=0;relay_structure[7].no_Ctrl.all_open_or_close=0;}
				
				TIM8_Counter_test++;
				if(TIM8_Counter_test>=78)test_code=0;
			}
			//测试代码结束
			
			
			if(TIM3_Counter_10s>8)//
			{
			  TIM3_flag=1;
			}
			TIM3_Counter_10s++;
			
			//mqtt连接判断计数器，一小时判断一次是否连接服务器-----修改为5分钟
			if(MQTT_CON_Counter>300)
			{
			  MQTT_CON_flag=1;
			}
			MQTT_CON_Counter++;
			
			
			
			// 定时计数器
			static uint32_t led_counters[11] = {0};
			static uint32_t led_counters_t[11] = {0};

			
			//时控局部变量，为什么设置成全局变量就赋值就乱了？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
//			static uint8_t led_states[] = {0}; // 0: 关, 1: 开
			
			//以下变量需设置成静态局部变量，每个下标对应固定的一个风机，
			static uint16_t relay_on_times[11] = {0};
			static uint16_t relay_off_times[11] = {0};
			uint8_t  relay_No[11] = {0};
			//uint8_t  change_flag[] = {0};

			//printf("TIM3-------\r\n");
			uint8_t j = 0;
			
			//此处是否可以判断是否需要for循环
			//时控
			for(int ii=0; ii<ROAD_COUNT; ii++){
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

				//如果同一种模式，值被改动，重新开始
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
			
			//温时控局部变量
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
			
			//温时控
			uint8_t t = 0;
		    for(int i=0; i<ROAD_COUNT; i++){
				if(relay_structure[i].relay_mode == 3){
					//温控参数
					relay_No_t[t] = relay_structure[i].relayNo;
					max_temp_t[relay_No_t[t]-1] = relay_structure[i].temp_control.max_temp;
					min_temp_t[relay_No_t[t]-1] = relay_structure[i].temp_control.min_temp;
					temp_choose_flag_t[relay_No_t[t]-1] = relay_structure[i].temp_control.temp_choose_flag;
					//temp_choose[j] = relay_structure[i].temp_control.temp_choose;
					temp_choose_t[relay_No_t[t]-1] = get_temperature(temp_choose_flag_t[relay_No_t[t]-1]);
					startup_mode_t[relay_No_t[t]-1] = relay_structure[i].temp_control.startup_mode;
					//时控参数

					relay_on_times_t[relay_No_t[t]-1] = relay_structure[i].time_control.time_open*60;
					relay_off_times_t[relay_No_t[t]-1] = relay_structure[i].time_control.time_stop*60;
					
					//change_flag_t[relay_No_t[t]-1] = relay_structure[i].time_control.change_flag;
					
					t++;
				}
			}
			
		    for (int i = 0; i < t; i++)
			{
//				if (led_counters_t[relay_No_t[i]-1] <= relay_off_times_t[relay_No_t[i]-1] && change_flag_t[relay_No_t[i]-1] == 0){
//					led_counters_t[relay_No_t[i]-1]++;
//					//printf("111\n");
//					
//				    if(startup_mode_t[relay_No_t[i]-1] == 0){
//						//冷启动
////						if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] < 90){
////							relay_structure[relay_No_t[i]-1].on_off = 1;
////						}else if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[t]-1]){
////							relay_structure[relay_No_t[i]-1].on_off = 0;
////						}
//						if((uint16_t)temp_choose_t[relay_No_t[i]-1] == 99){
//							relay_structure[relay_No_t[i]-1].on_off = 0;
//							//printf("222\n");
//						}else{
//							if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] < 90){
//								relay_structure[relay_No_t[i]-1].on_off = 1;
//							}else if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[t]-1]){
//								relay_structure[relay_No_t[i]-1].on_off = 0;
//							}
//							//printf("333\n");
//						}	
//					}else{
//						//热启动.
////						if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1]>0){
////							relay_structure[relay_No_t[i]-1].on_off = 1;
////						}else if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[t]-1]){
////							relay_structure[relay_No_t[i]-1].on_off = 0;
////						}
//						if((uint16_t)temp_choose_t[relay_No_t[i]-1] == 99){
//							relay_structure[relay_No_t[i]-1].on_off = 0;
//						}else{
//							if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1]>0){
//								relay_structure[relay_No_t[i]-1].on_off = 1;
//							}else if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[t]-1]){
//								relay_structure[relay_No_t[i]-1].on_off = 0;
//							}
//						}					

//			        }
//					
//				}else if(led_counters_t[relay_No_t[i]-1] > relay_off_times_t[relay_No_t[i]-1] && change_flag_t[relay_No_t[i]-1] == 0){
//					change_flag_t[relay_No_t[i]-1] = 1;
//					led_counters_t[relay_No_t[i]-1] = 0;
//					//printf("555\n");
//				}
				
				
				
				//修改温室控逻辑----------开始
				if(startup_mode_t[relay_No_t[i]-1] == 0){
					//冷启动
					if(temp_choose_t[relay_No_t[i]-1] >= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] <= max_temp_t[relay_No_t[i]-1]){
						led_counters_t[relay_No_t[i]-1]++;
						if (relay_structure[relay_No_t[i]-1].on_off == 0 && led_counters_t[relay_No_t[i]-1] >= relay_off_times_t[relay_No_t[i]-1])
						{
							relay_structure[relay_No_t[i]-1].on_off = 1;
							led_counters_t[relay_No_t[i]-1] = 0;
						}
						else if (relay_structure[relay_No_t[i]-1].on_off == 1 && led_counters_t[relay_No_t[i]-1] >= relay_on_times_t[relay_No_t[i]-1])
						{
							relay_structure[relay_No_t[i]-1].on_off = 0;
							led_counters_t[relay_No_t[i]-1] = 0;
									
						}
					}else if((temp_choose_t[relay_No_t[i]-1] < min_temp_t[relay_No_t[i]-1]) && (temp_choose_t[relay_No_t[i]-1]>=0)){
						led_counters_t[relay_No_t[i]-1]=0;
						relay_structure[relay_No_t[i]-1].on_off = 0;
					}else if((temp_choose_t[relay_No_t[i]-1] > max_temp_t[relay_No_t[i]-1]) && (temp_choose_t[relay_No_t[i]-1]<=90)){
						led_counters_t[relay_No_t[i]-1]=0;
						relay_structure[relay_No_t[i]-1].on_off = 1;
					}else if(temp_choose_t[relay_No_t[i]-1]>91){
						led_counters_t[relay_No_t[i]-1]=0;
						relay_structure[relay_No_t[i]-1].on_off = 0;
					}
					
				}else{
					//热启动
					if(temp_choose_t[relay_No_t[i]-1] >= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] <= max_temp_t[relay_No_t[i]-1]){
						led_counters_t[relay_No_t[i]-1]++;
						if (relay_structure[relay_No_t[i]-1].on_off == 0 && led_counters_t[relay_No_t[i]-1] >= relay_off_times_t[relay_No_t[i]-1])
						{
							relay_structure[relay_No_t[i]-1].on_off = 1;
							led_counters_t[relay_No_t[i]-1] = 0;
						}
						else if (relay_structure[relay_No_t[i]-1].on_off == 1 && led_counters_t[relay_No_t[i]-1] >= relay_on_times_t[relay_No_t[i]-1])
						{
							relay_structure[relay_No_t[i]-1].on_off = 0;
							led_counters_t[relay_No_t[i]-1] = 0;
									
						}
					}else if((temp_choose_t[relay_No_t[i]-1] < min_temp_t[relay_No_t[i]-1]) && (temp_choose_t[relay_No_t[i]-1]>=0)){
						led_counters_t[relay_No_t[i]-1]=0;
						relay_structure[relay_No_t[i]-1].on_off = 1;
					}else if((temp_choose_t[relay_No_t[i]-1] > max_temp_t[relay_No_t[i]-1]) && (temp_choose_t[relay_No_t[i]-1]<=90)){
						led_counters_t[relay_No_t[i]-1]=0;
						relay_structure[relay_No_t[i]-1].on_off = 0;
					}else if(temp_choose_t[relay_No_t[i]-1]>91){
						led_counters_t[relay_No_t[i]-1]=0;
						relay_structure[relay_No_t[i]-1].on_off = 0;
					}
				}
				
				//修改温室控逻辑----------结束
				
				
//				if (led_counters_t[relay_No_t[i]-1] <= relay_on_times_t[relay_No_t[i]-1] && change_flag_t[relay_No_t[i]-1] == 1){
//					led_counters_t[relay_No_t[i]-1]++;
//					
//					if(startup_mode_t[relay_No_t[i]-1] == 0){
//						//冷启动
////						if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] < 90){
////							relay_structure[relay_No_t[i]-1].on_off = 1;
////						}else if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[t]-1]){
////							relay_structure[relay_No_t[i]-1].on_off = 0;
////						}
//						//printf("666\n");
//						if((uint16_t)temp_choose_t[relay_No_t[i]-1] == 99){
//							relay_structure[relay_No_t[i]-1].on_off = 1;
//							//printf("777\n");
//						}else{
//							if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1] < 90){
//								relay_structure[relay_No_t[i]-1].on_off = 1;
//							}else if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[t]-1]){
//								relay_structure[relay_No_t[i]-1].on_off = 0;
//							}
//						}	
//					}else{
//						//热启动.
////						if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1]>0){
////							relay_structure[relay_No_t[i]-1].on_off = 1;
////						}else if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[t]-1]){
////							relay_structure[relay_No_t[i]-1].on_off = 0;
////						}
//						if((uint16_t)temp_choose_t[relay_No_t[i]-1] == 99){
//							relay_structure[relay_No_t[i]-1].on_off = 1;
//							//printf("888\n");
//						}else{
//							if(temp_choose_t[relay_No_t[i]-1] <= min_temp_t[relay_No_t[i]-1] && temp_choose_t[relay_No_t[i]-1]>0){
//								relay_structure[relay_No_t[i]-1].on_off = 1;
//							}else if(temp_choose_t[relay_No_t[i]-1] >= max_temp_t[relay_No_t[t]-1]){
//								relay_structure[relay_No_t[i]-1].on_off = 0;
//							}
//						}	
//						
//			        }
//					
//				}else if(led_counters_t[relay_No_t[i]-1] > relay_on_times_t[relay_No_t[i]-1] && change_flag_t[relay_No_t[i]-1] == 1){
//					change_flag_t[relay_No_t[i]-1] = 0;
//					led_counters_t[relay_No_t[i]-1] = 0;
//					//printf("999\n");
//				}
				
				
			}
			
			
		}
			

}




//#include <stdint.h>

//uint8_t atomic_load(volatile uint8_t *ptr) {
//    uint8_t val;
//    do {
//        val = __ldrex(ptr);  // 原子加载
//    } while (__strex(val, ptr));  // 确保原子性
//    return val;
//}


void TIM1_UP_IRQHandler(void){
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //清除TIM4更新中断标志
		
//		static uint16_t timer_ralay = 0;
//		timer_ralay++;
//		if(timer_ralay>=200)timer_ralay=0;

//		//顺序间隔开启风机
//		static uint32_t current_time = 0;
//		static uint32_t next_action_time = 0;         // 下次允许操作的时间戳
//		
//		//static uint8_t relay_on_off[10]={0};
//		static uint8_t relay_on_off_temp[10]={0};
//		

//		//for(int i=0; i<ROAD_COUNT; i++){relay_on_off_temp[i]=relay_structure[i].on_off;}//


//		static uint8_t on_flag=0;
//		static uint8_t relay_no[20]={9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9};
//		static uint8_t relay_off_on_count = 0;
//		static uint8_t current_relay = 0;
//		static uint8_t last_state[10] = {0};
//			
//		uint8_t relay_index = 0;
//		while(relay_index < ROAD_COUNT)
//		{

//			relay_on_off_temp[relay_index]=relay_structure[relay_index].on_off;
//	
//			if(last_state[relay_index]==0 && relay_structure[relay_index].on_off==1){
//				relay_no[relay_off_on_count]=relay_index;
//				relay_off_on_count++;
//				last_state[relay_index]=1;
//				//printf("relay_index:%d\n",relay_index);
//			}
//			relay_index++;
//			
//	  }
//		
//		if(relay_no[0] != 9 && relay_structure[relay_no[0]].on_off)
//			current_time++;
//		else {
//			if(relay_no[0] != 9){
//				for (int i = 0; i < relay_off_on_count; i++) {
//					relay_no[i] = relay_no[i + 1];
//				}
//				current_time=0;
//				relay_off_on_count--;
//			}
//		}
//		
//		if(current_time>=100){
//				// 更新控制寄存器---只发送一位
//				update_relay_control(relay_no[0], relay_structure[relay_no[0]].on_off);
//				// 发送数据到硬件
//				HC595_Send_Multi_Byte(relay_Control, 2);
//				current_time=0;
//				for (int i = 0; i < relay_off_on_count; i++) {
//					relay_no[i] = relay_no[i + 1];
//				}
//				relay_off_on_count--;
//		}

//		
//		
//		//关闭风机时，不间隔时间
//		static u8 count_595 = 0;
//		if(count_595%8==0){
//			//风机
//			for(int i =0; i<ROAD_COUNT; i++){
//				if(relay_structure[i].on_off==0)update_relay_control(i,0);
//			}
//			//报警继电器
//			if(warn_flag){relay_Control[0] = relay_Control[0] | 0x10;}else{relay_Control[0] = relay_Control[0] & ~0x10;}
//			HC595_Send_Multi_Byte(relay_Control, 2);
//		}
//		if(++count_595>=32)count_595=0;

		
		
		
		
		
		//
		for(int i=0; i<ROAD_COUNT; i++){
			if(relay_structure[i].relay_mode==0 && relay_structure[i].no_Ctrl.all_open_or_close==0)relay_structure[i].on_off=0;
			if(relay_structure[i].relay_mode==0 && relay_structure[i].no_Ctrl.all_open_or_close==1)relay_structure[i].on_off=1;
		}
		
		//继电器输出
//		u8 temp;
//		temp = relay_Control[1] & 0X3f | (relay_structure[0].on_off << 7) | (relay_structure[1].on_off << 6);
//		//if(relay_Control[1]!=temp && TIM3_flag){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);}
//		
//		temp = relay_Control[1] & 0Xcf | (relay_structure[2].on_off << 5) | (relay_structure[3].on_off << 4);
//		//if(relay_Control[1]!=temp && TIM3_flag){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);}
//		
//		temp = relay_Control[1] & 0Xf3 | (relay_structure[4].on_off << 3) | (relay_structure[5].on_off << 2);
//		//if(relay_Control[1]!=temp && TIM3_flag){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);}
//		
//		temp = relay_Control[1] & 0Xfd | (relay_structure[6].on_off << 1);
//		//if(relay_Control[1]!=temp && TIM3_flag){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);}
//		
//		temp = relay_Control[0] & 0X3f | (relay_structure[7].on_off << 7) | (relay_structure[8].on_off << 6);
//		//if(relay_Control[0]!=temp && TIM3_flag){relay_Control[0]=temp;HC595_Send_Multi_Byte(relay_Control, 2);}
//		
//		temp = relay_Control[0] & 0Xcf | (relay_structure[9].on_off << 5) | (warn_flag << 4);
//		//if(relay_Control[0]!=temp && TIM3_flag){relay_Control[0]=temp;HC595_Send_Multi_Byte(relay_Control, 2);}

//		relay_Control[1] = (relay_structure[0].on_off << 7) | (relay_structure[1].on_off << 6) | (relay_structure[2].on_off << 5) | (relay_structure[3].on_off << 4) | (relay_structure[4].on_off << 3) | (relay_structure[5].on_off << 2) | (relay_structure[6].on_off << 1);
//		relay_Control[0] = (relay_structure[7].on_off << 7) | (relay_structure[8].on_off << 6) | (relay_structure[9].on_off << 5) | (warn_flag << 4);
//		HC595_Send_Multi_Byte(relay_Control, 2);
		
		//控制显示的风机转的频率（控制风机顺序输出）
//		if(relay_speed_ui_count>=99999)
//		{
//		  relay_speed_ui_count=0;
//		}
//		relay_speed_ui_count++;





//		if(TIM1_Counter>10)
//		{
//		  //TIM1_flag=1;
//		  TIM1_Counter=0;
//		}
//		TIM1_Counter++;
		
		
		
		
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
		//if(TIM1_Counter%10 == 0){
		if(1){
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
		//if(TIM1_Counter%1 == 0){
		if(1){
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

		
//		float temperature3_buff = 0.0;
//		list_change(temp3_list_buff,ADC_Channel_4,10);
//		//if(TIM1_Counter%1 == 0){
//		if(1){
//			float temperature3_buff = middleValueFilter(temp3_list_buff,10);
//			list_change_end(temp3_list,temperature3_buff,10);
//			if((int)temp3_list[0]==99 || (int)temp3_list[1]==99 || (int)temp3_list[2]==99 || 
//			(int)temp3_list[3]==99 || (int)temp3_list[4]==99 || (int)temp3_list[5]==99 ||
//			(int)temp3_list[6]==99 || (int)temp3_list[7]==99 || (int)temp3_list[8]==99 ||
//			(int)temp3_list[9]==99){
//				temperature3 = 99.0;
//			}else{
//				temperature3 = middleValueFilter(temp3_list,10)-temp3_correct;
//			}
//		}
//		
//		
		temperature3 = send_TEMP / 10.0;
		if(temperature3<0 || temperature3>90.0)temperature3=99.0;			
		
		
		
		if((((temperature1>=0 && temperature1<=90) ? 1 :0)+ ((temperature2>=0 && temperature2<=90) ? 1 :0) + ((temperature3>=0 && temperature3<=90) ? 1 :0)) == 0){average_temp=99.0;}else{
			average_temp = (((temperature1>=0 && temperature1<=90) ? temperature1 :0)+ ((temperature2>=0 && temperature2<=90) ? temperature2 :0) + ((temperature3>=0 && temperature3<=90) ? temperature3 :0)) / (((temperature1>=0 && temperature1<=90) ? 1 :0)+ ((temperature2>=0 && temperature2<=90) ? 1 :0) + ((temperature3>=0 && temperature3<=90) ? 1 :0));	
		}
		
	}
}

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
//void TIM3_PWM_Init(u16 arr,u16 psc)
//{  
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM_OCInitStructure;
//	

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
//	
////	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
// 
//   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOA.5
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
// 
//   //初始化TIM3
//	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
//	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值 
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
//	
//	//初始化TIM3 Channel2 PWM模式	 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
//	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3的通道2

//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
// 
//	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
//	

//}
