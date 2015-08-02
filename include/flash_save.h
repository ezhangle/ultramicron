#ifndef __flashsave_user_H
#define __flashsave_user_H
                       
void full_erase_flash(void); // Erase full dataflash
void page_erase_flash(uint32_t); // Erase 16 elements
void flash_write_page(uint32_t); // Write 32 element of massive ram_Doze_massive and ram_max_fon_massive

#endif
