#include "tools.h"

/**
 * 微秒级别延时
 * 			0.675us= 1
 * 			67.5us = 100
 * 			10us   = 15
 * 9ms    = 9000us = 13333
 * 4.5ms  = 4500us = 6667
 * 			560us  = 830
 *    		1690us = 2504
 */
void delay(u16 us){
	while(us)us--;
}