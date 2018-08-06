

#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include <MsTimer2.h>
#include "SystemFont5x7.h"
#include "Arial_black_16.h"


#define DISPLAYS_ACROSS 2
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
int buttonReset = 17;
int savedTime[4], time[4], i;


DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
char b[8];
String str;

void ScanDMD() {
	dmd.scanDisplayBySPI();
}

void DecrementTime() {
	if((counting) && ((time[0] != 0) || (time[1] != 0) || (time[2] != 0) || (time[3] != 0))) {
		counting = true;
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
	} if ((time[0] == 0) && (time[1] == 0) && (time[2] == 0) && (time[3] == 0)) {
		counting = false;
	}
}

void setup(){
	pinMode(buttonStartStop, INPUT_PULLUP);
    pinMode(buttonSetMode, INPUT_PULLUP);
    pinMode(buttonTimeUp, INPUT_PULLUP);
	pinMode(buttonReset, INPUT_PULLUP);
	

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

	dmd.drawMarquee("Archana InfoTech 2018",21,(32*DISPLAYS_ACROSS)-1,1);
	long start=millis();
	long timer=start;
	boolean ret=false;
	while(!ret)
	{
		if ((timer+30) < millis()) 
		{
		ret=dmd.stepMarquee(-1,0);
		timer=millis();
		}
	}  
	
}

void loop() {
	if (digitalRead(buttonSetMode) == LOW) {
		dmd.clearScreen(true);
		Serial.println("Mode Changed To "+ Mode);
		if(Mode == SET_TIME_SS){
			Mode = COUNTER;
			savedTime[0] = time[0];
			savedTime[1] = time[1];
			savedTime[2] = time[2];
			savedTime[3] = time[3];
		} else {
			Mode += 1;
		}
		delay(500);
    }
	if (digitalRead(buttonStartStop) == LOW && Mode == COUNTER) {
		counting = !counting;
		if(counting)Serial.println("Start");
		else Serial.println("Stop"); 
		delay(500);
	}
	if (digitalRead(buttonReset) == LOW && Mode == COUNTER) {
		if(!counting) {
			time[0] = savedTime[0];
			time[1] = savedTime[1];
			time[2] = savedTime[2];
			time[3] = savedTime[3];
		}
		delay(500);
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
	delay(100);

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
		delay(200);
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
		delay(200);
	}
	printTimeToLEDSS();
}

void printTimeToLED() {
	str = String(time[0])+String(time[1])+":"+String(time[2])+String(time[3])+" ";
	str.toCharArray(b,7);
	dmd.drawString( 2, 1, "T-", 2, GRAPHICS_NORMAL );
	dmd.drawString( 23, 1, b, 6, GRAPHICS_NORMAL );
}

void printTimeToLEDMM()
{
	str = String(time[0]) + String(time[1])+" ";
	str.toCharArray(b, 4);
	dmd.drawString(5, 1, "mm: ", 4, GRAPHICS_NORMAL);
	dmd.drawString(41, 1, b, 3, GRAPHICS_NORMAL);
	
}
void printTimeToLEDSS()
{
	str = String(time[2]) + String(time[3])+" ";
	str.toCharArray(b, 4);
	dmd.drawString(5, 1, "ss: ", 4, GRAPHICS_NORMAL);
	dmd.drawString(41, 1, b, 3, GRAPHICS_NORMAL);
} 
