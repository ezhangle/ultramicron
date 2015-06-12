#include "main.h"
#include "menu.h"
#include "delay.h"
#include "ext2760.h"
#include "eeprom.h"
#include "usb.h"
#include <stdio.h>
#include <string.h>
#include "lang.h"
#include "stm32l1xx_it.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Изменение порога тревоги
void  plus_alarm(uint32_t *param) // +
{
  if((*Menu_list[*param-1].Parameter_value  >= 1000)&(*Menu_list[*param-1].Parameter_value  < 9999)) *Menu_list[*param-1].Parameter_value+= 500;
  if((*Menu_list[*param-1].Parameter_value  >=  100)&(*Menu_list[*param-1].Parameter_value  < 1000)) *Menu_list[*param-1].Parameter_value+= 100;
  if(                                                 *Menu_list[*param-1].Parameter_value  <  100)  *Menu_list[*param-1].Parameter_value+=  25;
  if(*Menu_list[*param-1].Parameter_value   >  9999)                                                 *Menu_list[*param-1].Parameter_value=    0;
  
}
void minus_alarm(uint32_t *param) // -
{
  if( *Menu_list[*param-1].Parameter_value  <=  100)                                                  *Menu_list[*param-1].Parameter_value-=   25;
  if((*Menu_list[*param-1].Parameter_value  >   100)&(*Menu_list[*param-1].Parameter_value <= 1000))  *Menu_list[*param-1].Parameter_value-=  100;
  if((*Menu_list[*param-1].Parameter_value  >  1000)&(*Menu_list[*param-1].Parameter_value <  9999))  *Menu_list[*param-1].Parameter_value-=  500;
  if( *Menu_list[*param-1].Parameter_value  >  9999)                                                  *Menu_list[*param-1].Parameter_value=  9500;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Изменение порога сна
void  plus_sleep(uint32_t *param) //+
{
  *Menu_list[*param-1].Parameter_value+=5;
  if(*Menu_list[*param-1].Parameter_value>250)*Menu_list[*param-1].Parameter_value=0;
}

void minus_sleep(uint32_t *param) //-
{
  *Menu_list[*param-1].Parameter_value-=5;
  if(*Menu_list[*param-1].Parameter_value>250)*Menu_list[*param-1].Parameter_value=250;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// вкл-откл
void  plus_on(uint32_t *param) // вкл
{  
  *Menu_list[*param-1].Parameter_value=1;
}

void minus_off(uint32_t *param) // откл
{
  *Menu_list[*param-1].Parameter_value=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  plus_doze_reset(uint32_t *param) // Сброс дозы
{
	int i;
	for(i=doze_length_week;i>0;i--)
	{
		Doze_massive[i]=0;
		Doze_hour_count+=Doze_massive[i];
		Doze_day_count+=Doze_massive[i];
		Doze_week_count+=Doze_massive[i];
	}
Doze_hour_count=0;
Doze_day_count=0;
Doze_week_count=0;
DataUpdate.doze_sec_count=0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// +1 -1
void  plus_one(uint32_t *param) // +1
{
  //Если пытаемся привысить максимально допустимое значение, то переходим на минимум
  if(*Menu_list[*param-1].Parameter_value >= Menu_list[*param-1].Max_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Min_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value+1; //+1
  }
}

void minus_one(uint32_t *param) // -1
{
  //Если пытаемся привысить минимально допустимое значение, то переходим на максимум
  if(*Menu_list[*param-1].Parameter_value <= Menu_list[*param-1].Min_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Max_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value-1; // -1
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// +10 -10
void  plus_ten(uint32_t *param) // +10
{
  //Если пытаемся привысить максимально допустимое значение, то переходим на минимум
  if(*Menu_list[*param-1].Parameter_value >= Menu_list[*param-1].Max_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Min_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value+10; //+10
  }
}


void minus_ten(uint32_t *param) // -10
{
  //Если пытаемся привысить минимально допустимое значение, то переходим на максимум
  if(*Menu_list[*param-1].Parameter_value <= Menu_list[*param-1].Min_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Max_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value-10; // -10
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// +50 -50
void  plus_50(uint32_t *param) // +50
{
  //Если пытаемся привысить максимально допустимое значение, то переходим на минимум
  if(*Menu_list[*param-1].Parameter_value >= Menu_list[*param-1].Max_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Min_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value+50; //+50
  }
}

void minus_50(uint32_t *param) // -50
{
  //Если пытаемся привысить минимально допустимое значение, то переходим на максимум
  if(*Menu_list[*param-1].Parameter_value <= Menu_list[*param-1].Min_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Max_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value-50; // -50
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// +500 -500
void  plus_500(uint32_t *param) // +50
{
  //Если пытаемся привысить максимально допустимое значение, то переходим на минимум
  if(*Menu_list[*param-1].Parameter_value >= Menu_list[*param-1].Max_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Min_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value+500; //+500
  }
}

void minus_500(uint32_t *param) // -500
{
  //Если пытаемся привысить минимально допустимое значение, то переходим на максимум
  if(*Menu_list[*param-1].Parameter_value <= Menu_list[*param-1].Min_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Max_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value-500; // -500
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// перезагрузка и выключение
void plus_reboot(uint32_t *param) // перезагрузка
{
	LcdClear_massive();
	sprintf (lcd_buf, LANG_REBOOTPR); // Пишем в буфер значение счетчика
	LcdString(1,5); // // Выводим обычным текстом содержание буфера
	sprintf (lcd_buf, LANG_DONTOTO1); // Пишем в буфер значение счетчика
	LcdString(1,7); // // Выводим обычным текстом содержание буфера
	sprintf (lcd_buf, LANG_DONTOTO2); // Пишем в буфер значение счетчика
	LcdString(1,8); // // Выводим обычным текстом содержание буфера

  LcdUpdate(); // записываем данные из сформированного фрейм-буфера на дисплей
	
  delay_ms(6000);
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_4);
  IWDG_SetReload(2);
  IWDG_ReloadCounter();
  IWDG_Enable();
  while(1);
}

void minus_poweroff(uint32_t *param) // выключение
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;

	LcdClear_massive();
	sprintf (lcd_buf, LANG_POWEROFF); // Пишем в буфер значение счетчика
	LcdString(1,5); // // Выводим обычным текстом содержание буфера
	sprintf (lcd_buf, LANG_DONTOTO1); // Пишем в буфер значение счетчика
	LcdString(1,7); // // Выводим обычным текстом содержание буфера
	sprintf (lcd_buf, LANG_DONTOTO2); // Пишем в буфер значение счетчика
	LcdString(1,8); // // Выводим обычным текстом содержание буфера

  LcdUpdate(); // записываем данные из сформированного фрейм-буфера на дисплей
	delay_ms(6000);

	
	sleep_mode(ENABLE);
	poweroff_state=ENABLE;
	Pump_now(DISABLE);
// =======================================================
// Де-Инициализация прерывания датчика 1

  EXTI_InitStructure.EXTI_Line = EXTI_Line8;             // Номер EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // Режим прерывания
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // Триггер по нарастающему фронту
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;              // Включаем
  EXTI_Init(&EXTI_InitStructure);                        // записиваем конфигурацию
  
  // Описываем канал прерывания
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;           // канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
// =======================================================
  NVIC_InitStructure.NVIC_IRQChannel = COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);


/////////////////////////////////////////////////////
  /* Enable the alarm  A */
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  
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
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

  // EXTI configuration
  EXTI_ClearITPendingBit(EXTI_Line20);
  EXTI_InitStructure.EXTI_Line = EXTI_Line20;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  // Enable the RTC Wakeup Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  RTC_ClearFlag(RTC_FLAG_ALRAF);

	while(RTC_WakeUpCmd(DISABLE)!=SUCCESS);
  RTC_ITConfig(RTC_IT_WUT, DISABLE);
  RTC_ITConfig(RTC_IT_ALRA, DISABLE);
  PWR_RTCAccessCmd(DISABLE);
  RCC_RTCCLKCmd(DISABLE);

#ifdef version_204  // версия без кварца
	RCC_LSICmd(DISABLE);
#else // версии с кварцем
	RCC_LSEConfig(RCC_LSE_OFF);
#endif
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);
/////////////////////////////////////////////////////

	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))
	{
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);    // Переходим в сон
	}

	sleep_mode(DISABLE);

	LcdClear_massive();
	sprintf (lcd_buf, LANG_REBOOTPR); // Пишем в буфер значение счетчика
	LcdString(1,5); // // Выводим обычным текстом содержание буфера
	sprintf (lcd_buf, LANG_DONTOTO1); // Пишем в буфер значение счетчика
	LcdString(1,7); // // Выводим обычным текстом содержание буфера
	sprintf (lcd_buf, LANG_DONTOTO2); // Пишем в буфер значение счетчика
	LcdString(1,8); // // Выводим обычным текстом содержание буфера

  LcdUpdate(); // записываем данные из сформированного фрейм-буфера на дисплей

	
  delay_ms(6000);
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_4);
  IWDG_SetReload(2);
  IWDG_ReloadCounter();
  IWDG_Enable();
  while(1);

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// перезагрузка и выключение
void usb_activate(uint32_t *param) // Включение USB
{

USB_not_active=0;
if(!Power.USB_active)
{
	USB_on(); 		
	LcdClear_massive();
	sprintf (lcd_buf, LANG_USBON); // Пишем в буфер значение счетчика
	LcdString(1,5); // // Выводим обычным текстом содержание буфера
  LcdUpdate(); // записываем данные из сформированного фрейм-буфера на дисплей
  delay_ms(1000);
}
#ifndef version_401
Settings.USB=1;
#endif
}

void usb_deactivate(uint32_t *param) // Выключение USB
{
	if(Power.USB_active)
	{
#ifndef version_401
		Settings.USB=0;
#endif
		USB_off(); 		
		LcdClear_massive();
		sprintf (lcd_buf, LANG_USBOFF); // Пишем в буфер значение счетчика
		LcdString(1,5); // // Выводим обычным текстом содержание буфера
		LcdUpdate(); // записываем данные из сформированного фрейм-буфера на дисплей
		delay_ms(1000);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void keys_proccessing(void)
{
  extern uint16_t key;
  extern SettingsDef Settings;
  
  /////////////////////////////////
  if(key & 0x2)  // Кнопка +
  {
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));
    delay_ms(10);
		DataUpdate.Need_display_update=ENABLE;
    if (screen==2 && enter_menu_item==DISABLE)menu_select--;
		if (screen==1)main_menu_stat++;
		if (screen==3)
		{
#ifdef debug
			if(stat_screen_number==2)
#else
			if(stat_screen_number==1)
#endif
			{
				stat_screen_number=0;
			} else {
				stat_screen_number++;
			}
		}
		if(hidden_menu){
			if(menu_select>max_struct_index)menu_select=max_struct_index;
		}else{
			if(menu_select>(max_string_count-start_offset))menu_select=(max_string_count-start_offset);
		}
		
    key=0;
		
		///////////
    if(enter_menu_item==ENABLE)
    {
      
      if(Menu_list[menu_select-1].Plus_reaction!=0x00) // Если адрес функции существует, то выполнить ее.
      {
        (*Menu_list[menu_select-1].Plus_reaction)(&menu_select); // запуск  функции - гиперхак мать его :)
      }
    }    
  }
  
  
  
  
  /////////////////////////////////
  if(key & 0x4)  // Кнопка -
  {
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));
    delay_ms(10);
		DataUpdate.Need_display_update=ENABLE;
    if (screen==2 && enter_menu_item==DISABLE)menu_select++;
		if (screen==1)main_menu_stat--;
		if (screen==3)
		{
			if(stat_screen_number==0)
				{
					
#ifdef debug
					stat_screen_number=2;
#else
					stat_screen_number=1;
#endif
				} else {
					stat_screen_number--;
				}
		}
		if(hidden_menu){
			if(menu_select>max_struct_index)menu_select=0;
		}else{
		  if(menu_select>(max_string_count-start_offset))menu_select=0;
		}

    key=0;

    ///////////
    if(enter_menu_item==ENABLE) // тревога
    {
      if(Menu_list[menu_select-1].Minus_reaction!=0x00) // Если адрес функции существует, то выполнить ее.
      {
        (*Menu_list[menu_select-1].Minus_reaction)(&menu_select); // запуск  функции - гиперхак мать его :)
      }
    }
    
    
  }
  /////////////////////////////////
  if (key & 0x1) // Кнопка Меnu
  {
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));

		DataUpdate.Need_display_update=ENABLE;
    LcdClear(); // Очищаем фрейм буфер и дисплей
    
    delay_ms(10);
    key=0;

    ///////////
    if(menu_select>0)
    {
      if(enter_menu_item==DISABLE){enter_menu_item=ENABLE;}
      else                    
      {
        enter_menu_item=DISABLE;
				eeprom_apply_settings(); //применяем параметры
        eeprom_write_settings(); //сохраняем параметры
      }
    }
    if(menu_select==0)screen++;
  }
  if (screen>3) screen=1;
  
}
