#ifndef __clock_user_H
#define __clock_user_H
#include "STM32L1xx.h"                  // Device header

void set_msi(FunctionalState sleep);
void set_pll_for_usb(void);

#endif
