#include "lpc1769.h"

void config();
void contador();

int main(){

    config();

    while(true){
        if(!((LPC_GPIO1 -> FIOPIN >> 4) & 1)){
            contador();
        }
        if(!((LPC_GPIO1 -> FIOPIN >> 5) & 1)){
            LPC_GPIO1 -> FIOCLR = (0xF << 0);
        }
    }

}

void config(){

    LPC_PINCON -> PINSEL2 &= ~(0xFFF << 0);

    LPC_GPIO1 ->FIODIR |= (1 << 0);
    LPC_GPIO1 ->FIODIR |= (1 << 1);
    LPC_GPIO1 ->FIODIR |= (1 << 2);
    LPC_GPIO1 ->FIODIR |= (1 << 3);
}

void contador(){

    for(int i=0; i<=15; i++){
        LPC_GPIO1 -> FIOCLR = (0xF << 0);
        LPC_GPIO1 -> FIOSET = ((i & 0xF) << 0);
    }    
}