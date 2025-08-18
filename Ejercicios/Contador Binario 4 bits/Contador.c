// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 1000000

#define LED1 (1 << 0)
#define LED2 (1 << 1)
#define LED3 (1 << 2)
#define LED4 (1 << 3)

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    int contador = 0;

    int leds[4] = {LED1, LED2, LED3, LED4};

    while(1) {

    	for(volatile int i = 0; i < 4; i++){
            LPC_GPIO2->FIOCLR = leds[i];
        }

        LPC_GPIO2->FIOSET = contador;

        contador++;

        delay(3);

        if(contador == 16){
            contador = 0;
        }

    }
    return 0 ;
}

void delay(int cantidad){
	for(volatile int i=0; i<timing * cantidad; i++){}
}


void configuraciones(void){

	LPC_PINCON->PINSEL4 &= ~(3 << 0);

	LPC_GPIO2->FIODIR |= LED1;

	LPC_PINCON->PINSEL4 &= ~(3 << 2);

	LPC_GPIO2->FIODIR |= LED2;

	LPC_PINCON->PINSEL4 &= ~(3 << 4);

	LPC_GPIO2->FIODIR |= LED3;

	LPC_PINCON->PINSEL4 &= ~(3 << 6);

	LPC_GPIO2->FIODIR |= LED4;
}
