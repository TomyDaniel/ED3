#include "LPC17xx.h"

#define PINES (0x7F << 0)
#define BOTON (1 << 5)

void config();
void EINT3_IRQHandler();

int secuencia [16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int contador = 0;

int main(){

    config();

    NVIC_EnableIRQ(EINT3_IRQn);

    while(1){

    }

}

void config(){

    // Configuracion PINES

    LPC_PINCON -> PINSEL4 &= ~(0x3FFF << 0);

    LPC_PINCON -> PINMODE4 &= ~(0x3FFF << 0);

    LPC_PINCON -> PINMODE4 |= (0x2AAA << 0);

    LPC_GPIO2 -> FIODIR |= PINES;

    LPC_GPIO2 -> FIOSET = secuencia[contador];

    //Configuracion boton

    LPC_PINCON -> PINSEL0 &= ~(3 << 10);

    LPC_PINCON -> PINMODE0 &= ~(3 << 10);

    LPC_PINCON -> PINMODE0 |= (3 << 10);

    LPC_GPIO0 -> FIODIR &= ~BOTON;

    LPC_GPIOINT -> IO0IntEnR |= (1 << 5);

}

void EINT3_IRQHandler(){

    if(contador >= 15){
        contador = 0;
        LPC_GPIO2 -> FIOSET = secuencia[contador];

        LPC_GPIOINT -> IO0IntClr = BOTON;
    }else{
        contador++;
        LPC_GPIO2 -> FIOSET = secuencia[contador];

        LPC_GPIOINT -> IO0IntClr = BOTON;
    }

}
