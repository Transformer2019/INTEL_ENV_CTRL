#ifndef __GLOBAL_H
#define __GLOBAL_H

//extern float temperature1;
//extern float temperature2;
//extern float temperature3;
//extern float average_temp;

//分段控制结构体
//typedef struct {
//	uint8_t hour;
//	uint8_t minutes;
//	uint8_t seconds;
//	uint8_t on_off;
//} relay_Time;

//分段控制结构体
typedef struct {
	uint8_t hour_start;
	uint8_t minutes_start;
	//uint8_t seconds_start;
	uint8_t hour_stop;
	uint8_t minutes_stop;
	//uint8_t seconds_stop;
	uint8_t on_off;
} relay_Time;

typedef struct {
    relay_Time Time1;  
    relay_Time Time2; 
	relay_Time Time3;  
    relay_Time Time4; 
	relay_Time Time5;
} Relay_Time_Seg;

typedef struct {
    uint8_t relayNo;  
    Relay_Time_Seg relay_time_seg;
} RelaySchedule;

//extern RelaySchedule schedules[];

//RelaySchedule schedules[] = {
////    {1, {8, 0, 0}, {12, 0, 0}},  // 继电器 1: 8:00 开启, 12:00 关闭
////    {2, {13, 0, 0}, {17, 0, 0}}, // 继电器 2: 13:00 开启, 17:00 关闭
////    {3, {18, 0, 0}, {22, 0, 0}}  // 继电器 3: 18:00 开启, 22:00 关闭
////	{4, {13, 0, 0}, {17, 0, 0}}, // 继电器 2: 13:00 开启, 17:00 关闭
////    {5, {18, 0, 0}, {22, 0, 0}}  // 继电器 3: 18:00 开启, 22:00 关闭
////	{6, {8, 0, 0}, {12, 0, 0}},  // 继电器 1: 8:00 开启, 12:00 关闭
////    {7, {13, 0, 0}, {17, 0, 0}}, // 继电器 2: 13:00 开启, 17:00 关闭
////    {8, {18, 0, 0}, {22, 0, 0}}  // 继电器 3: 18:00 开启, 22:00 关闭
////	{9, {13, 0, 0}, {17, 0, 0}}, // 继电器 2: 13:00 开启, 17:00 关闭
////    {10, {18, 0, 0}, {22, 0, 0}}  // 继电器 3: 18:00 开启, 22:00 关闭
//};

//时控结构体
typedef struct {
    uint8_t  relayNo;
    uint16_t time_stop;
	uint16_t time_open;
    uint8_t change_flag;
} Time_Control;

//温控
typedef enum {
    temperature1_flag = 0,
    temperature2_flag = 1,
	temperature3_flag = 2,
	average_temp_flag = 3
} Temp_Choose_flag;
typedef struct {
    uint8_t  relayNo;
	uint8_t  max_temp;
    uint8_t  min_temp;
	uint16_t max_nh3;
	uint16_t min_nh3;
	uint8_t temp_choose_flag;//取值1,2,3(温度三),4(平均温度)
	//uint8_t  temp_choose; 
	uint8_t startup_mode;
} Temp_Control;

//温时控结构体
//typedef struct {
//    uint8_t  relayNo;
//    uint16_t time_stop;
//	uint16_t time_open;
//	Temp_Control temp_control;
//    uint8_t change_flag;
//} Time_Temp_Control;


//extern Time_Control       time_control_list[];
//extern Time_Temp_Control  time_temp_control_list[];


typedef struct {
    uint8_t  relayNo;
    uint8_t  relay_mode;
	uint8_t  on_off;
	//uint8_t  max_temp;
    //uint8_t  min_temp;
	Temp_Control  temp_control;
    Time_Control  time_control;
	//Time_Temp_Control  time_temp_control;
	RelaySchedule  time_schedule;
} Relay_Structure;


#endif
