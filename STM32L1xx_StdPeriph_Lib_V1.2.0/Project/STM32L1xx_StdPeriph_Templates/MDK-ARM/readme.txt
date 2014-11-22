/**
  @page mdkarm MDK-ARM Project Template for STM32L1xx Ultra Low Power devices
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    readme.txt
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    13-April-2012
  * @brief   This sub-directory contains all the user-modifiable files 
  *          needed to create a new project linked with the STM32L1xx  
  *          Standard Peripheral Library and working with RealView Microcontroller
  *          Development Kit(MDK-ARM) software toolchain.
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
 
 @par Directory contents
 
 - Project.uvproj/.uvopt: A pre-configured project file with the provided library structure
                          that produces an executable image with MDK-ARM.

Enabling "Options for Target 'Output' Browser Information" is useful for quick 
source files navigation but may slow the compilation time.
 
 
 @par How to use it ?
 
 - Open the Project.Uv2 project
 - In the build toolbar select the project config:
     - STM32L152-EVAL: to configure the project for STM32L Ultra Low Power 
                       Medium-density devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: USE_STDPERIPH_DRIVER, STM32L1XX_MD, USE_STM32L152_EVAL
     - STM32L152D-EVAL: to configure the project for STM32L Ultra Low Power 
                       High-density devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: USE_STDPERIPH_DRIVER, STM32L1XX_HD, USE_STM32L152D_EVAL
 - Rebuild all files: Project->Rebuild all target files
 - Load project image: Debug->Start/Stop Debug Session
 - Run program: Debug->Run (F5)

@note
- Ultra Low Power Medium-density devices are STM32L151xx and STM32L152xx 
  microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
- Ultra Low Power Medium-density Plus devices are STM32L151xx, STM32L152xx and 
  STM32L162xx microcontrollers where the Flash memory density is 256 Kbytes.
- Ultra Low Power High-density devices are STM32L151xx, STM32L152xx and STM32L162xx 
  microcontrollers where the Flash memory density is 384 Kbytes.
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


