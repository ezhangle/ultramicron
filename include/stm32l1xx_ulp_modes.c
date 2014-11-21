/**
******************************************************************************
* @file    PWR/CurrentConsumption/stm32l1xx_ulp_modes.c 
* @author  MCD Application Team
* @version V1.1.1
* @date    13-April-2012
* @brief   This file provides firmware functions to manage the following 
*          functionalities of the STM32L1xx Ultra Low Power Modes:           
*           - Low Power Run Mode from Internal SRAM
*           - Low Power Run Mode from Internal FLASH
*           - Sleep Mode
*           - Low Power Sleep Mode
*           - STOP mode with or without RTC
*           - STANDBY mode with or without RTC
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
/*
#include "stm32l1xx_ulp_modes.h"
#ifdef USE_STM32L152D_EVAL
#include "stm32l152d_eval.h"
#else
#include "stm32l152_eval.h"
#endif
*/

#ifdef USE_STDPERIPH_DRIVER
#include "stm32l1xx_conf.h"
#endif

#include "main.h"
#include "rtc.h"
#include "ext2760.h"
#include "keys.h"


/** @addtogroup STM32L1xx_StdPeriph_Examples
* @{
*/

/** @addtogroup CurrentConsumption
* @{
*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t index = 0;
GPIO_InitTypeDef GPIO_InitStructure;
uint32_t fullstop=0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  This function configures the system to enter Low Power Run mode for
*         current consumption measurement purpose.
*         The maximum clock when the system is in Low Power Run mode is ~128KHz.
*         This mode can only be entered when Voltage Range 2 is selected. 
*         Low Power Run Mode from SRAM:
*         =============================
*           - System Running at MSI (~32KHz)
*           - Flash 0 wait state  
*           - Voltage Range 2
*           - Code running from Internal SRAM
*           - All peripherals OFF
*           - FLASH switched OFF
*           - VDD from 1.65V to 3.6V
*           - Current Consumption ~10.5uA
*           - Wakeup using Key Button PA.00  
*         Low Power Run Mode from FLASH:
*         ==============================
*           - System Running at MSI (~32KHz)
*           - Flash 0 wait state  
*           - Voltage Range 2
*           - Code running from Internal FLASH
*           - All peripherals OFF
*           - VDD from 1.65V to 3.6V
*           - Current Consumption ~25uA 
*           - Wakeup using Key Button PA.00   
* @param  None
* @retval None
*/
void SYSCLKConfig_STOP(void)
{ 
  
  /* After wake-up from STOP reconfigure the system clock */
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* Wait till HSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {}
  
  /* Enable PLL */
//  RCC_PLLCmd(ENABLE);
  
  /* Wait till PLL is ready */
//  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {}
  
  /* Select PLL as system clock source */
//  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
      RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
  
  /* Wait till PLL is used as system clock source */
//  while((RCC->CR & RCC_CR_PLLRDY) == 0)
      while((RCC->CR & RCC_CR_HSERDY) == 0)
  {}
}

////////////////////////////////////////

void StopMode_Measure(void)
{
  
  ////////////////////////////////////////////////////
  // Disable GPIOs clock 

  /* Enable Ultra low power mode */

//	PWR_UltraLowPowerCmd(ENABLE);
//	PWR_FastWakeUpCmd(ENABLE);
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	//SystemCoreClockUpdate();

  //Выход из стопа
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
void Standbuy_Mode(void)
{
  extern uint32_t fullstop;
//  EXTI_InitTypeDef   EXTI_InitStructure;
//  NVIC_InitTypeDef NVIC_InitStructure;
  fullstop=1;
  display_off();
	Settings.Sleep_time=2;
	sleep_mode(ENABLE);
/*
  TIM_Cmd(TIM9,  DISABLE); 
  TIM_Cmd(TIM10, DISABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,  DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, DISABLE);
*/
  ADC_Cmd(ADC1,  DISABLE); // ВКЛ!
	DAC_Cmd(DAC_Channel_2, DISABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP,  DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,  DISABLE);  // Разрешаем тактирование ADC
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,   DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
	SYSCFG_RIIOSwitchConfig(RI_IOSwitch_GR6_1,   DISABLE);
/*	
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);// Отключаем
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);// Отключаем
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_InitStructure.GPIO_Pin);// Отключаем

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, DISABLE);
  */
	while(RTC_WakeUpCmd(DISABLE)!=SUCCESS);
  RTC_SetWakeUpCounter(8192); 			// Установить таймаут просыпания
	while(RTC_WakeUpCmd(ENABLE)!=SUCCESS);
  /*
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); // Подаем тактирование на порт
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;        // Режим ножки "вход"
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        // Подтяжка к +
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           // Номер ножки в порту
  GPIO_Init(GPIOA, &GPIO_InitStructure);              // записиваем конфигурацию
  
  // Описываем EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;             // Номер EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // Режим прерывания
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Триггер по спадающему фронту
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;              // Включаем
  EXTI_Init(&EXTI_InitStructure);                        // записиваем конфигурацию
  
  // Описываем канал прерывания
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;           // канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  */
  
  while(1)
  {
    
/*    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); // Подаем тактирование на порт
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;        // Режим ножки "вход"
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        // Подтяжка к +
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           // Номер ножки в порту
    GPIO_Init(GPIOA, &GPIO_InitStructure);              // записиваем конфигурацию
  */  
    if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))
    {
      plus_reboot(0x00);
    }   
    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, DISABLE);
    StopMode_Measure();
  }
}
