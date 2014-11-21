#include "main.h"
#include "power.h"

void set_bor(void)
{
__IO uint32_t BOROptionBytes = 0;
  /* Get BOR Option Bytes */
  BOROptionBytes = FLASH_OB_GetBOR();

  
  if((BOROptionBytes & 0x0F) != BOR_LEVEL) 
  {
    /* Unlocks the option bytes block access */
    FLASH_OB_Unlock();

    /* Clears the FLASH pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);

    /* Select the desired V(BOR) Level ---------------------------------------*/
    FLASH_OB_BORConfig(BOR_LEVEL); 

    /* Launch the option byte loading */
    FLASH_OB_Launch();  
		FLASH_OB_Lock();
  }
}
