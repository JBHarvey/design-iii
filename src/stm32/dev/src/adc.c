#include "adc.h"

void initAdcIR() {
	/* Intitialisation de l'ADC pour l'antenne
	 * ADC1
	 * CHANNEL7
	 * PA7
	 */
	/* Initialize ADC1 on channel 0, this is pin PA7 */
	TM_ADC_Init(ADC1, ADC_Channel_1);
	TM_ADC_Init(ADC2, ADC_Channel_2);
	TM_ADC_Init(ADC3, ADC_Channel_3);
}

uint16_t getAdcIR1() {
	return TM_ADC_Read(ADC1, ADC_Channel_1);
}

uint16_t getAdcIR2() {
	return TM_ADC_Read(ADC2, ADC_Channel_2);
}

uint16_t getAdcIR3() {
	return TM_ADC_Read(ADC3, ADC_Channel_3);
}
