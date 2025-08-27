// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

volatile uint32_t delay = 0;

#define LED1 (1 << 22)
#define LED2 (1 << 0)
#define BUTTON (1 << 10)

void SysTick_Handler(void);

void configuraciones(void);


int main(void) {

    configuraciones();

    SysTick_Config(SystemCoreClock / 1000);

    uint32_t FastTime = 0;

    uint32_t SlowTime = 0;

    while(1) {

        if((delay - SlowTime) >= 1000){

            if(LPC_GPIO0->FIOPIN & LED1){
                LPC_GPIO0->FIOCLR = LED1;
            }else{
                LPC_GPIO0->FIOSET = LED1;
            }

            SlowTime = delay;
        }

        if(LPC_GPIO2->FIOSET & ~BUTTON){

            if((delay - FastTime) >= 250){
                if(LPC_GPIO0->FIOPIN & LED2){
                    LPC_GPIO0->FIOCLR = LED2;
                }else{
                    LPC_GPIO0->FIOSET = LED2;
                }

                FastTime = delay;

            }

        }else{

            LPC_GPIO0->FIOCLR = LED2;

        }


    }
    return 0 ;
}

void SysTick_Handler(void){
	delay++;
}


void configuraciones(void){

	LPC_PINCON->PINSEL1 &= ~(3 << 12);

	LPC_GPIO0->FIODIR |= LED1;

    LPC_PINCON->PINSEL0 &= ~(3 << 0);

    LPC_GPIO0->FIODIR |= LED2;

    LPC_PINCON->PINSEL4 &= ~(3 << 20);

    LPC_GPIO2->FIODIR &= ~BUTTON;
}
