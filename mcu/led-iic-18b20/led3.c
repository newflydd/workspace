#include<reg51.h>

/**
 *	四位数码管，显示递增数字，每秒递增1
 */

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
int  number  = 9999;

void init_timer1(){
	TMOD |= 0x10;
	TMOD &= 0xDF;		//设置定时器1，而不影响TMOD的其他位
	TH1 = 0xFC;
	TL1 = 0x67;			//FC67表示1ms
	TR1 = 1;				//TCON中的TR1，定时器1开始计时
}

void init_interrupt(){
	ET1 = 1;
	EA  = 1;				//IE中定时器1中断和总开关打开
}

void main(void){
	p07 = 1;
	init_timer1();
	init_interrupt();
	
	while(1){
	}				
}

void timer1Callback() interrupt 3{
	counter++;
		
	if(counter == 10){
		if(p07 == 1){
			number--;
			if(number < 0)
				number = 9999;
		}
		
		counter = 0;
	}
	
	TH1 = 0xFC;
	TL1 = 0x67;
	
	display(counter % 4 + 1);
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