#ifndef __eeprom_user_H
#define __eeprom_user_H
#include <STM32L1xx.h>


/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define DATA_EEPROM_START_ADDR     0x08080000
#define DATA_EEPROM_END_ADDR       0x080827FF
#define DATA_EEPROM_PAGE_SIZE      0x8

#define Alarm_level_address                                     0x00
#define Sleep_time_address                                      0x04
#define Display_reverse_address                                 0x08
#define contrast_address                                        0x0C
#define second_pump_address                                     0x10
#define Geiger_angle_of_counter_characteristics_address         0x14
#define Sound_address                                           0x18
#define Geiger_plato_begin_address                              0x1C
#define Geiger_plato_address                                    0x20
#define HV_ADC_Corr_address                                     0x24
#define pump_pulse_by_impulse_address                           0x28
#define pump_skvagennost_address                                0x2C
#define Geiger_voltage_address																	0x30
#define Pump_Energy_address																			0x34
#define Second_count_address																		0x38
#define Sound_freq_address																			0x3C
#define Led_Sleep_time_address                                  0x40
#define LSI_freq_address                                  			0x44

extern uint32_t NbrOfPage, Address;
  


void eeprom_erase(void);
uint32_t eeprom_read(uint32_t address_of_read);
void eeprom_write(uint32_t address_of_read, uint32_t data);
void eeprom_write_settings(void);
void eeprom_apply_settings(void);
void eeprom_write_default_settings(void);
void eeprom_read_settings(void);


#endif
