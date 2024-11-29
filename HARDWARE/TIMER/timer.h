#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


extern volatile uint16_t TIM3_Counter_10s;
extern volatile u8 TIM3_flag;

extern volatile uint16_t TIM4_Counter_10s;
extern volatile u8 TIM4_flag;

extern volatile uint16_t TIM5_Counter_10s;
extern volatile u8 TIM5_flag;

extern volatile uint8_t Heartbeat_Counter_1s;
extern volatile u8 Heartbeat_flag;


extern volatile float temperature1;
extern volatile float temperature2;
extern volatile float temperature3;
extern volatile float average_temp;


//void TIM4_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);

//void TIM3_PWM_Init(u16 arr,u16 psc);
#endif
