#include "usb.h"
#include "main.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb.h"
#include "clock.h"

extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
uint8_t Send_Buffer[64];
uint32_t packet_sent=1;
uint32_t packet_receive=1;

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
      /*Check to see if we have data yet */
      if (Receive_length  != 0)
      {
        if (packet_sent == 1)
          CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
        Receive_length = 0;
      }
    }
}

void USB_send_gamma3_data()
{
//---------------------------------------------КомПорт от гаммы 3------------------------------------
  unsigned char IndexHi;                //  старший байт индекса
  unsigned char IndexLo;                // младший  байт индекса 
  unsigned char voltIndexHi;                //  старший байт индекса
  unsigned char voltIndexLo;                // младший  байт индекса 
  unsigned char crc;                // контрольная сумма
	unsigned int  Volt;

	Volt=ADCData.Batt_voltage;
	crc=((fon_level+Volt) & 0xff);			    //вычисление контрольной суммы

	IndexLo =  fon_level & 0xff;                         // разбить индекс на младший байт
	IndexHi = (fon_level >> 8) & 0xff;                   // разбить индекс на старший байт  
	voltIndexLo =  Volt & 0xff;                         // разбить индекс на младший байт
	voltIndexHi = (Volt >> 8) & 0xff;                   // разбить индекс на старший байт  
	
	Receive_Buffer[0]=0xE9;                                      // передать ключь Гаммы3
	Receive_Buffer[1]=IndexHi;                                   // передать по УСАПП 
	Receive_Buffer[2]=IndexLo;                                   // передать по УСАПП 
	Receive_Buffer[3]=0xE5;                                      // передать ключь напряжения
	Receive_Buffer[4]=voltIndexHi;                               // передать по УСАПП 
	Receive_Buffer[5]=voltIndexLo;                               // передать по УСАПП 
	Receive_Buffer[6]=crc;                                       // передать контрольную сумму
	Receive_Buffer[7]=0xA4;                                      // передать ключь окончания передачи
	
	Receive_length=8;
}

void USB_send_madorc_data()
{
//---------------------------------------------КомПорт для MadOrc------------------------------------
  uint8_t i;
	uint8_t fonMegaHi=0;                //  старший байт индекса
  uint8_t fonHi=0;                //  старший байт индекса
  uint8_t fonLo=0;                // младший  байт индекса 
  uint8_t impulseHi=0;                //  старший байт индекса
  uint8_t impulseLo=0;                // младший  байт индекса 
  uint8_t voltIndexHi=0;                //  старший байт индекса
  uint8_t voltIndexLo=0;                // младший  байт индекса 
  uint32_t crc=0;                // контрольная сумма

	
	impulseLo =  Detector_massive[Detector_massive_pointer] & 0xff;                         // разбить индекс на младший байт
	impulseHi = (Detector_massive[Detector_massive_pointer] >> 8) & 0xff;                   // разбить индекс на старший байт  

  voltIndexLo =  ADCData.Batt_voltage & 0xff;                         // разбить индекс на младший байт
	voltIndexHi = (ADCData.Batt_voltage >> 8) & 0xff;                   // разбить индекс на старший байт  

	
	fonLo =  fon_level & 0xff;                                   // разбить индекс на младший байт
	fonHi = (fon_level >> 8) & 0xff;                             // разбить индекс на старший байт  
	fonMegaHi = (fon_level >> 16) & 0xff;                        // разбить индекс на старший байт  

	Receive_Buffer[0]=0xD1;                                      // передать ключь Marorc
	Receive_Buffer[1]=impulseHi;                                 // передать по УСАПП 
	Receive_Buffer[2]=impulseLo;                                 // передать по УСАПП 
	Receive_Buffer[3]=fonMegaHi;                                 // передать по УСАПП 
	Receive_Buffer[4]=fonHi;                                     // передать по УСАПП 
	Receive_Buffer	[5]=fonLo;                                     // передать по УСАПП 
	Receive_Buffer[6]=voltIndexHi;                               // передать по УСАПП 
	Receive_Buffer[7]=voltIndexLo;                               // передать по УСАПП 
                              	for(i=1;i<8;i++)crc+=Receive_Buffer[i]; //расчет контрольной суммы
	Receive_Buffer[8]=crc & 0xff;                                // передать контрольную сумму
	Receive_Buffer[9]=0xD2;                                      // передать ключь окончания передачи
	
	Receive_length=10;
}

void USB_off()
{
//---------------------------------------------Отключение USB------------------------------------
	Power.USB_active=DISABLE;
	PowerOff();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
	set_msi_2mhz();
}


