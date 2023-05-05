/**
  ******************************************************************************
  * 文件名程: bsp_lcd.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 液晶底层驱动函数实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_lcd.h"
#include "main.h"
#include "ascii.h"

#if 0
#if USB_SPIFLASH_CH==1
#include "spiflash/bsp_spiflash.h"
#define GBK_HZ1616_ADDR       (10*4096)   // GBK_HZ1616.FON字库文件存放在串行Flash的地址
#define GBK_HZ2424_ADDR       (75*4096)   // GBK_HZ2424.FON字库文件存放在串行Flash的地址
#else
#include "ff.h"
/* 文件句柄 */
FIL file_ch;
#endif
#endif

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
//__IO uint32_t lcd_id=0;                // 保存当前检查到的液晶模块ID
//SRAM_HandleTypeDef hlcd;               // SRAM外设句柄
//static int FSMC_LCD_Initialized = 0;   // FSMC初始化标志位：0：未初始化；1：已完成初始化
//static int FSMC_LCD_DeInitialized = 0; // FSMC反初始化标志位：0：未反初始化；1：已完成反初始化

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 初始化LCD寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：需要配置哪些寄存器，需要设置什么值与液晶厂家生产环境密切相关，
  *           所以这些参数由厂家提供，不同厂家可能不同。也可以根据ILI9488芯片
  *           手册内容参考修改。
  */
static void ILI9488_REG_Config ( void )
{
  //************* Start Initial Sequence **********//
  /* PGAMCTRL (Positive Gamma Control) (E0h) */
  LCD_WRITE_CMD(0xE0);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x07);
  LCD_WRITE_DATA(0x10);
  LCD_WRITE_DATA(0x09);
  LCD_WRITE_DATA(0x17);
  LCD_WRITE_DATA(0x0B);
  LCD_WRITE_DATA(0x41);
  LCD_WRITE_DATA(0x89);
  LCD_WRITE_DATA(0x4B);
  LCD_WRITE_DATA(0x0A);
  LCD_WRITE_DATA(0x0C);
  LCD_WRITE_DATA(0x0E);
  LCD_WRITE_DATA(0x18);
  LCD_WRITE_DATA(0x1B);
  LCD_WRITE_DATA(0x0F);

  /* NGAMCTRL (Negative Gamma Control) (E1h)  */
  LCD_WRITE_CMD(0XE1);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x17);
  LCD_WRITE_DATA(0x1A);
  LCD_WRITE_DATA(0x04);
  LCD_WRITE_DATA(0x0E);
  LCD_WRITE_DATA(0x06);
  LCD_WRITE_DATA(0x2F);
  LCD_WRITE_DATA(0x45);
  LCD_WRITE_DATA(0x43);
  LCD_WRITE_DATA(0x02);
  LCD_WRITE_DATA(0x0A);
  LCD_WRITE_DATA(0x09);
  LCD_WRITE_DATA(0x32);
  LCD_WRITE_DATA(0x36);
  LCD_WRITE_DATA(0x0F);
  
  /* Adjust Control 3 (F7h)  */
  LCD_WRITE_CMD(0XF7);
  LCD_WRITE_DATA(0xA9);
  LCD_WRITE_DATA(0x51);
  LCD_WRITE_DATA(0x2C);
  LCD_WRITE_DATA(0x82);/* DSI write DCS command, use loose packet RGB 666 */

  /* Power Control 1 (C0h)  */
  LCD_WRITE_CMD(0xC0);
  LCD_WRITE_DATA(0x11);
  LCD_WRITE_DATA(0x09);

  /* Power Control 2 (C1h) */
  LCD_WRITE_CMD(0xC1);
  LCD_WRITE_DATA(0x41);

  /* VCOM Control (C5h)  */
  LCD_WRITE_CMD(0XC5);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x0A);
  LCD_WRITE_DATA(0x80);

  /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
  LCD_WRITE_CMD(0xB1);
  LCD_WRITE_DATA(0xB0);
  LCD_WRITE_DATA(0x11);

  /* Display Inversion Control (B4h) */
  LCD_WRITE_CMD(0xB4);
  LCD_WRITE_DATA(0x02);

  /* Display Function Control (B6h)  */
  LCD_WRITE_CMD(0xB6);
  LCD_WRITE_DATA(0x02);
  LCD_WRITE_DATA(0x22);

  /* Entry Mode Set (B7h)  */
  LCD_WRITE_CMD(0xB7);
  LCD_WRITE_DATA(0xc6);

  /* HS Lanes Control (BEh) */
  LCD_WRITE_CMD(0xBE);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x04);

  /* Set Image Function (E9h)  */
  LCD_WRITE_CMD(0xE9);
  LCD_WRITE_DATA(0x00);
 
  /* 设置屏幕方向和尺寸 */
  LCD_SetDirection(LCD_DIRECTION);
  
  /* Interface Pixel Format (3Ah) */
  LCD_WRITE_CMD(0x3A);
  LCD_WRITE_DATA(0x55);/* 0x55 : 16 bits/pixel  */

  /* Sleep Out (11h) */
  LCD_WRITE_CMD(0x11);
  HAL_Delay(120);
  /* Display On */
  LCD_WRITE_CMD(0x29);
}

/**
  * 函数功能: 读取液晶模组ID
  * 输入参数: 无
  * 返 回 值: 液晶模组的ID
  * 说    明：这是通过读取04H寄存器获取得到液晶模组ID，该ID值有液晶厂家编程，不同液晶
  *           厂家的液晶模组得到的ID值可能不同。这也可以分辨不同型号的液晶模组。
  */
static uint32_t LCD_ReadID(void)
{
	uint16_t buf[4];

	LCD_WRITE_CMD(0x04);  
	buf[0] = LCD_READ_DATA();        // 第一个读取数据无效
	buf[1] = LCD_READ_DATA()&0x00ff; // 只有低8位数据有效
	buf[2] = LCD_READ_DATA()&0x00ff; // 只有低8位数据有效
	buf[3] = LCD_READ_DATA()&0x00ff; // 只有低8位数据有效
	return (buf[1] << 16) + (buf[2] << 8) + buf[3];  
}

/**
  * 函数功能: 液晶模组初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void BSP_LCD_Init(void)
{	
	uint32_t lcd_id=LCD_ReadID(); 
  if(lcd_id==0x548066 || lcd_id==0x8066)
  {
    ILI9488_REG_Config();
  }
  LCD_Clear(0,0,LCD_DEFAULT_WIDTH,LCD_DEFAULT_HEIGTH,BLACK);
  HAL_Delay(20);
	LCD_BK_ON();
}

/**
  * 函数功能: 设置LCD的GRAM的扫描方向 
  * 输入参数: ucOption ：选择GRAM的扫描方向 
  *           可选值：1 :原点在屏幕左上角 X*Y=320*480
  *                   2 :原点在屏幕右上角 X*Y=480*320
  *                   3 :原点在屏幕右下角 X*Y=320*480
  *                   4 :原点在屏幕左下角 X*Y=480*320
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_SetDirection( uint8_t ucOption )
{	
/**
  * Memory Access Control (36h)
  * This command defines read/write scanning direction of the frame memory.
  *
  * These 3 bits control the direction from the MPU to memory write/read.
  *
  * Bit  Symbol  Name  Description
  * D7   MY  Row Address Order     -- 以X轴镜像
  * D6   MX  Column Address Order  -- 以Y轴镜像
  * D5   MV  Row/Column Exchange   -- X轴与Y轴交换
  * D4   ML  Vertical Refresh Order  LCD vertical refresh direction control. 
  *
  * D3   BGR RGB-BGR Order   Color selector switch control
  *      (0 = RGB color filter panel, 1 = BGR color filter panel )
  * D2   MH  Horizontal Refresh ORDER  LCD horizontal refreshing direction control.
  * D1   X   Reserved  Reserved
  * D0   X   Reserved  Reserved
  */
	switch ( ucOption )
	{
		case 1:
//   左上角->右下角 
//	(0,0)	___ x(320)
//	     |  
//	     |
//       |	y(480) 
			LCD_WRITE_CMD(0x36); 
			LCD_WRITE_DATA(0x08); 
      
			LCD_WRITE_CMD(0x2A); 
			LCD_WRITE_DATA(0x00);	/* x start */	
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);  /* x end */	
			LCD_WRITE_DATA(0x3F);

			LCD_WRITE_CMD(0x2B); 
			LCD_WRITE_DATA(0x00);	/* y start */  
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);	/* y end */   
			LCD_WRITE_DATA(0xDF);					
		  break;
		
		case 2:
//		右上角-> 左下角
//		y(320)___ (0,0)            
//		         |
//		         |
//             |x(480)    
			LCD_WRITE_CMD(0x36); 
			LCD_WRITE_DATA(0x68);	
			LCD_WRITE_CMD(0x2A); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0xDF);	

			LCD_WRITE_CMD(0x2B); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0x3F);				
		  break;
		
		case 3:
//		右下角->左上角
//		          |y(480)
//		          |           
//		x(320) ___|(0,0)		
			LCD_WRITE_CMD(0x36); 
			LCD_WRITE_DATA(0xC8);	
			LCD_WRITE_CMD(0x2A); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0x3F);	

			LCD_WRITE_CMD(0x2B); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0x3F);			  
		  break;

		case 4:
//		左下角->右上角
//		|x(480)
//		|
//		|___ y(320)					  
			LCD_WRITE_CMD(0x36); 
			LCD_WRITE_DATA(0xA8);	
    
			LCD_WRITE_CMD(0x2A); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0xDF);	

			LCD_WRITE_CMD(0x2B); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0x3F);				
	    break;		
	}	
	/* 开始向GRAM写入数据 */
	LCD_WRITE_CMD (0x2C);	
}

/**
  * 函数功能: 在LCD显示器上开辟一个窗口
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  *           usWidth ：窗口的宽度
  *           usHeight ：窗口的高度
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{	
	LCD_WRITE_CMD(0x2A ); 				       /* 设置X坐标 */
	LCD_WRITE_DATA(usX>>8);	             /* 设置起始点：先高8位 */
	LCD_WRITE_DATA(usX&0xff);	           /* 然后低8位 */
	LCD_WRITE_DATA((usX+usWidth-1)>>8);  /* 设置结束点：先高8位 */
	LCD_WRITE_DATA((usX+usWidth-1)&0xff);/* 然后低8位 */

	LCD_WRITE_CMD(0x2B); 			           /* 设置Y坐标*/
	LCD_WRITE_DATA(usY>>8);              /* 设置起始点：先高8位 */
	LCD_WRITE_DATA(usY&0xff);            /* 然后低8位 */
	LCD_WRITE_DATA((usY+usHeight-1)>>8); /* 设置结束点：先高8位 */
	LCD_WRITE_DATA((usY+usHeight-1)&0xff);/* 然后低8位 */
}

/**
  * 函数功能: 设定LCD的光标坐标
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  * 返 回 值: 无
  * 说    明：无
  */
static void LCD_SetCursor(uint16_t usX,uint16_t usY)	
{
	LCD_OpenWindow(usX,usY,1,1);
}

/**
  * 函数功能: 在LCD显示器上以某一颜色填充像素点
  * 输入参数: ulAmout_Point ：要填充颜色的像素点的总数目
  *           usColor ：颜色
  * 返 回 值: 无
  * 说    明：无
  */
#if defined ( __CC_ARM )  // 使用Keil编译环境
static __inline void LCD_FillColor ( uint32_t ulAmout_Point, uint16_t usColor )
{
	uint32_t i = 0;	
	
	/* 开始向GRAM写入数据 */
	LCD_WRITE_CMD ( 0x2C );	
	
	for ( i = 0; i < ulAmout_Point; i ++ )
		LCD_WRITE_DATA ( usColor );	
}
#elif defined ( __ICCARM__ ) // 使用IAR编译环境
#pragma inline
static void LCD_FillColor ( uint32_t ulAmout_Point, uint16_t usColor )
{
	uint32_t i = 0;	
	
	/* 开始向GRAM写入数据 */
	LCD_WRITE_CMD ( 0x2C );	
	
	for ( i = 0; i < ulAmout_Point; i ++ )
		LCD_WRITE_DATA ( usColor );	
}
#endif

/**
  * 函数功能: 对LCD显示器的某一窗口以某种颜色进行清屏
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  *           usWidth ：窗口的宽度
  *           usHeight ：窗口的高度
  *           usColor ：颜色
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_Clear(uint16_t usX,uint16_t usY,uint16_t usWidth,uint16_t usHeight,uint16_t usColor)
{	 
#if 0   /* 优化代码执行速度 */
  uint32_t i;
	uint32_t n,m;
  /* 在LCD显示器上开辟一个窗口 */
  LCD_OpenWindow(usX,usY,usWidth,usHeight); 
  /* 开始向GRAM写入数据 */
  LCD_WRITE_CMD(0x2C);
  
  m=usWidth * usHeight;
  n=m/8;
  m=m-8*n;
	for(i=0;i<n;i++)
	{
		LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
	}
  for(i=0;i<m;i++)
	{
		LCD_WRITE_DATA(usColor);	
	}
#else
  /* 在LCD显示器上开辟一个窗口 */
  LCD_OpenWindow(usX,usY,usWidth,usHeight);
  /* 在LCD显示器上以某一颜色填充像素点 */
	LCD_FillColor(usWidth*usHeight,usColor);	
#endif	
}

/**
  * 函数功能: 对LCD显示器的某一点以某种颜色进行填充
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  *           usColor ：颜色
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_SetPointPixel(uint16_t usX,uint16_t usY,uint16_t usColor)	
{	
	if((usX<LCD_DEFAULT_WIDTH)&&(usY<LCD_DEFAULT_HEIGTH))
  {
		LCD_OpenWindow(usX,usY,1,1);
		LCD_FillColor(1,usColor);
	}
}

/**
  * 函数功能: 对LCD显示器的某一点以某种颜色进行填充
  * 输入参数: 无
  * 返 回 值: uint16_t:像素数据RGB565
  * 说    明：无
  */
static uint16_t LCD_Read_PixelData ( void )	
{	
	uint16_t usR=0, usG=0, usB=0 ;
	
	LCD_WRITE_CMD ( 0x2E );   /* 读数据 */
	usR = LCD_READ_DATA (); 	/*FIRST READ OUT DUMMY DATA*/
	
	usR = LCD_READ_DATA ();  	/*READ OUT RED DATA  */
	usB = LCD_READ_DATA ();  	/*READ OUT BLUE DATA*/
	usG = LCD_READ_DATA ();  	/*READ OUT GREEN DATA*/	
	
  return (((usR>>11)<<11) | ((usG>>10)<<5) | (usB>>11));
	
}

/**
  * 函数功能: 获取 LCD 显示器上某一个坐标点的像素数据
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  * 返 回 值: uint16_t:像素数据RGB565
  * 说    明：无
  */
uint16_t LCD_GetPointPixel ( uint16_t usX, uint16_t usY )
{ 
	uint16_t usPixelData;
	
	LCD_SetCursor ( usX, usY );
	
	usPixelData = LCD_Read_PixelData ();
	
	return usPixelData;
	
}

/**
  * 函数功能: 在 LCD 显示器上使用 Bresenham 算法画线段
  * 输入参数: usX1 ：在特定扫描方向下窗口的起点X坐标
  *           usY1 ：在特定扫描方向下窗口的起点Y坐标
  *           usX2 ：在特定扫描方向下线段的另一个端点X坐标
  *           usY2 ：在特定扫描方向下线段的另一个端点Y坐标
  *           usColor ：线段的颜色
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DrawLine(uint16_t usX1,uint16_t usY1,uint16_t usX2,uint16_t usY2,uint16_t usColor)
{
	uint16_t us; 
	uint16_t usX_Current, usY_Current;
	int32_t lError_X=0,lError_Y=0,lDelta_X,lDelta_Y,lDistance; 
	int32_t lIncrease_X, lIncrease_Y;
	
	lDelta_X=usX2-usX1; //计算坐标增量 
	lDelta_Y=usY2-usY1; 
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	if(lDelta_X>0)
  {
    lIncrease_X=1; //设置单步方向 
	}
  else if(lDelta_X==0)
  {
		lIncrease_X=0;//垂直线 
	}
	else 
  { 
    lIncrease_X=-1;
    lDelta_X=-lDelta_X;
  }
  
	if(lDelta_Y>0)
  {
		lIncrease_Y=1;
	}
	else if(lDelta_Y==0)
  {
		lIncrease_Y=0;//水平线 
	}
	else
  {
    lIncrease_Y=-1;
    lDelta_Y=-lDelta_Y;
  }
	
	if(lDelta_X>lDelta_Y)
  {
		lDistance=lDelta_X; //选取基本增量坐标轴 
	}
	else
  {
		lDistance=lDelta_Y; 
  }
	
	for(us=0;us<=lDistance+1;us++)//画线输出 
	{
		LCD_SetPointPixel(usX_Current,usY_Current,usColor);//画点 
		lError_X+=lDelta_X; 
		lError_Y+=lDelta_Y;
		if(lError_X>lDistance)
		{ 
			lError_X-=lDistance; 
			usX_Current+=lIncrease_X; 
		}
		if(lError_Y>lDistance) 
		{ 
			lError_Y-=lDistance; 
			usY_Current+=lIncrease_Y; 
		}		
	}
}

/**
  * 函数功能: 在LCD显示器上画一个矩形
  * 输入参数: usX_Start ：在特定扫描方向下窗口的起点X坐标
  *           usY_Start ：在特定扫描方向下窗口的起点Y坐标
  *           usWidth：矩形的宽度（单位：像素）
  *           usHeight：矩形的高度（单位：像素）
  *           usColor ：矩形的颜色
  *           ucFilled ：选择是否填充该矩形
  *             可选值：0：空心矩形
  *                     1：实心矩形
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DrawRectangle ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint16_t usColor, uint8_t ucFilled )
{
	if(ucFilled)
  {
	  LCD_Clear ( usX_Start, usY_Start, usWidth, usHeight, usColor);
  }
	else
	{
		LCD_DrawLine ( usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start, usColor );
		LCD_DrawLine ( usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1, usColor );
		LCD_DrawLine ( usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1, usColor );
		LCD_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1, usColor );		
	}
}

/**
  * 函数功能: 在 LCD 显示器上使用 Bresenham 算法画圆
  * 输入参数: usX_Center ：在特定扫描方向下圆心的X坐标
  *           usY_Center ：在特定扫描方向下圆心的Y坐标
  *           usRadius：圆的半径（单位：像素）
  *           usColor ：圆的颜色
  *           ucFilled ：选择是否填充该圆
  *             可选值：0：空心圆
  *                     1：实心圆
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DrawCircle(uint16_t usX_Center,uint16_t usY_Center,uint16_t usRadius,uint16_t usColor,uint8_t ucFilled)
{
	int16_t sCurrentX, sCurrentY;
	int16_t sError;
  
	sCurrentX=0;
  sCurrentY=usRadius;	
	sError=3-(usRadius<<1);   //判断下个点位置的标志
	
	while(sCurrentX<=sCurrentY)
	{
		int16_t sCountY;		
		if(ucFilled)
    {			
			for(sCountY=sCurrentX;sCountY<=sCurrentY;sCountY++)
			{                      
				LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center+sCountY,usColor);          //1，研究对象 
				LCD_SetPointPixel(usX_Center-sCurrentX,usY_Center+sCountY,usColor);           //2       
				LCD_SetPointPixel(usX_Center-sCountY,  usY_Center+sCurrentX,usColor);           //3
				LCD_SetPointPixel(usX_Center-sCountY,  usY_Center-sCurrentX,usColor);           //4
				LCD_SetPointPixel(usX_Center-sCurrentX,usY_Center-sCountY,usColor);           //5    
        LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center-sCountY,usColor);           //6
				LCD_SetPointPixel(usX_Center+sCountY,  usY_Center-sCurrentX,usColor);           //7 	
        LCD_SetPointPixel(usX_Center+sCountY,  usY_Center+sCurrentX,usColor);           //0				
			}
    }		
		else
		{          
			LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center+sCurrentY,usColor);             //1，研究对象
			LCD_SetPointPixel(usX_Center-sCurrentX,usY_Center+sCurrentY,usColor);             //2      
			LCD_SetPointPixel(usX_Center-sCurrentY,usY_Center+sCurrentX,usColor);             //3
			LCD_SetPointPixel(usX_Center-sCurrentY,usY_Center-sCurrentX,usColor);             //4
			LCD_SetPointPixel(usX_Center-sCurrentX,usY_Center-sCurrentY,usColor);             //5       
			LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center-sCurrentY,usColor);             //6
			LCD_SetPointPixel(usX_Center+sCurrentY,usY_Center-sCurrentX,usColor);             //7 
			LCD_SetPointPixel(usX_Center+sCurrentY,usY_Center+sCurrentX,usColor);             //0
    }			
		sCurrentX ++;		
		if(sError<0) 
    {
			sError+=(4*sCurrentX+6);	  
		}
		else
		{
			sError +=(10+4*(sCurrentX-sCurrentY));   
			sCurrentY--;
		} 
	}
}

/**
  * 函数功能: 在 LCD 显示器上显示一个英文字符
  * 输入参数: usX：在特定扫描方向下字符的起始X坐标
  *           usY ：在特定扫描方向下该点的起始Y坐标
  *           cChar ：要显示的英文字符
  *           usColor_Background ：选择英文字符的背景色
  *           usColor_Foreground ：选择英文字符的前景色
  *           font：字体选择
  *             参数：USB_FONT_16 ：16号字体
  *                   USB_FONT_24 ：24号字体 
  * 返 回 值: 无
  * 说    明：该函数必须与ascii.h内容对应使用
  */
void LCD_DispChar_EN( uint16_t usX, uint16_t usY, const char cChar, uint16_t usColor_Background, uint16_t usColor_Foreground,USB_FONT_Typdef font)
{
	uint8_t ucTemp, ucRelativePositon, ucPage, ucColumn;
  
  /* 检查输入参数是否合法 */
  assert_param(IS_USB_FONT(font));
  
	ucRelativePositon = cChar - ' ';
  
	if(font==USB_FONT_16)
  {
    LCD_OpenWindow(usX,usY,8,16);
    LCD_WRITE_CMD(0x2C);
    
    for(ucPage=0;ucPage<16;ucPage++)
    {
      ucTemp=ucAscii_1608[ucRelativePositon][ucPage];		
      for(ucColumn=0;ucColumn<8;ucColumn++)
      {
        if(ucTemp&0x01)
          LCD_WRITE_DATA(usColor_Foreground);			
        else
          LCD_WRITE_DATA(usColor_Background);								
        ucTemp >>= 1;					
      }
    }    
  }
  else
  {
    LCD_OpenWindow(usX,usY,12,24);
    LCD_WRITE_CMD(0x2C);
    
    for(ucPage=0;ucPage<48;ucPage++)
    {
      ucTemp=ucAscii_2412[ucRelativePositon][ucPage];		
      for(ucColumn=0;ucColumn<8;ucColumn++)
      {
        if(ucTemp&0x01)
          LCD_WRITE_DATA(usColor_Foreground);			
        else
          LCD_WRITE_DATA(usColor_Background);								
        ucTemp >>= 1;					
      }	
      ucPage++;
      ucTemp=ucAscii_2412[ucRelativePositon][ucPage];
      /* 只显示前面4个位，与上面8位总共12位 */
      for(ucColumn=0;ucColumn<4;ucColumn++)
      {
        if(ucTemp&0x01)
          LCD_WRITE_DATA(usColor_Foreground);			
        else
          LCD_WRITE_DATA(usColor_Background);								
        ucTemp >>= 1;					
      }	
    }
  }	
}

/**
  * 函数功能: 在 LCD 显示器上显示英文字符串
  * 输入参数: usX：在特定扫描方向下字符的起始X坐标
  *           usY ：在特定扫描方向下该点的起始Y坐标
  *           pStr ：要显示的英文字符串的首地址
  *           usColor_Background ：选择英文字符的背景色
  *           usColor_Foreground ：选择英文字符的前景色
  *           font：字体选择
  *             参数：USB_FONT_16 ：16号字体
  *                   USB_FONT_24 ：24号字体 
  * 返 回 值: 无
  * 说    明：该函数必须与ascii.h内容对应使用
  */
void LCD_DispString_EN ( uint16_t usX, uint16_t usY, const char * pStr, uint16_t usColor_Background, uint16_t usColor_Foreground,USB_FONT_Typdef font)
{
  /* 检查输入参数是否合法 */
  assert_param(IS_USB_FONT(font));
  
	while ( * pStr != '\0' )
	{
    if(font==USB_FONT_16)
    {
      if ( ( usX +  8 ) > LCD_DEFAULT_WIDTH )
      {
        usX = 0;
        usY += 16;
      }      
      if ( ( usY +  16 ) > LCD_DEFAULT_HEIGTH )
      {
        usX = 0;
        usY = 0;
      }      
      LCD_DispChar_EN ( usX, usY, * pStr, usColor_Background, usColor_Foreground,font);
      pStr ++;      
      usX += 8;
    }
    else
    {
      if ( ( usX +  12 ) > LCD_DEFAULT_WIDTH )
      {
        usX = 0;
        usY += 24;
      }      
      if ( ( usY +  24 ) > LCD_DEFAULT_HEIGTH )
      {
        usX = 0;
        usY = 0;
      }      
      LCD_DispChar_EN ( usX, usY, * pStr, usColor_Background, usColor_Foreground,font);
      pStr ++;      
      usX += 12;
    }
	}
}

/**
  * 函数功能: 在 LCD 显示器上显示数值
  * 输入参数: usX：在特定扫描方向下字符的起始X坐标
  *           usY ：在特定扫描方向下该点的起始Y坐标
  *           num ：数值大小
  *           usColor_Background ：选择英文字符的背景色
  *           usColor_Foreground ：选择英文字符的前景色
  *           font：字体选择
  *             参数：USB_FONT_16 ：16号字体
  *                   USB_FONT_24 ：24号字体 
  * 返 回 值: 无
  * 说    明：该函数必须与ascii.h内容对应使用
  */
void LCD_DisNum(uint16_t usX, uint16_t usY, uint32_t num, uint16_t usColor_Background, uint16_t usColor_Foreground, USB_FONT_Typdef font)
{
	uint32_t length = 0, temp = 0;
	temp = num;
	
	if( temp == 0 )
	{
		LCD_DispChar_EN(usX, usY, '0', usColor_Background, usColor_Foreground, font);
		return;
	}
	
	while( temp )
	{// 得到num的长度
		temp /= 10;
		length ++;
	}
	
	while( num )
	{
		/* 从个位开始显示 */
		if(font == USB_FONT_16)
			LCD_DispChar_EN((usX+8*(length--)-8), usY, (num%10)+'0', usColor_Background, usColor_Foreground, font);
		else if(font == USB_FONT_24)
			LCD_DispChar_EN((usX+12*(length--)-12), usY, (num%10)+'0', usColor_Background, usColor_Foreground, font);
		num /= 10;
	}	
}


#if 0
#if USB_SPIFLASH_CH==1
/**
  * 函数功能: 从串行Flash读取GBK码
  * 输入参数: pBuffer：数据保存地址
  *           gbk ：汉字字符低字节码
  *           font：字体选择
  *           可选值：FONT_16 ：16号字体
  *                   FONT_24 ：24号字体 
  * 返 回 值: uint8_t: 0:读取失败，1：读取成功
  * 说    明：无
  */
static uint8_t GetGBKCode_SPIFLASH(unsigned char* pBuffer,const uint8_t *pstr,USB_FONT_Typdef font)
{
  uint32_t pos;
  uint8_t high8bit,low8bit;
  static uint8_t startflag=1;
  
  /* 检查输入参数是否合法 */
  assert_param(IS_FONT(font));
  
  if(startflag)
  {
    MX_SPIFlash_Init();
    startflag=0;
  }
  
  high8bit=*pstr;
  low8bit=*(pstr+1);
  if(font==USB_FONT_16)
  {
    // 16*16大小的汉字 其字模 占用16*16/8个字节
    pos=((high8bit-0xa1)*94+low8bit-0xa1)*16*16/8;
    SPI_FLASH_BufferRead(pBuffer,GBK_HZ1616_ADDR+pos,32);
  }
  else
  {
    // 24*24大小的汉字 其字模 占用24*24/8个字节
    pos=((high8bit-0xa1)*94+low8bit-0xa1)*24*24/8; 
    SPI_FLASH_BufferRead(pBuffer,GBK_HZ2424_ADDR+pos,72);
  }
  if((pBuffer[0]==0xFF)&&(pBuffer[1]==0xFF))
  {
    return 0;
  }  
  return 1;  
}

#else

/**
  * 函数功能: 从SD卡读取GBK码
  * 输入参数: pBuffer：数据保存地址
  *           gbk ：汉字字符低字节码
  *           font：字体选择
  *           可选值：USB_FONT_16 ：16号字体
  *                   USB_FONT_24 ：24号字体 
  * 返 回 值: uint8_t: 0:读取失败，1：读取成功
  * 说    明：使用该函数之前确保已运行了f_mount(&fs,"0:",1);程序。
  */
static uint8_t GetGBKCode_SD(unsigned char* pBuffer,const uint8_t *pstr,USB_FONT_Typdef font)
{
  uint32_t pos;
  uint8_t high8bit,low8bit;
  FRESULT f_res;
  UINT f_num;
  
  high8bit=*pstr;
  low8bit=*(pstr+1);
  if(font==USB_FONT_16)
  {
    // 16*16大小的汉字 其字模 占用16*16/8个字节
    pos=((high8bit-0xa1)*94+low8bit-0xa1)*16*16/8;
    f_res=f_open(&file_ch,"0:/GBK_HZ1616.FON",FA_OPEN_EXISTING|FA_READ);
  }
  else
  {
    // 24*24大小的汉字 其字模 占用24*24/8个字节
    pos=((high8bit-0xa1)*94+low8bit-0xa1)*24*24/8; 
    f_res=f_open(&file_ch,"0:/GBK_HZ2424.FON",FA_OPEN_EXISTING|FA_READ);
  } 
  if(f_res == FR_OK ) 
  {
    f_lseek (&file_ch, pos);		//指针偏移
    if(font==USB_FONT_16)
    {
      f_res=f_read(&file_ch,pBuffer,32,&f_num); 
    }
    else
    {
      f_res=f_read(&file_ch,pBuffer,72,&f_num);
    }
    f_close(&file_ch);      
    return 1;  
  }    
  else
    return 0;    
}
#endif

/**
  * 函数功能: 在 LCD 显示器上显示一个中文
  * 输入参数: usX：在特定扫描方向下字符的起始X坐标
  *           usY ：在特定扫描方向下该点的起始Y坐标
  *           pstr: 汉字字符低字节码
  *           usColor_Background ：选择英文字符的背景色
  *           usColor_Foreground ：选择英文字符的前景色
  *           font：字体选择
  *           可选值：USB_FONT_16 ：16号字体
  *                   USB_FONT_24 ：24号字体 
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DispCHAR_CH(uint16_t usX,uint16_t usY,const uint8_t *pstr,uint16_t usColor_Background, uint16_t usColor_Foreground,USB_FONT_Typdef font)
{
	uint8_t ucTemp, ucPage, ucColumn;
  uint8_t gbk_buffer[72];
  
  /* 检查输入参数是否合法 */
  assert_param(IS_FONT(font));
  
	if(font==USB_FONT_16)
  {
    LCD_OpenWindow(usX,usY,16,16);
    LCD_WRITE_CMD(0x2C);
#if USB_SPIFLASH_CH==1
    GetGBKCode_SPIFLASH(gbk_buffer,pstr,USB_FONT_16);
#else
    GetGBKCode_SD(gbk_buffer,pstr,USB_FONT_16);
#endif 
    
    for(ucPage=0;ucPage<32;ucPage++)
    {
      ucTemp=gbk_buffer[ucPage];		
      for(ucColumn=0;ucColumn<8;ucColumn++)
      {
        if(ucTemp&0x01)
          LCD_WRITE_DATA(usColor_Foreground);			
        else
          LCD_WRITE_DATA(usColor_Background);								
        ucTemp >>= 1;					
      }
    }    
  }
  else
  {
    LCD_OpenWindow(usX,usY,24,24);
    LCD_WRITE_CMD(0x2C);
#if USB_SPIFLASH_CH==1
    GetGBKCode_SPIFLASH(gbk_buffer,pstr,USB_FONT_24);
#else
    GetGBKCode_SD(gbk_buffer,pstr,USB_FONT_24);
#endif    
    
    for(ucPage=0;ucPage<72;ucPage++)
    {
      ucTemp=gbk_buffer[ucPage];		
      for(ucColumn=0;ucColumn<8;ucColumn++)
      {
        if(ucTemp&0x01)
          LCD_WRITE_DATA(usColor_Foreground);			
        else
          LCD_WRITE_DATA(usColor_Background);								
        ucTemp >>= 1;					
      }
    } 
  }	
}

/**
  * 函数功能: 在 LCD 显示器上显示一串中文
  * 输入参数: usX：在特定扫描方向下字符的起始X坐标
  *           usY ：在特定扫描方向下该点的起始Y坐标
  *           pstr: 汉字字符低字节码
  *           usColor_Background ：选择英文字符的背景色
  *           usColor_Foreground ：选择英文字符的前景色
  *           font：字体选择
  *           可选值：USB_FONT_16 ：16号字体
  *                   USB_FONT_24 ：24号字体 
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DispString_CH(uint16_t usX,uint16_t usY,const uint8_t *pstr,uint16_t usColor_Background, uint16_t usColor_Foreground,USB_FONT_Typdef font)
{
  /* 检查输入参数是否合法 */
  assert_param(IS_FONT(font));
  
  while(*pstr != '\0')
	{
    if(font==USB_FONT_16)
    {
      if((usX+16)>LCD_DEFAULT_WIDTH)
      {
        usX = 0;
        usY += 16;
      }      
      if((usY+16)>LCD_DEFAULT_HEIGTH)
      {
        usX=0;
        usY=0;
      }      
      LCD_DispCHAR_CH(usX,usY,pstr,usColor_Background,usColor_Foreground,USB_FONT_16);
      pstr+=2;      
      usX+=16;
    }
    else
    {
      if((usX+24)>LCD_DEFAULT_WIDTH)
      {
        usX = 0;
        usY += 24;
      }      
      if((usY+24)>LCD_DEFAULT_HEIGTH)
      {
        usX=0;
        usY=0;
      }      
      LCD_DispCHAR_CH(usX,usY,pstr,usColor_Background,usColor_Foreground,USB_FONT_24);
      pstr+=2;      
      usX+=24;
    }
	}  
}

/**
  * 函数功能: 在 LCD 显示器上显示一串中英文
  * 输入参数: usX：在特定扫描方向下字符的起始X坐标
  *           usY ：在特定扫描方向下该点的起始Y坐标
  *           pstr: 汉字字符低字节码
  *           usColor_Background ：选择英文字符的背景色
  *           usColor_Foreground ：选择英文字符的前景色
  *           font：字体选择
  *           可选值：USB_FONT_16 ：16号字体
  *                   USB_FONT_24 ：24号字体 
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DispString_EN_CH(uint16_t usX,uint16_t usY,const uint8_t *pstr,uint16_t usColor_Background, uint16_t usColor_Foreground,USB_FONT_Typdef font)
{
  /* 检查输入参数是否合法 */
  assert_param(IS_FONT(font));
  
  while(*pstr != '\0')
	{
    if(*pstr<=0x7f)
    {
      if(font==USB_FONT_16)
      {
        if((usX+8)>LCD_DEFAULT_WIDTH)
        {
          usX = 0;
          usY += 16;
        }      
        if((usY+16)>LCD_DEFAULT_HEIGTH)
        {
          usX=0;
          usY=0;
        }      
        LCD_DispChar_EN(usX,usY,*pstr,usColor_Background,usColor_Foreground,USB_FONT_16);
        pstr++;      
        usX+=8;
      }
      else
      {
        if((usX+12)>LCD_DEFAULT_WIDTH)
        {
          usX=0;
          usY+=24;
        }      
        if((usY+24)>LCD_DEFAULT_HEIGTH)
        {
          usX=0;
          usY=0;
        }      
        LCD_DispChar_EN(usX,usY,*pstr,usColor_Background,usColor_Foreground,USB_FONT_24);
        pstr++;      
        usX+=12;
      }
    }
    else
    {
      if(font==USB_FONT_16)
      {
        if((usX+16)>LCD_DEFAULT_WIDTH)
        {
          usX = 0;
          usY += 16;
        }      
        if((usY+16)>LCD_DEFAULT_HEIGTH)
        {
          usX=0;
          usY=0;
        }      
        LCD_DispCHAR_CH(usX,usY,pstr,usColor_Background,usColor_Foreground,USB_FONT_16);
        pstr+=2;      
        usX+=16;
      }
      else
      {
        if((usX+24)>LCD_DEFAULT_WIDTH)
        {
          usX = 0;
          usY += 24;
        }      
        if((usY+24)>LCD_DEFAULT_HEIGTH)
        {
          usX=0;
          usY=0;
        }      
        LCD_DispCHAR_CH(usX,usY,pstr,usColor_Background,usColor_Foreground,USB_FONT_24);
        pstr+=2;      
        usX+=24;
      }
    }
	}  
}
#endif
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
