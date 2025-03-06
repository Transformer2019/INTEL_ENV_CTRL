#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


extern volatile uint16_t TIM3_Counter_10s;
extern volatile u8 TIM3_flag;

extern volatile uint16_t TIM4_Counter_10s;
extern volatile u8 TIM4_flag;

extern volatile uint16_t TIM5_Counter_10s;
extern volatile u8 TIM5_flag;

extern volatile uint16_t TIM6_Counter;
extern volatile u8 TIM6_flag;

extern volatile uint16_t TIM7_Counter;

extern volatile uint8_t Heartbeat_Counter_1s;
extern volatile u8 Heartbeat_flag;

extern volatile u8 TIM1_Counter;
extern volatile u8 TIM1_flag;

extern volatile uint16_t MQTT_CON_Counter;
extern volatile u8 MQTT_CON_flag;

//ui风机转速计数器
extern volatile uint32_t relay_speed_ui_count;

//继电器输出
//extern volatile u8 relay_Control[2]; //控制继电器

	
//快加键使用的计数器
//extern volatile uint16_t TIM3_Add_Counter;

//温度数组
//extern volatile float temp1_list[11];
//extern volatile float temp2_list[11];
//extern volatile float temp3_list[11];
//extern volatile float temp_avg_list[11];

extern volatile float temperature1;
extern volatile float temperature2;
extern volatile float temperature3;
extern volatile float average_temp;


void TIM1_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);

//void TIM3_PWM_Init(u16 arr,u16 psc);
#endif
