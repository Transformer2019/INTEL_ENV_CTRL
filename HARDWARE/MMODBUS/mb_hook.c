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


void mbh_hook_rec01(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec02(uint8_t add,uint8_t *data,uint8_t datalen)
{

}
void mbh_hook_rec03(uint8_t add,uint8_t *data,uint8_t datalen)
{
	static uint8_t send485_flag=0;
    //if(mqtt_flag && send485_flag==11){send485_flag=0;}
	send485_flag++;
	if(send485_flag==10)send485_flag=0;
	send_NH3 = *(data+2);
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

