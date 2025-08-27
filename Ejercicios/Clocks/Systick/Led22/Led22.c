// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

volatile uint32_t delay = 0;

#define LED (1 << 22)

void SysTick_Handler(void);

void configuraciones(void);

int main(void) {

    configuraciones();

    SysTick_Config(SystemCoreClock / 1000); // Parpadee 50 ms

    uint32_t a = 0;

    while(1) {

        if((delay - a) >= 500){
            
            if(LPC_GPIO0->FIOPIN & LED){
                LPC_GPIO0->FIOCLR = LED;
            }else{
                LPC_GPIO0->FIOSET = LED;
            }
            
            a = delay;
            
        }

    }
    return 0 ;
}

void configuraciones(void){

	LPC_PINCON->PINSEL1 &= ~(3 << 12);

	LPC_GPIO0->FIODIR |= LED;
}

void SysTick_Handler(void){
    delay++;
}