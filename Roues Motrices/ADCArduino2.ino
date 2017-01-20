
#define BUFFER_SIZE 5000
volatile int bufn,obufn;
uint16_t buf[4][BUFFER_SIZE];   // 4 buffers of 256 readings

int input = A0;
int led = 13;
int pwm=3;
int val;

unsigned long startTime;
unsigned long elapsedTime;
uint32_t freq;

void ADC_Handler(){     // move DMA pointers to next buffer
  int f=ADC->ADC_ISR;
  if (f&(1<<27)){
   bufn=(bufn+1)&3;
   ADC->ADC_RNPR=(uint32_t)buf[bufn];
   ADC->ADC_RNCR=BUFFER_SIZE;
   elapsedTime = millis() - startTime;
   freq =  5;//(uint32_t)BUFFER_SIZE/(elapsedTime/1000); 
   startTime = millis();
  } 
}

void setup(){
  // set les pins
  pinMode(input,INPUT);
  pinMode(led,OUTPUT);
  pinMode(pwm,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  pmc_enable_periph_clk(ID_ADC);
  adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
  ADC->ADC_MR |=0x80; // free running

  ADC->ADC_CHER=0x80; 

  NVIC_EnableIRQ(ADC_IRQn);
  ADC->ADC_IDR=~(1<<27);
  ADC->ADC_IER=1<<27;
  ADC->ADC_RPR=(uint32_t)buf[0];   // DMA buffer actuel, sert juste
                                  // au début.
  ADC->ADC_RCR=BUFFER_SIZE;
  ADC->ADC_RNPR=(uint32_t)buf[1]; // prochain DMA buffer
  ADC->ADC_RNCR=BUFFER_SIZE;
  bufn=obufn=1;
  ADC->ADC_PTCR=1; // active le mode recevoir de l'ADC.
  ADC->ADC_CR=2; // start le ADC

  static uint8_t pwmCnt=10;
  analogWrite(pwm,pwmCnt);
  startTime = millis();
}

void loop(){
  while(obufn==bufn); // wait for buffer to be full
  Serial.write((char*) &freq, 4);
  Serial.write((uint8_t *)buf[obufn],2* BUFFER_SIZE);
  obufn=(obufn+1)&3;    
}
