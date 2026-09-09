#include "LPC17xx.h"

#define LED (1 << 22)
#define PIN (1 << 10)

#define ticks (10000)

void config();
void SysTick_Handler();
void EINT0_IRQHandler();

int count = 0;
static int cantidad = 100;

enum velocidad{
    velUno,
    velDos,
    velTres
};

int main(){

    config();

    SysTick_Config(ticks);

    NVIC_EnableIRQ(EINT0_IRQn);

    while(1){
        if(count <= cantidad){

        }else{
            if(LPC_GPIO0 -> FIOPIN & LED){
                LPC_GPIO0 -> FIOCLR = LED;
            }else{
                LPC_GPIO0 -> FIOSET = LED;
            }

            count = 0;
        }
    }

}

void config(){

    // LED

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 |= (1 << 13);

    LPC_GPIO0 -> FIODIR = LED;

    LPC_GPIO0 -> FIOSET = LED;

    // PIN

    LPC_PINCON -> PINSEL4 &= ~(3 << 20);

    LPC_PINCON -> PINSEL4 |= (1 << 20);

    LPC_PINCON -> PINMODE4 &= ~(3 << 20);

    LPC_PINCON -> PINMODE4 |= (1 << 21);

    LPC_GPIO2 -> FIODIR = PIN;

    LPC_SC -> EXTMODE |= (1 << 0);

    LPC_SC -> EXTPOLAR &= ~(1 << 0);

}

void EINT0_IRQHandler(){

    switch(momento){
        case(velUno):
            cantidad = 75;
            SysTick_Config(fr2);
            break;
        case(velDos):
            cantidad = 25;
            SysTick_Config(fr3);
            break;
        case(velTres):
            cantidad = 100;
            SysTick_Config(fr1);
            break;
    }

    LPC_SC -> EXTINT = 1;
}

void SysTick_Handler(){
    count++;
}
