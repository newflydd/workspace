#ifndef __RTC__
#define __RTC__

#include "stm32f10x.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"

void initRTC(){
	//后备电池使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);

	BKP_DeInit();								//将外设 BKP 的全部寄存器重设为缺省值
	RCC_LSEConfig(RCC_LSE_ON);					//使用LSE外部32.768KHz晶振

	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);		//等待LSE晶振起振

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//选择LSE外部晶振作为RTC的晶振
	RCC_RTCCLKCmd(ENABLE);						//RTC使能
	RTC_WaitForSynchro();						//同步等待RTC就绪
	RTC_WaitForLastTask();						//等待最后一次对RTC的写操作结束

	RTC_ITConfig(RTC_IT_SEC, ENABLE);			//RTC秒中断使能
	RTC_WaitForLastTask();

	RTC_SetPrescaler(32767);					//外部晶振32768分频32767使得秒中断频率为1Hz
	RTC_WaitForLastTask();
}

#endif
