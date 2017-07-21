#ifndef _DS18B20_
#define _DS18B20_

#include <STC15F2K.h>
#include "tools.h"

sbit DS18B20_DQ = P0^5;

u8   ds18b20Init();
int  Ds18b20GetTemperature();
int  Ds18b20GetTemperatureFromInt(int);

#endif