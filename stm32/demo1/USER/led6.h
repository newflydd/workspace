/**
 * 6位共阴极数码管的控制，不含小数点
 */

#ifndef __LED6__
#define __LED6__

#include "stm32f10x.h"

/**
 * LED数据针脚定义
 */
#define LEDaU GPIOE
#define LEDaP GPIO_Pin_5

#define LEDbU GPIOC
#define LEDbP GPIO_Pin_13

#define LEDcU GPIOC
#define LEDcP GPIO_Pin_15

#define LEDdU GPIOC
#define LEDdP GPIO_Pin_1

#define LEDeU GPIOC
#define LEDeP GPIO_Pin_3

#define LEDfU GPIOA
#define LEDfP GPIO_Pin_1

#define LEDgU GPIOA
#define LEDgP GPIO_Pin_3

/**
 * LED位选针脚定义
 */
#define LED0U GPIOD
#define LED0P GPIO_Pin_2

#define LED1U GPIOD
#define LED1P GPIO_Pin_4

#define LED2U GPIOD
#define LED2P GPIO_Pin_6

#define LED3U GPIOB
#define LED3P GPIO_Pin_3

#define LED4U GPIOB
#define LED4P GPIO_Pin_5

#define LED5U GPIOB
#define LED5P GPIO_Pin_7

/*     数码管字符表     0     1     2     3     4     5     6     7     8    9      A     b     C     d     E     F   NULL*/
u8 digitalTable[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7C, 0x39, 0x5E, 0X79, 0X71, 0x00 };

/**
 * 根据每个字符在数码管上的结构组成，控制GPIO
 * @param pos    [字符在数码管上的显示位]
 * @param ifshow [该位是否显示]
 */
void digitalUnitShow(__IO u8 pos,__IO u8 ifshow){
	switch(pos){
		case 0:
			if(ifshow)
				GPIO_SetBits(LEDaU, LEDaP);
			else
				GPIO_ResetBits(LEDaU, LEDaP);
			break;
		case 1:
			if(ifshow)
				GPIO_SetBits(LEDbU, LEDbP);
			else
				GPIO_ResetBits(LEDbU, LEDbP);
			break;
		case 2:
			if(ifshow)
				GPIO_SetBits(LEDcU, LEDcP);
			else
				GPIO_ResetBits(LEDcU, LEDcP);
			break;
		case 3:
			if(ifshow)
				GPIO_SetBits(LEDdU, LEDdP);
			else
				GPIO_ResetBits(LEDdU, LEDdP);
			break;
		case 4:
			if(ifshow)
				GPIO_SetBits(LEDeU, LEDeP);
			else
				GPIO_ResetBits(LEDeU, LEDeP);
			break;
		case 5:
			if(ifshow)
				GPIO_SetBits(LEDfU, LEDfP);
			else
				GPIO_ResetBits(LEDfU, LEDfP);
			break;
		case 6:
			if(ifshow)
				GPIO_SetBits(LEDgU, LEDgP);
			else
				GPIO_ResetBits(LEDgU, LEDgP);
			break;		
	}
}

/**
 * 显示单个字符
 * @param num [需要显示的字符]
 */
void digitalNumShow(__IO u8 num){
	for(__IO u8 pos = 0; pos < 8; pos++){
		if(digitalTable[num] >> pos & 0x01 == 0x01)
			digitalUnitShow(pos, 0x01);
		else
			digitalUnitShow(pos, 0x00);
	}
}

/**
 * 位选函数
 * @param pos [数码管位置]
 */
void digitalPosChoose(__IO u8 pos){
	switch(pos){
		case 0:
			GPIO_ResetBits(LED0U, LED0P);
			GPIO_SetBits(  LED1U, LED1P);
			GPIO_SetBits(  LED2U, LED2P);
			GPIO_SetBits(  LED3U, LED3P);
			GPIO_SetBits(  LED4U, LED4P);
			GPIO_SetBits(  LED5U, LED5P);
			break;
		case 1:
			GPIO_SetBits(  LED0U, LED0P);
			GPIO_ResetBits(LED1U, LED1P);
			GPIO_SetBits(  LED2U, LED2P);
			GPIO_SetBits(  LED3U, LED3P);
			GPIO_SetBits(  LED4U, LED4P);
			GPIO_SetBits(  LED5U, LED5P);
			break;
		case 2:
			GPIO_SetBits(  LED0U, LED0P);
			GPIO_SetBits(  LED1U, LED1P);
			GPIO_ResetBits(LED2U, LED2P);
			GPIO_SetBits(  LED3U, LED3P);
			GPIO_SetBits(  LED4U, LED4P);
			GPIO_SetBits(  LED5U, LED5P);
			break;
		case 3:
			GPIO_SetBits(  LED0U, LED0P);
			GPIO_SetBits(  LED1U, LED1P);
			GPIO_SetBits(  LED2U, LED2P);
			GPIO_ResetBits(LED3U, LED3P);
			GPIO_SetBits(  LED4U, LED4P);
			GPIO_SetBits(  LED5U, LED5P);
			break;
		case 4:
			GPIO_SetBits(  LED0U, LED0P);
			GPIO_SetBits(  LED1U, LED1P);
			GPIO_SetBits(  LED2U, LED2P);
			GPIO_SetBits(  LED3U, LED3P);
			GPIO_ResetBits(LED4U, LED4P);
			GPIO_SetBits(  LED5U, LED5P);
			break;
		case 5:
			GPIO_SetBits(  LED0U, LED0P);
			GPIO_SetBits(  LED1U, LED1P);
			GPIO_SetBits(  LED2U, LED2P);
			GPIO_SetBits(  LED3U, LED3P);
			GPIO_SetBits(  LED4U, LED4P);
			GPIO_ResetBits(LED5U, LED5P);
			break;
	}
}

void digitalNumsShow(u8* nums){
	for(__IO u8 pos = 0; pos < 6; pos++){
		digitalNumShow(16);		//先熄灭所有灯
		digitalPosChoose(pos);
		digitalNumShow(nums[pos]);
	}
}

#endif
