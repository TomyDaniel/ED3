#include "LPC17xx.h"

#define LEDROJO (1 << 22)
#define LEDVERDE (1 << 25)
#define LEDAZUL (1 << 26)

int segA = 2500;
int segB = 5000;

void confiGPIO();
void delayA();
void delayB();

void secuenciaA();
void secuenciaB();

int main(){

    confGPIO();

    while(1){

        secuenciaA();
        delayB();
        secuenciaB();
        delayB();

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

void secuenciaA(){

    LPC_GPIO0 -> FIOCLR = LEDROJO;
    delayA();
    LPC_GPIO0 -> FIOSET = LEDROJO;
    delayA();
    LPC_GPIO3 -> FIOCLR = LEDVERDE;
    delayA();
    LPC_GPIO3 -> FIOSET = LEDVERDE;
    delayA();
    LPC_GPIO0 -> FIOCLR = LEDROJO;
    LPC_GPIO3 -> FIOCLR = LEDVERDE;
    delayA();
    LPC_GPIO0 -> FIOSET = LEDROJO;
    LPC_GPIO3 -> FIOSET = LEDVERDE;
    delayA();    

}

void secuenciaB(){

    LPC_GPIO3 -> FIOCLR = LEDAZUL;
    delayA();
    LPC_GPIO3 -> FIOSET = LEDAZUL;
    delayA();
    LPC_GPIO0 -> FIOCLR = LEDROJO;
    LPC_GPIO3 -> FIOCLR = LEDAZUL;
    delayA();
    LPC_GPIO0 -> FIOSET = LEDROJO;
    LPC_GPIO3 -> FIOSET = LEDAZUL;
    delayA();
    LPC_GPIO0 -> FIOCLR = LEDROJO;
    LPC_GPIO3 -> FIOCLR = LEDVERDE;
    LPC_GPIO3 -> FIOCLR = LEDAZUL;
    delayA();
    LPC_GPIO0 -> FIOSET = LEDROJO;
    LPC_GPIO3 -> FIOSET = LEDVERDE;
    LPC_GPIO3 -> FIOSET = LEDAZUL;
    delayA();

}

void delayA(){

    for(int i=0; i < segA; i++){
        for(int j=0; j < seg; j++){}
    }
}

void delayB(){

    for(int i=0; i < segB; i++){
        for(int j=0; j < seg; j++){}
    }

}
