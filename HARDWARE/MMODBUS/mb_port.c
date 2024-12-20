/**
  ******************************************************************************
  * @file    mb_port.c
  * @author  Derrick Wang
  * @brief   modebus移植接口
  ******************************************************************************
  * @note
  * 该文件为modbus移植接口的实现，根据不同的MCU平台进行移植
  ******************************************************************************
  */


#include "mb_include.h"
//#include "tim.h"
//#include "usart.h"
//#include "gpio.h"


void mb_port_uartInit(uint32_t ulBaudRate,uint8_t eParity)
{
	/*串口部分初始化*/
//	huart1.Instance = USART1;
//	huart1.Init.BaudRate = baud;
//	huart1.Init.WordLength = UART_WORDLENGTH_8B;
//	if(parity==MB_PARITY_ODD)
//	{
//		huart1.Init.StopBits = UART_STOPBITS_1;
//		huart1.Init.Parity = UART_PARITY_ODD;	
//	}
//	else if(parity==MB_PARITY_EVEN)
//	{
//		huart1.Init.StopBits = UART_STOPBITS_1;
//		huart1.Init.Parity = UART_PARITY_EVEN;
//	}
//	else
//	{
//		huart1.Init.StopBits = UART_STOPBITS_2;
//		huart1.Init.Parity = UART_PARITY_NONE;	
//	}
//	huart1.Init.Mode = UART_MODE_TX_RX;
//	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
//	if (HAL_UART_Init(&huart1) != HAL_OK)
//	{
//	Error_Handler();
//	}	
	
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PB1端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
	GPIO_Init(GPIOB, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,ENABLE);//复位串口2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,DISABLE);//停止复位
	
	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = ulBaudRate;//波特率设置
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	switch(eParity)
	{
		case 0:
		{
			USART_InitStructure.USART_Parity =USART_Parity_No;
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
			USART_ITConfig(USART3, USART_IT_PE, DISABLE);//关闭奇偶校验错中断
			break;
		}
		case 1: 
		{
			USART_InitStructure.USART_Parity =USART_Parity_Odd;
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
			USART_ITConfig(USART3, USART_IT_PE, ENABLE);//使能奇偶校验错中断
			break;
		}
		case 2:
		{
			USART_InitStructure.USART_Parity =USART_Parity_Even;
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
			USART_ITConfig(USART3, USART_IT_PE, ENABLE);//使能奇偶校验错中断
			break;
		}
		default:break;
	}
	USART_Init(USART3, &USART_InitStructure); //初始化串口2
	USART_Cmd(USART3, ENABLE);  //使能串口 2
	
	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
}

void mb_port_uartEnable(uint8_t xTxEnable,uint8_t xRxEnable)
{
//	if(txen)
//	{
//		__HAL_UART_ENABLE_IT(&huart1,UART_IT_TXE);
//	}
//	else
//	{
//		__HAL_UART_DISABLE_IT(&huart1,UART_IT_TXE);
//	}
//	
//	if(rxen)
//	{
//		__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
//	}
//	else
//	{
//		__HAL_UART_DISABLE_IT(&huart1,UART_IT_RXNE);
//	}	


	    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	//STM32串口 接收中断使能
	if(xRxEnable==1)   
	{   
		//使能接收和接收中断  
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);//RS485驱动芯片，接收使能
	} 
	else 
	{
		//禁止接收和接收中断    
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
		GPIO_SetBits(GPIOB,GPIO_Pin_1);//RS485驱动芯片，发送使能
	}
	//STM32串口 发送中断使能  
	if(xTxEnable==1)   
	{  
		//使能发送完成中断  
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);  
	}   
	else  
	{  
		//禁止发送完成中断  
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);  
	}  
}
void mb_port_putchar(uint8_t ch)
{
//	huart1.Instance->DR = ch;  //直接操作寄存器比HAL封装的更高效
	USART_SendData(USART3, ch);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}

void mb_port_getchar(uint8_t *ch)
{
//	*ch= (uint8_t)(huart1.Instance->DR & (uint8_t)0x00FF);	
	*ch = USART_ReceiveData(USART3);
}

void mb_port_timerInit(uint32_t baud)
{
//	/*定时器部分初始化*/
//	htim3.Instance = TIM3;
//	htim3.Init.Prescaler = 71;
//	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
//	if(baud>19200)   //波特率大于19200固定使用1800作为3.5T
//	{
//		htim3.Init.Period = 1800;
//	}
//	else   //其他波特率的需要根据计算
//	{
//		/*	us=1s/(baud/11)*1000000*3.5
//		*			=(11*1000000*3.5)/baud
//		*			=38500000/baud
//		*/
//		htim3.Init.Period = (uint32_t)38500000/baud;
//	
//	}
//	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
//	{
//	Error_Handler();
//	}		
    uint16_t Tim1Timerout50us;

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	uint16_t PrescalerValue = 0;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//使能TIM4时钟
	
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 20000) - 1; // 1/20000=50us 
	
	if(baud>19200)   //波特率大于19200固定使用1800作为3.5T
	{
		Tim1Timerout50us = 1800;
	}
	else   //其他波特率的需要根据计算
	{
		/*	us=1s/(baud/11)*1000000*3.5
		*			=(11*1000000*3.5)/baud
		*			=38500000/baud
		*/
		Tim1Timerout50us = (uint32_t)38500000/baud;//8020
	
	}
	
	TIM_TimeBaseInitStructure.TIM_Period=Tim1Timerout50us;   //自动装载值,8.02ms
	TIM_TimeBaseInitStructure.TIM_Prescaler=71; //分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;//定时器中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update,DISABLE);
	TIM_Cmd(TIM4,DISABLE);
}

void mb_port_timerEnable()
{
//	__HAL_TIM_DISABLE(&htim3);
//	__HAL_TIM_CLEAR_IT(&htim3,TIM_IT_UPDATE);                //清除中断位
//	__HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE);                //使能中断位
//	__HAL_TIM_SET_COUNTER(&htim3,0);                           //设置定时器计数为0
//	__HAL_TIM_ENABLE(&htim3);                                 //使能定时器
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM4,0x0000); 
	TIM_Cmd(TIM4, ENABLE);
}

void mb_port_timerDisable()
{
//	__HAL_TIM_DISABLE(&htim3);
//	__HAL_TIM_SET_COUNTER(&htim3,0); 
//	__HAL_TIM_DISABLE_IT(&htim3,TIM_IT_UPDATE);
//	__HAL_TIM_CLEAR_IT(&htim3,TIM_IT_UPDATE);
	TIM_Cmd(TIM4, DISABLE);
	TIM_SetCounter(TIM4,0x0000); 
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
}

//串口中断服务函数
void USART3_IRQHandler()
{
//	HAL_NVIC_ClearPendingIRQ(USART1_IRQn);
//	if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE)!=RESET))
//	{
//		__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
//		mbh_uartRxIsr();
//	}
//	if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TXE)!=RESET))
//	{
//		__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_TXE);
//		mbh_uartTxIsr();
//	}



	NVIC_ClearPendingIRQ(USART3_IRQn);

	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);		
		mbh_uartRxIsr();//接受中断

	}
	if(USART_GetITStatus(USART3, USART_IT_TXE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_TXE);
		mbh_uartTxIsr();//发送完成中断

	}
	//溢出-如果发生溢出需要先读SR,再读DR寄存器 则可清除不断进入中断的问题
	if(USART_GetFlagStatus(USART3,USART_FLAG_ORE)==SET)
	{
		USART_ClearFlag(USART3,USART_FLAG_ORE);	//读SR
		USART_ReceiveData(USART3);				//读DR
	}	

}

//定时器中断服务函数
void TIM4_IRQHandler()
{
//	__HAL_TIM_CLEAR_IT(&htim3,TIM_IT_UPDATE);
//	mbh_timer3T5Isr();
	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
	{
		mbh_timer3T5Isr();
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	
}

