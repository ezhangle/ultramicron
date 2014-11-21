#include "main.h"
#include "delay.h"
#include "adc.h"
#include "timers.h"

#define  ADC_CCR_ADCPRE                      ((uint32_t)0x00030000)        /*!< ADC prescaler*/
#define  ADC_CCR_ADCPRE_0                    ((uint32_t)0x00010000)        /*!< Bit 0 */
#define  ADC_CCR_ADCPRE_1                    ((uint32_t)0x00020000)        /*!< Bit 1 */ 

//************************************************************************************************************
void adc_check_event(void)
{
	uint32_t pump_period;
  if(DataUpdate.Need_batt_voltage_update)
  {
    adc_init();
    adc_calibration();
    ADC_Batt_Read();

    ADC_Cmd(ADC1, DISABLE); // ВЫКЛ!
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
    RCC_HSICmd(DISABLE); // Выключаем HSI
		ADCData.Batt_voltage=(ADCData.Calibration_bit_voltage*ADCData.Batt_voltage_raw/1000)*2;
		pump_period=(176*Settings.Pump_Energy)/ADCData.Batt_voltage;
		if((pump_period>16) & (Settings.LSI_freq==0)) // не привышать критический уровень для верии 3.*
		{
			TIM_SetCompare1(TIM9,16); // изменение энергии накачки
		} else TIM_SetCompare1(TIM9,pump_period); // изменение энергии накачки
    DataUpdate.Need_batt_voltage_update=DISABLE;
 }
// -----------
}


//************************************************************************************************************
void adc_calibration(void)
{

  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_4Cycles); // Конфигурирование канала

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze); // Задержка до момента чтения данных из АЦП

  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE); // отключение питания АЦП в интервалах Idle и Delay
  
  ADC_Cmd(ADC1, ENABLE); // ВКЛ!

  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);  // Тупо ждем запуска АЦП
  ADC_SoftwareStartConv(ADC1); // Стартуем преобразование

  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // Тупо ждем завершения преобразования
		
  ADCData.Calibration_bit_voltage=(1220000/ADC_GetConversionValue(ADC1)); // битовое значение соотв. напряжению референса 1.22в, из него вычисляем скольким микровольтам соответствует 1 бит.
  ADCData.Power_voltage=((ADCData.Calibration_bit_voltage * 4095)/1000);

	// Расчет напряжения компаратора
  ADCData.DAC_voltage_raw=((Settings.Geiger_voltage*1000)/30/11);
  ADCData.DAC_voltage_raw=(ADCData.DAC_voltage_raw*1000)/ADCData.Calibration_bit_voltage;
	DAC_SetChannel2Data(DAC_Align_12b_R, ADCData.DAC_voltage_raw);   /* Set DAC Channel2 DHR register: DAC_OUT2 = (1.224 * 128) / 256 = 0.612 V */
}

//************************************************************************************************************
void adc_init(void)
{
  ADC_InitTypeDef ADC_InitStructure;
	uint32_t ccr = ADC->CCR;
	
	ADC_StructInit(&ADC_InitStructure);
	
  ccr &= ~ADC_CCR_ADCPRE;
  ccr |= ADC_CCR_ADCPRE_0;
  ADC->CCR = ccr; // устанавливаем скорость перобразования АЦП 500ksps для режима пониженного питания

  RCC_HSICmd(ENABLE); // Включаем HSI

  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);   // Ждем пока запустатися HSI

	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // Разрешаем тактирование ADC
  
  ADC_BankSelection(ADC1, ADC_Bank_B);
    
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // Разрешение АЦП 12 бит.
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;          // Режим сканирования выключен
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;    // Непрерывное преобразование - выкл.
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // период срабатывания старта конверсии
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // Орентация битов результата
  ADC_InitStructure.ADC_NbrOfConversion = 1;		 // Число преобразований
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze); // Задержка до момента чтения данных из АЦП

  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE); // отключение питания АЦП в интервалах Idle и Delay
  
  ADC_Cmd(ADC1, ENABLE); // ВКЛ!

  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)  // Тупо ждем запуска АЦП
  {
  }
}

//************************************************************************************************************
void ADC_Batt_Read (void)
{
//  uint32_t avg=0,i;
  
  GPIO_InitTypeDef   GPIO_InitStructure;

  // Ножка изиерения напряжения АКБ
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;           // Ножка
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;         // Аналоговый режим
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;     // Без подтяжки
  GPIO_Init(GPIOB, &GPIO_InitStructure);  // Загружаем конфигурацию

  // ===============================================================================================  
  //Подключение токосемной цепочки
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;           // Ножка
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  // Загружаем конфигурацию
	ADC_RegularChannelConfig(ADC1, ADC_Channel_20, 1, ADC_SampleTime_4Cycles); // Конфигурирование канала

  GPIO_ResetBits(GPIOB,GPIO_Pin_15);// Подключаем токосемник
  ADC_SoftwareStartConv(ADC1); // Стартуем преобразование
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // Тупо ждем завершения преобразования
  ADCData.Batt_voltage_raw=ADC_GetConversionValue(ADC1);
  // ===============================================================================================  
  // Отключаем токосемную цепь
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB,GPIO_InitStructure.GPIO_Pin);// Подключаем токосемник
}
