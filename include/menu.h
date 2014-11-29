#ifndef __menu_user_H
#define __menu_user_H
#include <STM32L1xx.h>

#define max_string_len 16   // максимальная длинна строки
#define max_string_count 8  // максимальное количество строк
#define max_struct_index 10 // максимальное количество пунктов меню 
#define start_offset 2      // на сколько строк сместить вниз меню

typedef struct
{
   int8_t Hide;
   char Text[17];
   char Param_is_0[10];
   char Param_is_1[10];
   char Param_is_N[10];
   uint32_t *Parameter_value;
   uint32_t Min_limit;
   uint32_t Max_limit;
   uint32_t Value_default;
   void( *Plus_reaction) (uint32_t *);
   void( *Minus_reaction)(uint32_t *);
   
}  MenuItem;

void main_screen(void);
void menu_screen(void);
void stat_screen(void);

extern MenuItem Menu_list[];

#endif
