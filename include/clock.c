#include "clock.h"
#include "main.h"
#include "timers.h"
#include "stm32l1xx_rcc.h"
/*
  ============================================================================== 
             ##### FLASH Interface configuration functions #####
  ==============================================================================

    [..] FLASH_Interface configuration_Functions, includes the following functions:
     (+) void FLASH_SetLatency(uint32_t FLASH_Latency):
    [..] To correctly read data from Flash memory, the number of wait states (LATENCY) 
         must be correctly programmed according to the frequency of the CPU clock 
        (HCLK) and the supply voltage of the device.
  [..] 
  ----------------------------------------------------------------
 |  Wait states  |                HCLK clock frequency (MHz)      |
 |               |------------------------------------------------|
 |   (Latency)   |            voltage range       | voltage range |
 |               |            1.65 V - 3.6 V      | 2.0 V - 3.6 V |
 |               |----------------|---------------|---------------|
 |               |  VCORE = 1.2 V | VCORE = 1.5 V | VCORE = 1.8 V |
 |-------------- |----------------|---------------|---------------|
 |0WS(1CPU cycle)|0 < HCLK <= 2   |0 < HCLK <= 8  |0 < HCLK <= 16 |
 |---------------|----------------|---------------|---------------|
 |1WS(2CPU cycle)|2 < HCLK <= 4   |8 < HCLK <= 16 |16 < HCLK <= 32|
  ----------------------------------------------------------------
  [..]
     (+) void FLASH_PrefetchBufferCmd(FunctionalState NewState);
     (+) void FLASH_ReadAccess64Cmd(FunctionalState NewState);
     (+) void FLASH_RUNPowerDownCmd(FunctionalState NewState);
     (+) void FLASH_SLEEPPowerDownCmd(FunctionalState NewState);
     (+) void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
  [..]     
  Here below the allowed configuration of Latency, 64Bit access and prefetch buffer
  [..]  
  --------------------------------------------------------------------------------
 |               |              ACC64 = 0         |              ACC64 = 1        |
 |   Latency     |----------------|---------------|---------------|---------------|
 |               |   PRFTEN = 0   |   PRFTEN = 1  |   PRFTEN = 0  |   PRFTEN = 1  |
 |---------------|----------------|---------------|---------------|---------------|
 |0WS(1CPU cycle)|     YES        |     NO        |     YES       |     YES       |
 |---------------|----------------|---------------|---------------|---------------|
 |1WS(2CPU cycle)|     NO         |     NO        |     YES       |     YES       |
  --------------------------------------------------------------------------------
*/
//-------------------------------------------------------------------------------------------------------
void set_msi(FunctionalState sleep)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	 
	TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable); // запретить накачку
	Power.Pump_active=DISABLE;
   RCC_MSICmd(ENABLE); // Включить MSI
   while(RCC_GetFlagStatus(RCC_FLAG_MSIRDY)==RESET); // Ждем включения MSI

    /* Power enable */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1); // Voltage Scaling Range 1 (VCORE = 1.8V)
    while(PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET); // Wait Until the Voltage Regulator is ready

	/* HCLK = SYSCLK /1*/
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK /1*/
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK /4*/
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV4;

		if(sleep)
    {
			RCC_MSIRangeConfig(RCC_MSIRange_5); // 2.097 MHZ
    }else{
			RCC_MSIRangeConfig(RCC_MSIRange_6); // 4.194 MHz
    }

	
/*                - 0x00: MSI used as system clock
                - 0x04: HSI used as system clock  
                - 0x08: HSE used as system clock
                - 0x0C: PLL used as system clock
*/
switch (RCC_GetSYSCLKSource())
{
  case 0x00: // MSI уже установлен
   break;

  case 0x04: // HSI, переконфигурировать
   RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI); // Установить источником тактирования MSI
   while(RCC_GetSYSCLKSource() != 0x00); // Подождать пока произойдет смена
   RCC_HSICmd(DISABLE);  // Отключить HSI
   while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)!=RESET); // ждем остановки HSI
   break;

  case 0x08: // HSE, переконфигурировать
   RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI); // Установить источником тактирования MSI
   while(RCC_GetSYSCLKSource() != 0x00); // Подождать пока произойдет смена
   RCC_HSEConfig(RCC_HSE_OFF);  // Отключить HSE
   while(RCC_GetFlagStatus(RCC_FLAG_HSERDY)!=RESET); // ждем остановки HSE
   break;
	

  case 0x0C: // PLL, переконфигурировать
   RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI); // Установить источником тактирования MSI
   while(RCC_GetSYSCLKSource() != 0x00); // Подождать пока произойдет смена
   RCC_PLLCmd(DISABLE);  // Отключить PLL
   while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)!=RESET); // ждем остановки PLL
   RCC_HSEConfig(RCC_HSE_OFF);  // Отключить HSE
   while(RCC_GetFlagStatus(RCC_FLAG_HSERDY)!=RESET); // ждем остановки HSE
   break;
  default:
   while(1);
}

// Переконфигурируем FLASH
FLASH_SetLatency(FLASH_Latency_0);
FLASH_PrefetchBufferCmd(DISABLE);
FLASH_ReadAccess64Cmd(DISABLE);

PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3); // Voltage Scaling Range 3 (VCORE = 1.2V)
while(PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET); // Wait Until the Voltage Regulator is ready

SystemCoreClockUpdate();

sound_reset_prescaller();

TIM_PrescalerConfig(TIM2, (uint16_t) (SystemCoreClock / (100*8)) - 1,TIM_PSCReloadMode_Immediate); // Делитель (1 тик = 10мс)

TIM_PrescalerConfig(TIM9,(uint16_t)  (SystemCoreClock / 4000000) - 1,                   TIM_PSCReloadMode_Immediate);
#ifdef version_401
	TIM_SetCompare1    (TIM9,            4 ); // перерасчет энергии накачки (для версии 4 строго 1 мкс)
#else
	TIM_SetCompare1    (TIM9,            (352*Settings.Pump_Energy)/ADCData.Batt_voltage); // перерасчет энергии накачки
#endif
Power.Pump_active=DISABLE;

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
GPIO_Init(GPIOH, &GPIO_InitStructure);
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOH, DISABLE);

}
//-------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------
void set_pll_for_usb()
{
uint32_t pump_period;
	
TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable); // запретить накачку	
Power.Pump_active=DISABLE;
PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1); // Voltage Scaling Range 1 (VCORE = 1.8V)
while(PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET); // Wait Until the Voltage Regulator is ready

// Переконфигурируем FLASH
FLASH_ReadAccess64Cmd(ENABLE);
FLASH_PrefetchBufferCmd(ENABLE);
FLASH_SetLatency(FLASH_Latency_1);

	 if(!RCC_GetFlagStatus(RCC_FLAG_HSERDY))
	 {
		 RCC_HSEConfig(RCC_HSE_ON);  // Включить HSE
		 while(RCC_GetFlagStatus(RCC_FLAG_HSERDY)!=SET); // ждем старта HSE
	 }
			  /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
 
        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1);
 
        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);
 
        /* PLLCLK = 8MHz * 12 / 3 = 32 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE, RCC_PLLMul_12, RCC_PLLDiv_3);
 
        /* Enable PLL */
        RCC_PLLCmd(ENABLE);
 
        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY)!=SET);
 
        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
 
        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x0C);

SystemCoreClockUpdate();

sound_reset_prescaller();

TIM_PrescalerConfig(TIM2, (uint16_t) (SystemCoreClock / 10) - 1,TIM_PSCReloadMode_Immediate); // Делитель (1 тик = 10мс)


TIM_PrescalerConfig(TIM9, (uint16_t) (SystemCoreClock / 4000000) - 1,                   TIM_PSCReloadMode_Immediate);

#ifdef version_401
	pump_period=(v4_target_pump*4200)/ADCData.Batt_voltage; // расчет целевой накачки (Пример 1мкс*4.2В/3.3напряжение АКБ=1.25мкс)
#else
	pump_period=(352*Settings.Pump_Energy)/ADCData.Batt_voltage; // перерасчет энергии накачки
#endif
TIM_SetCompare1(TIM9,pump_period); // перерасчет энергии накачки

Power.Pump_active=DISABLE;
}
//-------------------------------------------------------------------------------------------------------
