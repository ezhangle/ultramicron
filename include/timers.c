#include "stm32l1xx_tim.h"
#include "main.h"

void tim10_sound_activate(void)
{
		Power.Sound_active=ENABLE;

		TIM_ITConfig(TIM10, TIM_IT_CC1, ENABLE);
		TIM10->EGR |= 0x0001;  // Устанавливаем бит UG для принудительного сброса счетчика
		TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Enable); // разрешить накачку	

		TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);
}

void tim10_sound_deactivate(void)
{	
    Power.Sound_active=DISABLE;      
		Sound_key_pressed=DISABLE;

		TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Disable); // запретить накачку
		TIM_ITConfig(TIM10, TIM_IT_Update, DISABLE);

		TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
}



void timer9_Config(void) // Таймер накачки
{
TIM_TimeBaseInitTypeDef TIM_BaseConfig;
TIM_OCInitTypeDef TIM_OCConfig;
NVIC_InitTypeDef NVIC_InitStructure;

	TIM_TimeBaseStructInit(&TIM_BaseConfig);
	TIM_OCStructInit(&TIM_OCConfig);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1; // Конфигурируем выход таймера, режим - PWM1
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;   // Собственно - выход включен
  TIM_OCConfig.TIM_Pulse = 1;   // Расчет необходимой энергии
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High; // Полярность => пульс - это единица (+3.3V)

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 2000000) - 1; // Делитель (1 тик = 0.5мкс)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 280;  // Общее количество тиков (скваженность) 140мкс (было 500)
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up; // Отсчет от нуля до TIM_Period
  
  
  TIM_DeInit(TIM9); // Де-инициализируем таймер №9
  TIM_TimeBaseInit(TIM9, &TIM_BaseConfig);
  TIM_OC1Init(TIM9, &TIM_OCConfig);  // Инициализируем первый выход таймера №9 (у HD это PB13)

  // Как я понял - автоматическая перезарядка таймера, если неправ - поправте.
  TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM9, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
  TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);

  TIM9->EGR |= 0x0001;  // Устанавливаем бит UG для принудительного сброса счетчика
  TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable); // запретить накачку
  TIM_Cmd(TIM9, ENABLE);
}

void timer10_Config(void) // Таймер спикера
{
TIM_TimeBaseInitTypeDef TIM_BaseConfig;
TIM_OCInitTypeDef TIM_OCConfig;
NVIC_InitTypeDef NVIC_InitStructure;

	TIM_TimeBaseStructInit(&TIM_BaseConfig);
	TIM_OCStructInit(&TIM_OCConfig);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 2000000) - 1; // Делитель (1 тик = 0.5мкс)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 2000000 /(Settings.Sound_freq*1000);
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up; // Отсчет от нуля до TIM_Period

  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1; // Конфигурируем выход таймера, режим - PWM1
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;   // Собственно - выход включен
  TIM_OCConfig.TIM_Pulse = TIM_BaseConfig.TIM_Period / 2;
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High; // Полярность => пульс - это единица (+3.3V)
  
  
//  TIM_DeInit(TIM10); // Де-инициализируем таймер №9
  // Как я понял - автоматическая перезарядка таймера, если неправ - поправте.
  TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM10, ENABLE);

  TIM_TimeBaseInit(TIM10, &TIM_BaseConfig);
  TIM_OC1Init(TIM10, &TIM_OCConfig);  // Инициализируем первый выход таймера №9 (у HD это PB13)

  NVIC_InitStructure.NVIC_IRQChannel = TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);

  TIM10->EGR |= 0x0001;  // Устанавливаем бит UG для принудительного сброса счетчика
  TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Enable); // запретить накачку
  TIM_Cmd(TIM10, ENABLE);
}
