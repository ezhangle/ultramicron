#include "main.h"



// =======================================================
// Инициализация прерывания датчика 1
void EXTI8_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  
  // Описываем ножку
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); // Подаем тактирование на порт
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;        // Режим ножки "вход"
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    // Без подтяжки
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;           // Номер ножки в порту
  GPIO_Init(GPIOA, &GPIO_InitStructure);              // записиваем конфигурацию
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Подаем тактирование на SYSCFG
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8); // Подключаем EXTI к ножке
  
  // Описываем EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line8;             // Номер EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // Режим прерывания
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // Триггер по нарастающему фронту
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;              // Включаем
  EXTI_Init(&EXTI_InitStructure);                        // записиваем конфигурацию
  
  // Описываем канал прерывания
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;           // канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

// =======================================================
// Инициализация прерывания по нажатию кнопки 0
void EXTI3_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  
  // Описываем ножку
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); // Подаем тактирование на порт
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;        // Режим ножки "вход"
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        // Подтяжка к +
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;           // Номер ножки в порту
  GPIO_Init(GPIOA, &GPIO_InitStructure);              // записиваем конфигурацию
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Подаем тактирование на SYSCFG
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3); // Подключаем EXTI к ножке
  
  // Описываем EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line3;             // Номер EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // Режим прерывания
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Триггер по спадающему фронту
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;              // Включаем
  EXTI_Init(&EXTI_InitStructure);                        // записиваем конфигурацию
  
  // Описываем канал прерывания
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;           // канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

// =======================================================
// Инициализация прерывания по нажатию кнопки 1
void EXTI4_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  
  // Описываем ножку
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); // Подаем тактирование на порт
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;        // Режим ножки "вход"
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        // Подтяжка к +
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           // Номер ножки в порту
  GPIO_Init(GPIOA, &GPIO_InitStructure);              // записиваем конфигурацию
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Подаем тактирование на SYSCFG
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4); // Подключаем EXTI к ножке
  
  // Описываем EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;             // Номер EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // Режим прерывания
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Триггер по спадающему фронту
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;              // Включаем
  EXTI_Init(&EXTI_InitStructure);                        // записиваем конфигурацию
  
  // Описываем канал прерывания
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;           // канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


// =======================================================
// Инициализация прерывания по нажатию кнопки 2
void EXTI6_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  
  // Описываем ножку
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); // Подаем тактирование на порт
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;        // Режим ножки "вход"
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        // Подтяжка к +
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           // Номер ножки в порту
  GPIO_Init(GPIOA, &GPIO_InitStructure);              // записиваем конфигурацию
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Подаем тактирование на SYSCFG
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6); // Подключаем EXTI к ножке
  
  // Описываем EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;             // Номер EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // Режим прерывания
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // Триггер по спадающему фронту
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;              // Включаем
  EXTI_Init(&EXTI_InitStructure);                        // записиваем конфигурацию
  
  // Описываем канал прерывания
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;           // канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
