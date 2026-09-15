#include "LPC17xx.h"

#define PINS (0x0F << 0)

uint8_t array[10];

int p=0;
static uint32_t contador = 0;

void config();
void EINT3_IRQHandler();

int main(){

    config();

    NVIC_EnableIRQ(EINT3_IRQn);
    NVIC_SetPriority(EINT3_IRQn, p);

    while(1){}

}

void config(){

    // Configuracion
    LPC_PINCON -> PINSEL0 &= ~(0xFF << 0);
    LPC_PINCON -> PINMODE0 &= ~(0xFF << 0);
    LPC_PINCON -> PINMODE0 |= (0xAA << 0);

    LPC_GPIO0 -> FIODIR &= ~PINS;

    LPC_GPIOINT -> IO0IntEnF |= PINS;
    LPC_GPIOINT -> IO0IntEnR |= PINS;

}

void EINT3_IRQHandler(){

    for(int j = 9; j > 0; j--){
        array[j] = array[j-1];
    }

    array[0] = LPC_GPIO0 -> FIOPIN & PINS;

    contador++;
    if(contador == 200 && p < 31){
        NVIC_SetPriority(EINT3_IRQn, p+1);
        contador = 0;
        p++;
    }
    if(p == 31){
        NVIC_DisableIRQ(EINT3_IRQn);
    }

    LPC_GPIOINT -> IO0IntClr = 0x0F;

}
