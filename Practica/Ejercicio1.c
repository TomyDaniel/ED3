#include "lpc1769.h"

void config();

int main(){

    config();

    while(1){
        if ((LPC_GPIO0->FIOPIN >> 23) & 1) {
            LPC_GPIO0->FIOSET = (1 << 22);  // P0.23 alto → P0.22 alto
        } else {
            LPC_GPIO0->FIOCLR = (1 << 22);  // P0.23 bajo → P0.22 bajo
        }
    }

}

void config(){

    LPC_PINCON -> PINSEL1 &= ~(0xF << 12);

    LPC_GPIO0-> FIODIR |= (1 << 22);

}