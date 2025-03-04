#include "delay.h"
#include "sys.h"
#include "usart1.h"
#include "rtc.h"
#include "global.h"
#include "74HC595.h"
#include "ntc.h"
#include "timer.h"

#include "lcd.h"
#include "gui.h"
#include "UI.h"
#include "flash.h"

#include "string.h"
#include "GP8201S.h"
#include <jansson.h>

#include "mb_host.h"

#include "mb_hook.h"


#define SIZE_OF_RELAY_STRUCTURE (10 * sizeof(Relay_Structure)) // 计算数组大小
	
#define REMOVE_memset 0  //去除发布消息后清零串口接收缓冲区
#define REMOVE_PUBLISH_memset 0  //去除发布消息后清零串口接收缓冲区

uint8_t network_flag=0;
uint8_t register_flag=0;
uint8_t mqtt_flag=0;

char MQTT_RxDataBuf_nochange[RXBUFF_SIZE];

volatile u8 relay_Control[2] = {0x00, 0x00}; //控制继电器
volatile u8 relay1_Control_1[2] = {0x00, 0x00};

char flash_data[9]={0x00};
//static char send_data_config[350]="";
//uint8_t send_index_t;

volatile uint8_t addr;
volatile uint8_t addr_flag;

//volatile char userData[330];

char time_str[20]="";

//报警间隔发送给服务器的计数器
uint64_t warn_timer_count=0;

u8 is_all_ff(uint8_t *buffer, uint16_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (buffer[i] != 0xFF) {
            return 0;  // 发现一个不是 0xFF 的元素，返回 false
        }
    }
    return 1;  // 如果所有元素都是 0xFF，返回 true
}

int main(void)
{	
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 						 
		//=============================================================================
		//LED -> PA8
		//=============================================================================			 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	  	/* 日历结构体 */
	    //_calendar_obj calendar_temp;
	    //中断优先级分组 分2组
	  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  
		delay_init();	    	 //延时函数初始化	 
		//USART1_Init(115200);
		UART3_Init();
	    /* RTC初始化 */
        RTC_Init();
		/* NTC初始化 */		
		ADC_Configuration();
		TIM1_Int_Init(0,0);
	    /* TIM3初始化 */	
		TIM3_Int_Init(0,0);	
	    /* TIM2初始化 */	
		TIM2_Int_Init(0,0);	
		
		
		LCD_Init();	   //液晶屏初始化
		LCD_Clear(HOME_BACK);
		//LCD_Fill(0,0,lcddev.width,lcddev.height,HOME_BACK);
		
		/* GP8201S初始化 */
		IIC_Init();
		GP8201S_Write(0,0,0x00,0x00);//上电输出0V
		
        /* 74HC595初始化 */
		HC595_GPIO_Config();
        HC595_Send_Multi_Byte(relay1_Control_1, 2);
		
		/* 485初始化 */
		mbh_init(4800,0);
		uint8_t relay_structure_buffer[SIZE_OF_RELAY_STRUCTURE];
		
		//读取flash固定位置的数据
//		ReadFlashData(0, relay_structure_buffer, SIZE_OF_RELAY_STRUCTURE);
//		if(is_all_ff(relay_structure_buffer, sizeof(relay_structure_buffer))) {
//			// 如果数组中的所有元素都是 0xFF
//			printf("is all 0Xff\n");
//		}else{
//			// 如果数组中有元素不是 0xFF
//			Relay_Structure * tt = (Relay_Structure *)relay_structure_buffer;
//			memcpy(relay_structure, relay_structure_buffer, SIZE_OF_RELAY_STRUCTURE);
//		}


		
	while(1) 
	{	
		u8 warn_flag1=0;
		u8 warn_flag2=0;
		u8 warn_flag3=0;
		u8 warn_485temp_flag=0;
		u8 warn_485rh_flag=0;

		u8 warn_flag1_high = 0;
		u8 warn_flag1_low = 0;
		u8 warn_flag2_high = 0;
		u8 warn_flag2_low = 0;
		u8 warn_flag3_high = 0;
		u8 warn_flag3_low = 0;
		
		u8 warn_485temp_flag_high = 0;
		u8 warn_485temp_flag_low = 0;
		u8 warn_485rh_flag_high = 0;
		u8 warn_485rh_flag_low = 0;
		
		u8 warn_NH3=0;
		
//		if(warn_temp1_flag && (temperature1>0.0) && (temperature1<90.0)){
//			if(temperature1>limit_temp1_maxvalue || temperature1<limit_temp1_minvalue)warn_flag1=1;else{warn_flag1=0;warn_flag1_high=0;warn_flag1_low=0;}
//			if(temperature1>limit_temp1_maxvalue)warn_flag1_high=1;else warn_flag1_high=0;
//			if(temperature1<limit_temp1_minvalue)warn_flag1_low=1;else warn_flag1_low=0;
//		}
//		if(warn_temp2_flag && (temperature2>0.0) && (temperature2<90.0)){
//			if(temperature2>limit_temp2_maxvalue || temperature2<limit_temp2_minvalue)warn_flag2=1;else{warn_flag2=0;warn_flag2_high=0;warn_flag2_low=0;}
//			if(temperature2>limit_temp2_maxvalue)warn_flag2_high=1;else warn_flag2_high=0;
//			if(temperature2<limit_temp2_minvalue)warn_flag2_low=1;else warn_flag2_low=0;
//		}
//		if(warn_temp3_flag && (temperature3>0.0) && (temperature3<90.0)){
//			if(temperature3>limit_temp3_maxvalue || temperature3<limit_temp3_minvalue)warn_flag3=1;else{warn_flag3=0;warn_flag3_high=0;warn_flag3_low=0;}
//			if(temperature3>limit_temp3_maxvalue)warn_flag3_high=1;else warn_flag3_high=0;
//			if(temperature3<limit_temp3_minvalue)warn_flag3_low=1;else warn_flag3_low=0;
//		}
//		if(warn_485temp_flag && (send_TEMP>0) && (send_TEMP<900)){
//			if((send_TEMP/10)>limit_temp485_maxvalue || (send_TEMP/10)<limit_temp485_minvalue)warn_485temp_flag=1;else{warn_485temp_flag=0;warn_485temp_flag_high=0;warn_485temp_flag_low=0;}
//			if((send_TEMP/10)>limit_temp485_maxvalue)warn_485temp_flag_high=1;else warn_485temp_flag_high=0;
//			if((send_TEMP/10)<limit_temp485_minvalue)warn_485temp_flag_low=1;else warn_485temp_flag_low=0;
//		}
//		if(warn_485rh_flag && (send_RH>0) && (send_RH<900)){
//			if((send_RH/10)>limit_rh_maxvalue || (send_RH/10)<limit_rh_minvalue)warn_485rh_flag=1;else{warn_485rh_flag=0;warn_485rh_flag_high=0;warn_485rh_flag_low=0;}
//			if((send_RH/10)>limit_rh_maxvalue)warn_485rh_flag_high=1;else warn_485rh_flag_high=0;
//			if((send_RH/10)<limit_rh_minvalue)warn_485rh_flag_low=1;else warn_485rh_flag_low=0;
//		}
//		
//		if(warn_temp1_flag==0 || temperature1>90){warn_flag1=0;warn_flag1_high=0;warn_flag1_low=0;}
//		if(warn_temp2_flag==0 || temperature2>90){warn_flag2=0;warn_flag2_high=0;warn_flag2_low=0;}
//		if(warn_temp3_flag==0 || temperature3>90){warn_flag3=0;warn_flag3_high=0;warn_flag3_low=0;}
//		if(warn_485temp_flag==0 || send_TEMP>900){warn_485temp_flag=0;warn_485temp_flag_high=0;warn_485temp_flag_low=0;}
//		if(warn_485rh_flag==0 || send_RH>999){warn_485rh_flag=0;warn_485rh_flag_high=0;warn_485rh_flag_low=0;}
//		
//		if(NH3_warn_flag){
//			if(send_NH3!=999 && send_NH3>=NH3_max && send_NH3<100)warn_NH3=1;
//			if(send_NH3==999 || send_NH3<NH3_max)warn_NH3=0;
//		}else{
//			warn_NH3=0;
//		}
//		if(send_NH3>900)warn_NH3=0;
//		//printf("warn_NH3:%d\n",warn_NH3);




		
		if(warn_temp1_flag && (temperature1>0.0) && (temperature1<90.0)){
			if(temperature1>limit_temp1_maxvalue || temperature1<limit_temp1_minvalue){
				warn_flag1=1;
				if(temperature1>limit_temp1_maxvalue)warn_flag1_high=1;
				if(temperature1<limit_temp1_minvalue)warn_flag1_low=1;
			}else{warn_flag1=0;warn_flag1_high=0;warn_flag1_low=0;}
		}
		if(warn_temp2_flag && (temperature2>0.0) && (temperature2<90.0)){
			if(temperature2>limit_temp2_maxvalue || temperature2<limit_temp2_minvalue){
				warn_flag2=1;
				if(temperature2>limit_temp2_maxvalue)warn_flag2_high=1;
				if(temperature2<limit_temp2_minvalue)warn_flag2_low=1;
			}else{warn_flag2=0;warn_flag2_high=0;warn_flag2_low=0;}
		}
		if(warn_temp3_flag && (temperature3>0.0) && (temperature3<90.0)){
			if(temperature3>limit_temp3_maxvalue || temperature3<limit_temp3_minvalue){
				warn_flag3=1;
				if(temperature3>limit_temp3_maxvalue)warn_flag3_high=1;
				if(temperature3<limit_temp3_minvalue)warn_flag3_low=1;
			}else{warn_flag3=0;warn_flag3_high=0;warn_flag3_low=0;}
		}
		if(warn_temp485_flag && (send_TEMP>0) && (send_TEMP<900)){
			if((send_TEMP/10)>limit_temp485_maxvalue || (send_TEMP/10)<limit_temp485_minvalue){
				warn_485temp_flag=1;
				if((send_TEMP/10)>limit_temp485_maxvalue)warn_485temp_flag_high=1;
				if((send_TEMP/10)<limit_temp485_minvalue)warn_485temp_flag_low=1;
			}else{warn_485temp_flag=0;warn_485temp_flag_high=0;warn_485temp_flag_low=0;}
		}
		if(warn_rh_flag && (send_RH>0) && (send_RH<900)){
			if((send_RH/10)>limit_rh_maxvalue || (send_RH/10)<limit_rh_minvalue){
				warn_485rh_flag=1;
				if((send_RH/10)>limit_rh_maxvalue)warn_485rh_flag_high=1;
				if((send_RH/10)<limit_rh_minvalue)warn_485rh_flag_low=1;
			}else{warn_485rh_flag=0;warn_485rh_flag_high=0;warn_485rh_flag_low=0;}
		}

		if(NH3_warn_flag && send_NH3<900){
			if(send_NH3>=NH3_max)warn_NH3=1;else warn_NH3=0;
		}else{
			warn_NH3=0;
		}
		
		if(warn_temp1_flag==0 || temperature1>90){warn_flag1=0;warn_flag1_high=0;warn_flag1_low=0;}
		if(warn_temp2_flag==0 || temperature2>90){warn_flag2=0;warn_flag2_high=0;warn_flag2_low=0;}
		if(warn_temp3_flag==0 || temperature3>90){warn_flag3=0;warn_flag3_high=0;warn_flag3_low=0;}
		if(warn_temp485_flag==0 || send_TEMP>900){warn_485temp_flag=0;warn_485temp_flag_high=0;warn_485temp_flag_low=0;}
		if(warn_rh_flag==0 || send_RH>999){warn_485rh_flag=0;warn_485rh_flag_high=0;warn_485rh_flag_low=0;}
		if(NH3_warn_flag==0 || send_NH3>900)warn_NH3=0;
		
		
		warn_flag = warn_flag1 || warn_flag2 || warn_flag3 || warn_485temp_flag || warn_485rh_flag || warn_NH3;
		//printf("warn_flag:%d\n",warn_flag);
		//showdigit_color(150,210,warn_flag,WHITE,HOME_BACK);
		
		//顺序开启继电器
//		u8 temp;
//		temp = relay_Control[1] & 0X3f | (relay_structure[0].on_off << 7) | (relay_structure[1].on_off << 6);
//		if(relay_Control[1]!=temp){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);delay_ms(50);}
//		
//		temp = relay_Control[1] & 0Xcf | (relay_structure[2].on_off << 5) | (relay_structure[3].on_off << 4);
//		if(relay_Control[1]!=temp){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);delay_ms(50);}
//		
//		temp = relay_Control[1] & 0Xf3 | (relay_structure[4].on_off << 3) | (relay_structure[5].on_off << 2);
//		if(relay_Control[1]!=temp){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);delay_ms(50);}
//		
//		temp = relay_Control[1] & 0Xfd | (relay_structure[6].on_off << 1);
//		if(relay_Control[1]!=temp){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);delay_ms(50);}
//		
//		temp = relay_Control[0] & 0X3f | (relay_structure[7].on_off << 7) | (relay_structure[8].on_off << 6);
//		if(relay_Control[0]!=temp){relay_Control[0]=temp;HC595_Send_Multi_Byte(relay_Control, 2);delay_ms(50);}
//		
//		temp = relay_Control[0] & 0Xdf | (relay_structure[9].on_off << 5) | (warn_flag << 4);
//		if(relay_Control[0]!=temp){relay_Control[0]=temp;HC595_Send_Multi_Byte(relay_Control, 2);delay_ms(50);}
		
		//2-3秒间隔顺序开启,问题有待解决（if判断语句里边是两个风机的开关状态）
		
		
		u8 temp;
		temp = relay_Control[1] & 0X3f | (relay_structure[0].on_off << 7) | (relay_structure[1].on_off << 6);
		if(relay_Control[1]!=temp && TIM3_flag){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);goto next;}
		
		temp = relay_Control[1] & 0Xcf | (relay_structure[2].on_off << 5) | (relay_structure[3].on_off << 4);
		if(relay_Control[1]!=temp && TIM3_flag){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);goto next;}
		
		temp = relay_Control[1] & 0Xf3 | (relay_structure[4].on_off << 3) | (relay_structure[5].on_off << 2);
		if(relay_Control[1]!=temp && TIM3_flag){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);goto next;}
		
		temp = relay_Control[1] & 0Xfd | (relay_structure[6].on_off << 1);
		if(relay_Control[1]!=temp && TIM3_flag){relay_Control[1]=temp;HC595_Send_Multi_Byte(relay_Control, 2);goto next;}
		
		temp = relay_Control[0] & 0X3f | (relay_structure[7].on_off << 7) | (relay_structure[8].on_off << 6);
		if(relay_Control[0]!=temp && TIM3_flag){relay_Control[0]=temp;HC595_Send_Multi_Byte(relay_Control, 2);goto next;}
		
		temp = relay_Control[0] & 0Xcf | (relay_structure[9].on_off << 5) | (warn_flag << 4);
		if(relay_Control[0]!=temp && TIM3_flag){relay_Control[0]=temp;HC595_Send_Multi_Byte(relay_Control, 2);goto next;}



	//next:	
		//485代码
//		addr_flag++;
//		if(addr_flag==1){
//			uint8_t c1[4] = {0x00,0x00,0x00,0x03};
//			mbh_send(5,0x03,c1,4);
//		}
//		if(addr_flag == 31){
//			uint8_t c2[4] = {0x00,0x00,0x00,0x02};
//			mbh_send(1,0x03,c2,4);
//		}
//		if(addr_flag == 61){
//			uint8_t c3[4] = {0x00,0x00,0x00,0x02};
//			mbh_send(2,0x03,c3,4);
//		}
//		if(addr_flag == 80)addr_flag = 0;
		uint8_t c[4] = {0x00,0x00,0x00,0x03};
		mbh_send(1,0x03,c,4);
		mbh_poll();
		//printf("pframe[4]:%x\n",temp1);
		
		if(mqtt_flag && network_flag){
//			UART3_Puts("AT+MQTTREAD=0?\r\n"); 
//			delay_ms(100);
			
			
			//LCD_ShowString(100,200,16,"mq",0);
			//判断是否注册，根据协议判断服务器发过来的消息和需要改变的变量
		//	if(UART3_RxCounter != 0) {
//					char im1[40];
//					char *start1 = strstr(UART3_RxBuff, "publish");
//					if (start1 != NULL) {
//						strncpy(im1, start1, 39);
//						im1[39] = '\0'; // 确保字符串以 '\0' 结束
//						//LCD_ShowString(0,220,16,im,0);
//					}
				    //LCD_ShowString(120,200,16,im1,0);
				//判断服务器发过来的信息
				//char MQTT_RxDataBuf_nochange[RXBUFF_SIZE]; 
			
				memcpy(MQTT_RxDataBuf_nochange, MQTT_RxDataBuf, RXBUFF_SIZE);//此处复制肯定是一个没有错误且完整的能被正确解析的指令
				memset(MQTT_RxDataBuf, 0, RXBUFF_SIZE); // 一级缓冲区是否清零？？？？？？？？？？？？？？？？必须清零，不行零的话会一直进入下边的if语句里边
			
				if(strstr(MQTT_RxDataBuf_nochange,"publish")!=NULL){
//					UART3_RxCounter = 0; //重新等待接收下一个推送消息
//				    memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0		//			   
					//delay_ms(10);
				   	char *str1 = NULL;							
					char *str2 = NULL;			
					char *pcBegin = NULL;
					char *pcEnd = NULL;
					char userData[350]={0};
					memset(userData, 0, sizeof(userData));//清空userData,不然会出现字符串叠加;或者设置成局部变量char userData[150]={0};
					//指针操作的对象最好不被别的程序同时操作
					str1=(char *)MQTT_RxDataBuf_nochange;
					str2=strstr(str1,"data=");
					if(str2!=NULL){
						pcBegin=strstr(str2,"data=");
						pcEnd=strstr(str2,"end");
						//LCD_ShowString(0,42,16,pcBegin+336,0);
						pcBegin+=5;
						if((pcBegin != NULL) && (pcEnd != NULL))		//开始和结束都有对应的字符
						{
							memcpy(userData, pcBegin, (pcEnd-pcBegin));	//从pcBegin地址开始，复制出(pcEnd-pcBegin)个数据
						}	
					}


					//LCD_ShowString(125,216,16,(char*)userData,0);
					
//				   u8 rev_len = 0;
//				   char rev_data[3];
////				   char *dd = "+MQTTURC: \"publish\",0,2,\"world\",5,5,12345";
//				   char *token = strtok(UART3_RxBuff,",");
//				   
//				   u8 i = 0;
//				   while (token != NULL) { // 遍历每个键值对
//						
//						if (strlen(token) > 0 && i==5) { // 确保token不是空字符串
//							if (sscanf(token, "%d", &rev_len) == 1) { // 解析形如 "n":"xx.xx&xx.xx" 的字符串，提取两个浮点数
//								// 这里我们假设数值是以 "xx.xx&xx.xx" 的格式出现的，可以根据实际情况调整 sscanf 的格式字符串
//								//printf("提取到的数字是：%d\n", rev_len); 	
//								char str[5]; // 假设足够大以容纳整数的字符串表示
//                                sprintf(str, "%d", rev_len); // 将整数转换为字符串
//								//LCD_ShowString(125,216,16,str,0);
//							}
//						}
//						if(strlen(token) > 0 && i==6){
//							if(sscanf(token, "%s", rev_data) == 1){
//								//printf("提取到的数字是：%s\n", rev_data);
//								//LCD_ShowString(125,216,16,rev_data,0);
//							}
//						}
//						i++;
//						token = strtok(NULL, ","); // 继续处理下一个键值对
//					}
//					i=0;
					//解析字符串
					json_error_t error;
					//json_t *root;
//					json_t *temp_ctrl;
//					json_t *time_ctrl;
//					json_t *temp_time_ctrl;
//					json_t *time_seg_ctrl;
//					json_t *timer1;
//					json_t *timer2;
//					json_t *timer3;
//					json_t *timer4;
//					json_t *timer5;
//					json_t *hh;
//					json_t *mm;
//					json_t *ss;
//					json_t *on_off;
					
               //showdigit_color(336,132,1,BLUE,BLACK);
			  // const char *text = "{\"success\":2}";
			  //char text[100]="{\"no\":5,\"mode\":2}";
			 // char *text = "{\"no\":5,\"mode\":2}";
//			        LCD_Fill(0,204,480,217,HOME_BACK);
//			        LCD_ShowString(0,204,12,(char*)userData,0);
					json_t *root = json_loads((const char*)userData, 0, &error); 
					if(json_is_object(root)){
						json_t *no_j = json_object_get(root, "no");
//						if (!json_is_integer(no_j)) {
//							json_decref(root);
//						}
						
						//showdigit_color(360,132,no,BLUE,BLACK);
						
						json_t *mode_j = json_object_get(root, "mode");
//						if (!json_is_integer(mode_j)) {
//							json_decref(root);
//						}
						

						//showdigit_color(0,200,3,BLUE,BLACK);
						//LCD_ShowString(0,216,16,"time_ctrl",0);
						//showdigit_color(380,132,3,BLUE,BLACK);
						
//						json_t *out_voltage_j = json_object_get(root, "out_voltage");
//						if(out_voltage_j){
//							out_voltage = json_integer_value(out_voltage_j);
//							if(out_voltage>=0 && out_voltage<=10){
//								u16 data_v = out_voltage*4095/10;
//								u8 v_low = data_v<<4 | 0x00;
//								u8 v_high = data_v>>4 | 0x00;
//								GP8201S_Write(0,0,v_low,v_high);
//							}
//						}

						if(json_is_integer(mode_j) && json_is_integer(no_j)){
							uint8_t no = json_integer_value(no_j)-1;
							uint8_t mode = json_integer_value(mode_j);
//							relay_structure[no].relay_mode = mode;
						
							switch(mode){
								case 0:
									relay_structure[no].relay_mode = 0;
									UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"M0OK\"\r\n",imei_no);//发布消息
									#if(REMOVE_memset)
									delay_ms(30);
									UART3_RxCounter = 0; //重新等待接收下一个推送消息
									memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
									#endif
									break;
								case 1:	
									json_t *temp_ctrl = json_object_get(root, "temp_ctrl");
									if(json_is_object(temp_ctrl)){
										
										json_t *max_temp = json_object_get(temp_ctrl, "max_temp");
										json_t *min_temp = json_object_get(temp_ctrl, "min_temp");
										json_t *temp_choose_flag = json_object_get(temp_ctrl, "temp_choose_flag");
										json_t *startup_mode = json_object_get(temp_ctrl, "startup_mode");
										json_t *max_nh3 = json_object_get(temp_ctrl, "max_nh3");
										json_t *min_nh3 = json_object_get(temp_ctrl, "min_nh3");
										//showdigit_color(0,90,8,BLUE,BLACK);
										if(json_is_integer(max_temp) && json_is_integer(min_temp) && json_is_integer(temp_choose_flag) && json_is_integer(startup_mode) &&
										    json_is_integer(max_nh3) && json_is_integer(min_nh3)
										){
											relay_structure[no].temp_control.max_temp = json_integer_value(max_temp);
											//showdigit_color(0,110,relay_structure[no].temp_control.max_temp%10,WHITE,HOME_BACK);
											relay_structure[no].temp_control.min_temp = json_integer_value(min_temp);
											relay_structure[no].temp_control.temp_choose_flag = json_integer_value(temp_choose_flag);
											relay_structure[no].temp_control.startup_mode = json_integer_value(startup_mode);
											relay_structure[no].temp_control.max_nh3 = json_integer_value(max_nh3);
											relay_structure[no].temp_control.min_nh3 = json_integer_value(min_nh3);
											relay_structure[no].relay_mode = 1;
											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"M1OK\"\r\n",imei_no);//发布消息
											#if(REMOVE_memset)
											delay_ms(30);
											UART3_RxCounter = 0; //重新等待接收下一个推送消息
											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
											#endif
										}else{
											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,16,\"M1V Invalid data\"\r\n",imei_no);//发布消息
											#if(REMOVE_memset)
											delay_ms(30);
											UART3_RxCounter = 0; //重新等待接收下一个推送消息
											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
											#endif
										}

									}else{
										UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,15,\"M1 Invalid data\"\r\n",imei_no);//发布消息
										#if(REMOVE_memset)
										delay_ms(30);
										UART3_RxCounter = 0; //重新等待接收下一个推送消息
										memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
										#endif
									}
									
																		
//									temp_ctrl = json_object_get(root, "temp_ctrl");
//									const char *temp_ctrl_text = json_string_value(temp_ctrl);
//									json_t *temp_ctrl_j  = json_loads(temp_ctrl_text, 0, &error);	
//									if(json_is_object(temp_ctrl_j)){
//										//showdigit_color(0,90,8,BLUE,BLACK);
//										if(json_object_get(temp_ctrl_j, "max_temp") &&
//											json_object_get(temp_ctrl_j, "min_temp") &&
//										    json_object_get(temp_ctrl_j, "temp_choose_flag") &&
//										    json_object_get(temp_ctrl_j, "startup_mode") &&
//										    json_object_get(temp_ctrl_j, "max_nh3") &&
//											json_object_get(temp_ctrl_j, "min_nh3")
//										){
//											relay_structure[no].temp_control.max_temp = json_integer_value(json_object_get(temp_ctrl_j, "max_temp"));
//											//showdigit_color(0,110,relay_structure[no].temp_control.max_temp%10,WHITE,HOME_BACK);
//											relay_structure[no].temp_control.min_temp = json_integer_value(json_object_get(temp_ctrl_j, "min_temp"));
//											relay_structure[no].temp_control.temp_choose_flag = json_integer_value(json_object_get(temp_ctrl_j, "temp_choose_flag"));
//											relay_structure[no].temp_control.startup_mode = json_integer_value(json_object_get(temp_ctrl_j, "startup_mode"));
//											relay_structure[no].temp_control.max_nh3 = json_integer_value(json_object_get(temp_ctrl_j, "max_nh3"));
//											relay_structure[no].temp_control.min_nh3 = json_integer_value(json_object_get(temp_ctrl_j, "min_nh3"));
//											
//											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"M1OK\"\r\n",imei_no);//发布消息
//											delay_ms(30);
//											UART3_RxCounter = 0; //重新等待接收下一个推送消息
//											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
//										}
//										// 释放 JSON 对象
//										
//									}	
//									//释放 JSON 对象（位置有待确定）
//									json_decref(temp_ctrl_j);									
									break;
								case 2:
									json_t *time_ctrl = json_object_get(root, "time_ctrl");
									if(json_is_object(time_ctrl)){
										json_t *time_stop = json_object_get(time_ctrl, "time_stop");
										json_t *time_open = json_object_get(time_ctrl, "time_open");
										if(json_is_integer(time_stop) && json_is_integer(time_open)){
											relay_structure[no].time_control.time_open = json_integer_value(time_open);
											relay_structure[no].time_control.time_stop = json_integer_value(time_stop);
											relay_structure[no].relay_mode = 2;
											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"M2OK\"\r\n",imei_no);//发布消息
											#if(REMOVE_memset)
											delay_ms(30);
											UART3_RxCounter = 0; //重新等待接收下一个推送消息
											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
											#endif
										}else{
											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,16,\"M2V Invalid data\"\r\n",imei_no);//发布消息
											#if(REMOVE_memset)
											delay_ms(30);
											UART3_RxCounter = 0; //重新等待接收下一个推送消息
											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
											#endif
										}

									}else{
										UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,15,\"M2 Invalid data\"\r\n",imei_no);//发布消息
										#if(REMOVE_memset)
										delay_ms(30);
										UART3_RxCounter = 0; //重新等待接收下一个推送消息
										memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
										#endif
									}
									
									//									time_ctrl = json_object_get(root, "time_ctrl");
//	//							    if (!json_is_string(time_ctrl)) {
//	//									json_decref(root);
//	//								}
//									// 解析 `data` 中的 JSON 字符串 
//									const char *time_ctrl_text = json_string_value(time_ctrl);
//									json_t *time_ctrl_j  = json_loads(time_ctrl_text, 0, &error);		
//									if(json_is_object(time_ctrl_j)){


//										//showdigit_color(0,110,8,BLUE,BLACK);
//										// 提取 "time_stop" 和 "time_open" 字段
//										json_t *time_stop_j = json_object_get(time_ctrl_j, "time_stop");
//										json_t *time_open_j = json_object_get(time_ctrl_j, "time_open");
//	//									if (!json_is_integer(time_stop_j) || !json_is_integer(time_open_j)) {
//	//										json_decref(time_ctrl_j);
//	//									}
//										if(time_stop_j && time_open_j){
//											relay_structure[no].time_control.time_open = json_integer_value(time_open_j);
//											relay_structure[no].time_control.time_stop = json_integer_value(time_stop_j);
//											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"M2OK\"\r\n",imei_no);//发布消息
//											delay_ms(30);
//											UART3_RxCounter = 0; //重新等待接收下一个推送消息
//											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
//										}

//										// 释放 JSON 对象
//										json_decref(time_ctrl_j);
//									}	
									break;
								case 3:
									json_t *temp_time_ctrl = json_object_get(root, "temp_time_ctrl");
									if(json_is_object(temp_time_ctrl)){
										json_t *max_temp_j = json_object_get(temp_ctrl, "max_temp");
										json_t *min_temp_j = json_object_get(temp_ctrl, "min_temp");
										json_t *temp_choose_flag_j = json_object_get(temp_ctrl, "temp_choose_flag");
										json_t *startup_mode_j = json_object_get(temp_ctrl, "startup_mode");
										json_t *time_stop_j = json_object_get(time_ctrl, "time_stop");
										json_t *time_open_j = json_object_get(time_ctrl, "time_open");
										
										if(json_is_integer(max_temp_j) && json_is_integer(min_temp_j) && json_is_integer(temp_choose_flag_j) && json_is_integer(startup_mode_j) &&
										    json_is_integer(time_stop_j) && json_is_integer(time_open_j)
										){
											relay_structure[no].temp_control.max_temp = json_integer_value(max_temp_j);
											relay_structure[no].temp_control.min_temp = json_integer_value(min_temp_j);
											relay_structure[no].temp_control.startup_mode = json_integer_value(startup_mode_j);
											relay_structure[no].temp_control.temp_choose_flag = json_integer_value(temp_choose_flag_j);
											relay_structure[no].time_control.time_open = json_integer_value(time_open_j);
											relay_structure[no].time_control.time_stop = json_integer_value(time_stop_j);
											relay_structure[no].relay_mode = 3;
											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"M3OK\"\r\n",imei_no);//发布消息
											#if(REMOVE_memset)
											delay_ms(30);
											UART3_RxCounter = 0; //重新等待接收下一个推送消息
											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
											#endif
										}else{
											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,16,\"M3V Invalid data\"\r\n",imei_no);//发布消息
											#if(REMOVE_memset)
											delay_ms(30);
											UART3_RxCounter = 0; //重新等待接收下一个推送消息
											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
											#endif
										}
										
									}else{
										UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,15,\"M3 Invalid data\"\r\n",imei_no);//发布消息
										#if(REMOVE_memset)
										delay_ms(30);
										UART3_RxCounter = 0; //重新等待接收下一个推送消息
										memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
										#endif
									}
//									temp_time_ctrl = json_object_get(root, "temp_time_ctrl");
//									const char *temp_time_ctrl_text = json_string_value(temp_time_ctrl);
//									json_t *temp_time_ctrl_j  = json_loads(temp_time_ctrl_text, 0, &error);
//									if(json_is_object(temp_time_ctrl_j)){
//										
//										if(json_object_get(temp_time_ctrl_j, "max_temp") &&
//											json_object_get(temp_time_ctrl_j, "min_temp") &&
//											json_object_get(temp_time_ctrl_j, "startup_mode") &&
//											json_object_get(temp_time_ctrl_j, "temp_choose_flag") &&
//											json_object_get(temp_time_ctrl_j, "time_open") &&
//											json_object_get(temp_time_ctrl_j, "time_stop")
//										){
//											relay_structure[no].temp_control.max_temp = json_integer_value(json_object_get(temp_time_ctrl_j, "max_temp"));
//											relay_structure[no].temp_control.min_temp = json_integer_value(json_object_get(temp_time_ctrl_j, "min_temp"));
//											relay_structure[no].temp_control.startup_mode = json_integer_value(json_object_get(temp_time_ctrl_j, "startup_mode"));
//											relay_structure[no].temp_control.temp_choose_flag = json_integer_value(json_object_get(temp_time_ctrl_j, "temp_choose_flag"));
//											relay_structure[no].time_control.time_open = json_integer_value(json_object_get(temp_time_ctrl_j, "time_open"));
//											relay_structure[no].time_control.time_stop = json_integer_value(json_object_get(temp_time_ctrl_j, "time_stop"));
//											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"M3OK\"\r\n",imei_no);//发布消息
//											delay_ms(30);
//											UART3_RxCounter = 0; //重新等待接收下一个推送消息
//											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
//										}
//										json_decref(temp_time_ctrl_j);
//									}
									break;
								case 4:
									json_t *time_seg_ctrl = json_object_get(root,"time_seg_ctrl");
									if(json_is_object(time_seg_ctrl)){
										//timer1
										json_t *timer1 = json_object_get(time_seg_ctrl,"timer1");
										json_t *timer2 = json_object_get(time_seg_ctrl,"timer2");
										json_t *timer3 = json_object_get(time_seg_ctrl,"timer3");
										json_t *timer4 = json_object_get(time_seg_ctrl,"timer4");
										json_t *timer5 = json_object_get(time_seg_ctrl,"timer5");
										if(json_is_object(timer1) && json_is_object(timer2) && json_is_object(timer3) && json_is_object(timer4) && json_is_object(timer5)){
											json_t *h1_start = json_object_get(timer1,"h_s");
											json_t *m1_start = json_object_get(timer1,"m_s");
											json_t *h1_stop = json_object_get(timer1,"h_e");
											json_t *m1_stop = json_object_get(timer1,"m_e");
											json_t *on_off_1 = json_object_get(timer1,"on-off");
											
											json_t *h2_start = json_object_get(timer2,"h_s");
											json_t *m2_start = json_object_get(timer2,"m_s");
											json_t *h2_stop = json_object_get(timer2,"h_e");
											json_t *m2_stop = json_object_get(timer2,"m_e");
											json_t *on_off_2 = json_object_get(timer2,"on-off");
											
											json_t *h3_start = json_object_get(timer3,"h_s");
											json_t *m3_start = json_object_get(timer3,"m_s");
											json_t *h3_stop = json_object_get(timer3,"h_e");
											json_t *m3_stop = json_object_get(timer3,"m_e");
											json_t *on_off_3 = json_object_get(timer3,"on-off");
											
											json_t *h4_start = json_object_get(timer4,"h_s");
											json_t *m4_start = json_object_get(timer4,"m_s");
											json_t *h4_stop = json_object_get(timer4,"h_e");
											json_t *m4_stop = json_object_get(timer4,"m_e");
											json_t *on_off_4 = json_object_get(timer4,"on-off");
											
											json_t *h5_start = json_object_get(timer5,"h_s");
											json_t *m5_start = json_object_get(timer5,"m_s");
											json_t *h5_stop = json_object_get(timer5,"h_e");
											json_t *m5_stop = json_object_get(timer5,"m_e");
											json_t *on_off_5 = json_object_get(timer5,"on-off");
											
											if(json_is_integer(h1_start) && json_is_integer(m1_start) && json_is_integer(h1_stop) && json_is_integer(m1_stop) && json_is_integer(on_off_1) &&
												json_is_integer(h2_start) && json_is_integer(m2_start) && json_is_integer(h2_stop) && json_is_integer(m2_stop) && json_is_integer(on_off_2) &&
												json_is_integer(h3_start) && json_is_integer(m3_start) && json_is_integer(h3_stop) && json_is_integer(m3_stop) && json_is_integer(on_off_3) &&
												json_is_integer(h4_start) && json_is_integer(m4_start) && json_is_integer(h4_stop) && json_is_integer(m4_stop) && json_is_integer(on_off_4) &&
												json_is_integer(h5_start) && json_is_integer(m5_start) && json_is_integer(h5_stop) && json_is_integer(m5_stop) && json_is_integer(on_off_5)
											){
												relay_structure[no].time_schedule.relay_time_seg.Time1.hour_start = json_integer_value(h1_start);
												relay_structure[no].time_schedule.relay_time_seg.Time1.minutes_start = json_integer_value(m1_start);
												relay_structure[no].time_schedule.relay_time_seg.Time1.hour_stop = json_integer_value(h1_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time1.minutes_stop = json_integer_value(m1_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time1.on_off = json_integer_value(on_off_1);
													
												relay_structure[no].time_schedule.relay_time_seg.Time2.hour_start = json_integer_value(h2_start);
												relay_structure[no].time_schedule.relay_time_seg.Time2.minutes_start = json_integer_value(m2_start);
												relay_structure[no].time_schedule.relay_time_seg.Time2.hour_stop = json_integer_value(h2_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time2.minutes_stop = json_integer_value(m2_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time2.on_off = json_integer_value(on_off_2);
												
												relay_structure[no].time_schedule.relay_time_seg.Time3.hour_start = json_integer_value(h3_start);
												relay_structure[no].time_schedule.relay_time_seg.Time3.minutes_start = json_integer_value(m3_start);
												relay_structure[no].time_schedule.relay_time_seg.Time3.hour_stop = json_integer_value(h3_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time3.minutes_stop = json_integer_value(m3_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time3.on_off = json_integer_value(on_off_3);
												
												relay_structure[no].time_schedule.relay_time_seg.Time4.hour_start = json_integer_value(h4_start);
												relay_structure[no].time_schedule.relay_time_seg.Time4.minutes_start = json_integer_value(m4_start);
												relay_structure[no].time_schedule.relay_time_seg.Time4.hour_stop = json_integer_value(h4_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time4.minutes_stop = json_integer_value(m4_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time4.on_off = json_integer_value(on_off_4);
												
												relay_structure[no].time_schedule.relay_time_seg.Time5.hour_start = json_integer_value(h5_start);
												relay_structure[no].time_schedule.relay_time_seg.Time5.minutes_start = json_integer_value(m5_start);
												relay_structure[no].time_schedule.relay_time_seg.Time5.hour_stop = json_integer_value(h5_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time5.minutes_stop = json_integer_value(m5_stop);
												relay_structure[no].time_schedule.relay_time_seg.Time5.on_off = json_integer_value(on_off_5);
												relay_structure[no].relay_mode = 4;
												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"M4OK\"\r\n",imei_no);//timer1配置正确
												#if(REMOVE_memset)
												delay_ms(30);
												UART3_RxCounter = 0; 
												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
												#endif
											}else{
												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,18,\"M4hms Invalid data\"\r\n",imei_no);//h,m,s,on-off键值错误
												#if(REMOVE_memset)
												delay_ms(30);
												UART3_RxCounter = 0;
												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
												#endif
											}
										}else{
											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,16,\"M4T Invalid data\"\r\n",imei_no);//发布消息
											#if(REMOVE_memset)
											delay_ms(30);
											UART3_RxCounter = 0;
											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
											#endif
										}
										//timer2
//										timer2 = json_object_get(time_seg_ctrl,"timer2");
//										if(json_is_object(timer2)){
//											hh = json_object_get(timer2,"h");
//											mm = json_object_get(timer2,"m");
//											ss = json_object_get(timer2,"s");
//											on_off = json_object_get(timer2,"on-off");
//											if(json_is_integer(hh) && json_is_integer(mm) && json_is_integer(ss) && json_is_integer(on_off)){
//												relay_structure[no].time_schedule.relay_time_seg.Time2.hour = json_integer_value(hh);
//												relay_structure[no].time_schedule.relay_time_seg.Time2.minutes = json_integer_value(mm);
//												relay_structure[no].time_schedule.relay_time_seg.Time2.seconds = json_integer_value(ss);
//												relay_structure[no].time_schedule.relay_time_seg.Time2.on_off = json_integer_value(on_off);
//												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,6,\"M4T2OK\"\r\n",imei_no);//timer1配置正确
//												delay_ms(30);
//												UART3_RxCounter = 0; 
//												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
//											}else{
//												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,17,\"hms2 Invalid data\"\r\n",imei_no);//h,m,s,on-off键值错误
//												delay_ms(30);
//												UART3_RxCounter = 0;
//												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
//											}
//										}else{
//											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,15,\"T2 Invalid data\"\r\n",imei_no);//发布消息
//											delay_ms(30);
//											UART3_RxCounter = 0;
//											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
//										}
//										//timer3
//										timer3 = json_object_get(time_seg_ctrl,"timer3");
//										if(json_is_object(timer3)){
//											hh = json_object_get(timer3,"h");
//											mm = json_object_get(timer3,"m");
//											ss = json_object_get(timer3,"s");
//											on_off = json_object_get(timer3,"on-off");
//											if(json_is_integer(hh) && json_is_integer(mm) && json_is_integer(ss) && json_is_integer(on_off)){
//												relay_structure[no].time_schedule.relay_time_seg.Time3.hour = json_integer_value(hh);
//												relay_structure[no].time_schedule.relay_time_seg.Time3.minutes = json_integer_value(mm);
//												relay_structure[no].time_schedule.relay_time_seg.Time3.seconds = json_integer_value(ss);
//												relay_structure[no].time_schedule.relay_time_seg.Time3.on_off = json_integer_value(on_off);
//												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,6,\"M4T3OK\"\r\n",imei_no);//timer1配置正确
//												delay_ms(30);
//												UART3_RxCounter = 0; 
//												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
//											}else{
//												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,17,\"hms3 Invalid data\"\r\n",imei_no);//h,m,s,on-off键值错误
//												delay_ms(30);
//												UART3_RxCounter = 0;
//												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
//											}
//										}else{
//											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,15,\"T3 Invalid data\"\r\n",imei_no);//发布消息
//											delay_ms(30);
//											UART3_RxCounter = 0;
//											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
//										}
//										//timer4
//										timer4 = json_object_get(time_seg_ctrl,"timer4");
//										if(json_is_object(timer4)){
//											hh = json_object_get(timer4,"h");
//											mm = json_object_get(timer4,"m");
//											ss = json_object_get(timer4,"s");
//											on_off = json_object_get(timer4,"on-off");
//											if(json_is_integer(hh) && json_is_integer(mm) && json_is_integer(ss) && json_is_integer(on_off)){
//												relay_structure[no].time_schedule.relay_time_seg.Time4.hour = json_integer_value(hh);
//												relay_structure[no].time_schedule.relay_time_seg.Time4.minutes = json_integer_value(mm);
//												relay_structure[no].time_schedule.relay_time_seg.Time4.seconds = json_integer_value(ss);
//												relay_structure[no].time_schedule.relay_time_seg.Time4.on_off = json_integer_value(on_off);
//												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,6,\"M4T4OK\"\r\n",imei_no);//timer1配置正确
//												delay_ms(30);
//												UART3_RxCounter = 0; 
//												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
//											}else{
//												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,17,\"hms4 Invalid data\"\r\n",imei_no);//h,m,s,on-off键值错误
//												delay_ms(30);
//												UART3_RxCounter = 0;
//												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
//											}
//										}else{
//											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,15,\"T4 Invalid data\"\r\n",imei_no);//发布消息
//											delay_ms(30);
//											UART3_RxCounter = 0;
//											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
//										}
//										//timer5
//										timer5 = json_object_get(time_seg_ctrl,"timer5");
//										if(json_is_object(timer5)){
//											hh = json_object_get(timer5,"h");
//											mm = json_object_get(timer5,"m");
//											ss = json_object_get(timer5,"s");
//											on_off = json_object_get(timer5,"on-off");
//											if(json_is_integer(hh) && json_is_integer(mm) && json_is_integer(ss) && json_is_integer(on_off)){
//												relay_structure[no].time_schedule.relay_time_seg.Time5.hour = json_integer_value(hh);
//												relay_structure[no].time_schedule.relay_time_seg.Time5.minutes = json_integer_value(mm);
//												relay_structure[no].time_schedule.relay_time_seg.Time5.seconds = json_integer_value(ss);
//												relay_structure[no].time_schedule.relay_time_seg.Time5.on_off = json_integer_value(on_off);
//												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,6,\"M4T5OK\"\r\n",imei_no);//timer1配置正确
//												delay_ms(30);
//												UART3_RxCounter = 0; 
//												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
//											}else{
//												UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,17,\"hms5 Invalid data\"\r\n",imei_no);//h,m,s,on-off键值错误
//												delay_ms(30);
//												UART3_RxCounter = 0;
//												memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
//											}
//										}else{
//											UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,15,\"T5 Invalid data\"\r\n",imei_no);//发布消息
//											delay_ms(30);
//											UART3_RxCounter = 0;
//											memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
//										}
										
										
									}else{
										UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,15,\"M4 Invalid data\"\r\n",imei_no);//发布消息
										#if(REMOVE_memset)
										delay_ms(30);
										UART3_RxCounter = 0; //重新等待接收下一个推送消息
										memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
										#endif
									}
								
								
								
//									time_seg_ctrl = json_object_get(root, "time_seg_ctrl");
//									const char *time_seg_ctrl_text = json_string_value(time_seg_ctrl);
//									json_t *time_seg_ctrl_j  = json_loads(time_seg_ctrl_text, 0, &error);
//									if(json_is_object(time_seg_ctrl_j)){
//										timer1 = json_object_get(time_seg_ctrl, "timer1");
//										const char *timer1_text = json_string_value(timer1);
//										json_t *time1_j  = json_loads(timer1_text, 0, &error);
//										if(json_is_object(time1_j)){
//											relay_structure[no].time_schedule.relay_time_seg.Time1.hour = json_integer_value(json_object_get(time1_j,"h"));
//											relay_structure[no].time_schedule.relay_time_seg.Time1.minutes = json_integer_value(json_object_get(time1_j,"m"));
//											relay_structure[no].time_schedule.relay_time_seg.Time1.seconds = json_integer_value(json_object_get(time1_j,"s"));
//										}
//										json_decref(time1_j);
//										
//										timer2 = json_object_get(time_seg_ctrl, "timer2");
//										const char *timer2_text = json_string_value(timer2);
//										json_t *time2_j  = json_loads(timer2_text, 0, &error);
//										if(json_is_object(time2_j)){
//											relay_structure[no].time_schedule.relay_time_seg.Time2.hour = json_integer_value(json_object_get(time2_j,"h"));
//											relay_structure[no].time_schedule.relay_time_seg.Time2.minutes = json_integer_value(json_object_get(time2_j,"m"));
//											relay_structure[no].time_schedule.relay_time_seg.Time2.seconds = json_integer_value(json_object_get(time2_j,"s"));
//										}
//										json_decref(time2_j);
//										
//										timer3 = json_object_get(time_seg_ctrl, "timer3");
//										const char *timer3_text = json_string_value(timer3);
//										json_t *time3_j  = json_loads(timer3_text, 0, &error);
//										if(json_is_object(time3_j)){
//											relay_structure[no].time_schedule.relay_time_seg.Time3.hour = json_integer_value(json_object_get(time3_j,"h"));
//											relay_structure[no].time_schedule.relay_time_seg.Time3.minutes = json_integer_value(json_object_get(time3_j,"m"));
//											relay_structure[no].time_schedule.relay_time_seg.Time3.seconds = json_integer_value(json_object_get(time3_j,"s"));
//										}
//										json_decref(time3_j);
//										
//										timer4 = json_object_get(time_seg_ctrl, "timer4");
//										const char *timer4_text = json_string_value(timer4);
//										json_t *time4_j  = json_loads(timer4_text, 0, &error);
//										if(json_is_object(time4_j)){
//											relay_structure[no].time_schedule.relay_time_seg.Time4.hour = json_integer_value(json_object_get(time4_j,"h"));
//											relay_structure[no].time_schedule.relay_time_seg.Time4.minutes = json_integer_value(json_object_get(time4_j,"m"));
//											relay_structure[no].time_schedule.relay_time_seg.Time4.seconds = json_integer_value(json_object_get(time4_j,"s"));
//										}
//										json_decref(time4_j);
//										
//										timer5 = json_object_get(time_seg_ctrl, "timer5");
//										const char *timer5_text = json_string_value(timer5);
//										json_t *time5_j  = json_loads(timer5_text, 0, &error);
//										if(json_is_object(time5_j)){
//											relay_structure[no].time_schedule.relay_time_seg.Time5.hour = json_integer_value(json_object_get(time5_j,"h"));
//											relay_structure[no].time_schedule.relay_time_seg.Time5.minutes = json_integer_value(json_object_get(time5_j,"m"));
//											relay_structure[no].time_schedule.relay_time_seg.Time5.seconds = json_integer_value(json_object_get(time5_j,"s"));
//										}
//										json_decref(time5_j);
//									}
//									json_decref(time_seg_ctrl_j);
									break;
								default:break;
							}
						
						
							//发送回执消息
//							UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,2,\"OK\"\r\n",imei_no);//发布消息
//							delay_ms(30);
//							
//							UART3_RxCounter = 0; //重新等待接收下一个推送消息
//							memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
						}else{
							
							json_t *t1_flag = json_object_get(root, "t1");
							json_t *t2_flag = json_object_get(root, "t2");
							json_t *t3_flag = json_object_get(root, "t3");
							json_t *t485_flag = json_object_get(root, "t485");
							json_t *rh_flag = json_object_get(root, "rh");

							json_t *t1_max = json_object_get(root, "t1_max");
							json_t *t1_min = json_object_get(root, "t1_min");
							
							json_t *t2_max = json_object_get(root, "t2_max");
							json_t *t2_min = json_object_get(root, "t2_min");
							
							json_t *t3_max = json_object_get(root, "t3_max");
							json_t *t3_min = json_object_get(root, "t3_min");
							
							json_t *t485_max = json_object_get(root, "t485_max");
							json_t *t485_min = json_object_get(root, "t485_min");
							
							json_t *rh_max = json_object_get(root, "rh_max");
							json_t *rh_min = json_object_get(root, "rh_min");
							
							json_t *NH3_max_data = json_object_get(root, "NH3_max");
							json_t *NH3_flag_data = json_object_get(root, "NH3_flag");
							
							
							
							json_t *t_high = json_object_get(root, "t_high");
							json_t *t_low = json_object_get(root, "t_low");
							json_t *v_high = json_object_get(root, "v_high");
							json_t *v_low = json_object_get(root, "v_low");
							json_t *t_flag = json_object_get(root, "t_flag");
							
							if(t1_flag && t2_flag && t3_flag && t1_max && t1_min && t2_max && t2_min && t3_max && t3_min && t485_max && t485_min && rh_max && rh_min && NH3_max_data && NH3_flag_data){
								warn_temp1_flag = json_integer_value(t1_flag);
								warn_temp2_flag = json_integer_value(t2_flag);
								warn_temp3_flag = json_integer_value(t3_flag);
								warn_temp485_flag = json_integer_value(t485_flag);
								warn_rh_flag = json_integer_value(rh_flag);
								
								limit_temp1_maxvalue = json_integer_value(t1_max);
								limit_temp1_minvalue = json_integer_value(t1_min);
								
								limit_temp2_maxvalue = json_integer_value(t2_max);
								limit_temp2_minvalue = json_integer_value(t2_min);
								
								limit_temp3_maxvalue = json_integer_value(t3_max);
								limit_temp3_minvalue = json_integer_value(t3_min);
								
								limit_temp485_maxvalue = json_integer_value(t485_max);
								limit_temp485_minvalue = json_integer_value(t485_min);
								
								limit_rh_maxvalue = json_integer_value(rh_max);
								limit_rh_minvalue = json_integer_value(rh_min);	
								
								NH3_max = json_integer_value(NH3_max_data);
								NH3_warn_flag = json_integer_value(NH3_flag_data);
								UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"TNOK\"\r\n",imei_no);//发布消息
								#if(REMOVE_memset)
								delay_ms(30);
								UART3_RxCounter = 0; 
								memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
								#endif
							}else if(t_high && t_low && v_high && v_low && t_flag){
								hz_control.max_temp = json_real_value(t_high);
								hz_control.min_temp = json_real_value(t_low);
								hz_control.voltage_high = json_real_value(v_high);
								hz_control.voltage_low = json_real_value(v_low);
								hz_control.temp_choose = json_integer_value(t_flag);
								UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,4,\"HZOK\"\r\n",imei_no);//发布消息
								#if(REMOVE_memset)
								delay_ms(30);
								UART3_RxCounter = 0; 
								memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
								#endif
							}else{
								UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,17,\"TNHZ Invalid data\"\r\n",imei_no);//发布消息
								#if(REMOVE_memset)
								delay_ms(30);
								UART3_RxCounter = 0; 
								memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE);
								#endif
							}
							
					
						}
						// 释放 JSON 对象
						json_decref(root);
		
					}else{
						UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,17,\"Invalid root data\"\r\n",imei_no);//发布消息
						#if(REMOVE_memset)
						delay_ms(30);
						
						UART3_RxCounter = 0; //重新等待接收下一个推送消息
                        memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
						#endif
					}
					//json_decref(root);
//				delay_ms(30);
//				UART3_RxCounter = 0; //重新等待接收下一个推送消息
//				memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
				    	
//				memset(MQTT_RxDataBuf, 0, RXBUFF_SIZE); //将 MQTT_RxDataBuf 接收缓冲区清0	
				   					
				//LCD_ShowString(60,180,16,(char *)error.text,0);

				}//if(strstr(UART3_RxBuff,"publish")!=NULL)
			UART3_RxCounter = 0; //重新等待接收下一个推送消息
			memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	

		//	}//if(UART3_RxCounter != 0)
		
		}//if(mqtt_flag)
		
		for(int i=0; i<10; i++){
			if(relay_structure[i].relay_mode==0)relay_structure[i].on_off=0;
		}

		if(TIM3_flag){
		    /*定时器标志归零*/
//			UART3_RxCounter = 0; //现将接收的反馈信息数据个数清0
//	        memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
			TIM3_flag=0;//归零
		    TIM3_Counter_10s=0; //未使用，请注释掉该代码，归零，重新开始计时

			if(network_flag==0){
				UART3_Puts("AT+CEREG?\r\n"); 
				delay_ms(10);
				//判断是否联网
				if(UART3_RxCounter != 0){
					if(strstr(UART3_RxBuff,"+CEREG: 0,1")){//判断是否联网
						network_flag=1;
					}else if(strstr(UART3_RxBuff,"+CEREG: 0,0")){
						network_flag=0;
						mqtt_flag=0;
					}else if(strstr(UART3_RxBuff,"error")){
						network_flag=0;
						mqtt_flag=0;
					}
					UART3_RxCounter = 0; //重新等待接收下一个推送消息
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
				}
				
				//读取IMEI
				UART3_Puts("AT+CGSN=1\r\n");
				delay_ms(10);
				if(UART3_RxCounter != 0){
					if(strstr(UART3_RxBuff,"+CGSN")!=NULL){
						//读取IMEI号到imei_no——————————————————————————————————————————————————————————
						//char imei_no[16];
						// 查找 "+CGSN: " 的位置
						
						char *start = strstr(UART3_RxBuff, "+CGSN: ");
						if (start != NULL) {
							start += 7; // 跳过 "+CGSN: " 部分
							strncpy(imei_no, start, 15);
							imei_no[15] = '\0'; // 确保字符串以 '\0' 结束
							//LCD_ShowString(0,220,16,imei_no,0);
						}
					}
					UART3_RxCounter = 0; //重新等待接收下一个推送消息
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
				}

			}
			

			if(network_flag==1 && mqtt_flag==0){
				//判断是否连接mqtt服务器
				
				UART3_Puts("AT+MQTTCFG=\"pingresp\",0,1\r\n"); 

				delay_ms(20);
				UART3_RxCounter = 0; //重新等待接收下一个推送消息
				memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
				//UART3_Puts("AT+MQTTCONN=0,\"8.130.168.245\",1883,\"%s\",\"admin\",\"public\"\r\n",imei_no);
				UART3_Puts("AT+MQTTCONN=0,\"1.192.215.33\",1883,\"%s\",\"admin\",\"public\"\r\n",imei_no);
				//UART3_Puts("AT+MQTTCONN=0,\"39.105.15.166\",1883,\"%s\",\"admin\",\"bdwl123456\"\r\n",imei_no);
				delay_ms(500);
				

//					char im[10];
//					char *start1 = strstr(UART3_RxBuff, "conn");
//					if (start1 != NULL) {
//						strncpy(im, start1, 9);
//						im[9] = '\0'; // 确保字符串以 '\0' 结束
//						//LCD_ShowString(0,220,16,im,0);
//					}
				
				if(UART3_RxCounter != 0){
					if(strstr(UART3_RxBuff,"\"conn\",0,0")!=NULL){
						mqtt_flag = 1;
	                    //LCD_ShowString(0,0,16,"sub",0);
	                    
						//订阅报文
						UART3_Puts("AT+MQTTSUB=0,\"YKWL/CTRL/%s\",2\r\n",imei_no);
						delay_ms(50);
						
						//UART3_Puts("AT+MQTTSUB=0,\"testTopic/1234586\",1\r\n");
						UART3_RxCounter = 0; //重新等待接收下一个推送消息
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
						
						UART3_Puts("AT+MQTTSUB=0,\"YKWL/TNHZCTRL/%s\",2\r\n",imei_no);
						delay_ms(50);
						
						//UART3_Puts("AT+MQTTSUB=0,\"testTopic/1234586\",1\r\n");
						UART3_RxCounter = 0; //重新等待接收下一个推送消息
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	

					}else{
						//LCD_ShowString(0,220,16,"no sub",0);
						mqtt_flag = 0;
					}
					UART3_RxCounter = 0; //重新等待接收下一个推送消息
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
					delay_ms(10);
				}
				
				//获取网络时间
				UART3_Puts("AT+CCLK?\r\n"); 
				delay_ms(25);
				if(UART3_RxCounter != 0){
					//char time_str[20]="";
					if(strstr(UART3_RxBuff,"+CCLK:")){
						char *clock_str = strstr(UART3_RxBuff, "+CCLK: ");
						if (clock_str != NULL) {
							clock_str += 8; // 跳过 "+CGSN: " 部分
							strncpy(time_str, clock_str, 20);
							time_str[20] = '\0'; // 确保字符串以 '\0' 结束
							//LCD_ShowString(0,40,16,time_str,0);
							uint16_t year_t = 0;
							uint8_t month_t = 0;
							uint8_t day_t = 0;
							uint8_t hour_t = 0;
							uint8_t minute_t = 0;
							uint8_t second_t = 0;
							uint8_t tz_offset = 0;
							int result = sscanf(time_str, "%d/%d/%d,%d:%d:%d+%d",&year_t, &month_t, &day_t, &hour_t, &minute_t, &second_t, &tz_offset);
							if(result==7){
								year_t+=2000;
								if((hour_t+(tz_offset/4))>23){
									hour_t-=24;
									if ((year_t % 4 == 0 && year_t % 100 != 0) || (year_t % 400 == 0)) {
										// 闰年 366
										if ((month_t==1 || month_t==3 || month_t==5 || month_t==7 || month_t==8 || month_t==10) && day_t==31) {day_t=1;month_t+=1;}
										if ((month_t==4 || month_t==6 || month_t==8 || month_t==10) && day_t==30) {day_t=1;month_t+=1;}
										if (month_t==2 || day_t==29) {day_t=1;month_t+=1;}
										if (month_t==12 && day_t==31) {year_t+=1;month_t=1;day_t=1;}
										 
									} else {
										// 平年 365
										if ((month_t==1 || month_t==3 || month_t==5 || month_t==7 || month_t==8 || month_t==10) && day_t==31) {day_t=1;month_t+=1;}
										if ((month_t==4 || month_t==6 || month_t==8 || month_t==10) && day_t==30) {day_t=1;month_t+=1;}
										if (month_t==2 || day_t==28) {day_t=1;month_t+=1;}
										if (month_t==12 && day_t==31) {year_t+=1;month_t=1;day_t=1;}
									}
									//赋值给系统时间
									RTC_Set(year_t,month_t,day_t,hour_t,minute_t,second_t);
								} else {
									//赋值给系统时间
									RTC_Set(year_t,month_t,day_t,hour_t+(tz_offset/4),minute_t,second_t);
								}

							}
						}
//						UART3_RxCounter = 0; //重新等待接收下一个推送消息
//						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
					}
					UART3_RxCounter = 0; //重新等待接收下一个推送消息
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
			    }
			}//if(network_flag==1 && mqtt_flag==0)
			
			if(network_flag==1 && MQTT_CON_flag){
				//判断是否连接mqtt服务器
				UART3_Puts("AT+MQTTSTATE=0\r\n");
				delay_ms(10);
				MQTT_CON_Counter=0;
				MQTT_CON_flag=0;
//				if(UART3_RxCounter != 0){
//					if(strstr(UART3_RxBuff,"+MQTTSTATE:2")!=NULL){
//						mqtt_flag = 1;
//						UART3_RxCounter = 0; //重新等待接收下一个推送消息
//						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
//					}else if(strstr(UART3_RxBuff,"+MQTTSTATE:3")!=NULL){
//						UART3_RxCounter = 0; //重新等待接收下一个推送消息
//						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
//						mqtt_flag = 0;
//					}else if(strstr(UART3_RxBuff,"+MQTTSTATE:1")!=NULL){
//						UART3_RxCounter = 0; //重新等待接收下一个推送消息
//						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
//						mqtt_flag = 0;
//					}
//				}
				
				if(UART3_RxCounter != 0){
					if(strstr(UART3_RxBuff,"+MQTTSTATE:2")!=NULL){
						mqtt_flag = 1;
					}else if(strstr(UART3_RxBuff,"+MQTTSTATE:3")!=NULL || strstr(UART3_RxBuff,"+MQTTSTATE:1")!=NULL){

						mqtt_flag = 0;
					}
					
					UART3_RxCounter = 0; //重新等待接收下一个推送消息
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
				}

			}
			
		}//TIM3_flag
		//向服务器发布采集信息
		if(TIM4_flag && mqtt_flag && network_flag){
		//if(TIM4_flag){
			TIM4_flag=0;
			TIM4_Counter_10s=0;
			//实时发送采集数据
			char send_data_collect[20] ="";
			sprintf(send_data_collect,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d;%.2f,%.2f,%.2f,%d,%d,%d,%d,%d",
			relay_structure[0].on_off,relay_structure[1].on_off,relay_structure[2].on_off,relay_structure[3].on_off,relay_structure[4].on_off,relay_structure[5].on_off,relay_structure[6].on_off,relay_structure[7].on_off,relay_structure[8].on_off,relay_structure[9].on_off,
			temperature1,temperature2,temperature3,warn_flag,out_voltage,send_NH3,send_RH,send_TEMP);
			u16 data_collect_len = strlen(send_data_collect);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/COLLECT\",1,0,0,%d,\"%s\"\r\n",imei_no,data_collect_len,send_data_collect);//发布消息
			UART3_RxCounter = 0; //重新等待接收下一个推送消息
			memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
			
//			static uint8_t dd=0;
//			showdigit_color(232,160,dd,GREEN,HOME_BACK);
//			dd++;
			
		}//TIM4_flag
		
		//发送风机配置信息
		//if(send_MQTT_Flag && TIM5_flag && mqtt_flag){
		if(send_CONFIG_FLAG && mqtt_flag && network_flag){
//			TIM5_flag = 0;
//			TIM5_Counter_10s = 0;
			send_CONFIG_FLAG=0;
			u8 send_index = send_CONFIG_INDEX-12;
			char send_data_config[50]="";
			sprintf(send_data_config,"%d,%d,%d;%d,%d,%d,%d,%d,%d;%d,%d;%d,%d,%d,%d,%d;%d,%d,%d,%d,%d;%d,%d,%d,%d,%d;%d,%d,%d,%d,%d;%d,%d,%d,%d,%d",
			relay_structure[send_index].relayNo,relay_structure[send_index].relay_mode,relay_structure[send_index].on_off,
			relay_structure[send_index].temp_control.max_temp,relay_structure[send_index].temp_control.min_temp,relay_structure[send_index].temp_control.temp_choose_flag,relay_structure[send_index].temp_control.startup_mode,
			relay_structure[send_index].temp_control.max_nh3,relay_structure[send_index].temp_control.min_nh3,
			relay_structure[send_index].time_control.time_stop,relay_structure[send_index].time_control.time_open,
			relay_structure[send_index].time_schedule.relay_time_seg.Time1.hour_start,relay_structure[send_index].time_schedule.relay_time_seg.Time1.minutes_start,relay_structure[send_index].time_schedule.relay_time_seg.Time1.hour_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time1.minutes_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time1.on_off,
			relay_structure[send_index].time_schedule.relay_time_seg.Time2.hour_start,relay_structure[send_index].time_schedule.relay_time_seg.Time2.minutes_start,relay_structure[send_index].time_schedule.relay_time_seg.Time2.hour_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time2.minutes_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time2.on_off,
			relay_structure[send_index].time_schedule.relay_time_seg.Time3.hour_start,relay_structure[send_index].time_schedule.relay_time_seg.Time3.minutes_start,relay_structure[send_index].time_schedule.relay_time_seg.Time3.hour_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time3.minutes_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time3.on_off,
			relay_structure[send_index].time_schedule.relay_time_seg.Time4.hour_start,relay_structure[send_index].time_schedule.relay_time_seg.Time4.minutes_start,relay_structure[send_index].time_schedule.relay_time_seg.Time4.hour_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time4.minutes_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time4.on_off,
			relay_structure[send_index].time_schedule.relay_time_seg.Time5.hour_start,relay_structure[send_index].time_schedule.relay_time_seg.Time5.minutes_start,relay_structure[send_index].time_schedule.relay_time_seg.Time5.hour_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time5.minutes_stop,relay_structure[send_index].time_schedule.relay_time_seg.Time5.on_off
			);//sprintf
			u16 config_len = strlen(send_data_config);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/CONFIG\",2,0,0,%d,\"%s\"\r\n",imei_no,config_len,send_data_config);//发布消息
		}	
		//发布报警信息	
		if(warn_flag && mqtt_flag && network_flag){
			
			if((warn_flag1_high || warn_flag1_low) && (warn_timer_count%20==0)){
				char temp1_str[8];
				sprintf(temp1_str, "%.1f", temperature1);
				if(warn_flag1_high){
					char t1_warn_str_h[50] = "{\"b1\" \"NO1 High-temperature alarm\"}-";
					strcat(t1_warn_str_h,temp1_str);
					u16 t1_warn_len_h = strlen(t1_warn_str_h);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,t1_warn_len_h,t1_warn_str_h);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
				}
				if(warn_flag1_low){
					char t1_warn_str_l[50] = "{\"w1\" \"NO1 Low-temperature alarm\"}-";
					strcat(t1_warn_str_l,temp1_str);
					u16 t1_warn_len_l = strlen(t1_warn_str_l);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,t1_warn_len_l,t1_warn_str_l);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
				}

			}
			if((warn_flag2_high || warn_flag2_low) && (warn_timer_count%25==0)){
				char temp2_str[8];
				sprintf(temp2_str, "%.1f", temperature2);
				if(warn_flag2_high){
					char t2_warn_str_h[50] = "{\"b2\" \"NO2 High-temperature alarm\"}-";
					strcat(t2_warn_str_h,temp2_str);
					u16 t2_warn_len_h = strlen(t2_warn_str_h);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,t2_warn_len_h,t2_warn_str_h);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
				
				}
				if(warn_flag2_low){
					char t2_warn_str_l[50] = "{\"w2\" \"NO2 Low-temperature alarm\"}-";
					strcat(t2_warn_str_l,temp2_str);
					u16 t2_warn_len_l = strlen(t2_warn_str_l);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,t2_warn_len_l,t2_warn_str_l);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
				
				}

			}
			if((warn_flag3_high || warn_flag3_low) && (warn_timer_count%20==0)){
				char temp3_str[8];
				sprintf(temp3_str, "%.1f", temperature3);
				if(warn_flag3_high){
					char t3_warn_str_h[50] = "{\"b3\" \"NO3 High-temperature alarm\"}-";
					strcat(t3_warn_str_h,temp3_str);
					u16 t3_warn_len_h = strlen(t3_warn_str_h);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,t3_warn_len_h,t3_warn_str_h);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 			
				
				}
				if(warn_flag3_low){
					char t3_warn_str_l[50] = "{\"w3\" \"NO3 Low-temperature alarm\"}-";
					strcat(t3_warn_str_l,temp3_str);
					u16 t3_warn_len_l = strlen(t3_warn_str_l);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,t3_warn_len_l,t3_warn_str_l);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 			
				
				}

			}
			if((warn_485temp_flag_high ||  warn_485temp_flag_low) && (warn_timer_count%25==0)){
				char temp485_str[8];
				float send_TEMP_BUF=send_TEMP/10;
				sprintf(temp485_str, "%.1f", send_TEMP_BUF);
				if(warn_485temp_flag_high){
					char t485_warn_str_h[55] = "{\"b485\" \"485 High-temperature alarm\"}-";
					strcat(t485_warn_str_h,temp485_str);
					u16 t485_warn_len_h = strlen(t485_warn_str_h);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,t485_warn_len_h,t485_warn_str_h);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 			
				}
				if(warn_485temp_flag_low){
					char t485_warn_str_l[55] = "{\"w485\" \"485 Low-temperature alarm\"}-";
					strcat(t485_warn_str_l,temp485_str);
					u16 t485_warn_len_l = strlen(t485_warn_str_l);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,t485_warn_len_l,t485_warn_str_l);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 			
				}

			}
			if((warn_485rh_flag_high || warn_485rh_flag_low) && (warn_timer_count%20==0)){
				char rh_str[8];
				float send_RH_BUF = send_RH/10;
				sprintf(rh_str, "%.1f", send_RH_BUF);
				if(warn_485rh_flag_high){
					char rh_warn_str_h[40] = "{\"bRH\" \"High-RH alarm\"}-";
					strcat(rh_warn_str_h,rh_str);
					u16 rh_warn_len_h = strlen(rh_warn_str_h);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,rh_warn_len_h,rh_warn_str_h);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
				}
				if(warn_485rh_flag_low){
					char rh_warn_str_l[40] = "{\"wRH\" \"Low-RH alarm\"}-";
					strcat(rh_warn_str_l,rh_str);
					u16 rh_warn_len_l = strlen(rh_warn_str_l);
					UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,rh_warn_len_l,rh_warn_str_l);//发布消息
					delay_ms(5);
					UART3_RxCounter = 0; 
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
				}
			}
			
			if(warn_NH3 && (warn_timer_count%28==0)){
				char NH3_str[8];
				sprintf(NH3_str, "%d", send_NH3);
				char NH3_warn_str[40] = "{\"n1\" \"High-NH3 alarm\"}-";
				strcat(NH3_warn_str,NH3_str);
				u16 NH3_warn_len = strlen(NH3_warn_str);
				UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",2,0,0,%d,\"%s\"\r\n",imei_no,NH3_warn_len,NH3_warn_str);//发布消息
				delay_ms(5);
				UART3_RxCounter = 0; 
				memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); 
			}
			warn_timer_count++;

		}
		if(warn_flag==0)warn_timer_count=0;
//		if(warn_flag && mqtt_flag){
//			char send_data_warn[20]="";
//			sprintf(send_data_warn,"%0.1f,%0.1f,%d,%d,%d",temperature1,temperature2,(warn_flag2<<1)|warn_flag1,send_NH3,warn_NH3);//sprintf
//			u16 warn_len = strlen(send_data_warn);
//			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",0,0,0,%d,\"%s\"\r\n",imei_no,warn_len,send_data_warn);//发布消息
//			delay_ms(10);
//			UART3_RxCounter = 0;
//			memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
//		}
		//发布报警配置信息
		if(send_warn_Flag && mqtt_flag && network_flag){
			send_warn_Flag = 0;
			char send_warn_config[30]="";
			sprintf(send_warn_config,"%d,%d,%d,%d,%d,%d;%d,%d;%d,%d;%d,%d;%d,%d;%d,%d;%d",warn_temp1_flag,warn_temp2_flag,warn_temp3_flag,warn_temp485_flag,warn_rh_flag,NH3_warn_flag,limit_temp1_maxvalue,limit_temp1_minvalue,limit_temp2_maxvalue,limit_temp2_minvalue,limit_temp3_maxvalue,limit_temp3_minvalue,limit_temp485_maxvalue,limit_temp485_minvalue,limit_rh_maxvalue,limit_rh_minvalue,NH3_max);//sprintf
			u16 warn_config_len = strlen(send_warn_config);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARNCONFIG\",2,0,0,%d,\"%s\"\r\n",imei_no,warn_config_len,send_warn_config);//发布消息
		}	
		
		//发布变频器配置信息
		if(send_HZ_Flag && mqtt_flag && network_flag){
			send_HZ_Flag = 0;
			char send_hz_config[30]="";
			sprintf(send_hz_config,"%.1f,%.1f,%.1f,%.1f,%d",hz_control.max_temp,hz_control.min_temp,hz_control.voltage_high,hz_control.voltage_low,hz_control.temp_choose);//sprintf
			u16 hz_config_len = strlen(send_hz_config);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/HZCONFIG\",2,0,0,%d,\"%s\"\r\n",imei_no,hz_config_len,send_hz_config);//发布消息
		}
		
		//发布心跳
		if(Heartbeat_flag && mqtt_flag && network_flag){
			Heartbeat_Counter_1s=0;
			Heartbeat_flag=0;
//			char send_data_Heartbeat[10]="";
//			sprintf(send_data_warn,"%0.1f,%0.1f,%d,%d,%d,%d",);
//			u8 Heartbeat_len = strlen(send_data_Heartbeat);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/HAERTBEAT\",0,0,0,2,\"on\"\r\n",imei_no);//发布心跳

		}
		
		
		

		GUI_Refresh();//刷新显示
		
		ctrl_ui++;
		if(ctrl_ui>3)ctrl_ui=0;
		
		//将配置参数写入flash
		
		// 一分钟一次:1获取网络时间 2判断是否连接网络
		if(TIM5_flag)
		{
			TIM5_flag=0;
			TIM5_Counter_10s=0;
			// 将结构体转换为字节数据
//			uint16_t *data = (uint16_t *)&relay_structure;
//			WriteFlashData(0,data,sizeof(SIZE_OF_RELAY_STRUCTURE));//只写进去一个字节
			
			// 判断是否连接网络
			UART3_Puts("AT+CEREG?\r\n"); 
			delay_ms(10);
			// 判断是否联网
			if(UART3_RxCounter != 0){
				if(strstr(UART3_RxBuff,"+CEREG: 0,1")){//判断是否联网
					network_flag=1;
				}else if(strstr(UART3_RxBuff,"+CEREG: 0,0")){
					network_flag=0;
					mqtt_flag=0;
				}else if(strstr(UART3_RxBuff,"error")){
					network_flag=0;
					mqtt_flag=0;

				}
				UART3_RxCounter = 0; //重新等待接收下一个推送消息
				memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
			}
			
			if(network_flag){
				UART3_Puts("AT+CCLK?\r\n"); 
				delay_ms(25);
				if(UART3_RxCounter != 0){
					char time_str_test[20]="";
					if(strstr(UART3_RxBuff,"+CCLK:")){
						char *clock_str = strstr(UART3_RxBuff, "+CCLK: ");
						if (clock_str != NULL) {
							clock_str += 8; // 跳过 "+CGSN: " 部分
							strncpy(time_str_test, clock_str, 20);
							time_str_test[20] = '\0'; // 确保字符串以 '\0' 结束
							//LCD_ShowString(0,50,16,time_str_test,0);
							uint16_t year_t = 0;
							uint8_t month_t = 0;
							uint8_t day_t = 0;
							uint8_t hour_t = 0;
							uint8_t minute_t = 0;
							uint8_t second_t = 0;
							uint8_t tz_offset = 0;
							int result = sscanf(time_str_test, "%d/%d/%d,%d:%d:%d+%d",&year_t, &month_t, &day_t, &hour_t, &minute_t, &second_t, &tz_offset);
							if(result==7){
								year_t+=2000;
								if((hour_t+(tz_offset/4))>23){
									hour_t-=24;
									if ((year_t % 4 == 0 && year_t % 100 != 0) || (year_t % 400 == 0)) {
										// 闰年 366
										if ((month_t==1 || month_t==3 || month_t==5 || month_t==7 || month_t==8 || month_t==10) && day_t==31) {day_t=1;month_t+=1;}
										if ((month_t==4 || month_t==6 || month_t==8 || month_t==10) && day_t==30) {day_t=1;month_t+=1;}
										if (month_t==2 || day_t==29) {day_t=1;month_t+=1;}
										if (month_t==12 && day_t==31) {year_t+=1;month_t=1;day_t=1;}
										 
									} else {
										// 平年 365
										if ((month_t==1 || month_t==3 || month_t==5 || month_t==7 || month_t==8 || month_t==10) && day_t==31) {day_t=1;month_t+=1;}
										if ((month_t==4 || month_t==6 || month_t==8 || month_t==10) && day_t==30) {day_t=1;month_t+=1;}
										if (month_t==2 || day_t==28) {day_t=1;month_t+=1;}
										if (month_t==12 && day_t==31) {year_t+=1;month_t=1;day_t=1;}
									}
									//赋值给系统时间
									RTC_Set(year_t,month_t,day_t,hour_t,minute_t,second_t);
								} else {
									//赋值给系统时间
									RTC_Set(year_t,month_t,day_t,hour_t+(tz_offset/4),minute_t,second_t);
								}

							}
						}
//						UART3_RxCounter = 0; //重新等待接收下一个推送消息
//						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
					}
					UART3_RxCounter = 0; //重新等待接收下一个推送消息
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
				}
			
			}// network_flag

		}// TIM5_flag
next:

	}//while 
	
}//main


	 
