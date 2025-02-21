#ifndef __UART3_H
#define __UART3_H

#include "stm32f10x.h"
#include "sys.h"

#define RXBUFF_SIZE		1024		//指定接收MQTT数据缓冲区的大小
//存放从服务器发送来的报文数据,注意：缓冲区第一个字节存放数据的长度：数据长度+数据信息
extern char MQTT_RxDataBuf[RXBUFF_SIZE];


#define UART3_TXBUFF_SIZE		1024		//定义串口3发送缓冲区大小
#define UART3_RXBUFF_SIZE		1024		//定义串口3接收缓冲区大小


extern u32 UART3_RxCounter; //指定串口3接收数据计数
extern char UART3_RxBuff[UART3_RXBUFF_SIZE]; //指定串口3接收缓冲区
extern char UART3_TxBuff[UART3_TXBUFF_SIZE]; //指定串口3发送缓冲区

extern void UART3_Init(void); //初始化串口3函数
extern void UART3_Puts(char *, ...); //串口3发送数据可变参函数
extern void UART3DMA_Init(void); //串口3的DMA初始化函数
extern void UART3_TxData(unsigned char *data); //串口3发送MQTT数据报文函数

#endif
