

#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include <MsTimer2.h>
#include "SystemFont5x7.h"
#include "Arial_black_16.h"


#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

#define COUNTER 0
#define SET_TIME_MM 1
#define SET_TIME_SS 2

boolean counting = true;
int Mode = COUNTER;

/* Pin Comfig */
int buttonStartStop = 14;
int buttonSetMode = 15;
int buttonTimeUp = 16;
int time[4];


DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
char b[8];
String str;

void ScanDMD() {
	dmd.scanDisplayBySPI();
}

void DecrementTime() {
	if((counting) && ((time[0] != 0) || (time[1] != 0) || (time[2] != 0) || (time[3] != 0))) {
		Serial.println("decrementing");
		if(time[3] == 0) {
			time[3] = 9;
			if(time[2] == 0) {
				time[2] =5;
				if(time[1] == 0) {
					time[1] = 9;
					time[0] -= 1;
				} else {
					time[1] -= 1;
				}
			} else {
				time[2] -= 1;
			}
		} else {
			time[3] -= 1;
		}
	}
}

void setup(){
	pinMode(buttonStartStop, INPUT_PULLUP);
    pinMode(buttonSetMode, INPUT_PULLUP);
    pinMode(buttonTimeUp, INPUT_PULLUP);

	Serial.begin(9600);
	Timer1.initialize( 5000 );
	Timer1.attachInterrupt( ScanDMD );
	MsTimer2::set(1000, DecrementTime); // 1000ms period
  	MsTimer2::start();
	dmd.clearScreen( true );
	time[0] = 1;
	time[1] = 0;
	time[2] = 0;
	time[3] = 0;
	
}

void loop() {
	if (digitalRead(buttonSetMode) == LOW){
		if(Mode == SET_TIME_SS){
			Mode = COUNTER;
		} else {
			Mode += 1;
		}
    }
	if (digitalRead(buttonStartStop) == LOW && Mode == COUNTER) {
		counting = !counting;
	}
    switch (Mode) {
		case COUNTER: 
			Timer();
			break;
		case SET_TIME_MM:
			DisplaySetTimeMM();
			break;
		case SET_TIME_SS:
			DisplaySetTimeMM();
			break;
	}
	delay(100);
}

void Timer() {
	printTimeToLED();
}

void DisplaySetTimeMM(void)
{
    if (digitalRead(buttonTimeUp) == LOW) {    // Increment
        if (time[0] == 5 && time[1] == 9) {
            time[0] = 0;
            time[1] = 0;
        }
        else if (time[1] == 9) {
            time[0] += 1;
            time[1] = 0;
        }
        else {
            time[1] += 1;
        }
    }
    printTimeToLED();
}

void printTimeToLED() {
	str="";
	str = String(time[0])+String(time[1])+":"+String(time[2])+String(time[3]);
	str.toCharArray(b,6);
	dmd.selectFont(Arial_Black_16);
	dmd.drawString( 0, 1, b, 5, GRAPHICS_NORMAL );
}
