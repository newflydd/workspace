/**
 * 将红外数据接收到后，发往UART
 * 计时使用定时器T0
 * UART使用定时器T1
 */

#include "infrared.h"
#include "intrins.h"

uchar idata buffer[128];
uchar bufferLength = 0;
volatile unsigned char sending;
uchar sendCmd[] = {0xC3, 0xE1, 0x00, 0x00, 0x04, 0x02, 0x01, 0x00, 0x00, 0x04, 0x00, 0xA0, 0xF2, 0x00};

sbit  SendDataPin = P1^0;	/* 定义红外发射开关引脚 */
sbit  SendCmdPin  = P1^1;	/* 定义发射按钮微动开关引脚，上拉电阻维持高电平，微动按下有个下降沿触发 */
uchar canSend     = 1;		/* 表示是否可以接收发送指令，上升沿后才可发送 */

uint  uintBuffer;
uchar ucharBuffer;
uchar shake;				/* 去抖变量 */

void InitBuffer(){
	uchar i;
	for(i = 0; i < 128; i++){
		buffer[i] = 0x00;
	}
}

void main(){
	EA = 1;				/* 总中断使能 */
	canSend = 1;
	
	InitBuffer();
	InitInfrared();
	InitUART();

	while(1){
		/* 微动开关引脚置位，保证其可读 */
		SendCmdPin = 1;

		ucharBuffer = SendCmdPin;

		/* 第一阶段，canSend置位 */
		if(canSend == 0 && SendCmdPin == 1){
			/* 不可发送时，接收到上升信号，此时canSend置为可发送 */
			shake = 10;
			while(SendCmdPin && shake){
				shake--;
			}
			if(!shake)							/* 去抖 */
				canSend = 1;
			continue;
		}

		/* 第二阶段，发送红外 */
		if(canSend == 1 && SendCmdPin == 0){
			shake = 10;
			while(!SendCmdPin && shake){
				shake--;
			}
			if(!shake){							/* 去抖 */
				canSend = 0;
				SendInfraredSignal();
			}
			continue;
		}
	}
}

/**
 * interrupt 0 : 外部中断0 P3.2
 * interrupt 1 : 定时器中断0
 * interrupt 2 : 外部中断1 P3.3
 * interrupt 3 : 定时器中断1
 * interrupt 4 : 串口中断
 */
void EXINT1_ISR() interrupt 2{
	uchar j;
	uint  time;

	time = GetLowTime();
	if(time < 7833 || time > 8755){				/* 判断是否在9ms以内 */
		IE1 = 0;
		return;
	}

	time = GetHighTime();
	if(time < 3686 || time > 4608){				/* 判断是否在4.5ms以内 */
		IE1 = 0;
		return;	
	}

	/* 接收红外字节，直到高电平时间长于1751，延时一会儿发送UART */
	while(1){
		for(j = 0; j < 8; j++){
			time = GetLowTime();
			if(time < 313 || time > 718){				/* 判断是否是560us */
				IE1 = 0;
				return;
			}
			time = GetHighTime();
			if(time > 313 && time < 718){				/* 接收到了0 */
				buffer[bufferLength] = buffer[bufferLength] << 1;
			}else if(time > 1345 && time < 1751){		/* 接收到了1 */
				buffer[bufferLength] = buffer[bufferLength] << 1;
				buffer[bufferLength] |= 0x01;
			}else{										/* >=1751时表示结束位，无信号了 */
				IE1 = 0;
				uintBuffer = 5000;
				while(uintBuffer--);

				for(ucharBuffer = 0; ucharBuffer <= bufferLength; ucharBuffer++){
					SBUF = buffer[ucharBuffer];
					sending = 1;
					while(sending);		/* 发送每个字节后都等UART中断中将sending复位，防止错乱 */
				}			

				bufferLength = 0;
				return;
			}
		}

		bufferLength++;
	}
}

/**
 * 延时微秒级别，公式为延时的微秒数值 * 0.9215
 * @param ns [description]
 */
void delay(uint us){
	while(us)us--;
}

uint GetHighTime(){
	TH0 = 0;
	TL0 = 0;				/* 计数器的高低4位，初始设为0 */

	TR0 = 1;				/* 计数器计时使能 */
	while(IR_INPUT){
		if(TH0 > 0x40)		/* 如果高4位大于某一阈值，表示异常 */
			break;
	}
	TR0 = 0;				/* 计数器失能 */
	return TH0 * 256 + TL0;	/* 高4位*256 + 低4位 = 计时开始后跑过的时间 */
}

uint GetLowTime(){
	TH0 = 0;
	TL0 = 0;				/* 计数器的高低4位，初始设为0 */

	TR0 = 1;				/* 计数器计时使能 */
	while(!IR_INPUT){
		if(TH0 > 0x40)		/* 如果高4位大于某一阈值，表示异常 */
			break;
	}
	TR0 = 0;				/* 计数器失能 */
	return TH0 * 256 + TL0;	/* 高4位*256 + 低4位 = 计时开始后跑过的时间 */
}

/**
 * 计时使用T0
 */
void InitInfrared(){
	IR_INPUT = 1;

	TMOD &= 0xF0;
	TMOD |= 0x01;				/* 将TMOD 高4位不变，低4位置0001，使能定时器T0 */

	TR0 = 0;					/* TR:计时使能，置1计时，置0停止 */
	ET0 = 0;					/* ET:定时器中断使能，置1使能，置0失能 */
	IT1 = 1;					/* IT:外部中断方式选择，0:低电平触发，1:下降沿触发 */
	EX1 = 1;					/* EX:外部中断使能，1:使能，0:失能 */

	SendCmdPin = 1;				/* 微动开关引脚置1，上拉电阻维持，保证其可读 */
	SendDataPin = 0;			/* 发射开关置0，维持低电平输出 */
}

/**
 * 串口通信使用定时器T1
 */
void InitUART(){
	SCON =  0x50;
	
	TMOD |= 0x20;
	TMOD &= 0xEF;		//设置定时器1，工作在2模式，串口中断使用
	
	TH1 = 0xFD;
	TL1 = 0xFD;			//波特率9600
	
	TR1 = 1;			//TCON中的TR1，定时器1开始计时
	ES  = 1;			//串口中断打开

	IP = 0x10;
}

void UartInterrupt() interrupt 4{
	if(TI){				//本次中断是发送中断
		TI = 0;			//发送完了清零
		REN = 1;		//发送完了再接受
		sending = 0;	//清空发射标志
	}
}

/**
 * 红外发射一个字节，从高位向低位发射
 * 0： 560us高电平，560us低电平
 * 1： 560us高电平，1690us低电平
 */
void sendInfraredByte(uchar dat){
	uchar pos2;

	for(pos2 = 0; pos2 < 8; pos2++){
		if((dat & (0x80 >> pos2)) == 0x00){
			/* 发射0 */
			SendDataPin = 1;
			delay(65);
			SendDataPin = 0;
			delay(65);
			SendDataPin = 1;
		}else{
			/* 发射1 */
			SendDataPin = 1;
			delay(65);
			SendDataPin = 0;
			delay(190);
			SendDataPin = 1;
		}
	}
}

/* 发射红外信号 */
void SendInfraredSignal(){
	uchar pos;

	EX1 = 0;

	/* 9ms高电平 */
	SendDataPin = 1;
	delay(1040);

	/* 4.5ms低电平 */
	SendDataPin = 0;
	delay(510);

	/**
	 * 按照数组中的数据依次向外发射
	 */
	for(pos = 0; pos < 14; pos++){
		sendInfraredByte(sendCmd[pos]);
	}

	SendDataPin = 0;

	EX1 = 1;
}
