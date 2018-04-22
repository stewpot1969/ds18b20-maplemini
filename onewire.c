#ifndef ONEWIRE_C
#define ONEWIRE_C


uint8_t ds_scratchpad[8];

void ds_init(void);
void ds_reset(void);
void ds_delay(int us);
void ds_writebit(int bit);
int ds_readbit(void);
uint8_t ds_readbyte(void);
void ds_writebyte(int x);
void ds_read_scratchpad(void);
void ds_write_scratchpad(void);
void ds_read_temp(void);

/* Set these three to define which timer is used.
  Valid values are TIM2-TIM5 */

#define DS_SIMPLETIMER TIM3
#define DS_RCC_TIMER RCC_TIM3
#define DS_RST_TIMER RST_TIM3


/* Set these to define which I/O pin to use */
#define DS_RCC_GPIO RCC_GPIOB
#define DS_GPIO GPIOB
#define DS_GPIO_PIN GPIO14
#define DS_BSRR GPIOB_BSRR
#define DS_BIT_HIGH 0x00004000
#define DS_BIT_LOW 0x40000000
#define DS_INPUT GPIOB_IDR
#define DS_INPUT_BIT 14

/* Set these to define which I/O pin is used. */

void ds_init(void) {
  
  /* Enable GPIO port */
  rcc_periph_clock_enable(DS_RCC_GPIO);
  
  /* Configure GPIO pin */
  gpio_set_mode(DS_GPIO,
    GPIO_MODE_OUTPUT_2_MHZ,
    GPIO_CNF_OUTPUT_PUSHPULL,
    DS_GPIO_PIN);

	/* Reset SIMPLETIMER peripheral to defaults. */
	rcc_periph_clock_enable(DS_RCC_TIMER);
	
	rcc_periph_reset_pulse(DS_RST_TIMER);
	TIM_CR1(DS_SIMPLETIMER) = TIM_CR1_CKD_CK_INT | 0x0010 | TIM_CR1_OPM | TIM_CR1_ARPE;
	TIM_PSC(DS_SIMPLETIMER) = 7;
  TIM_EGR(DS_SIMPLETIMER) = TIM_EGR_UG;

}

void ds_reset(void) {

  DS_BSRR=DS_BIT_LOW;
  ds_delay(520);
  DS_BSRR=DS_BIT_HIGH;
  ds_delay(520);
}

  
void ds_delay(int us) {

  TIM_CNT(DS_SIMPLETIMER) = us;
  TIM_CR1(DS_SIMPLETIMER) |= TIM_CR1_CEN;
  while ( TIM_CR1(DS_SIMPLETIMER) & TIM_CR1_CEN );
    
}

void ds_writebit(int bit) {

  int wait_after;
  /* Choose low time */
  if (bit) {
    TIM_CNT(DS_SIMPLETIMER)=2;
    wait_after=98;
  }
  else {
    TIM_CNT(DS_SIMPLETIMER)=110;
    wait_after=40;
  }
  
  /* Start the timer */
  TIM_CR1(DS_SIMPLETIMER) |= TIM_CR1_CEN;
  
  /* Set the bus low */
  DS_BSRR=DS_BIT_LOW;
  
  /* Wait an appropriate amount of time */
  while ( TIM_CR1(DS_SIMPLETIMER) & TIM_CR1_CEN );
  
  /* Set the bus high */
  DS_BSRR=DS_BIT_HIGH;
  
  ds_delay(wait_after);

}

int ds_readbit(void) {

  int bit=0;
  
  /* 5uS low */
  TIM_CNT(DS_SIMPLETIMER)=1;
  TIM_CR1(DS_SIMPLETIMER) |= TIM_CR1_CEN;
  DS_BSRR=DS_BIT_LOW;
  while ( TIM_CR1(DS_SIMPLETIMER) & TIM_CR1_CEN );
  DS_BSRR=DS_BIT_HIGH;
  /* wait 10uS */
  TIM_CNT(DS_SIMPLETIMER)=6;
  TIM_CR1(DS_SIMPLETIMER) |= TIM_CR1_CEN;
  
  /* check the state of the bus */
  if (DS_INPUT & (1 << DS_INPUT_BIT)) {
    bit=1;
  }
  
  /* Wait a bit */
  ds_delay(200);
  return bit;
}

uint8_t ds_readbyte(void) {
  uint8_t ret=0;
  int i;
  for ( i=0 ; i<8 ; i++ ) {
    if (ds_readbit()) {
      ret |= (1 << i);
    }
  }
  return ret;
}

void ds_writebyte(int x) {
  int i;
  for ( i=0 ; i<8 ; i++ ) {
    ds_writebit(x & ( 1 << i ) );
  }
}

void ds_read_scratchpad(void) {

  int i;    // iterator for eight scratchpad bytes
  ds_reset();
  
  /* Skip ROM command */
  ds_writebyte(0xCC);
  
  /* read scratchpad command */
  ds_writebyte(0xBE);   // read scratchpad command

  printf("Reading scratchpad: ");
  
  /* read scratchpad data */
  for ( i=0 ; i<8 ; i++ ) {
    ds_scratchpad[i]=ds_readbyte();
    printf("%02X",ds_scratchpad[i]);
  }
  printf("\n");
}

void ds_write_scratchpad(void) {

  int i;    //iterator for eight scratchpad bytes
  ds_reset();
  
  /* write scratchpad command */
  ds_writebyte(0x4E);
  
  /* write scratchpad data */
  for ( i=0 ; i<=8 ; i++ ) {
    ds_writebyte(ds_scratchpad[i]);
  }
}

void ds_read_temp(void) {

  int i;    // iterator for eight scratchpad bytes
  ds_reset();
  
  /* Skip ROM command */
  ds_writebyte(0xCC);
  
  /* convert temp command */
  ds_writebyte(0x44);   // convert temp command

  for (i=0 ; i<100 ; i++) {
    ds_delay(10000);
  }

  ds_reset();
  /* Skip ROM command */
  ds_writebyte(0xCC);
  /* read scratchpad command */
  ds_writebyte(0xBE);   // read scratchpad command
  
  printf("Reading temp: ");
  
  /* read scratchpad data */
  for ( i=0 ; i<=8 ; i++ ) {
    ds_scratchpad[i]=ds_readbyte();
    printf("%02X",ds_scratchpad[i]);
  }
  printf("\n");
}
























































#endif
