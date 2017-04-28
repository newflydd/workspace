#include<reg51.h>

/**
 *	四位数码管，显示固定数字
 *  使用定时器，每秒闪烁一次
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

unsigned char poss[] = {0x01, 0x02, 0x04, 0x08};
unsigned char nums[] = {0xA0, 0xBB, 0x62, 0x2A, 0x39, 0x2C, 0x25, 0xBA, 0x20, 0x38};
unsigned char show[] = {2,0,1,7};
unsigned char counter = 0;

void delay(unsigned ms);
void display(unsigned char pos);
void delay20ms(int);

void main(void){
	TMOD = 0x01;	// 使用定时0
	TH0 = 0xB8;
	TL0 = 0x00;
	TR0 = 1;
	
	while(1){
		if(counter > 100)
			counter = 0;
		
		display(1);
		delay(3);
		display(2);
		delay(3);
		display(3);
		delay(3);
		display(4);
		delay(3);
		
		if(TF0 == 1){
			counter++;
			TH0 = 0xB8;
			TL0 = 0x00;
			TF0 = 0;
		}
	}				
}

void display(unsigned char pos){
	if(counter <= 50)
		P0 = poss[pos - 1];
	else
		P0 = 0x00;
	P2 = nums[show[pos - 1]];
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