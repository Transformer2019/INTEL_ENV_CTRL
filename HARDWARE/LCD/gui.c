#include "gui.h"
#include "UI.h"
#include "key.h"
#include "lcd.h"
#include "delay.h"
#include "timer.h"
#include "rtc.h"
#include "GP8201S.h"
#include "pic.h"
#include "mb_hook.h"
//#include "home.h"

//#pragma pack(4) 

Relay_Structure relay_structure[10] ={{.relayNo=1,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=2,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=3,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=4,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=5,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=6,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=7,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=8,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=9,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=10,.relay_mode=0,.no_Ctrl.all_open_or_close=0,.temp_control.max_temp=30.0,.temp_control.min_temp=20.0,.temp_control.max_nh3=20,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
};

//变频控制结构体
volatile Hz_Control hz_control={
	.max_temp=35.00,
	.min_temp=15.00,
	.voltage_high=10.0,
	.voltage_low=0.0,
	.temp_choose=1
};

//控制模式选择时不进入设置缓存控制模式数组
volatile u8 relay_mode_buf[10]={0};

//保存风机编号
volatile u8 last_index_save=0;

//风机结构体缓存
Relay_Structure relay_structure_temp[10]={0};


//变频器选择的温度
volatile float Hz_temp_choose=99;

//变频输出
volatile u8 out_voltage=0;


//#pragma pack()    //取消自定义字节对齐方式


//报警参数
volatile uint8_t warn_flag=0;
uint8_t NH3_warn_flag=0;
uint8_t warn_temp1_flag=0;
uint8_t warn_temp2_flag=0;
uint8_t warn_temp3_flag=0;
//uint8_t limit_temp_maxvalue=50;
//uint8_t limit_temp_minvalue=0;
uint8_t warn_temp485_flag=0;

uint8_t warn_rh_flag=0;


uint8_t NH3_max=13;

uint8_t limit_temp1_maxvalue=50;//温度高限值
uint8_t limit_temp1_minvalue=0;
uint8_t limit_temp2_maxvalue=50;//温度高限值
uint8_t limit_temp2_minvalue=0;
uint8_t limit_temp3_maxvalue=50;//温度高限值
uint8_t limit_temp3_minvalue=0;

uint8_t limit_temp485_maxvalue=50;//485温度高限值
uint8_t limit_temp485_minvalue=0;

uint8_t limit_rh_maxvalue=70;//湿度高限值
uint8_t limit_rh_minvalue=50;



volatile u8 relay_Control[2] = {0x00, 0x00}; //控制继电器


float temp1_correct=0.0;// 
float temp2_correct=0.0;// 
float temp3_correct=0.0;// 

char imei_no[16]={0};

static u8 func_index=_Main_UI;//当前页面索引值
static u8 last_index=_Main_UI;//上一个界面索引值
static void (*current_operation_func)(u8,u8);//定义一个函数指针

uint8_t ctrl_ui=0;


//设置个全局变量用于判断是否需要刷新页面（温控，温时控。。。）
static u8 fresh_flag_1=0;//风机一
static u8 fresh_flag_2=0;//风机二.
static u8 fresh_flag_3=0;//风机一
static u8 fresh_flag_4=0;//风机二.
static u8 fresh_flag_5=0;//风机一
static u8 fresh_flag_6=0;//风机二.
static u8 fresh_flag_7=0;//风机一
static u8 fresh_flag_8=0;//风机二.
static u8 fresh_flag_9=0;//风机一
static u8 fresh_flag_10=0;//风机二.

//判断是否进入风机配置界面还是选择界面
static u8 enter_config_flag_1=0;//风机一
static u8 enter_config_flag_2=0;//风机二
static u8 enter_config_flag_3=0;//风机一
static u8 enter_config_flag_4=0;//风机二
static u8 enter_config_flag_5=0;//风机一
static u8 enter_config_flag_6=0;//风机二
static u8 enter_config_flag_7=0;//风机一
static u8 enter_config_flag_8=0;//风机二
static u8 enter_config_flag_9=0;//风机一
static u8 enter_config_flag_10=0;//风机二


static u8 cur_pos0=1;
static u8 cur_pos1=1;
static u8 cur_pos2=1;
static u8 cur_pos3=1;
static u8 cur_pos4=1;

uint8_t send_CONFIG_INDEX=0;

uint8_t send_CONFIG_FLAG=0;


uint8_t send_warn_Flag=0;

//发送变频器配置数据标志
uint8_t send_HZ_Flag=0;

void fresh_page(){
	LCD_Fill(0,66,448,287,BLACK);
}

//索引表
Main_Menu table[25]=
{
	//Cur_Index,   previous,   next,   enter,   back,   (*current_operation)(u8,u8)
	//主界面
	{_Main_UI,_Main_UI,_Main_UI,_Air_Blower_Option,_Main_Back_UI,Main_UI},
	
	//主菜单
    {_Air_Blower_Option,_Iot_Option,_Hz_Ctrl_Option,_Air_Blower_Child_1,_Main_UI,Main_Menu_Func},//普通风机
	{_Hz_Ctrl_Option,_Air_Blower_Option,_Alarm_Option,_Hz_Ctrl_Child,_Main_UI,Main_Menu_Func},//变频控制
	{_Alarm_Option,_Hz_Ctrl_Option,_Time_Option,_Alarm_Child,_Main_UI,Main_Menu_Func},//报警参数
	{_Time_Option,_Alarm_Option,_Data_Change_Option,_Time_Child,_Main_UI,Main_Menu_Func},//设置时间
	{_Data_Change_Option,_Time_Option,_Iot_Option,_Data_Change_Child,_Main_UI,Main_Menu_Func},//修正参数
	{_Iot_Option,_Data_Change_Option,_Air_Blower_Option,_Iot_Child,_Main_UI,Main_Menu_Func},//智能物联
	//{_RS485_Option,_Iot_Option,_Air_Blower_Option,_RS485_Child,_Main_UI,Main_Menu_Func},//智能物联
	
	//子菜单
	//{_Air_Blower_Child,_Air_Blower_Child,_Air_Blower_Child,_Air_Blower_Child_1,_Air_Blower_Option,Air_Blower_Child},
	{_Hz_Ctrl_Child,_Hz_Ctrl_Child,_Hz_Ctrl_Child,_Hz_Ctrl_Child,_Hz_Ctrl_Option,Hz_Ctrl_Child},//7
	{_Alarm_Child,_Alarm_Child,_Alarm_Child,_Alarm_Child,_Alarm_Option,Alarm_Child},//8
	{_Time_Child,_Time_Child,_Time_Child,_Time_Child,_Time_Option,Time_Child},//9
	{_Data_Change_Child,_Data_Change_Child,_Data_Change_Child,_Data_Change_Child,_Data_Change_Option,Data_Change_Child},//10
	{_Iot_Child,_Iot_Child,_Iot_Child,_Iot_Child,_Iot_Option,Iot_Child},//11
    //{_RS485_Child,_RS485_Child,_RS485_Child,_RS485_Child,_RS485_Option,RS485_Child},//11

#if (ROAD_COUNT==10)
	//风机1-10
	{_Air_Blower_Child_1,_Air_Blower_Child_10,_Air_Blower_Child_2,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},//12
	{_Air_Blower_Child_2,_Air_Blower_Child_1,_Air_Blower_Child_3,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_3,_Air_Blower_Child_2,_Air_Blower_Child_4,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_4,_Air_Blower_Child_3,_Air_Blower_Child_5,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_5,_Air_Blower_Child_4,_Air_Blower_Child_6,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_6,_Air_Blower_Child_5,_Air_Blower_Child_7,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_7,_Air_Blower_Child_6,_Air_Blower_Child_8,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_8,_Air_Blower_Child_7,_Air_Blower_Child_9,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_9,_Air_Blower_Child_8,_Air_Blower_Child_10,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_10,_Air_Blower_Child_9,_Air_Blower_Child_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},//21
#endif 	
	
#if (ROAD_COUNT==8)
	//风机1-10
	{_Air_Blower_Child_1,_Air_Blower_Child_8,_Air_Blower_Child_2,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},//12
	{_Air_Blower_Child_2,_Air_Blower_Child_1,_Air_Blower_Child_3,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_3,_Air_Blower_Child_2,_Air_Blower_Child_4,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_4,_Air_Blower_Child_3,_Air_Blower_Child_5,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_5,_Air_Blower_Child_4,_Air_Blower_Child_6,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_6,_Air_Blower_Child_5,_Air_Blower_Child_7,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_7,_Air_Blower_Child_6,_Air_Blower_Child_8,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_8,_Air_Blower_Child_7,_Air_Blower_Child_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
#endif 	

#if (ROAD_COUNT==4)
	//风机1-10
	{_Air_Blower_Child_1,_Air_Blower_Child_4,_Air_Blower_Child_2,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},//12
	{_Air_Blower_Child_2,_Air_Blower_Child_1,_Air_Blower_Child_3,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_3,_Air_Blower_Child_2,_Air_Blower_Child_4,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
	{_Air_Blower_Child_4,_Air_Blower_Child_3,_Air_Blower_Child_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Option,Air_Blower_Child_Ctrl},
#endif 
	
	
	{_Air_Blower_Child_Ctrl_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Child_1,Air_Blower_Child_Ctrl_1_Fun},//10-22 8-20 4-16
	 
	//主界面返回
	{_Main_Back_UI,_Main_Back_UI,_Main_Back_UI,_Main_Back_UI,_Main_UI,Main_Back_UI}//10-23 8-21 4-17
	
};


	
/*
函数功能：刷新界面
参数：无
返回值：无
*/
uint8_t GUI_Refresh(void)
{
	
	u8 key_val=Get_KEY_Value();
	if(key_val!=0)//只有按键按下才刷屏
	{

	    if(fresh_flag_1||fresh_flag_2||fresh_flag_3||fresh_flag_4||fresh_flag_5
			||fresh_flag_6||fresh_flag_7||fresh_flag_8||fresh_flag_9||fresh_flag_10
		)fresh_page();//判断是否需要刷新部分页面
		
		last_index=func_index;//更新上一界面索引值
		switch(key_val)
		{
			case KEY_PREVIOUS: func_index=table[func_index].previous;//更新索引值
//					printf("key_val:%d\n", key_val);
//			        printf("func_index:%d\n", func_index);
					break;
			case KEY_ENTER: func_index=table[func_index].enter;//更新索引值
//					printf("key_val:%d\n", key_val);
//			        printf("func_index:%d\n", func_index);
					break;
			case KEY_NEXT:func_index=table[func_index].next;//更新索引值
//					printf("key_val:%d\n", key_val);
//			        printf("func_index:%d\n", func_index);
					break;
			case KEY_BACK:
				//按返回键后，如果修改风机控制模式和或参数，保存新的参数
				//如果用户远程操作风机参数的话，并且用在设备正在操作风机参数，此时设备操作优先级高
				if(table[func_index].back==_Air_Blower_Child_1){
					
					// 检查是否发生了变化
					//printf("KEY_BACK\n");
					if (memcmp(relay_structure, relay_structure_temp, sizeof(relay_structure)) != 0) {
						send_CONFIG_INDEX = last_index_save-12;
						send_CONFIG_FLAG=1;
						memcpy(relay_structure, relay_structure_temp, sizeof(relay_structure));
						//printf("---%d---\n",send_CONFIG_INDEX);
					}
				}
				func_index=table[func_index].back;//更新索引值

					break;
			default:break;
		}
	    //delay_ms(5);
		
		if(func_index == 0 && (last_index == 1 || last_index == 2 || last_index == 3 || last_index == 4 || last_index == 5 || last_index == 6)){LCD_Clear(HOME_BACK);}//从主菜单返回主页
		if(func_index == 1 && last_index == 0){LCD_Clear(BLACK);}//从主页进入主菜单
		if(func_index == 12 && last_index == 1){LCD_Clear_upper_tail(BLACK);}//从主菜单进入风机一
		
		if(func_index == 7 && last_index == 2){LCD_Clear_upper_tail(BLACK);}//从主菜单进入变频控制
		if(func_index == 2 && last_index == 7){LCD_Clear_upper_tail(BLACK);}//从变频控制返回主菜单
		
		if(func_index == 8 && last_index == 3){LCD_Clear_upper_tail(BLACK);}//从主菜单进入报警参数
		if(func_index == 3 && last_index == 8){LCD_Clear_upper_tail(BLACK);}//从报警参数返回主菜单
		
		if(func_index == 9 && last_index == 4){LCD_Clear_upper_tail(BLACK);}//从主菜单进入时间设置
		if(func_index == 4 && last_index == 9){LCD_Clear_upper_tail(BLACK);}//从时间设置返回主菜单	
		
		if(func_index == 10 && last_index == 5){LCD_Clear_upper_tail(BLACK);}//从主菜单进入修正参数
		if(func_index == 5 && last_index == 10){LCD_Clear_upper_tail(BLACK);}//从修正参数返回主菜单
		
		if(func_index == 11 && last_index == 6){LCD_Clear_upper_tail(BLACK);}//从主菜单进入智能物联
		if(func_index == 6 && last_index == 11){LCD_Clear_upper_tail(BLACK);}//从智能物联返回主菜单
		
		#if (ROAD_COUNT==10)
		if(func_index == 1 && last_index == 12){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 13){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 14){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 15){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 16){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 17){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 18){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 19){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 20){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 21){LCD_Clear_upper_tail(BLACK);}
		#endif
		
		#if (ROAD_COUNT==8)
		if(func_index == 1 && last_index == 12){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 13){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 14){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 15){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 16){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 17){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 18){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 19){LCD_Clear_upper_tail(BLACK);}
		#endif
		
		#if (ROAD_COUNT==4)
		if(func_index == 1 && last_index == 12){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 13){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 14){LCD_Clear_upper_tail(BLACK);}
		if(func_index == 1 && last_index == 15){LCD_Clear_upper_tail(BLACK);}
		#endif
		
		if(func_index == (23-(10-ROAD_COUNT)) && last_index == 0){LCD_Clear(HOME_BACK);}
		if(func_index == 0 && last_index == (23-(10-ROAD_COUNT))){LCD_Clear(HOME_BACK);}
		
		#if (ROAD_COUNT==10)
        if(func_index == (22-(10-ROAD_COUNT)) && (last_index == 12||last_index == 13||last_index == 14||last_index == 15||last_index == 16||last_index == 17||last_index == 18||last_index == 19||last_index == 20||last_index == 21)){LCD_Clear_upper_tail(BLACK);}//从风机一控制类型进入
		if((func_index == 12||func_index == 13||func_index == 14||func_index == 15||func_index == 16||func_index == 17||func_index == 18||func_index == 19||func_index == 20||func_index == 21) && last_index == (22-(10-ROAD_COUNT)))
		{
			LCD_Clear_upper_tail(BLACK);
			enter_config_flag_1=0;fresh_flag_1=0;
			enter_config_flag_2=0;fresh_flag_2=0;
			enter_config_flag_3=0;fresh_flag_3=0;
			enter_config_flag_4=0;fresh_flag_4=0;
			enter_config_flag_5=0;fresh_flag_5=0;
		    enter_config_flag_6=0;fresh_flag_6=0;
			enter_config_flag_7=0;fresh_flag_7=0;
			enter_config_flag_8=0;fresh_flag_8=0;
			enter_config_flag_9=0;fresh_flag_9=0;
			enter_config_flag_10=0;fresh_flag_10=0;
			cur_pos1=1;cur_pos2=1;cur_pos3=1;cur_pos4=1;
		}//刷新带清除进入配置界面标志,清除刷新标志
		#endif
		
		#if (ROAD_COUNT==8)
        if(func_index == (22-(10-ROAD_COUNT)) && (last_index == 12||last_index == 13||last_index == 14||last_index == 15||last_index == 16||last_index == 17||last_index == 18||last_index == 19)){LCD_Clear_upper_tail(BLACK);}//从风机一控制类型进入
		if((func_index == 12||func_index == 13||func_index == 14||func_index == 15||func_index == 16||func_index == 17||func_index == 18||func_index == 19) && last_index == (22-(10-ROAD_COUNT)))
		{
			LCD_Clear_upper_tail(BLACK);
			enter_config_flag_1=0;fresh_flag_1=0;
			enter_config_flag_2=0;fresh_flag_2=0;
			enter_config_flag_3=0;fresh_flag_3=0;
			enter_config_flag_4=0;fresh_flag_4=0;
			enter_config_flag_5=0;fresh_flag_5=0;
		    enter_config_flag_6=0;fresh_flag_6=0;
			enter_config_flag_7=0;fresh_flag_7=0;
			enter_config_flag_8=0;fresh_flag_8=0;
			enter_config_flag_9=0;fresh_flag_9=0;
			enter_config_flag_10=0;fresh_flag_10=0;
			cur_pos1=1;cur_pos2=1;cur_pos3=1;cur_pos4=1;
		}//刷新带清除进入配置界面标志,清除刷新标志
		#endif
		
		#if (ROAD_COUNT==4)
        if(func_index == (22-(10-ROAD_COUNT)) && (last_index == 12||last_index == 13||last_index == 14||last_index == 15)){LCD_Clear_upper_tail(BLACK);}//从风机一控制类型进入
		if((func_index == 12||func_index == 13||func_index == 14||func_index == 15) && last_index == (22-(10-ROAD_COUNT)))
		{
			LCD_Clear_upper_tail(BLACK);
			enter_config_flag_1=0;fresh_flag_1=0;
			enter_config_flag_2=0;fresh_flag_2=0;
			enter_config_flag_3=0;fresh_flag_3=0;
			enter_config_flag_4=0;fresh_flag_4=0;
			enter_config_flag_5=0;fresh_flag_5=0;
		    enter_config_flag_6=0;fresh_flag_6=0;
			enter_config_flag_7=0;fresh_flag_7=0;
			enter_config_flag_8=0;fresh_flag_8=0;
			enter_config_flag_9=0;fresh_flag_9=0;
			enter_config_flag_10=0;fresh_flag_10=0;
			cur_pos1=1;cur_pos2=1;cur_pos3=1;cur_pos4=1;
		}//刷新带清除进入配置界面标志,清除刷新标志
		#endif
		
//		if(1){

			if(func_index == 1 || func_index == 2 || func_index == 3 || func_index == 4 || func_index == 5 || func_index == 6){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				//菜单选择
					showhanzi_1(177,0,0,0);  
					showhanzi_1(209,0,1,0);  
					showhanzi_1(241,0,2,0);  
					showhanzi_1(273,0,3,0);
				LCD_Fill(0,lcddev.height-32,lcddev.width,lcddev.height,WHITE);
			}
			if(func_index == 12){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				//风机菜单
				showhanzi_1(177,0,6,0);  
				showhanzi_1(209,0,7,0);  
				showhanzi_1(241,0,0,0);  
				showhanzi_1(273,0,1,0);  
				LCD_Fill(0,lcddev.height-32,lcddev.width,lcddev.height,WHITE);
			}

			if(func_index == (22-(10-ROAD_COUNT))){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				showhanzi(0,35,10);showhanzi(32,35,11);showhanzi(64,35,36);showhanzi(96,35,37);showhanzi(128,35,44);//控制类型：
				LCD_Fill(0,lcddev.height-32,lcddev.width,lcddev.height,WHITE);
				switch(last_index){
					#if (ROAD_COUNT==10)
					case 12://风机1号
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,26,1); showhanzi_1(273,0,27,1);
						break;
					case 13:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,28,1); showhanzi_1(273,0,27,1);
						break;
					case 14:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,29,1); showhanzi_1(273,0,27,1);
						break;
					case 15:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,30,1); showhanzi_1(273,0,27,1);
						break;
					case 16:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,31,1); showhanzi_1(273,0,27,1);
						break;
					case 17:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,32,1); showhanzi_1(273,0,27,1);
						break;
					case 18:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,33,1); showhanzi_1(273,0,27,1);
						break;
					case 19:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,34,1); showhanzi_1(273,0,27,1);
						break;
					case 20:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,35,1); showhanzi_1(273,0,27,1);
						break;
					case 21:
						showhanzi_1(177,0,6,1);  
						showhanzi_1(209,0,7,1); 
						showdigit_color(241,0,1,BLACK,WHITE);
						showdigit_color(257,0,0,BLACK,WHITE);
						showhanzi_1(273,0,27,1);
						break;
					#endif
					
					#if (ROAD_COUNT==8)
					case 12://风机1号
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,26,1); showhanzi_1(273,0,27,1);
						break;
					case 13:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,28,1); showhanzi_1(273,0,27,1);
						break;
					case 14:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,29,1); showhanzi_1(273,0,27,1);
						break;
					case 15:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,30,1); showhanzi_1(273,0,27,1);
						break;
					case 16:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,31,1); showhanzi_1(273,0,27,1);
						break;
					case 17:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,32,1); showhanzi_1(273,0,27,1);
						break;
					case 18:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,33,1); showhanzi_1(273,0,27,1);
						break;
					case 19:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,34,1); showhanzi_1(273,0,27,1);
						break;
					#endif
					
					#if (ROAD_COUNT==4)
					case 12://风机1号
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,26,1); showhanzi_1(273,0,27,1);
						break;
					case 13:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,28,1); showhanzi_1(273,0,27,1);
						break;
					case 14:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,29,1); showhanzi_1(273,0,27,1);
						break;
					case 15:
						showhanzi_1(177,0,6,1);  showhanzi_1(209,0,7,1); showhanzi_1(241,0,30,1); showhanzi_1(273,0,27,1);
						break;
					#endif
					default:break;				
				
				}
			}
		
			
			if(func_index == 7){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				//变频控制
				showhanzi_1(177,0,8,1);  
				showhanzi_1(209,0,9,1);  
				showhanzi_1(241,0,10,1);  
				showhanzi_1(273,0,11,1);
			}
			
			if(func_index == 8){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				//报警参数
				showhanzi_1(177,0,12,1);  
				showhanzi_1(209,0,13,1);  
				showhanzi_1(241,0,14,1);  
				showhanzi_1(273,0,15,1);
			}
			if(func_index == 9){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				//时间设置
				showhanzi_1(177,0,16,1);  
				showhanzi_1(209,0,17,1);  
				showhanzi_1(241,0,18,1);  
				showhanzi_1(273,0,19,1);
			}
			if(func_index == 10){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				//修正参数
				showhanzi_1(177,0,20,1);  
				showhanzi_1(209,0,21,1);  
				showhanzi_1(241,0,14,1);  
				showhanzi_1(273,0,15,1);
			}
			if(func_index == 11){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				//智能
				showhanzi_1(177,0,22,1);  
				showhanzi_1(209,0,23,1);  
				showhanzi_1(241,0,24,1);  
				showhanzi_1(273,0,25,1);
			}

	}
	current_operation_func=table[func_index].current_operation;
	(*current_operation_func)(func_index,key_val);//执行当前索引对应的函数
    return func_index;
}


/*
函数功能：显示主界面
参数：u8 page_index,u8 key_val
返回值：无
*/

#if (HOME_Page==1)
void Main_UI(u8 page_index,u8 key_val)
{
	u8 y1=calendar.w_year/1000; u8 y2=(calendar.w_year/100)%10; u8 y3=(calendar.w_year/10)%10; u8 y4=calendar.w_year%10;
	u8 mh1=calendar.w_month/10; u8 mh2=calendar.w_month%10;
	u8 d1=calendar.w_date/10; u8 d2=calendar.w_date%10;
	u8 h1=calendar.hour/10; u8 h2=calendar.hour%10;
	u8 m1=calendar.min/10; u8 m2=calendar.min%10;
	u8 s1=calendar.sec/10; u8 s2=calendar.sec%10;
	
	
	
	showdigit_color(20,96,y1,WHITE,HOME_BACK);showdigit_color(36,96,y2,WHITE,HOME_BACK);showdigit_color(52,96,y3,WHITE,HOME_BACK);showdigit_color(68,96,y4,WHITE,HOME_BACK);
	showdigit_color(84,96,11,WHITE,HOME_BACK);
	showdigit_color(100,96,mh1,WHITE,HOME_BACK);showdigit_color(116,96,mh2,WHITE,HOME_BACK);
	showdigit_color(132,96,11,WHITE,HOME_BACK);
	showdigit_color(148,96,d1,WHITE,HOME_BACK);showdigit_color(164,96,d2,WHITE,HOME_BACK);
	
	showdigit_color(60,168,h1,WHITE,HOME_BACK);showdigit_color(76,168,h2,WHITE,HOME_BACK);
	showdigit_color(92,168,12,WHITE,HOME_BACK);
	showdigit_color(108,168,m1,WHITE,HOME_BACK);showdigit_color(124,168,m2,WHITE,HOME_BACK);
//	showdigit_color(96,150,12,GREEN,BLACK);
//	showdigit_color(112,150,s1,GREEN,BLACK);showdigit_color(128,150,s2,GREEN,BLACK);

	//根据network_flag判断是否联网
	if(network_flag){
		//网络已连接
//		showhanzi_1(0,0,75,1);  
//		showhanzi_1(32,0,76,1);  
//		showhanzi_1(64,0,72,1);  
//		showhanzi_1(96,0,73,1); 
//		showhanzi_1(128,0,74,1);
		
		//Gui_Drawbmp_logo(0,0,gImage_88);
		
		LCD_Fill(128,0,436,31,HOME_BACK);
		show_4g(437,0,0);
		show_4g(453,0,1);
		LCD_Fill(469,0,480,31,HOME_BACK);
	}else{
		//连接网络中
//		showhanzi_1(0,0,73,1);   
//		showhanzi_1(32,0,74,1);  
//		showhanzi_1(64,0,75,1);  
//		showhanzi_1(96,0,76,1); 
//		showhanzi_1(128,0,77,1); 
		
		//Gui_Drawbmp_logo(0,0,gImage_88);
		LCD_Fill(0,0,320,31,HOME_BACK);
		showhanzi_color(320,0,73,FONT_TOP,HOME_BACK);   
		showhanzi_color(352,0,74,FONT_TOP,HOME_BACK);  
		showhanzi_color(384,0,75,FONT_TOP,HOME_BACK);  
		showhanzi_color(416,0,76,FONT_TOP,HOME_BACK);  
		showhanzi_color(448,0,77,FONT_TOP,HOME_BACK); 
		
	}

	LCD_Fill(0,38,480,40,LINE);
	LCD_Fill(0,246,480,248,LINE);
    //显示温度
	showhanzi_color(224,60,111,WHITE,HOME_BACK);  
	showhanzi_color(256,60,112,WHITE,HOME_BACK);  
	showhanzi_color(288,60,113,WHITE,HOME_BACK); 
	showdigit_color(320,60,12,WHITE,HOME_BACK);
	
	
	showhanzi_color(224,132,111,WHITE,HOME_BACK);  
	showhanzi_color(256,132,112,WHITE,HOME_BACK);  
	showhanzi_color(288,132,114,WHITE,HOME_BACK); 
	showdigit_color(320,132,12,WHITE,HOME_BACK);
	
	
	//showhanzi_color(336,60,39,GREEN,BLACK);  
//	showhanzi_color(368,60,66,GREEN,BLACK);  
//	showhanzi_color(400,60,65,GREEN,BLACK); 
	
	
	//变频器 氨气值
	showhanzi_color(224,204,109,WHITE,HOME_BACK);  
	showhanzi_color(256,204,110,WHITE,HOME_BACK);  
	showhanzi_color(288,204,107,WHITE,HOME_BACK); 
	showdigit_color(320,204,12,WHITE,HOME_BACK);



//	uint8_t t3_1 = (unsigned int)((temperature3+temp3_correct)/10) % 10;
//	uint8_t t3_2 = (unsigned int)(temperature3+temp3_correct) % 10;
//	uint8_t t3_3 = (unsigned int)((temperature3+temp3_correct)*10) % 10;
	
	//如果温度小于-5度，显示断开，表示温度传感器没连接
	if(temperature1<0 || temperature1>90){
		showhanzi_color(336,60,95,FONT_MID,HOME_BACK);showhanzi_color(368,60,108,FONT_MID,HOME_BACK);LCD_Fill(400,60,420,92,HOME_BACK);
	}else{
		
		uint8_t t1_1 = (unsigned int)((temperature1+temp1_correct)/10) % 10;
		uint8_t t1_2 = (unsigned int)(temperature1+temp1_correct) % 10;
		uint8_t t1_3 = (unsigned int)((temperature1+temp1_correct)*10) % 10;
		LCD_Fill(336,60,352,92,HOME_BACK);
		showdigit_color(352,60,t1_1,FONT_MID,HOME_BACK);
		showdigit_color(368,60,t1_2,FONT_MID,HOME_BACK);
		showdigit_color(384,60,10,FONT_MID,HOME_BACK);
		showdigit_color(400,60,t1_3,FONT_MID,HOME_BACK);
	}
	
	if(temperature2<0 || temperature2>90){
		showhanzi_color(336,132,95,FONT_MID,HOME_BACK);showhanzi_color(368,132,108,FONT_MID,HOME_BACK);LCD_Fill(400,132,420,164,HOME_BACK);
	}else{
		uint8_t t2_1 = (unsigned int)((temperature2+temp2_correct)/10) % 10;
		uint8_t t2_2 = (unsigned int)(temperature2+temp2_correct) % 10;
		uint8_t t2_3 = (unsigned int)((temperature2+temp2_correct)*10) % 10;
		LCD_Fill(336,132,351,164,HOME_BACK);
		showdigit_color(352,132,t2_1,FONT_MID,HOME_BACK);
		showdigit_color(368,132,t2_2,FONT_MID,HOME_BACK);
		showdigit_color(384,132,10,FONT_MID,HOME_BACK);
		showdigit_color(400,132,t2_3,FONT_MID,HOME_BACK);
	}
	
//	if(temperature3<-5){
//		showhanzi_color(336,132,115,UNDET,BLACK);showhanzi_color(368,132,88,UNDET,BLACK);
//	}else{
//		showdigit_color(336,132,t3_1,UNDET,BLACK);
//		showdigit_color(352,132,t3_2,UNDET,BLACK);
//		showdigit_color(368,132,10,UNDET,BLACK);
//		showdigit_color(384,132,t3_3,UNDET,BLACK);
//	}
//	
	
	
//	if(out_voltage==10){
//		showdigit_color(336,132,0,UNDET,BLACK);
//		showdigit_color(352,132,1,UNDET,BLACK);
//		showdigit_color(368,132,14,UNDET,BLACK);
//	}else{
//		showdigit_color(352,132,out_voltage,UNDET,BLACK);
//		showdigit_color(368,132,14,UNDET,BLACK);
//	}
	

	

	if(send_NH3>100 || send_NH3<0){
       showhanzi_color(336,204,95,FONT_MID,HOME_BACK);
		showhanzi_color(368,204,108,FONT_MID,HOME_BACK);
		LCD_Fill(400,204,468,236,HOME_BACK);
	}else if(send_NH3>=10&& send_NH3<100){
		LCD_Fill(336,204,351,236,HOME_BACK);
		showdigit_color(352,204,send_NH3 / 10,FONT_MID,HOME_BACK);
		showdigit_color(368,204,send_NH3 % 10,FONT_MID,HOME_BACK);
		LCD_Fill(384,204,405,236,HOME_BACK);
		showdigit_color(406,204,16,FONT_MID,HOME_BACK);
		showdigit_color(422,204,16,FONT_MID,HOME_BACK);
		showdigit_color(438,204,17,FONT_MID,HOME_BACK);
	}else if(send_NH3>=0&& send_NH3<10){
		LCD_Fill(336,204,367,236,HOME_BACK);
		showdigit_color(368,204,send_NH3,FONT_MID,HOME_BACK);
		LCD_Fill(384,204,405,236,HOME_BACK);
		showdigit_color(406,204,16,FONT_MID,HOME_BACK);
		showdigit_color(422,204,16,FONT_MID,HOME_BACK);
		showdigit_color(438,204,17,FONT_MID,HOME_BACK);
	}

	//根据relay_structure[10]数组判断风机1-10的开关状态
	showhanzi_color(12,288,78,WHITE,HOME_BACK);
	showhanzi_color(59,288,79,WHITE,HOME_BACK);
	showhanzi_color(106,288,80,WHITE,HOME_BACK);
	showhanzi_color(153,288,81,WHITE,HOME_BACK);
	showhanzi_color(200,288,82,WHITE,HOME_BACK);
	showhanzi_color(247,288,83,WHITE,HOME_BACK);
	showhanzi_color(294,288,84,WHITE,HOME_BACK);
	showhanzi_color(341,288,85,WHITE,HOME_BACK);
	showhanzi_color(388,288,86,WHITE,HOME_BACK);
	showhanzi_color(435,288,87,WHITE,HOME_BACK);

//	if(relay_structure[0].on_off)gui_circle(28,303,GREEN,16,1);else gui_circle(28,303,RED,16,1);
//	if(relay_structure[1].on_off)gui_circle(75,303,GREEN,16,1);else gui_circle(75,303,RED,16,1);
//	if(relay_structure[2].on_off)gui_circle(122,303,GREEN,16,1);else gui_circle(122,303,RED,16,1);
//	if(relay_structure[3].on_off)gui_circle(169,303,GREEN,16,1);else gui_circle(169,303,RED,16,1);
//	if(relay_structure[4].on_off)gui_circle(216,303,GREEN,16,1);else gui_circle(216,303,RED,16,1);
//	if(relay_structure[5].on_off)gui_circle(263,303,GREEN,16,1);else gui_circle(263,303,RED,16,1);
//	if(relay_structure[6].on_off)gui_circle(310,303,GREEN,16,1);else gui_circle(310,303,RED,16,1);
//	if(relay_structure[7].on_off)gui_circle(357,303,GREEN,16,1);else gui_circle(357,303,RED,16,1);
//	if(relay_structure[8].on_off)gui_circle(404,303,GREEN,16,1);else gui_circle(404,303,RED,16,1);
//	if(relay_structure[9].on_off)gui_circle(451,303,GREEN,16,1);else gui_circle(451,303,RED,16,1);

    //printf("ctrl_ui:%d",ctrl_ui);
	
	//relay_structure[0].on_off 
	if(relay_Control[1] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(12,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(12,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(12,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(12,253,gImage_4);
	}else Gui_Drawbmp16(12,253,gImage_1);
	//relay_structure[1].on_off
	if(relay_Control[1] & 0x40){
		if(ctrl_ui==0)Gui_Drawbmp16(59,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(59,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(59,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(59,253,gImage_4);
	}else Gui_Drawbmp16(59,253,gImage_1);
	if(relay_Control[1] & 0x20){
		if(ctrl_ui==0)Gui_Drawbmp16(106,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(106,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(106,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(106,253,gImage_4);
	}else Gui_Drawbmp16(106,253,gImage_1);
	if(relay_Control[1] & 0x10){
		if(ctrl_ui==0)Gui_Drawbmp16(153,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(153,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(153,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(153,253,gImage_4);
	}else Gui_Drawbmp16(153,253,gImage_1);
	if(relay_Control[1] & 0x08){
		if(ctrl_ui==0)Gui_Drawbmp16(200,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(200,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(200,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(200,253,gImage_4);
	}else Gui_Drawbmp16(200,253,gImage_1);
	if(relay_Control[1] & 0x04){
		if(ctrl_ui==0)Gui_Drawbmp16(247,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(247,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(247,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(247,253,gImage_4);
	}else Gui_Drawbmp16(247,253,gImage_1);
	if(relay_Control[1] & 0x02){
		if(ctrl_ui==0)Gui_Drawbmp16(294,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(294,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(294,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(294,253,gImage_4);
	}else Gui_Drawbmp16(294,253,gImage_1);
	if(relay_Control[0] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(341,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(341,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(341,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(341,253,gImage_4);
	}else Gui_Drawbmp16(341,253,gImage_1);
	if(relay_Control[0] & 0x40){
		if(ctrl_ui==0)Gui_Drawbmp16(388,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(388,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(388,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(388,253,gImage_4);
	}else Gui_Drawbmp16(388,253,gImage_1);
	if(relay_Control[0] & 0x20){
		if(ctrl_ui==0)Gui_Drawbmp16(435,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(435,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(435,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(435,253,gImage_4);
	}else Gui_Drawbmp16(435,253,gImage_1);
	
}
#endif

#if (HOME_Page==2)
void Main_UI(u8 page_index,u8 key_val)
{
	u8 y1=calendar.w_year/1000; u8 y2=(calendar.w_year/100)%10; u8 y3=(calendar.w_year/10)%10; u8 y4=calendar.w_year%10;
	u8 mh1=calendar.w_month/10; u8 mh2=calendar.w_month%10;
	u8 d1=calendar.w_date/10; u8 d2=calendar.w_date%10;
	u8 h1=calendar.hour/10; u8 h2=calendar.hour%10;
	u8 m1=calendar.min/10; u8 m2=calendar.min%10;
	u8 s1=calendar.sec/10; u8 s2=calendar.sec%10;
	
#if(HOME_Page_time==1)	
	showdigit_color(0,0,y1,WHITE,HOME_BACK);showdigit_color(16,0,y2,WHITE,HOME_BACK);showdigit_color(32,0,y3,WHITE,HOME_BACK);showdigit_color(48,0,y4,WHITE,HOME_BACK);
	showdigit_color(64,0,11,WHITE,HOME_BACK);
	showdigit_color(80,0,mh1,WHITE,HOME_BACK);showdigit_color(96,0,mh2,WHITE,HOME_BACK);
	showdigit_color(112,0,11,WHITE,HOME_BACK);
	showdigit_color(128,0,d1,WHITE,HOME_BACK);showdigit_color(144,0,d2,WHITE,HOME_BACK);
	
	showdigit_color(200,0,h1,WHITE,HOME_BACK);showdigit_color(216,0,h2,WHITE,HOME_BACK);
	showdigit_color(232,0,12,WHITE,HOME_BACK);
	showdigit_color(248,0,m1,WHITE,HOME_BACK);showdigit_color(264,0,m2,WHITE,HOME_BACK);
#endif

#if(HOME_Page_time==2)		
	showdigit_color(0,0,y1,FONT_MID,HOME_BACK);showdigit_color(16,0,y2,FONT_MID,HOME_BACK);showdigit_color(32,0,y3,FONT_MID,HOME_BACK);showdigit_color(48,0,y4,FONT_MID,HOME_BACK);
	showdigit_color(64,0,11,FONT_MID,HOME_BACK);
	showdigit_color(80,0,mh1,FONT_MID,HOME_BACK);showdigit_color(96,0,mh2,FONT_MID,HOME_BACK);
	showdigit_color(112,0,11,FONT_MID,HOME_BACK);
	showdigit_color(128,0,d1,FONT_MID,HOME_BACK);showdigit_color(144,0,d2,FONT_MID,HOME_BACK);
	
	showdigit_color(200,0,h1,FONT_MID,HOME_BACK);showdigit_color(216,0,h2,FONT_MID,HOME_BACK);
	showdigit_color(232,0,12,FONT_MID,HOME_BACK);
	showdigit_color(248,0,m1,FONT_MID,HOME_BACK);showdigit_color(264,0,m2,FONT_MID,HOME_BACK);
#endif
	
//	showdigit_color(96,150,12,GREEN,BLACK);
//	showdigit_color(112,150,s1,GREEN,BLACK);showdigit_color(128,150,s2,GREEN,BLACK);

	//根据network_flag判断是否联网
	if(network_flag){
		
		LCD_Fill(280,0,436,31,HOME_BACK);
		show_4g(437,0,0);
		show_4g(453,0,1);
		LCD_Fill(469,0,480,31,HOME_BACK);
	}else{
		//连接网络中
//		showhanzi_1(0,0,73,1);   
//		showhanzi_1(32,0,74,1);  
//		showhanzi_1(64,0,75,1);  
//		showhanzi_1(96,0,76,1); 
//		showhanzi_1(128,0,77,1); 
		
		//Gui_Drawbmp_logo(0,0,gImage_88);
		LCD_Fill(280,0,320,31,HOME_BACK);
		showhanzi_color(320,0,73,FONT_TOP,HOME_BACK);   
		showhanzi_color(352,0,74,FONT_TOP,HOME_BACK);  
		showhanzi_color(384,0,75,FONT_TOP,HOME_BACK);  
		showhanzi_color(416,0,76,FONT_TOP,HOME_BACK);  
		showhanzi_color(448,0,77,FONT_TOP,HOME_BACK); 
		
	}

	LCD_Fill(0,38,480,40,LINE);
	LCD_Fill(0,246,480,248,LINE);
	LCD_Fill(238,40,240,202,LINE);
	LCD_Fill(240,200,480,202,LINE);
	LCD_Fill(272,200,274,246,LINE);
	
    //显示NTC温度
	showhanzi_color(5,53,111,WHITE,HOME_BACK);  
	showhanzi_color(37,53,112,WHITE,HOME_BACK);  
	showhanzi_color(69,53,113,WHITE,HOME_BACK); 
	showdigit_color(101,53,12,WHITE,HOME_BACK);
	
	
	showhanzi_color(5,107,111,WHITE,HOME_BACK);  
	showhanzi_color(37,107,112,WHITE,HOME_BACK);  
	showhanzi_color(69,107,114,WHITE,HOME_BACK); 
	showdigit_color(101,107,12,WHITE,HOME_BACK);
	
	
	showhanzi_color(5,160,111,WHITE,HOME_BACK);  
	showhanzi_color(37,160,112,WHITE,HOME_BACK);  
	showhanzi_color(69,160,65,WHITE,HOME_BACK); 
	showdigit_color(101,160,12,WHITE,HOME_BACK);
	//平均温度
	showhanzi_color(5,208,135,WHITE,HOME_BACK);  
	showhanzi_color(37,208,136,WHITE,HOME_BACK);  
	showhanzi_color(69,208,111,WHITE,HOME_BACK); 
	showhanzi_color(101,208,112,WHITE,HOME_BACK);
	showdigit_color(133,208,12,WHITE,HOME_BACK);
	
	
	
	//482 氨气值 温度值 湿度值
	showhanzi_color(255,53,111,WHITE,HOME_BACK);  
	showhanzi_color(287,53,112,WHITE,HOME_BACK);  
	showhanzi_color(319,53,107,WHITE,HOME_BACK); 
	showdigit_color(351,53,12,WHITE,HOME_BACK);
	
	showhanzi_color(255,107,137,WHITE,HOME_BACK);  
	showhanzi_color(287,107,112,WHITE,HOME_BACK);  
	showhanzi_color(319,107,107,WHITE,HOME_BACK); 
	showdigit_color(351,107,12,WHITE,HOME_BACK);
	
	showhanzi_color(255,160,109,WHITE,HOME_BACK);  
	showhanzi_color(287,160,110,WHITE,HOME_BACK);  
	showhanzi_color(319,160,107,WHITE,HOME_BACK); 
	showdigit_color(351,160,12,WHITE,HOME_BACK);

	//变频
	showhanzi_color(288,208,133,WHITE,HOME_BACK);  
	showhanzi_color(320,208,134,WHITE,HOME_BACK); 
	showdigit_color(352,208,12,WHITE,HOME_BACK);
	showdigit_color(427,208,20,UNDET,HOME_BACK);
	showdigit_color(443,208,21,UNDET,HOME_BACK);
	
	//如果温度小于-5度，显示断开，表示温度传感器没连接
	if(temperature1<0 || temperature1>90){
		showhanzi_color(123,53,95,FONT_MID,HOME_BACK);showhanzi_color(155,53,108,FONT_MID,HOME_BACK);LCD_Fill(187,53,225,85,HOME_BACK);
	}else{
		
		uint8_t t1_1 = (unsigned int)((temperature1+temp1_correct)/10) % 10;
		uint8_t t1_2 = (unsigned int)(temperature1+temp1_correct) % 10;
		uint8_t t1_3 = (unsigned int)((temperature1+temp1_correct)*10) % 10;
		//LCD_Fill(336,60,352,92,HOME_BACK);
		showdigit_color(123,53,t1_1,FONT_MID,HOME_BACK);
		showdigit_color(139,53,t1_2,FONT_MID,HOME_BACK);
		showdigit_color(155,53,10,FONT_MID,HOME_BACK);
		showdigit_color(171,53,t1_3,FONT_MID,HOME_BACK);
	}
	
	if(temperature2<0 || temperature2>90){
		showhanzi_color(123,107,95,FONT_MID,HOME_BACK);showhanzi_color(155,107,108,FONT_MID,HOME_BACK);LCD_Fill(187,107,225,139,HOME_BACK);
	}else{
		uint8_t t2_1 = (unsigned int)((temperature2+temp2_correct)/10) % 10;
		uint8_t t2_2 = (unsigned int)(temperature2+temp2_correct) % 10;
		uint8_t t2_3 = (unsigned int)((temperature2+temp2_correct)*10) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		showdigit_color(123,107,t2_1,FONT_MID,HOME_BACK);
		showdigit_color(139,107,t2_2,FONT_MID,HOME_BACK);
		showdigit_color(155,107,10,FONT_MID,HOME_BACK);
		showdigit_color(171,107,t2_3,FONT_MID,HOME_BACK);
	}
	
	if(temperature3<0 || temperature3>90){
		showhanzi_color(123,160,95,FONT_MID,HOME_BACK);showhanzi_color(155,160,108,FONT_MID,HOME_BACK);LCD_Fill(187,160,225,192,HOME_BACK);
	}else{
		uint8_t t3_1 = (unsigned int)((temperature3+temp3_correct)/10) % 10;
		uint8_t t3_2 = (unsigned int)(temperature3+temp3_correct) % 10;
		uint8_t t3_3 = (unsigned int)((temperature3+temp3_correct)*10) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		showdigit_color(123,160,t3_1,FONT_MID,HOME_BACK);
		showdigit_color(139,160,t3_2,FONT_MID,HOME_BACK);
		showdigit_color(155,160,10,FONT_MID,HOME_BACK);
		showdigit_color(171,160,t3_3,FONT_MID,HOME_BACK);
	}
	
	if(average_temp<0 || average_temp>90){
		showhanzi_color(152,208,95,FONT_MID,HOME_BACK);showhanzi_color(184,208,108,FONT_MID,HOME_BACK);LCD_Fill(216,208,254,240,HOME_BACK);
	}else{
		uint8_t t4_1 = (unsigned int)(average_temp)/10;
		uint8_t t4_2 = (unsigned int)(average_temp) % 10;
		uint8_t t4_3 = (unsigned int)(average_temp*10) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		showdigit_color(152,208,t4_1,FONT_MID,HOME_BACK);
		showdigit_color(168,208,t4_2,FONT_MID,HOME_BACK);
		showdigit_color(184,208,10,FONT_MID,HOME_BACK);
		showdigit_color(200,208,t4_3,FONT_MID,HOME_BACK);
	}
	
	// 10V -- 50Hz  5V -- 25Hz
//	if(out_voltage==10){
//		showdigit_color(373,208,1,FONT_MID,HOME_BACK);
//		showdigit_color(389,208,0,FONT_MID,HOME_BACK);
//		showdigit_color(405,208,0,FONT_MID,HOME_BACK);
//		showdigit_color(427,208,20,UNDET,HOME_BACK);
//		showdigit_color(443,208,21,UNDET,HOME_BACK);
//	}else if(out_voltage<10 && out_voltage >0){
//		LCD_Fill(373,208,389,240,HOME_BACK);
//		showdigit_color(389,208,out_voltage,FONT_MID,HOME_BACK);
//		showdigit_color(405,208,0,FONT_MID,HOME_BACK);
//		showdigit_color(427,208,20,UNDET,HOME_BACK);
//		showdigit_color(443,208,21,UNDET,HOME_BACK);
//	}else if(out_voltage==0){
//		LCD_Fill(373,208,405,240,HOME_BACK);
//		showdigit_color(405,208,0,FONT_MID,HOME_BACK);
//		showdigit_color(427,208,20,UNDET,HOME_BACK);
//		showdigit_color(443,208,21,UNDET,HOME_BACK);
//	}
	
	if(out_voltage>0){
		//LCD_Fill(373,208,389,240,HOME_BACK);
		showdigit_color(389,208,out_voltage/10,FONT_MID,HOME_BACK);
		showdigit_color(405,208,out_voltage%10,FONT_MID,HOME_BACK);
	}else if(out_voltage==0){
		LCD_Fill(389,208,405,240,HOME_BACK);
		showdigit_color(405,208,0,FONT_MID,HOME_BACK);
	}
	

	if(send_TEMP<0 || send_TEMP>800){
		showhanzi_color(373,53,95,FONT_MID,HOME_BACK);showhanzi_color(405,53,108,FONT_MID,HOME_BACK);LCD_Fill(437,53,475,85,HOME_BACK);
	}else{
		uint8_t t5_1 = (unsigned int)(send_TEMP) / 100;
		uint8_t t5_2 = (unsigned int)(send_TEMP/10) % 10;
		uint8_t t5_3 = (unsigned int)(send_TEMP) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		if(send_TEMP_buf > 0x7FFF){
			showdigit_color(373,53,11,FONT_MID,HOME_BACK);
			showdigit_color(389,53,t5_1,FONT_MID,HOME_BACK);
			showdigit_color(405,53,t5_2,FONT_MID,HOME_BACK);
			showdigit_color(421,53,10,FONT_MID,HOME_BACK);
			showdigit_color(437,53,t5_3,FONT_MID,HOME_BACK);
			LCD_Fill(453,53,475,85,HOME_BACK);
		}else{
			showdigit_color(373,53,t5_1,FONT_MID,HOME_BACK);
			showdigit_color(389,53,t5_2,FONT_MID,HOME_BACK);
			showdigit_color(405,53,10,FONT_MID,HOME_BACK);
			showdigit_color(421,53,t5_3,FONT_MID,HOME_BACK);
			LCD_Fill(437,53,475,85,HOME_BACK);
		}

	}

	if(send_RH<0 || send_RH>999){
		showhanzi_color(373,107,95,FONT_MID,HOME_BACK);showhanzi_color(405,107,108,FONT_MID,HOME_BACK);LCD_Fill(437,107,475,139,HOME_BACK);
	}else{
		uint8_t RH_1 = (unsigned int)(send_RH) / 100;
		uint8_t RH_2 = (unsigned int)(send_RH/10) % 10;
		uint8_t RH_3 = (unsigned int)(send_RH) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		showdigit_color(373,107,RH_1,FONT_MID,HOME_BACK);
		showdigit_color(389,107,RH_2,FONT_MID,HOME_BACK);
		showdigit_color(405,107,10,FONT_MID,HOME_BACK);
		showdigit_color(421,107,RH_3,FONT_MID,HOME_BACK);
		showdigit_color(443,107,19,UNDET,HOME_BACK);
		showdigit_color(459,107,20,UNDET,HOME_BACK);

		//LCD_Fill(453,53,475,85,HOME_BACK);

	}

	if(send_NH3>100 || send_NH3<0){
		showhanzi_color(373,160,95,FONT_MID,HOME_BACK);showhanzi_color(405,160,108,FONT_MID,HOME_BACK);LCD_Fill(437,160,475,192,HOME_BACK);
	}else if(send_NH3>=10&& send_NH3<100){
		LCD_Fill(373,160,389,236,HOME_BACK);
		showdigit_color(389,160,send_NH3 / 10,FONT_MID,HOME_BACK);
		showdigit_color(405,160,send_NH3 % 10,FONT_MID,HOME_BACK);
		LCD_Fill(421,160,427,192,HOME_BACK);
		showdigit_color(427,160,16,UNDET,HOME_BACK);
		showdigit_color(443,160,16,UNDET,HOME_BACK);
		showdigit_color(459,160,17,UNDET,HOME_BACK);
	}else if(send_NH3>=0&& send_NH3<10){
		LCD_Fill(373,160,405,192,HOME_BACK);
		showdigit_color(405,160,send_NH3,FONT_MID,HOME_BACK);
		LCD_Fill(421,160,427,192,HOME_BACK);
		showdigit_color(427,160,16,UNDET,HOME_BACK);
		showdigit_color(443,160,16,UNDET,HOME_BACK);
		showdigit_color(459,160,17,UNDET,HOME_BACK);
	}

	//根据relay_structure[10]数组判断风机1-10的开关状态
	showhanzi_color(12,288,78,WHITE,HOME_BACK);
	showhanzi_color(59,288,79,WHITE,HOME_BACK);
	showhanzi_color(106,288,80,WHITE,HOME_BACK);
	showhanzi_color(153,288,81,WHITE,HOME_BACK);
	showhanzi_color(200,288,82,WHITE,HOME_BACK);
	showhanzi_color(247,288,83,WHITE,HOME_BACK);
	showhanzi_color(294,288,84,WHITE,HOME_BACK);
	showhanzi_color(341,288,85,WHITE,HOME_BACK);
	showhanzi_color(388,288,86,WHITE,HOME_BACK);
	showhanzi_color(435,288,87,WHITE,HOME_BACK);


	//relay_structure[0].on_off 
	if(relay_Control[1] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(12,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(12,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(12,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(12,253,gImage_4);
	}else Gui_Drawbmp16(12,253,gImage_1);
	//relay_structure[1].on_off
	if(relay_Control[1] & 0x40){
		if(ctrl_ui==0)Gui_Drawbmp16(59,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(59,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(59,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(59,253,gImage_4);
	}else Gui_Drawbmp16(59,253,gImage_1);
	if(relay_Control[1] & 0x20){
		if(ctrl_ui==0)Gui_Drawbmp16(106,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(106,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(106,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(106,253,gImage_4);
	}else Gui_Drawbmp16(106,253,gImage_1);
	if(relay_Control[1] & 0x10){
		if(ctrl_ui==0)Gui_Drawbmp16(153,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(153,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(153,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(153,253,gImage_4);
	}else Gui_Drawbmp16(153,253,gImage_1);
	if(relay_Control[1] & 0x08){
		if(ctrl_ui==0)Gui_Drawbmp16(200,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(200,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(200,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(200,253,gImage_4);
	}else Gui_Drawbmp16(200,253,gImage_1);
	if(relay_Control[1] & 0x04){
		if(ctrl_ui==0)Gui_Drawbmp16(247,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(247,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(247,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(247,253,gImage_4);
	}else Gui_Drawbmp16(247,253,gImage_1);
	if(relay_Control[1] & 0x02){
		if(ctrl_ui==0)Gui_Drawbmp16(294,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(294,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(294,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(294,253,gImage_4);
	}else Gui_Drawbmp16(294,253,gImage_1);
	if(relay_Control[0] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(341,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(341,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(341,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(341,253,gImage_4);
	}else Gui_Drawbmp16(341,253,gImage_1);
	if(relay_Control[0] & 0x40){
		if(ctrl_ui==0)Gui_Drawbmp16(388,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(388,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(388,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(388,253,gImage_4);
	}else Gui_Drawbmp16(388,253,gImage_1);
	if(relay_Control[0] & 0x20){
		if(ctrl_ui==0)Gui_Drawbmp16(435,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(435,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(435,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(435,253,gImage_4);
	}else Gui_Drawbmp16(435,253,gImage_1);
	
}
#endif

#if (HOME_Page==3)
void Main_UI(u8 page_index,u8 key_val)
{
	u8 y1=calendar.w_year/1000; u8 y2=(calendar.w_year/100)%10; u8 y3=(calendar.w_year/10)%10; u8 y4=calendar.w_year%10;
	u8 mh1=calendar.w_month/10; u8 mh2=calendar.w_month%10;
	u8 d1=calendar.w_date/10; u8 d2=calendar.w_date%10;
	u8 h1=calendar.hour/10; u8 h2=calendar.hour%10;
	u8 m1=calendar.min/10; u8 m2=calendar.min%10;
	u8 s1=calendar.sec/10; u8 s2=calendar.sec%10;
	

#if(HOME_Page_time==1)	
	showdigit_color(0,0,y1,WHITE,HOME_BACK);showdigit_color(16,0,y2,WHITE,HOME_BACK);showdigit_color(32,0,y3,WHITE,HOME_BACK);showdigit_color(48,0,y4,WHITE,HOME_BACK);
	showdigit_color(64,0,11,WHITE,HOME_BACK);
	showdigit_color(80,0,mh1,WHITE,HOME_BACK);showdigit_color(96,0,mh2,WHITE,HOME_BACK);
	showdigit_color(112,0,11,WHITE,HOME_BACK);
	showdigit_color(128,0,d1,WHITE,HOME_BACK);showdigit_color(144,0,d2,WHITE,HOME_BACK);
	
	showdigit_color(200,0,h1,WHITE,HOME_BACK);showdigit_color(216,0,h2,WHITE,HOME_BACK);
	showdigit_color(232,0,12,WHITE,HOME_BACK);
	showdigit_color(248,0,m1,WHITE,HOME_BACK);showdigit_color(264,0,m2,WHITE,HOME_BACK);
#endif

#if(HOME_Page_time==2)		
	showdigit_color(0,0,y1,FONT_MID,HOME_BACK);showdigit_color(16,0,y2,FONT_MID,HOME_BACK);showdigit_color(32,0,y3,FONT_MID,HOME_BACK);showdigit_color(48,0,y4,FONT_MID,HOME_BACK);
	showdigit_color(64,0,11,FONT_MID,HOME_BACK);
	showdigit_color(80,0,mh1,FONT_MID,HOME_BACK);showdigit_color(96,0,mh2,FONT_MID,HOME_BACK);
	showdigit_color(112,0,11,FONT_MID,HOME_BACK);
	showdigit_color(128,0,d1,FONT_MID,HOME_BACK);showdigit_color(144,0,d2,FONT_MID,HOME_BACK);
	
	showdigit_color(200,0,h1,FONT_MID,HOME_BACK);showdigit_color(216,0,h2,FONT_MID,HOME_BACK);
	showdigit_color(232,0,12,FONT_MID,HOME_BACK);
	showdigit_color(248,0,m1,FONT_MID,HOME_BACK);showdigit_color(264,0,m2,FONT_MID,HOME_BACK);
#endif
	
//	showdigit_color(96,150,12,GREEN,BLACK);
//	showdigit_color(112,150,s1,GREEN,BLACK);showdigit_color(128,150,s2,GREEN,BLACK);

	//根据network_flag判断是否联网
	if(network_flag){
		
		LCD_Fill(280,0,436,31,HOME_BACK);
		show_4g(437,0,0);
		show_4g(453,0,1);
		LCD_Fill(469,0,480,31,HOME_BACK);
	}else{
		//连接网络中
//		showhanzi_1(0,0,73,1);   
//		showhanzi_1(32,0,74,1);  
//		showhanzi_1(64,0,75,1);  
//		showhanzi_1(96,0,76,1); 
//		showhanzi_1(128,0,77,1); 
		
		//Gui_Drawbmp_logo(0,0,gImage_88);
		LCD_Fill(280,0,320,31,HOME_BACK);
		showhanzi_color(320,0,73,FONT_TOP,HOME_BACK);   
		showhanzi_color(352,0,74,FONT_TOP,HOME_BACK);  
		showhanzi_color(384,0,75,FONT_TOP,HOME_BACK);  
		showhanzi_color(416,0,76,FONT_TOP,HOME_BACK);  
		showhanzi_color(448,0,77,FONT_TOP,HOME_BACK); 
		
	}

	LCD_Fill(0,38,480,40,LINE);
	LCD_Fill(0,246,480,248,LINE);
	LCD_Fill(239,40,241,246,LINE);

	
    //显示NTC温度
	showhanzi_color(5,53,111,WHITE,HOME_BACK);  
	showhanzi_color(37,53,112,WHITE,HOME_BACK);  
	showhanzi_color(69,53,113,WHITE,HOME_BACK); 
	showdigit_color(101,53,12,WHITE,HOME_BACK);
	
	
	showhanzi_color(5,107,111,WHITE,HOME_BACK);  
	showhanzi_color(37,107,112,WHITE,HOME_BACK);  
	showhanzi_color(69,107,114,WHITE,HOME_BACK); 
	showdigit_color(101,107,12,WHITE,HOME_BACK);
	
	//温度三
//	showhanzi_color(5,160,111,WHITE,HOME_BACK);  
//	showhanzi_color(37,160,112,WHITE,HOME_BACK);  
//	showhanzi_color(69,160,65,WHITE,HOME_BACK); 
//	showdigit_color(101,160,12,WHITE,HOME_BACK);
	//平均温度
	showhanzi_color(5,160,135,WHITE,HOME_BACK);  
	showhanzi_color(37,160,136,WHITE,HOME_BACK);  
	showhanzi_color(69,160,111,WHITE,HOME_BACK); 
	showhanzi_color(101,160,112,WHITE,HOME_BACK);
	showdigit_color(133,160,12,WHITE,HOME_BACK); 
	
	
	
	//482 氨气值 温度值 湿度值
	showhanzi_color(255,53,111,WHITE,HOME_BACK);  
	showhanzi_color(287,53,112,WHITE,HOME_BACK);  
	showhanzi_color(319,53,107,WHITE,HOME_BACK); 
	showdigit_color(351,53,12,WHITE,HOME_BACK);
	
	showhanzi_color(255,107,137,WHITE,HOME_BACK);  
	showhanzi_color(287,107,112,WHITE,HOME_BACK);  
	showhanzi_color(319,107,107,WHITE,HOME_BACK); 
	showdigit_color(351,107,12,WHITE,HOME_BACK);
	
	showhanzi_color(255,160,109,WHITE,HOME_BACK);  
	showhanzi_color(287,160,110,WHITE,HOME_BACK);  
	showhanzi_color(319,160,107,WHITE,HOME_BACK); 
	showdigit_color(351,160,12,WHITE,HOME_BACK);

	//变频
//	showhanzi_color(288,208,133,WHITE,HOME_BACK);  
//	showhanzi_color(320,208,134,WHITE,HOME_BACK); 
//	showdigit_color(352,208,12,WHITE,HOME_BACK);

	showhanzi_color(5,208,133,WHITE,HOME_BACK);  
	showhanzi_color(37,208,134,WHITE,HOME_BACK); 
	showdigit_color(69,208,12,WHITE,HOME_BACK);//69+16=85 91 
	showdigit_color(123,208,20,UNDET,HOME_BACK);
	showdigit_color(139,208,21,UNDET,HOME_BACK);
	
	//如果温度小于-5度，显示断开，表示温度传感器没连接
	if(temperature1<0 || temperature1>90){
		showhanzi_color(123,53,95,FONT_MID,HOME_BACK);showhanzi_color(155,53,108,FONT_MID,HOME_BACK);LCD_Fill(187,53,225,85,HOME_BACK);
	}else{
		
		uint8_t t1_1 = (unsigned int)((temperature1+temp1_correct)/10) % 10;
		uint8_t t1_2 = (unsigned int)(temperature1+temp1_correct) % 10;
		uint8_t t1_3 = (unsigned int)((temperature1+temp1_correct)*10) % 10;
		//LCD_Fill(336,60,352,92,HOME_BACK);
		showdigit_color(123,53,t1_1,FONT_MID,HOME_BACK);
		showdigit_color(139,53,t1_2,FONT_MID,HOME_BACK);
		showdigit_color(155,53,10,FONT_MID,HOME_BACK);
		showdigit_color(171,53,t1_3,FONT_MID,HOME_BACK);
	}
	
	if(temperature2<0 || temperature2>90){
		showhanzi_color(123,107,95,FONT_MID,HOME_BACK);showhanzi_color(155,107,108,FONT_MID,HOME_BACK);LCD_Fill(187,107,225,139,HOME_BACK);
	}else{
		uint8_t t2_1 = (unsigned int)((temperature2+temp2_correct)/10) % 10;
		uint8_t t2_2 = (unsigned int)(temperature2+temp2_correct) % 10;
		uint8_t t2_3 = (unsigned int)((temperature2+temp2_correct)*10) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		showdigit_color(123,107,t2_1,FONT_MID,HOME_BACK);
		showdigit_color(139,107,t2_2,FONT_MID,HOME_BACK);
		showdigit_color(155,107,10,FONT_MID,HOME_BACK);
		showdigit_color(171,107,t2_3,FONT_MID,HOME_BACK);
	}
	
//	if(temperature3<0 || temperature3>90){
//		showhanzi_color(123,160,95,FONT_MID,HOME_BACK);showhanzi_color(155,160,108,FONT_MID,HOME_BACK);LCD_Fill(187,160,225,192,HOME_BACK);
//	}else{
//		uint8_t t3_1 = (unsigned int)((temperature3+temp3_correct)/10) % 10;
//		uint8_t t3_2 = (unsigned int)(temperature3+temp3_correct) % 10;
//		uint8_t t3_3 = (unsigned int)((temperature3+temp3_correct)*10) % 10;
//		//LCD_Fill(336,132,351,164,HOME_BACK);
//		showdigit_color(123,160,t3_1,FONT_MID,HOME_BACK);
//		showdigit_color(139,160,t3_2,FONT_MID,HOME_BACK);
//		showdigit_color(155,160,10,FONT_MID,HOME_BACK);
//		showdigit_color(171,160,t3_3,FONT_MID,HOME_BACK);
//	}
	
	if(average_temp<0 || average_temp>90){
		showhanzi_color(155,160,95,FONT_MID,HOME_BACK);showhanzi_color(187,160,108,FONT_MID,HOME_BACK);
		//LCD_Fill(216,208,254,240,HOME_BACK);
	}else{
		uint8_t t4_1 = (unsigned int)(average_temp)/10;
		uint8_t t4_2 = (unsigned int)(average_temp) % 10;
		uint8_t t4_3 = (unsigned int)(average_temp*10) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		showdigit_color(155,160,t4_1,FONT_MID,HOME_BACK);
		showdigit_color(171,160,t4_2,FONT_MID,HOME_BACK);
		showdigit_color(187,160,10,FONT_MID,HOME_BACK);
		showdigit_color(203,160,t4_3,FONT_MID,HOME_BACK);
	}
	
	// 10V -- 50Hz  5V -- 25Hz
//	if(out_voltage==10){
//		showdigit_color(373,208,1,FONT_MID,HOME_BACK);
//		showdigit_color(389,208,0,FONT_MID,HOME_BACK);
//		showdigit_color(405,208,0,FONT_MID,HOME_BACK);
//		showdigit_color(427,208,20,UNDET,HOME_BACK);
//		showdigit_color(443,208,21,UNDET,HOME_BACK);
//	}else if(out_voltage<10 && out_voltage >0){
//		LCD_Fill(373,208,389,240,HOME_BACK);
//		showdigit_color(389,208,out_voltage,FONT_MID,HOME_BACK);
//		showdigit_color(405,208,0,FONT_MID,HOME_BACK);
//		showdigit_color(427,208,20,UNDET,HOME_BACK);
//		showdigit_color(443,208,21,UNDET,HOME_BACK);
//	}else if(out_voltage==0){
//		LCD_Fill(373,208,405,240,HOME_BACK);
//		showdigit_color(405,208,0,FONT_MID,HOME_BACK);
//		showdigit_color(427,208,20,UNDET,HOME_BACK);
//		showdigit_color(443,208,21,UNDET,HOME_BACK);
//	}
	
	if(out_voltage>0){
		//LCD_Fill(373,208,389,240,HOME_BACK);
		showdigit_color(91,208,out_voltage/10,FONT_MID,HOME_BACK);
		showdigit_color(107,208,out_voltage%10,FONT_MID,HOME_BACK);
	}else if(out_voltage==0){
		LCD_Fill(91,208,107,240,HOME_BACK);
		showdigit_color(107,208,0,FONT_MID,HOME_BACK);
	}
	

	if(send_TEMP<0 || send_TEMP>800){
		showhanzi_color(373,53,95,FONT_MID,HOME_BACK);showhanzi_color(405,53,108,FONT_MID,HOME_BACK);LCD_Fill(437,53,475,85,HOME_BACK);
	}else{
		uint8_t t5_1 = (unsigned int)(send_TEMP) / 100;
		uint8_t t5_2 = (unsigned int)(send_TEMP/10) % 10;
		uint8_t t5_3 = (unsigned int)(send_TEMP) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		if(send_TEMP_buf > 0x7FFF){
			showdigit_color(373,53,11,FONT_MID,HOME_BACK);
			showdigit_color(389,53,t5_1,FONT_MID,HOME_BACK);
			showdigit_color(405,53,t5_2,FONT_MID,HOME_BACK);
			showdigit_color(421,53,10,FONT_MID,HOME_BACK);
			showdigit_color(437,53,t5_3,FONT_MID,HOME_BACK);
			LCD_Fill(453,53,475,85,HOME_BACK);
		}else{
			showdigit_color(373,53,t5_1,FONT_MID,HOME_BACK);
			showdigit_color(389,53,t5_2,FONT_MID,HOME_BACK);
			showdigit_color(405,53,10,FONT_MID,HOME_BACK);
			showdigit_color(421,53,t5_3,FONT_MID,HOME_BACK);
			LCD_Fill(437,53,475,85,HOME_BACK);
		}

	}

	if(send_RH<0 || send_RH>999){
		showhanzi_color(373,107,95,FONT_MID,HOME_BACK);showhanzi_color(405,107,108,FONT_MID,HOME_BACK);LCD_Fill(437,107,475,139,HOME_BACK);
	}else{
		uint8_t RH_1 = (unsigned int)(send_RH) / 100;
		uint8_t RH_2 = (unsigned int)(send_RH/10) % 10;
		uint8_t RH_3 = (unsigned int)(send_RH) % 10;
		//LCD_Fill(336,132,351,164,HOME_BACK);
		showdigit_color(373,107,RH_1,FONT_MID,HOME_BACK);
		showdigit_color(389,107,RH_2,FONT_MID,HOME_BACK);
		showdigit_color(405,107,10,FONT_MID,HOME_BACK);
		showdigit_color(421,107,RH_3,FONT_MID,HOME_BACK);
		showdigit_color(443,107,19,UNDET,HOME_BACK);
		showdigit_color(459,107,20,UNDET,HOME_BACK);

		//LCD_Fill(453,53,475,85,HOME_BACK);

	}

	if(send_NH3>100 || send_NH3<0){
		showhanzi_color(373,160,95,FONT_MID,HOME_BACK);showhanzi_color(405,160,108,FONT_MID,HOME_BACK);LCD_Fill(437,160,475,192,HOME_BACK);
	}else if(send_NH3>=10&& send_NH3<100){
		LCD_Fill(373,160,389,236,HOME_BACK);
		showdigit_color(389,160,send_NH3 / 10,FONT_MID,HOME_BACK);
		showdigit_color(405,160,send_NH3 % 10,FONT_MID,HOME_BACK);
		LCD_Fill(421,160,427,192,HOME_BACK);
		showdigit_color(427,160,16,UNDET,HOME_BACK);
		showdigit_color(443,160,16,UNDET,HOME_BACK);
		showdigit_color(459,160,17,UNDET,HOME_BACK);
	}else if(send_NH3>=0&& send_NH3<10){
		LCD_Fill(373,160,405,192,HOME_BACK);
		showdigit_color(405,160,send_NH3,FONT_MID,HOME_BACK);
		LCD_Fill(421,160,427,192,HOME_BACK);
		showdigit_color(427,160,16,UNDET,HOME_BACK);
		showdigit_color(443,160,16,UNDET,HOME_BACK);
		showdigit_color(459,160,17,UNDET,HOME_BACK);
	}


	
	#if (ROAD_COUNT==10)
	
	//根据relay_structure[10]数组判断风机1-10的开关状态
	showhanzi_color(12,288,78,WHITE,HOME_BACK);
	showhanzi_color(59,288,79,WHITE,HOME_BACK);
	showhanzi_color(106,288,80,WHITE,HOME_BACK);
	showhanzi_color(153,288,81,WHITE,HOME_BACK);
	showhanzi_color(200,288,82,WHITE,HOME_BACK);
	showhanzi_color(247,288,83,WHITE,HOME_BACK);
	showhanzi_color(294,288,84,WHITE,HOME_BACK);
	showhanzi_color(341,288,85,WHITE,HOME_BACK);
	showhanzi_color(388,288,86,WHITE,HOME_BACK);
	showhanzi_color(435,288,87,WHITE,HOME_BACK);
	if(relay_Control[1] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(12,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(12,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(12,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(12,253,gImage_4);
	}else Gui_Drawbmp16(12,253,gImage_1);
	//relay_structure[1].on_off
	if(relay_Control[1] & 0x40){
		if(ctrl_ui==0)Gui_Drawbmp16(59,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(59,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(59,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(59,253,gImage_4);
	}else Gui_Drawbmp16(59,253,gImage_1);
	if(relay_Control[1] & 0x20){
		if(ctrl_ui==0)Gui_Drawbmp16(106,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(106,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(106,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(106,253,gImage_4);
	}else Gui_Drawbmp16(106,253,gImage_1);
	if(relay_Control[1] & 0x10){
		if(ctrl_ui==0)Gui_Drawbmp16(153,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(153,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(153,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(153,253,gImage_4);
	}else Gui_Drawbmp16(153,253,gImage_1);
	if(relay_Control[1] & 0x08){
		if(ctrl_ui==0)Gui_Drawbmp16(200,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(200,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(200,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(200,253,gImage_4);
	}else Gui_Drawbmp16(200,253,gImage_1);
	if(relay_Control[1] & 0x04){
		if(ctrl_ui==0)Gui_Drawbmp16(247,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(247,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(247,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(247,253,gImage_4);
	}else Gui_Drawbmp16(247,253,gImage_1);
	if(relay_Control[1] & 0x02){
		if(ctrl_ui==0)Gui_Drawbmp16(294,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(294,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(294,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(294,253,gImage_4);
	}else Gui_Drawbmp16(294,253,gImage_1);
	if(relay_Control[0] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(341,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(341,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(341,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(341,253,gImage_4);
	}else Gui_Drawbmp16(341,253,gImage_1);
	if(relay_Control[0] & 0x40){
		if(ctrl_ui==0)Gui_Drawbmp16(388,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(388,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(388,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(388,253,gImage_4);
	}else Gui_Drawbmp16(388,253,gImage_1);
	if(relay_Control[0] & 0x20){
		if(ctrl_ui==0)Gui_Drawbmp16(435,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(435,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(435,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(435,253,gImage_4);
	}else Gui_Drawbmp16(435,253,gImage_1);
	#endif
	
	#if (ROAD_COUNT==8)
	//根据relay_structure[10]数组判断风机1-10的开关状态
	showhanzi_color(21,288,78,WHITE,HOME_BACK);
	showhanzi_color(79,288,79,WHITE,HOME_BACK);
	showhanzi_color(137,288,80,WHITE,HOME_BACK);
	showhanzi_color(195,288,81,WHITE,HOME_BACK);
	showhanzi_color(253,288,82,WHITE,HOME_BACK);
	showhanzi_color(311,288,83,WHITE,HOME_BACK);
	showhanzi_color(369,288,84,WHITE,HOME_BACK);
	showhanzi_color(427,288,85,WHITE,HOME_BACK);
	if(relay_Control[1] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(21,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(21,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(21,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(21,253,gImage_4);
	}else Gui_Drawbmp16(21,253,gImage_1);
	if(relay_Control[1] & 0x40){
		if(ctrl_ui==0)Gui_Drawbmp16(79,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(79,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(79,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(79,253,gImage_4);
	}else Gui_Drawbmp16(79,253,gImage_1);
	if(relay_Control[1] & 0x20){
		if(ctrl_ui==0)Gui_Drawbmp16(137,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(137,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(137,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(137,253,gImage_4);
	}else Gui_Drawbmp16(137,253,gImage_1);
	if(relay_Control[1] & 0x10){
		if(ctrl_ui==0)Gui_Drawbmp16(195,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(195,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(195,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(195,253,gImage_4);
	}else Gui_Drawbmp16(195,253,gImage_1);
	if(relay_Control[1] & 0x08){
		if(ctrl_ui==0)Gui_Drawbmp16(253,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(253,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(253,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(253,253,gImage_4);
	}else Gui_Drawbmp16(253,253,gImage_1);
	if(relay_Control[1] & 0x04){
		if(ctrl_ui==0)Gui_Drawbmp16(311,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(311,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(311,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(311,253,gImage_4);
	}else Gui_Drawbmp16(311,253,gImage_1);
	if(relay_Control[1] & 0x02){
		if(ctrl_ui==0)Gui_Drawbmp16(369,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(369,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(369,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(369,253,gImage_4);
	}else Gui_Drawbmp16(369,253,gImage_1);
	if(relay_Control[0] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(427,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(427,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(427,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(427,253,gImage_4);
	}else Gui_Drawbmp16(427,253,gImage_1);
	#endif
	
	
	#if (ROAD_COUNT==4)
	//根据relay_structure[10]数组判断风机1-10的开关状态
	showhanzi_color(18,288,78,WHITE,HOME_BACK);
	showhanzi_color(155,288,79,WHITE,HOME_BACK);
	showhanzi_color(292,288,80,WHITE,HOME_BACK);
	showhanzi_color(429,288,81,WHITE,HOME_BACK);
	if(relay_Control[1] & 0x80){
		if(ctrl_ui==0)Gui_Drawbmp16(18,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(18,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(18,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(18,253,gImage_4);
	}else Gui_Drawbmp16(18,253,gImage_1);
	if(relay_Control[1] & 0x40){
		if(ctrl_ui==0)Gui_Drawbmp16(155,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(155,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(155,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(155,253,gImage_4);
	}else Gui_Drawbmp16(155,253,gImage_1);
	if(relay_Control[1] & 0x20){
		if(ctrl_ui==0)Gui_Drawbmp16(292,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(292,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(292,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(292,253,gImage_4);
	}else Gui_Drawbmp16(292,253,gImage_1);
	if(relay_Control[1] & 0x10){
		if(ctrl_ui==0)Gui_Drawbmp16(429,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(429,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(429,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(429,253,gImage_4);
	}else Gui_Drawbmp16(429,253,gImage_1);
	#endif
}
#endif


/*
函数功能：主菜单按返回键显示函数
参数：u8 page_index,u8 key_val
返回值：无
*/
void Main_Back_UI(u8 page_index,u8 key_val){
	static u8 obj_value=25;
	static u8 obj_value_temp=25;
	LCD_Fill(0,38,480,40,LINE);
	LCD_Fill(0,246,480,248,LINE);
	//目标温度-快速同步设置
	showhanzi_color(73,0,122,WHITE,HOME_BACK);  
	showhanzi_color(105,0,123,WHITE,HOME_BACK);  
	showhanzi_color(137,0,39,WHITE,HOME_BACK);  
	showhanzi_color(169,0,66,WHITE,HOME_BACK);  
	showdigit_color(201,0,11,WHITE,HOME_BACK);
	showhanzi_color(217,0,124,WHITE,HOME_BACK);  
	showhanzi_color(249,0,125,WHITE,HOME_BACK);  
	showhanzi_color(281,0,126,WHITE,HOME_BACK);  
	showhanzi_color(313,0,127,WHITE,HOME_BACK);  
	showhanzi_color(345,0,16,WHITE,HOME_BACK);
	showhanzi_color(377,0,17,WHITE,HOME_BACK);
	//中间显示部分 目标值 修改为
	showhanzi_color(130,87,122,WHITE,HOME_BACK);  
	showhanzi_color(162,87,123,WHITE,HOME_BACK);  
	showhanzi_color(194,87,88,WHITE,HOME_BACK);  
	showdigit_color(226,87,12,WHITE,HOME_BACK); 
	
	showhanzi_color(130,169,20,WHITE,HOME_BACK);  
	showhanzi_color(162,169,120,WHITE,HOME_BACK);  
	showhanzi_color(194,169,121,WHITE,HOME_BACK);  
	showdigit_color(226,169,12,WHITE,HOME_BACK); 
	//底部说明
	showhanzi_color(0,256,132,WHITE,HOME_BACK);  
	showhanzi_color(32,256,138,GREEN,HOME_BACK);  
	showhanzi_color(64,256,119,GREEN,HOME_BACK);  
	showhanzi_color(96,256,128,WHITE,HOME_BACK);  
	showhanzi_color(128,256,120,WHITE,HOME_BACK);
	showhanzi_color(160,256,15,WHITE,HOME_BACK);  
	showdigit_color(192,256,18,WHITE,HOME_BACK);  
	showhanzi_color(224,256,132,WHITE,HOME_BACK);  
	showhanzi_color(256,256,16,GREEN,HOME_BACK);  
	showhanzi_color(288,256,17,GREEN,HOME_BACK);
	showhanzi_color(320,256,128,WHITE,HOME_BACK);
	showhanzi_color(352,256,20,WHITE,HOME_BACK);
	showhanzi_color(384,256,120,WHITE,HOME_BACK);
	
	showhanzi_color(0,288,132,WHITE,HOME_BACK);  
	showhanzi_color(32,288,139,GREEN,HOME_BACK);  
	showhanzi_color(64,288,140,GREEN,HOME_BACK);  
	showhanzi_color(96,288,128,WHITE,HOME_BACK);  
	showhanzi_color(128,288,92,WHITE,HOME_BACK);
	showhanzi_color(160,288,20,WHITE,HOME_BACK);  
	showhanzi_color(192,288,120,WHITE,HOME_BACK);  
	showdigit_color(224,288,18,WHITE,HOME_BACK);  
	showhanzi_color(256,288,129,WHITE,HOME_BACK);  
	showhanzi_color(288,288,74,WHITE,HOME_BACK);
	showhanzi_color(320,288,130,WHITE,HOME_BACK);
	showhanzi_color(352,288,131,WHITE,HOME_BACK);
	
	switch(key_val)
	{
		case KEY_PREVIOUS:
				obj_value_temp +=1;
				if(obj_value_temp>50){obj_value_temp=25;obj_value=25;}
				break;
		case KEY_ENTER:
				if(obj_value>obj_value_temp){
					u8 sub_data = obj_value - obj_value_temp;
					for(int i=0; i<10; i++){
						if(relay_structure[i].relay_mode==1 || relay_structure[i].relay_mode==3){
							relay_structure[i].temp_control.max_temp -= sub_data;
							relay_structure[i].temp_control.min_temp -= sub_data;
							if(relay_structure[i].temp_control.max_temp<=0)relay_structure[i].temp_control.max_temp=0;
							if(relay_structure[i].temp_control.min_temp<=0)relay_structure[i].temp_control.min_temp=0;
						}
					}
				}
				if(obj_value<obj_value_temp){
					u8 sub_data = obj_value_temp - obj_value;
					for(int i=0; i<10; i++){
						if(relay_structure[i].relay_mode==1 || relay_structure[i].relay_mode==3){
							relay_structure[i].temp_control.max_temp += sub_data;
							relay_structure[i].temp_control.min_temp += sub_data;
							if(relay_structure[i].temp_control.max_temp>90)relay_structure[i].temp_control.max_temp=90;
							if(relay_structure[i].temp_control.min_temp>90)relay_structure[i].temp_control.min_temp=90;
						}
					}
				}
				obj_value = obj_value_temp;
				break;
		case KEY_NEXT:
				obj_value_temp -=1;
				if(obj_value_temp==255){obj_value_temp=25;obj_value=25;}
				break;
		case KEY_BACK:

				break;
		default:break;
	}
	u8 obj_value_temp_1 = obj_value_temp / 10;
	u8 obj_value_temp_2 = obj_value_temp % 10;
	if(obj_value>=10){
		showdigit_color(252,87,obj_value/10,GREEN,HOME_BACK);
		showdigit_color(268,87,obj_value%10,GREEN,HOME_BACK);
	}else if(obj_value<10){
		showdigit_color(252,87,13,HOME_BACK,HOME_BACK);
		showdigit_color(268,87,obj_value%10,GREEN,HOME_BACK);
	}
	if(obj_value_temp>=10){
		showdigit_color(252,169,obj_value_temp_1,WHITE,RED);
		showdigit_color(268,169,obj_value_temp_2,WHITE,RED);
	}else if(obj_value_temp<10){
		showdigit_color(252,169,13,HOME_BACK,HOME_BACK);
		showdigit_color(268,169,obj_value_temp,WHITE,RED);
	}

	
}

/*
函数功能：主菜单显示函数
参数：u8 page_index,u8 key_val
返回值：无
*/
void Main_Menu_Func(u8 page_index,u8 key_val)
{
	
	if(page_index!=_Air_Blower_Option){showhanzi(177,34,4);showhanzi(209,34,5);showhanzi(241,34,6);showhanzi(273,34,7);}
	if(page_index!=_Hz_Ctrl_Option){showhanzi(177,68,8);showhanzi(209,68,9);showhanzi(241,68,10);showhanzi(273,68,11);}
	if(page_index!=_Alarm_Option){showhanzi(177,102,12);showhanzi(209,102,13);showhanzi(241,102,14);showhanzi(273,102,15);}
	if(page_index!=_Time_Option){showhanzi(177,136,16);showhanzi(209,136,17);showhanzi(241,136,18);showhanzi(273,136,19);}
	if(page_index!=_Data_Change_Option){showhanzi(177,170,20);showhanzi(209,170,21);showhanzi(241,170,14);showhanzi(273,170,15);}
	if(page_index!=_Iot_Option){showhanzi(177,204,22);showhanzi(209,204,23);showhanzi(241,204,24);showhanzi(273,204,25);}
	
	switch(page_index)
	{
		case _Air_Blower_Option://显示普通风机
			showhanzi_1(177,34,4,GBLUE);  
			showhanzi_1(209,34,5,GBLUE);  
			showhanzi_1(241,34,6,GBLUE);  
			showhanzi_1(273,34,7,GBLUE); 
			break;
		case _Hz_Ctrl_Option://显示变频控制
			showhanzi_1(177,68,8,GBLUE);  
			showhanzi_1(209,68,9,GBLUE);  
			showhanzi_1(241,68,10,GBLUE);  
			showhanzi_1(273,68,11,GBLUE);  
			break;
		case _Alarm_Option:
			showhanzi_1(177,102,12,GBLUE);  
			showhanzi_1(209,102,13,GBLUE);  
			showhanzi_1(241,102,14,GBLUE);  
			showhanzi_1(273,102,15,GBLUE);  
			break;
		case _Time_Option:
			showhanzi_1(177,136,16,GBLUE);  
			showhanzi_1(209,136,17,GBLUE);  
			showhanzi_1(241,136,18,GBLUE);  
			showhanzi_1(273,136,19,GBLUE);  
			break;
		case _Data_Change_Option:
			showhanzi_1(177,170,20,GBLUE);  
			showhanzi_1(209,170,21,GBLUE);  
			showhanzi_1(241,170,14,GBLUE);  
			showhanzi_1(273,170,15,GBLUE);  
			break;
		case _Iot_Option:
			showhanzi_1(177,204,22,GBLUE);  
			showhanzi_1(209,204,23,GBLUE);  
			showhanzi_1(241,204,24,GBLUE);  
			showhanzi_1(273,204,25,GBLUE); 
			break;
		default:break;
	}
}




void Hz_Ctrl_Child(u8 page_index,u8 key_val){
	static u8 cur_pos=1;
	Hz_Control hz_control_buf;
	memcpy(&hz_control_buf,&hz_control,sizeof(Hz_Control));

	showhanzi_color(0,288,16,GREEN,WHITE);showhanzi_color(32,288,17,GREEN,WHITE);showhanzi_color(64,288,128,BLACK,WHITE);showhanzi_color(96,288,146,BLACK,WHITE);
	showhanzi_color(128,288,147,BLACK,WHITE);showhanzi_color(170,288,138,GREEN,WHITE);showhanzi_color(202,288,119,GREEN,WHITE);showhanzi_color(234,288,128,BLACK,WHITE);
	showhanzi_color(266,288,20,BLACK,WHITE);showhanzi_color(298,288,120,BLACK,WHITE);
	
	//中间部分
	showhanzi(0,48,39);showhanzi(32,48,66);showhanzi(64,48,42);showhanzi(96,48,45);showhanzi(128,48,88);showdigit_color(160,48,12,WHITE,BLACK);showhanzi(250,48,66);
	showhanzi(0,80,39);showhanzi(32,80,66);showhanzi(64,80,43);showhanzi(96,80,45);showhanzi(128,80,88);showdigit_color(160,80,12,WHITE,BLACK);showhanzi(250,80,66);
	
	showhanzi(0,112,39);showhanzi(32,112,66);showhanzi(64,112,42);showhanzi(96,112,45);showhanzi(128,112,50);showhanzi(160,112,51);showhanzi(192,112,141);
	showhanzi(224,112,142);showhanzi(256,112,143);showhanzi(288,112,144);showhanzi(320,112,145);showdigit_color(352,112,12,WHITE,BLACK);
	showdigit_color(442,144,14,WHITE,BLACK);
	
	showhanzi(0,176,39);showhanzi(32,176,66);showhanzi(64,176,43);showhanzi(96,176,45);showhanzi(128,176,50);showhanzi(160,176,51);showhanzi(192,176,141);
	showhanzi(224,176,142);showhanzi(256,176,143);showhanzi(288,176,144);showhanzi(320,176,145);showdigit_color(352,176,12,WHITE,BLACK);
	showdigit_color(442,208,14,WHITE,BLACK);
	
	showhanzi(0,240,50);showhanzi(32,240,51);showhanzi(64,240,141);showhanzi(96,240,52);showhanzi(128,240,53);showhanzi(160,240,54);showdigit_color(192,240,12,WHITE,BLACK);
	
	if(last_index!=_Hz_Ctrl_Option)//判断是否是s第一次进入此界面
	{	switch(key_val)
		{
			case KEY_PREVIOUS:
				if(cur_pos == 1) hz_control.max_temp += 0.1;
				if(cur_pos == 2) if((hz_control.min_temp+2)<hz_control.max_temp)hz_control.min_temp+=0.1;
				if(cur_pos == 3) hz_control.voltage_high += 0.1;
				if(cur_pos == 4) if((hz_control.voltage_low+2)<hz_control.voltage_high)hz_control.voltage_low += 0.1;
				if(cur_pos == 5) hz_control.temp_choose += 1;
				if(hz_control.max_temp>50)hz_control.max_temp=50;
				if(hz_control.min_temp>45)hz_control.min_temp=45;
				if(hz_control.voltage_high>10)hz_control.voltage_high=10;
				if(hz_control.voltage_low>8)hz_control.voltage_low=8;
				if(hz_control.temp_choose>5)hz_control.temp_choose=1;
				
					break;
			case KEY_ENTER:
				cur_pos++;
				if(cur_pos == 6){
					cur_pos = 1;
				}
					break;
			case KEY_NEXT:
				if(cur_pos == 1) if((hz_control.max_temp-2)>hz_control.min_temp)hz_control.max_temp -= 0.1;
				if(cur_pos == 2) hz_control.min_temp -= 0.1;
				if(cur_pos == 3) if((hz_control.voltage_high-2)>hz_control.voltage_low)hz_control.voltage_high -= 0.1;
				if(cur_pos == 4) hz_control.voltage_low -= 0.1;
				if(cur_pos == 5) hz_control.temp_choose -= 1;
				if(hz_control.max_temp<5)hz_control.max_temp=5;
				if(hz_control.min_temp<0)hz_control.min_temp=0;
				if(hz_control.voltage_high<2)hz_control.voltage_high=2;
				if(hz_control.voltage_low<0)hz_control.voltage_low=0;
				if(hz_control.temp_choose==0)hz_control.temp_choose=5;

					break;
			default:break;
		}
	}else cur_pos=1;//第一次进入此界面,界面指针清零

	if(cur_pos!=1){
		showdigit_color(176,48,(uint8_t)hz_control.max_temp/10,WHITE,BLACK);showdigit_color(192,48,(uint8_t)hz_control.max_temp%10,WHITE,BLACK);
		showdigit_color(208,48,10,WHITE,BLACK);showdigit_color(224,48,(uint16_t)(hz_control.max_temp*10)%10,WHITE,BLACK);
	}
	if(cur_pos!=2){
		showdigit_color(176,80,(uint8_t)hz_control.min_temp/10,WHITE,BLACK);showdigit_color(192,80,(uint8_t)hz_control.min_temp%10,WHITE,BLACK);
		showdigit_color(208,80,10,WHITE,BLACK);showdigit_color(224,80,(uint16_t)(hz_control.min_temp*10)%10,WHITE,BLACK);
	}
	if(cur_pos!=3){
		if((uint8_t)hz_control.voltage_high == 10) showdigit_color(368,144,1,WHITE,BLACK); else showdigit_color(368,144,13,BLACK,BLACK);
		showdigit_color(384,144,(uint8_t)hz_control.voltage_high%10,WHITE,BLACK);
		showdigit_color(400,144,10,WHITE,BLACK);showdigit_color(416,144,(uint16_t)(hz_control.voltage_high*10)%10,WHITE,BLACK);
	}
	if(cur_pos!=4){
		showdigit_color(384,208,(uint8_t)hz_control.voltage_low,WHITE,BLACK);
		showdigit_color(400,208,10,WHITE,BLACK);showdigit_color(416,208,(uint16_t)(hz_control.voltage_low*10)%10,WHITE,BLACK);
	}
	if(cur_pos!=5){
		if(hz_control.temp_choose == 1) {showhanzi(208,240,39);showhanzi(240,240,66);showhanzi(272,240,63);showhanzi(304,240,38);}
		if(hz_control.temp_choose == 2) {showhanzi(208,240,39);showhanzi(240,240,66);showhanzi(272,240,64);showhanzi(304,240,38);}
		if(hz_control.temp_choose == 3) {showhanzi(208,240,39);showhanzi(240,240,66);showhanzi(272,240,65);showhanzi(304,240,38);}	
		if(hz_control.temp_choose == 4) {showhanzi(208,240,67);showhanzi(240,240,68);showhanzi(272,240,39);showhanzi(304,240,66);}//平均温度
		if(hz_control.temp_choose == 5) {showdigit_color(208,240,4,WHITE,BLACK);showdigit_color(224,240,8,WHITE,BLACK);showdigit_color(240,240,5,WHITE,BLACK);showhanzi(256,240,39);showhanzi(288,240,66);showdigit_color(320,240,13,BLACK,BLACK);}
	}
	//showdigit_color(280,0,(uint16_t)(hz_control.max_temp*10)%10,WHITE,HOME_BACK);
	switch(cur_pos){
		case 1:
			showdigit_color(176,48,(uint8_t)hz_control.max_temp/10,BLACK,WHITE);showdigit_color(192,48,(uint8_t)hz_control.max_temp%10,BLACK,WHITE);
			showdigit_color(208,48,10,BLACK,WHITE);showdigit_color(224,48,(uint16_t)(hz_control.max_temp*10)%10,BLACK,WHITE);
			break;
		case 2:
			showdigit_color(176,80,(uint8_t)hz_control.min_temp/10,BLACK,WHITE);showdigit_color(192,80,(uint8_t)hz_control.min_temp%10,BLACK,WHITE);
			showdigit_color(208,80,10,BLACK,WHITE);showdigit_color(224,80,(uint16_t)(hz_control.min_temp*10)%10,BLACK,WHITE);

			break;
		case 3:
			if((uint8_t)hz_control.voltage_high == 10) showdigit_color(368,144,1,BLACK,WHITE); else showdigit_color(368,144,13,BLACK,BLACK);
			showdigit_color(384,144,(uint8_t)hz_control.voltage_high%10,BLACK,WHITE);
			showdigit_color(400,144,10,BLACK,WHITE);showdigit_color(416,144,(uint16_t)(hz_control.voltage_high*10)%10,BLACK,WHITE);
			break;
		case 4:
			showdigit_color(384,208,(uint8_t)hz_control.voltage_low,BLACK,WHITE);
			showdigit_color(400,208,10,BLACK,WHITE);showdigit_color(416,208,(uint16_t)(hz_control.voltage_low*10)%10,BLACK,WHITE);
			break;
		case 5:
			if(hz_control.temp_choose == 1) {showhanzi_1(208,240,39,1);showhanzi_1(240,240,66,1);showhanzi_1(272,240,63,1);showhanzi_1(304,240,38,1);}
			if(hz_control.temp_choose == 2) {showhanzi_1(208,240,39,1);showhanzi_1(240,240,66,1);showhanzi_1(272,240,64,1);showhanzi_1(304,240,38,1);}
			if(hz_control.temp_choose == 3) {showhanzi_1(208,240,39,1);showhanzi_1(240,240,66,1);showhanzi_1(272,240,65,1);showhanzi_1(304,240,38,1);}	
			if(hz_control.temp_choose == 4) {showhanzi_1(208,240,67,1);showhanzi_1(240,240,68,1);showhanzi_1(272,240,39,1);showhanzi_1(304,240,66,1);}//平均温度
			if(hz_control.temp_choose == 5) {showdigit_color(208,240,4,BLACK,WHITE);showdigit_color(224,240,8,BLACK,WHITE);showdigit_color(240,240,5,BLACK,WHITE);showhanzi_1(256,240,39,1);showhanzi_1(288,240,66,1);showdigit_color(320,240,13,WHITE,WHITE);}
			break;
		default:break;
	}
	
	//判断是否改变变频器控制参数
	if (memcmp(&hz_control_buf, &hz_control, sizeof(Hz_Control)) != 0) {
		send_HZ_Flag=1;
	}
	
//	switch(hz_control.temp_choose){
//		case 1:
//			Hz_temp_choose = temperature1;
//			break;
//		case 2:
//			Hz_temp_choose = temperature2;
//			break;
//		case 3:
//			Hz_temp_choose = temperature3;
//			break;
//		case 4:
//			Hz_temp_choose = average_temp;
//			break;
//		case 5:
//			Hz_temp_choose = send_TEMP/10;
//			break;
//		default:break;
//	}
	
//	if(Hz_temp_choose<=90){
//		if(Hz_temp_choose<=hz_control.min_temp){
//			data_v = hz_control.voltage_low*4095/10;
//			out_voltage=hz_control.voltage_low*5;
//		}else if(Hz_temp_choose>=hz_control.max_temp){
//			data_v = hz_control.voltage_high*4095/10;
//			out_voltage=hz_control.voltage_high*5;
//		}else{
//			float data_v_t = ((Hz_temp_choose-hz_control.min_temp)/(hz_control.max_temp-hz_control.min_temp)*(hz_control.voltage_high-hz_control.voltage_low))+hz_control.voltage_low;
//			data_v = data_v_t*4095/10;
//			out_voltage=data_v_t;
//		}
//	}else{
//		data_v=0;
//		out_voltage=data_v;
//	}

//	v_low = data_v<<4 | 0x00;
//	v_high = data_v>>4 | 0x00;
//	GP8201S_Write(0,0,v_low,v_high);
//	
}
void Alarm_Child(u8 page_index,u8 key_val){
	
	u8 w1_buff = warn_temp1_flag;
	u8 w2_buff = warn_temp2_flag;
	u8 w3_buff = warn_temp3_flag;
	u8 w485_buff = warn_temp485_flag;
	u8 w_rh_buff = warn_rh_flag;
	
	u8 limit_max_temp1_buff = limit_temp1_maxvalue;
	u8 limit_min_temp1_buff = limit_temp1_minvalue;
	u8 limit_max_temp2_buff = limit_temp2_maxvalue;
	u8 limit_min_temp2_buff = limit_temp2_minvalue;
	u8 limit_max_temp3_buff = limit_temp3_maxvalue;
	u8 limit_min_temp3_buff = limit_temp3_minvalue;
	u8 limit_max_temp485_buff = limit_temp485_maxvalue;
	u8 limit_min_temp485_buff = limit_temp485_minvalue;
	u8 limit_max_rh_buff = limit_rh_maxvalue;
	u8 limit_min_rh_buff = limit_rh_minvalue;
	
	u8 NH3_warn_buff = NH3_warn_flag;
	u8 NH3_buff = NH3_max;
	static u8 cur_pos=1;
	
	//参与报警
	showhanzi(121,39,14);showhanzi(153,39,89);showhanzi(185,39,12);showhanzi(217,39,13);
	//最高限
	showhanzi(268,39,41);showhanzi(300,39,42);showhanzi(332,39,45);
	//最低限
	showhanzi(383,39,41);showhanzi(415,39,43);showhanzi(447,39,45);
	
	
	//温度一
	showhanzi(0,75,39);showhanzi(32,75,66);showhanzi(64,75,63);showhanzi(96,75,44);
	//温度二
	showhanzi(0,110,39);showhanzi(32,110,66);showhanzi(64,110,64);showhanzi(96,110,44);
	//温度三
	showhanzi(0,145,39);showhanzi(32,145,66);showhanzi(64,145,65);showhanzi(96,145,44);
	//485温度
	showdigit_color(0,180,4,WHITE,BLACK);showdigit_color(16,180,8,WHITE,BLACK);showdigit_color(32,180,5,WHITE,BLACK);showhanzi(48,180,39);showhanzi(80,180,66);showhanzi(112,180,44);
	//485湿度
	showdigit_color(0,215,4,WHITE,BLACK);showdigit_color(16,215,8,WHITE,BLACK);showdigit_color(32,215,5,WHITE,BLACK);showhanzi(48,215,148);showhanzi(80,215,66);showhanzi(112,215,44);
	//485氨气
	showdigit_color(0,250,4,WHITE,BLACK);showdigit_color(16,250,8,WHITE,BLACK);showdigit_color(32,250,5,WHITE,BLACK);showhanzi(48,250,105);showhanzi(80,250,106);showhanzi(112,250,44);
	
	
	if(last_index!=_Alarm_Option)//判断是否是s第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS:
				if(cur_pos == 1) warn_temp1_flag = !warn_temp1_flag;
				if(cur_pos == 2) limit_temp1_maxvalue += 1;
				if(cur_pos == 3) if(limit_temp1_minvalue+2<limit_temp1_maxvalue){limit_temp1_minvalue += 1;}
				if(cur_pos == 4) warn_temp2_flag = !warn_temp2_flag;
				if(cur_pos == 5) limit_temp2_maxvalue += 1;
				if(cur_pos == 6) if(limit_temp2_minvalue+2<limit_temp2_maxvalue){limit_temp2_minvalue += 1;}
			    if(cur_pos == 7) warn_temp3_flag = !warn_temp3_flag;
				if(cur_pos == 8) limit_temp3_maxvalue += 1;
				if(cur_pos == 9) if(limit_temp3_minvalue+2<limit_temp3_maxvalue){limit_temp3_minvalue += 1;}
				if(cur_pos == 10) warn_temp485_flag = !warn_temp485_flag;
				if(cur_pos == 11) limit_temp485_maxvalue += 1;
				if(cur_pos == 12) if(limit_temp485_minvalue+2<limit_temp485_maxvalue){limit_temp485_minvalue += 1;}
				if(cur_pos == 13) warn_rh_flag = !warn_rh_flag;
				if(cur_pos == 14) limit_rh_maxvalue += 1;
				if(cur_pos == 15) if(limit_rh_minvalue+2<limit_rh_maxvalue){limit_rh_minvalue += 1;}
				if(cur_pos == 16) NH3_warn_flag = !NH3_warn_flag;
				if(cur_pos == 17) NH3_max += 1;
				if(limit_temp1_maxvalue>=50)limit_temp1_maxvalue=50;
				if(limit_temp2_maxvalue>=50)limit_temp2_maxvalue=50;
				if(limit_temp3_maxvalue>=50)limit_temp3_maxvalue=50;
				if(limit_temp485_maxvalue>=50)limit_temp485_maxvalue=50;
				if(limit_rh_maxvalue>=80)limit_rh_maxvalue=80;
				if(NH3_max>=30)NH3_max=30;
					break;
			case KEY_ENTER://确定(设置)按键
				cur_pos++;
				if(cur_pos == 18){
					cur_pos = 1;
				}
					break;
			case KEY_NEXT:
				if(cur_pos == 1) warn_temp1_flag = !warn_temp1_flag;
				if(cur_pos == 2) if(limit_temp1_maxvalue-2>limit_temp1_minvalue){limit_temp1_maxvalue -= 1;}
				if(cur_pos == 3) limit_temp1_minvalue -= 1;
				if(cur_pos == 4) warn_temp2_flag = !warn_temp2_flag;
				if(cur_pos == 5) if(limit_temp2_maxvalue-2>limit_temp2_minvalue){limit_temp2_maxvalue -= 1;}
				if(cur_pos == 6) limit_temp2_minvalue -= 1;
			    if(cur_pos == 7) warn_temp3_flag = !warn_temp3_flag;
				if(cur_pos == 8) if(limit_temp3_maxvalue-2>limit_temp3_minvalue){limit_temp3_maxvalue -= 1;}
				if(cur_pos == 9) limit_temp3_minvalue -= 1;
				if(cur_pos == 10) warn_temp485_flag = !warn_temp485_flag;
				if(cur_pos == 11) if(limit_temp485_maxvalue-2>limit_temp485_minvalue){limit_temp485_maxvalue -= 1;}
				if(cur_pos == 12) limit_temp485_minvalue -= 1;
				if(cur_pos == 13) warn_rh_flag = !warn_rh_flag;
				if(cur_pos == 14) if(limit_rh_maxvalue-2>limit_rh_minvalue){limit_rh_maxvalue -= 1;}
				if(cur_pos == 15) limit_rh_minvalue -= 1;
				if(cur_pos == 16) NH3_warn_flag = !NH3_warn_flag;
				if(cur_pos == 17) NH3_max -= 1;
				if(limit_temp1_minvalue==255)limit_temp1_minvalue=0;
				if(limit_temp2_minvalue==255)limit_temp2_minvalue=0;
				if(limit_temp3_minvalue==255)limit_temp3_minvalue=0;
				if(limit_temp485_minvalue==255)limit_temp485_minvalue=0;
				if(limit_rh_minvalue==255)limit_rh_minvalue=0;
				if(NH3_max==255)NH3_max=0;
					break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零
	
	
	uint8_t nh3_max_1 = NH3_max / 10;
	uint8_t nh3_max_2 = NH3_max % 10;
	
	uint8_t temp1_max_1 = limit_temp1_maxvalue / 10;
	uint8_t temp1_max_2 = limit_temp1_maxvalue % 10;
	uint8_t temp1_min_1 = limit_temp1_minvalue / 10;
	uint8_t temp1_min_2 = limit_temp1_minvalue % 10;
	
	uint8_t temp2_max_1 = limit_temp2_maxvalue / 10;
	uint8_t temp2_max_2 = limit_temp2_maxvalue % 10;
	uint8_t temp2_min_1 = limit_temp2_minvalue / 10;
	uint8_t temp2_min_2 = limit_temp2_minvalue % 10;
	
	uint8_t temp3_max_1 = limit_temp3_maxvalue / 10;
	uint8_t temp3_max_2 = limit_temp3_maxvalue % 10;
	uint8_t temp3_min_1 = limit_temp3_minvalue / 10;
	uint8_t temp3_min_2 = limit_temp3_minvalue % 10;
		
	uint8_t temp485_max_1 = limit_temp485_maxvalue / 10;
	uint8_t temp485_max_2 = limit_temp485_maxvalue % 10;
	uint8_t temp485_min_1 = limit_temp485_minvalue / 10;
	uint8_t temp485_min_2 = limit_temp485_minvalue % 10;
	
	uint8_t rh_max_1 = limit_rh_maxvalue / 10;
	uint8_t rh_max_2 = limit_rh_maxvalue % 10;
	uint8_t rh_min_1 = limit_rh_minvalue / 10;
	uint8_t rh_min_2 = limit_rh_minvalue % 10;
	
	if(cur_pos!=1){if(warn_temp1_flag){showhanzi(153,75,90);showhanzi(185,75,91);}else{showhanzi(153,75,92);showhanzi(185,75,91);}}
	if(cur_pos!=2){showdigit_color(300,75,temp1_max_1,WHITE,BLACK);showdigit_color(316,75,temp1_max_2,WHITE,BLACK);}
	if(cur_pos!=3){showdigit_color(415,75,temp1_min_1,WHITE,BLACK);showdigit_color(431,75,temp1_min_2,WHITE,BLACK);}
	if(cur_pos!=4){if(warn_temp2_flag){showhanzi(153,110,90);showhanzi(185,110,91);}else{showhanzi(153,110,92);showhanzi(185,110,91);}}
	if(cur_pos!=5){showdigit_color(300,110,temp2_max_1,WHITE,BLACK);showdigit_color(316,110,temp2_max_2,WHITE,BLACK);}
	if(cur_pos!=6){showdigit_color(415,110,temp2_min_1,WHITE,BLACK);showdigit_color(431,110,temp2_min_2,WHITE,BLACK);}	
	if(cur_pos!=7){if(warn_temp3_flag){showhanzi(153,145,90);showhanzi(185,145,91);}else{showhanzi(153,145,92);showhanzi(185,145,91);}}
	if(cur_pos!=8){showdigit_color(300,145,temp3_max_1,WHITE,BLACK);showdigit_color(316,145,temp3_max_2,WHITE,BLACK);}
	if(cur_pos!=9){showdigit_color(415,145,temp3_min_1,WHITE,BLACK);showdigit_color(431,145,temp3_min_2,WHITE,BLACK);}
	if(cur_pos!=10){if(warn_temp485_flag){showhanzi(153,180,90);showhanzi(185,180,91);}else{showhanzi(153,180,92);showhanzi(185,180,91);}}
	if(cur_pos!=11){showdigit_color(300,180,temp485_max_1,WHITE,BLACK);showdigit_color(316,180,temp485_max_2,WHITE,BLACK);}
	if(cur_pos!=12){showdigit_color(415,180,temp485_min_1,WHITE,BLACK);showdigit_color(431,180,temp485_min_2,WHITE,BLACK);}	
	if(cur_pos!=13){if(warn_rh_flag){showhanzi(153,215,90);showhanzi(185,215,91);}else{showhanzi(153,215,92);showhanzi(185,215,91);}}
	if(cur_pos!=14){showdigit_color(300,215,rh_max_1,WHITE,BLACK);showdigit_color(316,215,rh_max_2,WHITE,BLACK);}
	if(cur_pos!=15){showdigit_color(415,215,rh_min_1,WHITE,BLACK);showdigit_color(431,215,rh_min_2,WHITE,BLACK);}
	if(cur_pos!=16){if(NH3_warn_flag){showhanzi(153,250,90);showhanzi(185,250,91);}else{showhanzi(153,250,92);showhanzi(185,250,91);}}
	if(cur_pos!=17){showdigit_color(300,250,nh3_max_1,WHITE,BLACK);showdigit_color(316,250,nh3_max_2,WHITE,BLACK);}	
	
	switch(cur_pos)
	{
		case 1:
			if(warn_temp1_flag){showhanzi_1(153,75,90,1);showhanzi_1(185,75,91,1);}else{showhanzi_1(153,75,92,1);showhanzi_1(185,75,91,1);}
			break;
		case 2:
			showdigit_color(300,75,temp1_max_1,BLACK,WHITE);showdigit_color(316,75,temp1_max_2,BLACK,WHITE);
			break;
		case 3:
			showdigit_color(415,75,temp1_min_1,BLACK,WHITE);showdigit_color(431,75,temp1_min_2,BLACK,WHITE);
			break;
		case 4:
			if(warn_temp2_flag){showhanzi_1(153,110,90,1);showhanzi_1(185,110,91,1);}else{showhanzi_1(153,110,92,1);showhanzi_1(185,110,91,1);}
			break;
		case 5:
			showdigit_color(300,110,temp2_max_1,BLACK,WHITE);showdigit_color(316,110,temp2_max_2,BLACK,WHITE);
			break;
		case 6:
			showdigit_color(415,110,temp2_min_1,BLACK,WHITE);showdigit_color(431,110,temp2_min_2,BLACK,WHITE);
			break;
		case 7:
			if(warn_temp3_flag){showhanzi_1(153,145,90,1);showhanzi_1(185,145,91,1);}else{showhanzi_1(153,145,92,1);showhanzi_1(185,145,91,1);}
			break;
		case 8:
			showdigit_color(300,145,temp3_max_1,BLACK,WHITE);showdigit_color(316,145,temp3_max_2,BLACK,WHITE);
			break;
		case 9:
			showdigit_color(415,145,temp3_min_1,BLACK,WHITE);showdigit_color(431,145,temp3_min_2,BLACK,WHITE);
			break;
		case 10:
			if(warn_temp485_flag){showhanzi_1(153,180,90,1);showhanzi_1(185,180,91,1);}else{showhanzi_1(153,180,92,1);showhanzi_1(185,180,91,1);}
			break;
		case 11:
			showdigit_color(300,180,temp485_max_1,BLACK,WHITE);showdigit_color(316,180,temp485_max_2,BLACK,WHITE);
			break;
		case 12:
			showdigit_color(415,180,temp485_min_1,BLACK,WHITE);showdigit_color(431,180,temp485_min_2,BLACK,WHITE);
			break;
		case 13:
			if(warn_rh_flag){showhanzi_1(153,215,90,1);showhanzi_1(185,215,91,1);}else{showhanzi_1(153,215,92,1);showhanzi_1(185,215,91,1);}
			break;
		case 14:
			showdigit_color(300,215,rh_max_1,BLACK,WHITE);showdigit_color(316,215,rh_max_2,BLACK,WHITE);
			break;
		case 15:
			showdigit_color(415,215,rh_min_1,BLACK,WHITE);showdigit_color(431,215,rh_min_2,BLACK,WHITE);
			break;
		case 16:
			if(NH3_warn_flag){showhanzi_1(153,250,90,1);showhanzi_1(185,250,91,1);}else{showhanzi_1(153,250,92,1);showhanzi_1(185,250,91,1);}
			break;
		case 17:
			showdigit_color(300,250,nh3_max_1,BLACK,WHITE);showdigit_color(316,250,nh3_max_2,BLACK,WHITE);
			break;
		default:break;
	}
	if(w1_buff != warn_temp1_flag ||
		w2_buff != warn_temp2_flag ||
		w3_buff != warn_temp3_flag ||
		w485_buff != warn_temp485_flag ||
		w_rh_buff != warn_rh_flag ||
		limit_max_temp1_buff != limit_temp1_maxvalue ||
		limit_min_temp1_buff != limit_temp1_minvalue ||
		limit_max_temp2_buff != limit_temp2_maxvalue ||
		limit_min_temp2_buff != limit_temp2_minvalue ||
		limit_max_temp3_buff != limit_temp3_maxvalue ||
		limit_min_temp3_buff != limit_temp3_minvalue ||
		limit_max_temp485_buff != limit_temp485_maxvalue ||
		limit_min_temp485_buff != limit_temp485_minvalue ||
		limit_max_rh_buff != limit_rh_maxvalue ||
		limit_min_rh_buff != limit_rh_minvalue ||
		NH3_warn_buff != NH3_warn_flag ||
		NH3_buff != NH3_max){
			send_warn_Flag=1;
		}
	

}
void Time_Child(u8 page_index,u8 key_val){
	static u8 cur_pos=1;
	static u16 temp_year=2024;
	static u8 temp_month=8;
	static u8 temp_day=15;
	static u8 temp_h=12;
	static u8 temp_m=30;
	static u8 temp_s=30;

	showhanzi(0,42,93);showhanzi(32,42,94);showhanzi(64,42,18);showhanzi(96,42,19);showhanzi(128,42,44);
	u8 y1=calendar.w_year/1000; u8 y2=(calendar.w_year/100)%10; u8 y3=(calendar.w_year/10)%10; u8 y4=calendar.w_year%10;
	u8 mh1=calendar.w_month/10; u8 mh2=calendar.w_month%10;
	u8 d1=calendar.w_date/10; u8 d2=calendar.w_date%10;
	u8 h1=calendar.hour/10; u8 h2=calendar.hour%10;
	u8 m1=calendar.min/10; u8 m2=calendar.min%10;
	u8 s1=calendar.sec/10; u8 s2=calendar.sec%10;

	showdigit_color(0,74,y1,GREEN,BLACK);showdigit_color(16,74,y2,GREEN,BLACK);showdigit_color(32,74,y3,GREEN,BLACK);showdigit_color(48,74,y4,GREEN,BLACK);
	showdigit_color(64,74,11,GREEN,BLACK);
	showdigit_color(80,74,mh1,GREEN,BLACK);showdigit_color(96,74,mh2,GREEN,BLACK);
	showdigit_color(112,74,11,GREEN,BLACK);
	showdigit_color(128,74,d1,GREEN,BLACK);showdigit_color(144,74,d2,GREEN,BLACK);
	showdigit_color(192,74,h1,GREEN,BLACK);showdigit_color(208,74,h2,GREEN,BLACK);
	showdigit_color(224,74,11,GREEN,BLACK);
	showdigit_color(240,74,m1,GREEN,BLACK);showdigit_color(256,74,m2,GREEN,BLACK);
	showdigit_color(272,74,11,GREEN,BLACK);
	showdigit_color(288,74,s1,GREEN,BLACK);showdigit_color(304,74,s2,GREEN,BLACK);
	
	
	//设置时间
	showhanzi(0,138,16);showhanzi(32,138,17);showhanzi(64,138,18);showhanzi(96,138,19);showhanzi(128,138,44);
	//横杠
	showdigit_color(64,170,11,WHITE,BLACK);showdigit_color(112,170,11,WHITE,BLACK);showdigit_color(224,170,11,WHITE,BLACK);showdigit_color(272,170,11,WHITE,BLACK);


	if(last_index!=_Time_Option)//判断是否是s第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS:
				if(cur_pos == 1) temp_year += 1;
				if(cur_pos == 2) temp_month += 1;
			    if(cur_pos == 3) temp_day += 1;
				if(cur_pos == 4) temp_h += 1;
				if(cur_pos == 5) temp_m += 1;
				if(cur_pos == 6) temp_s += 1;
			
				if(temp_year > 2099) temp_year=2099;
				if(temp_month >= 12) temp_month=12;	
				if(temp_day >= 31) temp_day=31;	
			    if(temp_h >= 23) temp_h=23;
				if(temp_m >= 59) temp_m=59;
			    if(temp_s >= 59) temp_s=59;
				RTC_Set(temp_year,temp_month,temp_day,temp_h,temp_m,temp_s);
					break;
			case KEY_ENTER://确定(设置)按键
				cur_pos++;
				if(cur_pos == 7){
					cur_pos = 1;
				}
					break;
			case KEY_NEXT:
				if(cur_pos == 1) temp_year -= 1;
				if(cur_pos == 2) temp_month -= 1;
			    if(cur_pos == 3) temp_day -= 1;
				if(cur_pos == 4) temp_h -= 1;
				if(cur_pos == 5) temp_m -= 1;
				if(cur_pos == 6) temp_s -= 1;
			
				if(temp_year < 1970) temp_year=1970;
				if(temp_month == 0) temp_month=1;	
				if(temp_day == 0) temp_day=1;	
			    if(temp_h == 255) temp_h=0;
				if(temp_m == 255) temp_m=0;
			    if(temp_s == 255) temp_s=0;
			    RTC_Set(temp_year,temp_month,temp_day,temp_h,temp_m,temp_s);
					break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零
	
	if(cur_pos!=1){showdigit_color(0,170,temp_year/1000,WHITE,BLACK);showdigit_color(16,170,(temp_year/100)%10,WHITE,BLACK);showdigit_color(32,170,(temp_year/10)%10,WHITE,BLACK);showdigit_color(48,170,temp_year%10,WHITE,BLACK);}
	if(cur_pos!=2){showdigit_color(80,170,temp_month/10,WHITE,BLACK);showdigit_color(96,170,temp_month%10,WHITE,BLACK);}
	if(cur_pos!=3){showdigit_color(128,170,temp_day/10,WHITE,BLACK);showdigit_color(144,170,temp_day%10,WHITE,BLACK);}
	if(cur_pos!=4){showdigit_color(192,170,temp_h/10,WHITE,BLACK);showdigit_color(208,170,temp_h%10,WHITE,BLACK);}
	if(cur_pos!=5){showdigit_color(240,170,temp_m/10,WHITE,BLACK);showdigit_color(256,170,temp_m%10,WHITE,BLACK);}
	if(cur_pos!=6){showdigit_color(288,170,temp_s/10,WHITE,BLACK);showdigit_color(304,170,temp_s%10,WHITE,BLACK);}
	switch(cur_pos)
	{
		case 1:
			showdigit_color(0,170,temp_year/1000,WHITE,BLUE);showdigit_color(16,170,(temp_year/100)%10,WHITE,BLUE);showdigit_color(32,170,(temp_year/10)%10,WHITE,BLUE);showdigit_color(48,170,temp_year%10,WHITE,BLUE);
			break;
		case 2:
			showdigit_color(80,170,temp_month/10,WHITE,BLUE);showdigit_color(96,170,temp_month%10,WHITE,BLUE);
			break;
		case 3:
			showdigit_color(128,170,temp_day/10,WHITE,BLUE);showdigit_color(144,170,temp_day%10,WHITE,BLUE);
			break;
		case 4:
			showdigit_color(192,170,temp_h/10,WHITE,BLUE);showdigit_color(208,170,temp_h%10,WHITE,BLUE);
			break;
		case 5:
			showdigit_color(240,170,temp_m/10,WHITE,BLUE);showdigit_color(256,170,temp_m%10,WHITE,BLUE);
			break;
		case 6:
			showdigit_color(288,170,temp_s/10,WHITE,BLUE);showdigit_color(304,170,temp_s%10,WHITE,BLUE);
			break;
		default:break;
	}
}
void Data_Change_Child(u8 page_index,u8 key_val){
	
	static u8 cur_pos=1;

//	extern float temp1_correct;// 
//	extern float temp2_correct;// 
//	extern float temp3_correct;//
	
	showhanzi(0,62,39);showhanzi(32,62,66);showhanzi(64,62,63);showhanzi(96,62,20);showhanzi(128,62,21);showhanzi(160,62,88);showhanzi(192,62,44);
	showhanzi(0,124,39);showhanzi(32,124,66);showhanzi(64,124,64);showhanzi(96,124,20);showhanzi(128,124,21);showhanzi(160,124,88);showhanzi(192,124,44);
	showhanzi(0,186,39);showhanzi(32,186,66);showhanzi(64,186,65);showhanzi(96,186,20);showhanzi(128,186,21);showhanzi(160,186,88);showhanzi(192,186,44);
	
	
	if(last_index!=_Data_Change_Option)//判断是否是s第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS:
				if(cur_pos == 1) temp1_correct += 0.1f;
				if(cur_pos == 2) temp2_correct += 0.1f;
			    if(cur_pos == 3) temp3_correct += 0.1f;
//				if(temp1_correct>=10)temp1_correct=9.9;
//				if(temp2_correct>=10)temp2_correct=9.9;
//				if(temp3_correct>=10)temp3_correct=9.9;			
					break;
			case KEY_ENTER://确定(设置)按键
				cur_pos++;
				if(cur_pos == 4){
					cur_pos = 1;
				}
					break;
			case KEY_NEXT:
				if(cur_pos == 1) temp1_correct -= 0.1f;
				if(cur_pos == 2) temp2_correct -= 0.1f;
			    if(cur_pos == 3) temp3_correct -= 0.1f;
				if(temp1_correct<0)temp1_correct=0.0;
				if(temp2_correct<0)temp2_correct=0.0;
				if(temp3_correct<0)temp3_correct=0.0;		
					break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零

	switch(cur_pos)
	{
		case 1:
			showdigit_color(224,62,(u8)temp1_correct%10,BLACK,WHITE);showdigit_color(240,62,10,BLACK,WHITE);showdigit_color(256,62,(u8)(temp1_correct*10)%10,BLACK,WHITE);
			showdigit_color(224,124,(u8)temp2_correct%10,WHITE,BLACK);showdigit_color(240,124,10,WHITE,BLACK);showdigit_color(256,124,(u8)(temp2_correct*10)%10,WHITE,BLACK);
			showdigit_color(224,186,(u8)temp3_correct%10,WHITE,BLACK);showdigit_color(240,186,10,WHITE,BLACK);showdigit_color(256,186,(u8)(temp3_correct*10)%10,WHITE,BLACK);
			break;
		case 2:
			showdigit_color(224,62,(u8)temp1_correct%10,WHITE,BLACK);showdigit_color(240,62,10,WHITE,BLACK);showdigit_color(256,62,(u8)(temp1_correct*10)%10,WHITE,BLACK);
			showdigit_color(224,124,(u8)temp2_correct%10,BLACK,WHITE);showdigit_color(240,124,10,BLACK,WHITE);showdigit_color(256,124,(u8)(temp2_correct*10)%10,BLACK,WHITE);
			showdigit_color(224,186,(u8)temp3_correct%10,WHITE,BLACK);showdigit_color(240,186,10,WHITE,BLACK);showdigit_color(256,186,(u8)(temp3_correct*10)%10,WHITE,BLACK);

			break;
		case 3:
			showdigit_color(224,62,(u8)temp1_correct%10,WHITE,BLACK);showdigit_color(240,62,10,WHITE,BLACK);showdigit_color(256,62,(u8)(temp1_correct*10)%10,WHITE,BLACK);
			showdigit_color(224,124,(u8)temp2_correct%10,WHITE,BLACK);showdigit_color(240,124,10,WHITE,BLACK);showdigit_color(256,124,(u8)(temp2_correct*10)%10,WHITE,BLACK);
			showdigit_color(224,186,(u8)temp3_correct%10,BLACK,WHITE);showdigit_color(240,186,10,BLACK,WHITE);showdigit_color(256,186,(u8)(temp3_correct*10)%10,BLACK,WHITE);
			break;
		default:break;
	}
	
}
void Iot_Child(u8 page_index,u8 key_val){

	showhanzi(28,74,78);showhanzi(64,74,97);showhanzi(96,74,98);showhanzi(128,74,99);showhanzi(160,74,100);
	showdigit_color(192,74,15,BLUE,BLACK);showdigit_color(208,74,16,BLUE,BLACK);showdigit_color(224,74,16,BLUE,BLACK);
	
	
	showhanzi(28,138,79);showhanzi(64,138,101);showhanzi(96,138,102);showhanzi(128,138,103);showhanzi(160,138,104);

	
	LCD_ShowString(64,202,16,"IMEI:",0);LCD_ShowString(104,202,16,imei_no,0);
//	showdigit_color(20,230,15,BLUE,BLACK);
//	showdigit_color(36,230,16,BLUE,BLACK);
//	showdigit_color(52,230,17,BLUE,BLACK);
//	showdigit_color(68,230,18,BLUE,BLACK);
//	showdigit_color(84,230,12,BLUE,BLACK);
//	u8 len = strlen((char *)imei_no);
//	u16 sour =100;
//    for (int i = 0; i < len; i++) {
//        if (isdigit(imei_no[i])) {
//            // 如果你想将字符转换为整数，可以使用下面的代码
//            u8 digit = imei_no[i] - '0';
//			showdigit_color(sour,230,digit,GREEN,BLACK);
//            // printf("%d ", digit);
//        }
//		sour+=16;
//    }
}




void Air_Blower_Child_Ctrl(u8 page_index,u8 key_val){

	
	#if (ROAD_COUNT==10)
	if(page_index!=_Air_Blower_Child_1){showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); }
	if(page_index!=_Air_Blower_Child_2){showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27); }
	if(page_index!=_Air_Blower_Child_3){showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);}
	if(page_index!=_Air_Blower_Child_4){showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27); }
	if(page_index!=_Air_Blower_Child_5){showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); }
	if(page_index!=_Air_Blower_Child_6){showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);}
	if(page_index!=_Air_Blower_Child_7){showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  }
	if(page_index!=_Air_Blower_Child_8){showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);}
	if(page_index!=_Air_Blower_Child_9){showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  }
	if(page_index!=_Air_Blower_Child_10){showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); }
	#endif
	
	#if (ROAD_COUNT==8)
	if(page_index!=_Air_Blower_Child_1){showhanzi(11,51,6);showhanzi(43,51,7);showhanzi(75,51,26);showhanzi(107,51,27); }
	if(page_index!=_Air_Blower_Child_2){showhanzi(11,113,6);showhanzi(43,113,7);showhanzi(75,113,28);showhanzi(107,113,27); }
	if(page_index!=_Air_Blower_Child_3){showhanzi(11,175,6);showhanzi(43,175,7);showhanzi(75,175,29);showhanzi(107,175,27);}
	if(page_index!=_Air_Blower_Child_4){showhanzi(11,237,6);showhanzi(43,237,7);showhanzi(75,237,30);showhanzi(107,237,27); }
	if(page_index!=_Air_Blower_Child_5){showhanzi(340,51,6);showhanzi(372,51,7);showhanzi(404,51,31);showhanzi(436,51,27); }
	if(page_index!=_Air_Blower_Child_6){showhanzi(340,113,6);showhanzi(372,113,7);showhanzi(404,113,32);showhanzi(436,113,27);}
	if(page_index!=_Air_Blower_Child_7){showhanzi(340,175,6);showhanzi(372,175,7);showhanzi(404,175,33);showhanzi(436,175,27);  }
	if(page_index!=_Air_Blower_Child_8){showhanzi(340,237,6);showhanzi(372,237,7);showhanzi(404,237,34);showhanzi(436,237,27);}
	#endif
	
	#if (ROAD_COUNT==4)
	if(page_index!=_Air_Blower_Child_1){showhanzi(176,51,6);showhanzi(208,51,7);showhanzi(240,51,26);showhanzi(272,51,27); }
	if(page_index!=_Air_Blower_Child_2){showhanzi(176,113,6);showhanzi(208,113,7);showhanzi(240,113,28);showhanzi(272,113,27); }
	if(page_index!=_Air_Blower_Child_3){showhanzi(176,175,6);showhanzi(208,175,7);showhanzi(240,175,29);showhanzi(272,175,27);}
	if(page_index!=_Air_Blower_Child_4){showhanzi(176,237,6);showhanzi(208,237,7);showhanzi(240,237,30);showhanzi(272,237,27); }
	#endif

	switch(page_index)
	{
		#if (ROAD_COUNT==10)
		case _Air_Blower_Child_1://显示风机1
			showhanzi_1(6,36,6,1);showhanzi_1(38,36,7,1);showhanzi_1(70,36,26,1);showhanzi_1(102,36,27,1); 
			break;
		case _Air_Blower_Child_2://显示风机2
			showhanzi_1(6,72,6,1);showhanzi_1(38,72,7,1);showhanzi_1(70,72,28,1);showhanzi_1(102,72,27,1);  
			break;
		case _Air_Blower_Child_3://显示风机3
			showhanzi_1(6,108,6,1);showhanzi_1(38,108,7,1);showhanzi_1(70,108,29,1);showhanzi_1(102,108,27,1);
			break;
		case _Air_Blower_Child_4://显示风机
			showhanzi_1(6,144,6,1);showhanzi_1(38,144,7,1);showhanzi_1(70,144,30,1);showhanzi_1(102,144,27,1);  
			break;
		case _Air_Blower_Child_5://显示风机
			showhanzi_1(6,180,6,1);showhanzi_1(38,180,7,1);showhanzi_1(70,180,31,1);showhanzi_1(102,180,27,1); 
			break;
		case _Air_Blower_Child_6://显示风机
			showhanzi_1(346,36,6,1);showhanzi_1(378,36,7,1);showhanzi_1(410,36,32,1);showhanzi_1(442,36,27,1);  
			break;
		case _Air_Blower_Child_7://显示风机
			showhanzi_1(346,72,6,1);showhanzi_1(378,72,7,1);showhanzi_1(410,72,33,1);showhanzi_1(442,72,27,1);  
			break;
		case _Air_Blower_Child_8://显示风机
			showhanzi_1(346,108,6,1);showhanzi_1(378,108,7,1);showhanzi_1(410,108,34,1);showhanzi_1(442,108,27,1);
			break;
		case _Air_Blower_Child_9://显示风机
			showhanzi_1(346,144,6,1);showhanzi_1(378,144,7,1);showhanzi_1(410,144,35,1);showhanzi_1(442,144,27,1);  
			break;
		case _Air_Blower_Child_10://显示风机
			showhanzi_1(346,180,6,1);showhanzi_1(378,180,7,1);showdigit_color(410,180,1,BLACK,WHITE);showdigit_color(426,180,0,BLACK,WHITE);showhanzi_1(442,180,27,1); 
			break;
		#endif
		
		#if (ROAD_COUNT==8)
		case _Air_Blower_Child_1://显示风机1
			showhanzi_1(11,51,6,1);showhanzi_1(43,51,7,1);showhanzi_1(75,51,26,1);showhanzi_1(107,51,27,1); 
			break;
		case _Air_Blower_Child_2://显示风机2
			showhanzi_1(11,113,6,1);showhanzi_1(43,113,7,1);showhanzi_1(75,113,28,1);showhanzi_1(107,113,27,1);  
			break;
		case _Air_Blower_Child_3://显示风机3
			showhanzi_1(11,175,6,1);showhanzi_1(43,175,7,1);showhanzi_1(75,175,29,1);showhanzi_1(107,175,27,1);
			break;
		case _Air_Blower_Child_4://显示风机
			showhanzi_1(11,237,6,1);showhanzi_1(43,237,7,1);showhanzi_1(75,237,30,1);showhanzi_1(107,237,27,1);  
			break;
		case _Air_Blower_Child_5://显示风机
			showhanzi_1(340,51,6,1);showhanzi_1(372,51,7,1);showhanzi_1(404,51,31,1);showhanzi_1(436,51,27,1); 
			break;
		case _Air_Blower_Child_6://显示风机
			showhanzi_1(340,113,6,1);showhanzi_1(372,113,7,1);showhanzi_1(404,113,32,1);showhanzi_1(436,113,27,1);  
			break;
		case _Air_Blower_Child_7://显示风机
			showhanzi_1(340,175,6,1);showhanzi_1(372,175,7,1);showhanzi_1(404,175,33,1);showhanzi_1(436,175,27,1);  
			break;
		case _Air_Blower_Child_8://显示风机8
			showhanzi_1(340,237,6,1);showhanzi_1(372,237,7,1);showhanzi_1(404,237,34,1);showhanzi_1(436,237,27,1);
			break;
		#endif
		
		#if (ROAD_COUNT==4)
		case _Air_Blower_Child_1://显示风机1
			showhanzi_1(176,51,6,1);showhanzi_1(208,51,7,1);showhanzi_1(240,51,26,1);showhanzi_1(272,51,27,1); 
			break;
		case _Air_Blower_Child_2://显示风机2
			showhanzi_1(176,113,6,1);showhanzi_1(208,113,7,1);showhanzi_1(240,113,28,1);showhanzi_1(272,113,27,1);  
			break;
		case _Air_Blower_Child_3://显示风机3
			showhanzi_1(176,175,6,1);showhanzi_1(208,175,7,1);showhanzi_1(240,175,29,1);showhanzi_1(272,175,27,1);
			break;
		case _Air_Blower_Child_4://显示风机4
			showhanzi_1(176,237,6,1);showhanzi_1(208,237,7,1);showhanzi_1(240,237,30,1);showhanzi_1(272,237,27,1);  
			break;
		#endif
		default:break;		
	}
}



void Air_Blower_Child_Ctrl_1_Fun(u8 page_index,u8 key_val){
	
	//保存原始的结构体,用于判断用户是否操作
    //Relay_Structure original_relay_structure[10];

	
	//static u8 last_index_save=0;
	
	
	
	//static Time_C time_c_v1[5]={0};
	//static Time_C time_c_v[5]={0};
	
//	static u8 cur_pos1=1;
//	static u8 cur_pos2=1;
//	static u8 cur_pos3=1;
//	static u8 cur_pos4=1;


	//static u8 enter_config_flag=0;//设为全局变量
//	printf("last_index:%d\n",last_index);
//	printf("page_index:%d\n",page_index);
//		printf("func_index:%d\n",func_index);

	
	if(
	   #if (ROAD_COUNT==10)
	   last_index!=_Air_Blower_Child_1 && last_index!=_Air_Blower_Child_2 && last_index!=_Air_Blower_Child_3 &&
	   last_index!=_Air_Blower_Child_4 && last_index!=_Air_Blower_Child_5 && last_index!=_Air_Blower_Child_6 &&
	   last_index!=_Air_Blower_Child_7 && last_index!=_Air_Blower_Child_8 && last_index!=_Air_Blower_Child_9 &&
	   last_index!=_Air_Blower_Child_10
	   #endif
	   #if (ROAD_COUNT==8)
	   last_index!=_Air_Blower_Child_1 && last_index!=_Air_Blower_Child_2 && last_index!=_Air_Blower_Child_3 &&
	   last_index!=_Air_Blower_Child_4 && last_index!=_Air_Blower_Child_5 && last_index!=_Air_Blower_Child_6 &&
	   last_index!=_Air_Blower_Child_7 && last_index!=_Air_Blower_Child_8
	   #endif
	   #if (ROAD_COUNT==4)
	   last_index!=_Air_Blower_Child_1 && last_index!=_Air_Blower_Child_2 && last_index!=_Air_Blower_Child_3 && last_index!=_Air_Blower_Child_4
	   #endif
	  )//判断是否是s第一次进入此界面
	{

	switch(key_val)
		{
			case KEY_PREVIOUS:
				if(enter_config_flag_1 == 0 && enter_config_flag_2 == 0 && enter_config_flag_3 == 0  && enter_config_flag_4 == 0 && enter_config_flag_5 == 0 
					&& enter_config_flag_6 == 0 && enter_config_flag_7 == 0 && enter_config_flag_8 == 0  && enter_config_flag_9 == 0 && enter_config_flag_10 == 0
				){
					relay_mode_buf[last_index_save-12] +=1;
					if(relay_mode_buf[last_index_save-12]==5)relay_mode_buf[last_index_save-12]=0;//不使用该风机
				}else{
					if(relay_structure_temp[last_index_save-12].relay_mode == 0){
						relay_structure_temp[last_index_save-12].no_Ctrl.all_open_or_close=!(relay_structure_temp[last_index_save-12].no_Ctrl.all_open_or_close);
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 1){
						if(cur_pos1 == 1)relay_structure_temp[last_index_save-12].temp_control.max_temp+=1;
						if(cur_pos1 == 2){if((relay_structure_temp[last_index_save-12].temp_control.min_temp+1)<relay_structure_temp[last_index_save-12].temp_control.max_temp)relay_structure_temp[last_index_save-12].temp_control.min_temp+=1;}
						if(cur_pos1 == 3)relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag+=1;
						if(cur_pos1 == 4)relay_structure_temp[last_index_save-12].temp_control.startup_mode = !(relay_structure_temp[last_index_save-12].temp_control.startup_mode);
						if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 4) relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag = 0;
						if(cur_pos1 == 5)relay_structure_temp[last_index_save-12].temp_control.max_nh3+=1;
						if(cur_pos1 == 6){if((relay_structure_temp[last_index_save-12].temp_control.min_nh3+1)<relay_structure_temp[last_index_save-12].temp_control.max_nh3)relay_structure_temp[last_index_save-12].temp_control.min_nh3+=1;}
						if(relay_structure_temp[last_index_save-12].temp_control.max_nh3>=50)relay_structure_temp[last_index_save-12].temp_control.max_nh3=50;
						//if(relay_structure_temp[last_index_save-12].temp_control.min_nh3>=50)relay_structure_temp[last_index_save-12].temp_control.min_nh3=50;
						if(relay_structure_temp[last_index_save-12].temp_control.max_temp>90)relay_structure_temp[last_index_save-12].temp_control.max_temp=90;
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 2){
						if(cur_pos2 == 1) relay_structure_temp[last_index_save-12].time_control.time_open += 1;
				        if(cur_pos2 == 2) relay_structure_temp[last_index_save-12].time_control.time_stop += 1;
						if(relay_structure_temp[last_index_save-12].time_control.time_open==10000)relay_structure_temp[last_index_save-12].time_control.time_open=0;
						if(relay_structure_temp[last_index_save-12].time_control.time_stop==10000)relay_structure_temp[last_index_save-12].time_control.time_stop=0;
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 3){
						if(cur_pos3 == 1)relay_structure_temp[last_index_save-12].temp_control.max_temp+=1;
						if(cur_pos3 == 2){if((relay_structure_temp[last_index_save-12].temp_control.min_temp+1)<relay_structure_temp[last_index_save-12].temp_control.max_temp)relay_structure_temp[last_index_save-12].temp_control.min_temp+=1;}
						if(cur_pos3 == 3)relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag+=1;
						if(cur_pos3 == 4)relay_structure_temp[last_index_save-12].temp_control.startup_mode = !(relay_structure_temp[last_index_save-12].temp_control.startup_mode);
						if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 4) relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag = 0;
						if(cur_pos3 == 5) relay_structure_temp[last_index_save-12].time_control.time_open += 1;
				        if(cur_pos3 == 6) relay_structure_temp[last_index_save-12].time_control.time_stop += 1;
						if(relay_structure_temp[last_index_save-12].time_control.time_open==10000)relay_structure_temp[last_index_save-12].time_control.time_open=0;
						if(relay_structure_temp[last_index_save-12].time_control.time_stop==10000)relay_structure_temp[last_index_save-12].time_control.time_stop=0;
						if(relay_structure_temp[last_index_save-12].temp_control.max_temp>90)relay_structure_temp[last_index_save-12].temp_control.max_temp=90;
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 4){
						if(cur_pos4 == 1)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_start+=1;
						if(cur_pos4 == 2)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_start+=1;
						if(cur_pos4 == 3)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_stop+=1;
						if(cur_pos4 == 4)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_stop+=1;					
						if(cur_pos4 == 5)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off;
						if(cur_pos4 == 6)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_start+=1;
						if(cur_pos4 == 7)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_start+=1;
						if(cur_pos4 == 8)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_stop+=1;
						if(cur_pos4 == 9)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_stop+=1;
						if(cur_pos4 == 10)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off;
						if(cur_pos4 == 11)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_start+=1;
						if(cur_pos4 == 12)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_start+=1;
						if(cur_pos4 == 13)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_stop+=1;
						if(cur_pos4 == 14)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_stop+=1;
						if(cur_pos4 == 15)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off;
						if(cur_pos4 == 16)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_start+=1;
						if(cur_pos4 == 17)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_start+=1;
						if(cur_pos4 == 18)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_stop+=1;
						if(cur_pos4 == 19)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_stop+=1;
						if(cur_pos4 == 20)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off;
						if(cur_pos4 == 21)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_start+=1;
						if(cur_pos4 == 22)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_start+=1;
						if(cur_pos4 == 23)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_stop+=1;
						if(cur_pos4 == 24)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_stop+=1;
						if(cur_pos4 == 25)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off;
						
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_start == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_start == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_start == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_start == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_start == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_start == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_start == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_start == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_start == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_start = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_start == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_start = 0;

						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_stop == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_stop == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_stop == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_stop == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_stop == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_stop == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_stop == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_stop == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_stop == 24) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_stop = 0;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_stop == 61) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_stop = 0;
					}

				}
				
				
					break;			
			case KEY_ENTER://确定(设置)按键
				#if(ROAD_COUNT==10)
				if(last_index_save-12==0){enter_config_flag_1 += 1;}
				if(last_index_save-12==1){enter_config_flag_2 += 1;}
				if(last_index_save-12==2){enter_config_flag_3 += 1;}
				if(last_index_save-12==3){enter_config_flag_4 += 1;}
				if(last_index_save-12==4){enter_config_flag_5 += 1;}
				if(last_index_save-12==5){enter_config_flag_6 += 1;}
				if(last_index_save-12==6){enter_config_flag_7 += 1;}
				if(last_index_save-12==7){enter_config_flag_8 += 1;}
				if(last_index_save-12==8){enter_config_flag_9 += 1;}
				if(last_index_save-12==9){enter_config_flag_10 += 1;}
				#endif
				#if(ROAD_COUNT==8)
				if(last_index_save-12==0){enter_config_flag_1 += 1;}
				if(last_index_save-12==1){enter_config_flag_2 += 1;}
				if(last_index_save-12==2){enter_config_flag_3 += 1;}
				if(last_index_save-12==3){enter_config_flag_4 += 1;}
				if(last_index_save-12==4){enter_config_flag_5 += 1;}
				if(last_index_save-12==5){enter_config_flag_6 += 1;}
				if(last_index_save-12==6){enter_config_flag_7 += 1;}
				if(last_index_save-12==7){enter_config_flag_8 += 1;}
				#endif
				#if(ROAD_COUNT==4)
				if(last_index_save-12==0){enter_config_flag_1 += 1;}
				if(last_index_save-12==1){enter_config_flag_2 += 1;}
				if(last_index_save-12==2){enter_config_flag_3 += 1;}
				if(last_index_save-12==3){enter_config_flag_4 += 1;}
				#endif
				relay_structure_temp[last_index_save-12].relay_mode=relay_mode_buf[last_index_save-12];
				
				//printf("%d\n---",enter_config_flag_1);
				//设置刷新标志位为刷新
				if(enter_config_flag_1>1 || enter_config_flag_2>1||enter_config_flag_3>1||enter_config_flag_4>1||enter_config_flag_5>1
					||enter_config_flag_6>1||enter_config_flag_7>1||enter_config_flag_8>1||enter_config_flag_9>1||enter_config_flag_10>1
				){
					
					if(relay_structure_temp[last_index_save-12].relay_mode == 1){
						cur_pos1++;
						if(cur_pos1 == 7){
							cur_pos1 = 1;
						}
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 2){
						cur_pos2++;
						if(cur_pos2 == 3){
							cur_pos2 = 1;
						}
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 3){
						cur_pos3++;
						if(cur_pos3 == 7){
							cur_pos3 = 1;
						}
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 4){
						cur_pos4++;
						if(cur_pos4 == 26){
							cur_pos4 = 1;
						}
					}
				}
					break;
			case KEY_NEXT:
				if(enter_config_flag_1 == 0 && enter_config_flag_2 == 0 && enter_config_flag_3 == 0  && enter_config_flag_4 == 0 && enter_config_flag_5 == 0 
					&& enter_config_flag_6 == 0 && enter_config_flag_7 == 0 && enter_config_flag_8 == 0  && enter_config_flag_9 == 0 && enter_config_flag_10 == 0
				){
					relay_mode_buf[last_index_save-12] -=1;
					if(relay_mode_buf[last_index_save-12]==255)relay_mode_buf[last_index_save-12]=4;//分时控
				}else{
					if(relay_structure_temp[last_index_save-12].relay_mode == 0){
						relay_structure_temp[last_index_save-12].no_Ctrl.all_open_or_close=!(relay_structure_temp[last_index_save-12].no_Ctrl.all_open_or_close);
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 1){
						if(cur_pos1 == 1){if((relay_structure_temp[last_index_save-12].temp_control.max_temp-1)>relay_structure_temp[last_index_save-12].temp_control.min_temp) relay_structure_temp[last_index_save-12].temp_control.max_temp-=1;}
						if(cur_pos1 == 2)relay_structure_temp[last_index_save-12].temp_control.min_temp-=1;
						if(cur_pos1 == 3)relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag+=1;
						if(cur_pos1 == 4)relay_structure_temp[last_index_save-12].temp_control.startup_mode = !(relay_structure_temp[last_index_save-12].temp_control.startup_mode);
						if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 4) relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag = 0;
						if(cur_pos1 == 5){if((relay_structure_temp[last_index_save-12].temp_control.max_nh3-1)>relay_structure_temp[last_index_save-12].temp_control.min_nh3)relay_structure_temp[last_index_save-12].temp_control.max_nh3-=1;}
						if(cur_pos1 == 6)relay_structure_temp[last_index_save-12].temp_control.min_nh3-=1;
						//if(relay_structure_temp[last_index_save-12].temp_control.max_nh3<=1)relay_structure_temp[last_index_save-12].temp_control.max_nh3=1;
						if(relay_structure_temp[last_index_save-12].temp_control.min_nh3<=1)relay_structure_temp[last_index_save-12].temp_control.min_nh3=1;
						if(relay_structure_temp[last_index_save-12].temp_control.min_temp==255)relay_structure_temp[last_index_save-12].temp_control.min_temp=0;
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 2){
						if(cur_pos2 == 1) relay_structure_temp[last_index_save-12].time_control.time_open -= 1;
				        if(cur_pos2 == 2) relay_structure_temp[last_index_save-12].time_control.time_stop -= 1;
						if(relay_structure_temp[last_index_save-12].time_control.time_open==65535)relay_structure_temp[last_index_save-12].time_control.time_open=9999;
						if(relay_structure_temp[last_index_save-12].time_control.time_stop==65535)relay_structure_temp[last_index_save-12].time_control.time_stop=9999;
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 3){
						if(cur_pos3 == 1){if((relay_structure_temp[last_index_save-12].temp_control.max_temp-1)>relay_structure_temp[last_index_save-12].temp_control.min_temp) relay_structure_temp[last_index_save-12].temp_control.max_temp-=1;}
						if(cur_pos3 == 2)relay_structure_temp[last_index_save-12].temp_control.min_temp-=1;
						if(cur_pos3 == 3)relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag+=1;
						if(cur_pos3 == 4)relay_structure_temp[last_index_save-12].temp_control.startup_mode = !(relay_structure_temp[last_index_save-12].temp_control.startup_mode);
						if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 4) relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag = 0;
						
						if(cur_pos3 == 5) relay_structure_temp[last_index_save-12].time_control.time_open -= 1;
				        if(cur_pos3 == 6) relay_structure_temp[last_index_save-12].time_control.time_stop -= 1;
						if(relay_structure_temp[last_index_save-12].time_control.time_open==65535)relay_structure_temp[last_index_save-12].time_control.time_open=9999;
						if(relay_structure_temp[last_index_save-12].time_control.time_stop==65535)relay_structure_temp[last_index_save-12].time_control.time_stop=9999;
						if(relay_structure_temp[last_index_save-12].temp_control.min_temp==255)relay_structure_temp[last_index_save-12].temp_control.min_temp=0;
					}
					if(relay_structure_temp[last_index_save-12].relay_mode == 4){
						if(cur_pos4 == 1)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_start-=1;
						if(cur_pos4 == 2)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_start-=1;
						if(cur_pos4 == 3)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_stop-=1;
						if(cur_pos4 == 4)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_stop-=1;					
						if(cur_pos4 == 5)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off;
						if(cur_pos4 == 6)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_start-=1;
						if(cur_pos4 == 7)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_start-=1;
						if(cur_pos4 == 8)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_stop-=1;
						if(cur_pos4 == 9)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_stop-=1;
						if(cur_pos4 == 10)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off;
						if(cur_pos4 == 11)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_start-=1;
						if(cur_pos4 == 12)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_start-=1;
						if(cur_pos4 == 13)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_stop-=1;
						if(cur_pos4 == 14)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_stop-=1;
						if(cur_pos4 == 15)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off;
						if(cur_pos4 == 16)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_start-=1;
						if(cur_pos4 == 17)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_start-=1;
						if(cur_pos4 == 18)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_stop-=1;
						if(cur_pos4 == 19)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_stop-=1;
						if(cur_pos4 == 20)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off;
						if(cur_pos4 == 21)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_start-=1;
						if(cur_pos4 == 22)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_start-=1;
						if(cur_pos4 == 23)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_stop-=1;
						if(cur_pos4 == 24)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_stop-=1;
						if(cur_pos4 == 25)relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off=!relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off;
						
						
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_start = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_start = 60;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_start = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_start = 60;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_start = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_start = 60;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_start = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_start = 60;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_start = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_start == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_start = 60;

						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_stop = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_stop = 60;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_stop = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_stop = 60;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_stop = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_stop = 60;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_stop = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_stop = 60;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_stop = 23;
						if(relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_stop == 255) relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_stop = 60;
					
					}
					
				}
				
					break;		
//			case KEY_BACK:
//				send_CONFIG_INDEX = last_index_save;
//				// 检查是否发生了变化
//				printf("KEY_BACK\n");
//				if (memcmp(relay_structure, relay_structure_temp, sizeof(relay_structure)) != 0) {
//					send_CONFIG_FLAG=1;
//					memcpy(relay_structure, relay_structure_temp, sizeof(relay_structure));
//					printf("---%d---\n",relay_structure[0].temp_control.max_temp);
//				}

//					break;
			default:break;
		}
    }else {
		last_index_save=last_index;
		relay_mode_buf[last_index_save-12]=relay_structure[last_index_save-12].relay_mode;
		memcpy(relay_structure_temp, relay_structure, sizeof(relay_structure));
		//memcpy(original_relay_structure, relay_structure, sizeof(relay_structure));
	
	}//第一次进入此界面,界面指针清零

	
	// 风机mode 
	//enter_config_flag 是否已进入配置界面
	//cur_pos
	//printf("relay_structure[last_index-12].relay_mode:%d\n",relay_structure[last_index-12].relay_mode);
	
	if(enter_config_flag_1==0 && enter_config_flag_2==0 && enter_config_flag_3==0 && enter_config_flag_4==0 && enter_config_flag_5==0
		&& enter_config_flag_6==0 && enter_config_flag_7==0 && enter_config_flag_8==0 && enter_config_flag_9==0 && enter_config_flag_10==0
		){
		fresh_flag_1 = 1;
		fresh_flag_2 = 1;
		fresh_flag_3 = 1;
		fresh_flag_4 = 1;
		fresh_flag_5 = 1;
		fresh_flag_6 = 1;
		fresh_flag_7 = 1;
		fresh_flag_8 = 1;
		fresh_flag_9 = 1;
		fresh_flag_10 = 1;

		switch(relay_mode_buf[last_index_save-12]){
					
			case 0:
				showhanzi(160,35,92);showhanzi(192,35,90);showhanzi(224,35,91);//不使用
				showhanzi(128,109,151);showhanzi(192,109,71);//常关
				showhanzi(128,178,151);showhanzi(192,178,70);//常开
				if(relay_structure_temp[last_index_save-12].no_Ctrl.all_open_or_close){
					showhanzi(240,109,150);showhanzi(240,178,149);
				}else{
					showhanzi(240,109,149);showhanzi(240,178,150);
				}
					break;	
			case 1:
			//显示温控信息
				u8 max_temp_v1 = relay_structure_temp[last_index_save-12].temp_control.max_temp;
				u8 min_temp_v1 = relay_structure_temp[last_index_save-12].temp_control.min_temp;
				u16 max_nh3_v1 = relay_structure_temp[last_index_save-12].temp_control.max_nh3;
				u16 min_nh3_v1 = relay_structure_temp[last_index_save-12].temp_control.min_nh3;
				showhanzi(160,35,39);showhanzi(192,35,10);showhanzi(224,35,38);
			
				showhanzi(6,67,41);showhanzi(38,67,42);showhanzi(70,67,39);showhanzi(102,67,44);//最高温
				showhanzi(6,103,41);showhanzi(38,103,43);showhanzi(70,103,39); showhanzi(102,103,44);//最低温
				showhanzi(6,139,50);showhanzi(38,139,51);showhanzi(70,139,52); showhanzi(102,139,53);showhanzi(134,139,54);showhanzi(166,139,44);//对应传感器
				showhanzi(6,175,55);showhanzi(38,175,56);showhanzi(70,175,57); showhanzi(102,175,58);showhanzi(134,175,44);//启动方式
				showhanzi(6,211,105);showhanzi(38,211,106);showhanzi(70,211,41); showhanzi(102,211,42);showhanzi(134,211,88);showhanzi(166,211,44);//氨气最高值
				showhanzi(6,247,105);showhanzi(38,247,106);showhanzi(70,247,41); showhanzi(102,247,43);showhanzi(134,247,88);showhanzi(166,247,44);//氨气最低值
				
				
			    showdigit_color(134,67,max_temp_v1/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v1%10,WHITE,BLACK);
			    showdigit_color(134,103,min_temp_v1/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v1%10,WHITE,BLACK);
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure_temp[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(200,211,max_nh3_v1/100,WHITE,BLACK);showdigit_color(216,211,(max_nh3_v1/10)%10,WHITE,BLACK);showdigit_color(232,211,max_nh3_v1%10,WHITE,BLACK);
			    showdigit_color(200,247,min_nh3_v1/100,WHITE,BLACK);showdigit_color(216,247,(min_nh3_v1/10)%10,WHITE,BLACK);showdigit_color(232,247,min_nh3_v1%10,WHITE,BLACK);
				
					break;
			case 2:
			//显示时控信息	
				uint16_t time_open_v1=relay_structure_temp[last_index_save-12].time_control.time_open;
		        uint16_t time_stop_v1=relay_structure_temp[last_index_save-12].time_control.time_stop;
				//显示时控
				showhanzi(160,35,18);showhanzi(192,35,10);showhanzi(224,35,38);
				//显示时控固定界面信息
				showhanzi(6,67,48);  showhanzi(38,67,49);  showhanzi(70,67,18);  showhanzi(102,67,19); showhanzi(134,67,44);//运转时间
				showhanzi(6,139,46);  showhanzi(38,139,47);  showhanzi(70,139,18); showhanzi(102,139,19);showhanzi(134,139,44);//停机时间
			
				showdigit_color(166,67,time_open_v1/1000,WHITE,BLACK);showdigit_color(182,67,(time_open_v1/100)%10,WHITE,BLACK);
			    showdigit_color(198,67,(time_open_v1/10)%10,WHITE,BLACK);showdigit_color(214,67,time_open_v1%10,WHITE,BLACK);
			
				showdigit_color(166,139,time_stop_v1/1000,WHITE,BLACK);showdigit_color(182,139,(time_stop_v1/100)%10,WHITE,BLACK);
			    showdigit_color(198,139,(time_stop_v1/10)%10,WHITE,BLACK);showdigit_color(214,139,time_stop_v1%10,WHITE,BLACK);
					break;
			case 3:
			//显示温时控信息
				u8 max_temp_v11 = relay_structure_temp[last_index_save-12].temp_control.max_temp;
				u8 min_temp_v11 = relay_structure_temp[last_index_save-12].temp_control.min_temp;
				uint16_t time_open_v11=relay_structure_temp[last_index_save-12].time_control.time_open;
		        uint16_t time_stop_v11=relay_structure_temp[last_index_save-12].time_control.time_stop;
				//温时控
				showhanzi(160,35,39);showhanzi(192,35,18);showhanzi(224,35,10);

				showhanzi(6,67,41);showhanzi(38,67,42);showhanzi(70,67,39);showhanzi(102,67,44);//最高温
				showhanzi(6,103,41);showhanzi(38,103,43);showhanzi(70,103,39); showhanzi(102,103,44);//最低温
				showhanzi(6,139,50);showhanzi(38,139,51);showhanzi(70,139,52); showhanzi(102,139,53);showhanzi(134,139,54);showhanzi(166,139,44);//对应传感器
				showhanzi(6,175,55);showhanzi(38,175,56);showhanzi(70,175,57); showhanzi(102,175,58);showhanzi(134,175,44);//启动方式
				
			    showdigit_color(134,67,max_temp_v11/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v11%10,WHITE,BLACK);
			    showdigit_color(134,103,min_temp_v11/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v11%10,WHITE,BLACK);
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure_temp[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				
				showhanzi(6,211,48);  showhanzi(38,211,49);  showhanzi(70,211,18);  showhanzi(102,211,19); showhanzi(134,211,44);//打开时间	
				showhanzi(6,247,46);  showhanzi(38,247,47);  showhanzi(70,247,18); showhanzi(102,247,19);showhanzi(134,247,44);//关闭时间
				
				showdigit_color(166,211,time_open_v11/1000,WHITE,BLACK);showdigit_color(182,211,(time_open_v11/100)%10,WHITE,BLACK);
			    showdigit_color(198,211,(time_open_v11/10)%10,WHITE,BLACK);showdigit_color(214,211,time_open_v11%10,WHITE,BLACK);
			
				showdigit_color(166,247,time_stop_v11/1000,WHITE,BLACK);showdigit_color(182,247,(time_stop_v11/100)%10,WHITE,BLACK);
			    showdigit_color(198,247,(time_stop_v11/10)%10,WHITE,BLACK);showdigit_color(214,247,time_stop_v11%10,WHITE,BLACK);
			
			
					break;
			case 4:
			//显示分时控信息
			    
//			printf("time_c_v1[0].time_h:%d",time_c_v1[0].time_h);
//			printf("time_c_v1[0].time_m:%d",time_c_v1[0].time_m);
//			printf("time_c_v1[0].time_s:%d",time_c_v1[0].time_s);
			    Time_C time_c_v1[5]={0};
			
				time_c_v1[0].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_start;
				time_c_v1[0].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_start;
				time_c_v1[0].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_stop;
				time_c_v1[0].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_stop;


				time_c_v1[1].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_start;
				time_c_v1[1].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_start;
				time_c_v1[1].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_stop;
				time_c_v1[1].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_stop;
				
				time_c_v1[2].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_start;
				time_c_v1[2].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_start;
				time_c_v1[2].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_stop;
				time_c_v1[2].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_stop;
				
				time_c_v1[3].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_start;
				time_c_v1[3].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_start;
				time_c_v1[3].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_stop;
				time_c_v1[3].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_stop;
				
				time_c_v1[4].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_start;
				time_c_v1[4].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_start;
				time_c_v1[4].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_stop;
				time_c_v1[4].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_stop;

				//分时控
				showhanzi(160,35,40);showhanzi(192,35,18);showhanzi(224,35,10);
				showhanzi_color(80,67,115,GREEN,BLACK);showhanzi_color(112,67,116,GREEN,BLACK);showhanzi_color(144,67,18,GREEN,BLACK);showhanzi_color(176,67,19,GREEN,BLACK);//起始时间
				showhanzi_color(272,67,117,RED,BLACK);showhanzi_color(304,67,118,RED,BLACK);showhanzi_color(336,67,18,RED,BLACK);showhanzi_color(368,67,19,RED,BLACK);//终止时间
				
				showhanzi(0,99,78);showhanzi(96,99,18);showhanzi(160,99,40);showhanzi(288,99,18);showhanzi(352,99,40);//1 时分秒
				showdigit_color(64,99,time_c_v1[0].time_h_start/10,WHITE,BLACK);showdigit_color(80,99,time_c_v1[0].time_h_start%10,WHITE,BLACK);
				showdigit_color(128,99,time_c_v1[0].time_m_start/10,WHITE,BLACK);showdigit_color(144,99,time_c_v1[0].time_m_start%10,WHITE,BLACK);
				showdigit_color(256,99,time_c_v1[0].time_h_stop/10,WHITE,BLACK);showdigit_color(272,99,time_c_v1[0].time_h_stop%10,WHITE,BLACK);
				showdigit_color(320,99,time_c_v1[0].time_m_stop/10,WHITE,BLACK);showdigit_color(336,99,time_c_v1[0].time_m_stop%10,WHITE,BLACK);
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off ? showhanzi(416,99,70) : showhanzi(416,99,71);
				
				showhanzi(0,131,79);showhanzi(96,131,18);showhanzi(160,131,40);showhanzi(288,131,18);showhanzi(352,131,40);
				showdigit_color(64,131,time_c_v1[1].time_h_start/10,WHITE,BLACK);showdigit_color(80,131,time_c_v1[1].time_h_start%10,WHITE,BLACK);
				showdigit_color(128,131,time_c_v1[1].time_m_start/10,WHITE,BLACK);showdigit_color(144,131,time_c_v1[1].time_m_start%10,WHITE,BLACK);
				showdigit_color(256,131,time_c_v1[1].time_h_stop/10,WHITE,BLACK);showdigit_color(272,131,time_c_v1[1].time_h_stop%10,WHITE,BLACK);
				showdigit_color(320,131,time_c_v1[1].time_m_stop/10,WHITE,BLACK);showdigit_color(336,131,time_c_v1[1].time_m_stop%10,WHITE,BLACK);
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off ? showhanzi(416,131,70) : showhanzi(416,131,71);
				
				showhanzi(0,163,80);showhanzi(96,163,18);showhanzi(160,163,40);showhanzi(288,163,18);showhanzi(352,163,40);
				showdigit_color(64,163,time_c_v1[2].time_h_start/10,WHITE,BLACK);showdigit_color(80,163,time_c_v1[2].time_h_start%10,WHITE,BLACK);
				showdigit_color(128,163,time_c_v1[2].time_m_start/10,WHITE,BLACK);showdigit_color(144,163,time_c_v1[2].time_m_start%10,WHITE,BLACK);
				showdigit_color(256,163,time_c_v1[2].time_h_stop/10,WHITE,BLACK);showdigit_color(272,163,time_c_v1[2].time_h_stop%10,WHITE,BLACK);
				showdigit_color(320,163,time_c_v1[2].time_m_stop/10,WHITE,BLACK);showdigit_color(336,163,time_c_v1[2].time_m_stop%10,WHITE,BLACK);
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off ? showhanzi(416,163,70) : showhanzi(416,163,71);
				
				showhanzi(0,195,81);showhanzi(96,195,18);showhanzi(160,195,40);showhanzi(288,195,18);showhanzi(352,195,40);
				showdigit_color(64,195,time_c_v1[3].time_h_start/10,WHITE,BLACK);showdigit_color(80,195,time_c_v1[3].time_h_start%10,WHITE,BLACK);
				showdigit_color(128,195,time_c_v1[3].time_m_start/10,WHITE,BLACK);showdigit_color(144,195,time_c_v1[3].time_m_start%10,WHITE,BLACK);
				showdigit_color(256,195,time_c_v1[3].time_h_stop/10,WHITE,BLACK);showdigit_color(272,195,time_c_v1[3].time_h_stop%10,WHITE,BLACK);
				showdigit_color(320,195,time_c_v1[3].time_m_stop/10,WHITE,BLACK);showdigit_color(336,195,time_c_v1[3].time_m_stop%10,WHITE,BLACK);
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off ? showhanzi(416,195,70) : showhanzi(416,195,71);
				
				showhanzi(0,227,82);showhanzi(96,227,18);showhanzi(160,227,40);showhanzi(288,227,18);showhanzi(352,227,40);
				showdigit_color(64,227,time_c_v1[4].time_h_start/10,WHITE,BLACK);showdigit_color(80,227,time_c_v1[4].time_h_start%10,WHITE,BLACK);
				showdigit_color(128,227,time_c_v1[4].time_m_start/10,WHITE,BLACK);showdigit_color(144,227,time_c_v1[4].time_m_start%10,WHITE,BLACK);
				showdigit_color(256,227,time_c_v1[4].time_h_stop/10,WHITE,BLACK);showdigit_color(272,227,time_c_v1[4].time_h_stop%10,WHITE,BLACK);
				showdigit_color(320,227,time_c_v1[4].time_m_stop/10,WHITE,BLACK);showdigit_color(336,227,time_c_v1[4].time_m_stop%10,WHITE,BLACK);
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off ? showhanzi(416,227,70) : showhanzi(416,227,71);
				
//				showhanzi(0,67,78);showhanzi(108,67,18);showhanzi(180,67,40);showhanzi(252,67,69);//1 时分秒
//				showdigit_color(72,67,time_c_v1[0].time_h/10,WHITE,BLACK);showdigit_color(88,67,time_c_v1[0].time_h%10,WHITE,BLACK);
//				showdigit_color(144,67,time_c_v1[0].time_m/10,WHITE,BLACK);showdigit_color(160,67,time_c_v1[0].time_m%10,WHITE,BLACK);
//				showdigit_color(216,67,time_c_v1[0].time_s/10,WHITE,BLACK);showdigit_color(232,67,time_c_v1[0].time_s%10,WHITE,BLACK);
//				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off ? showhanzi(315,67,70) : showhanzi(315,67,71);
//				
//				showhanzi(0,103,79);showhanzi(108,103,18);showhanzi(180,103,40);showhanzi(252,103,69);//2 时分秒
//				showdigit_color(72,103,time_c_v1[1].time_h/10,WHITE,BLACK);showdigit_color(88,103,time_c_v1[1].time_h%10,WHITE,BLACK);
//				showdigit_color(144,103,time_c_v1[1].time_m/10,WHITE,BLACK);showdigit_color(160,103,time_c_v1[1].time_m%10,WHITE,BLACK);
//				showdigit_color(216,103,time_c_v1[1].time_s/10,WHITE,BLACK);showdigit_color(232,103,time_c_v1[1].time_s%10,WHITE,BLACK);
//				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off ? showhanzi(315,103,70) : showhanzi(315,103,71);
//				
//				showhanzi(0,139,80);showhanzi(108,139,18);showhanzi(180,139,40);showhanzi(252,139,69);
//				showdigit_color(72,139,time_c_v1[2].time_h/10,WHITE,BLACK);showdigit_color(88,139,time_c_v1[2].time_h%10,WHITE,BLACK);
//				showdigit_color(144,139,time_c_v1[2].time_m/10,WHITE,BLACK);showdigit_color(160,139,time_c_v1[2].time_m%10,WHITE,BLACK);
//				showdigit_color(216,139,time_c_v1[2].time_s/10,WHITE,BLACK);showdigit_color(232,139,time_c_v1[2].time_s%10,WHITE,BLACK);
//				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off ? showhanzi(315,139,70) : showhanzi(315,139,71);
//				
//				showhanzi(0,175,81);showhanzi(108,175,18);showhanzi(180,175,40);showhanzi(252,175,69);
//				showdigit_color(72,175,time_c_v1[3].time_h/10,WHITE,BLACK);showdigit_color(88,175,time_c_v1[3].time_h%10,WHITE,BLACK);
//				showdigit_color(144,175,time_c_v1[3].time_m/10,WHITE,BLACK);showdigit_color(160,175,time_c_v1[3].time_m%10,WHITE,BLACK);
//				showdigit_color(216,175,time_c_v1[3].time_s/10,WHITE,BLACK);showdigit_color(232,175,time_c_v1[3].time_s%10,WHITE,BLACK);
//				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off ? showhanzi(315,175,70) : showhanzi(315,175,71);
//				
//				showhanzi(0,211,82);showhanzi(108,211,18);showhanzi(180,211,40);showhanzi(252,211,69);
//				showdigit_color(72,211,time_c_v1[4].time_h/10,WHITE,BLACK);showdigit_color(88,211,time_c_v1[4].time_h%10,WHITE,BLACK);
//				showdigit_color(144,211,time_c_v1[4].time_m/10,WHITE,BLACK);showdigit_color(160,211,time_c_v1[4].time_m%10,WHITE,BLACK);
//				showdigit_color(216,211,time_c_v1[4].time_s/10,WHITE,BLACK);showdigit_color(232,211,time_c_v1[4].time_s%10,WHITE,BLACK);
//				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off ? showhanzi(315,211,70) : showhanzi(315,211,71);

					break;
			default:break;

		}
	}else{fresh_flag_1 = 0;fresh_flag_2 = 0;fresh_flag_3 = 0;fresh_flag_4 = 0;fresh_flag_5 = 0;
		fresh_flag_6 = 0;fresh_flag_7 = 0;fresh_flag_8 = 0;fresh_flag_9 = 0;fresh_flag_10 = 0;
	}
	
	//不使用界面
	if((enter_config_flag_1 || enter_config_flag_2 ||enter_config_flag_3 || enter_config_flag_4 ||enter_config_flag_5
		|| enter_config_flag_6 ||enter_config_flag_7 || enter_config_flag_8 || enter_config_flag_9 || enter_config_flag_10
		)
	&& relay_structure_temp[last_index_save-12].relay_mode==0){
	    showhanzi(160,35,92);showhanzi(192,35,90);showhanzi(224,35,91);//不使用
		showhanzi(128,109,151);showhanzi(192,109,71);//常关
		showhanzi(128,178,151);showhanzi(192,178,70);//常开
		if(relay_structure_temp[last_index_save-12].no_Ctrl.all_open_or_close){
			showhanzi(240,109,150);showhanzi(240,178,149);
		}else{
			showhanzi(240,109,149);showhanzi(240,178,150);
		}
	}
	
	//温控界面
	if((enter_config_flag_1 || enter_config_flag_2 ||enter_config_flag_3 || enter_config_flag_4 ||enter_config_flag_5
		|| enter_config_flag_6 ||enter_config_flag_7 || enter_config_flag_8 || enter_config_flag_9 || enter_config_flag_10
		)
	&& relay_structure_temp[last_index_save-12].relay_mode==1){
		u8 max_temp_v = relay_structure_temp[last_index_save-12].temp_control.max_temp;
		u8 min_temp_v = relay_structure_temp[last_index_save-12].temp_control.min_temp;
		u16 max_nh3_v = relay_structure_temp[last_index_save-12].temp_control.max_nh3;
		u16 min_nh3_v = relay_structure_temp[last_index_save-12].temp_control.min_nh3;
		//显示固定界面信息
		showhanzi(160,35,39);showhanzi(192,35,10);showhanzi(224,35,38);//温控
	
		showhanzi(6,67,41);showhanzi(38,67,42);showhanzi(70,67,39);showhanzi(102,67,44);//最高温
		showhanzi(6,103,41);showhanzi(38,103,43);showhanzi(70,103,39); showhanzi(102,103,44);//最低温
		showhanzi(6,139,50);showhanzi(38,139,51);showhanzi(70,139,52); showhanzi(102,139,53);showhanzi(134,139,54);showhanzi(166,139,44);//对应传感器
		showhanzi(6,175,55);showhanzi(38,175,56);showhanzi(70,175,57); showhanzi(102,175,58);showhanzi(134,175,44);//启动方式
		showhanzi(6,211,105);showhanzi(38,211,106);showhanzi(70,211,41); showhanzi(102,211,42);showhanzi(134,211,88);showhanzi(166,211,44);//氨气最高值
		showhanzi(6,247,105);showhanzi(38,247,106);showhanzi(70,247,41); showhanzi(102,247,43);showhanzi(134,247,88);showhanzi(166,247,44);//氨气最低值
		//显示参数信息
		if(cur_pos1!=1){showdigit_color(134,67,max_temp_v/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v%10,WHITE,BLACK);}
		if(cur_pos1!=2){showdigit_color(134,103,min_temp_v/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v%10,WHITE,BLACK);}
		if(cur_pos1!=3){
			if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
			if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
			if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
			if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
		}
		if(cur_pos1!=4){				
			if(relay_structure_temp[last_index_save-12].temp_control.startup_mode){showhanzi(167,175,61);showhanzi(199,175,62);}else{showhanzi(167,175,59);showhanzi(199,175,60);} //加热//制冷
		}
		if(cur_pos1!=5){showdigit_color(200,211,max_nh3_v/100,WHITE,BLACK);showdigit_color(216,211,(max_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,211,max_nh3_v%10,WHITE,BLACK);}
		if(cur_pos1!=6){showdigit_color(200,247,min_nh3_v/100,WHITE,BLACK);showdigit_color(216,247,(min_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,247,min_nh3_v%10,WHITE,BLACK);}
		switch(cur_pos1)
		{
			case 1:
			    showdigit_color(134,67,max_temp_v/10,BLACK,WHITE);showdigit_color(150,67,max_temp_v%10,BLACK,WHITE);
				break;
			case 2:
			    showdigit_color(134,103,min_temp_v/10,BLACK,WHITE);showdigit_color(150,103,min_temp_v%10,BLACK,WHITE);
				break;
			case 3:
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,63,1);showhanzi_1(295,139,38,1);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,64,1);showhanzi_1(295,139,38,1);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,65,1);showhanzi_1(295,139,38,1);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi_1(199,139,67,1);showhanzi_1(231,139,68,1);showhanzi_1(263,139,39,1);showhanzi_1(295,139,66,1);}//平均温度
				break;
			case 4:
				if(relay_structure_temp[last_index_save-12].temp_control.startup_mode){
					showhanzi_1(167,175,61,1);showhanzi_1(199,175,62,1);
				}
				else{
					showhanzi_1(167,175,59,1);showhanzi_1(199,175,60,1);
				}
				break;
			case 5:
				showdigit_color(200,211,max_nh3_v/100,BLACK,WHITE);showdigit_color(216,211,(max_nh3_v/10)%10,BLACK,WHITE);showdigit_color(232,211,max_nh3_v%10,BLACK,WHITE);
				break;
			case 6:
			    showdigit_color(200,247,min_nh3_v/100,BLACK,WHITE);showdigit_color(216,247,(min_nh3_v/10)%10,BLACK,WHITE);showdigit_color(232,247,min_nh3_v%10,BLACK,WHITE);
				break;
			default:break;
		}
	}
	//时控界面
	if((enter_config_flag_1 || enter_config_flag_2 ||enter_config_flag_3 || enter_config_flag_4 ||enter_config_flag_5
		|| enter_config_flag_6 ||enter_config_flag_7 || enter_config_flag_8 || enter_config_flag_9 || enter_config_flag_10
		)
	&& relay_structure_temp[last_index_save-12].relay_mode==2){
		uint16_t time_open_v=relay_structure_temp[last_index_save-12].time_control.time_open;
		uint16_t time_stop_v=relay_structure_temp[last_index_save-12].time_control.time_stop;
		//时控固定界面信息
		//显示时控
		showhanzi(160,35,18);showhanzi(192,35,10);showhanzi(224,35,38);
		//显示时控固定界面信息
		showhanzi(6,67,48);  showhanzi(38,67,49);  showhanzi(70,67,18);  showhanzi(102,67,19); showhanzi(134,67,44);//运转时间
		showhanzi(6,139,46);  showhanzi(38,139,47);  showhanzi(70,139,18); showhanzi(102,139,19);showhanzi(134,139,44);//停机时间
		//显示参数信息
		switch(cur_pos2)
		{
			case 1:
				showdigit_color(166,67,time_open_v/1000,BLACK,WHITE);showdigit_color(182,67,(time_open_v/100)%10,BLACK,WHITE);
			    showdigit_color(198,67,(time_open_v/10)%10,BLACK,WHITE);showdigit_color(214,67,time_open_v%10,BLACK,WHITE);
			
				showdigit_color(166,139,time_stop_v/1000,WHITE,BLACK);showdigit_color(182,139,(time_stop_v/100)%10,WHITE,BLACK);
			    showdigit_color(198,139,(time_stop_v/10)%10,WHITE,BLACK);showdigit_color(214,139,time_stop_v%10,WHITE,BLACK);
				break;
			case 2:
				showdigit_color(166,67,time_open_v/1000,WHITE,BLACK);showdigit_color(182,67,(time_open_v/100)%10,WHITE,BLACK);
			    showdigit_color(198,67,(time_open_v/10)%10,WHITE,BLACK);showdigit_color(214,67,time_open_v%10,WHITE,BLACK);
			
				showdigit_color(166,139,time_stop_v/1000,BLACK,WHITE);showdigit_color(182,139,(time_stop_v/100)%10,BLACK,WHITE);
			    showdigit_color(198,139,(time_stop_v/10)%10,BLACK,WHITE);showdigit_color(214,139,time_stop_v%10,BLACK,WHITE);
				break;
			default:break;
		}
	}
	//温时控界面
	if((enter_config_flag_1 || enter_config_flag_2 ||enter_config_flag_3 || enter_config_flag_4 ||enter_config_flag_5
		|| enter_config_flag_6 ||enter_config_flag_7 || enter_config_flag_8 || enter_config_flag_9 || enter_config_flag_10
		)
	&& relay_structure_temp[last_index_save-12].relay_mode==3){
		u8 max_temp_v111 = relay_structure_temp[last_index_save-12].temp_control.max_temp;
		u8 min_temp_v111 = relay_structure_temp[last_index_save-12].temp_control.min_temp;
		uint16_t time_open_v111=relay_structure_temp[last_index_save-12].time_control.time_open;
		uint16_t time_stop_v111=relay_structure_temp[last_index_save-12].time_control.time_stop;
		//温时控
		showhanzi(160,35,39);showhanzi(192,35,18);showhanzi(224,35,10);

		showhanzi(6,67,41);showhanzi(38,67,42);showhanzi(70,67,39);showhanzi(102,67,44);//最高温
		showhanzi(6,103,41);showhanzi(38,103,43);showhanzi(70,103,39); showhanzi(102,103,44);//最低温
		showhanzi(6,139,50);showhanzi(38,139,51);showhanzi(70,139,52); showhanzi(102,139,53);showhanzi(134,139,54);showhanzi(166,139,44);//对应传感器
		showhanzi(6,175,55);showhanzi(38,175,56);showhanzi(70,175,57); showhanzi(102,175,58);showhanzi(134,175,44);//启动方式
		
		showhanzi(6,211,48);  showhanzi(38,211,49);  showhanzi(70,211,18);  showhanzi(102,211,19); showhanzi(134,211,44);//打开时间	
		showhanzi(6,247,46);  showhanzi(38,247,47);  showhanzi(70,247,18); showhanzi(102,247,19);showhanzi(134,247,44);//关闭时间
		
		if(cur_pos3!=1){showdigit_color(134,67,max_temp_v111/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v111%10,WHITE,BLACK);}
		if(cur_pos3!=2){showdigit_color(134,103,min_temp_v111/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v111%10,WHITE,BLACK);}
		if(cur_pos3!=3){
			if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
			if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
			if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
			if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
		}
		if(cur_pos3!=4){
			if(relay_structure_temp[last_index_save-12].temp_control.startup_mode){
				showhanzi(167,175,61);showhanzi(199,175,62);
			}else{
				showhanzi(167,175,59);showhanzi(199,175,60);
			}//制冷
		}
		if(cur_pos3!=5){			    
			showdigit_color(166,211,time_open_v111/1000,WHITE,BLACK);showdigit_color(182,211,(time_open_v111/100)%10,WHITE,BLACK);
			showdigit_color(198,211,(time_open_v111/10)%10,WHITE,BLACK);showdigit_color(214,211,time_open_v111%10,WHITE,BLACK);
		}
		if(cur_pos3!=6){
			showdigit_color(166,247,time_stop_v111/1000,WHITE,BLACK);showdigit_color(182,247,(time_stop_v111/100)%10,WHITE,BLACK);
			showdigit_color(198,247,(time_stop_v111/10)%10,WHITE,BLACK);showdigit_color(214,247,time_stop_v111%10,WHITE,BLACK);			
		}
		
		switch(cur_pos3){
			case 1:
				showdigit_color(134,67,max_temp_v111/10,BLACK,WHITE);showdigit_color(150,67,max_temp_v111%10,BLACK,WHITE);
				break;
			case 2:
			    showdigit_color(134,103,min_temp_v111/10,BLACK,WHITE);showdigit_color(150,103,min_temp_v111%10,BLACK,WHITE);
				break;
			case 3:
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,63,1);showhanzi_1(295,139,38,1);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,64,1);showhanzi_1(295,139,38,1);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,65,1);showhanzi_1(295,139,38,1);}
				if(relay_structure_temp[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi_1(199,139,67,1);showhanzi_1(231,139,68,1);showhanzi_1(263,139,39,1);showhanzi_1(295,139,66,1);}//平均温度
				break;
			case 4:
				if(relay_structure_temp[last_index_save-12].temp_control.startup_mode){
					showhanzi_1(167,175,61,1);showhanzi_1(199,175,62,1);//加热
				}else{
					showhanzi_1(167,175,59,1);showhanzi_1(199,175,60,1);//制冷
				}
				break;
			case 5:
				showdigit_color(166,211,time_open_v111/1000,BLACK,WHITE);showdigit_color(182,211,(time_open_v111/100)%10,BLACK,WHITE);
			    showdigit_color(198,211,(time_open_v111/10)%10,BLACK,WHITE);showdigit_color(214,211,time_open_v111%10,BLACK,WHITE);
				break;
			case 6:
				showdigit_color(166,247,time_stop_v111/1000,BLACK,WHITE);showdigit_color(182,247,(time_stop_v111/100)%10,BLACK,WHITE);
			    showdigit_color(198,247,(time_stop_v111/10)%10,BLACK,WHITE);showdigit_color(214,247,time_stop_v111%10,BLACK,WHITE);
				break;
			default:break;
		}

	}
	
	
	//分时控界面
	if((enter_config_flag_1 || enter_config_flag_2 ||enter_config_flag_3 || enter_config_flag_4 ||enter_config_flag_5
		|| enter_config_flag_6 ||enter_config_flag_7 || enter_config_flag_8 || enter_config_flag_9 || enter_config_flag_10
		)
	&& relay_structure_temp[last_index_save-12].relay_mode==4){
	
		Time_C time_c_v[5]={0};
		time_c_v[0].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_start;
		time_c_v[0].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_start;
		time_c_v[0].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.hour_stop;
		time_c_v[0].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes_stop;

		time_c_v[1].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_start;
		time_c_v[1].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_start;
		time_c_v[1].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.hour_stop;
		time_c_v[1].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes_stop;
		
		time_c_v[2].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_start;
		time_c_v[2].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_start;
		time_c_v[2].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.hour_stop;
		time_c_v[2].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes_stop;
		
		time_c_v[3].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_start;
		time_c_v[3].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_start;
		time_c_v[3].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.hour_stop;
		time_c_v[3].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes_stop;
		
		time_c_v[4].time_h_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_start;
		time_c_v[4].time_m_start = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_start;
		time_c_v[4].time_h_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.hour_stop;
		time_c_v[4].time_m_stop = relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes_stop;

		//分时控
		showhanzi(160,35,40);showhanzi(192,35,18);showhanzi(224,35,10);
		showhanzi_color(80,67,115,GREEN,BLACK);showhanzi_color(112,67,116,GREEN,BLACK);showhanzi_color(144,67,18,GREEN,BLACK);showhanzi_color(176,67,19,GREEN,BLACK);//起始时间
		showhanzi_color(272,67,117,RED,BLACK);showhanzi_color(304,67,118,RED,BLACK);showhanzi_color(336,67,18,RED,BLACK);showhanzi_color(368,67,19,RED,BLACK);//终止时间
		
		showhanzi(0,99,78);showhanzi(96,99,18);showhanzi(160,99,40);showhanzi(288,99,18);showhanzi(352,99,40);//1 时分秒
		if(cur_pos4!=1){showdigit_color(64,99,time_c_v[0].time_h_start/10,WHITE,BLACK);showdigit_color(80,99,time_c_v[0].time_h_start%10,WHITE,BLACK);}
		if(cur_pos4!=2){showdigit_color(128,99,time_c_v[0].time_m_start/10,WHITE,BLACK);showdigit_color(144,99,time_c_v[0].time_m_start%10,WHITE,BLACK);}
		if(cur_pos4!=3){showdigit_color(256,99,time_c_v[0].time_h_stop/10,WHITE,BLACK);showdigit_color(272,99,time_c_v[0].time_h_stop%10,WHITE,BLACK);}
		if(cur_pos4!=4){showdigit_color(320,99,time_c_v[0].time_m_stop/10,WHITE,BLACK);showdigit_color(336,99,time_c_v[0].time_m_stop%10,WHITE,BLACK);}
		if(cur_pos4!=5){relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off ? showhanzi(416,99,70) : showhanzi(416,99,71);}//开 /关
		
		
		showhanzi(0,131,79);showhanzi(96,131,18);showhanzi(160,131,40);showhanzi(288,131,18);showhanzi(352,131,40);
		if(cur_pos4!=6){showdigit_color(64,131,time_c_v[1].time_h_start/10,WHITE,BLACK);showdigit_color(80,131,time_c_v[1].time_h_start%10,WHITE,BLACK);}
		if(cur_pos4!=7){showdigit_color(128,131,time_c_v[1].time_m_start/10,WHITE,BLACK);showdigit_color(144,131,time_c_v[1].time_m_start%10,WHITE,BLACK);}
		if(cur_pos4!=8){showdigit_color(256,131,time_c_v[1].time_h_stop/10,WHITE,BLACK);showdigit_color(272,131,time_c_v[1].time_h_stop%10,WHITE,BLACK);}
		if(cur_pos4!=9){showdigit_color(320,131,time_c_v[1].time_m_stop/10,WHITE,BLACK);showdigit_color(336,131,time_c_v[1].time_m_stop%10,WHITE,BLACK);}
		if(cur_pos4!=10){relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off ? showhanzi(416,131,70) : showhanzi(416,131,71);}//开 /关
		
		showhanzi(0,163,80);showhanzi(96,163,18);showhanzi(160,163,40);showhanzi(288,163,18);showhanzi(352,163,40);
		if(cur_pos4!=11){showdigit_color(64,163,time_c_v[2].time_h_start/10,WHITE,BLACK);showdigit_color(80,163,time_c_v[2].time_h_start%10,WHITE,BLACK);}
		if(cur_pos4!=12){showdigit_color(128,163,time_c_v[2].time_m_start/10,WHITE,BLACK);showdigit_color(144,163,time_c_v[2].time_m_start%10,WHITE,BLACK);}
		if(cur_pos4!=13){showdigit_color(256,163,time_c_v[2].time_h_stop/10,WHITE,BLACK);showdigit_color(272,163,time_c_v[2].time_h_stop%10,WHITE,BLACK);}
		if(cur_pos4!=14){showdigit_color(320,163,time_c_v[2].time_m_stop/10,WHITE,BLACK);showdigit_color(336,163,time_c_v[2].time_m_stop%10,WHITE,BLACK);}
		if(cur_pos4!=15){relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off ? showhanzi(416,163,70) : showhanzi(416,163,71);}//开 /关
		
		showhanzi(0,195,81);showhanzi(96,195,18);showhanzi(160,195,40);showhanzi(288,195,18);showhanzi(352,195,40);
		if(cur_pos4!=16){showdigit_color(64,195,time_c_v[3].time_h_start/10,WHITE,BLACK);showdigit_color(80,195,time_c_v[3].time_h_start%10,WHITE,BLACK);}
		if(cur_pos4!=17){showdigit_color(128,195,time_c_v[3].time_m_start/10,WHITE,BLACK);showdigit_color(144,195,time_c_v[3].time_m_start%10,WHITE,BLACK);}
		if(cur_pos4!=18){showdigit_color(256,195,time_c_v[3].time_h_stop/10,WHITE,BLACK);showdigit_color(272,195,time_c_v[3].time_h_stop%10,WHITE,BLACK);}
		if(cur_pos4!=19){showdigit_color(320,195,time_c_v[3].time_m_stop/10,WHITE,BLACK);showdigit_color(336,195,time_c_v[3].time_m_stop%10,WHITE,BLACK);}
		if(cur_pos4!=20){relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off ? showhanzi(416,195,70) : showhanzi(416,195,71);}//开 /关
		
		showhanzi(0,227,82);showhanzi(96,227,18);showhanzi(160,227,40);showhanzi(288,227,18);showhanzi(352,227,40);
		if(cur_pos4!=21){showdigit_color(64,227,time_c_v[4].time_h_start/10,WHITE,BLACK);showdigit_color(80,227,time_c_v[4].time_h_start%10,WHITE,BLACK);}
		if(cur_pos4!=22){showdigit_color(128,227,time_c_v[4].time_m_start/10,WHITE,BLACK);showdigit_color(144,227,time_c_v[4].time_m_start%10,WHITE,BLACK);}
		if(cur_pos4!=23){showdigit_color(256,227,time_c_v[4].time_h_stop/10,WHITE,BLACK);showdigit_color(272,227,time_c_v[4].time_h_stop%10,WHITE,BLACK);}
		if(cur_pos4!=24){showdigit_color(320,227,time_c_v[4].time_m_stop/10,WHITE,BLACK);showdigit_color(336,227,time_c_v[4].time_m_stop%10,WHITE,BLACK);}
		if(cur_pos4!=25){relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off ? showhanzi(416,227,70) : showhanzi(416,227,71);}//开 /关
		
		switch(cur_pos4){
			case 1:
				showdigit_color(64,99,time_c_v[0].time_h_start/10,BLACK,WHITE);showdigit_color(80,99,time_c_v[0].time_h_start%10,BLACK,WHITE);
				break;
			case 2:
				showdigit_color(128,99,time_c_v[0].time_m_start/10,BLACK,WHITE);showdigit_color(144,99,time_c_v[0].time_m_start%10,BLACK,WHITE);
				break;
			case 3:
				showdigit_color(256,99,time_c_v[0].time_h_stop/10,BLACK,WHITE);showdigit_color(272,99,time_c_v[0].time_h_stop%10,BLACK,WHITE);
				break;
			case 4:
				showdigit_color(320,99,time_c_v[0].time_m_stop/10,BLACK,WHITE);showdigit_color(336,99,time_c_v[0].time_m_stop%10,BLACK,WHITE);
				break;
			case 5:
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off ? showhanzi_1(416,99,70,1) : showhanzi_1(416,99,71,1);
				break;
			case 6:
				showdigit_color(64,131,time_c_v[1].time_h_start/10,BLACK,WHITE);showdigit_color(80,131,time_c_v[1].time_h_start%10,BLACK,WHITE);
				break;
			case 7:
				showdigit_color(128,131,time_c_v[1].time_m_start/10,BLACK,WHITE);showdigit_color(144,131,time_c_v[1].time_m_start%10,BLACK,WHITE);
				break;
			case 8:
				showdigit_color(256,131,time_c_v[1].time_h_stop/10,BLACK,WHITE);showdigit_color(272,131,time_c_v[1].time_h_stop%10,BLACK,WHITE);
				break;
			case 9:
				showdigit_color(320,131,time_c_v[1].time_m_stop/10,BLACK,WHITE);showdigit_color(336,131,time_c_v[1].time_m_stop%10,BLACK,WHITE);
				break;
			case 10:
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off ? showhanzi_1(416,131,70,1) : showhanzi_1(416,131,71,1);
				break;
			case 11:
				showdigit_color(64,163,time_c_v[2].time_h_start/10,BLACK,WHITE);showdigit_color(80,163,time_c_v[2].time_h_start%10,BLACK,WHITE);
				break;
			case 12:
				showdigit_color(128,163,time_c_v[2].time_m_start/10,BLACK,WHITE);showdigit_color(144,163,time_c_v[2].time_m_start%10,BLACK,WHITE);
				break;
			case 13:
				showdigit_color(256,163,time_c_v[2].time_h_stop/10,BLACK,WHITE);showdigit_color(272,163,time_c_v[2].time_h_stop%10,BLACK,WHITE);
				break;
			case 14:
				showdigit_color(320,163,time_c_v[2].time_m_stop/10,BLACK,WHITE);showdigit_color(336,163,time_c_v[2].time_m_stop%10,BLACK,WHITE);
				break;
			case 15:
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off ? showhanzi_1(416,163,70,1) : showhanzi_1(416,163,71,1);
				break;
			case 16:
				showdigit_color(64,195,time_c_v[3].time_h_start/10,BLACK,WHITE);showdigit_color(80,195,time_c_v[3].time_h_start%10,BLACK,WHITE);
				break;
			case 17:
				showdigit_color(128,195,time_c_v[3].time_m_start/10,BLACK,WHITE);showdigit_color(144,195,time_c_v[3].time_m_start%10,BLACK,WHITE);
				break;
			case 18:
				showdigit_color(256,195,time_c_v[3].time_h_stop/10,BLACK,WHITE);showdigit_color(272,195,time_c_v[3].time_h_stop%10,BLACK,WHITE);
				break;
			case 19:
				showdigit_color(320,195,time_c_v[3].time_m_stop/10,BLACK,WHITE);showdigit_color(336,195,time_c_v[3].time_m_stop%10,BLACK,WHITE);
				break;
			case 20:
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off ? showhanzi_1(416,195,70,1) : showhanzi_1(416,195,71,1);
				break;
			case 21:
				showdigit_color(64,227,time_c_v[4].time_h_start/10,BLACK,WHITE);showdigit_color(80,227,time_c_v[4].time_h_start%10,BLACK,WHITE);
				break;
			case 22:
				showdigit_color(128,227,time_c_v[4].time_m_start/10,BLACK,WHITE);showdigit_color(144,227,time_c_v[4].time_m_start%10,BLACK,WHITE);
				break;
			case 23:
				showdigit_color(256,227,time_c_v[4].time_h_stop/10,BLACK,WHITE);showdigit_color(272,227,time_c_v[4].time_h_stop%10,BLACK,WHITE);
				break;
			case 24:
				showdigit_color(320,227,time_c_v[4].time_m_stop/10,BLACK,WHITE);showdigit_color(336,227,time_c_v[4].time_m_stop%10,BLACK,WHITE);
				break;
			case 25:
				relay_structure_temp[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off ? showhanzi_1(416,227,70,1) : showhanzi_1(416,227,71,1);
				break;
		
			default:break;
		}
	}
//	send_CONFIG_INDEX = last_index_save;
//	// 检查是否发生了变化
//    for (int i = 0; i < 10; i++) {
//        if (memcmp(&relay_structure[i], &original_relay_structure[i], sizeof(Relay_Structure)) != 0) {
//            send_CONFIG_FLAG=1;
//        }
//    }
	
	
}





	

