#include "LPC17xx.h"

#define BOTON (1 << 11)
#define LEDS (0xF << 0)

#define ticks (0.1 * 100000000 - 1) // 100ms

int count = 20;

int contador[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

int conteo = 0;

void config();
void EINT1_IRQHandler();
void SysTick_Handler();

int main(){

    config();

    NVIC_EnableIRQ(EINT1_IRQn);
    SysTick_Config(ticks);

    while(1){

    }

}

void config(){

    // Configuracion leds

    LPC_PINCON -> PINSEL0 &= ~(0xFF << 0);

    LPC_PINCON -> PINMODE0 &= ~(0xFF << 0);

    LPC_PINCON -> PINMODE0 |= (0xAA << 0);

    LPC_GPIO0 -> FIODIR |= LEDS;

    LPC_GPIO0 -> FIOSET = contador[conteo];

    // Configuracion boton

    LPC_PINCON -> PINSEL4 &= ~(3 << 22);

    LPC_PINCON -> PINSEL4 |= (1 << 22);

    LPC_PINCON -> PINMODE4 &= ~(3 << 22);

    LPC_PINCON -> PINMODE4 |= (3 << 22);

    LPC_GPIO4 -> FIODIR &= ~BOTON;

    LPC_SC -> EXTMODE |= (1 << 1);

    LPC_SC -> EXTPOLAR |= (1 << 1);

}

void SysTick_Handler(){
    count--;
    if(count == 0){
        conteo = 0;
        count = 20;

        LPC_GPIO0 -> FIOCLR = LEDS;
        LPC_GPIO0 -> FIOSET = contador[conteo];
    }
}

void EINT1_IRQHandler(){

    if(conteo < 15){
        conteo++;
    }

    LPC_GPIO0 -> FIOCLR = LEDS;
    LPC_GPIO0 -> FIOSET = contador[conteo];

    LPC_SC -> EXTINT = (1 << 1);
    
}
