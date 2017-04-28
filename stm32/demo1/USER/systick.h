#ifndef __SYSTICK__
#define __SYSTICK__

#include "stm32f10x.h"

static __IO u32 sysTimeDelay;

void sysTickInit(void);
void delay(__IO u32 nTime);

#endif
