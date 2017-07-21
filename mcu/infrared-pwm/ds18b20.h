#ifndef _DS18B20_
#define _DS18B20_

#include <STC15F2K.h>
#include "tools.h"

sbit DS18B20_DQ = P0^5;

int   Ds18b20GetTemperature();
float Ds18b20GetTemperatureFromInt(int);

#endif