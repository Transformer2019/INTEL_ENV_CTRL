#ifndef __ST7789_H__
#define __ST7789_H__
#include "stm32f10x.h"

	  	 
#include "sys.h"
#include "stdlib.h"	   
#define LCD_W 320
#define LCD_H 240
#define Rotate_0 0x00
#define Rotate_90 0x70
#define	u8 unsigned char
#define	u16 unsigned int
#define	u32 unsigned long
	    						  
//-----------------LCD�˿ڶ���----------------
#define SPI_PORT GPIOA
#define SPI_CLK  GPIO_Pin_5
#define SPI_MISO GPIO_Pin_6
#define SPI_MOSI GPIO_Pin_7
////DC 
//#define LCD_DC_Clr GPIOB->BRR =  0x0002
//#define LCD_DC_Set GPIOB->BSRR = 0x0002
////RES
//#define LCD_RST_Clr GPIOB->BRR =  0x0001
//#define LCD_RST_Set GPIOB->BSRR = 0x0001
////BLK 		     
//#define LCD_BLK_Clr  GPIOA->BRR =  0x0010
//#define LCD_BLK_Set  GPIOA->BSRR = 0x0010   

//#define LCD_RST_Clr GPIO_ResetBits(GPIOB,GPIO_Pin_0)//RES
//#define LCD_RST_Set GPIO_SetBits(GPIOB,GPIO_Pin_0)

//#define LCD_DC_Clr GPIO_ResetBits(GPIOB,GPIO_Pin_1)//DC
//#define LCD_DC_Set GPIO_SetBits(GPIOB,GPIO_Pin_1)
// 		     
//#define LCD_BLK_Clr  GPIO_ResetBits(GPIOA,GPIO_Pin_4)//CS
//#define LCD_BLK_Set  GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define LCD_RST_Clr GPIO_ResetBits(GPIOB,GPIO_Pin_4)//RES
#define LCD_RST_Set GPIO_SetBits(GPIOB,GPIO_Pin_4)

#define LCD_DC_Clr GPIO_ResetBits(GPIOB,GPIO_Pin_3)//DC
#define LCD_DC_Set GPIO_SetBits(GPIOB,GPIO_Pin_3)
 		     
#define LCD_BLK_Clr  GPIO_ResetBits(GPIOB,GPIO_Pin_12)//CS
#define LCD_BLK_Set  GPIO_SetBits(GPIOB,GPIO_Pin_12)

extern  u16 BACK_COLOR, POINT_COLOR;   //����ɫ������ɫ

void Lcd_Init(void); 
void LCD_Clear(u16 Color);
void LCD_WR_DATA8(char da); //��������-8λ����
void LCD_WR_DATA(int da);
void LCD_WR_DATA_p(u16 *da);
void LCD_WR_REG(char da);
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
void LCD_updat_speed(u16 Color);
void LCD_DrawPoint(u16 x,u16 y);//����
void LCD_DrawPoint_big(u16 x,u16 y);//��һ�����
void Lcd_Rotate_show(u8 angle);
u16  LCD_ReadPoint(u16 x,u16 y); //����
void Draw_Circle(u16 x0,u16 y0,u8 r);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode);//��ʾһ���ַ�
void LCD_ShowChar_NUM(u16 x,u16 y,u8 num,u8 mode);

void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len);//��ʾ����
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len);//��ʾ2������
void LCD_ShowString(u16 x,u16 y,const u8 *p);		 //��ʾһ���ַ���,16����
void LCD_ShowString_NUM(u16 x,u16 y,const u8 *p);
 void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);
void showhanzi(unsigned int x,unsigned int y,unsigned char index);
void showhanzi_1(unsigned int x,unsigned int y,unsigned char index, u16 color);
void showhanzi_10(unsigned int x,unsigned int y,unsigned char index, u16 color);
void showhanzi_1_F(unsigned int x,unsigned int y,unsigned char index, u16 color);
//������ɫ
#define WHITE      0xFFFF
#define BLACK      0x0000	  
#define BLUE       0x001F  
#define BRED       0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED        0xF800
#define MAGENTA    0xF81F
#define GREEN      0x07E0
#define CYAN       0x7FFF
#define YELLOW     0xFFE0
#define BROWN 		 0XBC40 //��ɫ
#define BRRED 		 0XFC07 //�غ�ɫ
#define GRAY  		 0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE   0X01CF	//����ɫ
#define LIGHTBLUE  0X7D7C	//ǳ��ɫ  
#define GRAYBLUE   0X5458 //����ɫ
//������ɫΪPANEL����ɫ 

#define LIGHTGREEN 0X841F //ǳ��ɫ
#define LGRAY 		 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE  0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE     0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


#endif

