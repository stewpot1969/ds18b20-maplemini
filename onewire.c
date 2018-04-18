#ifndef ONEWIRE_C
#define ONEWIRE_C


/* Set these three to define which timer is used.
  Valid values are TIM2-TIM5 */

#define SIMPLETIMER TIM3
#define RCC_TIMER RCC_TIM3
#define RST_TIMER RST_TIM3

/* Set these to define which I/O pin is used. */

void timer_init(void);
void timer_delay(int us);

void timer_init(void) {
    
	/* Reset SIMPLETIMER peripheral to defaults. */
	rcc_periph_clock_enable(RCC_TIMER);
	
	rcc_periph_reset_pulse(RST_TIMER);
	TIM_CR1(SIMPLETIMER) = TIM_CR1_CKD_CK_INT | 0x0010 | TIM_CR1_OPM | TIM_CR1_ARPE;
	TIM_PSC(SIMPLETIMER) = 7;
  TIM_EGR(SIMPLETIMER) = TIM_EGR_UG;

}

void timer_delay(int us) {

  TIM_CNT(SIMPLETIMER) = us;
  TIM_CR1(SIMPLETIMER) |= TIM_CR1_CEN;
  while ( TIM_CR1(SIMPLETIMER) & TIM_CR1_CEN );
    
}

#endif
