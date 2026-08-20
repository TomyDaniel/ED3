#include "lpc1769.h"

void config();
void prenderPares();
void prenderImpares();
void invertir();

int main(){

    config();

}

void config(){

    LPC_PINCON -> PINSEL4 &= ~(0xFFFF << 0);

    LPC_GPIO2 -> FIODIR |= (0xFF << 0);
}

void prenderPares(){

    LPC_GPIO2 -> FIOCLR = (0xFF << 0);

    LPC_GPIO2 -> FIOSET = (0xAA << 0); // LSB -> 10101010 -> MSB
}

void prenderImpares(){

    LPC_GPIO2 -> FIOCLR = (0xFF << 0);

    LPC_GPIO2 -> FIOSET = (0x55 << 0); // LSB -> 01010101 -> MSB
}

void invertir(){

    if((LPC_GPIO2 -> FIOPIN = 0) & 1){
        prenderImpares();
    }else{
        prenderPares();
    }

}