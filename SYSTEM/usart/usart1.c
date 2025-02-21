#include "usart1.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h" //�ɱ�κ���ʹ�õ�ͷ�ļ�,va_list

//���䴮��3���պͷ������ݵĻ�����
u32 UART3_RxCounter = 0; //��¼����3�������ݵĸ���
char UART3_RxBuff[UART3_RXBUFF_SIZE]; //������ջ�����
char UART3_TxBuff[UART3_TXBUFF_SIZE]; //���䷢�ͻ�����

char MQTT_RxDataBuf[RXBUFF_SIZE];


int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

//���崮��3��ʼ������ 
void UART3_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ָ��UART3�ĸ�λĬ��ֵ
	USART_DeInit(USART1);
	
	//ʹ��UART3��ʱ�ӺͶ�Ӧ��GPIO���ſ�������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	
	//��ʼ��GPIOB10������ΪUART3�ķ���TX����
//	GPIO_Config.GPIO_Pin = GPIO_Pin_10;
//	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP; //�����������,GPIOB10->UART3TX
//	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 
//	GPIO_Init(GPIOB, &GPIO_Config);
	
    //��ʼ��GPIOB11������ΪUART3�Ľ���RX����
//	GPIO_Config.GPIO_Pin = GPIO_Pin_11;
//	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING; //���븡��
//	GPIO_Init(GPIOB, &GPIO_Config);
	
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //�������PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //��������PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */
	
	
	//����UART3�������Ĺ�������
//	UART_Config.USART_BaudRate = 115200; //������
//	UART_Config.USART_WordLength = USART_WordLength_8b; //����λ8λ
//	UART_Config.USART_StopBits = USART_StopBits_1; //ֹͣλ1λ
//	UART_Config.USART_Parity = USART_Parity_No; //��У��
//	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //������Ӳ������
//	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�������պͷ���
//	USART_Init(USART3, &UART_Config);  //��ʼ��UART3��������
//	USART_Cmd(USART3, ENABLE); //��������3

	//USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = 115200;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
//	
//	USART_ClearFlag(USART1, USART_FLAG_TC);
		
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�


	//����UART3�����ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	

	

	
	//����NVIC֧�ִ���3���ж�
//	NVIC_Config.NVIC_IRQChannel = USART3_IRQn; //ָ������3�ж�
//	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0; //ָ����ռʽ���ȼ�
//	NVIC_Config.NVIC_IRQChannelSubPriority = 0; //ָ�������ȼ�
//	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //ʹ���ж�
//	NVIC_Init(&NVIC_Config);
	
	//����UART3��DMA���պͷ��͹���
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	
	//��ʼ��UART3��DMA��������
	UART3DMA_Init();
}

//�����ʼ��UART3��DMA��������
void UART3DMA_Init(void) {
	DMA_InitTypeDef DMA_Config;
	NVIC_InitTypeDef NVIC_Config;
	
	//ʹ��DMA1��ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//����DMA1��ͨ��2��Ϊ����3�ķ������ݹ���
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART1->DR; //ָ��DMA���ƶ�Ӧ�Ĵ���1���ݼĴ����Ļ���ַ
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART3_TxBuff; //ָ��DMA���ƶ�Ӧ���ڴ����ַ
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralDST; //ָ��DMA���Ƶķ����ڴ�->�Ĵ���
	DMA_Config.DMA_BufferSize = UART3_TXBUFF_SIZE; //ָ��DMA�������Ĵ�С
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //ָ��UART1���ݼĴ�����ַ������
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable; //ָ���ڴ�ĵ�ַ����
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //ָ��UART1���ݼĴ������ݿ��8λ
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //ָ���ڴ����ݿ��8λ
	DMA_Config.DMA_Mode = DMA_Mode_Normal; //ָ��DMA�Ĺ���ģʽΪ��ͨģʽ
	DMA_Config.DMA_Priority = DMA_Priority_Medium; //ָ��DMA1ͨ��4�����ȼ�Ϊ�е����ȼ�
	DMA_Config.DMA_M2M = DMA_M2M_Disable; //ָ��DMA�������ڴ浽�ڴ�ģʽ
	DMA_Init(DMA1_Channel4, &DMA_Config); //����DMA1��ͨ��4�Ĺ�������
	
	//����DMA1ͨ��3��Ϊ����3�Ľӿ�RX����
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART1->DR; //ָ��DMA���ƶ�Ӧ�Ĵ���1���ݼĴ����Ļ���ַ
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART3_RxBuff; //ָ��DMA���ƶ�Ӧ���ڴ����ַ
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralSRC; //ָ��DMA���Ƶķ���UART1���ݼĴ���->�ڴ�
	DMA_Config.DMA_BufferSize = UART3_RXBUFF_SIZE; //ָ��DMA�������Ĵ�С
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //ָ��UART1���ݼĴ�����ַ������
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable; //ָ���ڴ�ĵ�ַ����
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //ָ��UART1���ݼĴ������ݿ��8λ
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //ָ���ڴ����ݿ��8λ
	DMA_Config.DMA_Mode = DMA_Mode_Normal; //ָ��DMA�Ĺ���ģʽΪ��ͨģʽ
	DMA_Config.DMA_Priority = DMA_Priority_High; //ָ��DMA1ͨ��4�����ȼ�Ϊ�����ȼ�
	DMA_Config.DMA_M2M = DMA_M2M_Disable; //ָ��DMA�������ڴ浽�ڴ�ģʽ
	DMA_Init(DMA1_Channel5, &DMA_Config); //����DMA1��ͨ��4�Ĺ�������
	DMA_Cmd(DMA1_Channel5, ENABLE); //����DMA1ͨ��5�Ľ������ݹ���
	
  //ʹ��DMA1ͨ��2��������жϹ���
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	
	//����NVIC��DMA1��ͨ��4�жϹ���
	NVIC_Config.NVIC_IRQChannel = DMA1_Channel4_IRQn; //ָ��DMA1ͨ��4���жϹ���,��ΪNVIC�������ж��ź�
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0; //ָ����ռʽ���ȼ�
	NVIC_Config.NVIC_IRQChannelSubPriority = 0; //ָ�������ȼ�
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //ʹ��DMA1ͨ��4�жϹ���
	NVIC_Init(&NVIC_Config);
}

//����UART3�������ݿɱ�κ���
void UART3_Puts(char *fmt, ...) {
	//Ŀ�꽫Ҫ���͵Ŀɱ��������Ϣ�����һ���ַ������浽����3���ͻ������У�������DMA����
	va_list ap;
	va_start(ap, fmt);
	vsprintf(UART3_TxBuff, fmt, ap);
	va_end(ap);
	//����DMA1ͨ��2��������
	DMA_Cmd(DMA1_Channel4, DISABLE); //�ȹر�
	
	size_t UART3_TxBuff_len = strlen(UART3_TxBuff);
	DMA_SetCurrDataCounter(DMA1_Channel4, UART3_TxBuff_len); //����DMA�������ݵĳ���
	
	DMA_Cmd(DMA1_Channel4, ENABLE); //����DMA�������ݣ����ͻ������еĸ���ATָ���ַ��������ķ��ͳ�ȥ
}

//����ͨ��ESP8266����MQTT���ĵĺ���
void UART3_TxData(unsigned char *data) {
	//MQTT���ͻ�����MQTT_TxBuf��ǰ�����ֽڷֱ�洢���ĵĸ��ֽں͵��ֽ�
	//�ӵ������ֽڿ�ʼ��ԭʼ��������,�����Ȼ�ȡ���ĵĳ���
	int len = (data[0] << 8) + data[1];
	//����DMA1ͨ��2���ͱ�������
	DMA_Cmd(DMA1_Channel4, DISABLE); //�ȹر�
	DMA1_Channel4->CMAR = (unsigned int)(&data[2]); //����DMA�������͵�MQTT�������ڵ�
													//�ڴ��׵�ַ�ӷ��ͻ������ĵ������ֽڿ�ʼ���а���
	DMA_SetCurrDataCounter(DMA1_Channel4, len); //���÷��͵����ݳ���
	DMA_Cmd(DMA1_Channel4, ENABLE); //����DMA��ʼ���Ʒ���������
}

//���崮��3�жϴ�����
void USART1_IRQHandler(void) {
	//�ж��Ƿ��Ǵ���3�Ľ��տ����ж�
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
		//���ж�
		USART1->SR;
		USART1->DR; 
		//����˴δ��ڽ��յ����ݸ���
		UART3_RxCounter = UART3_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
		//�����mqtt�������������������ݣ������ڽ��ջ������е����ݿ�����MQTT���ջ�������
		if(strstr(UART3_RxBuff,"publish")!=NULL){
			memcpy(MQTT_RxDataBuf, UART3_RxBuff, UART3_RxCounter);
			UART3_RxCounter = 0; //���µȴ�������һ��������Ϣ
			memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //������3���ջ�������0	
		}
		
		//UART3_RxBuff[UART3_RxCounter - 2] = '\0';
//		printf("Return message is :%s\n", UART3_RxBuff);	
		//�������ý��յ����ݳ���
		DMA_Cmd(DMA1_Channel5, DISABLE); //�ȹر�
		DMA1_Channel5->CNDTR = UART3_TXBUFF_SIZE; //�����µĳ���
		DMA_Cmd(DMA1_Channel5, ENABLE); //ʹ��DMA���ڽ��� 	
	}
}

//����DMA1ͨ��2���ݰ�������ж�
void DMA1_Channel4_IRQHandler(void) {
	//�ж��Ƿ���DMA1ͨ��2���ݰ�����ɲ������ж�
	if(DMA_GetITStatus(DMA1_IT_TC4) != RESET) {
		//���ж�
		DMA_ClearITPendingBit(DMA1_IT_TC4);
		//�ر�DMA1ͨ��2,������Ҫ���������ٴ򿪣��μ�UART3_Puts
		DMA_Cmd(DMA1_Channel4, DISABLE);
	}
}


