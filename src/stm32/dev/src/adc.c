#include "adc.h"

void initAdcAntenne() {
	/* Intitialisation de l'ADC pour l'antenne
	 * ADC1
	 * CHANNEL7
	 * PA7
	 */
	/* Initialize ADC1 on channel 0, this is pin PA7 */
	TM_ADC_Init(ADC1, ADC_Channel_7);
}

uint16_t getAdcAntenne() {
	return TM_ADC_Read(ADC1, ADC_Channel_7);
}
