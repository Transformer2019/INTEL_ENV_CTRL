#ifndef __UART3_H
#define __UART3_H

#include "stm32f10x.h"
#include "sys.h"

#define RXBUFF_SIZE		1024		//ָ������MQTT���ݻ������Ĵ�С
//��Ŵӷ������������ı�������,ע�⣺��������һ���ֽڴ�����ݵĳ��ȣ����ݳ���+������Ϣ
extern char MQTT_RxDataBuf[RXBUFF_SIZE];


#define UART3_TXBUFF_SIZE		1024		//���崮��3���ͻ�������С
#define UART3_RXBUFF_SIZE		1024		//���崮��3���ջ�������С


extern u32 UART3_RxCounter; //ָ������3�������ݼ���
extern char UART3_RxBuff[UART3_RXBUFF_SIZE]; //ָ������3���ջ�����
extern char UART3_TxBuff[UART3_TXBUFF_SIZE]; //ָ������3���ͻ�����

extern void UART3_Init(void); //��ʼ������3����
extern void UART3_Puts(char *, ...); //����3�������ݿɱ�κ���
extern void UART3DMA_Init(void); //����3��DMA��ʼ������
extern void UART3_TxData(unsigned char *data); //����3����MQTT���ݱ��ĺ���

#endif
