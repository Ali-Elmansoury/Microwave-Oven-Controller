#define F_CPU 16000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>


volatile char can_dec = 0;
volatile unsigned int time = 0 , beeb_beeb=1;



ISR(TIMER1_OVF_vect) {
	TCNT1 = 0xFF00;
	if (can_dec && time > 0)
	time--;
}


void init_Timer1() {
	TIFR1 |= 0x01;  // clear overflow flag
	TCNT1 = 50000;
	TCCR1A = 0x00;
	TCCR1B = 0x05;  // Timer mode with 1024 prescler
}

int main(){
	DDRD = 0xFF;  
	DDRC = 0xFF;  
	DDRB = 0x00;  
	PORTB |= 0xFF; 
	TCNT1 = 50000; 
	TCCR1A = 0x00;
	TCCR1B = (1<<CS10) | (1<<CS12);
	TIMSK1 = 0x01;   // Enable timer1 overflow interrupt(TOIE1)
	sei();           // set global interupt bit
	init_Timer1();
	PORTC =0;
	while (1){
		int mins = time/60;
		int secl = (time-(mins*60))%10;
		int sech = (time-(mins*60))/10;
		int num = 0;
		num = secl;
		sech = sech << 4;
		num ^= sech;
		PORTD = num;
		if (time > 0 && can_dec){
			int w = mins & 0x0F;
			w = mins | 0x10;
			PORTC = w;
		}else if (time > 0 && !can_dec){
			int w = mins & 0x0F;
			PORTC = w;
		}else if (time == 0 && beeb_beeb){
			int w = mins & 0x0F;
			w = mins | 0x20;
			PORTC = w;
			_delay_ms(100);
			PORTC &= 0x0F;
			_delay_ms(100);
			PORTC = w;
			_delay_ms(100);
			PORTC &= 0x0F;			
			beeb_beeb=0;
		}
		if (~PINB & (1<<0)){
			_delay_ms(50);
			if (PINB & (1<<0)){
				beeb_beeb=1;
				int x = time+10;
				if ( x < 599){
					time += 10;
					}else{
					time = 599;
				}
			}
		}
		if(~PINB & (1<<1)){
			_delay_ms(50);
			if (PINB & (1<<1) && !can_dec){
				time = 0;
			}else if (PINB & (1<<1)){
				can_dec = 0;
			}
		}
		if (~PINB & (1<<2)){
			can_dec = 1;
			beeb_beeb=1;
		}
		//_delay_ms(200);
	}
}
