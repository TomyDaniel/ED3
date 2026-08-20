#include "lpc1769.h"

void config();

int main(){

    config();
    while(true);

}

void config(){

    LPC_PINCON -> PINSEL4 |= (1 << 22);

    LPC_SC -> EXTMODE |= (1 << 1);

    LPC_SC -> EXTPOLAR &= ~(1 << 1);

    LPC_PINCON -> PINSEL2 &= ~(3 << 0);

    LPC_GPIO1 -> FIODIR |= (1 << 0);

    NVIC_EnableIRQ(EINT1_IRQn);
}

void EINT1_IRQHandler(void){

    if((LPC_GPIO1 -> FIOPIN >> 0) & 1){
        LPC_GPIO1 -> FIOCLR = (1 << 0);
    }else{
        LPC_GPIO1 -> FIOSET = (1 << 0);
    }

    LPC_SC->EXTINT = (1 << 1); 
}