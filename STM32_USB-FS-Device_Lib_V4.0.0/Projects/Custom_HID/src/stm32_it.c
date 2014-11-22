__IO uint8_t Send_Buffer[2];
extern __IO uint8_t PrevXferComplete;
extern uint32_t ADC_ConvertedValueX;
extern uint32_t ADC_ConvertedValueX_1;
extern __IO uint32_t TimingDelay;


void DMA1_Channel1_IRQHandler(void)
{  
  Send_Buffer[0] = 0x07;
  
  if((ADC_ConvertedValueX >>4) - (ADC_ConvertedValueX_1 >>4) > 4)
  {
    if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
    {
      Send_Buffer[1] = (uint8_t)(ADC_ConvertedValueX >>4);
      
      /* Write the descriptor through the endpoint */
      USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2);  
      SetEPTxValid(ENDP1);
      ADC_ConvertedValueX_1 = ADC_ConvertedValueX;
      PrevXferComplete = 0;
    }
  }
  
  DMA_ClearFlag(DMA1_FLAG_TC1);
}
