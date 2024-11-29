#include "stm32f10x.h"
#include "74HC595.h"
#include "delay.h"

void delay(uint16_t t);
/********  74HC595 GPIO ���� *************************/
void HC595_GPIO_Config(void)
{		
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(SHCP_GPIO_CLK|STCP_GPIO_CLK|DS_GPIO_CLK, ENABLE);

	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
		GPIO_InitStructure.GPIO_Pin = SHCP_GPIO_PIN;
		GPIO_Init(SHCP_GPIO_PORT, &GPIO_InitStructure);   // ��ʼ�� SHCP ����
	

	
		GPIO_InitStructure.GPIO_Pin = STCP_GPIO_PIN;
		GPIO_Init(STCP_GPIO_PORT, &GPIO_InitStructure);   // ��ʼ�� STCP ����
	
		GPIO_InitStructure.GPIO_Pin = DS_GPIO_PIN;
		GPIO_Init(DS_GPIO_PORT, &GPIO_InitStructure);     // ��ʼ�� DS   ����
	

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*ʹ��SWD ����JTAG*/
        //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

		GPIO_InitStructure.GPIO_Pin = OE_GPIO_PIN;
		GPIO_Init(OE_GPIO_PORT, &GPIO_InitStructure);   // ��ʼ�� OE ����
		
		GPIO_ResetBits(SHCP_GPIO_PORT, SHCP_GPIO_PIN);      // ���ų�ʼ״̬Ϊ�ߣ����ڲ���������
		GPIO_ResetBits(STCP_GPIO_PORT, STCP_GPIO_PIN);	 
		GPIO_ResetBits(DS_GPIO_PORT, DS_GPIO_PIN);
		GPIO_ResetBits(OE_GPIO_PORT, OE_GPIO_PIN);
}

/***
 *74HC595 ����һ���ֽ� 
 *����74HC595��DS���ŷ���һ���ֽ�
*/
void HC595_Send_Byte(u8 byte)
{
	u8 i;
	for (i = 0; i < 8; i ++)  //һ���ֽ�8λ������8�Σ�һ��һλ��ѭ��8�Σ��պ�����8λ
	{
        // �ʵ���ʱ
	  /****  ����1�������ݴ���DS����    ****/
		if (byte & 0x80){        //�ȴ����λ��ͨ���������жϵڰ��Ƿ�Ϊ1
			HC595_Data_High();    //����ڰ�λ��1������ 595 DS���ӵ���������ߵ�ƽ
		//printf("h-\n");
		}
		else {                  //��������͵�ƽ
			HC595_Data_Low();
				//printf("-\n");
		}
		/*** ����2��SHCPÿ����һ�������أ���ǰ��bit�ͱ�������λ�Ĵ��� ***/
		HC595_SHCP_Low();   // SHCP����
		
		delay_ms(1);   
		HC595_SHCP_High();  // SHCP���ߣ� SHCP����������
		delay_ms(1);   
		//delay(100);
		
		byte <<= 1;		// ����һλ������λ����λ�ƣ�ͨ��	if (byte & 0x80)�жϵ�λ�Ƿ�Ϊ1
	}

}

/**
 *74HC595������� ʹ�� 
**/
void HC595_CS(void) 
{
	/**  ����3��STCP����һ�������أ���λ�Ĵ�������������洢�Ĵ���  **/
	HC595_STCP_Low();   // ��STCP����
	//delay(1);           // �ʵ���ʱ
	delay_us(100);   
	HC595_STCP_High();  // �ٽ�STCP���ߣ�STCP���ɲ���һ��������
	//delay(1);
	delay_us(100); 
}

/**
 *���Ͷ���ֽ�
 *���ڼ���ʱ���ݵķ���
 *����N��������Ҫ����N���ֽڿ���HC595
***/
void HC595_Send_Multi_Byte(u8 *data, u16 len)
{
    //GPIO_ResetBits(OE_GPIO_PORT, OE_GPIO_PIN);
	u8 i;
	for(i = 0; i < len; i++) // len ���ֽ�
	{
		HC595_Send_Byte(data[i]);
		//printf("data[%d]:%x\n", i, data[i]);
	}
	HC595_CS(); //�Ȱ������ֽڷ����꣬��ʹ�����
	//delay(100);
	delay_ms(1); 
	//GPIO_SetBits(OE_GPIO_PORT, OE_GPIO_PIN);
	//printf("ffffffffffffff\n");
}

void delay(uint16_t t)
{
	for(; t!=0; t--);
}
