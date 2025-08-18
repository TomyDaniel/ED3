// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 1000000

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    while(1) {

    	LPC_GPIO2->FIOCLR = (1 << 5);
    	LPC_GPIO0->FIOCLR = (1 << 22);
        LPC_GPIO1->FIOSET = (1 << 29);

        delay(5);

        LPC_GPIO1->FIOCLR = (1 << 29);
        LPC_GPIO2->FIOSET = (1 << 5);

        delay(2);

        LPC_GPIO1->FIOCLR = (1 << 29); // Por las dudas
        LPC_GPIO2->FIOCLR = (1 << 5);
        LPC_GPIO0->FIOSET = (1 << 22);

        delay(5);

    }
    return 0 ;
}

void delay(int cantidad){
	for(volatile int i=0; i<timing * cantidad; i++){}
}


void configuraciones(void){

	LPC_PINCON->PINSEL3 &= ~(3 << 26);

	LPC_GPIO1->FIODIR |= (1 << 29);

	LPC_PINCON->PINSEL4 &= ~(3 << 10);

	LPC_GPIO2->FIODIR |= (1 << 5);

	LPC_PINCON->PINSEL1 &= ~(3 << 12);

	LPC_GPIO0->FIODIR |= (1 << 22);
}
