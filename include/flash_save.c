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
if(((FLASH_END_ADDR-FLASH_START_ADDR)/FLASH_PAGE_SIZE) > page) // если не за границами диапазона
{
	
  /* Unlock the FLASH Program memory */ 
  FLASH_Unlock();

  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

  Address = FLASH_START_ADDR + (page * FLASH_PAGE_SIZE * 4);
  
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

if(((((FLASH_END_ADDR-FLASH_START_ADDR)+1)/FLASH_PAGE_SIZE)) > page) // если не за границами диапазона
{	

	Address = FLASH_START_ADDR + (page * FLASH_PAGE_SIZE);
	
	// Если в страницу уже что-то записано, ее надо стереть
	if(((*(__IO uint32_t*)Address) != 0xFFFFFFFF) && ((*(__IO uint32_t*)Address) != 0x00000000))
		page_erase_flash(page);
	
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

