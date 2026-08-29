#include "LPC17xx.h"

#define LEDROJO (1 << 22)
#define LEDVERDE (1 << 25)
#define LEDAZUL (1 << 26)

int seg = 2500;

void confiGPIO();
void delay();

int main(){

    confGPIO();

    while(1){

        LPC_GPIO0 -> FIOCLR = LEDROJO;
        delay();
        LPC_GPIO0 -> FIOSET = LEDROJO;
        delay();
        LPC_GPIO3 -> FIOCLR = LEDVERDE;
        delay();
        LPC_GPIO3 -> FIOSET = LEDVERDE;
        delay();
        LPC_GPIO3 -> FIOCLR = LEDAZUL;
        delay();
        LPC_GPIO3 -> FIOSET = LEDAZUL;
        delay();
    }

}

void confGPIO(){

    /*
    Led Rojo 
    */

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 |= (1 << 13);

    LPC_GPIO0 -> FIODIR |= LEDROJO;

    LPC_GPIO0 -> FIOSET = LEDROJO;

    /*
    Led Azul
    */

    LPC_PINCON -> PINSEL7 &= ~(3 << 20);

    LPC_PINCON -> PINMODE7 &= ~(3 << 20);

    LPC_PINCON -> PINMODE7 |= (1 << 21);

    LPC_GPIO3 -> FIODIR |= LEDAZUL;

    LPC_GPIO3 -> FIOSET = LEDAZUL;

    /*
    Led Verde
    */

    LPC_PINCON -> PINSEL7 &= ~(3 << 18);

    LPC_PINCON -> PINMODE7 &= ~(3 << 18);

    LPC_PINCON -> PINMODE7 |= (1 << 19);

    LPC_GPIO3 -> FIODIR |= LEDVERDE;

    LPC_GPIO3 -> FIOSET = LEDVERDE;

}

void delay(){

    for(int i=0; i < seg; i++){
        for(int j=0; j < seg; j++){}
    }
}
