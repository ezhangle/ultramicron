#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"                  // Device header

#include "main.h"
#include "delay.h"
#include "io_ports.h"
#include "rtc.h"
#include "timers.h"
#include "stm32l1xx_ulp_modes.h"
#include "ext2760.h"
#include "interrupt.h"
#include "menu.h"
#include "keys.h"
#include "eeprom.h"
#include "adc.h"
#include "dac.h"
#include "comp.h"
#include "usb.h"
#include "clock.h"




uint16_t key; // массив нажатых кнопок [012]
uint32_t ix;
uint32_t ix_update;

uint16_t Detector_massive[120+1];
uint32_t Doze_massive[doze_length+1]; // 1 ячейка = 10 минут, на протяжении суток
uint32_t max_fon_massive[doze_length+1]; // 1 ячейка = 10 минут, на протяжении суток
uint16_t current_pulse_count=0;
uint8_t pump_count=0;
uint8_t second_divide=0;
uint32_t Doze_count=0;
uint32_t Doze_hour_count=0;
uint32_t Max_fon=0;
uint8_t  main_menu_stat=0;
uint16_t Detector_massive_pointer=0;
uint8_t  auto_speedup_factor=0;

uint32_t menu_select=0;

FunctionalState enter_menu_item=DISABLE;
uint8_t screen=1;
uint8_t stat_screen_number=0;
uint16_t pump_counter_avg_impulse_by_1sec[2];
uint32_t fon_level=0;

FunctionalState Sound_key_pressed=DISABLE;

DataUpdateDef DataUpdate;
ADCDataDef ADCData;
SettingsDef Settings;
AlarmDef Alarm;
PowerDef Power;

void sleep_mode(FunctionalState sleep)
{ 
  if(Settings.Sleep_time>0 && !Power.USB_active)
  {
    if(sleep)
    {
			RTC_ITConfig(RTC_IT_WUT, DISABLE);
      display_off(); // выключить дисплей
 			GPIO_ResetBits(GPIOA,GPIO_Pin_7);// Фиксируем режим 1.8 вольта, с низким потреблением ножки
			delay_ms(1000); // подождать установки напряжения
			DataUpdate.Need_batt_voltage_update=ENABLE; // разрешить работу АЦП
			adc_check_event(); // запустить преобразование
			RTC_ITConfig(RTC_IT_WUT, ENABLE);
    }
    else
    {
			RTC_ITConfig(RTC_IT_WUT, DISABLE);
			GPIO_SetBits(GPIOA,GPIO_Pin_7);// Переключаем в режим 3 вольта
			delay_ms(400); // подождать установки напряжения
      display_on(); // включить дисплей
			DataUpdate.Need_batt_voltage_update=ENABLE; // разрешить работу АЦП
			adc_check_event(); // запустить преобразование
			RTC_ITConfig(RTC_IT_WUT, ENABLE);
    }
  } 
}


void geiger_calc_fon(void)
{
	DataUpdate.Need_fon_update=DISABLE;  
  if(fon_level>Settings.Alarm_level && Settings.Alarm_level>0 && Alarm.Alarm_active==DISABLE)
  {
    Alarm.Alarm_active=ENABLE;
		Alarm.User_cancel=DISABLE;
    if(!Power.Display_active)sleep_mode(DISABLE);
		tim10_sound_activate();
    
  }
  if((Alarm.Alarm_active && fon_level<Settings.Alarm_level) || (Alarm.Alarm_active && Settings.Alarm_level==0))
  {
		tim10_sound_deactivate();
    Power.Sound_active=DISABLE;
    Alarm.Alarm_active=DISABLE;
    Alarm.User_cancel=DISABLE;
    Alarm.Alarm_beep_count=0;
    
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
int main(void)

{
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
COMP_InitTypeDef COMP_InitStructure;
__IO uint32_t StartUpCounter = 0, HSEStatus = 0;



	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x3000);  
	set_msi_2mhz();
	DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STANDBY | DBGMCU_STOP, ENABLE);

	io_init(); // Инициализация потров МК

	eeprom_write_default_settings(); // Проверка, заполнен ли EEPROM
  eeprom_read_settings(); // Чтение настроек из EEPROM
	
  screen=1;
	Power.sleep_time=Settings.Sleep_time;
  Power.Display_active=ENABLE;
//--------------------------------------------------------------------
	ADCData.DAC_voltage_raw=0x610;
  dac_init();
//--------------------------------------------------------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_RIIOSwitchConfig(RI_IOSwitch_GR6_1, ENABLE);
	
  COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_DAC2;
  COMP_InitStructure.COMP_OutputSelect = COMP_OutputSelect_None;
  COMP_InitStructure.COMP_Speed = COMP_Speed_Slow;
  COMP_Init(&COMP_InitStructure);
	  
	EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  EXTI_ClearITPendingBit(EXTI_Line22);

  NVIC_InitStructure.NVIC_IRQChannel = COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
//--------------------------------------------------------------------
	timer9_Config(); // Конфигурируем таймер накачки	
	timer10_Config();
	tim10_sound_activate();
	delay_ms(100);
	tim10_sound_deactivate();
//--------------------------------------------------------------------
	RTC_Config();	   // Конфигурируем часы
//--------------------------------------------------------------------
// инициализация дисплея
//--------------------------------------------------------------------
	delay_ms(50); // подождать установки напряжения
  display_on(); 
  LcdInit(); 
  LcdClear(); 
//--------------------------------------------------------------------
  adc_init();
  delay_ms(100);
  adc_calibration();
	delay_ms(10);
//--------------------------------------------------------------------
  EXTI8_Config();
  EXTI3_Config();
  EXTI4_Config();
  EXTI6_Config();
		
//	DataUpdate.Calibration_update_time_counter=100;
	DataUpdate.Need_batt_voltage_update=ENABLE;
	
	delay_ms(500); // подождать установки напряжения
  while(1) 
/////////////////////////////////
  {
		if(DataUpdate.Need_fon_update==ENABLE)geiger_calc_fon();
			
    keys_proccessing();
		adc_check_event();
		
    if(Power.Display_active  & Power.sleep_time==0 & !Alarm.Alarm_active)
			sleep_mode(ENABLE);  // ???? ??????? ?????, ?? ??????? ???????, ?? ?????????
    if(!Power.Display_active & Power.sleep_time>0 )
			sleep_mode(DISABLE); // ???? ?? ??????, ?? ??????? ????????, ?? ????????
    
    if(Power.Display_active)
    {
			if(Power.led_sleep_time>0)
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_13);// Включаем подсветку 
			} else {
				GPIO_SetBits(GPIOC,GPIO_Pin_13);// Выключаем подсветку  				
			}
			LcdClear_massive();
      if (screen==1)main_screen();
      if (screen==2)menu_screen();
      if (screen==3)stat_screen();
///////////////////////////////////////////////////////////////////////////////
		}

		if(!Power.USB_active)		// если USB не активен, можно уходить в сон
		{
			if(current_pulse_count<30)      // Если счетчик не зашкаливает, то можно уйти в сон
			{
				if(!Power.Pump_active & !Power.Sound_active)
				{
					PWR_UltraLowPowerCmd(ENABLE);
					PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);    // Переходим в сон
				} else
				{
						PWR_EnterSleepMode(PWR_Regulator_ON, PWR_SLEEPEntry_WFI);
				}
			}
		}else USB_work(); 		// если USB активен, попробовать передать данные
  }
/////////////////////////////////////////////////////////////////////////////// 
}
