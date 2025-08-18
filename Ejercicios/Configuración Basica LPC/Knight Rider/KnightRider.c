// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 1000000

#define LED1 (1 << 18)
#define LED2 (1 << 19)
#define LED3 (1 << 20)
#define LED4 (1 << 21)
#define LED5 (1 << 22)
#define LED6 (1 << 23)

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    int leds [6] = {LED1, LED2, LED3, LED4, LED5, LED6};

    while(1) {

    	for(volatile int i=0; i<6; i++){
    		LPC_GPIO1->FIOSET = leds[i];
    		delay(2);
    		LPC_GPIO1->FIOCLR = leds[i];
    	}

    	for(volatile int i=4; i>0; i--){
    	    LPC_GPIO1->FIOSET = leds[i];
    	    delay(2);
    	    LPC_GPIO1->FIOCLR = leds[i];
    	}

    }
    return 0 ;
}

void delay(int cantidad){
	for(volatile int i=0; i<timing * cantidad; i++){}
}


void configuraciones(void){

	LPC_PINCON->PINSEL3 &= ~(3 << 4);

	LPC_GPIO1->FIODIR |= LED1;

	LPC_PINCON->PINSEL3 &= ~(3 << 6);

	LPC_GPIO1->FIODIR |= LED2;

	LPC_PINCON->PINSEL3 &= ~(3 << 8);

	LPC_GPIO1->FIODIR |= LED3;

	LPC_PINCON->PINSEL3 &= ~(3 << 10);

	LPC_GPIO1->FIODIR |= LED4;

	LPC_PINCON->PINSEL3 &= ~(3 << 12);

	LPC_GPIO1->FIODIR |= LED5;

	LPC_PINCON->PINSEL3 &= ~(3 << 14);

	LPC_GPIO1->FIODIR |= LED6;
}
