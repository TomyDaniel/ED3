#include "LPC17xx.h"

#define ticks (0.05 * 100000000 - 1)
#define LEDS (0xFF << 0)
#define BOTON (1 << 10)

void config();
void SysTick_Handler();
void EINT0_IRQHandler();

int count = 5;

int secuencia[8] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

int contador = 0;

enum toque{
    IDLE,
    CORRIENDO,
    PAUSA
};

enum toque estado = IDLE;

int main(){

    config();

    SysTick_Config(ticks);

    NVIC_EnableIRQ(EINT0_IRQn);

    while(1){}

}

void config(){

    // Configuracion LEDS

    LPC_PINCON -> PINSEL0 &= ~(0xFFFF << 0);

    LPC_PINCON -> PINMODE0 &= ~(0xFFFF << 0);

    LPC_PINCON -> PINMODE0 |= (0xAAAA << 0);

    LPC_GPIO0 -> FIODIR |= LEDS;

    LPC_GPIO0 -> FIOCLR = LEDS;

    // Configuracion boton

    LPC_PINCON -> PINSEL4 &= ~(3 << 20);

    LPC_PINCON -> PINSEL4 |= (1 << 20);

    LPC_PINCON -> PINMODE4 &= ~(3 << 20);

    LPC_PINCON -> PINMODE4 |= (3 << 20);

    LPC_GPIO2 -> FIODIR &= ~BOTON;

    LPC_SC -> EXTPOLAR |= (1 << 0);

    LPC_SC -> EXTMODE |= (1 << 0);

}

void SysTick_Handler(){

    if(estado == CORRIENDO){
        count--;
        if(count == 0){
            if(contador == 8){
                contador = 0;
                LPC_GPIO0 -> FIOCLR = LEDS;
                LPC_GPIO0 -> FIOSET = secuencia[contador];
            }else{
                LPC_GPIO0 -> FIOCLR = LEDS;
                LPC_GPIO0 -> FIOSET = secuencia[contador];
                contador++;
            }
            count = 5;
        }
    }

    if(estado == PAUSA){
        LPC_GPIO0 -> FIOSET = secuencia[contador];
    }

}

void EINT0_IRQHandler(){

    if(estado == CORRIENDO){
        estado = PAUSA;
    }else{
        estado = CORRIENDO;
    }

    LPC_SC -> EXTINT = 1;

}
