#include "LPC17xx.h"

#define LED (1 << 22)
#define Boton (1 << 2)

void config();

int main(){

    config();

    while(1){

        if(LPC_GPIO0 -> FIOPIN & Boton){
            LPC_GPIO0 -> FIOCLR = LED;
        }else{
            LPC_GPIO0 -> FIOSET = LED;
        }

    }
}

void config(){

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);

    LPC_PINCON -> PINSEL0 &= ~(3 << 4);

    LPC_PINCON -> PINSEL0 |= (3 << 4);

    LPC_GPIO0 -> FIODIR |= LED;

    LPC_GPIO0 -> FIODIR |= Boton;

}
