#include "usb.h"
#include "main.h"
#include "delay.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb.h"
#include "keys.h"
#include "clock.h"
#include "flash_save.h"

#define max_fon_select 1
#define dose_select 2

extern __IO uint8_t Receive_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
extern __IO  uint32_t Send_length ;
uint8_t Send_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
uint32_t packet_sent=1;
uint32_t packet_receive=1;

#pragma O0
void USB_on()
{
//---------------------------------------------Включение USB------------------------------------
	set_pll_for_usb();
	Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  SYSCFG->PMC |= (uint32_t) SYSCFG_PMC_USB_PU; // Connect internal pull-up on USB DP line
	Power.USB_active=ENABLE;
}


void USB_work()
{
//---------------------------------------------Передача данных------------------------------------
	if (bDeviceState == CONFIGURED)
    {
      CDC_Receive_DATA();

#ifndef version_401 // Версия платы дозиметра 4.01+
			if(Settings.USB == 1) // MadOrc
#endif				
			{
			
				/*Check to see if we have data yet */
				if (Receive_length  == 1)
				{

					USB_not_active=0; // Сброс четчика неактивности USB 
					
					if(Receive_Buffer[0] == 0xD4) // передача основных данных в USB Gaiger
					{
						USB_send_madorc_data();
					}
					if(Receive_Buffer[0] == 0x31) // передача массива максимального фона
					{
						Send_length = prepare_data(max_fon_select, &USB_maxfon_massive_pointer, 0xF1, 0xF2); // Подготовка массива данных к передаче
					}
					if(Receive_Buffer[0] == 0x32) // передача массива дозы
					{
						Send_length = prepare_data(dose_select,    &USB_doze_massive_pointer,   0xF3, 0xF4); // Подготовка массива данных к передаче
					}
					if(Receive_Buffer[0] == 0x33) // передача настроек
					{
						USB_maxfon_massive_pointer=0;
						USB_doze_massive_pointer=0;
						USB_send_settings_data();
					}
					if(Receive_Buffer[0] == 0x39) // завершение передачи
					{
						USB_maxfon_massive_pointer=0;
						USB_doze_massive_pointer=0;
					}

					Receive_length = 0;
					if(Send_length>0)	CDC_Send_DATA ((unsigned char*)Send_Buffer,Send_length);
					while(packet_sent != 1);
					Send_length=0;
			}
		}
// -----------------------------------------------------------------------------------------------------------------------
	}
#ifdef version_401
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) // если 5В на USB не подается, то отключаем его
#else
	if ((USB_not_active>60) && (Settings.USB == 1)) // если 4 минуты USB не активно, то отключаем его
#endif
	{
		delay_ms(100);
#ifndef version_401 
		Settings.USB=0;
#endif
		usb_deactivate(0x00);
	}

}

// =========================================================================================
uint8_t prepare_data(uint32_t mode, uint16_t *massive_pointer, uint8_t start_key, uint8_t end_key) // Подготовка массива данных к передаче
{
	uint8_t address_hi=0;
  uint8_t address_lo=0;
	uint8_t fon_1_4=0;
  uint8_t fon_2_4=0;
  uint8_t fon_3_4=0;
  uint8_t fon_4_4=0;
	uint8_t used_lenght=0;
	uint32_t tmp=0;                // контрольная сумма
	
	while(used_lenght <= (VIRTUAL_COM_PORT_DATA_SIZE-7))
	{
		address_lo =  *massive_pointer       & 0xff;	
		address_hi = (*massive_pointer >> 8) & 0xff;	

		if(mode == max_fon_select)tmp=flash_read_max_fon_massive(*massive_pointer);
		if(mode == dose_select)tmp=flash_read_Doze_massive(*massive_pointer);
		
		fon_1_4 =  tmp        & 0xff;       
		fon_2_4 = (tmp >> 8)  & 0xff; 
		fon_3_4 = (tmp >> 16) & 0xff;
		fon_4_4 = (tmp >> 24) & 0xff;
		*massive_pointer = *massive_pointer + 1;

		Send_Buffer[used_lenght  ]=start_key;                                  // передать ключ
		Send_Buffer[used_lenght+1]=address_hi;                                 // передать по УСАПП 
		Send_Buffer[used_lenght+2]=address_lo;                                 // передать по УСАПП 
		Send_Buffer[used_lenght+3]=fon_4_4;                                    // передать по УСАПП 
		Send_Buffer[used_lenght+4]=fon_3_4;                                    // передать по УСАПП 
		Send_Buffer[used_lenght+5]=fon_2_4;                                    // передать по УСАПП 
		Send_Buffer[used_lenght+6]=fon_1_4;                                    // передать по УСАПП 
		
		used_lenght+=7;

		if(*massive_pointer>=FLASH_MAX_ELEMENT)
		{
			*massive_pointer=0;
			return used_lenght;
		}
	}
	return used_lenght;
}
// =========================================================================================


void USB_send_madorc_data()
{
//---------------------------------------------КомПорт для MadOrc------------------------------------
	uint8_t fonMegaHi=0;                //  старший байт индекса
  uint8_t fonHi=0;                //  старший байт индекса
  uint8_t fonLo=0;                // младший  байт индекса 
  uint8_t impulseHi=0;                //  старший байт индекса
  uint8_t impulseLo=0;                // младший  байт индекса 
  uint8_t voltIndexLo=0;                // младший  байт индекса 

	
	impulseLo =  madorc_impulse & 0xff;                         // разбить индекс на младший байт
	impulseHi = (madorc_impulse >> 8) & 0xff;                   // разбить индекс на старший байт  
  madorc_impulse=0;
	
  voltIndexLo = ((ADCData.Batt_voltage/10)-300) & 0xff;                         // разбить индекс на младший байт

	
	fonLo =  fon_level & 0xff;                                   // разбить индекс на младший байт
	fonHi = (fon_level >> 8) & 0xff;                             // разбить индекс на старший байт  
	fonMegaHi = (fon_level >> 16) & 0xff;                        // разбить индекс на старший байт  

	Send_Buffer[0]=0xD1;                                      // передать ключь Marorc
	Send_Buffer[1]=impulseHi;                                 // передать по УСАПП 
	Send_Buffer[2]=impulseLo;                                 // передать по УСАПП 
	Send_Buffer[3]=fonMegaHi;                                 // передать по УСАПП 
	Send_Buffer[4]=fonHi;                                     // передать по УСАПП 
	Send_Buffer[5]=fonLo;                                     // передать по УСАПП 
	Send_Buffer[6]=voltIndexLo;                               // передать по УСАПП 
	
	Send_length=7;
}


void USB_send_settings_data()
{
//---------------------------------------------КомПорт для MadOrc------------------------------------

	uint8_t count_time_hi=0;
  uint8_t count_time_lo=0;

  count_time_lo =  Settings.Second_count       & 0xff;	
  count_time_hi = (Settings.Second_count >> 8) & 0xff;	
		Send_Buffer[0]=0xF5;                                       // передать ключь
		Send_Buffer[1]=count_time_hi;                                 // передать по УСАПП 
		Send_Buffer[2]=count_time_lo;                                 // передать по УСАПП 
		Send_Buffer[3]=0xFF;                                    // передать по УСАПП 
		Send_Buffer[4]=0xFF;                                    // передать по УСАПП 
		Send_Buffer[5]=0xFF;                                    // передать по УСАПП 
		Send_Buffer[6]=0xFF;                                    // передать по УСАПП 
		Send_Buffer[7]=0xFF;                                       // передать по УСАПП 
	
		Send_length=7;
}



void USB_off()
{
//---------------------------------------------Отключение USB------------------------------------
	Power.USB_active=DISABLE;
	PowerOff();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
	set_msi();

	// Приводим в порядок систему тактирования (заплатка)
	sleep_mode(ENABLE);
	while(Power.Pump_active || Power.Sound_active);
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);    // Переходим в сон
	sleep_mode(DISABLE);

	Power.sleep_time=Settings.Sleep_time;
	Power.led_sleep_time=Settings.Sleep_time-3;
}


