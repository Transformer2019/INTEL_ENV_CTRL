/****************************************************************************************************
//=========================================电源接线================================================//
//     LCD模块                STM32单片机
//      VCC          接        DC5V/3.3V      //电源
//      GND          接          GND          //电源地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为SPI总线
//     LCD模块                STM32单片机    
//    SDI(MOSI)      接          PB15         //液晶屏SPI总线数据写信号
//    SDO(MISO)      接          PB14         //液晶屏SPI总线数据读信号，如果不需要读，可以不接线
//=======================================液晶屏控制线接线==========================================//
//     LCD模块 					      STM32单片机 
//       LED         接          PB9          //液晶屏背光控制信号，如果不需要控制，接5V或3.3V
//       SCK         接          PB13         //液晶屏SPI总线时钟信号
//      DC/RS        接          PB10         //液晶屏数据/命令控制信号
//       RST         接          PB12         //液晶屏复位控制信号
//       CS          接          PB11         //液晶屏片选控制信号
//=========================================触摸屏触接线=========================================//
//如果模块不带触摸功能或者带有触摸功能，但是不需要触摸功能，则不需要进行触摸屏接线
//	   LCD模块                STM32单片机 
//      T_IRQ        接          PC10         //触摸屏触摸中断信号
//      T_DO         接          PC2          //触摸屏SPI总线读信号
//      T_DIN        接          PC3          //触摸屏SPI总线写信号
//      T_CS         接          PC13         //触摸屏片选控制信号
//      T_CLK        接          PC0          //触摸屏SPI总线时钟信号
**************************************************************************************************/	
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#include "lcd.h"
#include "stdlib.h"
#include "delay.h"	 
#include "SPI.h"

	   
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//画笔颜色,背景颜色
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u8 data)
{ 
   LCD_CS_CLR;    //注释后屏幕出现隔点画点现象，可能是spi写速度太快导致 
	 LCD_RS_CLR;	  
   SPI_WriteByte(SPI2,data);
//	while((SPI2->SR&SPI_I2S_FLAG_TXE)==RESET);		//等待发送区空	  
//	SPI2->DR=data;	 	//发送一个byte   
     LCD_CS_SET;
	
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u8 data)
{
   LCD_CS_CLR; //注释后屏幕出现隔点画点现象，可能是spi写速度太快导致 
	 LCD_RS_SET;
   SPI_WriteByte(SPI2,data);
//	while((SPI2->SR&SPI_I2S_FLAG_TXE)==RESET);		//等待发送区空	  
//	SPI2->DR=data;	 	//发送一个byte   
   LCD_CS_SET;

}

void LCD_WR_DATA16(u16 data)
{
   LCD_CS_CLR;
	 LCD_RS_SET;
   SPI_WriteByte(SPI2,data>>8);
   SPI_WriteByte(SPI2,data);
   LCD_CS_SET;

}


/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(u16 Data)
{	
  //原代码	
//	LCD_WR_DATA((Data>>8)&0xF8);//RED
//	LCD_WR_DATA((Data>>3)&0xFC);//GREEN
//	LCD_WR_DATA(Data<<3);//BLUE
	
	//优化刷屏率1
//	LCD_RS_SET;
//    SPI_WriteByte(SPI2,(Data>>8)&0xF8);
//	SPI_WriteByte(SPI2,(Data>>3)&0xFC);
//	SPI_WriteByte(SPI2,Data<<3);
	
	//优化刷屏率2
	LCD_RS_SET;
//	while((SPI2->SR&SPI_I2S_FLAG_TXE)==RESET);		//等待发送区空	  
//	SPI2->DR=(Data>>8)&0xF8;	 	//发送一个byte  
//	while((SPI2->SR&SPI_I2S_FLAG_TXE)==RESET);		//等待发送区空	  
//	SPI2->DR=(Data>>3)&0xFC;	 	//发送一个byte  
//    while((SPI2->SR&SPI_I2S_FLAG_TXE)==RESET);		//等待发送区空	  
//	SPI2->DR=Data<<3;	 	//发送一个byte  
	
	//优化刷屏率3
	((SPI_TypeDef *)0x40003800)->DR=(Data>>8)&0xF8;	 	//发送一个byte  
	while(!((((SPI_TypeDef *)0x40003800)->SR)&0x0002));	//等待发送区空	  
	((SPI_TypeDef *)0x40003800)->DR=(Data>>3)&0xFC;	 	//发送一个byte  
	while(!((((SPI_TypeDef *)0x40003800)->SR)&0x0002));		//等待发送区空	  
	((SPI_TypeDef *)0x40003800)->DR=Data<<3;	 	//发送一个byte  
	
	
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None                                                                                                                         
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//设置光标位置 
	//Lcd_WriteData_16Bit(POINT_COLOR);
	//优化
	//while(!((((SPI_TypeDef *)0x40003800)->SR)&0x000                                     2));	//等待发送区空	  
	((SPI_TypeDef *)0x40003800)->DR=(POINT_COLOR>>8)&0xF8;	 	//发送一个byte  
	while(!((((SPI_TypeDef *)0x40003800)->SR)&0x0002));	//等待发送区空	  
	((SPI_TypeDef *)0x40003800)->DR=(POINT_COLOR>>3)&0xFC;	 	//发送一个byte  
	while(!((((SPI_TypeDef *)0x40003800)->SR)&0x0002));		//等待发送区空	  
	((SPI_TypeDef *)0x40003800)->DR=POINT_COLOR<<3;	 	//发送一个byte  
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(u16 Color)
{
    unsigned int i,m;  
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
	//LCD_CS_CLR;
	//PBout(12) = 0;
	LCD_RS_SET;
	for(i=0;i<lcddev.height*lcddev.width;i++)
	{
//    for(m=0;m<lcddev.width;m++)
//    {	
			((SPI_TypeDef *)0x40003800)->DR=0x00;	 	//发送一个byte  
			while(!((((SPI_TypeDef *)0x40003800)->SR)&0x0002));	//等待发送区空	  
			((SPI_TypeDef *)0x40003800)->DR=0x00;	 	//发送一个byte  
			while(!((((SPI_TypeDef *)0x40003800)->SR)&0x0002));		//等待发送区空	  
			((SPI_TypeDef *)0x40003800)->DR=0x00;	 	//发送一个byte  
//		}
	}
	//LCD_CS_SET;
	//PBout(12) = 1;
} 
void LCD_Clear_upper_tail(u16 Color)
{
    unsigned int i,m;  
	LCD_SetWindows(0,32,lcddev.width-1,lcddev.height-32-1);   
	//LCD_CS_CLR;
	//PBout(12) = 0;
	LCD_RS_SET;
	for(i=32;i<lcddev.height-32;i++)
	{
    for(m=0;m<lcddev.width;m++)
    {	
			Lcd_WriteData_16Bit(Color);
		}
	}
	//LCD_CS_SET;
	//PBout(12) = 1;
} 
/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_GPIOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	      
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);	//使能GPIOB时钟
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12; //GPIOB9,10,11,12
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	//打开PB口时钟,AFIO复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);	//使能GPIOB时钟
		//CS:PB12
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  			 //初始化GPIOD3,6
 	//GPIO_SetBits(GPIOB,GPIO_Pin_12);
	//RES:PB3  DC:PB4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	 
 	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4);	

}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	

void LCD_RESET(void)
{

	LCD_RST_CLR;
	delay_ms(20);	
	LCD_RST_SET;
	delay_ms(10);
	
//	LCD_RST_SET;
//	delay_ms(10);
//	LCD_RST_CLR;
//	delay_ms(10);
//	LCD_RST_SET;
//	delay_ms(150);
	
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{  
	SPI2_Init(); //硬件SPI2初始化
	LCD_GPIOInit();//LCD GPIO初始化		

								 
 	LCD_RESET(); //LCD 复位
	
	delay_ms(20);
//************* ILI9488初始化**********//	

//	LCD_WR_REG(0xE0);
//	LCD_WR_DATA(0x00);
//	LCD_WR_DATA(0x07);
//	LCD_WR_DATA(0x10);
//	LCD_WR_DATA(0x09);
//	LCD_WR_DATA(0x17);
//	LCD_WR_DATA(0x0B);
//	LCD_WR_DATA(0x41);
//	LCD_WR_DATA(0x89);
//	LCD_WR_DATA(0x4B);
//	LCD_WR_DATA(0x0A);
//	LCD_WR_DATA(0x0C);
//	LCD_WR_DATA(0x0E);
//	LCD_WR_DATA(0x18);
//	LCD_WR_DATA(0x1B);
//	LCD_WR_DATA(0x0F);
//	
//	LCD_WR_REG(0XE1);
//	LCD_WR_DATA(0x00);
//	LCD_WR_DATA(0x17);
//	LCD_WR_DATA(0x1A);
//	LCD_WR_DATA(0x04);
//	LCD_WR_DATA(0x0E);
//	LCD_WR_DATA(0x06);
//	LCD_WR_DATA(0x2F);
//	LCD_WR_DATA(0x45);
//	LCD_WR_DATA(0x43);
//	LCD_WR_DATA(0x02);
//	LCD_WR_DATA(0x0A);
//	LCD_WR_DATA(0x09);
//	LCD_WR_DATA(0x32);
//	LCD_WR_DATA(0x36);
//	LCD_WR_DATA(0x0F);

//	
//	LCD_WR_REG(0xC0);  
//	LCD_WR_DATA(0x10);  
//	LCD_WR_DATA(0x10);  

//	LCD_WR_REG(0xC1);  
//	LCD_WR_DATA(0x41);  

//	LCD_WR_REG(0xC5);  
//	LCD_WR_DATA(0x00);  
//	LCD_WR_DATA(0x22);  
//	LCD_WR_DATA(0x80); 


////	LCD_WR_REG(0x36);  
////	LCD_WR_DATA(0xe8);  
//	LCD_WR_REG(0x3A); //Interface Mode Control 
//	LCD_WR_DATA(0x66); 
//	LCD_WR_REG(0XB0);  //Interface Mode Control  
//	LCD_WR_DATA(0x00);  

//	LCD_WR_REG(0xB1);   //Frame rate 70HZ   
//	LCD_WR_DATA(0xB0);  
//	LCD_WR_DATA(0x11);  
//	LCD_WR_REG(0xB4);  
//	LCD_WR_DATA(0x02);    
//	LCD_WR_REG(0xB6); //RGB/MCU Interface Control 
//	LCD_WR_DATA(0x02); 
//    //LCD_WR_DATA(0x02);  	
//	LCD_WR_DATA(0x42);  
//	LCD_WR_REG(0xB7);  
//	LCD_WR_DATA(0xC6);  
//	//LCD_WR_REG(0XBE); 
//	//LCD_WR_DATA(0x00); 
//	//LCD_WR_DATA(0x04); 
//	LCD_WR_REG(0xE9);  
//	LCD_WR_DATA(0x00); 
//	LCD_WR_REG(0XF7);     
//	LCD_WR_DATA(0xA9);  
//	LCD_WR_DATA(0x51);  
//	LCD_WR_DATA(0x2C);  
//	LCD_WR_DATA(0x82); 
//	LCD_WR_REG(0x11);  
//	delay_ms(120);  
//	LCD_WR_REG(0x29);
//  delay_ms(10); 	
////	LCD_WR_REG(0x2C);  //memory write 

//    LCD_direction(USE_HORIZONTAL);//设置LCD显示方向
//	//LCD_LED=1;//点亮背光	 
//	//清全屏白色

//中景园LCD初始化

	LCD_WR_REG(0xE0); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x0D); 
	LCD_WR_DATA(0x1B); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x3c); 
	LCD_WR_DATA(0x78); 
	LCD_WR_DATA(0x4A); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x0E); 
	LCD_WR_DATA(0x09); 
	LCD_WR_DATA(0x1B); 
	LCD_WR_DATA(0x1e); 
	LCD_WR_DATA(0x0f);  
	
	LCD_WR_REG(0xE1); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x24); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x12); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x36); 
	LCD_WR_DATA(0x47); 
	LCD_WR_DATA(0x47); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x0a); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x37); 
	LCD_WR_DATA(0x0f); 
	
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x10); 
	
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA(0x41); 
	
	LCD_WR_REG(0xC5); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x80); 
	
//	LCD_WR_REG(0x36);    // Memory Access Control 
//	if(USE_HORIZONTAL==0)LCD_WR_DATA(0x48);
//	else if(USE_HORIZONTAL==1)LCD_WR_DATA(0x88);
//	else if(USE_HORIZONTAL==2)LCD_WR_DATA(0x28);
//	else LCD_WR_DATA(0xE8);

    LCD_direction(USE_HORIZONTAL);//设置LCD显示方向
	
	LCD_WR_REG(0x3A); //Interface Mode Control，此处ILI9486为0X55
	LCD_WR_DATA(0x66);
		
	LCD_WR_REG(0XB0);  //Interface Mode Control  
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xB1);   //Frame rate 70HZ  
	LCD_WR_DATA(0xB0); 
	LCD_WR_DATA(0x11); 
	LCD_WR_REG(0xB4); 
	LCD_WR_DATA(0x02);   
	LCD_WR_REG(0xB6); //RGB/MCU Interface Control
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x42); 
	
	LCD_WR_REG(0xB7); 
	LCD_WR_DATA(0xC6); 
	LCD_WR_REG(0xE9); 
	LCD_WR_DATA(0x00);
	
	LCD_WR_REG(0XF7);    
	LCD_WR_DATA(0xA9); 
	LCD_WR_DATA(0x51); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x82);
	
	LCD_WR_REG(0x11); 
	delay_ms(10); 
	LCD_WR_REG(0x29); 



}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd);
	
//	Lcd_WriteData_16Bit(xStar);
//	Lcd_WriteData_16Bit(xEnd);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd);
	
//	Lcd_WriteData_16Bit(yStar);
//	Lcd_WriteData_16Bit(yEnd);
	
	

	LCD_WriteRAM_Prepare();	//开始写入GRAM			
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{ 
			lcddev.setxcmd=0x2A;
			lcddev.setycmd=0x2B;
			lcddev.wramcmd=0x2C;
	switch(direction){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(1<<3)|(0<<6)|(0<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(1<<3)|(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;	
		default:break;
	}		
}	 
