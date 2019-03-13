#include "c8051F020.h"
#include <stdlib.h>
#include <string.h>

//cmd servomotors : CS H/V [angle]
//freq oscillator = 22.1184 MHz 
//1 us ~= 22 tick

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int j = 0;
char currentPosCmd = 0; //iterateur sur la commande
char cmdRecue = 0; //bool indiquant qu'il y a une commande � traiter
char cmd[20] = ""; //chaine complete de commande
char typeCmd[5]; //premi�res lettres de la commande; indiquant son type
int timeHighServoHoriz = 2350;
unsigned int valTimer = 0;
unsigned int valHigh = 0;

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
void ServoHorizontal(char* typeCmd, char* cmd){
	char consigne[5];
	char currentPosConsigne = 0;
	if(strcmp(typeCmd, "CS") == 0){
		while(cmd[currentPosCmd] != ' '){ //relecture CS
			currentPosCmd++;
		}
		currentPosCmd++;
		if(cmd[currentPosCmd] == 'H'){ // 'H' pour le servo horizontal, 'V' pour le vertical
			while((cmd[currentPosCmd] != ' ') && (cmd[currentPosCmd] != '\r')){ //lecture commande en degres
				currentPosCmd++;
				currentPosConsigne++;
				consigne[currentPosConsigne] = cmd[currentPosCmd];
			}
			timeHighServoHoriz = 1500+atoi(consigne)*10; //consigne = +/- 90 => timeHigh = [600; 2400] us
			if(timeHighServoHoriz < 600){
				timeHighServoHoriz = 600;
			}
			if(timeHighServoHoriz > 2400){
				timeHighServoHoriz = 2400;
			}
		}
		currentPosCmd = 0;
	}
	
	valTimer = (TH2<<8)+TL2;
	valHigh = timeHighServoHoriz*22;
	
	
	if(valTimer < valHigh){
		OutServoH = 1;
	} else {
		OutServoH = 0;
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