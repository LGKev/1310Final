//
//  RFID_UI.c
//  Mifare_SERVO
//
//  Created by Kevin Kuwata on 4/30/17.
//  Copyright © 2017 Kevin Kuwata. All rights reserved.
//

#include "RFID_UI.h"

void initLED(void){
    DDRC = BV(ledB);
    
    PORTC = BV(ledB);
    _delay_ms(100);
    PORTC ^= BV(ledB);
}

int addQ(int str[], int ADD[]){
    for(int i=1; i<=5; i++){
        if(str[i] != ADD[i])
            return -1;
    }
    return 0;
}

void addTag(int str[], int tagNumber){
    if(tagNumber ==1){
        for(int i=1; i<=5; i++){
            write_eeprom_word(tag1[i], str[i]);
            PORTC = BV(ledB);
            _delay_ms(100);
        }
        tagNumber++;
    }
    else if(tagNumber ==2){
        for(int i=1; i<=5; i++){
            write_eeprom_word(tag2[i], str[i]);
        }
        tagNumber++; //incriment so we can keep track if we have room or not
    }
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

void initButton(){
    // uint8_t button = 3;
    //uint8_t ledB = 2;
    DDRC ^= BV(button); //set as input
    DDRC |= BV(ledB); //set output
    
    PORTC ^= BV(button); // set low
    
}

int buttonRead(){
    if(bit_is_clear(PINC, button)){
        LCDClear();
        _delay_ms(40);
        LCDWriteString("button press");
        open();
        _delay_ms(500);
        close();
        _delay_ms(1000);
        LCDClear();
        return 1;
    }
    return 0;
}


void loadTags(void){
    for(int i=0; i<5; i++){
        ram_tag1[i] = read_eeprom_word(tag1[i]);
        ram_tag2[i] = read_eeprom_word(tag2[i]);
    }
}
