#include "main.h"
#include "rtc.h"

RTC_InitTypeDef   RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;

__IO uint32_t LsiFreq = 0;
__IO uint32_t CaptureNumber = 0, PeriodValue = 0;


uint32_t GetLSIFrequency(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  RCC_ClocksTypeDef  RCC_ClockFreq;
  
  /* Enable the LSI oscillator ************************************************/
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  
  /* TIM10 configuration *******************************************************/ 
  /* Enable TIM10 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
  
  /* Reset TIM10 registers */
  TIM_DeInit(TIM10);
  
  /* Configure TIM10 prescaler */
  TIM_PrescalerConfig(TIM10, 0, TIM_PSCReloadMode_Immediate);
  
  /* Connect LSI clock to TIM10 Input Capture 1 */
  TIM_RemapConfig(TIM10, TIM10_LSI);
  
  /* TIM10 configuration: Input Capture mode ---------------------
  The reference clock(LSE or external) is connected to TIM10 CH1
  The Rising edge is used as active edge,
  The TIM10 CCR1 is used to compute the frequency value 
  ------------------------------------------------------------ */
  TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM10, &TIM_ICInitStructure);
  
  /* Enable the TIM10 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable TIM10 counter */
  TIM_Cmd(TIM10, ENABLE);
  
  /* Reset the flags */
  TIM10->SR = 0;
  
  /* Enable the CC4 Interrupt Request */  
  TIM_ITConfig(TIM10, TIM_IT_CC1, ENABLE);
  
  
  /* Wait until the TIM10 get 2 LSI edges (refer to TIM10_IRQHandler() in 
  stm32l1xx_it.c file) ******************************************************/
  while(CaptureNumber != 2)
  {
  }
  /* Deinitialize the TIM10 peripheral registers to their default reset values */
  TIM_DeInit(TIM10);
  
  
  /* Compute the LSI frequency, depending on TIM10 input clock frequency (PCLK1)*/
  /* Get SYSCLK, HCLK and PCLKx frequency */
  RCC_GetClocksFreq(&RCC_ClockFreq);
  
  /* Get PCLK1 prescaler */
  if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
  { 
    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
    return ((RCC_ClockFreq.PCLK1_Frequency / PeriodValue) * 8);
  }
  else
  { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
    return (((2 * RCC_ClockFreq.PCLK1_Frequency) / PeriodValue) * 8) ;
  }
}


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


void RTC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  EXTI_InitTypeDef EXTI_InitStructure;
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);

  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);
  
  /* LSI used as RTC source clock */
  /* The RTC Clock may varies due to LSI frequency dispersion. */   
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv	=  (Settings.LSI_freq/128) - 1;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

    RTC_AlarmStructInit(&RTC_AlarmStructure);

    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;

    /* Configure the RTC Alarm A register */
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

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



