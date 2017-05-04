#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define ToggleBit(port, bit) (port ^= (1 << bit) //  toggles the bit by negating

#define BV(x) (1<<x)


void initIO(void) {
    DDRC |= BV(led2);
    ToggleBit(PORTC, led2);
}
int open(void){
    
    PORTC = (BV(5));
    _delay_us(2000);
    PORTC  ^=BV(5);
    _delay_us(18000);
    
    return 1;
}

int close(void){
    //ToDo: need to update this
    //close sequence
    PORTC = (BV(5));
    _delay_us(1000);
    PORTC   ^=BV(5);
    _delay_us(19000);
    return 1;
}

void initServo(void){
    //            //make A5 output.
    //            *ddr=BV(pin);      // PORTC declared as output 0xFF is all output 11111 so i have 8 bits i can set and 8 leds....
    //            *port=BV(pin);     // PORTC is initially LOW OFF the led initially /
    //to do make the register parameter work
    
    DDRC = BV(5);
    //set to closed position
    PORTC = (BV(5));
    _delay_us(1000);
    PORTC   ^=BV(5);
    _delay_us(19000); //remember that total period is 2ms or 2k micros and you need to do it this way because of standard 50hz servo
    
}

int main(void) {
   
    initServo();
    
    while(1){
        ToggleBit(PORTC, led2);

        open();
        _delay_ms(500);
        close();
        _delay_ms(500);
    }
    
    return 0;
    
}