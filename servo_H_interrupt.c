#include "c8051F020.h"
#include <stdlib.h>
#include <string.h>

//cmd servomotors : CS H/V [angle]
//freq oscillator = 22.1184 MHz 
//1 us ~= 22 tick

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int j = 0;
char* param1 = "CS";
char* param2 = "H";
char* param3 = "45";

int valHighServoHoriz = 0;
unsigned int valTimer = 0;
unsigned int valHigh = 0;
int numberOfTick = 0;
int ticksPerPeriod = 0;

sbit OutServoH = P0 ^ 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void timerDelay(){ //delai de 10ms
	j = 0;
	for(j = 0; j < 25000; j++){
	}
}

void enable_int () {
	EA = 1; // General interrupt enable
	ET2 = 0x01; // TIMER2-OVFLW
	return;
}


void configTimer2(){
	RCLK0 = 0;//pas UART0
	TCLK0 = 0;//pas UART0
	CPRL2 = 0;
	CKCON |= 0x20;
	RCAP2H = 0x00; //reload value
	RCAP2L = 0x00;
	TR2 = 1; // Activation du timer 2
}


void init(){
	configTimer2();
	enable_int();
	P3MDOUT = P3MDOUT | 0x80; //Passe le pin 3.7 en push-pull
	P0MDOUT = P0MDOUT | 0x01; //Passe le pin 0.0 en push-pull
	XBR2 = XBR2 | 0x40;
}

//cmd = "CS H -45"

int ServoHorizontal interrupt x { //remplacer x 
	if(strcmp(param1, "CS") == 0){
		if(strcmp(param2, "H") == 0){
			valHighServoHoriz = 150+atoi(param3); //1 interruption / 10us
		}
	}
	numberOfTick += 1;
	
	if(numberOfTick < valHighServoHoriz){
		OutServoH = 1;
	} else {
		OutServoH = 0;
	}
	
	if(numberOfTick >= ticksPerPeriod){
		numberOfTick = 0;
	}
	
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void main(){
	WDTCN = 0xDE;   // Devalidation du watchdog 
	WDTCN = 0xAD;

	OSCXCN =  0xef; //configure external oscillator
	timerDelay();
	while(!(OSCXCN&(0x80))){}

	OSCICN = OSCICN | 0x08; //utilise l'oscillateur externe
	
	init();

	while(1) 
	{
		ServoHorizontal(typeCmd, cmd);
	}
}