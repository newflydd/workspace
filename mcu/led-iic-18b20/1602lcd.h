#ifndef __1602LCD__
#define __1602LCD__

#include <reg52.h>
#include <intrins.h>

//---??????---//
#ifndef uchar
#define uchar unsigned char
#endif

#define GPIO_LCD_DATA P1

sbit  GPIO_LCD_RS	= P3^7;
sbit  GPIO_LCD_RW	= P3^6;
sbit  GPIO_LCD_E	= P3^5; 

void delay10ms();
void lcdWriteCmd(uchar cmd);
void lcdWriteData(uchar dat);
void lcdInit();

/**
 * 让1602在一个指定的位置显示浮点数
 * @param dat [浮点数据，精度为小数点后面4位]
 * @param pos [1602的屏幕位置]
 */
void lcdShowFloat(float dat, uchar pos);


#endif