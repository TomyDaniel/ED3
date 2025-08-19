// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define LED1 (1 << 22)
#define B_PUSH (1 << 10)

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    while(1) {

        if((LPC_GPIO2->FIOPIN & B_PUSH) == 0){
            LPC_GPIO0->FIOSET = LED1;
        }else{
            LPC_GPIO0->FIOSET = LED1;
            delay(500);
            LPC_GPIO0->FIOCLR = LED1;
            delay(500);
        }
        
    }

    return 0 ;
}

void delay(int cantidad){
	int ticks =(SystemCoreClock / 1000) * cantidad;

    SysTick->LOAD = ticks - 1;

    SysTick->VAL = 0;

    SysTick->CTRL = (1 << 2) | (1 << 0);

    while(!(SysTick->CTRL & (1 << 16)));

    SysTick->CTRL = 0;
}


void configuraciones(void){

    LPC_PINCON->PINSEL1 &= ~(3 << 12);

    LPC_GPIO0->FIODIR |= LED1;

    LPC_PINCON->PINSEL4 &= ~(3 << 20);

    LPC_GPIO2->FIODIR &= ~B_PUSH;

}
