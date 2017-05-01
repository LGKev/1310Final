//
//  RFID_UI.h
//  Mifare_SERVO
//
//  Created by Kevin Kuwata on 4/30/17.
//  Copyright © 2017 Kevin Kuwata. All rights reserved.
//

#ifndef RFID_UI_h
#define RFID_UI_h

#include <stdio.h>

void initServo(void);
int open(void);
int close(void);


void initButton();


void initLED(void);
int addQ(int str[], int ADD[]);
void addTag(int str[], int tagNumber)

void loadTags(void)

#endif /* RFID_UI_h */
