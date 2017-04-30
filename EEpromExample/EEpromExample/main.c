#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

/*
 
 this project will show the use of eeprom
 
 
 cannnot get serial to work at this time, will use an led to indicate that serial EE prom saved and read.
 its an example of serial communicaiton as well.
 */

#define button 3 //that is the pin on portc that is input for button
#define ledB 2 //led for indicator for button
#define BV(x) (1<<x)     // shifts bits by x. and sets the bit to 1 or 0 based on x 0 is zero shift, 1st bit.
#define ToggleBit(port, bit) (port ^= (1 << bit) //  toggles the bit by negating

#define read_eeprom_word(address) eeprom_read_word ((const uint16_t*)address)
#define write_eeprom_word(address,value) eeprom_write_word ((uint16_t*)address,(uint16_t)value)
#define update_eeprom_word(address,value) eeprom_update_word ((uint16_t*)address,(uint16_t)value)


unsigned int EEMEM   my_eeprom_array[5];

void initButton(void){
    // uint8_t button = 3;
    //uint8_t ledB = 2;
    DDRC ^= BV(button); //set as input
    DDRC |= BV(ledB); //set output
    
    PORTC ^= BV(button); // set low
    
}



void initIO(void) {
    //set input.
    // uint8_t button = 3;
    uint8_t led2 = 2;
    uint8_t led = 4;
    //    DDRC &= ~(1 << button);
    DDRC ^= BV(button); //set the pin 3 in DDRC as input;
    DDRC |= BV(led2);
    DDRC |= BV(led); //set pin 4 as output
    
    PORTC ^= BV(button); //don't forget to set as input
    for (int i = 0; i<5; i++) {
        PORTC |= (BV(led));//set high
        PORTC |=(BV(led2));
        _delay_ms(80);
        PORTC ^= (BV(led));
        PORTC ^=(BV(led2));
        _delay_ms(80);
    }
}


int main(void)
{
    initIO();
    
    initButton();
    
    
   // write_eeprom_word(&my_eeprom_array[0],1);       // store value in eeprom
    
   int my_ram_array[5]={0,0,0,0,0};     // restore from eeprom
    
    
    int i=0;
    int count =0;
    
    
    write_eeprom_word(&my_eeprom_array[0], 0);

    while(1)
    {
        
        my_ram_array[2] = read_eeprom_word(my_eeprom_array[2]);     // restore from eeprom
        
        if(my_ram_array[2]==2){
            for(int j=1;j<=3; j++){
                PORTC |= BV(ledB);
                _delay_ms(5);
                PORTC ^= BV(ledB);
                _delay_ms(80);
                
            }
            write_eeprom_word(my_eeprom_array[2],0);       // store value in eeprom

        }
        
        if(bit_is_clear(PINC, button)){
            ///my_ram_array[i] = count;
            PORTC |= BV(ledB);
            _delay_ms(100);
            PORTC ^= BV(ledB);
            _delay_ms(100);
            write_eeprom_word(my_eeprom_array[i],count);       // store value in eeprom
            _delay_ms(100);
            i++;
            count++;
            if(i> 5){
                _delay_ms(300);

                PORTC |= BV(ledB);
                _delay_ms(1000);
                PORTC ^= BV(ledB);
                _delay_ms(40);
                count =0;
                i =0;
                

            }
            //_delay_ms(1000);
        }
        
        //_delay_ms(3000);
        
     

        }///end of while loop
}