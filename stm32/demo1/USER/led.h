#ifndef __LED__
#define __LED__

#include "stm32f10x.h"

#define LED0U GPIOB
#define LED0P GPIO_Pin_3

#define LED1U GPIOD
#define LED1P GPIO_Pin_6

#define LED2U GPIOD
#define LED2P GPIO_Pin_4

#define LED3U GPIOB
#define LED3P GPIO_Pin_9

#define LED4U GPIOE
#define LED4P GPIO_Pin_1

#define LED5U GPIOB
#define LED5P GPIO_Pin_7

#define LED6U GPIOB
#define LED6P GPIO_Pin_5

#define LED7U GPIOD
#define LED7P GPIO_Pin_2

/*     数码管字符表     0     1     2     3     4     5     6     7     8    9      A     b     C     d     E     F   NULL*/
u8 digitalTable[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7C, 0x39, 0x5E, 0X79, 0X71, 0x00 };

/**
 * 根据每个字符在数码管上的结构组成，控制GPIO
 * @param pos    [字符在数码管上的显示位]
 * @param ifshow [该位是否显示]
 */
void digitalUnitShow(u8 pos, u8 ifshow){
	switch(pos){
		case 0:
			if(ifshow)
				GPIO_SetBits(LED0U, LED0P);
			else
				GPIO_ResetBits(LED0U, LED0P);
			break;
		case 1:
			if(ifshow)
				GPIO_SetBits(LED1U, LED1P);
			else
				GPIO_ResetBits(LED1U, LED1P);
			break;
		case 2:
			if(ifshow)
				GPIO_SetBits(LED2U, LED2P);
			else
				GPIO_ResetBits(LED2U, LED2P);
			break;
		case 3:
			if(ifshow)
				GPIO_SetBits(LED3U, LED3P);
			else
				GPIO_ResetBits(LED3U, LED3P);
			break;
		case 4:
			if(ifshow)
				GPIO_SetBits(LED4U, LED4P);
			else
				GPIO_ResetBits(LED4U, LED4P);
			break;
		case 5:
			if(ifshow)
				GPIO_SetBits(LED5U, LED5P);
			else
				GPIO_ResetBits(LED5U, LED5P);
			break;
		case 6:
			if(ifshow)
				GPIO_SetBits(LED6U, LED6P);
			else
				GPIO_ResetBits(LED6U, LED6P);
			break;
		case 7:
			if(ifshow)
				GPIO_SetBits(LED7U, LED7P);
			else
				GPIO_ResetBits(LED7U, LED7P);
			break;
	}
}

/**
 * 显示单个字符
 * @param num [需要显示的字符]
 */
void digitalNumShow(u8 num){
	for(u8 pos = 0; pos < 8; pos++){
		if(digitalTable[num] >> pos & 0x01 == 0x01)
			digitalUnitShow(pos, 0x01);
		else
			digitalUnitShow(pos, 0x00);
	}
}

#endif
