#define TOP 10000   //choose a appropriate value by test on board

#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd_drv.h>
#include <avr/interrupt.h>


//global variables
#define CURRENT_STATE _IRAM_BYTE(0x250)	//display mode
#define RNDNUM _IRAM_BYTE(0x251)	//random number
#define RESET_FLAG _IRAM_BYTE(0x252)	//control bit, make sure initialization only run once

void show_led(int num)
{
	uint8_t temp = 0x00;
	if(num & 0x01)
		temp |= 0x02;
	if(num & 0x02)
		temp |= 0x08;
	PORTB = temp;

	temp = 0x00;
	if(num & 0x04)
		temp |= 0x02;
	if(num & 0x08)
		temp |= 0x08;
	if(num & 0x10)
		temp |= 0x20;
	if(num & 0x20)
		temp |= 0x80;
	PORTL = temp;
}

ISR(ADC_vect) {

  char str[10];

  uint16_t signal = ADC;   //combination of ADCH and ADCL
  if(signal < ADC_BTN_SELECT ){
    if(CURRENT_STATE == 0){
      CURRENT_STATE = 1;
    } else {
      CURRENT_STATE = 0;
    }
  }
	RNDNUM = ((13*RNDNUM) + 17)%64;
  int num = RNDNUM;
  show_led(num);

	// when CURRENT_STATE = 0 = temperature
	if(CURRENT_STATE == 0){
		if((num & 0b00100000) == 0){
		  sprintf(str, "T: %2dC", num);
		  lcd_xy(0,0);
		  lcd_puts(str);
		} else {
			sprintf(str, "T:- %2dC", num);
		  lcd_xy(0,0);
		  lcd_puts(str);
		}
	} else if(CURRENT_STATE == 1){
		sprintf(str, "H: %2d%%", num);
	  lcd_xy(0,0);
	  lcd_puts(str);
	}

  reti();
	//to do...
	//check button state and update display mode
	//update random number
	//update LEDs
	//display temp/humi reading

}

ISR(TIMER1_COMPB_vect) {
	reti();
}


int main(void)
{
	RNDNUM = 19;

	if(RESET_FLAG)
	{
    CURRENT_STATE = 0;

    DDRB = 0xFF;

  	// initialize LED Strip
  	// initialize Timer/Counter 1
  	TCCR1A = 0x30;		// clear register OC1A on Compare Match
  	TCCR1B = 0x03;		// timer clock = system clock/64
  	TIMSK1 = 0x04;		// Output Compare A Interrupt
  	OCR1B = TOP;		// TOP Value

  	// initialize ADC
  	ADMUX = 0x40;
  	ADCSRB = 0x05;
  	ADCSRA = 0xAF;
  	DIDR0 = 0xFE;
  	DIDR2 = 0xFF;

    lcd_init();

		//initialize Timer/Counter 1 and ADC
		//show your V-number
    lcd_xy(10,1);     // 1 = second row 10 = 11th col
    lcd_puts("V-1321");

		RESET_FLAG = 0;
	}

	sei();

	while (1){}
}
