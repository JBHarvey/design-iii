int input = A0;
int led = 13;
int outputPin = 53;
int pwm=3;
int val;
#define BUFFER_SIZE  5000
uint16_t buf[BUFFER_SIZE];
unsigned long startTime;
unsigned long elapsedTime;
uint32_t freq;

void setup()
{
  pinMode(input,INPUT);
  pinMode(led,OUTPUT);
  pinMode(pwm,OUTPUT);
  // Setup all registers
  pmc_enable_periph_clk (ID_ADC);
  adc_init (ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
  adc_disable_interrupt (ADC, 0xFFFFFFFF);
  adc_set_resolution (ADC, ADC_10_BITS);
  adc_configure_power_save (ADC, ADC_MR_SLEEP_NORMAL, ADC_MR_FWUP_OFF);
  adc_configure_timing (ADC, 1, ADC_SETTLING_TIME_3, 1);
  adc_set_bias_current (ADC, 1);
  adc_disable_tag (ADC);
  adc_disable_ts (ADC);
  adc_stop_sequencer (ADC);
  adc_disable_channel_differential_input (ADC, ADC_CHANNEL_7);
  adc_disable_all_channel (ADC);
  adc_enable_channel (ADC, ADC_CHANNEL_7);
  adc_configure_trigger (ADC, ADC_TRIG_SW, 1);
  Serial.begin(115200);
  adc_start( ADC );

}

void loop() 
{
  static uint8_t pwmCnt=10;
  analogWrite(pwm,pwmCnt);
  // configure Peripheral DMA
  PDC_ADC->PERIPH_RPR = (uint32_t) buf; // address of buffer
  PDC_ADC->PERIPH_RCR = BUFFER_SIZE; 
  PDC_ADC->PERIPH_PTCR = PERIPH_PTCR_RXTEN; // enable receive
  digitalWrite(led,HIGH);
  startTime = millis();
  while((adc_get_status(ADC) & ADC_ISR_ENDRX) == 0)
  {};
  elapsedTime = millis() - startTime;
  freq =  (uint32_t)BUFFER_SIZE/(elapsedTime/1000);  
  digitalWrite(led,LOW);
  Serial.write((char*) &freq, 4);
  Serial.write((char*)buf, 2*5000);
  delay(500);
}

void plot(uint16_t data1)
{
  uint16_t pktSize = sizeof(uint16_t);
  uint16_t buffer[pktSize*3];
  buffer[0] = 0xCDAB;             //SimPlot packet header. Indicates start of data packet
  buffer[1] = pktSize;      //Size of data in bytes. Does not include the header and size fields
  buffer[2] = data1;
  pktSize += 2*sizeof(uint16_t); //Header bytes + size field bytes + data
  Serial1.write((uint8_t * )buffer, pktSize);
}
