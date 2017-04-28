#include "1602lcd.h"

//===============LCD=======================
void delay10ms(){
	int temp;
	for(temp = 0 ; temp < 250; temp++)
	  _nop_();
}

void lcdWriteCmd(uchar cmd){
	GPIO_LCD_RS = 0;
	GPIO_LCD_RW = 0;
	GPIO_LCD_DATA = cmd;
	delay10ms();
	GPIO_LCD_E = 1;
	delay10ms();
	GPIO_LCD_E = 0;
}

void lcdWriteData(uchar dat){
	GPIO_LCD_RS = 1;
	GPIO_LCD_RW = 0;
	GPIO_LCD_DATA = dat;
	delay10ms();
	GPIO_LCD_E = 1;
	delay10ms();
	GPIO_LCD_E = 0;
}

void lcdInit(){
	lcdWriteCmd(0x38);	//初始化
	lcdWriteCmd(0x0C);	//开显示，不显示光标，不闪烁
	lcdWriteCmd(0x06);	//指针+1，光标+1
	lcdWriteCmd(0x01);	//清屏
	lcdWriteCmd(0x80);	//指针起始位置为第一排左边
}

void lcdShowFloat(float dat, uchar pos){
	long int result = dat * 1000;
	char ch[7] = {' ','0','0','.','0','0','0'};
	if(result < 0)
		ch[0] = '-';

	ch[6] = result % 10 + '0'; result /= 10;
	ch[5] = result % 10 + '0'; result /= 10;
	ch[4] = result % 10 + '0'; result /= 10;

	ch[2] = result % 10 + '0'; result /= 10;
	ch[1] = result % 10 + '0'; result /= 10;

	lcdWriteCmd(pos);
	for(result = 0; result < 7; result++){
		lcdWriteData(ch[result]);
	}
}