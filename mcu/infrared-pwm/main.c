/**
 * STC15F2K60S系列，红外学习和发送遥控器
 * 主频：19.456M（能精确实现38KHz调制波形）
 * 
 * 红外发送功能：
 * P02发送sendCmd1脉冲信号，通过与门，与PWM混合，发往红外发射器
 * P03接微动开关，下拉输入，平时低电平，有信号时高电平
 *
 * 红外解析功能：
 * INT0和INT1是外部中断
 * P3.2是INT0，P3.3是INT1
 * 本例通过INT0来接收HS0038的解调信号
 * 通过UART向上位机发送接收的红外数据
 */
#include <STC15F2K.h>

#define FOSC 19456000L
#define CCP_S0	0x10 	//P_SW1.4
#define CCP_S1	0x20	//P_SW1.5

void PWM0Init(void);				/* PWM0初始化，主频19.456M，分频为38KHz */
void Uart1Init(void);				/* 9600bps@19.456MHz */
void T0Init(void);					/* 计时器0初始化 */
void IT0Init();						/* 外部中断IT0初始化 */

void sendInfraredByte(u8 dat);		/* 按字节发送红外数据 */
void SendInfraredSignal();			/* 发送红外指令 */

void delay(u16 us);					/* 延时函数 */

/* 红外发送功能 */
sbit  SendDataPin = P0^2;	/* 红外数据发射引脚，与PWM通过74HC08混合形成调制信号 */
sbit  SendCmdPin  = P0^3;	/* 定义发射按钮微动开关引脚，上拉电阻维持高电平，微动按下有个下降沿触发 */
u8 sendCmd1[] = {0xC3, 0xE0, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xA0, 0xF1};

/* 红外解调功能 */
u8 idata buffer[128];				/* 红外接收数据区，最大长度128字节，完全满足各种红外遥控器指令集 */
u8 bufferLength = 0;				/* 红外接收数据区长度 */
sbit IR_INPUT = P3^2;				/* 使用外部中断INT0接收红外信号 */

u8  u8temp;
u16 u16temp;

int main(){
	EA = 1;							/* 总中断使能 */

	PWM0Init();						/* PWM0初始化，主频19.456M，分频为38KHz */
	Uart1Init();					/* 串口传输（串口1）初始化 */
	T0Init();						/* 计时器0初始化 */
	IT0Init();						/* 外部中断IT0初始化 */
	
	IR_INPUT	= 1;				/* 外部中断置高电平  */
	SendCmdPin 	= 1;				/* 使SendCmdPin可读 */

	while(1){
		for(u8temp = 0; u8temp < 120; u8temp++)
			delay(13333);
		SendInfraredSignal();
	}
}

/* PWM0初始化，主频19.456M，分频为38KHz */
void PWM0Init(void){
	ACC		=	P_SW1;
	ACC		&=	~(CCP_S0|CCP_S1);		//CCP_S0=0, CCP_S1=0
	P_SW1	=	ACC;					//P1.1:CCP0 , P1.0:CCP1 , P3.7:CCP2

	CCON = 0;		//初始化PCA控制寄存器
	CL   = 0;		//复位PCA
	CH   = 0;
	CMOD = 0x0A;	//设置PCA时钟源，sysclk/4

	PCA_PWM0	=	0x40;		//7位，频率 = 19456000/4/128 = 38000
	CCAP0H = CCAP0L = 0x59;		//占空比 = (0x7F - 0x59) / 0x7F = (127 - 89)/127 = 29.9%
	CCAPM0 = 0x42;

	CR = 1;						//PCA定时器开始工作
}

/* 串口传输（串口1）初始化 */
void Uart1Init(void)		//9600bps@19.456MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1  = 0x05;		//设定定时初值
	TH1  = 0xFE;		//设定定时初值
	ET1  = 0;			//禁止定时器1中断
	TR1  = 1;			//启动定时器1
}

/* 计时器0初始化 */
void T0Init(void){
	TMOD &= 0xF0;
	TMOD |= 0x01;				/* 将TMOD 高4位不变，低4位置0001，使用定时器T0 */

	TR0 = 0;					/* TR:计时使能，置1计时，置0停止 */
	ET0 = 0;					/* ET:定时器中断使能，置1使能，置0失能 */
}

/* 外部中断IT0初始化 */
void IT0Init(){
	IT0 = 1;						/* IT:外部中断方式选择，0:低电平触发，1:下降沿触发 */
	EX0 = 1;						/* EX:外部中断使能，1:使能，0:失能 */
}

/**
 * 微秒级别延时
 * 			67.5us = 100
 * 9ms    = 9000us = 13333
 * 4.5ms  = 4500us = 6667
 * 			560us  = 830
 *    		1690us = 2504
 */
void delay(u16 us){
	while(us)us--;
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

u16 GetLowTime(){
	TH0 = 0;
	TL0 = 0;				/* 计数器0的高低4位，初始设为0 */

	TR0 = 1;				/* 计数器0计时使能 */
	while(!IR_INPUT){
		if(TH0 > 0x40)		/* 如果高4位大于某一阈值，表示异常 */
			break;
	}
	TR0 = 0;				/* 计数器失能 */
	return TH0 * 256 + TL0;	/* 高4位*256 + 低4位 = 计时开始后跑过的时间 */
}

/**
 * INT0中断入口
 * interrupt关键字用法
 * interrupt 0 : 外部中断0 P3.2
 * interrupt 1 : 定时器中断0
 * interrupt 2 : 外部中断1 P3.3
 * interrupt 3 : 定时器中断1
 * interrupt 4 : 串口中断
 */
void INT0_Routine() interrupt 0{
	u16temp = GetLowTime();

	if(u16temp < 9333 || u16temp > 17333){				/* 判断是否在9ms以内 */
		IE1 = 0;
		return;
	}
}
