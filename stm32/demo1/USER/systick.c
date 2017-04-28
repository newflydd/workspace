#include "systick.h"

void sysTickInit(void) {
    if (SysTick_Config(72000)) { // 1ms一次中断
        while (1);
    }

    // 关闭滴答定时器
    SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void delay(__IO u32 nTime) {
    sysTimeDelay = nTime;

    // 使能滴答定时器
    SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
    while(sysTimeDelay != 0);
}

void sysTimeDelayDecrement(void) {
    if(sysTimeDelay != 0x00) {
        sysTimeDelay--;
    }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
#ifndef RTE_CMSIS_RTOS_RTX
void SysTick_Handler(void) {
    sysTimeDelayDecrement();
}
#endif
