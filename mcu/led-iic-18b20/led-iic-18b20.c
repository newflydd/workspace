//#include <reg51.h>
#include "iic.h"
#include "1602lcd.h"
#include "ds18b20.h"

/**
 *	四位数码管，显示递增数字，每秒递增1，通过PC可以发送串口数据给单片机
 */

#define delay10ms() {for(i = 0; i < 2500; i++)i2cDelay();}

sbit p07 = P0^7;

sbit p2_0 = P2^0;
sbit p2_1 = P2^1;
sbit p2_2 = P2^2;
sbit p2_3 = P2^3;
sbit p2_4 = P2^4;
sbit p2_5 = P2^5;
sbit p2_6 = P2^6;
sbit p2_7 = P2^7;

void display(unsigned char pos);

code unsigned char poss[] = {0xF1, 0xF2, 0xF4, 0xF8};
code unsigned char nums[] = {0xA0, 0xBB, 0x62, 0x2A, 0x39, 0x2C, 0x25, 0xBA, 0x20, 0x38, 0x00};
unsigned char show[] = {2,0,1,7};
unsigned char counter = 0;
unsigned char buff[4];
unsigned pos = 0;
int  number  = 8888;
int  i = 0;

//初始化时间中断的定时器T0
void init_timer1(){
	TMOD |= 0x01;
	TMOD &= 0xFD;		//设置定时器0，而不影响TMOD的其他位，工作在模式1，16位计数器
	TH0 = 0xFC;
	TL0 = 0x67;			//FC67表示1ms
	TR0 = 1;				//TCON中的TR0，定时器1开始计时
}

//初始化串口中断的定时器T1
void init_com(){
	SCON =  0x50;
	TMOD |= 0x20;
	TMOD &= 0xEF;		//设置定时器1，工作在2模式，串口中断使用
	TH1 = 0xFD;
	TL1 = 0xFD;			//波特率9600
	TR1 = 1;			//TCON中的TR1，定时器1开始计时
}

void init_interrupt(){
	ET0 = 1;				//定时器T0打开
	ES  = 1;				//串口中断打开
	EA  = 1;				//总开关打开
}

void main(void){
	int temperatureInt;
	float temperature;
	int kk;
	p07 = 1;
	
	init_timer1();
	init_com();
	init_interrupt();
	
	//writeByteAt24c08(0x02, 0xCC);
	//number = readByteAt24c08(0x02);
	//i2cDelay();
	//writeUIntAt24c08(0x04, 9876);
	
	//delay10ms();
	
	number = readUIntAt24c08(0x04);
	//number = readByteAt24c08(0x04);

	lcdInit();
	lcdWriteData('I');
	lcdWriteData('L');
	lcdWriteData('O');
	lcdWriteData('V');
	lcdWriteData('E');
	lcdWriteData('U');
	lcdWriteCmd(0x8d);	//指针起始位置为第一排左边
	lcdWriteData(0xdf);
	lcdWriteData('C');
	while(1){
		temperatureInt = ds18b20GetTemperature();
		temperature = ds18b20GetTemperatureFromInt(temperatureInt);
		lcdShowFloat(temperature, 0x86);
		lcdWriteData(0xdf);
		lcdWriteData('C');
		for(kk = 0; kk < 50; kk++)
			delay10ms();
	}				
}

//定时器T0的时间中断
void timer1Callback() interrupt 1{
	counter++;
		
	if(counter == 10){
		//if(p07 == 1){
		//	number--;
		//	if(number < 0)
		//		number = 9999;
		//}
		
		counter = 0;
	}
	
	TH0 = 0xFC;
	TL0 = 0x67;
	
	display(counter % 4 + 1);
}


//串口中断接受函数
void comReceiveCallback() interrupt 4{
	unsigned char temp;
	if(TI){				//本次中断是发送中断
		TI = 0;			//发送完了清零
		REN = 1;		//发送完了再接受
	}
	
	if(RI){				//本次中断是接受中断
		RI = 0;			//接受完了清零
		temp = SBUF;
		if(temp == 'S'){
			pos = 0;
		}else if(temp == 'E'){
			number = buff[0] * 1000 + buff[1] * 100 + buff[2] * 10 + buff[3];
		}else{
			buff[pos] = temp - 48;
			pos++;
		}
	}
}

void display(unsigned char pos){
	int n;
	
	switch(pos){
		case 1:
			n = number / 1000;
			break;
		case 2:
			n = number / 100 % 10;
			break;
		case 3:
			n = number / 10 % 10;
			break;
		case 4:
			n = number % 10;
			break;
	}
	P0 = poss[pos - 1];
	P2 = nums[n];
}