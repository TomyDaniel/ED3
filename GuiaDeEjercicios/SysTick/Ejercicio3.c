#include "LPC17xx.h"

#define ticks (0.5 * 100000000 - 1)
#define LED (1 << 22)

void config();
void SysTick_Handler();

int main(){

    config();

    SysTick_Config(ticks);

    while(1){}

}

void config(){

    // Configuracion LED

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 |= (1 << 13);

    LPC_GPIO0 -> FIODIR |= LED;

    LPC_GPIO0 -> FIOSET = LED;

}

void SysTick_Handler(){

    if(LPC_GPIO0 -> FIOPIN & LED){
        LPC_GPIO0 -> FIOCLR = LED;
    }else{
        LPC_GPIO -> FIOSET = LED;
    }

}
