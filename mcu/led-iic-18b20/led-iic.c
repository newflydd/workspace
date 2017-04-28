//#include <reg51.h>
#include "iic.h"
#include "1602lcd.h"
#include "ds18b20.h"

/**
 *	ËÄÎ»ÊýÂë¹Ü£¬ÏÔÊ¾µÝÔöÊý×Ö£¬Ã¿ÃëµÝÔö1£¬Í¨¹ýPC¿ÉÒÔ·¢ËÍ´®¿ÚÊý¾Ý¸øµ¥Æ¬»ú
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

//³õÊ¼»¯Ê±¼äÖÐ¶ÏµÄ¶¨Ê±Æ÷T0
void init_timer1(){
	TMOD |= 0x01;
	TMOD &= 0xFD;		//ÉèÖÃ¶¨Ê±Æ÷0£¬¶ø²»Ó°ÏìTMODµÄÆäËûÎ»£¬¹¤×÷ÔÚÄ£Ê½1£¬16Î»¼ÆÊýÆ÷
	TH0 = 0xFC;
	TL0 = 0x67;			//FC67±íÊ¾1ms
	TR0 = 1;				//TCONÖÐµÄTR0£¬¶¨Ê±Æ÷1¿ªÊ¼¼ÆÊ±
}

//³õÊ¼»¯´®¿ÚÖÐ¶ÏµÄ¶¨Ê±Æ÷T1
void init_com(){
	SCON =  0x50;
	TMOD |= 0x20;
	TMOD &= 0xEF;		//ÉèÖÃ¶¨Ê±Æ÷1£¬¹¤×÷ÔÚ2Ä£Ê½£¬´®¿ÚÖÐ¶ÏÊ¹ÓÃ
	TH1 = 0xFD;
	TL1 = 0xFD;			//²¨ÌØÂÊ9600
	TR1 = 1;				//TCONÖÐµÄTR1£¬¶¨Ê±Æ÷1¿ªÊ¼¼ÆÊ±
}

void init_interrupt(){
	ET0 = 1;				//¶¨Ê±Æ÷T0´ò¿ª
	ES  = 1;				//´®¿ÚÖÐ¶Ï´ò¿ª
	EA  = 1;				//×Ü¿ª¹Ø´ò¿ª
}

void main(void){
	int k;
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
	lcdWriteData(' ');
	lcdWriteData('L');
	lcdWriteData('O');
	lcdWriteData('V');
	lcdWriteData('E');
	lcdWriteData(' ');
	lcdWriteData('U');
	
	while(1){
		float temprerature = ds18b20GetTempreratureFromInt(ds18b20GetTemprerature());
		lcdShowFloat(temprerature, 0x00);
	}				
}

//¶¨Ê±Æ÷T0µÄÊ±¼äÖÐ¶Ï
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


//´®¿ÚÖÐ¶Ï½ÓÊÜº¯Êý
void comReceiveCallback() interrupt 4{
	unsigned char temp;
	if(TI){				//±¾´ÎÖÐ¶ÏÊÇ·¢ËÍÖÐ¶Ï
		TI = 0;			//·¢ËÍÍêÁËÇåÁã
		REN = 1;		//·¢ËÍÍêÁËÔÙ½ÓÊÜ
	}
	
	if(RI){			//±¾´ÎÖÐ¶ÏÊÇ½ÓÊÜÖÐ¶Ï
		RI = 0;		//½ÓÊÜÍêÁËÇåÁã
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