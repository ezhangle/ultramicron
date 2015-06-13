#include "comp.h"
#include "main.h"


void comp_off()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_StructInit(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line22);
	
  COMP_DeInit();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, DISABLE);
	
}

void comp_on()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, ENABLE);
	
	COMP->CSR = (uint32_t)0x00FC1000; // COMP_InvertingInput_DAC2 + COMP_OutputSelect_None + COMP_Speed_Fast
	
	EXTI_StructInit(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void comp_init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	COMP_InitTypeDef COMP_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_RIIOSwitchConfig(RI_IOSwitch_GR6_1, ENABLE);
	
  COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_DAC2;
  COMP_InitStructure.COMP_OutputSelect = COMP_OutputSelect_None;
  COMP_InitStructure.COMP_Speed = COMP_Speed_Fast;
  COMP_Init(&COMP_InitStructure);
	
	EXTI_StructInit(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


	while(!(COMP->CSR  & COMP_CSR_INSEL)); // ждем пока не включиться компаратор
}

