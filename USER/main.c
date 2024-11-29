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


uint8_t network_flag=0;
uint8_t register_flag=0;
uint8_t mqtt_flag=0;

u8 relay_Control[2] = {0x00, 0x00}; //���Ƽ̵���
u8 relay1_Control_1[2] = {0x00, 0x00};

//static char send_data_config_temp[350]="";
//static char send_data_config[350]="";
//uint8_t send_index_t;

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
	  	/* �����ṹ�� */
	    //_calendar_obj calendar_temp;
	    //�ж����ȼ����� ��2��
	  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  
		delay_init();	    	 //��ʱ������ʼ��	 
		//USART1_Init(115200);
		UART3_Init();
	    /* RTC��ʼ�� */
        RTC_Init();
		/* NTC��ʼ�� */		
		ADC_Configuration();
		//TIM4_Int_Init(0,0);
	    /* TIM3��ʼ�� */	
		TIM3_Int_Init(0,0);	
	    /* TIM2��ʼ�� */	
		TIM2_Int_Init(0,0);	
		
		
		LCD_Init();	   //Һ������ʼ��
		//LCD_Clear(BLACK);
		LCD_Fill(0,0,lcddev.width,lcddev.height,BLACK);
		
		/* GP8201S��ʼ�� */
		IIC_Init();
		GP8201S_Write(0,0,0x00,0x00);//�ϵ����0V
		
        /* 74HC595��ʼ�� */
		HC595_GPIO_Config();
        HC595_Send_Multi_Byte(relay1_Control_1, 2);
		
		/* 485��ʼ�� */
		mbh_init(4800,0);
		
		//��ȡflash�̶�λ�õ�����
//      ReadFlashData(0, (uint8_t *)relay_structure, 410);
//		if(relay_structure[1].relayNo == 0){
//			Relay_Structure relay_structure[10] = {{0}};
//			relay_structure[0].relayNo = 1;
//		    printf("%d\n", relay_structure[0].relayNo);
//		}

//		if(relay_structure[1].relayNo == 255){
//			//relay_structure[9] = {{0}};
//			relay_structure[0].relayNo = 1;
//		    printf("%d+\n", relay_structure[0].relayNo);
//		}
		
	while(1) 
	{	
		u8 warn_flag1=0;
		u8 warn_flag2=0;
		u8 warn_flag3=0;
		u8 warn_NH3=0;
		if(warn_temp1_flag && (temperature1>0.0) && (temperature1<90.0)){
			if(temperature1>limit_temp_maxvalue || temperature1<limit_temp_minvalue)warn_flag1=1;else warn_flag1=0;
		}
		if(warn_temp2_flag && (temperature2>0.0) && (temperature2<90.0)){
			if(temperature2>limit_temp_maxvalue || temperature2<limit_temp_minvalue)warn_flag2=1;else warn_flag2=0;
		}
		if(warn_temp3_flag && (temperature3>0.0) && (temperature3<90.0)){
			if(temperature3>limit_temp_maxvalue || temperature3<limit_temp_minvalue)warn_flag3=1;else warn_flag3=0;
		}
		if(NH3_warn_flag){
			if(send_NH3!=999 && send_NH3>=NH3_max && send_NH3<100)warn_NH3=1;
			if(send_NH3==999 || send_NH3<NH3_max)warn_NH3=0;
		}else{
			warn_NH3=0;
		}

		//printf("warn_NH3:%d\n",warn_NH3);
		
		warn_flag = warn_flag1 || warn_flag2 || warn_flag3 || warn_NH3;
		//printf("warn_flag:%d\n",warn_flag);
		
		
		
		//˳�����̵���
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
		
		//2-3����˳����,�����д������if�ж�����������������Ŀ���״̬��

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
		
		temp = relay_Control[0] & 0Xdf | (relay_structure[9].on_off << 5) | (warn_flag << 4);
		if(relay_Control[0]!=temp && TIM3_flag){relay_Control[0]=temp;HC595_Send_Multi_Byte(relay_Control, 2);goto next;}

	next:	
		//485����
		uint8_t cc[4] = {0x00,0x02,0x00,0x01};
		mbh_send(1,0x03,cc,4);
		mbh_poll();
		
		//printf("pframe[4]:%x\n",temp1);
		
		if(mqtt_flag){
//			UART3_Puts("AT+MQTTREAD=0?\r\n"); 
//			delay_ms(100);
			
			
			//LCD_ShowString(100,200,16,"mq",0);
			//�ж��Ƿ�ע�ᣬ����Э���жϷ���������������Ϣ����Ҫ�ı�ı���
			if(UART3_RxCounter != 0) {
//					char im1[40];
//					char *start1 = strstr(UART3_RxBuff, "publish");
//					if (start1 != NULL) {
//						strncpy(im1, start1, 39);
//						im1[39] = '\0'; // ȷ���ַ����� '\0' ����
//						//LCD_ShowString(0,220,16,im,0);
//					}
				    //LCD_ShowString(120,200,16,im1,0);
				//�жϷ���������������Ϣ
				if(strstr(UART3_RxBuff,"publish")!=NULL){
	//			   PAout(8)=0;
	//			   delay_ms(1000);
				   	char *str1 = NULL;							//AT commnd string pointer
					char *str2 = NULL;
					char userData[250];				//��������
					char *pcBegin = NULL;
					char *pcEnd = NULL;
					
					str1=(char *)UART3_RxBuff;
					str2=strstr(str1,"data=");
					if(str2!=NULL){
						pcBegin=strstr(str2,"data=");
						pcEnd=strstr(str2,"end");
						pcBegin+=5;
						if((pcBegin != NULL) && (pcEnd != NULL))		//��ʼ�ͽ������ж�Ӧ���ַ�
						{
							memcpy(userData, pcBegin, (pcEnd-pcBegin));	//��pcBegin��ַ��ʼ�����Ƴ�(pcEnd-pcBegin)������
						}	
					}


					//LCD_ShowString(125,216,16,(char*)userData,0);
					
//				   u8 rev_len = 0;
//				   char rev_data[3];
////				   char *dd = "+MQTTURC: \"publish\",0,2,\"world\",5,5,12345";
//				   char *token = strtok(UART3_RxBuff,",");
//				   
//				   u8 i = 0;
//				   while (token != NULL) { // ����ÿ����ֵ��
//						
//						if (strlen(token) > 0 && i==5) { // ȷ��token���ǿ��ַ���
//							if (sscanf(token, "%d", &rev_len) == 1) { // �������� "n":"xx.xx&xx.xx" ���ַ�������ȡ����������
//								// �������Ǽ�����ֵ���� "xx.xx&xx.xx" �ĸ�ʽ���ֵģ����Ը���ʵ��������� sscanf �ĸ�ʽ�ַ���
//								//printf("��ȡ���������ǣ�%d\n", rev_len); 	
//								char str[5]; // �����㹻���������������ַ�����ʾ
//                                sprintf(str, "%d", rev_len); // ������ת��Ϊ�ַ���
//								//LCD_ShowString(125,216,16,str,0);
//							}
//						}
//						if(strlen(token) > 0 && i==6){
//							if(sscanf(token, "%s", rev_data) == 1){
//								//printf("��ȡ���������ǣ�%s\n", rev_data);
//								//LCD_ShowString(125,216,16,rev_data,0);
//							}
//						}
//						i++;
//						token = strtok(NULL, ","); // ����������һ����ֵ��
//					}
//					i=0;
					//�����ַ���
					json_error_t error;
					json_t *root;
					json_t *temp_ctrl;
					json_t *time_ctrl;
					json_t *temp_time_ctrl;
					json_t *time_seg_ctrl;
					json_t *timer1;
					json_t *timer2;
					json_t *timer3;
					json_t *timer4;
					json_t *timer5;
               //showdigit_color(336,132,1,BLUE,BLACK);
			  // const char *text = "{\"success\":2}";
			  //char text[100]="{\"no\":5,\"mode\":2}";
			 // char *text = "{\"no\":5,\"mode\":2}";
			  //LCD_ShowString(0,216,16,(char*)userData,0);
					root = json_loads((const char*)userData, 0, &error); 
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
						
						json_t *out_voltage_j = json_object_get(root, "out_voltage");
//						if (!json_is_integer(out_voltage_j)) {
//							json_decref(root);
//						}
						if(out_voltage_j){
							out_voltage = json_integer_value(out_voltage_j);
							if(out_voltage>=0 && out_voltage<=10){
								u16 data_v = out_voltage*4095/10;
								u8 v_low = data_v<<4 | 0x00;
								u8 v_high = data_v>>4 | 0x00;
								GP8201S_Write(0,0,v_low,v_high);
							}
						}

						if(mode_j && no_j){
							int no = json_integer_value(no_j)-1;
							int mode = json_integer_value(mode_j);
							relay_structure[no].relay_mode = mode;
						
							switch(mode){
								case 1:						
	//								temp_ctrl = json_object_get(root, "temp_ctrl");
	//								if(json_is_object(temp_ctrl)){
	//									relay_structure[no].temp_control.max_temp = json_integer_value(json_object_get(temp_ctrl, "max_temp"));
	//									relay_structure[no].temp_control.min_temp = json_integer_value(json_object_get(temp_ctrl, "min_temp"));
	//									relay_structure[no].temp_control.temp_choose_flag = json_integer_value(json_object_get(temp_ctrl, "temp_choose_flag"));
	//									relay_structure[no].temp_control.startup_mode = json_integer_value(json_object_get(temp_ctrl, "startup_mode"));
	//									relay_structure[no].temp_control.max_nh3 = json_integer_value(json_object_get(temp_ctrl, "max_nh3"));
	//									relay_structure[no].temp_control.min_nh3 = json_integer_value(json_object_get(temp_ctrl, "min_nh3"));
	//								}
									temp_ctrl = json_object_get(root, "temp_ctrl");
									const char *temp_ctrl_text = json_string_value(temp_ctrl);
									json_t *temp_ctrl_j  = json_loads(temp_ctrl_text, 0, &error);	
									if(json_is_object(temp_ctrl_j)){
										//showdigit_color(0,90,8,BLUE,BLACK);
										if(json_object_get(temp_ctrl_j, "max_temp") &&
											json_object_get(temp_ctrl_j, "min_temp") &&
										    json_object_get(temp_ctrl_j, "temp_choose_flag") &&
										    json_object_get(temp_ctrl_j, "startup_mode") &&
										    json_object_get(temp_ctrl_j, "max_nh3") &&
											json_object_get(temp_ctrl_j, "min_nh3")
										){
											relay_structure[no].temp_control.max_temp = json_integer_value(json_object_get(temp_ctrl_j, "max_temp"));
											relay_structure[no].temp_control.min_temp = json_integer_value(json_object_get(temp_ctrl_j, "min_temp"));
											relay_structure[no].temp_control.temp_choose_flag = json_integer_value(json_object_get(temp_ctrl_j, "temp_choose_flag"));
											relay_structure[no].temp_control.startup_mode = json_integer_value(json_object_get(temp_ctrl_j, "startup_mode"));
											relay_structure[no].temp_control.max_nh3 = json_integer_value(json_object_get(temp_ctrl_j, "max_nh3"));
											relay_structure[no].temp_control.min_nh3 = json_integer_value(json_object_get(temp_ctrl_j, "min_nh3"));
										
										}
										// �ͷ� JSON ����
										json_decref(temp_ctrl_j);
									}								    
									break;
								case 2:
									time_ctrl = json_object_get(root, "time_ctrl");
	//							    if (!json_is_string(time_ctrl)) {
	//									json_decref(root);
	//								}
									// ���� `data` �е� JSON �ַ��� 
									const char *time_ctrl_text = json_string_value(time_ctrl);
									json_t *time_ctrl_j  = json_loads(time_ctrl_text, 0, &error);		
									if(json_is_object(time_ctrl_j)){


										//showdigit_color(0,110,8,BLUE,BLACK);
										// ��ȡ "time_stop" �� "time_open" �ֶ�
										json_t *time_stop_j = json_object_get(time_ctrl_j, "time_stop");
										json_t *time_open_j = json_object_get(time_ctrl_j, "time_open");
	//									if (!json_is_integer(time_stop_j) || !json_is_integer(time_open_j)) {
	//										json_decref(time_ctrl_j);
	//									}
										if(time_stop_j && time_open_j){
											relay_structure[no].time_control.time_open = json_integer_value(time_open_j);
											relay_structure[no].time_control.time_stop = json_integer_value(time_stop_j);
										}

										// �ͷ� JSON ����
										json_decref(time_ctrl_j);
									}								    

									break;
								case 3:
									temp_time_ctrl = json_object_get(root, "temp_time_ctrl");
									const char *temp_time_ctrl_text = json_string_value(temp_time_ctrl);
									json_t *temp_time_ctrl_j  = json_loads(temp_time_ctrl_text, 0, &error);
									if(json_is_object(temp_time_ctrl_j)){
										
										if(json_object_get(temp_time_ctrl_j, "max_temp") &&
											json_object_get(temp_time_ctrl_j, "min_temp") &&
											json_object_get(temp_time_ctrl_j, "startup_mode") &&
											json_object_get(temp_time_ctrl_j, "temp_choose_flag") &&
											json_object_get(temp_time_ctrl_j, "time_open") &&
											json_object_get(temp_time_ctrl_j, "time_stop")
										){
											relay_structure[no].temp_control.max_temp = json_integer_value(json_object_get(temp_time_ctrl_j, "max_temp"));
											relay_structure[no].temp_control.min_temp = json_integer_value(json_object_get(temp_time_ctrl_j, "min_temp"));
											relay_structure[no].temp_control.startup_mode = json_integer_value(json_object_get(temp_time_ctrl_j, "startup_mode"));
											relay_structure[no].temp_control.temp_choose_flag = json_integer_value(json_object_get(temp_time_ctrl_j, "temp_choose_flag"));
											relay_structure[no].time_control.time_open = json_integer_value(json_object_get(temp_time_ctrl_j, "time_open"));
											relay_structure[no].time_control.time_stop = json_integer_value(json_object_get(temp_time_ctrl_j, "time_stop"));
										}
										json_decref(temp_time_ctrl_j);
									}
									break;
								case 4:
									time_seg_ctrl = json_object_get(root, "time_seg_ctrl");
									const char *time_seg_ctrl_text = json_string_value(time_seg_ctrl);
									json_t *time_seg_ctrl_j  = json_loads(time_seg_ctrl_text, 0, &error);
									if(json_is_object(time_seg_ctrl_j)){
										timer1 = json_object_get(time_seg_ctrl, "timer1");
										const char *timer1_text = json_string_value(timer1);
										json_t *time1_j  = json_loads(timer1_text, 0, &error);
										if(json_is_object(time1_j)){
											relay_structure[no].time_schedule.relay_time_seg.Time1.hour = json_integer_value(json_object_get(time1_j,"h"));
											relay_structure[no].time_schedule.relay_time_seg.Time1.minutes = json_integer_value(json_object_get(time1_j,"m"));
											relay_structure[no].time_schedule.relay_time_seg.Time1.seconds = json_integer_value(json_object_get(time1_j,"s"));
										}
										json_decref(time1_j);
										
										timer2 = json_object_get(time_seg_ctrl, "timer2");
										const char *timer2_text = json_string_value(timer2);
										json_t *time2_j  = json_loads(timer2_text, 0, &error);
										if(json_is_object(time2_j)){
											relay_structure[no].time_schedule.relay_time_seg.Time2.hour = json_integer_value(json_object_get(time2_j,"h"));
											relay_structure[no].time_schedule.relay_time_seg.Time2.minutes = json_integer_value(json_object_get(time2_j,"m"));
											relay_structure[no].time_schedule.relay_time_seg.Time2.seconds = json_integer_value(json_object_get(time2_j,"s"));
										}
										json_decref(time2_j);
										
										timer3 = json_object_get(time_seg_ctrl, "timer3");
										const char *timer3_text = json_string_value(timer3);
										json_t *time3_j  = json_loads(timer3_text, 0, &error);
										if(json_is_object(time3_j)){
											relay_structure[no].time_schedule.relay_time_seg.Time3.hour = json_integer_value(json_object_get(time3_j,"h"));
											relay_structure[no].time_schedule.relay_time_seg.Time3.minutes = json_integer_value(json_object_get(time3_j,"m"));
											relay_structure[no].time_schedule.relay_time_seg.Time3.seconds = json_integer_value(json_object_get(time3_j,"s"));
										}
										json_decref(time3_j);
										
										timer4 = json_object_get(time_seg_ctrl, "timer4");
										const char *timer4_text = json_string_value(timer4);
										json_t *time4_j  = json_loads(timer4_text, 0, &error);
										if(json_is_object(time4_j)){
											relay_structure[no].time_schedule.relay_time_seg.Time4.hour = json_integer_value(json_object_get(time4_j,"h"));
											relay_structure[no].time_schedule.relay_time_seg.Time4.minutes = json_integer_value(json_object_get(time4_j,"m"));
											relay_structure[no].time_schedule.relay_time_seg.Time4.seconds = json_integer_value(json_object_get(time4_j,"s"));
										}
										json_decref(time4_j);
										
										timer5 = json_object_get(time_seg_ctrl, "timer5");
										const char *timer5_text = json_string_value(timer5);
										json_t *time5_j  = json_loads(timer5_text, 0, &error);
										if(json_is_object(time5_j)){
											relay_structure[no].time_schedule.relay_time_seg.Time5.hour = json_integer_value(json_object_get(time5_j,"h"));
											relay_structure[no].time_schedule.relay_time_seg.Time5.minutes = json_integer_value(json_object_get(time5_j,"m"));
											relay_structure[no].time_schedule.relay_time_seg.Time5.seconds = json_integer_value(json_object_get(time5_j,"s"));
										}
										json_decref(time5_j);
									}
									json_decref(time_seg_ctrl_j);
									break;
								default:break;
							}
						
						
							//���ͻ�ִ��Ϣ
							UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,2,\"OK\"\r\n",imei_no);//������Ϣ
							delay_ms(30);
							
							UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
							memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
						}else{
							UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,12,\"Invalid data\"\r\n",imei_no);//������Ϣ
							delay_ms(30);
							
							UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
							memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
						}
						
						json_t *t1_flag = json_object_get(root, "t1");
						json_t *t2_flag = json_object_get(root, "t2");
						json_t *t3_flag = json_object_get(root, "t3");
						json_t *t_max = json_object_get(root, "t_max");
						json_t *t_min = json_object_get(root, "t_min");
						json_t *NH3_max_data = json_object_get(root, "NH3_max");
						json_t *NH3_flag_data = json_object_get(root, "NH3_flag");
						
						if(t1_flag && t2_flag && t3_flag && t_max && t_min && NH3_max_data && NH3_flag_data){
							warn_temp1_flag = json_integer_value(t1_flag);
							warn_temp2_flag = json_integer_value(t2_flag);
							warn_temp3_flag = json_integer_value(t3_flag);
							limit_temp_maxvalue = json_integer_value(t_max);
							limit_temp_minvalue = json_integer_value(t_min);
							NH3_max = json_integer_value(NH3_max_data);
							NH3_warn_flag = json_integer_value(NH3_flag_data);
						}
						


						json_decref(root);
		
					}else{
						UART3_Puts("AT+MQTTPUB=0,\"YKWL/Callback/%s\",0,0,0,12,\"Invalid data\"\r\n",imei_no);//������Ϣ
						delay_ms(30);
						
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
                        memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
					}
					//json_decref(root);
					
				    	
					
				//LCD_ShowString(60,180,16,(char *)error.text,0);
//				UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
//				memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
				   
				}//if(strstr(UART3_RxBuff,"publish")!=NULL)

			}//if(UART3_RxCounter != 0)
		
		}//if(mqtt_flag)
		
		for(int i=0; i<10; i++){
			if(relay_structure[i].relay_mode==0)relay_structure[i].on_off=0;
		}

		if(TIM3_flag){
		    /*��ʱ����־����*/
//			UART3_RxCounter = 0; //�ֽ����յķ�����Ϣ���ݸ�����0
//	        memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0
			TIM3_flag=0;//����
		    TIM3_Counter_10s=0; //δʹ�ã���ע�͵��ô��룬���㣬���¿�ʼ��ʱ

			if(network_flag==0){
				UART3_Puts("AT+CEREG?\r\n"); 
				delay_ms(5);
				//�ж��Ƿ�����
				if(UART3_RxCounter != 0){
					if(strstr(UART3_RxBuff,"+CEREG: 0,1")){//�ж��Ƿ�����
						network_flag=1;
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	

					}else if(strstr(UART3_RxBuff,"+CEREG: 0,0")){
						network_flag=0;
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
					}else if(strstr(UART3_RxBuff,"error")){
						network_flag=0;
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
					}

				}
				

			}
			
			if(network_flag==1){
				//��ȡIMEI
				UART3_Puts("AT+CGSN=1\r\n");
				delay_ms(10);
				if(UART3_RxCounter != 0){
					if(strstr(UART3_RxBuff,"+CGSN")!=NULL){
						//��ȡIMEI�ŵ�imei_no��������������������������������������������������������������������������������������������������������������������
						//char imei_no[16];
						// ���� "+CGSN: " ��λ��
						
						char *start = strstr(UART3_RxBuff, "+CGSN: ");
						if (start != NULL) {
							start += 7; // ���� "+CGSN: " ����
							strncpy(imei_no, start, 15);
							imei_no[15] = '\0'; // ȷ���ַ����� '\0' ����
							//LCD_ShowString(0,220,16,imei_no,0);
						}
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
					}
				}

			}

			if(network_flag==1 && mqtt_flag==0){
				//�ж��Ƿ�����mqtt������
				
				UART3_Puts("AT+MQTTCFG=\"pingresp\",0,1\r\n"); 

				delay_ms(10);
				UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
				memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
				UART3_Puts("AT+MQTTCONN=0,\"8.130.168.245\",1883,\"%s\",\"admin\",\"public\"\r\n",imei_no);
				delay_ms(500);
				

//					char im[10];
//					char *start1 = strstr(UART3_RxBuff, "conn");
//					if (start1 != NULL) {
//						strncpy(im, start1, 9);
//						im[9] = '\0'; // ȷ���ַ����� '\0' ����
//						//LCD_ShowString(0,220,16,im,0);
//					}
				
				if(UART3_RxCounter != 0){
					if(strstr(UART3_RxBuff,"\"conn\",0,0")!=NULL){
						mqtt_flag = 1;
	                    //LCD_ShowString(0,200,16,"sub",0);
	                    
						//���ı���
						UART3_Puts("AT+MQTTSUB=0,\"YKWL/CTRL/%s\",0\r\n",imei_no);
						delay_ms(50);
						
						//UART3_Puts("AT+MQTTSUB=0,\"testTopic/1234586\",1\r\n");
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	

					}else{
						//LCD_ShowString(0,220,16,"no sub",0);
						mqtt_flag = 0;
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
					}
					UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
					memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
					delay_ms(10);
				}
				
				

			}
			
			
			if(network_flag==1){
				//�ж��Ƿ�����mqtt������
				UART3_Puts("AT+MQTTSTATE=0\r\n");
				delay_ms(50);

//				if(UART3_RxCounter != 0){
//					if(strstr(UART3_RxBuff,"+MQTTSTATE:2")!=NULL){
//						mqtt_flag = 1;
//						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
//						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
//					}else if(strstr(UART3_RxBuff,"+MQTTSTATE:3")!=NULL){
//						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
//						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
//						mqtt_flag = 0;
//					}else if(strstr(UART3_RxBuff,"+MQTTSTATE:1")!=NULL){
//						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
//						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
//						mqtt_flag = 0;
//					}
//				}
				
				
				if(UART3_RxCounter != 0){
					if(strstr(UART3_RxBuff,"+MQTTSTATE:2")!=NULL){
						mqtt_flag = 1;
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
					}else if(strstr(UART3_RxBuff,"+MQTTSTATE:3")!=NULL || strstr(UART3_RxBuff,"+MQTTSTATE:1")!=NULL){
						UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
						memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
						mqtt_flag = 0;
					}
				}

			}

			
			
		}//TIM3_flag
		//������������ɼ���Ϣ
		if(TIM4_flag && mqtt_flag){
		//if(TIM4_flag){
			TIM4_flag=0;
			TIM4_Counter_10s=0;
			//ʵʱ���Ͳɼ�����
			char send_data_collect[20] ="";
			sprintf(send_data_collect,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d;%.2f,%.2f,%.2f,%d,%d,%d",
			relay_structure[0].on_off,relay_structure[1].on_off,relay_structure[2].on_off,relay_structure[3].on_off,relay_structure[4].on_off,relay_structure[5].on_off,relay_structure[6].on_off,relay_structure[7].on_off,relay_structure[8].on_off,relay_structure[9].on_off,
			temperature1,temperature2,temperature3,warn_flag,out_voltage,send_NH3);
			u16 data_collect_len = strlen(send_data_collect);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/COLLECT\",0,0,0,%d,\"%s\"\r\n",imei_no,data_collect_len,send_data_collect);//������Ϣ
			delay_ms(10);
			UART3_RxCounter = 0;
			memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0
			
		}//TIM4_flag
		
		//����������Ϣ
		if(send_MQTT_Flag && TIM5_flag && mqtt_flag){
			TIM5_flag = 0;
			TIM5_Counter_10s = 0;
			//printf("%d\n",send_MQTT_Flag);
			u8 send_index = send_MQTT_Flag-12;
			char send_data_config[50]="";
			sprintf(send_data_config,"%d,%d,%d;%0.1f,%0.1f,%d,%d,%d,%d;%d,%d;%d,%d,%d,%d;%d,%d,%d,%d;%d,%d,%d,%d;%d,%d,%d,%d;%d,%d,%d,%d",
			relay_structure[send_index].relayNo,relay_structure[send_index].relay_mode,relay_structure[send_index].on_off,
			relay_structure[send_index].temp_control.max_temp,relay_structure[send_index].temp_control.min_temp,relay_structure[send_index].temp_control.temp_choose_flag,relay_structure[send_index].temp_control.startup_mode,
			relay_structure[send_index].temp_control.max_nh3,relay_structure[send_index].temp_control.min_nh3,
			relay_structure[send_index].time_control.time_stop,relay_structure[send_index].time_control.time_open,
			relay_structure[send_index].time_schedule.relay_time_seg.Time1.hour,relay_structure[send_index].time_schedule.relay_time_seg.Time1.minutes,relay_structure[send_index].time_schedule.relay_time_seg.Time1.seconds,relay_structure[send_index].time_schedule.relay_time_seg.Time1.on_off,
			relay_structure[send_index].time_schedule.relay_time_seg.Time2.hour,relay_structure[send_index].time_schedule.relay_time_seg.Time2.minutes,relay_structure[send_index].time_schedule.relay_time_seg.Time2.seconds,relay_structure[send_index].time_schedule.relay_time_seg.Time2.on_off,
			relay_structure[send_index].time_schedule.relay_time_seg.Time3.hour,relay_structure[send_index].time_schedule.relay_time_seg.Time3.minutes,relay_structure[send_index].time_schedule.relay_time_seg.Time3.seconds,relay_structure[send_index].time_schedule.relay_time_seg.Time3.on_off,
			relay_structure[send_index].time_schedule.relay_time_seg.Time4.hour,relay_structure[send_index].time_schedule.relay_time_seg.Time4.minutes,relay_structure[send_index].time_schedule.relay_time_seg.Time4.seconds,relay_structure[send_index].time_schedule.relay_time_seg.Time4.on_off,
			relay_structure[send_index].time_schedule.relay_time_seg.Time5.hour,relay_structure[send_index].time_schedule.relay_time_seg.Time5.minutes,relay_structure[send_index].time_schedule.relay_time_seg.Time5.seconds,relay_structure[send_index].time_schedule.relay_time_seg.Time5.on_off
			);//sprintf
			u16 config_len = strlen(send_data_config);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/CONFIG\",0,0,0,%d,\"%s\"\r\n",imei_no,config_len,send_data_config);//������Ϣ
			delay_ms(10);
			UART3_RxCounter = 0;
			memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0
			send_MQTT_Flag=0;
		}	
		//��������
		if(warn_flag && mqtt_flag){
			char send_data_warn[20]="";
			sprintf(send_data_warn,"%0.1f,%0.1f,%d,%d,%d",temperature1,temperature2,(warn_flag2<<1)|warn_flag1,send_NH3,warn_NH3);//sprintf
			u16 warn_len = strlen(send_data_warn);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/WARN\",0,0,0,%d,\"%s\"\r\n",imei_no,warn_len,send_data_warn);//������Ϣ
			delay_ms(10);
			UART3_RxCounter = 0;
			memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0
		}
		//��������
		if(Heartbeat_flag && mqtt_flag){
			Heartbeat_Counter_1s=0;
			Heartbeat_flag=0;
//			char send_data_Heartbeat[10]="";
//			sprintf(send_data_warn,"%0.1f,%0.1f,%d,%d,%d,%d",);
//			u8 Heartbeat_len = strlen(send_data_Heartbeat);
			UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s/HAERTBEAT\",0,0,0,2,\"on\"\r\n",imei_no);//��������
		}
		
		GUI_Refresh();//ˢ����ʾ

		ctrl_ui++;
		if(ctrl_ui>3)ctrl_ui=0;
		

	}//while 
	
}//main


	 