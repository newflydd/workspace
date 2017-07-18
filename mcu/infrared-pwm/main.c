/**
 * P00接微动开关，下拉输入，平时低电平，有信号时高电平
 * P01发送sendCmd1脉冲信号，通过与门，与PWM混合，发往红外发射器
 */
#include <STC15F2K.h>
#include <intrins.h>

#define FOSC 19456000L
#define CCP_S0	0x10 	//P_SW1.4
#define CCP_S1	0x20	//P_SW1.5

sbit  SendDataPin = P0^2;	/* 定义红外发射开关引脚 */
sbit  SendCmdPin  = P0^1;	/* 定义发射按钮微动开关引脚，上拉电阻维持高电平，微动按下有个下降沿触发 */

u8 sendCmd1[] = {0xC3, 0xE0, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xA0, 0xF1};

void sendInfraredByte(uchar dat);
void SendInfraredSignal();


//微秒级别延时，67.5us = 100
//9ms    = 9000us = 13333
//4.5ms  = 4500us = 6667
//560us  = 830
//1690us = 2504
void delay(u16 us){
	while(us)us--;
}

int main(){
	u8 delayTimes;

	ACC		=	P_SW1;
	ACC		&=	~(CCP_S0|CCP_S1);		//CCP_S0=0, CCP_S1=0
	P_SW1	=	ACC;					//P1.1:CCP0 , P1.0:CCP1 , P3.7:CCP2

	CCON = 0;		//初始化PCA控制寄存器
	CL   = 0;		//复位PCA
	CH   = 0;
	CMOD = 0x0A;	//设置PCA时钟源

	PCA_PWM0	=	0x40;		//7位，频率 = 19456000/4/128 = 38000
	CCAP0H = CCAP0L = 0x59;		//占空比 = (0x7F - 0x59) / 0x7F = (127 - 89)/127 = 29.9%
	CCAPM0 = 0x42;

	CR = 1;						//PCA定时器开始工作

	SendCmdPin = 1;				//使SendCmdPin可读

	while(1){
		for(delayTimes = 0; delayTimes < 120; delayTimes++)
			delay(13333);
		SendInfraredSignal();
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
			delay(830);
			SendDataPin = 0;
			delay(830);
			SendDataPin = 1;
		}else{
			/* 发射1 */
			SendDataPin = 1;
			delay(830);
			SendDataPin = 0;
			delay(2504);
			SendDataPin = 1;
		}
	}
}

/* 发射红外信号 */
void SendInfraredSignal(){
	uchar pos;

	/* 9ms高电平 */
	SendDataPin = 1;
	delay(13333);

	/* 4.5ms低电平 */
	SendDataPin = 0;
	delay(6667);

	/**
	 * 按照数组中的数据依次向外发射
	 */
	for(pos = 0; pos < 13; pos++){
		sendInfraredByte(sendCmd1[pos]);
	}

	SendDataPin = 1;
	delay(830);
	SendDataPin = 0;

}
