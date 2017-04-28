#ifndef __I2C_H
#define __I2C_H

#include <reg52.h>
#include <intrins.h>
//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

#define i2cDelay() {_nop_();_nop_();_nop_();_nop_();}

//定义使用的IO
sbit i2cSCL = P3^3;
sbit i2cSDA = P3^4;

//定义函数
void  i2cStart();
void  i2cStop();
uchar i2cSendByte(uchar udata);
uchar i2cReadByte(uchar ack);

uchar readByteAt24c08(uchar address);
void  writeByteAt24c08(uchar address, uchar udata);

uint  readUIntAt24c08(uchar address);
void  writeUIntAt24c08(uchar address, uint udata);

void  write24c08(uchar *buf, uchar address, uchar len);
void  read24c08(uchar *buf, uchar address, uchar len);

#endif