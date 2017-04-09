/*
 correctly reading tags now.
 on 3/17 this was working.
 
 I am modifying it now to see if i can get a servo to turn with a card read.
 
 * main.c
 *
 * Copyright 2013 Shimon <shimon@monistit.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include <lcd.h>
#include <utils.h>
#include <spi.h>
#include <mfrc522.h>
#include <string.h>

#define BV(x) (1<<x)     // shifts bits by x. and sets the bit to 1 or 0 based on x 0 is zero shift, 1st bit.
#define ToggleBit(port, bit) (port ^= (1 << bit) // hopefully toggles the bit


uint8_t SelfTestBuffer[64];

int main(void)
{
    
    /* === === ===    SERVO SETUP   === === === */
    /*
     pin PB1 using 16 bit timer
     Read servo example that I have in xcode.
     
     to move the servo to max:


     OCR1A = ICR1 - 2000; //18000
     _delay_ms(500);
     
     to move servo to min
     OCR1A = ICR1 - 1000; //1000 milisecond
     _delay_ms(500);

     
     cannot use timer 1 because its used for spi. so we have to use timer 2 or do all software
     
     
     */

    void initServo(void){
            //make A5 output.
            DDRC=BV(5);      // PORTC declared as output 0xFF is all output 11111 so i have 8 bits i can set and 8 leds....
            PORTC=BV(5);     // PORTC is initially LOW OFF the led initially
        
        //set to closed position
        PORTC = (BV(5));
        _delay_us(1000);
        PORTC   ^=BV(5);
        _delay_us(19000);
    }
    
    int open(void){
        PORTC = (BV(5));
        _delay_us(2000);
        PORTC   ^=BV(5);
        _delay_us(18000);
        
        _delay_ms(4000);
        
        
        PORTC = (BV(5));
        _delay_us(1000);
        PORTC   ^=BV(5);
        _delay_us(19000);
        
        return 1;
    }
    
    int close(void){
        return 1;
    }
    
    
    /* === === === END SERVO SETUP === === === */
    
    initServo();
    
    uint8_t byte;
    uint8_t str[MAX_LEN];
    _delay_ms(50);
    LCDInit(LS_BLINK);
    LCDWriteStringXY(2,0,"RFID Reader");
    LCDWriteStringXY(5,1,VERSION_STR);
    
    open();
    close();
    spi_init();
    _delay_ms(1000);
    LCDClear();
    
    open();
    close();
    

    //init reader
    mfrc522_init();
    
    //check version of the reader
    byte = mfrc522_read(VersionReg);
    if(byte == 0x92)
    {
        LCDWriteStringXY(2,0,"MIFARE RC522v2");
        LCDWriteStringXY(4,1,"Detected");
    }else if(byte == 0x91 || byte==0x90)
    {
        LCDWriteStringXY(2,0,"MIFARE RC522v1");
        LCDWriteStringXY(4,1,"Detected");
    }else
    {
        LCDWriteStringXY(0,0,"No reader found");
    }
    
    byte = mfrc522_read(ComIEnReg);
    mfrc522_write(ComIEnReg,byte|0x20);
    byte = mfrc522_read(DivIEnReg);
    mfrc522_write(DivIEnReg,byte|0x80);
    
    _delay_ms(1500);
    LCDClear();
    
    uint8_t card1[16] = {174,68,45,91,156}; //only 5 elements!!!
    int validCard = -2;
    
    while(1){
        byte = mfrc522_request(PICC_REQALL,str);
        LCDHexDumpXY(0,0,byte);
        
        if(byte == CARD_FOUND)
        {
            byte = mfrc522_get_card_serial(str);
            if(byte == CARD_FOUND)
            {
                _delay_ms(30);
                LCDClear();
                
                for(byte=0;byte<8;byte++)
                    // LCDHexDumpXY(byte*2,0,str[byte]);
                    //       LCDHexDumpXY(byte*2, 0, str[byte]);
                    LCDWriteInt(str[byte], -1);
                
            }
            else
            {
                LCDWriteStringXY(0,1,"Error");
            }
            
            LCDWriteStringXY(0, 1, "BOUT TO CHECK");
            _delay_ms(1000);
            LCDClear(); //test
            for(int i=0; i<5; i++){
                _delay_ms(30);//test
                //LCDWriteStringXY(0,0,"i:  ");
                // LCDWriteIntXY(5,0, i,1);//test
                LCDWriteInt(str[i], -1);//test
                //   _delay_ms(1000);
            }
            _delay_ms(100);//test
            LCDClear();//test
            
            for(int k=0;k<5 ;k++){
                // LCDHexDumpXY(byte*2,0,str[byte]);
                //       LCDHexDumpXY(byte*2, 0, str[byte]);
                LCDWriteIntXY(0,1,str[k], -1);
                if(str[k] == card1[k])
                {
                    LCDWriteIntXY(0,1,str[k], -1);
                    LCDWriteString("MATCH");
                    _delay_ms(50);
                    LCDClear();
                    validCard = 1;
                }
                else{
                    validCard = -1;
                }
                
            }
            if(validCard == 1){
                LCDClear();
                _delay_ms(500);
                LCDWriteString("SUCCESS");
                open();
                validCard=5;
            }
            if(validCard == -1){
                LCDClear();
                _delay_ms(100);
                LCDWriteString("not valid");
                close();
            }
            
            _delay_ms(750);
            LCDClear();
            LCDWriteString("CHECKED");
            _delay_ms(250);
            LCDClear();
        }
        
        LCDWriteString("present card");
        _delay_ms(1000);
        LCDClear();
    }
}
