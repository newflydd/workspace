#include<reg51.h>

/**
 *	四位数码管，显示递增数字，每秒递增1
 */

sbit p04 = P0^4;

sbit p2_0 = P2^0;
sbit p2_1 = P2^1;
sbit p2_2 = P2^2;
sbit p2_3 = P2^3;
sbit p2_4 = P2^4;
sbit p2_5 = P2^5;
sbit p2_6 = P2^6;
sbit p2_7 = P2^7;

code unsigned char poss[] = {0x01, 0x02, 0x04, 0x08};
code unsigned char nums[] = {0xA0, 0xBB, 0x62, 0x2A, 0x39, 0x2C, 0x25, 0xBA, 0x20, 0x38, 0x00};
unsigned char show[] = {2,0,1,7};
unsigned char counter = 0;
unsigned int  number  = 8;

void delay(unsigned ms);
void display(unsigned char pos);
void delay20ms(int);

void main(void){
	TMOD = 0x01;	// 使用定时0
	TH0 = 0xB8;
	TL0 = 0x00;
	TR0 = 1;
	
	while(1){
		if(counter > 50){
			number --;
			counter = 0;
			if(number == 0)
				number = 9999;
		}
		
		if(number >= 1000){
			display(1);
			delay(4);
		}
		
		if(number >= 100){
			display(2);
			delay(4);
		}
		
		if(number >= 10){
			display(3);
			delay(4);
		}
		
		if(number > 0){
			display(4);
			delay(4);
		}
		
		if(TF0 == 1){
			counter++;
			TH0 = 0xB8;
			TL0 = 0x00;
			TF0 = 0;
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

void delay(unsigned ms){
	unsigned char a, b;

  for (;ms>0;ms--)
	{
		for (b=38;b>0;b--)
		{
			for (a=13;a>0;a--);
		}   
	}
}