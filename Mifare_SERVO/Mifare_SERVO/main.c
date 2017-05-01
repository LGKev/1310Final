/*
 correctly reading tags now.
 on 3/17 this was working.

 I am modifying it now to see if i can get a servo to turn with a card read.

 * main.c
 *
 * Copyright 2013 Shimon <shimon@monistit.com>

 base code from Shimon to get the spi and rfid working. I added the servo support and implimented servo action on valid card read.

 the AVR requires a .1uF capacitor on power and ground. Without it the avr would randomly restart. I have had no restarts since the installation of the capacitors
 
 
 button polling works to turn on servo
 
 Not sure how I got card1 id. I dont know if it was uart or if it was with creative print lines 
    I used creative print lines
 
 I want to find a way to write the read card to an array. I think its stored in str[k] array so that should do it.

 x*/

#include <avr/io.h>
#include <util/delay.h>
#include <lcd.h>
#include <utils.h>
#include <spi.h>
#include <mfrc522.h>
#include <string.h>
#include <avr/eeprom.h> //so we can store cards

#define BV(x) (1<<x)     // shifts bits by x. and sets the bit to 1 or 0 based on x 0 is zero shift, 1st bit.
#define ToggleBit(port, bit) (port ^= (1 << bit) //  toggles the bit by negating

#define button 3 //that is the pin on portc that is input for button
#define ledB 2 //led for indicator for button
#define card_display_Delay 1 //adds delay for the card readout 0 for no delay, 1 for a 1000 delay

//macros for adding stuff to eeprom
#define read_eeprom_word(address) eeprom_read_word((const uint8_t *) address)
#define write_eeprom_word(address, value) eeprom_write_word((uint8_t *) address, (uint8_t *) value)

unsigned int EEMEM newCard;

//uint8_t SelfTestBuffer[64];

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

//void buttonInit(volatile uint8_t *port, int pin){
//    
//}

//void buttonRead(){
//    //button is hardware debounced.
//    
//}

//RFID constants
//holds id cards
uint8_t card1[5] = {174,68,45,91,156}; //only 5 elements!!!
uint8_t card2[5] = {64, 238 , 223, 135, 246};
uint8_t ADD[5] ={ 121, 241, 181 ,59, 6}; //add card used to add other cards.
uint8_t DELETE[5] = {145, 146  ,5 ,107 ,109}; //delete card used to delete the next card scanned.
uint8_t tag1[5];
uint8_t tag2[5];
uint8_t ram_tag1[5];
uint8_t ram_tag2[5];
uint8_t ram_tag3[5];
uint8_t ram_tag4[5];




int validCard = -2;
int tagNumber = 1; // iterrate after each tag added
int maxNumberTags =5; //maximum number of cards I can add- arbitrary, does not include the master keys (add, delete, master)


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

unsigned int EEMEM tag[5]= {0,0,0,0,0};
unsigned int ram_tag[5]; //this is where we write the read tag to the stack will be str[] array.

//to do int tagNumber)
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
    
//    _delay_ms(4000);
    
//    //close sequence
//   PORTC = (BV(5));
//    _delay_us(1000);
//   PORTC   ^=BV(5);
//    _delay_us(19000);
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
/* === === === END SERVO SETUP === === === */


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


int main(void)
{
    initLED();
    
    loadTags(); //load any tags stored in eeprom.
    initServo(); //make servo an output at pin 5

    uint8_t byte;
    uint8_t str[MAX_LEN];
    _delay_ms(50);
    LCDInit(LS_BLINK);
    LCDWriteStringXY(2,0,"ECEN1310 RFID Tag Reader");

    spi_init(); //start communication for rfid
    _delay_ms(1000);
    LCDClear();

    open();  //for testing
    close(); //was for testing

    //init reader
    mfrc522_init();

//    byte = mfrc522_read(ComIEnReg);
//    mfrc522_write(ComIEnReg,byte|0x20);
//    byte = mfrc522_read(DivIEnReg);
//    mfrc522_write(DivIEnReg,byte|0x80);

    _delay_ms(1500);
    LCDClear();

    int addNext = 0;

    
    PORTC |=BV(ledB);
    _delay_ms(90);
    PORTC ^= BV(ledB);
    _delay_ms(30);
    
while(1){
        byte = mfrc522_request(PICC_REQALL,str);

        LCDHexDumpXY(0,0,byte);
        buttonRead();
    if(addNext == 1){
        _delay_ms(100);
        LCDWriteString("Scan tag to ");
        LCDWriteStringXY(0, 1, "be added.");
       
        byte = mfrc522_request(PICC_REQALL,str);
        if(byte == CARD_FOUND)
        {
            byte = mfrc522_get_card_serial(str);
            if(byte == CARD_FOUND)
            {
                if(tagNumber ==1){
                    LCDClear();
                    LCDWriteString("ADDING CARD");
                    _delay_ms(2000);
                    PORTC =BV(ledB);
                    _delay_ms(90);
                    PORTC ^= BV(ledB);
                    _delay_ms(30);
                    for(int i=0; i<5; i++){
                        PORTC =BV(ledB);
                        _delay_ms(90);
                        PORTC ^= BV(ledB);
                        _delay_ms(30);
                        write_eeprom_word(tag1[i], str[i]);
                    }
                    tagNumber++;
                    addNext =0;
                    LCDClear();
                }
            }
        }
        loadTags();
        if(card_display_Delay==1){
            PORTC =BV(ledB);
            _delay_ms(90);
            PORTC ^= BV(ledB);
            _delay_ms(30);
        }
    }
    
    if(byte == CARD_FOUND && addNext !=1)
        {
            byte = mfrc522_get_card_serial(str);
            if(byte == CARD_FOUND)
            {
                for(byte=0;byte<5;byte++){
                    // LCDHexDumpXY(byte*2,0,str[byte]);
                    //       LCDHexDumpXY(byte*2, 0, str[byte]);
                  //  LCDWriteString(" ");
                  
                    LCDWriteInt(str[byte], -1);
                    if(card_display_Delay==1){_delay_ms(1000);}
                }

            }
            else
            { //not a valid card found
                LCDWriteStringXY(0,1,"Error");
            }

            LCDWriteStringXY(0, 1, "Card Comparison");
            _delay_ms(1000);
            LCDClear(); //test
            for(int i=0; i<5; i++){
                _delay_ms(30);//test
                //LCDWriteStringXY(0,0,"i:  ");                 // LCDWriteIntXY(5,0, i,1);//test
                LCDWriteInt(str[i], -1);//test
                //   _delay_ms(1000);
            }
            _delay_ms(100);//test
            LCDClear();//test

            for(int k=0;k<5 ;k++){
                // LCDHexDumpXY(byte*2,0,str[byte]);
                //       LCDHexDumpXY(byte*2, 0, str[byte]);
                LCDWriteIntXY(0,1,str[k], -1);
                if(str[k] == card2[k] || str[k] == card1[k] || str[k] == ram_tag1[k]) //this is where you are testing
                {
                    LCDWriteIntXY(0,1,str[k], -1);
                    LCDWriteString("MATCH"); //jk i am testing
                    _delay_ms(50);
                    LCDClear();
                    validCard = 1;
                }
                else{
                    validCard = -1;
                }
                
            if(str[k] == ADD[k]){
                //nextTag read add
                addNext = 1; //add the next tag
                break;
                } //end of add
            } //end of for loop checking if valid card
            if(validCard == 1){
                LCDClear();
                _delay_ms(500);
                LCDWriteString("SUCCESS");
                open();
                validCard=5;
            } //end if test worked
            if(validCard == -1){
                LCDClear();
                _delay_ms(100);
                LCDWriteString("not valid");
                close();
            } //end of loop for checking valid card

            _delay_ms(750);
            LCDClear();
            LCDWriteString("CHECKED");
            _delay_ms(250);
            LCDClear();
            
        } //end of valid card found

    LCDWriteStringXY(0,0,"Present Tag");
    //      _delay_ms(200);
    //    LCDClear();
    }

} //end of main
