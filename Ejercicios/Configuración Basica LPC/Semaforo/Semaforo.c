// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 1000000

#define LED1 (1 << 22)
#define LED2 (1 << 25)
#define LED3 (1 << 26)

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    while(1) {

        LPC_GPIO0->FIOCLR = LED1;
        delay(5);
        LPC_GPIO3->FIOSET = LED1;
        delay(5);
        LPC_GPIO0->FIOCLR = LED2;
        delay(5);
        LPC_GPIO3->FIOSET = LED2;
        delay(5);
        LPC_GPIO0->FIOCLR = LED3;
        delay(5);
        LPC_GPIO3->FIOSET = LED3;
        delay(5);

    }
    return 0 ;
}

void delay(int cantidad){
	for(volatile int i=0; i<timing * cantidad; i++){}
}


void configuraciones(void){

	LPC_PINCON->PINSEL1 &= ~(3 << 12);

    LPC_GPIO0->FIODIR |= LED1;

    LPC_PINCON->PINSEL7 &= ~(8 << 18);

    LPC_GPIO3->FIODIR |= (LED2 | LED3);
}
