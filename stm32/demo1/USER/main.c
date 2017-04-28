#include "stm32f10x.h"
#include "systick.h"
#include "led6.h"
#include "rtc.h"
#include <time.h>

u8 digitalNums[] = {0,1,2,3,4,5};

void GPIO_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);   //将PB3使能，禁用JTAG

    //声明对PB PD PE的GPIO使用
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = 0xFFFF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init(GPIOA , &GPIO_InitStructure);
    GPIO_Init(GPIOB , &GPIO_InitStructure);
    GPIO_Init(GPIOC , &GPIO_InitStructure);
    GPIO_Init(GPIOD , &GPIO_InitStructure);
    GPIO_Init(GPIOE , &GPIO_InitStructure);
}

int main(void) {

    GPIO_Config();
    SystemInit();   //72M晶振
    sysTickInit();
    

    while(1) {
        digitalNumsShow(digitalNums);
        //delay(1000);
    }
}
