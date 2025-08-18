// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 1000000

#define LED1 (1 << 4)
#define Button (1 << 12)

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    int led_encendido = 0;

    LPC_GPIO0->FIOCLR = LED1;

    while(1) {

        if((LPC_GPIO2->FIOPIN & Button) == 0){
            if(led_encendido == 0){
                LPC_GPIO0->FIOSET = LED1;
                led_encendido = 1;
            }else{
                LPC_GPIO0->FIOCLR = LED1;
                led_encendido = 0;
            }

            while((LPC_GPIO2->FIOPIN & Button) == 0){
                
            }
        }
        
    }

    return 0 ;
}

void delay(int cantidad){
	for(volatile int i=0; i<timing * cantidad; i++){}
}


void configuraciones(void){

	LPC_PINCON->PINSEL0 &= ~(3 << 8);

	LPC_GPIO0->FIODIR |= LED1;

    LPC_PINCON->PINSEL4 &= ~(3 << 24);

    LPC_GPIO2->FIODIR  &= ~Button;

}
