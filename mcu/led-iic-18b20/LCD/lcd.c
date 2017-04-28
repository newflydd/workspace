#include<reg52.h>

#define LCD1602_DB  P1      //data bus ????



sbit LCD1602_RS = P3^7;
sbit LCD1602_RW = P3^6;
sbit LCD1602_EN = P3^5;	 

void InitLcd1602();
void LcdShowStr(unsigned char x,unsigned char y,unsigned char *str);


void main()
{
    unsigned char str[] = "Welcome To Lcd";
    InitLcd1602();
    LcdShowStr(0,0,str);

    while(1);
}


void Read_Busy()           //?????,??bit7?0,????;1??
{
    unsigned char sta;      //
    LCD1602_DB = 0xff;
    LCD1602_RS = 0;
    LCD1602_RW = 1;
    do
    {
        LCD1602_EN = 1;
        sta = LCD1602_DB;
        LCD1602_EN = 0;    //??,?????,????
    }while(sta & 0x80);
}

void Lcd1602_Write_Cmd(unsigned char cmd)     //???
{
    Read_Busy();
    LCD1602_RS = 0;
    LCD1602_RW = 0;
    LCD1602_DB = cmd;
    LCD1602_EN = 1;
    LCD1602_EN = 0;    
}

void Lcd1602_Write_Data(unsigned char dat)   //???
{
      Read_Busy();
      LCD1602_RS = 1;
      LCD1602_RW = 0;
      LCD1602_DB = dat;
      LCD1602_EN = 1;
      LCD1602_EN = 0;
}

void LcdSetCursor(unsigned char x,unsigned char y)  //????
{
    unsigned char addr;
    if(y == 0)
        addr = 0x00 + x;
    else
        addr = 0x40 + x;
    
    Lcd1602_Write_Cmd(addr|0x80);
}

void LcdShowStr(unsigned char x,unsigned char y,unsigned char *str)     //?????
{
    LcdSetCursor(x,y);      //???????
    while(*str != '\0')
    {
        Lcd1602_Write_Data(*str++);
    }
}

void InitLcd1602()              //1602???
{
    Lcd1602_Write_Cmd(0x38);    //??,5*8,8???
    Lcd1602_Write_Cmd(0x0c);
    Lcd1602_Write_Cmd(0x06);
    Lcd1602_Write_Cmd(0x01);    //??   
}
