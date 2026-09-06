#include "LPC17xx.h"

#define ticks (0.1 * 100000000 - 1)

#define DISPLAY (0x7F << 0)

void config();
void SysTick_Handler();

int secuencia[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int contador = 0;

int count = 10;

int main(){

    config();

    SysTick_Config(ticks);

    while(1){}

}

void config(){

    // Configuracion display

    LPC_PINCON -> PINSEL0 &= ~(0x3FFF << 0);

    LPC_PINCON -> PINMODE0 &= ~(0x3FFF << 0);

    LPC_PINCON -> PINMODE0 |= (0x2AAA << 0);

    LPC_GPIO0 -> FIODIR |= DISPLAY;

    LPC_GPIO0 -> FIOSET = secuencia[contador];

}

void SysTick_Handler(){

    count--;

    if(count == 0){
        if(contador >= 15){
            contador = 0;
            LPC_GPIO0 -> FIOCLR = DISPLAY;
            LPC_GPIO0 -> FIOSET = secuencia[contador];
        }else{
            LPC_GPIO0 -> FIOCLR = DISPLAY;
            LPC_GPIO0 -> FIOSET = secuencia[contador];
            contador++;
        }
        count = 10;
    }
    
}
