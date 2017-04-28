#include "iic.h"

void  i2cStart(){
	i2cSDA = 1; //首先确保SDA、SCL都是高电平
    i2cSCL = 1;
    i2cDelay();
    i2cSDA = 0; //先拉低SDA
    i2cDelay();
    i2cSCL = 0; //再拉低SCL
}

void  i2cStop(){
	i2cSCL = 0; //首先确保SDA、SCL都是低电平
    i2cSDA = 0;
    i2cDelay();
    i2cSCL = 1; //先拉高SCL
    i2cDelay();
    i2cSDA = 1; //再拉高SDA
    i2cDelay();
}

/**
 * 发送一个8位字节
 * @param  udata [需要发送的数据]
 * @return  ack [应答信号，0成功，1失败]
 */
uchar i2cSendByte(uchar udata){
	uchar mask,ack;
	for(mask = 0x80; mask !=0; mask >>= 1){
		i2cSCL = 0;
		if((mask & udata) == 0)
			i2cSDA = 0;
		else
			i2cSDA = 1;

		i2cDelay();
		i2cSCL = 1;
		i2cDelay();
		i2cSCL = 0;
	}
	i2cDelay();
	i2cSDA = 1;
	i2cDelay();
	i2cSCL = 1;
	i2cDelay();
	ack = i2cSDA;
	i2cDelay();
	i2cSCL = 0;
	i2cDelay();

	return ack;
}

uchar i2cReadByte(uchar ack){
	unsigned char mask;
    unsigned char udata;

    i2cSDA = 1;  //首先确保主机释放SDA
    for (mask=0x80; mask!=0; mask>>=1) //从高位到低位依次进行
    {
        i2cDelay();
        i2cSCL = 1;      //拉高SCL
		i2cDelay();
        if(i2cSDA == 0)  //读取SDA的值
            udata &= ~mask; //为0时，dat中对应位清零
        else
            udata |= mask;  //为1时，dat中对应位置1
        i2cDelay();
        i2cSCL = 0;      //再拉低SCL，以使从机发送出下一位
    }
	i2cDelay();
    i2cSDA = ack;   //8位数据发送完后，发送应答信号
    i2cDelay();
    i2cSCL = 1;   //拉高SCL
    i2cDelay();
    i2cSCL = 0;   //再拉低SCL完成应答位，并保持住总线
	i2cDelay();

    return udata;
}

uchar readByteAt24c08(uchar address){
	uchar udata;
	i2cStart();
	i2cSendByte(0x50<<1);
	i2cSendByte(address);
	i2cStart();
	i2cSendByte((0x50<<1) | 0x01);
	udata = i2cReadByte(1);
	i2cStop();
	return udata;
}

void  writeByteAt24c08(uchar address, uchar udata){
	i2cStart();
	i2cSendByte(0x50<<1);
	i2cSendByte(address);
	i2cSendByte(udata);
	i2cStop();
}

uint  readUIntAt24c08(uchar address){
	uint udata;
	uchar d1,d2;

	i2cStart();
	i2cSendByte(0x50<<1);
	i2cSendByte(address);	//先写一个地址进去
	i2cStart();
	i2cSendByte((0x50<<1) | 0x01);	//再开始读
	d1 = i2cReadByte(0);
	d2 = i2cReadByte(1);
	i2cStop();
	udata = (d1<<8)|d2;
	return udata;
}

void  writeUIntAt24c08(uchar address, uint udata){
	i2cStart();
	i2cSendByte(0x50<<1);	//7位位选+1位读写
	i2cSendByte(address);
	i2cSendByte((uchar)(udata>>8));
	i2cSendByte((uchar)(udata));
	i2cStop();
}

void  write24c08(uchar *buf, uchar address, uchar len){

}

void  read24c08(uchar *buf, uchar address, uchar len){

}