#ifndef _GUI_H_
#define	_GUI_H_

#include "stm32f10x.h"
#include "global.h"

#define ON 1
#define OFF 0


//主页显示哪个页面
#define HOME_Page 3 // 1,2代表不同主页显示效果，1简单显示；2显示全部传感器信息和变频器输出；3在2的基础上修改不显示温度三

//主页时间显示颜色
#define HOME_Page_time 2 // 1:白色 2：绿色

extern uint8_t ctrl_ui;

extern uint8_t network_flag;
extern uint8_t register_flag;
extern uint8_t mqtt_flag;

//报警参数
volatile extern uint8_t warn_flag;//是否报警

extern uint8_t NH3_warn_flag;
extern uint8_t warn_temp1_flag;//温度一是否参与报警
extern uint8_t warn_temp2_flag;
extern uint8_t warn_temp3_flag;
//extern uint8_t limit_temp_maxvalue;//温度高限值
//extern uint8_t limit_temp_minvalue;
extern uint8_t warn_temp485_flag;
extern uint8_t warn_rh_flag;


extern uint8_t NH3_max;

extern uint8_t limit_temp1_maxvalue;//温度高限值
extern uint8_t limit_temp1_minvalue;

extern uint8_t limit_temp2_maxvalue;//温度高限值
extern uint8_t limit_temp2_minvalue;

extern uint8_t limit_temp3_maxvalue;//温度高限值
extern uint8_t limit_temp3_minvalue;

extern uint8_t limit_temp485_maxvalue;//485温度高限值
extern uint8_t limit_temp485_minvalue;

extern uint8_t limit_rh_maxvalue;//湿度高限值
extern uint8_t limit_rh_minvalue;




//继电器输出
extern volatile u8 relay_Control[2]; //控制继电器


//温度修正参数
extern float temp1_correct;// 
extern float temp2_correct;// 
extern float temp3_correct;//

//IMEI号
extern char imei_no[16];// 

extern Relay_Structure relay_structure[10];

//控制模式选择时不进入设置缓存控制模式数组
extern volatile u8 relay_mode_buf[10];

//风机结构体缓存
extern Relay_Structure relay_structure_temp[10];

//保存风机编号
extern volatile u8 last_index_save;

//变频控制
typedef struct {
	float max_temp;
    float min_temp;
	float voltage_high;
	float voltage_low;
	u8 temp_choose; // 1:温度一 2:温度二 3:温度三 4:平均温度 5:485温度
} Hz_Control;
extern volatile Hz_Control hz_control;
//变频器选择的温度
extern volatile float Hz_temp_choose;
//变频输出
extern volatile u8 out_voltage;


//MQTT配置数据发送标志
extern uint8_t send_CONFIG_INDEX;
extern uint8_t send_CONFIG_FLAG;


//MQTT报警数据发送标志
extern uint8_t send_warn_Flag;

//发送变频器配置数据标志
extern uint8_t send_HZ_Flag;

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
uint8_t time_h_start;
uint8_t time_m_start;
uint8_t time_h_stop;
uint8_t time_m_stop;
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
	
	//主界面返回
	_Main_Back_UI,

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

void Main_Back_UI(u8 page_index,u8 key_val);

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
