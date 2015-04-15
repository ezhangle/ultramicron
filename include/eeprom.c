#include "stm32l1xx.h"
#include "eeprom.h"
#include "main.h"
#include "ext2760.h"
#include "delay.h"
#include "timers.h"
#include "clock.h"

__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus DataMemoryProgramStatus = PASSED;
uint32_t NbrOfPage = 0, j = 0, Address = 0;

//**************************************************************************
// Запись параметров по умолчанию
    void eeprom_write_default_settings(void)
    {
      if(eeprom_read(Second_count_address) == 0x00)
      { 
        Settings.Alarm_level=50;
        Settings.Sound=ENABLE;
        Settings.Sleep_time=60;
        Settings.Display_reverse=2;
				Settings.Second_count=250;
				Settings.LSI_freq=37000;
				Settings.Geiger_voltage=360;
				Settings.Power_comp=0;
        eeprom_write_settings(); // Запись
      }  
    }
//**************************************************************************

void eeprom_write_settings(void)
{
	
  //Запись параметров в EEPROM
  if(eeprom_read(Alarm_level_address)    !=Settings.Alarm_level)    eeprom_write(Alarm_level_address,    Settings.Alarm_level);
  if(eeprom_read(Sleep_time_address)     !=Settings.Sleep_time )    eeprom_write(Sleep_time_address,     Settings.Sleep_time);
  if(eeprom_read(contrast_address)       !=Settings.contrast)       eeprom_write(contrast_address,       Settings.contrast);
  if(eeprom_read(Display_reverse_address)!=Settings.Display_reverse)eeprom_write(Display_reverse_address,Settings.Display_reverse);
	if(eeprom_read(Second_count_address)   !=Settings.Second_count)   eeprom_write(Second_count_address,   Settings.Second_count);
	if(eeprom_read(Sound_address)          !=Settings.Sound)          eeprom_write(Sound_address,          Settings.Sound);
	if(eeprom_read(Power_comp_address)     !=Settings.Power_comp)          eeprom_write(Power_comp_address,          Settings.Power_comp);
	if(eeprom_read(Geiger_voltage_address) !=Settings.Geiger_voltage) eeprom_write(Geiger_voltage_address, Settings.Geiger_voltage);
	if(Settings.LSI_freq != 0x00) // если запустился кварц, попытки сохранения игнорировать
	{
		if(eeprom_read(LSI_freq_address)       !=Settings.LSI_freq)       eeprom_write(LSI_freq_address,       Settings.LSI_freq);
	}
	Power.led_sleep_time=Settings.Sleep_time-3;
}

//**************************************************************************
void eeprom_apply_settings(void)
{
  if(eeprom_read(contrast_address)!=Settings.contrast)       
	{
		display_off();
		delay_ms(200);
		display_on();
	}
	// -------------------------------------------------------------------
  if(eeprom_read(Display_reverse_address)!=Settings.Display_reverse)
	{
		display_off();
		delay_ms(200);
		display_on();
	}
	// -------------------------------------------------------------------
	if(eeprom_read(Geiger_voltage_address) !=Settings.Geiger_voltage) 
 	{
		ADCData.DAC_voltage_raw=((Settings.Geiger_voltage*1000)/30/11);
		ADCData.DAC_voltage_raw=(ADCData.DAC_voltage_raw*1000)/ADCData.Calibration_bit_voltage;
		DAC_SetChannel2Data(DAC_Align_12b_R, ADCData.DAC_voltage_raw);   /* Set DAC Channel2 DHR register: DAC_OUT2 = (1.224 * 128) / 256 = 0.612 V */
 	}
	// -------------------------------------------------------------------
	if(eeprom_read(LSI_freq_address)       !=Settings.LSI_freq)
  {
		if(Settings.LSI_freq != 0x00) // если запустился кварц, попытки сохранения игнорировать
		{
			eeprom_write(LSI_freq_address,Settings.LSI_freq);   NVIC_SystemReset();
		}
	}
}

//**************************************************************************

void eeprom_read_settings(void)
{
  //Чтение параметров из EEPROM
  Settings.Alarm_level=           eeprom_read(Alarm_level_address);
  Settings.Sleep_time=            eeprom_read(Sleep_time_address);
  Settings.contrast=              eeprom_read(contrast_address);
  Settings.Display_reverse=				eeprom_read(Display_reverse_address);
  Settings.Second_count=          eeprom_read(Second_count_address);
	Settings.Sound=                 eeprom_read(Sound_address);
  Settings.LSI_freq=  			      eeprom_read(LSI_freq_address);  
	Settings.Power_comp= 			      eeprom_read(Power_comp_address);  
	Settings.Geiger_voltage=				eeprom_read(Geiger_voltage_address);
	Power.led_sleep_time=Settings.Sleep_time-3;
}


//**************************************************************************
void eeprom_erase(void)
{
  DATA_EEPROM_Unlock(); // разблокировка перед записью
  
  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);	
  
  /*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
  DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */  
  Address = DATA_EEPROM_START_ADDR;
  
  NbrOfPage = ((DATA_EEPROM_END_ADDR - Address) + 1 ) >> 2; 
  
  /* Erase the Data EEPROM Memory pages by Word (32-bit) */
  for(j = 0; j < NbrOfPage; j++)
  {
    FLASHStatus = DATA_EEPROM_EraseWord(Address + (4 * j));
  }
  DATA_EEPROM_Lock();
  
  /* Check the correctness of written data */
  while(Address < DATA_EEPROM_END_ADDR)
  {
    if(*(__IO uint32_t*)Address != 0x0)
    {
      DataMemoryProgramStatus = FAILED;
      //ОШИБКА стирания !!!!
      return;
    }
    Address = Address + 4;
  }
  
}



//************************************************************************
uint32_t eeprom_read(uint32_t address_of_read)
{
  
  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);	
  
  /*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
  DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */  
  Address = DATA_EEPROM_START_ADDR+address_of_read;
  if(Address > DATA_EEPROM_END_ADDR)
  {
    // ОШИБКА вне диапазона eeprom !!!!
    return 0x00;
  }
  
  // чтение данных
  return  *(__IO uint32_t*)Address;
}



//************************************************************************

void eeprom_write(uint32_t address_of_read, uint32_t data)
{
  
  DATA_EEPROM_Unlock(); // разблокировка перед записью
  
  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);	
  
  /*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
  DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */  
  Address = DATA_EEPROM_START_ADDR+address_of_read;
  if(Address > DATA_EEPROM_END_ADDR)
  {
    // ОШИБКА вне диапазона eeprom !!!!
    DATA_EEPROM_Lock();
    return;
  }
  
  // Запись данных по адресу
  FLASHStatus = DATA_EEPROM_ProgramWord(Address, data);
  DATA_EEPROM_Lock();  
  
  if(FLASHStatus != FLASH_COMPLETE)
  {
    // ОШИБКА записи eeprom !!!!
    return;
  }
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
  
  
  // Проверка записанных данных 
  if(*(__IO uint32_t*)Address != data)
  {
    DataMemoryProgramStatus = FAILED;
    // ОШИБКА записи !!!!!
    return;
  }
  
  
  
}

