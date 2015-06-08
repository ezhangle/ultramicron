#include "dac.h"
#include "main.h"

void dac_init()
{
	DAC_InitTypeDef  DAC_InitStructure;
  //--------------------------------------------------------------------
  //Конфигурируем систему детектирования выброса
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;

  DAC_Init(DAC_Channel_2, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_2, ENABLE);
  DAC_SetChannel2Data(DAC_Align_12b_R, ADCData.DAC_voltage_raw);   /* Set DAC Channel2 DHR register: DAC_OUT2 = (1.224 * 128) / 256 = 0.612 V */

}

void dac_off()
{
  DAC_Cmd(DAC_Channel_2, DISABLE);
	//DAC_DeInit();
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, DISABLE);
}

void dac_on()
{
	//dac_init();
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);
}


void dac_reload()
{
#ifdef version_401
  if(Settings.Geiger_voltage>200)
	{
		// 200  - Падение на диоде мВ
		// 34   - Коэфицент резистивного делителя (1к+33к)/1к  (! обратный расчет 1/x)
		// 45   - Коэфицент конденсаторного делителя 1000-(1000*(10нФ/(10нФ+470пФ))) (оптимизированно для вычисления)
		// 11   - Коэфицент трансформации (10+1:1)
		// 1000 - приведение к милливольтам, для минимизации ошибки целочисленного расчета
		ADCData.DAC_voltage_raw=(((Settings.Geiger_voltage*(1000-45))/11)-200)/34; 
	} else {
		ADCData.DAC_voltage_raw=1; //Заплатка
	}
#else
	ADCData.DAC_voltage_raw=((Settings.Geiger_voltage*1000)/30/11); // напряжение датчика/Ктансформации/коэффицент резистивного делителя
#endif

	ADCData.DAC_voltage_raw=(ADCData.DAC_voltage_raw*1000)/ADCData.Calibration_bit_voltage; // коррекция значения по напряжению опоры

	DAC_SetChannel2Data(DAC_Align_12b_R, ADCData.DAC_voltage_raw);   /* Set DAC Channel2 DHR register: DAC_OUT2 = (1.224 * 128) / 256 = 0.612 V */
}
