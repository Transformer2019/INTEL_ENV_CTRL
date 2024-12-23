/**
  ******************************************************************************
  * @file    mb_hook.c
  * @author  Derrick Wang
  * @brief   modebus回调函数接口
  ******************************************************************************
  * @note	
  * 针对modbus的回调处理，请再该文件中添加		
  ******************************************************************************
  */

#include "mb_include.h"
#include "usart1.h"
#include "delay.h"

uint16_t send_NH3 = 999;
int16_t send_TEMP = 999;
uint16_t send_RH = 999;
uint32_t send_CO2 = 99999;


int16_t send_TEMP_1 = 999;
uint16_t send_RH_1 = 999;
int16_t send_TEMP_2 = 999;
uint16_t send_RH_2 = 999;

void mbh_hook_rec01(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec02(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec03(uint8_t add,uint8_t *data,uint8_t datalen)
{
//	static uint8_t send485_flag=0;
//	send485_flag++;
//	if(send485_flag==10)send485_flag=0;
	
//	if(add == 1){send_NH3 = *(data+2);}
//	if(add == 2){}
//	if(add == 3){
//		send_RH = *(data+1)*0x100 + *(data+2);
//		uint16_t send_TEMP_buf = *(data+3)*0x100 + *(data+4);
//		if(send_TEMP_buf > 0x7FFF){
//			send_TEMP = (int16_t)(send_TEMP_buf - 0x10000);
//		}else{
//			send_TEMP = (int16_t)send_TEMP_buf;
//		}
//		
//		send_CO2 = *(data+5)*0x100 + *(data+6);
//	}
	
	
	if(add == 1){
		send_RH_1 = *(data+1)*0x100 + *(data+2);
		uint16_t send_TEMP_buf_1 = *(data+3)*0x100 + *(data+4);
		if(send_TEMP_buf_1 > 0x7FFF){
			send_TEMP_1 = (int16_t)(send_TEMP_buf_1 - 0x10000);
		}else{
			send_TEMP_1 = (int16_t)send_TEMP_buf_1;
		}
	}
	if(add == 2){
		send_RH_2 = *(data+1)*0x100 + *(data+2);
		uint16_t send_TEMP_buf_2 = *(data+3)*0x100 + *(data+4);
		if(send_TEMP_buf_2 > 0x7FFF){
			send_TEMP_2 = (int16_t)(send_TEMP_buf_2 - 0x10000);
		}else{
			send_TEMP_2 = (int16_t)send_TEMP_buf_2;
		}
	}
	if(add == 5){
		send_RH = *(data+1)*0x100 + *(data+2);
		uint16_t send_TEMP_buf = *(data+3)*0x100 + *(data+4);
		if(send_TEMP_buf > 0x7FFF){
			send_TEMP = (int16_t)(send_TEMP_buf - 0x10000);
		}else{
			send_TEMP = (int16_t)send_TEMP_buf;
		}
		
		send_CO2 = *(data+5)*0x100 + *(data+6);
	}
	
//	printf("湿度:%d\n",send_RH);
//	printf("温度:%d\n",send_TEMP);
//	printf("CO2浓度:%d\n\n",send_CO2);
//	
//	printf("湿度一:%d\n",send_RH_1);
//	printf("温度一:%d\n\n",send_TEMP_1);

//	printf("湿度二:%d\n",send_RH_2);
//	printf("温度二:%d\n\n",send_TEMP_2);

	
	//printf("send_NH3:%d\n",send_NH3);
//	UART3_Puts("AT+MQTTPUB=0,\"YKWL/%s\",0,0,0,%d,\"%s\"\r\n",imei_no,str_len,send_data);//发布消息
//	delay_ms(10);
//	UART3_RxCounter = 0;
//	memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0
}
void mbh_hook_rec04(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec05(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec06(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec15(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec16(uint8_t add,uint8_t *data,uint8_t datalen)
{

}


void mbh_hook_timesErr(uint8_t add,uint8_t cmd)
{
	//printf("mbh_hook_timesErr\n");
	send_NH3 = 999;
}

