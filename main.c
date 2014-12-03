#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"                  // Device header

#include "main.h"
#include "delay.h"
#include "io_ports.h"
#include "rtc.h"
#include "timers.h"
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
#include "power.h"


uint16_t key; // массив нажатых кнопок [012]
uint32_t ix;
uint32_t ix_update;

uint16_t Detector_massive[120+1];
uint32_t Doze_massive[doze_length_week+1]; // 1 ячейка = 10 минут, на протяжении суток
uint32_t max_fon_massive[doze_length_week+1]; // 1 ячейка = 10 минут, на протяжении суток
uint16_t USB_maxfon_massive_pointer=0;
uint16_t USB_doze_massive_pointer=0;
uint16_t current_pulse_count=0;
uint8_t pump_count=0;

uint32_t Doze_day_count=0;
uint32_t Doze_week_count=0;
uint32_t Doze_hour_count=0;
uint32_t Max_fon=0;
uint8_t  main_menu_stat=0;
uint16_t Detector_massive_pointer=0;
uint8_t  auto_speedup_factor=0;
uint32_t USB_not_active=0;
uint32_t last_count_pump_on_impulse=0;
FunctionalState pump_on_impulse=DISABLE;
uint32_t menu_select=0;
#ifdef debug
uint32_t debug_wutr=0;
#endif
FunctionalState enter_menu_item=DISABLE;
uint8_t screen=1;
uint8_t stat_screen_number=0;
uint16_t pump_counter_avg_impulse_by_1sec[2];
uint32_t fon_level=0;

uint32_t madorc_impulse=0;

FunctionalState Sound_key_pressed=DISABLE;

DataUpdateDef DataUpdate;
ADCDataDef ADCData;
SettingsDef Settings;
AlarmDef Alarm;
PowerDef Power;
#ifdef debug
WakeupDef Wakeup;
#endif

void sleep_mode(FunctionalState sleep)
{ 
  if(Settings.Sleep_time>0 && !Power.USB_active)
  {
		set_msi(sleep);
    if(sleep)
    {
			RTC_ITConfig(RTC_IT_WUT, DISABLE);

			Power.led_sleep_time=0;
			GPIO_SetBits(GPIOC,GPIO_Pin_13);// Выключаем подсветку  				

      display_off(); // выключить дисплей
 			GPIO_ResetBits(GPIOA,GPIO_Pin_7);// Фиксируем режим 1.8 вольта, с низким потреблением ножки
			delay_ms(1000); // подождать установки напряжения
			PWR_FastWakeUpCmd(DISABLE);
			PWR_UltraLowPowerCmd(ENABLE); 
			PWR_PVDCmd(DISABLE);
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
			DataUpdate.Need_display_update=ENABLE;
			adc_check_event(); // запустить преобразование
			RTC_ITConfig(RTC_IT_WUT, ENABLE);
			sound_deactivate();
    }
  } 
}


void geiger_calc_fon(void)
{
	DataUpdate.Need_fon_update=DISABLE;  
	DataUpdate.Need_display_update=ENABLE;
  if(fon_level>Settings.Alarm_level && Settings.Alarm_level>0 && Alarm.Alarm_active==DISABLE)
  {
    Alarm.Alarm_active=ENABLE;
		Alarm.User_cancel=DISABLE;
		if(Power.Display_active==DISABLE)
		{
			screen=1;
			Power.sleep_time=Settings.Sleep_time;
			Power.led_sleep_time=Settings.Sleep_time-3;
			sleep_mode(DISABLE);
			sound_activate();
		} else sound_activate();
    
  }
  if((Alarm.Alarm_active && fon_level<Settings.Alarm_level) || (Alarm.Alarm_active && Settings.Alarm_level==0))
  {
		sound_deactivate();
    Power.Sound_active=DISABLE;
    Alarm.Alarm_active=DISABLE;
    Alarm.User_cancel=DISABLE;
    Alarm.Alarm_beep_count=0;
    
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
int main(void)

{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x3000);  
	set_msi(DISABLE);
	DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STANDBY | DBGMCU_STOP, DISABLE);
	
  set_bor();
	Power.sleep_now=DISABLE;
	
  Settings.Geiger_voltage=360; // Напряжение на датчике 360 вольт
  Settings.Pump_Energy=350;    // энергия накачки 350 мТл
	
	io_init(); // Инициализация потров МК

	eeprom_write_default_settings(); // Проверка, заполнен ли EEPROM
  eeprom_read_settings(); // Чтение настроек из EEPROM
	
  screen=1;
	Power.USB_active=DISABLE;
	Power.sleep_time=Settings.Sleep_time;
  Power.Display_active=ENABLE;
	
	ADCData.DAC_voltage_raw=0x610;
		
  dac_init();
	comp_on();
	timer9_Config(); // Конфигурируем таймер накачки	
	timer10_Config();
	tim2_Config();
	sound_activate();
	delay_ms(100);
	sound_deactivate();
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
		
	DataUpdate.Need_batt_voltage_update=ENABLE;
	
	delay_ms(500); // подождать установки напряжения
  while(1) 
/////////////////////////////////
  {
		if(DataUpdate.Need_fon_update==ENABLE)	geiger_calc_fon();
    if(key>0)																keys_proccessing();
		if(DataUpdate.Need_batt_voltage_update)	adc_check_event();
    
		if((Power.sleep_time>0)&(!Power.Display_active))sleep_mode(DISABLE); // Если дисплей еще выключен, а счетчик сна уже отсчитывает, поднимаем напряжение и включаем дисплей
    
		if(Power.Display_active)
    {
			if(Power.sleep_time==0 && !Alarm.Alarm_active) sleep_mode(ENABLE);  // Счетчик сна досчитал до нуля, а дисплей еще активен, то выключаем его и понижаем напряжение
			if(Power.led_sleep_time>0)
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_13);// Включаем подсветку 
			} else {
				GPIO_SetBits(GPIOC,GPIO_Pin_13);// Выключаем подсветку  				
			}			
			if(DataUpdate.Need_display_update==ENABLE)
			{
				DataUpdate.Need_display_update=DISABLE;
				LcdClear_massive();
				if (screen==1)main_screen();
				if (screen==2)menu_screen();
				if (screen==3)stat_screen();
			}
///////////////////////////////////////////////////////////////////////////////
		}

		if(!Power.USB_active)		// если USB не активен, можно уходить в сон
		{
			if(current_pulse_count<30)      // Если счетчик не зашкаливает, то можно уйти в сон
			{
				if(!Power.Pump_active && !Power.Sound_active)
				{
					PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);    // Переходим в сон
#ifdef debug
 					Wakeup.total_wakeup++;
					DataUpdate.Need_display_update=ENABLE;
#endif

				} else
				{
						PWR_EnterSleepMode(PWR_Regulator_ON, PWR_SLEEPEntry_WFI);
#ifdef debug
  					Wakeup.total_wakeup++;
						DataUpdate.Need_display_update=ENABLE;
#endif
				}
			}
		}else USB_work(); 		// если USB активен, попробовать передать данные
#ifdef debug
 					Wakeup.total_cycle++;
					DataUpdate.Need_display_update=ENABLE;
#endif

  }
/////////////////////////////////////////////////////////////////////////////// 
}
