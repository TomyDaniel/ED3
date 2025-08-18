// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 1000000

#define LED (1 << 22)

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    while(1) {

        LPC_GPIO0->FIOSET = LED;
        delay(2);
        LPC_GPIO0->FIOCLR = LED;

    }
    return 0 ;
}

void delay(int cantidad){
	for(volatile int i=0; i<timing * cantidad; i++){}
}


void configuraciones(void){

	LPC_PINCON->PINSEL1 &= ~(3 << 12);

	LPC_GPIO0->FIODIR |= LED;
}
