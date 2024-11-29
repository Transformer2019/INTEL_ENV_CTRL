#ifndef __NTC_H__
#define __NTC_H__
#include "stm32f10x.h"

// 定义分压电阻值
#define R1 10000.0
// 定义ADC通道
#define ADC_CHANNEL 3

// 温度相关参数
#define B_VALUE 3450
#define REFERENCE_RESISTANCE 10000.0
#define ROOM_TEMPERATURE 25.0

void ADC_Configuration(void);
float Read_Temperature(uint8_t no);

#endif
