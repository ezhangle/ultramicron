#ifndef __main_user_H
#define __main_user_H

#include "stm32l1xx.h"


// ------------- СЕРИЙНИК ! ------------- 
#define U_ID_0 (*(uint32_t*) 0x1FF80050) // MCU Serial
#define U_ID_1 (*(uint32_t*) 0x1FF80054)
#define U_ID_2 (*(uint32_t*) 0x1FF80064)

#ifdef service
	#define debug // отладка
#endif

typedef struct
{
  FunctionalState Need_batt_voltage_update;    // Необходимо обновить данные по напряжению батарейки
//  FunctionalState Need_geiger_voltage_update;  // Необходимо обновить данные по высокому напряжению 
  FunctionalState Need_fon_update;             // Необходимо обновить данные по высокому напряжению 
  FunctionalState Need_display_update;         // Необходимо обновить данные по высокому напряжению 
  uint16_t Batt_update_time_counter;            // счетчик таймаута измерения напряжения АКБ
//  uint8_t Calibration_update_time_counter;     // счетчик таймаута измерения напряжения АКБ
  uint8_t pump_counter_update_time;
	uint16_t  doze_sec_count;                    // Служебный счетчик для времени дозы
	uint16_t  days_sec_count;
//  uint8_t second_pump_counter;
//  uint8_t pump_pulse_by_impulse_counter;
  
}DataUpdateDef;


typedef struct
{
  uint32_t Batt_voltage_raw;        // Напряжение АКБ
  uint32_t Batt_voltage;            // Напряжение АКБ
  uint32_t Power_voltage;           // Напряжение питания МК
//  uint32_t Geiger_voltage_raw;      // Напряжение счетчика
//  uint32_t Geiger_voltage;          // Напряжение счетчика
  uint32_t Calibration_bit_voltage; // Цена одного бита
  uint32_t Procent_battery;         // Цена одного бита
  uint32_t DAC_voltage_raw;         // Уровень накачки для DAC
  
}ADCDataDef;

/*
typedef struct
{
	uint32_t Hour;       
  uint32_t Minute;
  uint32_t Second;       
  uint32_t Day;       
  uint32_t Month;       
  uint32_t Year;       
} RTCDef;
*/


typedef struct
{
  uint32_t Alarm_level;                             // Уровень аларма
  uint32_t Sleep_time;                              // время до ухода в сон
  uint32_t contrast;                                 // Контраст дисплея
//  uint32_t second_pump;                              // кол-во импульсов подкачки каждую секунду
//  uint32_t Geiger_angle_of_counter_characteristics; // угол наклона счетной хакактиристики в десятых долях процента на Вольт
//  uint32_t Geiger_plato_begin;                      // начало плато
//  uint32_t Geiger_plato;                            // ширина плато
//  uint32_t HV_ADC_Corr;                             // Корректировка ВВ делителя
//  uint32_t pump_pulse_by_impulse;                   // кол-во импульсов подкачки на каждый импульс датчика
//  uint32_t pump_skvagennost;                        // скваженность накачки
//  uint32_t Sound_freq;				    // Частота звука в кГц
  uint32_t Power_comp;				    // Уровень потребления
  uint32_t Geiger_voltage;			    // Напряжение датчика
	uint32_t Pump_Energy;
  uint32_t Display_reverse;                  // переворот дисплея
  uint32_t Sound;                            // Звук вкл-выкл
  uint32_t Second_count;
  uint32_t LSI_freq;
#ifndef version_401 // Версия платы дозиметра 4.01+
  uint32_t USB;
#endif
	uint32_t v4_target_pump;
	uint32_t Vibro;
}SettingsDef;


typedef struct
{
  uint32_t Alarm_beep_count;
  uint32_t Tick_beep_count;
  FunctionalState Alarm_active; 
  FunctionalState User_cancel; 
}AlarmDef;

#ifdef debug
typedef struct
{
  uint32_t total_wakeup;
  uint32_t total_cycle;
  uint32_t tim9_wakeup;
  uint32_t tim10_wakeup;
  uint32_t sensor_wakeup;
  uint32_t rtc_wakeup;
  uint32_t pump_wakeup;
  uint32_t comp_wakeup;
}WakeupDef;
#endif

typedef struct
{
  FunctionalState ADC_active;     // Сейчас работает АЦП
  FunctionalState Pump_active;    // В данный момент идет накачка
  FunctionalState Sound_active;   // В данный подается звук
  FunctionalState Display_active; // Включен дисплей
  FunctionalState USB_active;     // Включен USB
  FunctionalState sleep_now;     // Активность сна

  uint32_t APB1ENR;  //
	
	uint32_t sleep_time;  //
  uint32_t led_sleep_time;  //
  FunctionalState Pump_deny;  
}PowerDef;

static __IO uint8_t  timer_is_reload = 0;	// counts 1ms timeTicks
extern uint16_t key; // массив нажатых кнопок [012]
extern uint32_t ix;
extern uint32_t ix_update;

//#define count_seconds 75 // 
extern uint16_t Detector_massive[120+1];

#define doze_length_day 144 // 1 день интервалами по 10 минут
#define doze_length_week 1008 // 7 дней интервалами по 10 минут  (6*24*10)=1008 (7 дней)
extern uint32_t Doze_massive[doze_length_week+1]; // 1 ячейка = 10 минут
extern uint32_t max_fon_massive[doze_length_week+1]; // 1 ячейка = 10 минут
extern uint16_t USB_maxfon_massive_pointer;
extern uint16_t USB_doze_massive_pointer;

extern uint16_t Doze_sec_count;
extern uint32_t Doze_day_count;
extern uint32_t Doze_hour_count;
extern uint32_t Doze_week_count;
extern uint32_t Max_fon;
extern uint8_t  main_menu_stat;
extern uint32_t menu_select;
extern FunctionalState enter_menu_item;
extern uint8_t screen;
extern uint8_t stat_screen_number;
extern uint16_t Detector_massive_pointer;
extern uint16_t pump_counter_avg_impulse_by_1sec[2];
extern uint32_t fon_level;
extern uint32_t working_days;
extern uint8_t  auto_speedup_factor;
extern uint32_t madorc_impulse;
extern uint32_t USB_not_active;
extern uint32_t last_count_pump_on_impulse;
extern FunctionalState pump_on_impulse;

extern FunctionalState hidden_menu;

extern FunctionalState poweroff_state;

extern uint32_t fullstop;
extern FunctionalState Sound_key_pressed;
extern FunctionalState Pump_on_alarm;
extern uint16_t current_pulse_count;

extern ADCDataDef    ADCData;
extern DataUpdateDef DataUpdate;
extern PowerDef      Power;
extern SettingsDef   Settings;
extern AlarmDef      Alarm;
#ifdef debug
extern WakeupDef     Wakeup;
extern uint32_t debug_wutr;
#endif
extern uint8_t pump_count;

void sleep_mode(FunctionalState sleep);

#endif
