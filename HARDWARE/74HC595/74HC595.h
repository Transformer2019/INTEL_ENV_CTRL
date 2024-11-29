#ifndef __74HC595_H__
#define __74HC595_H__
#include "stm32f10x.h"

#define SHCP_GPIO_PORT      GPIOA			              
#define SHCP_GPIO_CLK 	    RCC_APB2Periph_GPIOA		
#define SHCP_GPIO_PIN	    GPIO_Pin_12		       

#define STCP_GPIO_PORT      GPIOA		              
#define STCP_GPIO_CLK 	    RCC_APB2Periph_GPIOA		
#define STCP_GPIO_PIN	    GPIO_Pin_11		        

#define DS_GPIO_PORT        GPIOB			              
#define DS_GPIO_CLK 	    RCC_APB2Periph_GPIOB		
#define DS_GPIO_PIN	        GPIO_Pin_5	

#define OE_GPIO_PORT        GPIOA			              
#define OE_GPIO_CLK 	    RCC_APB2Periph_GPIOA		
#define OE_GPIO_PIN	        GPIO_Pin_15	



#define HC595_SHCP_Low()      GPIO_ResetBits(SHCP_GPIO_PORT, SHCP_GPIO_PIN)
#define HC595_SHCP_High()     GPIO_SetBits(SHCP_GPIO_PORT, SHCP_GPIO_PIN)
#define HC595_STCP_Low()      GPIO_ResetBits(STCP_GPIO_PORT, STCP_GPIO_PIN)
#define HC595_STCP_High()     GPIO_SetBits(STCP_GPIO_PORT, STCP_GPIO_PIN)
#define HC595_Data_Low()      GPIO_ResetBits(DS_GPIO_PORT, DS_GPIO_PIN)
#define HC595_Data_High()     GPIO_SetBits(DS_GPIO_PORT, DS_GPIO_PIN)

#define HC595_OE_Low()      GPIO_ResetBits(OE_GPIO_PORT, OE_GPIO_PIN)
#define HC595_OE_High()     GPIO_SetBits(OE_GPIO_PORT, OE_GPIO_PIN)

void HC595_GPIO_Config(void);
void HC595_Send_Byte(u8 byte);
void HC595_CS(void);
void HC595_Send_Multi_Byte(u8 *data, u16 len);

#endif

