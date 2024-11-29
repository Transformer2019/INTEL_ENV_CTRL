#include "stm32f10x.h"
#include "74HC595.h"
#include "delay.h"

void delay(uint16_t t);
/********  74HC595 GPIO 配置 *************************/
void HC595_GPIO_Config(void)
{		
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(SHCP_GPIO_CLK|STCP_GPIO_CLK|DS_GPIO_CLK, ENABLE);

	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
		GPIO_InitStructure.GPIO_Pin = SHCP_GPIO_PIN;
		GPIO_Init(SHCP_GPIO_PORT, &GPIO_InitStructure);   // 初始化 SHCP 引脚
	

	
		GPIO_InitStructure.GPIO_Pin = STCP_GPIO_PIN;
		GPIO_Init(STCP_GPIO_PORT, &GPIO_InitStructure);   // 初始化 STCP 引脚
	
		GPIO_InitStructure.GPIO_Pin = DS_GPIO_PIN;
		GPIO_Init(DS_GPIO_PORT, &GPIO_InitStructure);     // 初始化 DS   引脚
	

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*使能SWD 禁用JTAG*/
        //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

		GPIO_InitStructure.GPIO_Pin = OE_GPIO_PIN;
		GPIO_Init(OE_GPIO_PORT, &GPIO_InitStructure);   // 初始化 OE 引脚
		
		GPIO_ResetBits(SHCP_GPIO_PORT, SHCP_GPIO_PIN);      // 引脚初始状态为高，便于产生上升沿
		GPIO_ResetBits(STCP_GPIO_PORT, STCP_GPIO_PIN);	 
		GPIO_ResetBits(DS_GPIO_PORT, DS_GPIO_PIN);
		GPIO_ResetBits(OE_GPIO_PORT, OE_GPIO_PIN);
}

/***
 *74HC595 发送一个字节 
 *即往74HC595的DS引脚发送一个字节
*/
void HC595_Send_Byte(u8 byte)
{
	u8 i;
	for (i = 0; i < 8; i ++)  //一个字节8位，传输8次，一次一位，循环8次，刚好移完8位
	{
        // 适当延时
	  /****  步骤1：将数据传到DS引脚    ****/
		if (byte & 0x80){        //先传输高位，通过与运算判断第八是否为1
			HC595_Data_High();    //如果第八位是1，则与 595 DS连接的引脚输出高电平
		//printf("h-\n");
		}
		else {                  //否则输出低电平
			HC595_Data_Low();
				//printf("-\n");
		}
		/*** 步骤2：SHCP每产生一个上升沿，当前的bit就被送入移位寄存器 ***/
		HC595_SHCP_Low();   // SHCP拉低
		
		delay_ms(1);   
		HC595_SHCP_High();  // SHCP拉高， SHCP产生上升沿
		delay_ms(1);   
		//delay(100);
		
		byte <<= 1;		// 左移一位，将低位往高位移，通过	if (byte & 0x80)判断低位是否为1
	}

}

/**
 *74HC595输出锁存 使能 
**/
void HC595_CS(void) 
{
	/**  步骤3：STCP产生一个上升沿，移位寄存器的数据移入存储寄存器  **/
	HC595_STCP_Low();   // 将STCP拉低
	//delay(1);           // 适当延时
	delay_us(100);   
	HC595_STCP_High();  // 再将STCP拉高，STCP即可产生一个上升沿
	//delay(1);
	delay_us(100); 
}

/**
 *发送多个字节
 *便于级联时数据的发送
 *级联N级，就需要发送N个字节控制HC595
***/
void HC595_Send_Multi_Byte(u8 *data, u16 len)
{
    //GPIO_ResetBits(OE_GPIO_PORT, OE_GPIO_PIN);
	u8 i;
	for(i = 0; i < len; i++) // len 个字节
	{
		HC595_Send_Byte(data[i]);
		//printf("data[%d]:%x\n", i, data[i]);
	}
	HC595_CS(); //先把所有字节发送完，再使能输出
	//delay(100);
	delay_ms(1); 
	//GPIO_SetBits(OE_GPIO_PORT, OE_GPIO_PIN);
	//printf("ffffffffffffff\n");
}

void delay(uint16_t t)
{
	for(; t!=0; t--);
}
