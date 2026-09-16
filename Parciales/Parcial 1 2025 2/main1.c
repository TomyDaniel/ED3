#include "LPC17xx.h"

#define PINE2 (1 << 12)
#define exit (1 << 4)
#define ticks (0.01 * 60000000 - 1) // 10 ms

void config();
void config_sys();
void SysTick_Handler();
void EINT2_IRQHandler();

uint8_t pulso = 0; // Para saber si ya se pulso o no el EINT
uint8_t secuencia[15] = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0};
uint8_t i = 0; // Para recorrer el array


int main(){

    config();
    config_sys();

    NVIC_EnableIRQ(EINT2_IRQn);

    while(1){}

}

void config(){

    // Configuracion salida
    LPC_PINCON -> PINSEL4 &= ~(3 << 8); 
    LPC_PINCON -> PINMODE4 &= ~(3 << 8);
    LPC_PINCON -> PINMODE4 |= (1 << 9);
    LPC_GPIO2 -> FIODIR |= exit;
    LPC_GPIO2 -> FIOCLR = exit;

    // Configuracion EINT
    LPC_PINCON -> PINSEL4 &= ~(3 << 24);
    LPC_PINCON -> PINSEL4 |= (1 << 24); // Se pone como entrada por esta en EINT
    LPC_PINCON -> PINMODE4 &= ~(3 << 24); // Activo pull-up

    LPC_SC -> EXTMODE |= (1 << 2); // Por flanco
    LPC_SC -> EXTPOLAR &= ~(1 << 2); // Descendente
    LPC_SC -> EXTINT |= (1 << 2); // Limpio flag por residuos

    return;

}

void config_sys(){

    SysTick -> LOAD = ticks;
    SysTick -> VAL = 0;
    SysTick -> CTRL |= (1 << 1) | (1 << 2); // Supongo clock de 60 MHz interno

    return;

}

void EINT2_IRQHandler(){

    if(pulso == 0){
        pulso = 1;
        SysTick -> CTRL |= (1 << 0);
    }else{
        pulso = 0;
        LPC_GPIO2 -> FIOSET = exit; // Pongo un 1 a la salida
        SysTick -> CTRL &= ~(1 << 0); // Doy por finalizada la secuencia
    }

    LPC_SC -> EXTINT |= (1 << 2);

    return;

}

void SysTick_Handler(){

    if(i <= 14){ // Va de 0 a 14. 15 valores
        if(secuencia[i] == 0){
            LPC_GPIO2 -> FIOCLR = exit;
            i++; // Sumo y paso al siguiente valor del array
        }else{
            LPC_GPIO2 -> FIOSET = exit;
            i++; // Sumo y paso al siguiente valor del array
        }
    }else{
        SysTick -> CTRL &= ~(1 << 0); // Apago y doy fin a la secuencia
        i=0;
        pulso = 0; // Por si la secuencia termino
    }

    return;

}
