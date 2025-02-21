#include "usart1.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h" //可变参函数使用的头文件,va_list

//分配串口3接收和发送数据的缓冲区
u32 UART3_RxCounter = 0; //记录串口3接收数据的个数
char UART3_RxBuff[UART3_RXBUFF_SIZE]; //分配接收缓冲区
char UART3_TxBuff[UART3_TXBUFF_SIZE]; //分配发送缓冲区

char MQTT_RxDataBuf[RXBUFF_SIZE];


int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

//定义串口3初始化函数 
void UART3_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//指定UART3的复位默认值
	USART_DeInit(USART1);
	
	//使能UART3的时钟和对应的GPIO引脚控制器的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	
	//初始化GPIOB10引脚作为UART3的发送TX功能
//	GPIO_Config.GPIO_Pin = GPIO_Pin_10;
//	GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出,GPIOB10->UART3TX
//	GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz; 
//	GPIO_Init(GPIOB, &GPIO_Config);
	
    //初始化GPIOB11引脚作为UART3的接收RX功能
//	GPIO_Config.GPIO_Pin = GPIO_Pin_11;
//	GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING; //输入浮空
//	GPIO_Init(GPIOB, &GPIO_Config);
	
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //串口输出PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //串口输入PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //模拟输入
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* 初始化GPIO */
	
	
	//配置UART3控制器的工作参数
//	UART_Config.USART_BaudRate = 115200; //波特率
//	UART_Config.USART_WordLength = USART_WordLength_8b; //数据位8位
//	UART_Config.USART_StopBits = USART_StopBits_1; //停止位1位
//	UART_Config.USART_Parity = USART_Parity_No; //不校验
//	UART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //不采用硬件流控
//	UART_Config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //启动接收和发送
//	USART_Init(USART3, &UART_Config);  //初始化UART3工作参数
//	USART_Cmd(USART3, ENABLE); //启动串口3

	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = 115200;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
//	
//	USART_ClearFlag(USART1, USART_FLAG_TC);
		
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断


	//启动UART3空闲中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	

	

	
	//配置NVIC支持串口3的中断
//	NVIC_Config.NVIC_IRQChannel = USART3_IRQn; //指定串口3中断
//	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0; //指定抢占式优先级
//	NVIC_Config.NVIC_IRQChannelSubPriority = 0; //指定子优先级
//	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //使能中断
//	NVIC_Init(&NVIC_Config);
	
	//开启UART3的DMA接收和发送功能
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	
	//初始化UART3的DMA工作参数
	UART3DMA_Init();
}

//定义初始化UART3的DMA工作参数
void UART3DMA_Init(void) {
	DMA_InitTypeDef DMA_Config;
	NVIC_InitTypeDef NVIC_Config;
	
	//使能DMA1的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//配置DMA1的通道2作为串口3的发送数据功能
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART1->DR; //指定DMA搬移对应的串口1数据寄存器的基地址
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART3_TxBuff; //指定DMA搬移对应的内存基地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralDST; //指定DMA搬移的方向，内存->寄存器
	DMA_Config.DMA_BufferSize = UART3_TXBUFF_SIZE; //指定DMA缓冲区的大小
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //指定UART1数据寄存器地址不增加
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable; //指定内存的地址增加
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //指定UART1数据寄存器数据宽度8位
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //指定内存数据宽度8位
	DMA_Config.DMA_Mode = DMA_Mode_Normal; //指定DMA的工作模式为普通模式
	DMA_Config.DMA_Priority = DMA_Priority_Medium; //指定DMA1通道4的优先级为中等优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable; //指定DMA不采用内存到内存模式
	DMA_Init(DMA1_Channel4, &DMA_Config); //配置DMA1的通道4的工作参数
	
	//配置DMA1通道3作为串口3的接口RX功能
	DMA_Config.DMA_PeripheralBaseAddr = (u32)&USART1->DR; //指定DMA搬移对应的串口1数据寄存器的基地址
	DMA_Config.DMA_MemoryBaseAddr = (u32)UART3_RxBuff; //指定DMA搬移对应的内存基地址
	DMA_Config.DMA_DIR = DMA_DIR_PeripheralSRC; //指定DMA搬移的方向，UART1数据寄存器->内存
	DMA_Config.DMA_BufferSize = UART3_RXBUFF_SIZE; //指定DMA缓冲区的大小
	DMA_Config.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //指定UART1数据寄存器地址不增加
	DMA_Config.DMA_MemoryInc = DMA_MemoryInc_Enable; //指定内存的地址增加
	DMA_Config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //指定UART1数据寄存器数据宽度8位
	DMA_Config.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //指定内存数据宽度8位
	DMA_Config.DMA_Mode = DMA_Mode_Normal; //指定DMA的工作模式为普通模式
	DMA_Config.DMA_Priority = DMA_Priority_High; //指定DMA1通道4的优先级为高优先级
	DMA_Config.DMA_M2M = DMA_M2M_Disable; //指定DMA不采用内存到内存模式
	DMA_Init(DMA1_Channel5, &DMA_Config); //配置DMA1的通道4的工作参数
	DMA_Cmd(DMA1_Channel5, ENABLE); //启动DMA1通道5的接收数据功能
	
  //使能DMA1通道2发送完成中断功能
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	
	//配置NVIC的DMA1的通道4中断功能
	NVIC_Config.NVIC_IRQChannel = DMA1_Channel4_IRQn; //指定DMA1通道4的中断功能,作为NVIC的输入中断信号
	NVIC_Config.NVIC_IRQChannelPreemptionPriority = 0; //指定抢占式优先级
	NVIC_Config.NVIC_IRQChannelSubPriority = 0; //指定子优先级
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE; //使能DMA1通道4中断功能
	NVIC_Init(&NVIC_Config);
}

//定义UART3发送数据可变参函数
void UART3_Puts(char *fmt, ...) {
	//目标将要发送的可变参数据信息构造成一个字符串保存到串口3发送缓冲区中，将来让DMA发送
	va_list ap;
	va_start(ap, fmt);
	vsprintf(UART3_TxBuff, fmt, ap);
	va_end(ap);
	//启动DMA1通道2发送数据
	DMA_Cmd(DMA1_Channel4, DISABLE); //先关闭
	
	size_t UART3_TxBuff_len = strlen(UART3_TxBuff);
	DMA_SetCurrDataCounter(DMA1_Channel4, UART3_TxBuff_len); //设置DMA发送数据的长度
	
	DMA_Cmd(DMA1_Channel4, ENABLE); //启动DMA发送数据，发送缓冲区中的各种AT指令字符串哗哗的发送出去
}

//定义通过ESP8266发送MQTT报文的函数
void UART3_TxData(unsigned char *data) {
	//MQTT发送缓冲区MQTT_TxBuf中前两个字节分别存储报文的高字节和低字节
	//从第三个字节开始是原始报文数据,所以先获取报文的长度
	int len = (data[0] << 8) + data[1];
	//利用DMA1通道2发送报文数据
	DMA_Cmd(DMA1_Channel4, DISABLE); //先关闭
	DMA1_Channel4->CMAR = (unsigned int)(&data[2]); //告诉DMA将来发送的MQTT报文所在的
													//内存首地址从发送缓冲区的第三个字节开始进行搬移
	DMA_SetCurrDataCounter(DMA1_Channel4, len); //设置发送的数据长度
	DMA_Cmd(DMA1_Channel4, ENABLE); //启动DMA开始搬移发送数据了
}

//定义串口3中断处理函数
void USART1_IRQHandler(void) {
	//判断是否是串口3的接收空闲中断
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
		//清中断
		USART1->SR;
		USART1->DR; 
		//计算此次串口接收的数据个数
		UART3_RxCounter = UART3_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
		//如果是mqtt服务器发布过来的数据，将串口接收缓冲区中的数据拷贝到MQTT接收缓冲区中
		if(strstr(UART3_RxBuff,"publish")!=NULL){
			memcpy(MQTT_RxDataBuf, UART3_RxBuff, UART3_RxCounter);
			UART3_RxCounter = 0; //重新等待接收下一个推送消息
			memset(UART3_RxBuff, 0, UART3_RXBUFF_SIZE); //将串口3接收缓冲区清0	
		}
		
		//UART3_RxBuff[UART3_RxCounter - 2] = '\0';
//		printf("Return message is :%s\n", UART3_RxBuff);	
		//重新设置接收的数据长度
		DMA_Cmd(DMA1_Channel5, DISABLE); //先关闭
		DMA1_Channel5->CNDTR = UART3_TXBUFF_SIZE; //设置新的长度
		DMA_Cmd(DMA1_Channel5, ENABLE); //使能DMA串口接收 	
	}
}

//定义DMA1通道2数据搬移完成中断
void DMA1_Channel4_IRQHandler(void) {
	//判断是否是DMA1通道2数据搬移完成产生的中断
	if(DMA_GetITStatus(DMA1_IT_TC4) != RESET) {
		//清中断
		DMA_ClearITPendingBit(DMA1_IT_TC4);
		//关闭DMA1通道2,将来需要发送数据再打开，参见UART3_Puts
		DMA_Cmd(DMA1_Channel4, DISABLE);
	}
}


