#ifndef ADC_H_
#define ADC_H_

#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "tm_stm32f4_adc.h"

#define ADC_RESOLUTION 4096
#define MAXIMUM_VOLT 3.0

void initAdcIR();

uint16_t getAdcIR1();
uint16_t getAdcIR2();
uint16_t getAdcIR3();

float convertReadValueToVolt(uint16_t readValue);

#endif
