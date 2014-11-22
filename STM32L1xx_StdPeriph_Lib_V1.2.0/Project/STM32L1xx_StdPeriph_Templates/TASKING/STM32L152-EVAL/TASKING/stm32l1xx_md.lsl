////////////////////////////////////////////////////////////////////////////
//
//  File        :  stm32l1xx.lsl
//
//  Version     :  @(#)stm32l1xx.lsl    1.5  11/06/30
//
//  Description :  LSL file for the STMicroelectronics STM32L1xx
//
//  Copyright 2010-2011 Altium BV
//  
//  Macros specific to control this LSL file
//
//  __MEMORY            Define this macro to suppress definition of on-chip 
//                      memory. Must be defined when you want to define on-chip
//                      in your project's LSL file.
//  __FLASH_SIZE        Specifies the size of flash memory to be allocated
//  __SRAM_SIZE         Specifies the size of the SRAM memory to be allocated
//  __EEPROM_SIZE       Specifies the size of the EEPROM memory to be allocated
//  __NO_DEFAULT_AUTO_VECTORS
//                      When enabled the interrupt vector table will not be 
//                      generated
//  __VECTOR_TABLE_RAM_COPY
//                      Define this macro to enable copying the vector table 
//                      at startup from ROM to RAM.
//  __VECTOR_TABLE_ROM_ADDR
//                      Specify the vector table address in ROM
//  __VECTOR_TABLE_RAM_ADDR
//                      Specify the vector table address in RAM when the the
//                      it is copied from ROM to RAM (__VECTOR_TABLE_RAM_COPY)
//
//  See arm_arch.lsl for more available macros.
//
//  Notes:
//  In the STM32L1xx, 3 different boot modes can be selected
//  - User Flash memory is selected as boot space
//  - SystemMemory is selected as boot space
//  - Embedded SRAM is selected as boot space
//
//  This aliases the physical memory associated with each boot mode to Block
//  000 (0x00000000 boot memory). Even when aliased in the boot memory space,
//  the related memory (Flash memory or SRAM) is still accessible at its
//  original memory space.
//
////////////////////////////////////////////////////////////////////////////

#ifndef __NO_DEFAULT_AUTO_VECTORS
// Suppress the vectors as defined arm_arch.lsl, because we define our 
// own vectors for CMIS
#define __CMSIS_VECTORS                 1
#define __NO_DEFAULT_AUTO_VECTORS       1
#define __NR_OF_VECTORS                 61
#endif

#ifndef __VECTOR_TABLE_ROM_ADDR
#  define __VECTOR_TABLE_ROM_ADDR       0x08000000
#endif

#ifndef __VECTOR_TABLE_RAM_ADDR
#  define __VECTOR_TABLE_RAM_ADDR       0x00000000
#endif


#ifndef __STACK
#  define __STACK                       4k
#endif

#ifndef __HEAP
#  define __HEAP                        2k
#endif

#include <arm_arch.lsl>

//
//  If no memory is defined yet use the following memory settings
//
#ifndef __MEMORY

// Specify default size for Flash and SRAM
#ifndef __FLASH_SIZE
#  define __FLASH_SIZE  128k
#endif
#ifndef __SRAM_SIZE
#  define __SRAM_SIZE   16k
#endif
#ifndef __EEPROM_SIZE
#  define __EEPROM_SIZE 4k
#endif

memory STM32L1xx_Flash
{
    mau = 8;
    type = rom;
    size = __FLASH_SIZE;
    map ( size = __FLASH_SIZE, dest_offset=0x08000000, dest=bus:ARM:local_bus);
}

memory STM32L1xx_EEPROM
{
    mau = 8;
    type = reserved rom;
    size = __EEPROM_SIZE;
    map ( size = __EEPROM_SIZE, dest_offset=0x08080000, dest=bus:ARM:local_bus);
}

memory STM32L1xx_SRAM
{
    mau = 8;
    type = ram;
    size = __SRAM_SIZE;
    map ( size = __SRAM_SIZE, dest_offset=0x20000000, dest=bus:ARM:local_bus);
}

#endif /* __MEMORY */


//
// Define the vector table for CMSIS
//
#ifdef __CMSIS_VECTORS
section_setup ::linear
{
                // vector table with handler addresses
                vector_table "vector_table" ( vector_size = 4, size = __NR_OF_VECTORS, run_addr = __VECTOR_TABLE_RUN_ADDR,
                                              template = ".text.handler_address",
                                              template_symbol = "_lc_vector_handler",
                                              vector_prefix = "_vector_",
                                              __VECTOR_TABLE_COPY_ATTRIBUTE 
                                              fill = loop,
                                              no_inline
                                            )
                {
                        vector ( id = 0, fill = "_lc_ub_stack" );                               // Top of Stack
                        vector ( id = 1, fill = "Reset_Handler" );                              // Reset Handler
                        vector ( id = 2, optional, fill = "NMI_Handler" );                      // NMI Handler
                        vector ( id = 3, optional, fill = "HardFault_Handler" );                // Hard Fault Handler
                        vector ( id = 4, optional, fill = "MemManage_Handler" );                // MPU Fault Handler
                        vector ( id = 5, optional, fill = "BusFault_Handler" );                 // Bus Fault Handler
                        vector ( id = 6, optional, fill = "UsageFault_Handler" );               // Usage Fault Handler
                        vector ( id = 11, optional, fill = "SVC_Handler" );                     // SVCall Handler
                        vector ( id = 12, optional, fill = "DebugMon_Handler" );                // Debug Monitor Handler
                        vector ( id = 14, optional, fill = "PendSV_Handler" );                  // PendSV Handler
                        vector ( id = 15, optional, fill = "SysTick_Handler" );                 // SysTick Handler

                        // External Interrupts :
                        vector ( id = 16, optional, fill = "WWDG_IRQHandler" );                 // Window Watchdog
                        vector ( id = 17, optional, fill = "PVD_IRQHandler" );                  // PVD through EXTI Line detect
                        vector ( id = 18, optional, fill = "TAMPER_STAMP_IRQHandler" );         // Tamper and Time Stamp
                        vector ( id = 19, optional, fill = "RTC_WKUP_IRQHandler" );             // RTC Wakeup
                        vector ( id = 20, optional, fill = "FLASH_IRQHandler" );                // FLASH
                        vector ( id = 21, optional, fill = "RCC_IRQHandler" );                  // RCC
                        vector ( id = 22, optional, fill = "EXTI0_IRQHandler" );                // EXTI Line 0
                        vector ( id = 23, optional, fill = "EXTI1_IRQHandler" );                // EXTI Line 1
                        vector ( id = 24, optional, fill = "EXTI2_IRQHandler" );                // EXTI Line 2
                        vector ( id = 25, optional, fill = "EXTI3_IRQHandler" );                // EXTI Line 3
                        vector ( id = 26, optional, fill = "EXTI4_IRQHandler" );                // EXTI Line 4
                        vector ( id = 27, optional, fill = "DMA1_Channel1_IRQHandler" );        // DMA1 Channel 1
                        vector ( id = 28, optional, fill = "DMA1_Channel2_IRQHandler" );        // DMA1 Channel 2
                        vector ( id = 29, optional, fill = "DMA1_Channel3_IRQHandler" );        // DMA1 Channel 3
                        vector ( id = 30, optional, fill = "DMA1_Channel4_IRQHandler" );        // DMA1 Channel 4
                        vector ( id = 31, optional, fill = "DMA1_Channel5_IRQHandler" );        // DMA1 Channel 5
                        vector ( id = 32, optional, fill = "DMA1_Channel6_IRQHandler" );        // DMA1 Channel 6
                        vector ( id = 33, optional, fill = "DMA1_Channel7_IRQHandler" );        // DMA1 Channel 7
                        vector ( id = 34, optional, fill = "ADC1_IRQHandler" );                 // ADC1
                        vector ( id = 35, optional, fill = "USB_HP_IRQHandler" );               // USB High Priority
                        vector ( id = 36, optional, fill = "USB_LP_IRQHandler" );               // USB Low  Priority
                        vector ( id = 37, optional, fill = "DAC_IRQHandler" );                  // DAC
                        vector ( id = 38, optional, fill = "COMP_IRQHandler" );                 // COMP through EXTI Line
                        vector ( id = 39, optional, fill = "EXTI9_5_IRQHandler" );              // EXTI Line 9..5
                        vector ( id = 40, optional, fill = "LCD_IRQHandler" );                  // LCD
                        vector ( id = 41, optional, fill = "TIM9_IRQHandler" );                 // TIM9
                        vector ( id = 42, optional, fill = "TIM10_IRQHandler" );                // TIM10
                        vector ( id = 43, optional, fill = "TIM11_IRQHandler" );                // TIM11
                        vector ( id = 44, optional, fill = "TIM2_IRQHandler" );                 // TIM2
                        vector ( id = 45, optional, fill = "TIM3_IRQHandler" );                 // TIM3
                        vector ( id = 46, optional, fill = "TIM4_IRQHandler" );                 // TIM4
                        vector ( id = 47, optional, fill = "I2C1_EV_IRQHandler" );              // I2C1 Event
                        vector ( id = 48, optional, fill = "I2C1_ER_IRQHandler" );              // I2C1 Error
                        vector ( id = 49, optional, fill = "I2C2_EV_IRQHandler" );              // I2C2 Event
                        vector ( id = 50, optional, fill = "I2C2_ER_IRQHandler" );              // I2C2 Error
                        vector ( id = 51, optional, fill = "SPI1_IRQHandler" );                 // SPI1
                        vector ( id = 52, optional, fill = "SPI2_IRQHandler" );                 // SPI2
                        vector ( id = 53, optional, fill = "USART1_IRQHandler" );               // USART1
                        vector ( id = 54, optional, fill = "USART2_IRQHandler" );               // USART2
                        vector ( id = 55, optional, fill = "USART3_IRQHandler" );               // USART3
                        vector ( id = 56, optional, fill = "EXTI15_10_IRQHandler" );            // EXTI Line 15..10
                        vector ( id = 57, optional, fill = "RTC_Alarm_IRQHandler" );            // RTC Alarm through EXTI Line
                        vector ( id = 58, optional, fill = "USB_FS_WKUP_IRQHandler" );          // USB FS Wakeup from suspend
                        vector ( id = 59, optional, fill = "TIM6_IRQHandler" );                 // TIM6
                        vector ( id = 60, optional, fill = "TIM7_IRQHandler" );                 // TIM7
                }
}
#endif /* __CMSIS_VECTORS */
