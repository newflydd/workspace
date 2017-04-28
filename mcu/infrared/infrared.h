/**
 * 使用外部中断测试HS0038一体化红外接收头
 * 51单片机P3.2是INT0，P3.3是INT1
 * P3.4是T0，P3.5是T1
 * INT0和INT1是外部中断
 * T0和T1是定时器
 */
#include <reg52.h>

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

sbit IR_INPUT = P3^3;				/* 使用外部中断INT1接收红外信号 */

void InitInfrared();
uint GetLowTime();
uint GetHighTime();
void SendInfraredSignal();

void InitUART();

