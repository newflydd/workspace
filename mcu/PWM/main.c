#include <STC15F2K.h>

/**
 * CCP:Capture Compare PWM
 * PCA:可编程计数器阵列
 * ACC和P_SW1的作用是CCP通道选择，也就是CCP0,CCP1,CCP3分别作用的引脚（CCP是复用的，有多种选择）
 */
#define FOSC 19456000L
#define CCP_S0	0x10 	//P_SW1.4
#define CCP_S1	0x20	//P_SW1.5

int main(){
	ACC		=	P_SW1;
	ACC		&=	~(CCP_S0|CCP_S1);		//CCP_S0=0, CCP_S1=0
	P_SW1	=	ACC;					//P1.1:CCP0 , P1.0:CCP1 , P3.7:CCP2

	/**
	 下面复用引脚可供选择
	ACC		=	P_SW1;
	ACC		&=	~(CCP_S0|CCP_S1);		//CCP_S0=0, CCP_S1=0
	ACC     |=  CCP_S0;					//CCP_S0=1, CCP_S1=0
	P_SW1	=	ACC;					//P3.5:CCP0_2 , P3.6:CCP1_2 , P3.7:CCP2_2

	ACC		=	P_SW1;
	ACC		&=	~(CCP_S0|CCP_S1);		//CCP_S0=0, CCP_S1=0
	ACC     |=  CCP_S1;					//CCP_S0=0, CCP_S1=1
	P_SW1	=	ACC;					//P2.5:CCP0_3 P2.6:CCP1_3 P2.7:CCP2_3
	*/
	

	CCON = 0;		//初始化PCA控制寄存器
	CL   = 0;		//复位PCA
	CH   = 0;
	CMOD = 0x0A;	//设置PCA时钟源
					//CMOD:8bit:CIDL - - - CPS2 CPS1 CPS0 ECF
					//CIDL:空闲模式下是否停止PCA计数的控制位，CIDL = 0时，空闲模式下PCA继续工作。这里0
					//ECF：PCA技术溢出中断使能位，ECF = 1时，允许寄存器CCON中CF位的中断。这里0
					//CPS2 CPS1 CPS0 选择CCP时钟源
					// 0    0     0  系统时钟，SYSclk / 12
					// 0    0     1  系统时钟，SYSclk / 2  
					// 0    1     0  定时器0的溢出脉冲，T0可工作在1T模式下，因此可以达到1个时钟周期就溢出，从而达到最高频率SYSclk，通过改变T0的溢出率，可以实现PWM的频率调整
					// 0    1     1  ECI脚的外部时钟
					// 1    0     0  系统时钟，SYSclk
					// 1    0     1  系统时钟，SYSclk / 4 这里选择此模式
					// 1    1     0  系统时钟，SYSclk / 6
					// 1    1     1  系统时钟，SYSclk / 8

	PCA_PWM0	=	0x40;		//7位，频率 = 19456000/4/128 = 38000
	CCAP0H = CCAP0L = 0x59;		//占空比 = (0x7F - 0x59) / 0x7F = (127 - 89)/127 = 29.9%
	CCAPM0 = 0x42;

	// PCA_PWM0	=	0x00;		//8位，频率 = 19456000/2/256 = 38000
	// CCAP0H = CCAP0L = 0xB3;		//占空比 = (0xFF - 0xB3) / 0xFF = (255 - 179)/255 = 29.8%
	// CCAPM0 = 0x42;

	// PCA_PWM1	=	0x40;		//7位，频率 = 19456000/2/128 = 76000
	// CCAP1H = CCAP1L = 0x20;		//占空比 = (0x7F - 0x20) / 0xFF = (127 - 32)/127 = 74.8%
	// CCAPM1 = 0x42;

	// PCA_PWM2	=	0x80;		//6位，频率 = 19456000/2/64  = 152000
	// CCAP2H = CCAP2L = 0x20;		//占空比 = (0x3F - 0x20) / 0x3F = (63 - 32)/63 = 49.2%
	// CCAPM2 = 0x42;

	CR = 1;						//PCA定时器开始工作

	while(1){

	}
}