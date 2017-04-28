#include "ds18b20.h"

/* 软件延时函数，延时时间(t*10)us */
void delayX10us(uchar t)
{
    do {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    } while (--t);
}

/**
 *  初始化DS18B20
 *  0:成功
 *  1:失败
 */
uchar ds18b20Init(){
    uint dt = 0;
    
    EA = 0;
    
    DS18B20_DQ = 1;
    delayX10us(1);
    DS18B20_DQ = 0;             //拉低
    delayX10us(50);             //延时>480us 540us
    DS18B20_DQ = 1;             //拉高释放
    //delay(20);                //延时15-60us
    while(DS18B20_DQ){           //等待总线被18B20拉低响应
        dt++;
        if(dt>5000){
            return 1;           //如果5ms内没有拉低的响应，则返回失败
        }
    }
    DS18B20_DQ = 1;

    EA = 1;
    return 0;
}

/**
 * 向DS18B20发送数据
 * @param  dat [需要发送的字节]
 * @return     []
 */
void  ds18b20WriteByte(uchar dat){
    unsigned char mask;
    
    EA = 0;
    for(mask=0x01; mask!= 0; mask<<=1)
    {
        DS18B20_DQ = 0;
        _nop_();
        _nop_();
        if((mask&dat) == 0)
            DS18B20_DQ = 0;
        else
            DS18B20_DQ = 1;
        delayX10us(6);
        DS18B20_DQ = 1;
    }

    EA = 1;
}

/**
 * 从DS18B20中读取字节
 * @return [读取的字节]
 */
uchar ds18b20ReadByte(){
    unsigned char dat;
    unsigned char mask;

    EA = 0;

    for(mask=0x01; mask!=0; mask<<=1)
    {
        DS18B20_DQ = 0;
        _nop_();
        _nop_();
        DS18B20_DQ = 1;
        _nop_();
        _nop_();
        if(!DS18B20_DQ)
            dat &= ~mask;
        else
            dat |= mask;
        delayX10us(6);
    }

    EA = 1;
    return dat;
}

/**
 * 启动温度转化
 * @return []
 */
uchar  ds18b20Start(){
    if(ds18b20Init())
        return 1;
    ds18b20WriteByte(0xCC);
    ds18b20WriteByte(0x44);
    return 0;
}

/**
 * 发送读取命令
 */
uchar ds18b20ReadTemperature(){
    if(ds18b20Init())
        return 1;
    delayX10us(15);  //延时150us
    ds18b20WriteByte(0xCC);
    ds18b20WriteByte(0xBE);
    return 0;   
}

/**
 * public 公开函数，读取DS28B02整型温度
 * @return [整型变量，每个间隔代表0.0625摄氏度]
 */
int ds18b20GetTemperature(){
    int temp;
    uchar tempH,tempL;  //高低字节
    if(ds18b20Start())
        return -55;

    if(ds18b20ReadTemperature())
        return -54;
    // if(ds18b20Start())
    //     return -1;
    
    // if(ds18b20ReadTemperature())
    //     return -1;
    
    tempL = ds18b20ReadByte();  //先读低字节
    tempH = ds18b20ReadByte();  //再读高字节
    //tempL = 0xA2;
    //tempH = 0x00;
    //temp = (tempH<<8) | (tempL);
    temp = tempH;
    temp = temp<<8;
    temp = temp + tempL;
    return temp;
}

/**
 * 工具函数，将DS28B02的整数值转换成精确的符点摄氏温度
 * @param  int [description]
 * @return     [description]
 */
float ds18b20GetTemperatureFromInt(int temprInt){
    float result;
    if(temprInt < 0){
        //读取的值是实际温度的补码
        result = temprInt - 1;
        result = ~temprInt;
        result = 0.0 - (result * 0.0625);
        result = temprInt * 0.0625;
    }else{
        result = temprInt * 0.0625;
    }
    
    return result;
}
