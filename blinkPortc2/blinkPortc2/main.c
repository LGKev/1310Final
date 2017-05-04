#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BV(x) (1<<x)     // shifts bits by x. and sets the bit to 1 or 0 based on x 0 is zero shift, 1st bit.


void initIO(void) {
    DDRC |= BV(2); //port c pin 2 output
    DDRC |= BV(1); //port c pin 2 output
    DDRC ^= BV(0);
    PORTC ^= BV(0); //don't forget to set as input


    
}

int main(void) {
	initIO();

	while (1) {
        PORTC |= BV(2);
        _delay_ms(30);
        PORTC ^= BV(2);
        _delay_ms(30);
        
        PORTC |= BV(1);
        _delay_ms(30);
        PORTC ^= BV(1);
        _delay_ms(30);
        
    
        
        while(bit_is_clear(PINC, 0)){
            PORTC |= BV(1);
            _delay_ms(400);
            PORTC ^= BV(1);
            _delay_ms(400);
        }
	}
	return 0; // never reached
}
