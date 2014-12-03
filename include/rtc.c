#include "main.h"
#include "rtc.h"
#include "delay.h"

RTC_InitTypeDef   RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;

__IO uint32_t LsiFreq = 0;
__IO uint32_t CaptureNumber = 0, PeriodValue = 0;




void RTC_TimeRegulate(void)
{
  RTC_TimeTypeDef RTC_TimeStructure;
  
  uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;
  
  
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  
  tmp_hh = 21;
  RTC_TimeStructure.RTC_Hours = tmp_hh;
  
  tmp_mm = 55;
  RTC_TimeStructure.RTC_Minutes = tmp_mm;
  tmp_ss = 1;
  RTC_TimeStructure.RTC_Seconds = tmp_ss;
  
  /* Configure the RTC time register */
  if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
  {
    //    printf("\n\r>> !! RTC Set Time failed. !! <<\n\r");
  } 
  else
  {
    //    printf("\n\r>> !! RTC Set Time success. !! <<\n\r");
    //    RTC_TimeShow();
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
  }
  
}


#define RTC_AlarmMask_Exept_seconds                 ((uint32_t)0x80808000) // Все кроме секунд

void Set_next_alarm_wakeup(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	
	RTC_AlarmStructInit(&RTC_AlarmStructure);
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

	if (RTC_TimeStructure.RTC_Seconds<56)
	{
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds=RTC_TimeStructure.RTC_Seconds+4;		
	} else
	{
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds=0;		
	}
	//RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_Exept_seconds;

  /* Configure the RTC Alarm A register */
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

	RTC_ClearFlag(RTC_FLAG_ALRAF);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

}
void RTC_Config(void)
{
	uint32_t SynchPrediv;

  NVIC_InitTypeDef NVIC_InitStructure; 
  EXTI_InitTypeDef EXTI_InitStructure;
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);

  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

#ifdef version_204  // версия без кварца
	if(Settings.LSI_freq == 0x00)Settings.LSI_freq=37000;
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);  
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  SynchPrediv = (Settings.LSI_freq/128) - 1;
#else // версии с кварцем
	RCC_LSEConfig(RCC_LSE_ON); // Пытаемся включить LSE // по ДШ время запуска 1 секунда
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	SynchPrediv = 0xFF;
	Settings.LSI_freq=0x00;
#endif
	
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv	=  SynchPrediv;
	  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

	Set_next_alarm_wakeup(); // установить таймер просыпания на +4 секунды

    /* Enable the RTC Alarm A Interrupt */
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);

    /* Enable the alarm  A */
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

  
  /* Clear the RTC Alarm Flag */
  RTC_ClearFlag(RTC_FLAG_ALRAF);

  /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
  EXTI_ClearITPendingBit(EXTI_Line17);

	  /* RTC Alarm A Interrupt Configuration */
  /* EXTI configuration *********************************************************/
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // EXTI configuration
  EXTI_ClearITPendingBit(EXTI_Line20);
  EXTI_InitStructure.EXTI_Line = EXTI_Line20;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  // Enable the RTC Wakeup Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16); // 1 tick is 488 us
  RTC_SetWakeUpCounter(0xFFF);
	while(RTC_WakeUpCmd(ENABLE)!=SUCCESS);
  
  
  // Enable the RTC Wakeup Interrupt
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  
}



