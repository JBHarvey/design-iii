#ifndef ADC_H_
#define ADC_H_

#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "tm_stm32f4_adc.h"

void initAdcAntenne();

uint16_t getAdcAntenne();

#endif
