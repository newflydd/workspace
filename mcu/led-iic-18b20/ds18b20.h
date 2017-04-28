#ifndef _DS18B20_
#define _DS18B20_

#include <reg52.h>
#include <intrins.h>

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

sbit DS18B20_DQ = P3^2;

void  delayX10us(uchar);
uchar ds18b20Init();
void  ds18b20WriteByte(uchar dat);
uchar ds18b20ReadByte();
uchar ds18b20Start();
uchar ds18b20ReadTemperature();
int   ds18b20GetTemperature();
float ds18b20GetTemperatureFromInt(int);

#endif