#ifndef __GLOBAL_H
#define __GLOBAL_H

//extern float temperature1;
//extern float temperature2;
//extern float temperature3;
//extern float average_temp;

//�ֶο��ƽṹ��
//typedef struct {
//	uint8_t hour;
//	uint8_t minutes;
//	uint8_t seconds;
//	uint8_t on_off;
//} relay_Time;

//�ֶο��ƽṹ��
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
////    {1, {8, 0, 0}, {12, 0, 0}},  // �̵��� 1: 8:00 ����, 12:00 �ر�
////    {2, {13, 0, 0}, {17, 0, 0}}, // �̵��� 2: 13:00 ����, 17:00 �ر�
////    {3, {18, 0, 0}, {22, 0, 0}}  // �̵��� 3: 18:00 ����, 22:00 �ر�
////	{4, {13, 0, 0}, {17, 0, 0}}, // �̵��� 2: 13:00 ����, 17:00 �ر�
////    {5, {18, 0, 0}, {22, 0, 0}}  // �̵��� 3: 18:00 ����, 22:00 �ر�
////	{6, {8, 0, 0}, {12, 0, 0}},  // �̵��� 1: 8:00 ����, 12:00 �ر�
////    {7, {13, 0, 0}, {17, 0, 0}}, // �̵��� 2: 13:00 ����, 17:00 �ر�
////    {8, {18, 0, 0}, {22, 0, 0}}  // �̵��� 3: 18:00 ����, 22:00 �ر�
////	{9, {13, 0, 0}, {17, 0, 0}}, // �̵��� 2: 13:00 ����, 17:00 �ر�
////    {10, {18, 0, 0}, {22, 0, 0}}  // �̵��� 3: 18:00 ����, 22:00 �ر�
//};

//ʱ�ؽṹ��
typedef struct {
    uint8_t  relayNo;
    uint16_t time_stop;
	uint16_t time_open;
    uint8_t change_flag;
} Time_Control;

//�¿�
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
	uint8_t temp_choose_flag;//ȡֵ1,2,3(�¶���),4(ƽ���¶�)
	//uint8_t  temp_choose; 
	uint8_t startup_mode;
} Temp_Control;

//��ʱ�ؽṹ��
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
