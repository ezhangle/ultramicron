/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards/MasterTransmitterInterrupt/main.c
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "main.h"

/** @addtogroup STM32L1xx_StdPeriph_Examples
* @{
*/

/** @addtogroup MasterTransmitterInterrupt
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2C_InitTypeDef  I2C_InitStructure;
uint8_t TxBuffer[] = "I2C Interrupt Example: Communication between two I2C using Interrupt";
__IO uint8_t NumberOfByte = 0x00;

#if defined (I2C_SLAVE)
__IO uint8_t CmdReceived = 0x00;
__IO uint8_t Rx_Idx = 0x00;
uint8_t RxBuffer [RXBUFFERSIZE];
#endif /* I2C_SLAVE */

#if defined (I2C_MASTER)
__IO uint32_t TimeOut = 0x0;
__IO uint8_t CmdTransmitted = 0x00;
__IO JOYState_TypeDef PressedButton = JOY_NONE;
__IO uint8_t Tx_Idx = 0x00;
#endif /* I2C_MASTER */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void I2C_Config(void);
static void SysTickConfig(void);

#if defined (I2C_MASTER)
static void TimeOut_UserCallback(void);
static JOYState_TypeDef Read_Joystick(void);
#endif /* I2C_MASTER */

#if defined (I2C_SLAVE)
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength);
#endif /* I2C_SLAVE */

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
  this is done through SystemInit() function which is called from startup
  file (startup_stm32l1xx_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32l1xx.c file
  */
  
  /* I2C configuration ------------------------------------------------------*/
  I2C_Config();
  
  /* Initialize LEDs mounted on STM32L152-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
#if defined (I2C_MASTER)
  /* Initialize push-buttons mounted on STM32L152-EVAL board */
  STM_EVAL_PBInit(BUTTON_RIGHT, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_LEFT, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_UP, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_DOWN, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_SEL, BUTTON_MODE_GPIO);
#endif /* I2C_MASTER */
  
  /* SysTick configuration ---------------------------------------------------*/
  SysTickConfig();
  
  /*************************************Master Code******************************/
#if defined (I2C_MASTER)
  /* I2C De-initialize */
  I2C_DeInit(I2Cx);
  
  /*!< I2C Struct Initialize */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddress1 = 0xA0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  
#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  /*!< I2C Initialize */
  I2C_Init(I2Cx, &I2C_InitStructure);
  
  /* Enable Error Interrupt */
  I2C_ITConfig(I2Cx, I2C_IT_ERR , ENABLE);
  
  /* I2C ENABLE */
  I2C_Cmd(I2Cx, ENABLE);
  
  while (1)
  {
    CmdTransmitted = 0x00;
    NumberOfByte = 0x00;
    Tx_Idx = 0x00;
    /* Clear PressedButton by reading joystick */
    PressedButton = Read_Joystick();
    
    /* Waiting joystick pressed */
    while (PressedButton == JOY_NONE)
    {
      PressedButton = Read_Joystick();
    }
    
    /* I2C in Master Transmitter Mode-----------------------------------------*/    
    switch (PressedButton)
    {
      /* JOY_RIGHT button pressed */
      case JOY_RIGHT:
        NumberOfByte = CMD_RIGHT_SIZE;
        CmdTransmitted = CMD_RIGHT;
        break;
      /* JOY_LEFT button pressed */ 
      case JOY_LEFT:
        NumberOfByte = CMD_LEFT_SIZE;
        CmdTransmitted = CMD_LEFT;
        break;
      /* JOY_UP button pressed */ 
      case JOY_UP:
        NumberOfByte = CMD_UP_SIZE;
        CmdTransmitted = CMD_UP;
        break;
      /* JOY_DOWN button pressed */  
      case JOY_DOWN:
        NumberOfByte = CMD_DOWN_SIZE;
        CmdTransmitted = CMD_DOWN;
        break;
      /* JOY_SEL button pressed */ 
      case JOY_SEL:
        NumberOfByte = CMD_SEL_SIZE;
        CmdTransmitted = CMD_SEL;
        break;
      default:
        break;
    }
    
    if (CmdTransmitted != 0x00)
    {
      /* Enable Error and Buffer Interrupts */
      I2C_ITConfig(I2Cx, (I2C_IT_EVT | I2C_IT_BUF), ENABLE);
      /* Generate the Start condition */
      I2C_GenerateSTART(I2Cx, ENABLE);
      /* Data transfer is performed in the I2C interrupt routine */
      /* Wait until end of data transfer or time out */
       TimeOut = USER_TIMEOUT;
       while ((Tx_Idx < GetVar_NbrOfDataToTransfer())&&(TimeOut != 0x00))
       {}
       if(TimeOut == 0)
       {
         TimeOut_UserCallback();
       }
  
       TimeOut = USER_TIMEOUT;
       while ((I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))&&(TimeOut != 0x00))
       {}
       if(TimeOut == 0)
       {
         TimeOut_UserCallback();
       }
    }
  }
#endif /* I2C_MASTER */
  
  
  /**********************************Slave Code**********************************/
#if defined (I2C_SLAVE)
  
  I2C_DeInit(I2Cx);
  
  /* Initialize I2C peripheral */
  /*!< I2C Init */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddress1 = SLAVE_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  
#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  I2C_Init(I2Cx, &I2C_InitStructure);
  
  /* Enable Error Interrupt */
  I2C_ITConfig(I2Cx, (I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF), ENABLE);
  
  /* I2C ENABLE */
  I2C_Cmd(I2Cx, ENABLE);
  
  /* Infinite Loop */
  while (1)
  {
    CmdReceived = 0x00;
    NumberOfByte = 0x00;
    
    /* Clear the RxBuffer */
    Fill_Buffer(RxBuffer, RXBUFFERSIZE);
    
    while (CmdReceived == 0x00)
    {}
    
    /* Wait until end of data transfer */
    while (Rx_Idx < GetVar_NbrOfDataToReceive())
    {}
    
    /* I2C in Slave Receiver Mode-------------------------------------------------*/
    if (CmdReceived != 0x00)
    {  
      switch (Rx_Idx)
      {
        /* Right button pressed */
      case CMD_RIGHT_SIZE:
        if (Buffercmp(TxBuffer, RxBuffer, CMD_RIGHT_SIZE) == PASSED)
        {
          /* Turn ON LED2 and LED3 */
          STM_EVAL_LEDOn(LED2);
          STM_EVAL_LEDOn(LED3);
          /* Turn all other LEDs off */
          STM_EVAL_LEDOff(LED4);
        }
        break;
        /* Left button pressed*/
      case CMD_LEFT_SIZE:
        if (Buffercmp(TxBuffer, RxBuffer, CMD_LEFT_SIZE) == PASSED)
        {
          /* Turn ON LED4 */
          STM_EVAL_LEDOn(LED4);
          /* Turn all other LEDs off */
          STM_EVAL_LEDOff(LED2);
          STM_EVAL_LEDOff(LED3);
        }
        break;
        /* Up button pressed */
      case CMD_UP_SIZE:
        if (Buffercmp(TxBuffer, RxBuffer, CMD_UP_SIZE) == PASSED)
        {
          /* Turn ON LED2 */
          STM_EVAL_LEDOn(LED2);
          /* Turn all other LEDs off */
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED4);
        }
        break;
        /* Down button pressed */
      case CMD_DOWN_SIZE:
        if (Buffercmp(TxBuffer, RxBuffer, CMD_DOWN_SIZE) == PASSED)
        {
          /* Turn ON LED3 */
          STM_EVAL_LEDOn(LED3);
          /* Turn all other LEDs off */
          STM_EVAL_LEDOff(LED2);
          STM_EVAL_LEDOff(LED4);
        }
        break;
        /* Sel button pressed */
      case CMD_SEL_SIZE:
        if (Buffercmp(TxBuffer, RxBuffer, CMD_SEL_SIZE) == PASSED) 
        {
          /* Turn ON all LEDs */
          STM_EVAL_LEDOn(LED2);
          STM_EVAL_LEDOn(LED3);
          STM_EVAL_LEDOn(LED4);
        }
        break;
      default:
        break;
      }
    }
  }
#endif /* I2C_SLAVE */
}

/**
  * @brief  Enables the I2C Clock and configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void I2C_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  /*!< I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE);
  
  /*!< SDA GPIO clock enable */
  RCC_AHBPeriphClockCmd(I2Cx_SDA_GPIO_CLK, ENABLE);
  
  /*!< SCL GPIO clock enable */
  RCC_AHBPeriphClockCmd(I2Cx_SCL_GPIO_CLK, ENABLE);

  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_SOURCE, GPIO_AF_I2C1);
  
  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_SOURCE, GPIO_AF_I2C1);
  
  /*!< Configure I2C SCL pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure I2C SDA pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SDA_PIN;
  GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);
   
  /* Configure the Priority Group to 1 bit */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Configure the I2C event priority */
  NVIC_InitStructure.NVIC_IRQChannel = I2Cx_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configure I2C error interrupt to have the higher priority */
  NVIC_InitStructure.NVIC_IRQChannel = I2Cx_ER_IRQn;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure a SysTick Base time to 10 ms.
  * @param  None
  * @retval None
  */
static void SysTickConfig(void)
{
  /* Setup SysTick Timer for 10ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))
  {
    /* Capture error */
    while (1);
  }
  
  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

#if defined (I2C_MASTER)
/**
  * @brief  Returns NbrOfDataToTransfer value.
  * @param  None
  * @retval Tx Buffer Size (NbrOfDataToTransfer).
  */
uint8_t GetVar_NbrOfDataToTransfer(void)
{
  return NumberOfByte;
}

/**
* @brief  Reads key from evaluationboard.
* @param  None
* @retval Return JOY_RIGHT, JOY_LEFT, JOY_SEL, JOY_UP, JOY_DOWN or JOY_NONE
*/
static JOYState_TypeDef Read_Joystick(void)
{
  /* "JOY_RIGHT" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_RIGHT))
  {
    while (STM_EVAL_PBGetState(BUTTON_RIGHT) == RESET)
    {}
    return JOY_RIGHT;
  }
  /* "JOY_LEFT" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_LEFT))
  {
    while (STM_EVAL_PBGetState(BUTTON_LEFT) == RESET)
    {}
    return JOY_LEFT;
  }
  /* "JOY_UP" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_UP))
  {
    while (STM_EVAL_PBGetState(BUTTON_UP) == RESET)
    {}
    return JOY_UP;
  }
  /* "JOY_DOWN" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_DOWN))
  {
    while (STM_EVAL_PBGetState(BUTTON_DOWN) == RESET)
    {}
    return JOY_DOWN;
  }
  /* "JOY_SEL" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_SEL))
  {
    while (STM_EVAL_PBGetState(BUTTON_SEL) == RESET)
    {}
    return JOY_SEL;
  }
  /* No key is pressed */
  else
  {
    return JOY_NONE;
  }
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
static void TimeOut_UserCallback(void)
{
  /* User can add his own implementation to manage TimeOut Communication failure */
  /* Block communication and all processes */
  while (1)
  {   
  }
}
#endif /* I2C_MASTER */

#if defined (I2C_SLAVE)
/**
  * @brief  Returns NbrOfDataToReceive value.
  * @param  None
  * @retval Rx Buffer Size (NumberOfByte).
  */
uint8_t GetVar_NbrOfDataToReceive(void)
{
  switch (CmdReceived)
  {
  case CMD_RIGHT:
    NumberOfByte = CMD_RIGHT_SIZE;
    break;
  case CMD_LEFT:
    NumberOfByte =  CMD_LEFT_SIZE;
    break;
  case CMD_UP:
    NumberOfByte = CMD_UP_SIZE;
    break;
  case CMD_DOWN:
    NumberOfByte = CMD_DOWN_SIZE;
    break;
  case CMD_SEL:
    NumberOfByte = CMD_SEL_SIZE;
    break;
  default:
    break;
  }
  return NumberOfByte;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }
  
  return PASSED;
}

/**
  * @brief  Fills buffer.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @retval None
  */
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength)
{
  uint16_t index = 0;
  
  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = 0x00;
  }
}
#endif /* I2C_SLAVE */ 

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
