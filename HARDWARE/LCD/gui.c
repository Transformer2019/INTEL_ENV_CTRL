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

Relay_Structure relay_structure[10] ={{.relayNo=1,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=2,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=3,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=4,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=5,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=6,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=7,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=8,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=9,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
{.relayNo=10,.relay_mode=0,.temp_control.max_temp=50.0,.temp_control.max_nh3=30,.temp_control.min_nh3=1,.time_control.time_open=10,.time_control.time_stop=10},
};

//#pragma pack()    //取消自定义字节对齐方式

uint8_t warn_flag=0;
uint8_t warn_temp1_flag=0;
uint8_t warn_temp2_flag=0;
uint8_t warn_temp3_flag=0;
uint8_t limit_temp_maxvalue=50;
uint8_t limit_temp_minvalue=0;

uint8_t NH3_warn_flag=0;
uint8_t NH3_max=13;

float temp1_correct=0.0;// 
float temp2_correct=0.0;// 
float temp3_correct=0.0;// 

char imei_no[16]={0};

static u8 func_index=_Main_UI;//当前页面索引值
static u8 last_index=_Main_UI;//上一个界面索引值
static void (*current_operation_func)(u8,u8);//定义一个函数指针

uint8_t ctrl_ui=0;
//变频控制
u8 out_voltage=0;

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



static u8 cur_pos1=1;
static u8 cur_pos2=1;
static u8 cur_pos3=1;
static u8 cur_pos4=1;



uint8_t send_MQTT_Flag=0;


void fresh_page(){
	LCD_Fill(0,36,349,287,BLACK);
}

//索引表
Main_Menu table[25]=
{
	//Cur_Index,   previous,   next,   enter,   back,   (*current_operation)(u8,u8)
	//主界面
	{_Main_UI,_Main_UI,_Main_UI,_Air_Blower_Option,_Main_UI,Main_UI},
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
	
	{_Air_Blower_Child_Ctrl_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Child_Ctrl_1,_Air_Blower_Child_1,Air_Blower_Child_Ctrl_1_Fun}//22
	
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
			case KEY_BACK:func_index=table[func_index].back;//更新索引值

					break;
			default:break;
		}
	    delay_ms(5);
		if(func_index == 0 && (last_index == 1 || last_index == 2 || last_index == 3 || last_index == 4 || last_index == 5 || last_index == 6)){LCD_Fill(0,0,480,320,HOME_BACK);}//从主菜单返回主页
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
		
		
		
        if(func_index == 22 && (last_index == 12||last_index == 13||last_index == 14||last_index == 15||last_index == 16||last_index == 17||last_index == 18||last_index == 19||last_index == 20||last_index == 21))
		{LCD_Clear_upper_tail(BLACK);}//从风机一控制类型进入
		if((func_index == 12||func_index == 13||func_index == 14||func_index == 15||func_index == 16||func_index == 17||func_index == 18||func_index == 19||func_index == 20||func_index == 21) && last_index == 22){LCD_Clear_upper_tail(BLACK);
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
		cur_pos1=1;cur_pos2=1;cur_pos3=1;cur_pos4=1;}//刷新带清除进入配置界面标志,清除刷新标志

		
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

			if(func_index == 22){
				LCD_Fill(0,0,176,31,WHITE);
				LCD_Fill(304,0,480,31,WHITE);
				showhanzi(0,35,10);showhanzi(32,35,11);showhanzi(64,35,36);showhanzi(96,35,37);showhanzi(128,35,44);//控制类型：
				LCD_Fill(0,lcddev.height-32,lcddev.width,lcddev.height,WHITE);
				switch(last_index){
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
	
//	if(send_NH3>500 || send_NH3<0){
//        showhanzi_color(336,204,95,RED,BLACK);
//		showhanzi_color(368,204,70,RED,BLACK);
//		LCD_Fill(400,204,468,236,BLACK);
//	}else if(send_NH3>=100 && send_NH3<=500){
//		uint8_t show_nh3_1 = send_NH3 / 100;
//		uint8_t show_nh3_2 = (send_NH3 / 10) % 10;
//		uint8_t show_nh3_3 = send_NH3 % 10;
//		showdigit_color(336,204,show_nh3_1,UNDET,BLACK);
//		showdigit_color(352,204,show_nh3_2,UNDET,BLACK);
//		showdigit_color(368,204,10,UNDET,BLACK);
//		showdigit_color(384,204,show_nh3_3,UNDET,BLACK);
//		LCD_Fill(400,204,405,236,BLACK);
//		showdigit_color(406,204,16,UNDET,BLACK);
//		showdigit_color(422,204,16,UNDET,BLACK);
//		showdigit_color(438,204,17,UNDET,BLACK);
//	}else if(send_NH3>=10&& send_NH3<100){
//		LCD_Fill(336,204,351,236,BLACK);
//		showdigit_color(352,204,send_NH3 / 10,UNDET,BLACK);
//		showdigit_color(368,204,10,UNDET,BLACK);
//		showdigit_color(384,204,send_NH3 % 10,UNDET,BLACK);
//		LCD_Fill(400,204,405,236,BLACK);
//		showdigit_color(406,204,16,UNDET,BLACK);
//		showdigit_color(422,204,16,UNDET,BLACK);
//		showdigit_color(438,204,17,UNDET,BLACK);
//	}else if(send_NH3>=0&& send_NH3<10){
//		LCD_Fill(336,204,351,236,BLACK);
//		showdigit_color(352,204,0,UNDET,BLACK);
//		showdigit_color(368,204,10,UNDET,BLACK);
//		showdigit_color(384,204,send_NH3,UNDET,BLACK);
//		LCD_Fill(400,204,405,236,BLACK);
//		showdigit_color(406,204,16,UNDET,BLACK);
//		showdigit_color(422,204,16,UNDET,BLACK);
//		showdigit_color(438,204,17,UNDET,BLACK);
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
	if(relay_structure[0].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(12,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(12,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(12,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(12,253,gImage_4);
	}else Gui_Drawbmp16(12,253,gImage_1);
	if(relay_structure[1].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(59,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(59,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(59,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(59,253,gImage_4);
	}else Gui_Drawbmp16(59,253,gImage_1);
	if(relay_structure[2].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(106,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(106,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(106,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(106,253,gImage_4);
	}else Gui_Drawbmp16(106,253,gImage_1);
	if(relay_structure[3].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(153,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(153,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(153,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(153,253,gImage_4);
	}else Gui_Drawbmp16(153,253,gImage_1);
	if(relay_structure[4].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(200,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(200,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(200,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(200,253,gImage_4);
	}else Gui_Drawbmp16(200,253,gImage_1);
	if(relay_structure[5].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(247,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(247,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(247,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(247,253,gImage_4);
	}else Gui_Drawbmp16(247,253,gImage_1);
	if(relay_structure[6].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(294,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(294,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(294,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(294,253,gImage_4);
	}else Gui_Drawbmp16(294,253,gImage_1);
	if(relay_structure[7].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(341,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(341,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(341,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(341,253,gImage_4);
	}else Gui_Drawbmp16(341,253,gImage_1);
	if(relay_structure[8].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(388,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(388,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(388,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(388,253,gImage_4);
	}else Gui_Drawbmp16(388,253,gImage_1);
	if(relay_structure[9].on_off){
		if(ctrl_ui==0)Gui_Drawbmp16(435,253,gImage_1);if(ctrl_ui==1)Gui_Drawbmp16(435,253,gImage_2);if(ctrl_ui==2)Gui_Drawbmp16(435,253,gImage_3);if(ctrl_ui==3)Gui_Drawbmp16(435,253,gImage_4);
	}else Gui_Drawbmp16(435,253,gImage_1);
	
	
	
	
}


/*
函数功能：主菜单显示函数
参数：u8 page_index,u8 key_val
返回值：无
*/
void Main_Menu_Func(u8 page_index,u8 key_val)
{
	switch(page_index)
	{
		case _Air_Blower_Option://显示Wifi图标
			//LCD_Fill(96,34,192,204,BLACK);

			showhanzi_1(177,34,4,GBLUE);  
			showhanzi_1(209,34,5,GBLUE);  
			showhanzi_1(241,34,6,GBLUE);  
			showhanzi_1(273,34,7,GBLUE); 

			showhanzi(177,68,8);  
			showhanzi(209,68,9);  
			showhanzi(241,68,10);  
			showhanzi(273,68,11);  

			showhanzi(177,102,12);  
			showhanzi(209,102,13);  
			showhanzi(241,102,14);  
			showhanzi(273,102,15);  
			
			showhanzi(177,136,16);  
			showhanzi(209,136,17);  
			showhanzi(241,136,18);  
			showhanzi(273,136,19);  
			
			showhanzi(177,170,20);  
			showhanzi(209,170,21);  
			showhanzi(241,170,14);  
			showhanzi(273,170,15);  
			
			showhanzi(177,204,22);  
			showhanzi(209,204,23);  
			showhanzi(241,204,24);  
			showhanzi(273,204,25);  
			
			break;
		case _Hz_Ctrl_Option://显示蓝牙图标
			//LCD_Fill(96,34,192,204,BLACK);
			showhanzi(177,34,4);  
			showhanzi(209,34,5);  
			showhanzi(241,34,6);  
			showhanzi(273,34,7); 

			showhanzi_1(177,68,8,GBLUE);  
			showhanzi_1(209,68,9,GBLUE);  
			showhanzi_1(241,68,10,GBLUE);  
			showhanzi_1(273,68,11,GBLUE);  
		
			showhanzi(177,102,12);  
			showhanzi(209,102,13);  
			showhanzi(241,102,14);  
			showhanzi(273,102,15);  
			
			showhanzi(177,136,16);  
			showhanzi(209,136,17);  
			showhanzi(241,136,18);  
			showhanzi(273,136,19);  
			
			showhanzi(177,170,20);  
			showhanzi(209,170,21);  
			showhanzi(241,170,14);  
			showhanzi(273,170,15);  
			
			showhanzi(177,204,22);  
			showhanzi(209,204,23);  
			showhanzi(241,204,24);  
			showhanzi(273,204,25);  
						 break;
		case _Alarm_Option://显示设置图标
			//LCD_Fill(96,34,192,204,BLACK);
			showhanzi(177,34,4);  
			showhanzi(209,34,5);  
			showhanzi(241,34,6);  
			showhanzi(273,34,7); 

			showhanzi(177,68,8);  
			showhanzi(209,68,9);  
			showhanzi(241,68,10);  
			showhanzi(273,68,11);  

			showhanzi_1(177,102,12,GBLUE);  
			showhanzi_1(209,102,13,GBLUE);  
			showhanzi_1(241,102,14,GBLUE);  
			showhanzi_1(273,102,15,GBLUE);  
			
			showhanzi(177,136,16);  
			showhanzi(209,136,17);  
			showhanzi(241,136,18);  
			showhanzi(273,136,19);  
			
			showhanzi(177,170,20);  
			showhanzi(209,170,21);  
			showhanzi(241,170,14);  
			showhanzi(273,170,15);  
			
			showhanzi(177,204,22);  
			showhanzi(209,204,23);  
			showhanzi(241,204,24);  
			showhanzi(273,204,25);  
						 break;
		case _Time_Option://显示关于信息图标
			//LCD_Fill(96,34,192,204,BLACK);
			showhanzi(177,34,4);  
			showhanzi(209,34,5);  
			showhanzi(241,34,6);  
			showhanzi(273,34,7); 

			showhanzi(177,68,8);  
			showhanzi(209,68,9);  
			showhanzi(241,68,10);  
			showhanzi(273,68,11); 
		
			showhanzi(177,102,12);  
			showhanzi(209,102,13);  
			showhanzi(241,102,14);  
			showhanzi(273,102,15);   
			
			showhanzi_1(177,136,16,GBLUE);  
			showhanzi_1(209,136,17,GBLUE);  
			showhanzi_1(241,136,18,GBLUE);  
			showhanzi_1(273,136,19,GBLUE);  
			
			showhanzi(177,170,20);  
			showhanzi(209,170,21);  
			showhanzi(241,170,14);  
			showhanzi(273,170,15);  
			
			showhanzi(177,204,22);  
			showhanzi(209,204,23);  
			showhanzi(241,204,24);  
			showhanzi(273,204,25);  
						 break;
		case _Data_Change_Option://显示关于信息图标
			//LCD_Fill(96,34,192,204,BLACK);
			showhanzi(177,34,4);  
			showhanzi(209,34,5);  
			showhanzi(241,34,6);  
			showhanzi(273,34,7); 

			showhanzi(177,68,8);  
			showhanzi(209,68,9);  
			showhanzi(241,68,10);  
			showhanzi(273,68,11); 
		
			showhanzi(177,102,12);  
			showhanzi(209,102,13);  
			showhanzi(241,102,14);  
			showhanzi(273,102,15);   
			
			showhanzi(177,136,16);  
			showhanzi(209,136,17);  
			showhanzi(241,136,18);  
			showhanzi(273,136,19);  
			
			showhanzi_1(177,170,20,GBLUE);  
			showhanzi_1(209,170,21,GBLUE);  
			showhanzi_1(241,170,14,GBLUE);  
			showhanzi_1(273,170,15,GBLUE);  
			
			showhanzi(177,204,22);  
			showhanzi(209,204,23);  
			showhanzi(241,204,24);  
			showhanzi(273,204,25);  
				 break;
		case _Iot_Option://显示关于信息图标
			//LCD_Fill(96,34,192,204,BLACK);
			showhanzi(177,34,4);  
			showhanzi(209,34,5);  
			showhanzi(241,34,6);  
			showhanzi(273,34,7); 

			showhanzi(177,68,8);  
			showhanzi(209,68,9);  
			showhanzi(241,68,10);  
			showhanzi(273,68,11); 
		
			showhanzi(177,102,12);  
			showhanzi(209,102,13);  
			showhanzi(241,102,14);  
			showhanzi(273,102,15);   
			
			showhanzi(177,136,16);  
			showhanzi(209,136,17);  
			showhanzi(241,136,18);  
			showhanzi(273,136,19);  
			
			showhanzi(177,170,20);  
			showhanzi(209,170,21);  
			showhanzi(241,170,14);  
			showhanzi(273,170,15);   
			
			showhanzi_1(177,204,22,GBLUE);  
			showhanzi_1(209,204,23,GBLUE);  
			showhanzi_1(241,204,24,GBLUE);  
			showhanzi_1(273,204,25,GBLUE); 
				 break;
		default:break;
	}
}




void Hz_Ctrl_Child(u8 page_index,u8 key_val){
	
	
	u8 temp_1;
	u8 temp_2;
	u16 data_v;
	u8 v_low;
	u8 v_high;
	
	showhanzi(48,132,16); showhanzi(82,132,17); showhanzi(116,132,14); showhanzi(150,132,15); showhanzi(184,132,44); 
	switch(key_val)
	{
		case KEY_PREVIOUS:
			out_voltage+=1;
			if(out_voltage>10)out_voltage=0;
			data_v = out_voltage*4095/10;
			v_low = data_v<<4 | 0x00;
		    v_high = data_v>>4 | 0x00;
			GP8201S_Write(0,0,v_low,v_high);
				break;
		case KEY_ENTER:
				break;
		case KEY_NEXT:
			out_voltage-=1;
			if(out_voltage==255)out_voltage=10;
			data_v = out_voltage*4095/10;
			v_low = data_v<<4 | 0x00;
			v_high = data_v>>4 | 0x00;
			GP8201S_Write(0,0,v_low,v_high);
				break;
		default:break;
	}
	temp_1 = out_voltage/10;
	temp_2 = out_voltage%10;
	if(!temp_1){showdigit_color(216,132,13,BLACK,BLACK);showdigit_color(232,132,temp_2,GREEN,BLACK);showdigit_color(264,132,14,GREEN,BLACK);}
	else{showdigit_color(216,132,temp_1,GREEN,BLACK);showdigit_color(232,132,temp_2,GREEN,BLACK);showdigit_color(264,132,14,GREEN,BLACK);}
}
void Alarm_Child(u8 page_index,u8 key_val){
	
	static u8 cur_pos=1;
	//温度一参与报警
	showhanzi(0,42,39);showhanzi(32,42,66);showhanzi(64,42,63);showhanzi(96,42,14);showhanzi(128,42,89);showhanzi(160,42,12);showhanzi(192,42,13);showhanzi(224,42,44);
	//温度二参与报警
	showhanzi(0,74,39);showhanzi(32,74,66);showhanzi(64,74,64);showhanzi(96,74,14);showhanzi(128,74,89);showhanzi(160,74,12);showhanzi(192,74,13);showhanzi(224,74,44);
	showhanzi(0,106,39);showhanzi(32,106,66);showhanzi(64,106,65);showhanzi(96,106,14);showhanzi(128,106,89);showhanzi(160,106,12);showhanzi(192,106,13);showhanzi(224,106,44);	
	//报警最高限
	showhanzi(0,138,12);showhanzi(32,138,13);showhanzi(64,138,41);showhanzi(96,138,42);showhanzi(128,138,45);showhanzi(160,138,44);
	//报警最低限
	showhanzi(0,170,12);showhanzi(32,170,13);showhanzi(64,170,41);showhanzi(96,170,43);showhanzi(128,170,45);showhanzi(160,170,44);
	//氨气报警
	showhanzi(0,202,105);showhanzi(32,202,106);showhanzi(64,202,12);showhanzi(96,202,13);showhanzi(128,202,44);
	//氨气报警值
	showhanzi(0,234,105);showhanzi(32,234,106);showhanzi(64,234,12);showhanzi(96,234,13);showhanzi(128,234,88);showhanzi(160,234,44);
	
	if(last_index!=_Alarm_Option)//判断是否是s第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS:
				if(cur_pos == 1) warn_temp1_flag = !warn_temp1_flag;
				if(cur_pos == 2) warn_temp2_flag = !warn_temp2_flag;
			    if(cur_pos == 3) warn_temp3_flag = !warn_temp3_flag;
				if(cur_pos == 4) limit_temp_maxvalue += 1;
				if(cur_pos == 5) limit_temp_minvalue += 1;
				if(cur_pos == 6) NH3_warn_flag = !NH3_warn_flag;
				if(cur_pos == 7) NH3_max += 1;
				if(limit_temp_maxvalue>=50)limit_temp_maxvalue=50;
			    if(limit_temp_minvalue>=50)limit_temp_minvalue=50;
				if(NH3_max>=30)NH3_max=30;
					break;
			case KEY_ENTER://确定(设置)按键
				cur_pos++;
				if(cur_pos == 8){
					cur_pos = 1;
				}
					break;
			case KEY_NEXT:
				if(cur_pos == 1) warn_temp1_flag = !warn_temp1_flag;
				if(cur_pos == 2) warn_temp2_flag = !warn_temp2_flag;
			    if(cur_pos == 3) warn_temp3_flag = !warn_temp3_flag;
				if(cur_pos == 4) limit_temp_maxvalue -= 1;
				if(cur_pos == 5) limit_temp_minvalue -= 1;
				if(cur_pos == 6) NH3_warn_flag = !NH3_warn_flag;
				if(cur_pos == 7) NH3_max -= 1;		
				if(limit_temp_maxvalue==255)limit_temp_maxvalue=0;
			    if(limit_temp_minvalue==255)limit_temp_minvalue=0;
				if(NH3_max==255)NH3_max=0;
					break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零
	
	uint8_t max_1 = limit_temp_maxvalue / 10;
	uint8_t max_2 = limit_temp_maxvalue % 10;
	
	uint8_t min_1 = limit_temp_minvalue / 10;
	uint8_t min_2 = limit_temp_minvalue % 10;
	
	uint8_t nh3_max_1 = NH3_max / 10;
	uint8_t nh3_max_2 = NH3_max % 10;

//	snprintf(str_max, sizeof(str_max), "%.1f", relay_structure[0].temp_control.max_temp);
//	snprintf(str_min, sizeof(str_min), "%.1f", relay_structure[0].temp_control.min_temp);

	switch(cur_pos)
	{
		case 1:
			if(warn_temp1_flag){showhanzi_1(256,42,90,1);showhanzi_1(288,42,91,1);}else{showhanzi_1(256,42,92,1);showhanzi_1(288,42,91,1);}
			if(warn_temp2_flag){showhanzi(256,74,90);showhanzi(288,74,91);}else{showhanzi(256,74,92);showhanzi(288,74,91);}
			if(warn_temp3_flag){showhanzi(256,106,90);showhanzi(288,106,91);}else{showhanzi(256,106,92);showhanzi(288,106,91);}
			showdigit_color(192,138,max_1,WHITE,BLACK);showdigit_color(208,138,max_2,WHITE,BLACK);
			showdigit_color(192,170,min_1,WHITE,BLACK);showdigit_color(208,170,min_2,WHITE,BLACK);
			if(NH3_warn_flag){showhanzi(160,202,90);showhanzi(192,202,91);}else{showhanzi(160,202,92);showhanzi(192,202,91);}
			showdigit_color(192,234,nh3_max_1,WHITE,BLACK);showdigit_color(208,234,nh3_max_2,WHITE,BLACK);
			break;
		case 2:
			if(warn_temp1_flag){showhanzi(256,42,90);showhanzi(288,42,91);}else{showhanzi(256,42,92);showhanzi(288,42,91);}
			if(warn_temp2_flag){showhanzi_1(256,74,90,1);showhanzi_1(288,74,91,1);}else{showhanzi_1(256,74,92,1);showhanzi_1(288,74,91,1);}
			if(warn_temp3_flag){showhanzi(256,106,90);showhanzi(288,106,91);}else{showhanzi(256,106,92);showhanzi(288,106,91);}
			showdigit_color(192,138,max_1,WHITE,BLACK);showdigit_color(208,138,max_2,WHITE,BLACK);
			showdigit_color(192,170,min_1,WHITE,BLACK);showdigit_color(208,170,min_2,WHITE,BLACK);
			if(NH3_warn_flag){showhanzi(160,202,90);showhanzi(192,202,91);}else{showhanzi(160,202,92);showhanzi(192,202,91);}
			showdigit_color(192,234,nh3_max_1,WHITE,BLACK);showdigit_color(208,234,nh3_max_2,WHITE,BLACK);
			break;
		case 3:
			if(warn_temp1_flag){showhanzi(256,42,90);showhanzi(288,42,91);}else{showhanzi(256,42,92);showhanzi(288,42,91);}
			if(warn_temp2_flag){showhanzi(256,74,90);showhanzi(288,74,91);}else{showhanzi(256,74,92);showhanzi(288,74,91);}
			if(warn_temp3_flag){showhanzi_1(256,106,90,0);showhanzi_1(288,106,91,0);}else{showhanzi_1(256,106,92,0);showhanzi_1(288,106,91,0);}
			showdigit_color(192,138,max_1,WHITE,BLACK);showdigit_color(208,138,max_2,WHITE,BLACK);
			showdigit_color(192,170,min_1,WHITE,BLACK);showdigit_color(208,170,min_2,WHITE,BLACK);
			if(NH3_warn_flag){showhanzi(160,202,90);showhanzi(192,202,91);}else{showhanzi(160,202,92);showhanzi(192,202,91);}
			showdigit_color(192,234,nh3_max_1,WHITE,BLACK);showdigit_color(208,234,nh3_max_2,WHITE,BLACK);
			break;
		case 4:
			if(warn_temp1_flag){showhanzi(256,42,90);showhanzi(288,42,91);}else{showhanzi(256,42,92);showhanzi(288,42,91);}
			if(warn_temp2_flag){showhanzi(256,74,90);showhanzi(288,74,91);}else{showhanzi(256,74,92);showhanzi(288,74,91);}
			if(warn_temp3_flag){showhanzi(256,106,90);showhanzi(288,106,91);}else{showhanzi(256,106,92);showhanzi(288,106,91);}
			showdigit_color(192,138,max_1,BLACK,WHITE);showdigit_color(208,138,max_2,BLACK,WHITE);
			showdigit_color(192,170,min_1,WHITE,BLACK);showdigit_color(208,170,min_2,WHITE,BLACK);
			if(NH3_warn_flag){showhanzi(160,202,90);showhanzi(192,202,91);}else{showhanzi(160,202,92);showhanzi(192,202,91);}
			showdigit_color(192,234,nh3_max_1,WHITE,BLACK);showdigit_color(208,234,nh3_max_2,WHITE,BLACK);
			break;
		case 5:
			if(warn_temp1_flag){showhanzi(256,42,90);showhanzi(288,42,91);}else{showhanzi(256,42,92);showhanzi(288,42,91);}
			if(warn_temp2_flag){showhanzi(256,74,90);showhanzi(288,74,91);}else{showhanzi(256,74,92);showhanzi(288,74,91);}
			if(warn_temp3_flag){showhanzi(256,106,90);showhanzi(288,106,91);}else{showhanzi(256,106,92);showhanzi(288,106,91);}
			showdigit_color(192,138,max_1,WHITE,BLACK);showdigit_color(208,138,max_2,WHITE,BLACK);
			showdigit_color(192,170,min_1,BLACK,WHITE);showdigit_color(208,170,min_2,BLACK,WHITE);
			if(NH3_warn_flag){showhanzi(160,202,90);showhanzi(192,202,91);}else{showhanzi(160,202,92);showhanzi(192,202,91);}
			showdigit_color(192,234,nh3_max_1,WHITE,BLACK);showdigit_color(208,234,nh3_max_2,WHITE,BLACK);
			break;
		case 6:
			if(warn_temp1_flag){showhanzi(256,42,90);showhanzi(288,42,91);}else{showhanzi(256,42,92);showhanzi(288,42,91);}
			if(warn_temp2_flag){showhanzi(256,74,90);showhanzi(288,74,91);}else{showhanzi(256,74,92);showhanzi(288,74,91);}
			if(warn_temp3_flag){showhanzi(256,106,90);showhanzi(288,106,91);}else{showhanzi(256,106,92);showhanzi(288,106,91);}
			showdigit_color(192,138,max_1,WHITE,BLACK);showdigit_color(208,138,max_2,WHITE,BLACK);
			showdigit_color(192,170,min_1,WHITE,BLACK);showdigit_color(208,170,min_2,WHITE,BLACK);
			if(NH3_warn_flag){showhanzi_1(160,202,90,1);showhanzi_1(192,202,91,1);}else{showhanzi_1(160,202,92,1);showhanzi_1(192,202,91,1);}
			showdigit_color(192,234,nh3_max_1,WHITE,BLACK);showdigit_color(208,234,nh3_max_2,WHITE,BLACK);
			break;
		case 7:
			if(warn_temp1_flag){showhanzi(256,42,90);showhanzi(288,42,91);}else{showhanzi(256,42,92);showhanzi(288,42,91);}
			if(warn_temp2_flag){showhanzi(256,74,90);showhanzi(288,74,91);}else{showhanzi(256,74,92);showhanzi(288,74,91);}
			if(warn_temp3_flag){showhanzi(256,106,90);showhanzi(288,106,91);}else{showhanzi(256,106,92);showhanzi(288,106,91);}
			showdigit_color(192,138,max_1,WHITE,BLACK);showdigit_color(208,138,max_2,WHITE,BLACK);
			showdigit_color(192,170,min_1,WHITE,BLACK);showdigit_color(208,170,min_2,WHITE,BLACK);
			if(NH3_warn_flag){showhanzi(160,202,90);showhanzi(192,202,91);}else{showhanzi(160,202,92);showhanzi(192,202,91);}
			showdigit_color(192,234,nh3_max_1,BLACK,WHITE);showdigit_color(208,234,nh3_max_2,BLACK,WHITE);
			break;
		default:break;
	}
	

}
void Time_Child(u8 page_index,u8 key_val){
	static u8 cur_pos=1;
	static int temp_year=2024;
	static u8 temp_mouth=1;
	static u8 temp_day=1;
	static u8 temp_h=0;
	static u8 temp_m=0;
	static u8 temp_s=0;

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


	if(last_index!=_Time_Option)//判断是否是s第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS:
				if(cur_pos == 1) temp_year += 1;
				if(cur_pos == 2) temp_mouth += 1;
			    if(cur_pos == 3) temp_day += 1;
				if(cur_pos == 4) temp_h += 1;
				if(cur_pos == 5) temp_m += 1;
				if(cur_pos == 6) temp_s += 1;
			
				if(temp_year > 2099) temp_year=2099;
				if(temp_mouth > 12) temp_mouth=12;	
				if(temp_day > 31) temp_day=31;	
			    if(temp_h > 23) temp_h=23;
				if(temp_m > 60) temp_m=60;
			    if(temp_s > 60) temp_s=60;
				RTC_Set(temp_year,temp_mouth,temp_day,temp_h,temp_m,temp_s);
				
					break;
			case KEY_ENTER://确定(设置)按键
				cur_pos++;
				if(cur_pos == 7){
					cur_pos = 1;
				}
					break;
			case KEY_NEXT:
				if(cur_pos == 1) temp_year -= 1;
				if(cur_pos == 2) temp_mouth -= 1;
			    if(cur_pos == 3) temp_day -= 1;
				if(cur_pos == 4) temp_h -= 1;
				if(cur_pos == 5) temp_m -= 1;
				if(cur_pos == 6) temp_s -= 1;
			
				if(temp_year < 1970) temp_year=1970;
				if(temp_mouth == 0) temp_mouth=1;	
				if(temp_day == 0) temp_day=1;	
			    if(temp_h == 255) temp_h=0;
				if(temp_m == 255) temp_m=0;
			    if(temp_s == 255) temp_s=0;
			    RTC_Set(temp_year,temp_mouth,temp_day,temp_h,temp_m,temp_s);
					break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零

	switch(cur_pos)
	{
		case 1:
			showdigit_color(0,170,temp_year/1000,WHITE,BLUE);showdigit_color(16,170,(temp_year/100)%10,WHITE,BLUE);showdigit_color(32,170,(temp_year/10)%10,WHITE,BLUE);showdigit_color(48,170,temp_year%10,WHITE,BLUE);
			showdigit_color(64,170,11,WHITE,BLACK);
			showdigit_color(80,170,temp_mouth/10,WHITE,BLACK);showdigit_color(96,170,temp_mouth%10,WHITE,BLACK);
			showdigit_color(112,170,11,WHITE,BLACK);
			showdigit_color(128,170,temp_day/10,WHITE,BLACK);showdigit_color(144,170,temp_day%10,WHITE,BLACK);
			showdigit_color(192,170,temp_h/10,WHITE,BLACK);showdigit_color(208,170,temp_h%10,WHITE,BLACK);
			showdigit_color(224,170,11,WHITE,BLACK);
			showdigit_color(240,170,temp_m/10,WHITE,BLACK);showdigit_color(256,170,temp_m%10,WHITE,BLACK);
			showdigit_color(272,170,11,WHITE,BLACK);
			showdigit_color(288,170,temp_s/10,WHITE,BLACK);showdigit_color(304,170,temp_s%10,WHITE,BLACK);
			break;
		case 2:
			showdigit_color(0,170,temp_year/1000,WHITE,BLACK);showdigit_color(16,170,(temp_year/100)%10,WHITE,BLACK);showdigit_color(32,170,(temp_year/10)%10,WHITE,BLACK);showdigit_color(48,170,temp_year%10,WHITE,BLACK);
			showdigit_color(64,170,11,WHITE,BLACK);
			showdigit_color(80,170,temp_mouth/10,WHITE,BLUE);showdigit_color(96,170,temp_mouth%10,WHITE,BLUE);
			showdigit_color(112,170,11,WHITE,BLACK);
			showdigit_color(128,170,temp_day/10,WHITE,BLACK);showdigit_color(144,170,temp_day%10,WHITE,BLACK);
			showdigit_color(192,170,temp_h/10,WHITE,BLACK);showdigit_color(208,170,temp_h%10,WHITE,BLACK);
			showdigit_color(224,170,11,WHITE,BLACK);
			showdigit_color(240,170,temp_m/10,WHITE,BLACK);showdigit_color(256,170,temp_m%10,WHITE,BLACK);
			showdigit_color(272,170,11,WHITE,BLACK);
			showdigit_color(288,170,temp_s/10,WHITE,BLACK);showdigit_color(304,170,temp_s%10,WHITE,BLACK);
			break;
		case 3:
			showdigit_color(0,170,temp_year/1000,WHITE,BLACK);showdigit_color(16,170,(temp_year/100)%10,WHITE,BLACK);showdigit_color(32,170,(temp_year/10)%10,WHITE,BLACK);showdigit_color(48,170,temp_year%10,WHITE,BLACK);
			showdigit_color(64,170,11,WHITE,BLACK);
			showdigit_color(80,170,temp_mouth/10,WHITE,BLACK);showdigit_color(96,170,temp_mouth%10,WHITE,BLACK);
			showdigit_color(112,170,11,WHITE,BLACK);
			showdigit_color(128,170,temp_day/10,WHITE,BLUE);showdigit_color(144,170,temp_day%10,WHITE,BLUE);
			showdigit_color(192,170,temp_h/10,WHITE,BLACK);showdigit_color(208,170,temp_h%10,WHITE,BLACK);
			showdigit_color(224,170,11,WHITE,BLACK);
			showdigit_color(240,170,temp_m/10,WHITE,BLACK);showdigit_color(256,170,temp_m%10,WHITE,BLACK);
			showdigit_color(272,170,11,WHITE,BLACK);
			showdigit_color(288,170,temp_s/10,WHITE,BLACK);showdigit_color(304,170,temp_s%10,WHITE,BLACK);

			break;
		case 4:
			showdigit_color(0,170,temp_year/1000,WHITE,BLACK);showdigit_color(16,170,(temp_year/100)%10,WHITE,BLACK);showdigit_color(32,170,(temp_year/10)%10,WHITE,BLACK);showdigit_color(48,170,temp_year%10,WHITE,BLACK);
			showdigit_color(64,170,11,WHITE,BLACK);
			showdigit_color(80,170,temp_mouth/10,WHITE,BLACK);showdigit_color(96,170,temp_mouth%10,WHITE,BLACK);
			showdigit_color(112,170,11,WHITE,BLACK);
			showdigit_color(128,170,temp_day/10,WHITE,BLACK);showdigit_color(144,170,temp_day%10,WHITE,BLACK);
			showdigit_color(192,170,temp_h/10,WHITE,BLUE);showdigit_color(208,170,temp_h%10,WHITE,BLUE);
			showdigit_color(224,170,11,WHITE,BLACK);
			showdigit_color(240,170,temp_m/10,WHITE,BLACK);showdigit_color(256,170,temp_m%10,WHITE,BLACK);
			showdigit_color(272,170,11,WHITE,BLACK);
			showdigit_color(288,170,temp_s/10,WHITE,BLACK);showdigit_color(304,170,temp_s%10,WHITE,BLACK);
			break;
		case 5:
			showdigit_color(0,170,temp_year/1000,WHITE,BLACK);showdigit_color(16,170,(temp_year/100)%10,WHITE,BLACK);showdigit_color(32,170,(temp_year/10)%10,WHITE,BLACK);showdigit_color(48,170,temp_year%10,WHITE,BLACK);
			showdigit_color(64,170,11,WHITE,BLACK);
			showdigit_color(80,170,temp_mouth/10,WHITE,BLACK);showdigit_color(96,170,temp_mouth%10,WHITE,BLACK);
			showdigit_color(112,170,11,WHITE,BLACK);
			showdigit_color(128,170,temp_day/10,WHITE,BLACK);showdigit_color(144,170,temp_day%10,WHITE,BLACK);
			showdigit_color(192,170,temp_h/10,WHITE,BLACK);showdigit_color(208,170,temp_h%10,WHITE,BLACK);
			showdigit_color(224,170,11,WHITE,BLACK);
			showdigit_color(240,170,temp_m/10,WHITE,BLUE);showdigit_color(256,170,temp_m%10,WHITE,BLUE);
			showdigit_color(272,170,11,WHITE,BLACK);
			showdigit_color(288,170,temp_s/10,WHITE,BLACK);showdigit_color(304,170,temp_s%10,WHITE,BLACK);
			break;
		case 6:
			showdigit_color(0,170,temp_year/1000,WHITE,BLACK);showdigit_color(16,170,(temp_year/100)%10,WHITE,BLACK);showdigit_color(32,170,(temp_year/10)%10,WHITE,BLACK);showdigit_color(48,170,temp_year%10,WHITE,BLACK);
			showdigit_color(64,170,11,WHITE,BLACK);
			showdigit_color(80,170,temp_mouth/10,WHITE,BLACK);showdigit_color(96,170,temp_mouth%10,WHITE,BLACK);
			showdigit_color(112,170,11,WHITE,BLACK);
			showdigit_color(128,170,temp_day/10,WHITE,BLACK);showdigit_color(144,170,temp_day%10,WHITE,BLACK);
			showdigit_color(192,170,temp_h/10,WHITE,BLACK);showdigit_color(208,170,temp_h%10,WHITE,BLACK);
			showdigit_color(224,170,11,WHITE,BLACK);
			showdigit_color(240,170,temp_m/10,WHITE,BLACK);showdigit_color(256,170,temp_m%10,WHITE,BLACK);
			showdigit_color(272,170,11,WHITE,BLACK);
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

	
	LCD_ShowString(50,250,16,"IMEI:",0);LCD_ShowString(90,250,16,imei_no,0);
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

	switch(page_index)
	{
		case _Air_Blower_Child_1://显示风机1
			showhanzi_1(6,36,6,1);showhanzi_1(38,36,7,1);showhanzi_1(70,36,26,1);showhanzi_1(102,36,27,1); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); 
				 break;
		case _Air_Blower_Child_2://显示风机2
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi_1(6,72,6,1);showhanzi_1(38,72,7,1);showhanzi_1(70,72,28,1);showhanzi_1(102,72,27,1);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); 
				 break;
		case _Air_Blower_Child_3://显示风机3
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi_1(6,108,6,1);showhanzi_1(38,108,7,1);showhanzi_1(70,108,29,1);showhanzi_1(102,108,27,1);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); 
				 break;
		case _Air_Blower_Child_4://显示风机
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi_1(6,144,6,1);showhanzi_1(38,144,7,1);showhanzi_1(70,144,30,1);showhanzi_1(102,144,27,1);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); 
				 break;
		case _Air_Blower_Child_5://显示风机
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi_1(6,180,6,1);showhanzi_1(38,180,7,1);showhanzi_1(70,180,31,1);showhanzi_1(102,180,27,1); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); 
				 break;
		case _Air_Blower_Child_6://显示风机
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi_1(346,36,6,1);showhanzi_1(378,36,7,1);showhanzi_1(410,36,32,1);showhanzi_1(442,36,27,1);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); 
				 break;
		case _Air_Blower_Child_7://显示风机
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi_1(346,72,6,1);showhanzi_1(378,72,7,1);showhanzi_1(410,72,33,1);showhanzi_1(442,72,27,1);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); 
				 break;
		case _Air_Blower_Child_8://显示风机
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27); 
			showhanzi_1(346,108,6,1);showhanzi_1(378,108,7,1);showhanzi_1(410,108,34,1);showhanzi_1(442,108,27,1);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27);  
				 break;
		case _Air_Blower_Child_9://显示风机
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi_1(346,144,6,1);showhanzi_1(378,144,7,1);showhanzi_1(410,144,35,1);showhanzi_1(442,144,27,1);  
			showhanzi(346,180,6);showhanzi(378,180,7);showdigit_color(410,180,1,WHITE,BLACK);showdigit_color(426,180,0,WHITE,BLACK);showhanzi(442,180,27); 
				 break;
		case _Air_Blower_Child_10://显示风机
			showhanzi(6,36,6);showhanzi(38,36,7);showhanzi(70,36,26);showhanzi(102,36,27); 
			showhanzi(6,72,6);showhanzi(38,72,7);showhanzi(70,72,28);showhanzi(102,72,27);  
			showhanzi(6,108,6);showhanzi(38,108,7);showhanzi(70,108,29);showhanzi(102,108,27);
			showhanzi(6,144,6);showhanzi(38,144,7);showhanzi(70,144,30);showhanzi(102,144,27);  
			showhanzi(6,180,6);showhanzi(38,180,7);showhanzi(70,180,31);showhanzi(102,180,27); 

			showhanzi(346,36,6);showhanzi(378,36,7);showhanzi(410,36,32);showhanzi(442,36,27);  
			showhanzi(346,72,6);showhanzi(378,72,7);showhanzi(410,72,33);showhanzi(442,72,27);  
			showhanzi(346,108,6);showhanzi(378,108,7);showhanzi(410,108,34);showhanzi(442,108,27);
			showhanzi(346,144,6);showhanzi(378,144,7);showhanzi(410,144,35);showhanzi(442,144,27);  
			showhanzi_1(346,180,6,1);showhanzi_1(378,180,7,1);showdigit_color(410,180,1,BLACK,WHITE);showdigit_color(426,180,0,BLACK,WHITE);showhanzi_1(442,180,27,1); 
				 break;
		default:break;		
	}
}



void Air_Blower_Child_Ctrl_1_Fun(u8 page_index,u8 key_val){
	static u8 last_index_save=0;
	
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

	
	if(last_index!=_Air_Blower_Child_1 && 
	   last_index!=_Air_Blower_Child_2 &&
	   last_index!=_Air_Blower_Child_3 &&
	   last_index!=_Air_Blower_Child_4 &&
	   last_index!=_Air_Blower_Child_5 &&
	   last_index!=_Air_Blower_Child_6 &&
	   last_index!=_Air_Blower_Child_7 &&
	   last_index!=_Air_Blower_Child_8 &&
	   last_index!=_Air_Blower_Child_9 &&
	   last_index!=_Air_Blower_Child_10
	  )//判断是否是s第一次进入此界面
	{

		switch(key_val)
		{
			case KEY_PREVIOUS:
				if(enter_config_flag_1 == 0 && enter_config_flag_2 == 0 && enter_config_flag_3 == 0  && enter_config_flag_4 == 0 && enter_config_flag_5 == 0 
					&& enter_config_flag_6 == 0 && enter_config_flag_7 == 0 && enter_config_flag_8 == 0  && enter_config_flag_9 == 0 && enter_config_flag_10 == 0
				){
					relay_structure[last_index_save-12].relay_mode +=1;
				}else{
					if(relay_structure[last_index_save-12].relay_mode == 1){
						if(cur_pos1 == 1)relay_structure[last_index_save-12].temp_control.max_temp+=1;
						if(cur_pos1 == 2)relay_structure[last_index_save-12].temp_control.min_temp+=1;
						if(cur_pos1 == 3)relay_structure[last_index_save-12].temp_control.temp_choose_flag+=1;
						if(cur_pos1 == 4)relay_structure[last_index_save-12].temp_control.startup_mode = !(relay_structure[last_index_save-12].temp_control.startup_mode);
						if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 4) relay_structure[last_index_save-12].temp_control.temp_choose_flag = 0;
						if(cur_pos1 == 5)relay_structure[last_index_save-12].temp_control.max_nh3+=1;
						if(cur_pos1 == 6)relay_structure[last_index_save-12].temp_control.min_nh3+=1;
						if(relay_structure[last_index_save-12].temp_control.max_nh3>=50)relay_structure[last_index_save-12].temp_control.max_nh3=50;
						if(relay_structure[last_index_save-12].temp_control.min_nh3>=50)relay_structure[last_index_save-12].temp_control.min_nh3=50;
						
					}
					if(relay_structure[last_index_save-12].relay_mode == 2){
						if(cur_pos2 == 1) relay_structure[last_index_save-12].time_control.time_open += 1;
				        if(cur_pos2 == 2) relay_structure[last_index_save-12].time_control.time_stop += 1;
						if(relay_structure[last_index_save-12].time_control.time_open==10000)relay_structure[last_index_save-12].time_control.time_open=0;
						if(relay_structure[last_index_save-12].time_control.time_stop==10000)relay_structure[last_index_save-12].time_control.time_stop=0;
					}
					if(relay_structure[last_index_save-12].relay_mode == 3){
						if(cur_pos3 == 1)relay_structure[last_index_save-12].temp_control.max_temp+=1;
						if(cur_pos3 == 2)relay_structure[last_index_save-12].temp_control.min_temp+=1;
						if(cur_pos3 == 3)relay_structure[last_index_save-12].temp_control.temp_choose_flag+=1;
						if(cur_pos3 == 4)relay_structure[last_index_save-12].temp_control.startup_mode = !(relay_structure[last_index_save-12].temp_control.startup_mode);
						if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 4) relay_structure[last_index_save-12].temp_control.temp_choose_flag = 0;
						if(cur_pos3 == 5) relay_structure[last_index_save-12].time_control.time_open += 1;
				        if(cur_pos3 == 6) relay_structure[last_index_save-12].time_control.time_stop += 1;
						if(relay_structure[last_index_save-12].time_control.time_open==10000)relay_structure[last_index_save-12].time_control.time_open=0;
						if(relay_structure[last_index_save-12].time_control.time_stop==10000)relay_structure[last_index_save-12].time_control.time_stop=0;
					}
					if(relay_structure[last_index_save-12].relay_mode == 4){
						if(cur_pos4 == 1)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.hour+=1;
						if(cur_pos4 == 2)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes+=1;
						if(cur_pos4 == 3)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.seconds+=1;
						if(cur_pos4 == 4)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off;
						if(cur_pos4 == 5)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.hour+=1;
						if(cur_pos4 == 6)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes+=1;
						if(cur_pos4 == 7)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.seconds+=1;
						if(cur_pos4 == 8)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off;
						if(cur_pos4 == 9)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.hour+=1;
						if(cur_pos4 == 10)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes+=1;
						if(cur_pos4 == 11)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.seconds+=1;
						if(cur_pos4 == 12)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off;
						if(cur_pos4 == 13)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.hour+=1;
						if(cur_pos4 == 14)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes+=1;
						if(cur_pos4 == 15)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.seconds+=1;
						if(cur_pos4 == 16)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off;
						if(cur_pos4 == 17)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.hour+=1;
						if(cur_pos4 == 18)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes+=1;
						if(cur_pos4 == 19)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.seconds+=1;
						if(cur_pos4 == 20)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off;
						
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.hour == 24) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.hour = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.seconds == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.seconds = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.hour == 24) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.hour = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.seconds == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.seconds = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.hour == 24) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.hour = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.seconds == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.seconds = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.hour == 24) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.hour = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.seconds == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.seconds = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.hour == 24) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.hour = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes = 0;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.seconds == 61) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.seconds = 0;
					}
					
					
					if(relay_structure[last_index_save-12].temp_control.max_temp>99)relay_structure[last_index_save-12].temp_control.max_temp=99;
					
				}
				
				if(relay_structure[last_index_save-12].relay_mode==5)relay_structure[last_index_save-12].relay_mode=0;//不使用该风机
					break;
			case KEY_ENTER://确定(设置)按键
				
				if(last_index_save-12==0)enter_config_flag_1 += 1;
				if(last_index_save-12==1)enter_config_flag_2 += 1;
				if(last_index_save-12==2)enter_config_flag_3 += 1;
				if(last_index_save-12==3)enter_config_flag_4 += 1;
				if(last_index_save-12==4)enter_config_flag_5 += 1;
				if(last_index_save-12==5)enter_config_flag_6 += 1;
				if(last_index_save-12==6)enter_config_flag_7 += 1;
				if(last_index_save-12==7)enter_config_flag_8 += 1;
				if(last_index_save-12==8)enter_config_flag_9 += 1;
				if(last_index_save-12==9)enter_config_flag_10 += 1;
			
				//设置刷新标志位为刷新
				if(enter_config_flag_1>2 || enter_config_flag_2>2||enter_config_flag_3>2||enter_config_flag_4>2||enter_config_flag_5>2
					||enter_config_flag_6>2||enter_config_flag_7>2||enter_config_flag_8>2||enter_config_flag_9>2||enter_config_flag_10>2
				){
					if(relay_structure[last_index_save-12].relay_mode == 1){
						cur_pos1++;
						if(cur_pos1 == 7){
							cur_pos1 = 1;
						}
					}
					if(relay_structure[last_index_save-12].relay_mode == 2){
						cur_pos2++;
						if(cur_pos2 == 3){
							cur_pos2 = 1;
						}
					}
					if(relay_structure[last_index_save-12].relay_mode == 3){
						cur_pos3++;
						if(cur_pos3 == 7){
							cur_pos3 = 1;
						}
					}
					if(relay_structure[last_index_save-12].relay_mode == 4){
						cur_pos4++;
						if(cur_pos4 == 21){
							cur_pos4 = 1;
						}
					}
				}
					break;
			case KEY_NEXT:
				if(enter_config_flag_1 == 0 && enter_config_flag_2 == 0 && enter_config_flag_3 == 0  && enter_config_flag_4 == 0 && enter_config_flag_5 == 0 
					&& enter_config_flag_6 == 0 && enter_config_flag_7 == 0 && enter_config_flag_8 == 0  && enter_config_flag_9 == 0 && enter_config_flag_10 == 0
				){
					relay_structure[last_index_save-12].relay_mode -=1;
				}else{
					if(relay_structure[last_index_save-12].relay_mode == 1){
						if(cur_pos1 == 1)relay_structure[last_index_save-12].temp_control.max_temp-=1;
						if(cur_pos1 == 2)relay_structure[last_index_save-12].temp_control.min_temp-=1;
						if(cur_pos1 == 3)relay_structure[last_index_save-12].temp_control.temp_choose_flag+=1;
						if(cur_pos1 == 4)relay_structure[last_index_save-12].temp_control.startup_mode = !(relay_structure[last_index_save-12].temp_control.startup_mode);
						if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 4) relay_structure[last_index_save-12].temp_control.temp_choose_flag = 0;
						if(cur_pos1 == 5)relay_structure[last_index_save-12].temp_control.max_nh3-=1;
						if(cur_pos1 == 6)relay_structure[last_index_save-12].temp_control.min_nh3-=1;
						if(relay_structure[last_index_save-12].temp_control.max_nh3<=1)relay_structure[last_index_save-12].temp_control.max_nh3=1;
						if(relay_structure[last_index_save-12].temp_control.min_nh3<=1)relay_structure[last_index_save-12].temp_control.min_nh3=1;
						
					}
					if(relay_structure[last_index_save-12].relay_mode == 2){
						if(cur_pos2 == 1) relay_structure[last_index_save-12].time_control.time_open -= 1;
				        if(cur_pos2 == 2) relay_structure[last_index_save-12].time_control.time_stop -= 1;
						if(relay_structure[last_index_save-12].time_control.time_open==65535)relay_structure[last_index_save-12].time_control.time_open=9999;
						if(relay_structure[last_index_save-12].time_control.time_stop==65535)relay_structure[last_index_save-12].time_control.time_stop=9999;
					}
					if(relay_structure[last_index_save-12].relay_mode == 3){
						if(cur_pos3 == 1)relay_structure[last_index_save-12].temp_control.max_temp-=1;
						if(cur_pos3 == 2)relay_structure[last_index_save-12].temp_control.min_temp-=1;
						if(cur_pos3 == 3)relay_structure[last_index_save-12].temp_control.temp_choose_flag+=1;
						if(cur_pos3 == 4)relay_structure[last_index_save-12].temp_control.startup_mode = !(relay_structure[last_index_save-12].temp_control.startup_mode);
						if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 4) relay_structure[last_index_save-12].temp_control.temp_choose_flag = 0;
						
						if(cur_pos3 == 5) relay_structure[last_index_save-12].time_control.time_open -= 1;
				        if(cur_pos3 == 6) relay_structure[last_index_save-12].time_control.time_stop -= 1;
						if(relay_structure[last_index_save-12].time_control.time_open==65535)relay_structure[last_index_save-12].time_control.time_open=9999;
						if(relay_structure[last_index_save-12].time_control.time_stop==65535)relay_structure[last_index_save-12].time_control.time_stop=9999;
					}
					if(relay_structure[last_index_save-12].relay_mode == 4){
						if(cur_pos4 == 1)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.hour-=1;
						if(cur_pos4 == 2)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes-=1;
						if(cur_pos4 == 3)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.seconds-=1;
						if(cur_pos4 == 4)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off;
						if(cur_pos4 == 5)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.hour-=1;
						if(cur_pos4 == 6)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes-=1;
						if(cur_pos4 == 7)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.seconds-=1;
						if(cur_pos4 == 8)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off;
						if(cur_pos4 == 9)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.hour-=1;
						if(cur_pos4 == 10)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes-=1;
						if(cur_pos4 == 11)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.seconds-=1;
						if(cur_pos4 == 12)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off;
						if(cur_pos4 == 13)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.hour-=1;
						if(cur_pos4 == 14)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes-=1;
						if(cur_pos4 == 15)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.seconds-=1;
						if(cur_pos4 == 16)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off;
						if(cur_pos4 == 17)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.hour-=1;
						if(cur_pos4 == 18)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes-=1;
						if(cur_pos4 == 19)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.seconds-=1;
						if(cur_pos4 == 20)relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off=!relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off;
					
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.hour == 255) {relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.hour = 23;}
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.seconds == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.seconds = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.hour == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.hour = 23;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.seconds == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.seconds = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.hour == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.hour = 23;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.seconds == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.seconds = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.hour == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.hour = 23;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.seconds == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.seconds = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.hour == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.hour = 23;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes = 60;
						if(relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.seconds == 255) relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.seconds = 60;
					}
					if(relay_structure[last_index_save-12].temp_control.min_temp<0)relay_structure[last_index_save-12].temp_control.min_temp=0;
				}
				if(relay_structure[last_index_save-12].relay_mode==255)relay_structure[last_index_save-12].relay_mode=4;//分时控
					break;
				
//			case KEY_BACK:
//				enter_config_flag_1=0;
//				last_index_save=0;//为下一次进入页面判断是哪个风机；
//					break;
			default:break;
		}
    }else {last_index_save=last_index;}//第一次进入此界面,界面指针清零

	
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
		
		switch(relay_structure[last_index_save-12].relay_mode){
			case 0:
				showhanzi(160,35,92);showhanzi(192,35,90);showhanzi(224,35,91);//不使用
					break;	
			case 1:
			//显示温控信息
				u8 max_temp_v1 = relay_structure[last_index_save-12].temp_control.max_temp;
				u8 min_temp_v1 = relay_structure[last_index_save-12].temp_control.min_temp;
				u16 max_nh3_v1 = relay_structure[last_index_save-12].temp_control.max_nh3;
				u16 min_nh3_v1 = relay_structure[last_index_save-12].temp_control.min_nh3;
				showhanzi(160,35,39);showhanzi(192,35,10);showhanzi(224,35,38);
			
				showhanzi(6,67,41);showhanzi(38,67,42);showhanzi(70,67,39);showhanzi(102,67,44);//最高温
				showhanzi(6,103,41);showhanzi(38,103,43);showhanzi(70,103,39); showhanzi(102,103,44);//最低温
				showhanzi(6,139,50);showhanzi(38,139,51);showhanzi(70,139,52); showhanzi(102,139,53);showhanzi(134,139,54);showhanzi(166,139,44);//对应传感器
				showhanzi(6,175,55);showhanzi(38,175,56);showhanzi(70,175,57); showhanzi(102,175,58);showhanzi(134,175,44);//启动方式
				showhanzi(6,211,105);showhanzi(38,211,106);showhanzi(70,211,41); showhanzi(102,211,42);showhanzi(134,211,88);showhanzi(166,211,44);//氨气最高值
				showhanzi(6,247,105);showhanzi(38,247,106);showhanzi(70,247,41); showhanzi(102,247,43);showhanzi(134,247,88);showhanzi(166,247,44);//氨气最低值
				
				
			    showdigit_color(134,67,max_temp_v1/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v1%10,WHITE,BLACK);
			    showdigit_color(134,103,min_temp_v1/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v1%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(200,211,max_nh3_v1/100,WHITE,BLACK);showdigit_color(216,211,(max_nh3_v1/10)%10,WHITE,BLACK);showdigit_color(232,211,max_nh3_v1%10,WHITE,BLACK);
			    showdigit_color(200,247,min_nh3_v1/100,WHITE,BLACK);showdigit_color(216,247,(min_nh3_v1/10)%10,WHITE,BLACK);showdigit_color(232,247,min_nh3_v1%10,WHITE,BLACK);
				
					break;
			case 2:
			//显示时控信息	
				uint16_t time_open_v1=relay_structure[last_index_save-12].time_control.time_open;
		        uint16_t time_stop_v1=relay_structure[last_index_save-12].time_control.time_stop;
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
				u8 max_temp_v11 = relay_structure[last_index_save-12].temp_control.max_temp;
				u8 min_temp_v11 = relay_structure[last_index_save-12].temp_control.min_temp;
				uint16_t time_open_v11=relay_structure[last_index_save-12].time_control.time_open;
		        uint16_t time_stop_v11=relay_structure[last_index_save-12].time_control.time_stop;
				//温时控
				showhanzi(160,35,39);showhanzi(192,35,18);showhanzi(224,35,10);

				showhanzi(6,67,41);showhanzi(38,67,42);showhanzi(70,67,39);showhanzi(102,67,44);//最高温
				showhanzi(6,103,41);showhanzi(38,103,43);showhanzi(70,103,39); showhanzi(102,103,44);//最低温
				showhanzi(6,139,50);showhanzi(38,139,51);showhanzi(70,139,52); showhanzi(102,139,53);showhanzi(134,139,54);showhanzi(166,139,44);//对应传感器
				showhanzi(6,175,55);showhanzi(38,175,56);showhanzi(70,175,57); showhanzi(102,175,58);showhanzi(134,175,44);//启动方式
				
			    showdigit_color(134,67,max_temp_v11/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v11%10,WHITE,BLACK);
			    showdigit_color(134,103,min_temp_v11/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v11%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
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
			
				time_c_v1[0].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.hour;
				time_c_v1[0].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes;
				time_c_v1[0].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.seconds;
				time_c_v1[1].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.hour;
				time_c_v1[1].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes;
				time_c_v1[1].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.seconds;
				time_c_v1[2].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.hour;
				time_c_v1[2].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes;
				time_c_v1[2].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.seconds;
				time_c_v1[3].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.hour;
				time_c_v1[3].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes;
				time_c_v1[3].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.seconds;
				time_c_v1[4].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.hour;
				time_c_v1[4].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes;
				time_c_v1[4].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.seconds;

				//分时控
				showhanzi(160,35,40);showhanzi(192,35,18);showhanzi(224,35,10);
				
				showhanzi(0,67,78);showhanzi(108,67,18);showhanzi(180,67,40);showhanzi(252,67,69);//1 时分秒
				showdigit_color(72,67,time_c_v1[0].time_h/10,WHITE,BLACK);showdigit_color(88,67,time_c_v1[0].time_h%10,WHITE,BLACK);
				showdigit_color(144,67,time_c_v1[0].time_m/10,WHITE,BLACK);showdigit_color(160,67,time_c_v1[0].time_m%10,WHITE,BLACK);
				showdigit_color(216,67,time_c_v1[0].time_s/10,WHITE,BLACK);showdigit_color(232,67,time_c_v1[0].time_s%10,WHITE,BLACK);
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off ? showhanzi(315,67,70) : showhanzi(315,67,71);
				
				showhanzi(0,103,79);showhanzi(108,103,18);showhanzi(180,103,40);showhanzi(252,103,69);//2 时分秒
				showdigit_color(72,103,time_c_v1[1].time_h/10,WHITE,BLACK);showdigit_color(88,103,time_c_v1[1].time_h%10,WHITE,BLACK);
				showdigit_color(144,103,time_c_v1[1].time_m/10,WHITE,BLACK);showdigit_color(160,103,time_c_v1[1].time_m%10,WHITE,BLACK);
				showdigit_color(216,103,time_c_v1[1].time_s/10,WHITE,BLACK);showdigit_color(232,103,time_c_v1[1].time_s%10,WHITE,BLACK);
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off ? showhanzi(315,103,70) : showhanzi(315,103,71);
				
				showhanzi(0,139,80);showhanzi(108,139,18);showhanzi(180,139,40);showhanzi(252,139,69);
				showdigit_color(72,139,time_c_v1[2].time_h/10,WHITE,BLACK);showdigit_color(88,139,time_c_v1[2].time_h%10,WHITE,BLACK);
				showdigit_color(144,139,time_c_v1[2].time_m/10,WHITE,BLACK);showdigit_color(160,139,time_c_v1[2].time_m%10,WHITE,BLACK);
				showdigit_color(216,139,time_c_v1[2].time_s/10,WHITE,BLACK);showdigit_color(232,139,time_c_v1[2].time_s%10,WHITE,BLACK);
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off ? showhanzi(315,139,70) : showhanzi(315,139,71);
				
				showhanzi(0,175,81);showhanzi(108,175,18);showhanzi(180,175,40);showhanzi(252,175,69);
				showdigit_color(72,175,time_c_v1[3].time_h/10,WHITE,BLACK);showdigit_color(88,175,time_c_v1[3].time_h%10,WHITE,BLACK);
				showdigit_color(144,175,time_c_v1[3].time_m/10,WHITE,BLACK);showdigit_color(160,175,time_c_v1[3].time_m%10,WHITE,BLACK);
				showdigit_color(216,175,time_c_v1[3].time_s/10,WHITE,BLACK);showdigit_color(232,175,time_c_v1[3].time_s%10,WHITE,BLACK);
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off ? showhanzi(315,175,70) : showhanzi(315,175,71);
				
				showhanzi(0,211,82);showhanzi(108,211,18);showhanzi(180,211,40);showhanzi(252,211,69);
				showdigit_color(72,211,time_c_v1[4].time_h/10,WHITE,BLACK);showdigit_color(88,211,time_c_v1[4].time_h%10,WHITE,BLACK);
				showdigit_color(144,211,time_c_v1[4].time_m/10,WHITE,BLACK);showdigit_color(160,211,time_c_v1[4].time_m%10,WHITE,BLACK);
				showdigit_color(216,211,time_c_v1[4].time_s/10,WHITE,BLACK);showdigit_color(232,211,time_c_v1[4].time_s%10,WHITE,BLACK);
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off ? showhanzi(315,211,70) : showhanzi(315,211,71);

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
	&& relay_structure[last_index_save-12].relay_mode==0){
	    showhanzi(160,35,92);showhanzi(192,35,90);showhanzi(224,35,91);//不使用
	}
	
	//温控界面
	if((enter_config_flag_1 || enter_config_flag_2 ||enter_config_flag_3 || enter_config_flag_4 ||enter_config_flag_5
		|| enter_config_flag_6 ||enter_config_flag_7 || enter_config_flag_8 || enter_config_flag_9 || enter_config_flag_10
		)
	&& relay_structure[last_index_save-12].relay_mode==1){
		u8 max_temp_v = relay_structure[last_index_save-12].temp_control.max_temp;
		u8 min_temp_v = relay_structure[last_index_save-12].temp_control.min_temp;
		u16 max_nh3_v = relay_structure[last_index_save-12].temp_control.max_nh3;
		u16 min_nh3_v = relay_structure[last_index_save-12].temp_control.min_nh3;
		//显示固定界面信息
		showhanzi(160,35,39);showhanzi(192,35,10);showhanzi(224,35,38);//温控
	
		showhanzi(6,67,41);showhanzi(38,67,42);showhanzi(70,67,39);showhanzi(102,67,44);//最高温
		showhanzi(6,103,41);showhanzi(38,103,43);showhanzi(70,103,39); showhanzi(102,103,44);//最低温
		showhanzi(6,139,50);showhanzi(38,139,51);showhanzi(70,139,52); showhanzi(102,139,53);showhanzi(134,139,54);showhanzi(166,139,44);//对应传感器
		showhanzi(6,175,55);showhanzi(38,175,56);showhanzi(70,175,57); showhanzi(102,175,58);showhanzi(134,175,44);//启动方式
		showhanzi(6,211,105);showhanzi(38,211,106);showhanzi(70,211,41); showhanzi(102,211,42);showhanzi(134,211,88);showhanzi(166,211,44);//氨气最高值
		showhanzi(6,247,105);showhanzi(38,247,106);showhanzi(70,247,41); showhanzi(102,247,43);showhanzi(134,247,88);showhanzi(166,247,44);//氨气最低值
		//显示参数信息
		switch(cur_pos1)
		{
			case 1:
			    showdigit_color(134,67,max_temp_v/10,BLACK,WHITE);showdigit_color(150,67,max_temp_v%10,BLACK,WHITE);
			    showdigit_color(134,103,min_temp_v/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(200,211,max_nh3_v/100,WHITE,BLACK);showdigit_color(216,211,(max_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,211,max_nh3_v%10,WHITE,BLACK);
			    showdigit_color(200,247,min_nh3_v/100,WHITE,BLACK);showdigit_color(216,247,(min_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,247,min_nh3_v%10,WHITE,BLACK);
				break;
			case 2:
			    showdigit_color(134,67,max_temp_v/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v%10,WHITE,BLACK);
			    showdigit_color(134,103,min_temp_v/10,BLACK,WHITE);showdigit_color(150,103,min_temp_v%10,BLACK,WHITE);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(200,211,max_nh3_v/100,WHITE,BLACK);showdigit_color(216,211,(max_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,211,max_nh3_v%10,WHITE,BLACK);
			    showdigit_color(200,247,min_nh3_v/100,WHITE,BLACK);showdigit_color(216,247,(min_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,247,min_nh3_v%10,WHITE,BLACK);
				break;
			case 3:
			    showdigit_color(134,67,max_temp_v/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v%10,WHITE,BLACK);
				showdigit_color(134,103,min_temp_v/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,63,1);showhanzi_1(295,139,38,1);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,64,1);showhanzi_1(295,139,38,1);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,65,1);showhanzi_1(295,139,38,1);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi_1(199,139,67,1);showhanzi_1(231,139,68,1);showhanzi_1(263,139,39,1);showhanzi_1(295,139,66,1);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(200,211,max_nh3_v/100,WHITE,BLACK);showdigit_color(216,211,(max_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,211,max_nh3_v%10,WHITE,BLACK);
			    showdigit_color(200,247,min_nh3_v/100,WHITE,BLACK);showdigit_color(216,247,(min_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,247,min_nh3_v%10,WHITE,BLACK);
				break;
			case 4:
			    showdigit_color(134,67,max_temp_v/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v%10,WHITE,BLACK);
				showdigit_color(134,103,min_temp_v/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi_1(167,175,61,1);showhanzi_1(199,175,62,1);}
				else{showhanzi_1(167,175,59,1);showhanzi_1(199,175,60,1);}
				showdigit_color(200,211,max_nh3_v/100,WHITE,BLACK);showdigit_color(216,211,(max_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,211,max_nh3_v%10,WHITE,BLACK);
			    showdigit_color(200,247,min_nh3_v/100,WHITE,BLACK);showdigit_color(216,247,(min_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,247,min_nh3_v%10,WHITE,BLACK);
				break;
			case 5:
			    showdigit_color(134,67,max_temp_v/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v%10,WHITE,BLACK);
				showdigit_color(134,103,min_temp_v/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(200,211,max_nh3_v/100,BLACK,WHITE);showdigit_color(216,211,(max_nh3_v/10)%10,BLACK,WHITE);showdigit_color(232,211,max_nh3_v%10,BLACK,WHITE);
			    showdigit_color(200,247,min_nh3_v/100,WHITE,BLACK);showdigit_color(216,247,(min_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,247,min_nh3_v%10,WHITE,BLACK);
				break;
			case 6:
			    showdigit_color(134,67,max_temp_v/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v%10,WHITE,BLACK);
				showdigit_color(134,103,min_temp_v/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(200,211,max_nh3_v/100,WHITE,BLACK);showdigit_color(216,211,(max_nh3_v/10)%10,WHITE,BLACK);showdigit_color(232,211,max_nh3_v%10,WHITE,BLACK);
			    showdigit_color(200,247,min_nh3_v/100,BLACK,WHITE);showdigit_color(216,247,(min_nh3_v/10)%10,BLACK,WHITE);showdigit_color(232,247,min_nh3_v%10,BLACK,WHITE);
				break;
			default:break;
		}
	}
	//时控界面
	if((enter_config_flag_1 || enter_config_flag_2 ||enter_config_flag_3 || enter_config_flag_4 ||enter_config_flag_5
		|| enter_config_flag_6 ||enter_config_flag_7 || enter_config_flag_8 || enter_config_flag_9 || enter_config_flag_10
		)
	&& relay_structure[last_index_save-12].relay_mode==2){
		uint16_t time_open_v=relay_structure[last_index_save-12].time_control.time_open;
		uint16_t time_stop_v=relay_structure[last_index_save-12].time_control.time_stop;
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
	&& relay_structure[last_index_save-12].relay_mode==3){
		u8 max_temp_v111 = relay_structure[last_index_save-12].temp_control.max_temp;
		u8 min_temp_v111 = relay_structure[last_index_save-12].temp_control.min_temp;
		uint16_t time_open_v111=relay_structure[last_index_save-12].time_control.time_open;
		uint16_t time_stop_v111=relay_structure[last_index_save-12].time_control.time_stop;
		//温时控
		showhanzi(160,35,39);showhanzi(192,35,18);showhanzi(224,35,10);

		showhanzi(6,67,41);showhanzi(38,67,42);showhanzi(70,67,39);showhanzi(102,67,44);//最高温
		showhanzi(6,103,41);showhanzi(38,103,43);showhanzi(70,103,39); showhanzi(102,103,44);//最低温
		showhanzi(6,139,50);showhanzi(38,139,51);showhanzi(70,139,52); showhanzi(102,139,53);showhanzi(134,139,54);showhanzi(166,139,44);//对应传感器
		showhanzi(6,175,55);showhanzi(38,175,56);showhanzi(70,175,57); showhanzi(102,175,58);showhanzi(134,175,44);//启动方式
		
		showhanzi(6,211,48);  showhanzi(38,211,49);  showhanzi(70,211,18);  showhanzi(102,211,19); showhanzi(134,211,44);//打开时间	
		showhanzi(6,247,46);  showhanzi(38,247,47);  showhanzi(70,247,18); showhanzi(102,247,19);showhanzi(134,247,44);//关闭时间
		
		switch(cur_pos3){
			case 1:
				showdigit_color(134,67,max_temp_v111/10,BLACK,WHITE);showdigit_color(150,67,max_temp_v111%10,BLACK,WHITE);
			    showdigit_color(134,103,min_temp_v111/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v111%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				
			    showdigit_color(166,211,time_open_v111/1000,WHITE,BLACK);showdigit_color(182,211,(time_open_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,211,(time_open_v111/10)%10,WHITE,BLACK);showdigit_color(214,211,time_open_v111%10,WHITE,BLACK);
			
				showdigit_color(166,247,time_stop_v111/1000,WHITE,BLACK);showdigit_color(182,247,(time_stop_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,247,(time_stop_v111/10)%10,WHITE,BLACK);showdigit_color(214,247,time_stop_v111%10,WHITE,BLACK);
				
				break;
			case 2:
				showdigit_color(134,67,max_temp_v111/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v111%10,WHITE,BLACK);
			    showdigit_color(134,103,min_temp_v111/10,BLACK,WHITE);showdigit_color(150,103,min_temp_v111%10,BLACK,WHITE);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				
			    showdigit_color(166,211,time_open_v111/1000,WHITE,BLACK);showdigit_color(182,211,(time_open_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,211,(time_open_v111/10)%10,WHITE,BLACK);showdigit_color(214,211,time_open_v111%10,WHITE,BLACK);
			
				showdigit_color(166,247,time_stop_v111/1000,WHITE,BLACK);showdigit_color(182,247,(time_stop_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,247,(time_stop_v111/10)%10,WHITE,BLACK);showdigit_color(214,247,time_stop_v111%10,WHITE,BLACK);
				break;
			case 3:
				showdigit_color(134,67,max_temp_v111/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v111%10,WHITE,BLACK);
				showdigit_color(134,103,min_temp_v111/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v111%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,63,1);showhanzi_1(295,139,38,1);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,64,1);showhanzi_1(295,139,38,1);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi_1(199,139,39,1);showhanzi_1(231,139,66,1);showhanzi_1(263,139,65,1);showhanzi_1(295,139,38,1);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi_1(199,139,67,1);showhanzi_1(231,139,68,1);showhanzi_1(263,139,39,1);showhanzi_1(295,139,66,1);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				
			    showdigit_color(166,211,time_open_v111/1000,WHITE,BLACK);showdigit_color(182,211,(time_open_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,211,(time_open_v111/10)%10,WHITE,BLACK);showdigit_color(214,211,time_open_v111%10,WHITE,BLACK);
			
				showdigit_color(166,247,time_stop_v111/1000,WHITE,BLACK);showdigit_color(182,247,(time_stop_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,247,(time_stop_v111/10)%10,WHITE,BLACK);showdigit_color(214,247,time_stop_v111%10,WHITE,BLACK);
				break;
			case 4:
				showdigit_color(134,67,max_temp_v111/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v111%10,WHITE,BLACK);
				showdigit_color(134,103,min_temp_v111/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v111%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi_1(167,175,61,1);showhanzi_1(199,175,62,1);}//加热
				else{showhanzi_1(167,175,59,1);showhanzi_1(199,175,60,1);}//制冷
				showdigit_color(166,211,time_open_v111/1000,WHITE,BLACK);showdigit_color(182,211,(time_open_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,211,(time_open_v111/10)%10,WHITE,BLACK);showdigit_color(214,211,time_open_v111%10,WHITE,BLACK);
			
				showdigit_color(166,247,time_stop_v111/1000,WHITE,BLACK);showdigit_color(182,247,(time_stop_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,247,(time_stop_v111/10)%10,WHITE,BLACK);showdigit_color(214,247,time_stop_v111%10,WHITE,BLACK);
				
				break;
			case 5:
				showdigit_color(134,67,max_temp_v111/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v111%10,WHITE,BLACK);
				showdigit_color(134,103,min_temp_v111/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v111%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(166,211,time_open_v111/1000,BLACK,WHITE);showdigit_color(182,211,(time_open_v111/100)%10,BLACK,WHITE);
			    showdigit_color(198,211,(time_open_v111/10)%10,BLACK,WHITE);showdigit_color(214,211,time_open_v111%10,BLACK,WHITE);
			
				showdigit_color(166,247,time_stop_v111/1000,WHITE,BLACK);showdigit_color(182,247,(time_stop_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,247,(time_stop_v111/10)%10,WHITE,BLACK);showdigit_color(214,247,time_stop_v111%10,WHITE,BLACK);
				
				break;
			case 6:
				showdigit_color(134,67,max_temp_v111/10,WHITE,BLACK);showdigit_color(150,67,max_temp_v111%10,WHITE,BLACK);
				showdigit_color(134,103,min_temp_v111/10,WHITE,BLACK);showdigit_color(150,103,min_temp_v111%10,WHITE,BLACK);
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 0) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,63);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 1) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,64);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 2) {showhanzi(199,139,39);showhanzi(231,139,66);showhanzi(263,139,65);showhanzi(295,139,38);}
				if(relay_structure[last_index_save-12].temp_control.temp_choose_flag == 3) {showhanzi(199,139,67);showhanzi(231,139,68);showhanzi(263,139,39);showhanzi(295,139,66);}//平均温度
				if(relay_structure[last_index_save-12].temp_control.startup_mode){
					showhanzi(167,175,61);showhanzi(199,175,62);}//加热
				else{showhanzi(167,175,59);showhanzi(199,175,60);}//制冷
				showdigit_color(166,211,time_open_v111/1000,WHITE,BLACK);showdigit_color(182,211,(time_open_v111/100)%10,WHITE,BLACK);
			    showdigit_color(198,211,(time_open_v111/10)%10,WHITE,BLACK);showdigit_color(214,211,time_open_v111%10,WHITE,BLACK);
			
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
	&& relay_structure[last_index_save-12].relay_mode==4){
	
		Time_C time_c_v[5]={0};
		time_c_v[0].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.hour;
		time_c_v[0].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.minutes;
		time_c_v[0].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.seconds;
		time_c_v[1].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.hour;
		time_c_v[1].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.minutes;
		time_c_v[1].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.seconds;
		time_c_v[2].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.hour;
		time_c_v[2].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.minutes;
		time_c_v[2].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.seconds;
		time_c_v[3].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.hour;
		time_c_v[3].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.minutes;
		time_c_v[3].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.seconds;
		time_c_v[4].time_h = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.hour;
		time_c_v[4].time_m = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.minutes;
		time_c_v[4].time_s = relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.seconds;

		//分时控
		showhanzi(160,35,40);showhanzi(192,35,18);showhanzi(224,35,10);
		
		showhanzi(0,67,78);showhanzi(108,67,18);showhanzi(180,67,40);showhanzi(252,67,69);//1 时分秒
		showdigit_color(72,67,time_c_v[0].time_h/10,WHITE,BLACK);showdigit_color(88,67,time_c_v[0].time_h%10,WHITE,BLACK);
		showdigit_color(144,67,time_c_v[0].time_m/10,WHITE,BLACK);showdigit_color(160,67,time_c_v[0].time_m%10,WHITE,BLACK);
		showdigit_color(216,67,time_c_v[0].time_s/10,WHITE,BLACK);showdigit_color(232,67,time_c_v[0].time_s%10,WHITE,BLACK);
		relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off ? showhanzi(315,67,70) : showhanzi(315,67,71);//开 /关
		
		
		showhanzi(0,103,79);showhanzi(108,103,18);showhanzi(180,103,40);showhanzi(252,103,69);//2 时分秒
		showdigit_color(72,103,time_c_v[1].time_h/10,WHITE,BLACK);showdigit_color(88,103,time_c_v[1].time_h%10,WHITE,BLACK);
		showdigit_color(144,103,time_c_v[1].time_m/10,WHITE,BLACK);showdigit_color(160,103,time_c_v[1].time_m%10,WHITE,BLACK);
		showdigit_color(216,103,time_c_v[1].time_s/10,WHITE,BLACK);showdigit_color(232,103,time_c_v[1].time_s%10,WHITE,BLACK);
		relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off ? showhanzi(315,103,70) : showhanzi(315,103,71);
		
		showhanzi(0,139,80);showhanzi(108,139,18);showhanzi(180,139,40);showhanzi(252,139,69);//1 时分秒
		showdigit_color(72,139,time_c_v[2].time_h/10,WHITE,BLACK);showdigit_color(88,139,time_c_v[2].time_h%10,WHITE,BLACK);
		showdigit_color(144,139,time_c_v[2].time_m/10,WHITE,BLACK);showdigit_color(160,139,time_c_v[2].time_m%10,WHITE,BLACK);
		showdigit_color(216,139,time_c_v[2].time_s/10,WHITE,BLACK);showdigit_color(232,139,time_c_v[2].time_s%10,WHITE,BLACK);
		relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off ? showhanzi(315,139,70) : showhanzi(315,139,71);
		
		showhanzi(0,175,81);showhanzi(108,175,18);showhanzi(180,175,40);showhanzi(252,175,69);//1 时分秒
		showdigit_color(72,175,time_c_v[3].time_h/10,WHITE,BLACK);showdigit_color(88,175,time_c_v[3].time_h%10,WHITE,BLACK);
		showdigit_color(144,175,time_c_v[3].time_m/10,WHITE,BLACK);showdigit_color(160,175,time_c_v[3].time_m%10,WHITE,BLACK);
		showdigit_color(216,175,time_c_v[3].time_s/10,WHITE,BLACK);showdigit_color(232,175,time_c_v[3].time_s%10,WHITE,BLACK);
		relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off ? showhanzi(315,175,70) : showhanzi(315,175,71);
		
		showhanzi(0,211,82);showhanzi(108,211,18);showhanzi(180,211,40);showhanzi(252,211,69);//1 时分秒
		showdigit_color(72,211,time_c_v[4].time_h/10,WHITE,BLACK);showdigit_color(88,211,time_c_v[4].time_h%10,WHITE,BLACK);
		showdigit_color(144,211,time_c_v[4].time_m/10,WHITE,BLACK);showdigit_color(160,211,time_c_v[4].time_m%10,WHITE,BLACK);
		showdigit_color(216,211,time_c_v[4].time_s/10,WHITE,BLACK);showdigit_color(232,211,time_c_v[4].time_s%10,WHITE,BLACK);
		relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off ? showhanzi(315,211,70) : showhanzi(315,211,71);
		
		switch(cur_pos4){
			case 1:
				showdigit_color(72,67,time_c_v[0].time_h/10,BLACK,WHITE);showdigit_color(88,67,time_c_v[0].time_h%10,BLACK,WHITE);
				break;
			case 2:
				showdigit_color(144,67,time_c_v[0].time_m/10,BLACK,WHITE);showdigit_color(160,67,time_c_v[0].time_m%10,BLACK,WHITE);
				break;
			case 3:
				showdigit_color(216,67,time_c_v[0].time_s/10,BLACK,WHITE);showdigit_color(232,67,time_c_v[0].time_s%10,BLACK,WHITE);
				break;
			case 4:
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time1.on_off ? showhanzi_1(315,67,70,1) : showhanzi_1(315,67,71,1);
				break;
			case 5:
				showdigit_color(72,103,time_c_v[1].time_h/10,BLACK,WHITE);showdigit_color(88,103,time_c_v[1].time_h%10,BLACK,WHITE);
				break;
			case 6:
				showdigit_color(144,103,time_c_v[1].time_m/10,BLACK,WHITE);showdigit_color(160,103,time_c_v[1].time_m%10,BLACK,WHITE);
				break;
			case 7:
				showdigit_color(216,103,time_c_v[1].time_s/10,BLACK,WHITE);showdigit_color(232,103,time_c_v[1].time_s%10,BLACK,WHITE);
				break;
			case 8:
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time2.on_off ? showhanzi_1(315,103,70,1) : showhanzi_1(315,103,71,1);
				break;
			case 9:
				showdigit_color(72,139,time_c_v[2].time_h/10,BLACK,WHITE);showdigit_color(88,139,time_c_v[2].time_h%10,BLACK,WHITE);
				break;
			case 10:
				showdigit_color(144,139,time_c_v[2].time_m/10,BLACK,WHITE);showdigit_color(160,139,time_c_v[2].time_m%10,BLACK,WHITE);
				break;
			case 11:
				showdigit_color(216,139,time_c_v[2].time_s/10,BLACK,WHITE);showdigit_color(232,139,time_c_v[2].time_s%10,BLACK,WHITE);
				break;
			case 12:
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time3.on_off ? showhanzi_1(315,139,70,1) : showhanzi_1(315,139,71,1);
				break;
			case 13:
				showdigit_color(72,175,time_c_v[3].time_h/10,BLACK,WHITE);showdigit_color(88,175,time_c_v[3].time_h%10,BLACK,WHITE);
				break;
			case 14:
				showdigit_color(144,175,time_c_v[3].time_m/10,BLACK,WHITE);showdigit_color(160,175,time_c_v[3].time_m%10,BLACK,WHITE);
				break;
			case 15:
				showdigit_color(216,175,time_c_v[3].time_s/10,BLACK,WHITE);showdigit_color(232,175,time_c_v[3].time_s%10,BLACK,WHITE);
				break;
			case 16:
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time4.on_off ? showhanzi_1(315,175,70,1) : showhanzi_1(315,175,71,1);
				break;
			case 17:
				showdigit_color(72,211,time_c_v[4].time_h/10,BLACK,WHITE);showdigit_color(88,211,time_c_v[4].time_h%10,BLACK,WHITE);
				break;
			case 18:
				showdigit_color(144,211,time_c_v[4].time_m/10,BLACK,WHITE);showdigit_color(160,211,time_c_v[4].time_m%10,BLACK,WHITE);
				break;
			case 19:
				showdigit_color(216,211,time_c_v[4].time_s/10,BLACK,WHITE);showdigit_color(232,211,time_c_v[4].time_s%10,BLACK,WHITE);
				break;
			case 20:
				relay_structure[last_index_save-12].time_schedule.relay_time_seg.Time5.on_off ? showhanzi_1(315,211,70,1) : showhanzi_1(315,211,71,1);
				break;
		
			default:break;
		}
	}
	send_MQTT_Flag = last_index_save;
	
	
}





	

