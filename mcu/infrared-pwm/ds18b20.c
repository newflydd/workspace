#include "ds18b20.h"

void ds18b20WriteByte(u8 dat);
u8   ds18b20ReadByte();
u8   ds18b20Start();
u8   ds18b20ReadTemperature();

/**
 *  初始化DS18B20
 *  0:成功
 *  1:失败
 */
u8 ds18b20Init(){
    u16 dt = 0;
    EA = 0;
    
    DS18B20_DQ = 1;
    Delay(15);
    DS18B20_DQ = 0;             //拉低500us
    Delay(740);
    DS18B20_DQ = 1;                        //拉高释放
    while(DS18B20_DQ || dt < 50){          //等待总线被18B20拉低60-240us响应
        dt++;
        if(dt > 32767){
            EA = 1;
            return 1;                      //如果5ms内没有拉低的响应，则返回失败
        }
    }
    dt = 0;
    while(!DS18B20_DQ){                    //等待18B20复位
        dt++;
        if(dt > 32767){
            EA = 1;
            return 1;
        }
    }

    EA = 1;
    return 0;
}

/**
 * 向DS18B20发送数据
 * @param  dat [需要发送的字节]
 * @return     []
 */
void  ds18b20WriteByte(uchar dat){
    u8 mask;
    
    EA = 0;
    for(mask=0x01; mask!= 0; mask<<=1)
    {
        DS18B20_DQ = 0;
        Delay(1);
        if((mask&dat) == 0)
            DS18B20_DQ = 0;
        else
            DS18B20_DQ = 1;
        Delay(90);
        DS18B20_DQ = 1;
    }

    EA = 1;
}

/**
 * 从DS18B20中读取字节
 * @return [读取的字节]
 */
uchar ds18b20ReadByte(){
    u8 dat;
    u8 mask;

    EA = 0;

    for(mask=0x01; mask!=0; mask<<=1)
    {
        DS18B20_DQ = 0;
        Delay(2);
        DS18B20_DQ = 1;
        Delay(2);
        if(!DS18B20_DQ)
            dat &= ~mask;
        else
            dat |= mask;
        Delay(90);
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
    Delay(90);  //延时150us
    ds18b20WriteByte(0xCC);
    ds18b20WriteByte(0xBE);
    return 0;   
}

/**
 * public 公开函数，读取DS28B02整型温度
 * @return [整型变量，每个间隔代表0.0625摄氏度]
 */
int Ds18b20GetTemperature(){
    int temp;
    u8 tempH,tempL;  //高低字节
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
int Ds18b20GetTemperatureFromInt(int temprInt){
    int result;
    if(temprInt < 0){
        //读取的值是实际温度的补码
        result = temprInt - 1;
        result = ~temprInt;
        result = 0 - (result * 625);
    }else{
        result = temprInt * 625;
    }
    
    return result;
}
