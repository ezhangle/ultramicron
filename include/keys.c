#include "main.h"
#include "menu.h"
#include "delay.h"
#include "ext2760.h"
#include "eeprom.h"
#include "usb.h"
#include "stm32l1xx_ulp_modes.h"
#include <stdio.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������� ������ �������
void  plus_alarm(uint32_t *param) // +
{
  //  Menu_list[*param>-1].Parameter_value
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
// ��������� ������ ���
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
// ���-����
void  plus_on(uint32_t *param) // ���
{  
  *Menu_list[*param-1].Parameter_value=1;
}

void minus_off(uint32_t *param) // ����
{
  *Menu_list[*param-1].Parameter_value=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  plus_doze_reset(uint32_t *param) // ����� ����
{
	int i;
	for(i=doze_length;i>0;i--)
	{
		Doze_massive[i]=0;
		Doze_count+=Doze_massive[i];
	}
	Doze_count=0;
DataUpdate.doze_sec_count=0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// +1 -1
void  plus_one(uint32_t *param) // +1
{
  //���� �������� ��������� ����������� ���������� ��������, �� ��������� �� �������
  if(*Menu_list[*param-1].Parameter_value >= Menu_list[*param-1].Max_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Min_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value+1; //+1
  }
}

void minus_one(uint32_t *param) // -1
{
  //���� �������� ��������� ���������� ���������� ��������, �� ��������� �� ��������
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
  //���� �������� ��������� ����������� ���������� ��������, �� ��������� �� �������
  if(*Menu_list[*param-1].Parameter_value >= Menu_list[*param-1].Max_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Min_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value+10; //+10
  }
}

void minus_ten(uint32_t *param) // -10
{
  //���� �������� ��������� ���������� ���������� ��������, �� ��������� �� ��������
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
  //���� �������� ��������� ����������� ���������� ��������, �� ��������� �� �������
  if(*Menu_list[*param-1].Parameter_value >= Menu_list[*param-1].Max_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Min_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value+50; //+50
  }
}

void minus_50(uint32_t *param) // -50
{
  //���� �������� ��������� ���������� ���������� ��������, �� ��������� �� ��������
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
  //���� �������� ��������� ����������� ���������� ��������, �� ��������� �� �������
  if(*Menu_list[*param-1].Parameter_value >= Menu_list[*param-1].Max_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Min_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value+500; //+500
  }
}

void minus_500(uint32_t *param) // -500
{
  //���� �������� ��������� ���������� ���������� ��������, �� ��������� �� ��������
  if(*Menu_list[*param-1].Parameter_value <= Menu_list[*param-1].Min_limit)
  {
    *Menu_list[*param-1].Parameter_value=Menu_list[*param-1].Max_limit;
  } else {
    *Menu_list[*param-1].Parameter_value=*Menu_list[*param-1].Parameter_value-500; // -500
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������������ � ����������
void plus_reboot(uint32_t *param) // ������������
{
	LcdClear_massive();
	sprintf (lcd_buf, "������������...."); // ����� � ����� �������� ��������
	LcdString(1,5); // // ������� ������� ������� ���������� ������
  LcdUpdate(); // ���������� ������ �� ��������������� �����-������ �� �������
	
  delay_ms(3000);
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_4);
  IWDG_SetReload(2);
  IWDG_ReloadCounter();
  IWDG_Enable();
  while(1);
}

void plus_poweroff(uint32_t *param) // ����������
{
  Standbuy_Mode();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������������ � ����������
void usb_activate(uint32_t *param) // ��������� USB
{
if(!Power.USB_active)
{
	USB_on(); 		
	LcdClear_massive();
	sprintf (lcd_buf, "USB �����������"); // ����� � ����� �������� ��������
	LcdString(1,5); // // ������� ������� ������� ���������� ������
  LcdUpdate(); // ���������� ������ �� ��������������� �����-������ �� �������
  delay_ms(1000);
}
}

void usb_deactivate(uint32_t *param) // ���������� USB
{
if(Power.USB_active)
{
	USB_off(); 		
	LcdClear_massive();
	sprintf (lcd_buf, "USB  ��������"); // ����� � ����� �������� ��������
	LcdString(1,5); // // ������� ������� ������� ���������� ������
  LcdUpdate(); // ���������� ������ �� ��������������� �����-������ �� �������
  delay_ms(1000);
}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void keys_proccessing(void)
{
  extern uint16_t key;
  extern SettingsDef Settings;
  
  /////////////////////////////////
  if(key & 0x2)  // ������ +
  {
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));
    delay_ms(10);
    if (screen==2 && enter_menu_item==DISABLE)menu_select--;
		if (screen==1)main_menu_stat++;
		if (screen==3)
		{
			if(stat_screen_number==1)
				{
					stat_screen_number=0;
				} else {
					stat_screen_number++;
				}
		}
    if(menu_select>max_struct_index)menu_select=max_struct_index;
    key=0;
		
		///////////
    if(enter_menu_item==ENABLE)
    {
      
      if(Menu_list[menu_select-1].Plus_reaction!=0x00) // ���� ����� ������� ����������, �� ��������� ��.
      {
        (*Menu_list[menu_select-1].Plus_reaction)(&menu_select); // ������  ������� - �������� ���� ��� :)
      }
    }    
  }
  
  
  
  
  /////////////////////////////////
  if(key & 0x4)  // ������ -
  {
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));
    
    delay_ms(10);
    if (screen==2 && enter_menu_item==DISABLE)menu_select++;
		if (screen==1)main_menu_stat--;
		if (screen==3)
		{
			if(stat_screen_number==0)
				{
					stat_screen_number=1;
				} else {
					stat_screen_number--;
				}
		}
    if(menu_select>max_struct_index)menu_select=0;
    key=0;
    
//		DataUpdate.Need_display_update=ENABLE;
		
    ///////////
    if(enter_menu_item==ENABLE) // �������
    {
      if(Menu_list[menu_select-1].Minus_reaction!=0x00) // ���� ����� ������� ����������, �� ��������� ��.
      {
        (*Menu_list[menu_select-1].Minus_reaction)(&menu_select); // ������  ������� - �������� ���� ��� :)
      }
    }
    
    
  }
  /////////////////////////////////
  if (key & 0x1) // ������ ��nu
  {
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4));
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6));

    LcdClear(); // ������� ����� ����� � �������
    
    delay_ms(10);
    key=0;
    
//		DataUpdate.Need_display_update=ENABLE;
		
    ///////////
    if(menu_select>0)
    {
      if(enter_menu_item==DISABLE){enter_menu_item=ENABLE;}
      else                    
      {
        enter_menu_item=DISABLE;
				eeprom_apply_settings(); //��������� ���������
        eeprom_write_settings(); //��������� ���������
      }
    }
    if(menu_select==0)screen++;
  }
  if (screen>3) screen=1;
  
}
