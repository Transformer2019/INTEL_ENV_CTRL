#ifndef _GUI_H_
#define	_GUI_H_

#include "stm32f10x.h"
#include "global.h"

#define ON 1
#define OFF 0

extern uint8_t ctrl_ui;

extern uint8_t network_flag;
extern uint8_t register_flag;
extern uint8_t mqtt_flag;

//报警参数
extern uint8_t warn_flag;//是否报警
extern uint8_t warn_temp1_flag;//温度一是否参与报警
extern uint8_t warn_temp2_flag;
extern uint8_t warn_temp3_flag;
extern uint8_t limit_temp_maxvalue;//温度高限值
extern uint8_t limit_temp_minvalue;

extern uint8_t NH3_warn_flag;
extern uint8_t NH3_max;


//温度修正参数
extern float temp1_correct;// 
extern float temp2_correct;// 
extern float temp3_correct;//

//IMEI号
extern char imei_no[16];// 

extern Relay_Structure relay_structure[10];

//变频控制
extern u8 out_voltage;


//MQTT配置数据发送标志
extern uint8_t send_CONFIG_INDEX;
extern uint8_t send_CONFIG_FLAG;


//MQTT报警数据发送标志
extern uint8_t send_warn_Flag;

typedef struct
{
	u8 Cur_Index;//当前索引项
	u8 previous;//上一页
	u8 next;//下一页
	u8 enter;//确认
	u8 back;//返回
	void (*current_operation)(u8,u8);//	当前索引执行的函数(界面)
}Main_Menu;


typedef struct {
uint8_t time_h;
uint8_t time_m;
uint8_t time_s;
} Time_C;

//各界面的索引值
enum
{
	_Main_UI=0,
	_Air_Blower_Option,
	_Hz_Ctrl_Option,
	_Alarm_Option,
	_Time_Option,
	_Data_Change_Option,
	_Iot_Option,
	//_RS485_Option,
	
	//_Air_Blower_Child,
	_Hz_Ctrl_Child,
	_Alarm_Child,
	_Time_Child,
	_Data_Change_Child,
	_Iot_Child,
	//_RS485_Child,
	
	_Air_Blower_Child_1,
	_Air_Blower_Child_2,
	_Air_Blower_Child_3,
	_Air_Blower_Child_4,
	_Air_Blower_Child_5,
	_Air_Blower_Child_6,
	_Air_Blower_Child_7,
	_Air_Blower_Child_8,
	_Air_Blower_Child_9,
	_Air_Blower_Child_10,
	
	_Air_Blower_Child_Ctrl_1,

};

//按键索引值
enum
{
	KEY_NEXT=2,
	KEY_BACK,
	KEY_PREVIOUS,
	KEY_ENTER,
};

void Main_UI(u8 page_index,u8 key_val);
void Main_Menu_Func(u8 page_index,u8 key_val);

uint8_t GUI_Refresh(void);

void Hz_Ctrl_Child(u8 page_index,u8 key_val);
void Alarm_Child(u8 page_index,u8 key_val);
void Time_Child(u8 page_index,u8 key_val);
void Data_Change_Child(u8 page_index,u8 key_val);
void Iot_Child(u8 page_index,u8 key_val);
//void RS485_Child(u8 page_index,u8 key_val);

void Air_Blower_Child_Ctrl(u8 page_index,u8 key_val);

void Air_Blower_Child_Ctrl_1_Fun(u8 page_index,u8 key_val);
	

#endif
