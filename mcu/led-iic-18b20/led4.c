#include<reg51.h>

/**
 *	��λ����ܣ���ʾ�������֣�ÿ�����1��ͨ��PC���Է��ʹ������ݸ���Ƭ��
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
unsigned char buff[4];
unsigned pos = 0;
int  number  = 9999;

//��ʼ��ʱ���жϵĶ�ʱ��T0
void init_timer1(){
	TMOD |= 0x01;
	TMOD &= 0xFD;		//���ö�ʱ��0������Ӱ��TMOD������λ��������ģʽ1��16λ������
	TH0 = 0xFC;
	TL0 = 0x67;			//FC67��ʾ1ms
	TR0 = 1;				//TCON�е�TR0����ʱ��1��ʼ��ʱ
}

//��ʼ�������жϵĶ�ʱ��T1
void init_com(){
	SCON =  0x50;
	TMOD |= 0x20;
	TMOD &= 0xEF;		//���ö�ʱ��1��������2ģʽ�������ж�ʹ��
	TH1 = 0xFD;
	TL1 = 0xFD;			//������9600
	TR1 = 1;				//TCON�е�TR1����ʱ��1��ʼ��ʱ
}

void init_interrupt(){
	ET0 = 1;				//��ʱ��T0��
	ES  = 1;				//�����жϴ�
	EA  = 1;				//�ܿ��ش�
}

void main(void){
	p07 = 1;
	init_timer1();
	init_com();
	init_interrupt();
	
	while(1){
	}				
}

//��ʱ��T0��ʱ���ж�
void timer1Callback() interrupt 1{
	counter++;
		
	if(counter == 10){
		if(p07 == 1){
			number--;
			if(number < 0)
				number = 9999;
		}
		
		counter = 0;
	}
	
	TH0 = 0xFC;
	TL0 = 0x67;
	
	display(counter % 4 + 1);
}


//�����жϽ��ܺ���
void comReceiveCallback() interrupt 4{
	unsigned char temp;
	if(TI){				//�����ж��Ƿ����ж�
		TI = 0;			//������������
		REN = 1;		//���������ٽ���
	}
	
	if(RI){			//�����ж��ǽ����ж�
		RI = 0;		//������������
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