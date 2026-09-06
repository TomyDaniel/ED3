#include "LPC17xx.h"

#define LED (1 << 22)
#define PIN (1 << 10)

void config();
void delay();
void EINT0_IRQHandler();

int main(){

    config();

    NVIC_EnableIRQ(EINT0_IRQn);

    while(1){}

}

void config(){

    // Configuracion led

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 |= (1 << 13);

    LPC_GPIO0 -> FIODIR = LED;

    LPC_GPIO0 -> FIOSET = LED;

    // Configuracion boton

    LPC_PINCON -> PINSEL4 &= ~(3 << 20);

    LPC_PINCON -> PINSEL4 |= (1 << 20);

    LPC_PINCON -> PINMODE4 &= ~(3 << 20);

    LPC_GPIO2 -> FIODIR = PIN;

    LPC_SC -> EXTMODE = (1 << 0);
    
    LPC_SC -> EXTPOLAR &= ~(1 << 0);

}

void EINT0_IRQHandler(){

    if(LPC_GPIO0 -> FIOPIN & LED){
        LPC_GPIO0 -> FIOCLR = LED;
        LPC_SC -> EXTINT = 1;
    }else{
        LPC_GPIO0 -> FIOSET = LED;
        LPC_SC -> EXTINT = 1;
    }

}

void delay(){

    for(int i=0; i<2500; i++){
        for(int j=0; j<2500; j++){}
    }

}
