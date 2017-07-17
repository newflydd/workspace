#include <STC15F2K.h>
#include <intrins.h>

#define FOSC 19456000L
#define CCP_S0	0x10 	//P_SW1.4
#define CCP_S1	0x20	//P_SW1.5

//微秒级别延时，67.5us = 100
//9ms    = 9000us = 13333
//4.5ms  = 4500us = 6667
//560us  = 830
//1690us = 2504
void delay(u16 us){
	while(us)us--;
}

int main(){
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

	P00 = 1;
	while(1){
		delay(100);
		P00 = 0;
		delay(100);
		P00 = 1;
	}
}