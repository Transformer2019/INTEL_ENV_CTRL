#include "st7789.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"

void SPI2_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	/* SPI的IO口和SPI外设打开时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	/* SPI的IO口设置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	//CS:PB12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  			 //初始化GPIOD3,6
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	//RES:PB7  DC:PB6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	 
 	//GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);	
	

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	
	//SPI2->DR = (0xff);//启动传输	

}



u16 BACK_COLOR, POINT_COLOR;   //背景色，画笔色

void SPI1_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	/* SPI的IO口和SPI外设打开时钟 */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  			 //初始化GPIOD3,6
 	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_10;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOD3,6
 	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8);	
	
	/* SPI的IO口设置 */
	GPIO_InitStructure.GPIO_Pin = SPI_CLK | SPI_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 0;
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	
	SPI1->DR = (0xff);//启动传输	
	
}
//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); //使能SPI1
} 
//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
void SPI1_WriteByte(u8 TxData)
{		 			 
//	while ((SPI2->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);//等待发送区空
//	SPI2->DR = TxData;	
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, TxData);
	
	//以下两个因素影响到LCD的屏幕刷新，注释掉立马顿挫感减小？？？？？？？？？？？？？？？？？？？？？？？
//    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//    (void)SPI_I2S_ReceiveData(SPI2);
	
} 
void LCD_WR_DATA8(char da) //发送数据-8位参数
{	
  LCD_DC_Set;
		//LCD_BLK_Clr;
	SPI1_WriteByte(da); 
			//LCD_BLK_Set;
}  
void LCD_WR_DATA(int da)
{
  LCD_DC_Set;
			//LCD_BLK_Clr;
  SPI1_WriteByte(da>>8);
  SPI1_WriteByte(da);
				//LCD_BLK_Set;
}	  
void LCD_WR_REG(char da)	 
{
  LCD_DC_Clr;
			//LCD_BLK_Clr;
	SPI1_WriteByte(da);
				//LCD_BLK_Set;
}
 void LCD_WR_REG_DATA(int reg,int da)
{	
  LCD_WR_REG(reg);
	SPI1_WriteByte(da);
}
 void LCD_WR_DATA_p(u16 *da)
{
  //LCD_DAT;
	SPI1_WriteByte(*da>>8);
  SPI1_WriteByte(*da);
//	DMAx_Init(DMA1_Channel2,(u32)&SPI1->DR,(u32)da,1);
//	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
//	DMAx_Enable(DMA1_Channel2,1); 
//	while(DMA_GetFlagStatus(DMA1_FLAG_TC2)==0);
//	DMA_ClearFlag(DMA1_FLAG_TC2);
}

void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{ 
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);

	LCD_WR_REG(0x2C);					 						 
} 

//清屏函数
//Color:要清屏的填充色
void LCD_Clear(u16 Color)
{
	u16 i,j;  	
	Address_set(0,0,LCD_W-1,LCD_H-1);
	LCD_DC_Set;
  for(i=0;i<LCD_W;i++)
	{
	  for (j=0;j<LCD_H;j++)
	  {
			LCD_WR_DATA(Color);	 			 
	  }
	}
}
void LCD_updat_speed(u16 Color)
{
	u16 i,j;  	

  for(i=0;i<LCD_W;i++)
	{
	  for (j=0;j<LCD_H;j++)
	  {
      SPI1_WriteByte(0XFF);
			SPI1_WriteByte(0XFF);
			//LCD_WR_DATA(Color);	 			 
	  }
	}
}
//在指定位置显示一个汉字(32*33大小)
//dcolor为内容颜色，gbcolor为背静颜色
void showhanzi(unsigned int x,unsigned int y,unsigned char index)	
{  
	unsigned char i,j;
	unsigned char *temp=hanzi;    
  
	Address_set(x,y,x+31,y+31); //设置区域      
	temp+=index*128;
	
	for(j=0;j<128;j++)
	{
		for(i=0;i<8;i++)
		{ 		     
		 	if((*temp&(1<<i))!=0)
			{
				LCD_WR_DATA(POINT_COLOR);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
		temp++;
	 }
}


void showhanzi_1(unsigned int x,unsigned int y,unsigned char index, u16 color)	
{  
	unsigned char i,j;
	unsigned char *temp=hanzi;    
  
	Address_set(x,y,x+31,y+31); //设置区域      
	temp+=index*128;
	
	for(j=0;j<128;j++)
	{
		for(i=0;i<8;i++)
		{ 		     
		 	if((*temp&(1<<i))!=0)
			{
				LCD_WR_DATA(WHITE);
			} 
			else
			{
				LCD_WR_DATA(color);
			}   
		}
		temp++;
	 }
}


void showhanzi_1_F(unsigned int x,unsigned int y,unsigned char index, u16 color)	
{  
	unsigned char i,j;
	unsigned char *temp=hanzi;    
  
	Address_set(x,y,x+31,y+31); //设置区域      
	temp+=index*128;
	
	for(j=0;j<128;j++)
	{
		for(i=0;i<8;i++)
		{ 		     
		 	if((*temp&(1<<i))!=0)
			{
				LCD_WR_DATA(color);
			} 
			else
			{
				LCD_WR_DATA(WHITE);
			}   
		}
		temp++;
	 }
}

void showhanzi_10(unsigned int x,unsigned int y,unsigned char index, u16 color)	
{  
	unsigned char i,j;
	unsigned char *temp=fengji; 
 
	Address_set(x,y,x+15,y+31); //设置区域       
	temp+=index*64;
	for(j=0;j<64;j++){
		for(i=0;i<8;i++)
		{ 		     
		 	if((*temp&(1<<i))!=0)
			{
				LCD_WR_DATA(WHITE);
			} 
			else
			{
				LCD_WR_DATA(color);
			}   
		}
		temp++;
	}
	
}

//画点
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	Address_set(x,y,x,y);//设置光标位置 
	LCD_WR_DATA(POINT_COLOR); 	    
} 	 
//画一个大点
//POINT_COLOR:此点的颜色
void LCD_DrawPoint_big(u16 x,u16 y)
{
	LCD_Fill(x-1,y-1,x+1,y+1,POINT_COLOR);
} 
//在指定区域内填充指定颜色
//区域大小:
//  (xend-xsta)*(yend-ysta)
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	Address_set(xsta,ysta,xend,yend);      //设置光标位置 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//设置光标位置 	    
	} 					  	    
}  
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//画矩形
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
//在指定位置显示一个字符

//num:要显示的字符:" "--->"~"
//mode:叠加方式(1)还是非叠加方式(0)
//在指定位置显示一个字符

//num:要显示的字符:" "--->"~"

//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode)
{
    u8 temp;
    u8 pos,t;
	u16 x0=x;
	u16 colortemp=POINT_COLOR;      
    if(x>LCD_W-16||y>LCD_H-16)return;	    
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	Address_set(x,y,x+8-1,y+16-1);      //设置光标位置 
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<16;pos++)
		{ 
			temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				LCD_WR_DATA(POINT_COLOR);	
				temp>>=1; 
				x++;
		    }
			x=x0;
			y++;
		}	
	}else//叠加方式
	{
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点     
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   



void LCD_ShowChar_NUM(u16 x,u16 y,u8 num,u8 mode)
{
    u8 temp;
    u8 pos,t;
	u16 x0=x;    
    if(x>LCD_W-16||y>LCD_H-16)return;	    
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	Address_set(x,y,x+8-1,y+16-1);      //设置光标位置 

	for(pos=0;pos<16;pos++)
	{ 
		temp=asc2_1608[(u16)num*16+pos];		 //调用1608字体
		for(t=0;t<8;t++)
		{                 
			if(temp&0x01)
			LCD_WR_DATA(WHITE);
			else
			LCD_WR_DATA(GBLUE);	
			temp>>=1; 
			x++;
		}
		x=x0;
		y++;
	}  	   	 	  
}  
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//color:颜色
//num:数值(0~4294967295);	
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len)
{         	
	u8 t,temp;
	u8 enshow=0;
	num=(u16)num;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ',0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0); 
	}
} 
//显示2个数字
//x,y:起点坐标
//num:数值(0~99);	 
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len)
{         	
	u8 t,temp;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
	 	LCD_ShowChar(x+8*t,y,temp+'0',0); 
	}
} 
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void LCD_ShowString(u16 x,u16 y,const u8 *p)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-16){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0);
        x+=8;
        p++;
    }  
}


void LCD_ShowString_NUM(u16 x,u16 y,const u8 *p)
{         
    while(*p!='\0')
    {       
//        if(x>LCD_W-16){x=0;y+=16;}
//        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar_NUM(x,y,*p,0);
        x+=8;
        p++;
    }  
}


void Lcd_Rotate_show(u8 angle)
{
	LCD_WR_REG(0x36); 
	LCD_WR_DATA8(angle);
}
void Lcd_Init(void)
{
	SPI2_Init();
	
	LCD_RST_Clr;
	delay_ms(100);
	LCD_RST_Set;
	delay_ms(100);
	LCD_BLK_Clr;
	
//	LCD_WR_REG(0x01); // 软件复位
//	delay_ms(150); // 延时150ms
//    LCD_WR_REG(0x11); // 退出睡眠模式
//    delay_ms(255); // 延时255ms

	//************* Start Initial Sequence **********// 
//	LCD_WR_REG(0x36); 
//	LCD_WR_DATA8(0x70);

//	LCD_WR_REG(0x3A); 
//	LCD_WR_DATA8(0x05);

//	LCD_WR_REG(0xB2);
//	LCD_WR_DATA8(0x0C);
//	LCD_WR_DATA8(0x0C);
//	LCD_WR_DATA8(0x00);
//	LCD_WR_DATA8(0x33);
//	LCD_WR_DATA8(0x33);

//	LCD_WR_REG(0xB7); 
//	LCD_WR_DATA8(0x35);  

//	LCD_WR_REG(0xBB);
//	LCD_WR_DATA8(0x19);

//	LCD_WR_REG(0xC0);
//	LCD_WR_DATA8(0x2C);

//	LCD_WR_REG(0xC2);
//	LCD_WR_DATA8(0x01);

//	LCD_WR_REG(0xC3);
//	LCD_WR_DATA8(0x12);   

//	LCD_WR_REG(0xC4);
//	LCD_WR_DATA8(0x20);  

//	LCD_WR_REG(0xC6); 
//	LCD_WR_DATA8(0x0F);    

//	LCD_WR_REG(0xD0); 
//	LCD_WR_DATA8(0xA4);
//	LCD_WR_DATA8(0xA1);

//	LCD_WR_REG(0xE0);
//	LCD_WR_DATA8(0xD0);
//	LCD_WR_DATA8(0x04);
//	LCD_WR_DATA8(0x0D);
//	LCD_WR_DATA8(0x11);
//	LCD_WR_DATA8(0x13);
//	LCD_WR_DATA8(0x2B);
//	LCD_WR_DATA8(0x3F);
//	LCD_WR_DATA8(0x54);
//	LCD_WR_DATA8(0x4C);
//	LCD_WR_DATA8(0x18);
//	LCD_WR_DATA8(0x0D);
//	LCD_WR_DATA8(0x0B);
//	LCD_WR_DATA8(0x1F);
//	LCD_WR_DATA8(0x23);

//	LCD_WR_REG(0xE1);
//	LCD_WR_DATA8(0xD0);
//	LCD_WR_DATA8(0x04);
//	LCD_WR_DATA8(0x0C);
//	LCD_WR_DATA8(0x11);
//	LCD_WR_DATA8(0x13);
//	LCD_WR_DATA8(0x2C);
//	LCD_WR_DATA8(0x3F);
//	LCD_WR_DATA8(0x44);
//	LCD_WR_DATA8(0x51);
//	LCD_WR_DATA8(0x2F);
//	LCD_WR_DATA8(0x1F);
//	LCD_WR_DATA8(0x1F);
//	LCD_WR_DATA8(0x20);
//	LCD_WR_DATA8(0x23);

//	LCD_WR_REG(0x21); 

//	LCD_WR_REG(0x11);  

//	LCD_WR_REG(0x29); 

		LCD_WR_REG(0XF7);
	LCD_WR_DATA(0xA9);
	LCD_WR_DATA(0x51);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x82);
	LCD_WR_REG(0xC0);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x09);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0x41);
	LCD_WR_REG(0XC5);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x80);
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0xB0);
	LCD_WR_DATA(0x11);
	LCD_WR_REG(0xB4);
	LCD_WR_DATA(0x02);
	LCD_WR_REG(0xB6);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x42);
	LCD_WR_REG(0xB7);
	LCD_WR_DATA(0xc6);
	LCD_WR_REG(0xBE);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x04);
	LCD_WR_REG(0xE9);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0x36);
	LCD_WR_DATA((1<<3)|(0<<7)|(1<<6)|(1<<5));
	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x66);
	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x41);
	LCD_WR_DATA(0x89);
	LCD_WR_DATA(0x4B);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x1B);
	LCD_WR_DATA(0x0F);
	LCD_WR_REG(0XE1);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x1A);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x2F);
	LCD_WR_DATA(0x45);
	LCD_WR_DATA(0x43);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x32);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x0F);
	LCD_WR_REG(0x11);
	delay_ms(120);
	LCD_WR_REG(0x29);
	


//	LCD_WR_REG(0xE0);  
//	LCD_WR_DATA(0x00);  
//	LCD_WR_DATA(0x07);  
//	LCD_WR_DATA(0x0f);  
//	LCD_WR_DATA(0x0D);  
//	LCD_WR_DATA(0x1B);  
//	LCD_WR_DATA(0x0A);  
//	LCD_WR_DATA(0x3c);  
//	LCD_WR_DATA(0x78);  
//	LCD_WR_DATA(0x4A);  
//	LCD_WR_DATA(0x07);  
//	LCD_WR_DATA(0x0E);  
//	LCD_WR_DATA(0x09);  
//	LCD_WR_DATA(0x1B);  
//	LCD_WR_DATA(0x1e);  
//	LCD_WR_DATA(0x0f);   

//	LCD_WR_REG(0xE1);  
//	LCD_WR_DATA(0x00);  
//	LCD_WR_DATA(0x22);  
//	LCD_WR_DATA(0x24);  
//	LCD_WR_DATA(0x06);  
//	LCD_WR_DATA(0x12);  
//	LCD_WR_DATA(0x07);  
//	LCD_WR_DATA(0x36);  
//	LCD_WR_DATA(0x47);  
//	LCD_WR_DATA(0x47);  
//	LCD_WR_DATA(0x06);  
//	LCD_WR_DATA(0x0a);  
//	LCD_WR_DATA(0x07);  
//	LCD_WR_DATA(0x30);  
//	LCD_WR_DATA(0x37);  
//	LCD_WR_DATA(0x0f);  

	LCD_WR_REG(0xC0);  
	LCD_WR_DATA(0x10);  
	LCD_WR_DATA(0x10);  

	LCD_WR_REG(0xC1);  
	LCD_WR_DATA(0x41);  

	LCD_WR_REG(0xC5);  
	LCD_WR_DATA(0x00);  
	LCD_WR_DATA(0x22);  
	LCD_WR_DATA(0x80); 


	LCD_WR_REG(0x36);  
	LCD_WR_DATA(0x08);  
	LCD_WR_REG(0x3A); //Interface Mode Control 
	//LCD_WR_REG(0x3A); //Interface Mode Control
	LCD_WR_DATA(0x66); 
	LCD_WR_REG(0XB0);  //Interface Mode Control  
	LCD_WR_DATA(0x00);  
	//Interface Mode Control   
	LCD_WR_REG(0xB1);   //Frame rate 70HZ   
	//LCD_WR_REG(0xB1);   //Frame rate 70HZ  
	LCD_WR_DATA(0xB0);  
	LCD_WR_DATA(0x11);  
	LCD_WR_REG(0xB4);  
	LCD_WR_DATA(0x02);    
	LCD_WR_REG(0xB6); //RGB/MCU Interface Control 
	LCD_WR_REG(0xB6); //RGB/MCU Interface Control
	//LCD_WR_DATA(0x02);  
	LCD_WR_DATA(0x02);  
	LCD_WR_REG(0xB7);  
	LCD_WR_DATA(0xC6);  
	//LCD_WR_REG(0XBE); 
	//LCD_WR_DATA(0x00); 
	//LCD_WR_DATA(0x04); 
	LCD_WR_REG(0xE9);  
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0XF7);     
	LCD_WR_DATA(0xA9);  
	LCD_WR_DATA(0x51);  
	LCD_WR_DATA(0x2C);  
	LCD_WR_DATA(0x82); 
	LCD_WR_REG(0x11);  
	delay_ms(120);  
	LCD_WR_REG(0x29);
delay_ms(10); 	
	LCD_WR_REG(0x2C);  //memory write 

  LCD_direction(USE_HORIZONTAL);//设置LCD显示方向
 
}



