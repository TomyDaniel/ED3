#include "LPC17xx.h"

#define ticks = 1000000
#define i

void SysTick_Handler();
void config();

int main(){

    config();
    SysTick_Config(ticks);

    i = 100;

    while(1){

        SysTick_Handler();

    }

}

void config(){

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 |= (1 << 13);

    LPC_GPIO0 -> FIODIR |= (1 << 22);

    LPC_GPIO0 -> FIOSET = (1 << 22);

}

void SysTick_Handler(){

    i--;

    if(i == 0){
        i = 100;

        if((1 << 22) & LPC_GPIO0 -> FIOPIN){

            LPC_GPIO0 -> FIOCLR = (1 << 22);

        }else{

            LPC_GPIO0 -> FIOSET = (1 << 22);

        }
    }

}
