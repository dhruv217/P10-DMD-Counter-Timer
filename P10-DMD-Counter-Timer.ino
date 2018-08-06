

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

boolean counting = false;
int Mode = COUNTER;

/* Pin Comfig */
int buttonStartStop = 14;
int buttonSetMode = 15;
int buttonTimeUp = 16;
int time[4], i;


DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
char b[8];
String str;

void ScanDMD() {
	dmd.scanDisplayBySPI();
}

void DecrementTime() {
	if((counting) && ((time[0] != 0) || (time[1] != 0) || (time[2] != 0) || (time[3] != 0))) {
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
	dmd.selectFont(Arial_Black_16);
	time[0] = 0;
	time[1] = 0;
	time[2] = 0;
	time[3] = 0;
	
}

void loop() {
	if (digitalRead(buttonSetMode) == LOW) {
		Serial.println("Mode Changed To "+ Mode);
		if(Mode == SET_TIME_SS){
			Mode = COUNTER;
		} else {
			Mode += 1;
		}
    }
	if (digitalRead(buttonStartStop) == LOW && Mode == COUNTER) {
		counting = !counting;
		if(counting)Serial.println("Start");
		else Serial.println("Stop"); 
	}
    switch (Mode) {
		case COUNTER: 
			Timer();
			break;
		case SET_TIME_MM:
			counting = false;
			DisplaySetTimeMM();
			break;
		case SET_TIME_SS:
			counting = false;
			DisplaySetTimeSS();
			break;
	}
	if (Mode != COUNTER)
	{
		i += 1;
		if (i == 59999)
			i = 0;
	}
	delay(333);
}

void Timer() {
	printTimeToLED();
}

void DisplaySetTimeMM()
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
    printTimeToLEDMM();
}

void DisplaySetTimeSS()
{
	if (digitalRead(buttonTimeUp) == LOW)
	{ // Increment
		if (time[2] == 5 && time[3] == 9)
		{
			time[2] = 0;
			time[3] = 0;
		}
		else if (time[3] == 9)
		{
			time[2] += 1;
			time[3] = 0;
		}
		else
		{
			time[3] += 1;
		}
	}
	printTimeToLEDSS();
}

void printTimeToLED() {
	str = String(time[0])+String(time[1])+":"+String(time[2])+String(time[3]);
	str.toCharArray(b,6);
	dmd.drawString( 0, 1, b, 5, GRAPHICS_NORMAL );
}

void printTimeToLEDMM()
{
	if(i % 2 == 0) {
	str = String(time[0]) + String(time[1]) + ":" + String(time[2]) + String(time[3]);
	str.toCharArray(b, 6);
	dmd.drawString(0, 1, b, 5, GRAPHICS_NORMAL);
	} else {
		str = "  :" + String(time[2]) + String(time[3]);
		str.toCharArray(b, 6);
		dmd.drawString(0, 1, b, 5, GRAPHICS_NORMAL);
	}
}
void printTimeToLEDSS()
{
	if (i % 2 == 0)
	{
		str = String(time[0]) + String(time[1]) + ":" + String(time[2]) + String(time[3]);
		str.toCharArray(b, 6);
		dmd.drawString(0, 1, b, 5, GRAPHICS_NORMAL);
	}
	else
	{
		str = String(time[0]) + String(time[1]) + ":  ";
		str.toCharArray(b, 6);
		dmd.drawString(0, 1, b, 5, GRAPHICS_NORMAL);
	}
}
