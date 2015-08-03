#include "stm32l1xx.h"
#include "main.h"

#pragma O0

#include "flash_save.h"
                    

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

__IO FLASH_Status FLASHStatus_eeprom = FLASH_COMPLETE;
__IO TestStatus MemoryProgramStatus_eeprom = PASSED;


//////////////////////////////////////////////////////////////////////////////////////
void full_erase_flash(void) // Erase full dataflash
{  
	uint32_t j = 0, Address = 0, NbrOfPage = 0;

  /* Unlock the FLASH Program memory */ 
  FLASH_Unlock();

  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

  Address = FLASH_START_ADDR;
  NbrOfPage = ((FLASH_END_ADDR - Address) + 1 ) >> 8;   
  /* Erase the FLASH Program memory pages */
  for(j = 0; j < NbrOfPage; j++)
  {
    FLASHStatus_eeprom = FLASH_ErasePage(Address + (FLASH_PAGE_SIZE * j));

    if (FLASHStatus_eeprom != FLASH_COMPLETE)
    {
      MemoryProgramStatus_eeprom = FAILED;
    }
    else
    {
      FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                      | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
    }     
  }
   
  FLASH_Lock();
}

//////////////////////////////////////////////////////////////////////////////////////
void page_erase_flash(uint32_t page) // Erase 32 elements
{
uint32_t Address = 0;	
if(FLASH_MAX_PAGE >= page) // если не за границами диапазона
{
	
  /* Unlock the FLASH Program memory */ 
  FLASH_Unlock();

  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

  Address = FLASH_START_ADDR + (page * FLASH_PAGE_SIZE);
  /* Erase the FLASH Program memory pages */
  FLASHStatus_eeprom = FLASH_ErasePage(Address);

  if (FLASHStatus_eeprom != FLASH_COMPLETE)
  {
    MemoryProgramStatus_eeprom = FAILED;
  }
  else
  {
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                    | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  }     
   
  FLASH_Lock();
}
}

//////////////////////////////////////////////////////////////////////////////////////
void flash_write_page(uint32_t page) // Write 32 element of massive ram_Doze_massive and ram_max_fon_massive
{  
uint32_t Address = 0;

if(page <= FLASH_MAX_PAGE) // если не за границами диапазона
{	

	Address = FLASH_START_ADDR + (page * FLASH_PAGE_SIZE);
	
	///// ≈сли в страницу уже что-то записано, ее надо стереть
	/////if(((*(__IO uint32_t*)Address) != 0xFFFFFFFF) && ((*(__IO uint32_t*)Address) != 0x00000000))

	page_erase_flash(page); // —тереть страницу перед записью
	
	/* Unlock the FLASH Program memory */ 
  FLASH_Unlock();

  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  
  /* Write the FLASH Program memory using HalfPage operation */
  FLASHStatus_eeprom = FLASH_ProgramHalfPage(Address, ram_Doze_massive);

  if(FLASHStatus_eeprom == FLASH_COMPLETE)
  {
  }
  else
  {
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                    | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  } 

	Address = FLASH_START_ADDR + (page * FLASH_PAGE_SIZE)+(FLASH_PAGE_SIZE/2);
  /* Write the FLASH Program memory using HalfPage operation */
  FLASHStatus_eeprom = FLASH_ProgramHalfPage(Address, ram_max_fon_massive);

  if(FLASHStatus_eeprom == FLASH_COMPLETE)
  {
  }
  else
  {
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                    | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  } 

  FLASH_Lock();
}
}


//////////////////////////////////////////////////////////////////////////////////////
uint32_t flash_read_Doze_massive(uint32_t virt_element)
{  
	uint32_t Address=0, page=0, index=0;
	
	if(virt_element<doze_length) // 0-31
	{
		return ram_Doze_massive[virt_element];
	} else // >31
	{
		index=virt_element-doze_length;
		page=DataUpdate.current_flash_page;
		if(page==0) //если достигли начала, взад!
		{
			page=FLASH_MAX_PAGE;
		} else
		{
			page--; // преведуща€ страничка
		}

//////////////////////////////////////////////////
		while(1)
		{
			if(index<doze_length) // Ќашли нужную страничку
			{
				Address = FLASH_START_ADDR + (page * FLASH_PAGE_SIZE); // вычисл€ем адрес начала страницы
				Address = Address + (index*4); // ¬ычисл€ем адрес элемента на странице
				return 	(*(__IO uint32_t*)Address); // „итаем данные из флеша
			}
			else // ≈сли индекс не в этой странице пам€ти
			{
				index=index-doze_length;
				if(page==0) //если достигли начала, взад!
				{
					page=FLASH_MAX_PAGE;
				} else
				{
					page--; // преведуща€ страничка
				}
			}
		}
//////////////////////////////////////////////////		
	}
}



//////////////////////////////////////////////////////////////////////////////////////
uint32_t flash_read_max_fon_massive(uint32_t virt_element)
{  
	uint32_t Address=0, page=0, index=0;
	
	if(virt_element<doze_length) // 0-31
	{
		return ram_max_fon_massive[virt_element];
	} else // >31
	{
		index=virt_element-doze_length;
		page=DataUpdate.current_flash_page;
		if(page==0) //если достигли начала, взад!
		{
			page=FLASH_MAX_PAGE;
		} else
		{
			page--; // преведуща€ страничка
		}

//////////////////////////////////////////////////
		while(1)
		{
			if(index<doze_length) // Ќашли нужную страничку
			{
				Address = FLASH_START_ADDR + (page * FLASH_PAGE_SIZE); // вычисл€ем адрес начала страницы
				Address = Address + (index*4) + (FLASH_PAGE_SIZE/2); // ¬ычисл€ем адрес элемента на странице
				return 	(*(__IO uint32_t*)Address); // „итаем данные из флеша
			}
			else // ≈сли индекс не в этой странице пам€ти
			{
				index=index-doze_length;
				if(page==0) //если достигли начала, взад!
				{
					page=FLASH_MAX_PAGE;
				} else
				{
					page--; // преведуща€ страничка
				}
			}
		}
//////////////////////////////////////////////////		
	}
}
