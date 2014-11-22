/**
  @page AES_EncryptedDataExchangeInterrupt I2C secure Communication Boards Data Exchange using Interrupts example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    AES/AES_TwoBoards/EncryptedDataExchangeInterrupt/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    13-April-2012
  * @brief   Description of the AES encryption decryption and the I2C secure 
  *          Communication Boards Data Exchange using Interrupts example .
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
   @endverbatim

@par Example Description 

This is a typical example of how to use the AES software library to encrypt data 
in master board and decrypt it in slave board using I2C and interrupts.

- Hardware Description

To use this example, you need to load it on two STM32 boards (let's call them 
Board A and Board B) then connect these two boards through I2C lines and GND.
In the firmware example uncomment the dedicated line in the main.h file  to use
the I2C peripheral as STM32 Master device or as STM32 Slave .

@verbatim
*------------------------------------------------------------------------------*
|                BOARD A                                BOARD B                |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         |
|        |     __________     |                 |     __________     |         |
|        |    |I2C Master|____|_______SDA_______|____|I2C Slave |    |         |
|        |    |  Device  |____|_______SCL_______|____|  Device  |    |         |
|        |    |__________|    |                 |    |__________|    |         |
|        |  O LD1             |                 |  O LD1             |         |
|        |  O LD2     RESET   |                 |  O LD2     RESET   |         |
|        |  O LD3        _    |                 |  O LD3        _    |         |
|        |  O LD4       |_|   |                 |  O LD4       |_|   |         |
|        |                    |                 |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*
@endverbatim

- Software Description

This example requires two boards(Master and Slave)

  - Phase 1:
    In master board I2C peripheral is configured in Master Transmitter with Interrupt,
    where in Slave board I2C peripheral is configured in Slave Receiver with Interrupt.
    The master will encrypt the Data and send it to Slave according to the defined 
    buffer size.
   
  - Phase 2: 
    The slave device will decrypt received data
   
At the end of the encrypt of data, a check will be performed. In case of mismatch
between encrypted and Expected data, the result is displayed on LCD of master board

At the end of the data transfer, a data consistency check will be performed 
in master and slave sides. In case of mismatch between sent and received data,
all LED's are OFF. Otherwise LED2, LED3 and LED4 are ON. 

After receive the data and the decryption, a data consistency check will be performed. 
In case of mismatch between decrypted and original data,the result is displayed 
on LCD of slave board. 

Communication error are handled in master an slave sides, their occurrence will 
be signaled by puting LED3 and LED4 On.
  

The File "I2C/I2C_TwoBoards/DataExchangeInterrupt/main.h" allows different 
possibilities of configuration of this example:
  - Addressing mode : 7-bit or 10-bit
  - I2C speed: Fast mode or Standard mode

In both boards, the data transfers is managed using I2Cx_EV_IRQHandler 
and I2Cx_ER_IRQHandler in stm32l1xx_it.c file.

The SysTick is configured to generate interrupt each 10ms. A dedicated counter 
inside the SysTick ISR is used to toggle the LD1 each 100ms indicating that the 
firmware is running.

A defined communication timeout is insuring that the application will not remain 
stuck if the I2C Master communication is corrupted.
You can adjust this timeout through the USER_TIMEOUT  define inside main.h file
depending on CPU frequency and application conditions (interrupts routines, 
number of data to transfer, speed, CPU frequency...).


@par Directory contents 

  - AES/AES_TwoBoards/EncryptedDataExchangeInterrupt/stm32l1xx_conf.h    Library Configuration file
  - AES/AES_TwoBoards/EncryptedDataExchangeInterrupt/stm32l1xx_it.c      Interrupt handlers
  - AES/AES_TwoBoards/EncryptedDataExchangeInterrupt/stm32l1xx_it.h      Interrupt handlers header file
  - AES/AES_TwoBoards/EncryptedDataExchangeInterrupt/main.c              Main program
  - AES/AES_TwoBoards/EncryptedDataExchangeInterrupt/main.h              Main Header file
  - AES/AES_TwoBoards/EncryptedDataExchangeInterrupt/system_stm32l1xx.c  STM32L1xx system source file
  
@note The "system_stm32l1xx.c" is generated by an automatic clock configuration 
      system and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32L1xx_Clock_Configuration_V1.1.0.xls" 
      provided with the AN3309 package available on <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>
         
@par Hardware and Software environment
  
  - This example runs on STM32L162 Ultra Low Power High-Density and Medium-Density Plus Devices.

  - This example has been tested with STMicroelectronics STM32L152D-EVAL (STM32L1xx 
    Ultra Low Power High-Density) evaluation board and can be easily tailored 
    to any other supported device and development board.

  - STM32L152D-EVAL Set-up

    - Connect I2C1 Master SCL pin (PB.08) to I2C1 Slave SCL (PB.08)
    - Connect I2C1 Master SDA pin (PB.09) to I2C1 Slave SDA (PB.09)

   @note  
    - Pull-up resistors should be connected to I2C SCL and SDA Pins in case of 
      other boards use.
      Pull-up  resistors are already implemented on the STM32L152-EVAL evaluation 
      boards.  
        
@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32L1xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

@note
- Ultra Low Power Medium-density devices are STM32L151xx and STM32L152xx 
  microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
- Ultra Low Power Medium-density Plus devices are STM32L151xx, STM32L152xx and 
  STM32L162xx microcontrollers where the Flash memory density is 256 Kbytes.
- Ultra Low Power High-density devices are STM32L151xx, STM32L152xx and STM32L162xx 
  microcontrollers where the Flash memory density is 384 Kbytes.
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


