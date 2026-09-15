#include "LPC17xx.h"

#define PIN1 (1 << 8)
#define PINE0 (1 << 10)
#define PINE1 (1 << 11)

#define ticks (0.01 * 65000000 - 1) // 10 ms con 65 MHz

static uint32_t contador = 0;
static uint32_t pausa = 8;
static uint32_t i = 0;

static uint8_t sen1 = 0;
static uint8_t sen2 = 0;
static uint8_t sen3 = 0;
static uint8_t sen4 = 0;

uint32_t *pDatos = (uint32_t *) 0x20080000;
uint32_t datos = *pDatos;
sen1 = (datos >> 0) & 0xFF;
sen2 = (datos >> 8) & 0xFF;
sen3 = (datos >> 16) & 0xFF;
sen4 = (datos >> 24) & 0xFF;

void config();
void EINT0_IRQHandler();
void EINT1_IRQHandler();
void SysTick_Handler();

int estado = 0;

enum caso{
    Senial1,
    Senial2,
    Senial3,
    Senial4
};

enum caso senial = Senial1;

int main(){

    config();

    SysTick_Config(ticks);

    NVIC_ClearPending(EINT0_IRQn);
    NVIC_ClearPending(EINT1_IRQn);

    NVIC_SetPriority(EINT0_IRQn, 3);
    NVIC_SetPriority(EINT1_IRQn, 2);

    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT1_IRQn);

    while(1){

    }

}

void config(){

    // Configuracion salida
    LPC_PINCON -> PINSEL4 &= ~(3 << 16);
    LPC_PINCON -> PINMODE4 &= ~(3 << 16);
    LPC_PINCON -> PINMODE4 |= (1 << 17);
    LPC_GPIO2 -> FIODIR |= PIN1;

    // Configuracion EINT0
    LPC_PINCON -> PINSEL4 &= ~(3 << 20);
    LPC_PINCON -> PINSEL4 |= (1 << 20);
    LPC_PINCON -> PINMODE4 &= ~(3 << 20);
    LPC_PINCON -> PINMODE4 |= (1 << 21);

    LPC_SC -> EXTPOLAR |= (1 << 0);
    LPC_SC -> EXTMODE |= (1 << 0);

    // Configuracion EINT1
    LPC_PINCON -> PINSEL4 &= ~(3 << 22);
    LPC_PINCON -> PINSEL4 |= (1 << 22);
    LPC_PINCON -> PINMODE4 &= ~(3 << 22);
    LPC_PINCON -> PINMODE4 |= (1 << 23);

    LPC_SC -> EXTPOLAR &= ~(1 << 1); 
    LPC_SC -> EXTMODE |= (1 << 1);

}

void SysTick_Handler(){
    
    if(contador > pausa){
        contador = 0;
        i=0;
    }else{
        LPC_GPIO2 -> FIOSET = (senial >> i);
        i++;
        contador++;
    }
}

void EINT0_IRQHandler(){

    LPC_SC -> EXTINT |= (1 << 0);

}

void EINT1_IRQHandler(){

    if(estado == 0){
        estado = 1;
        pausa = 16;
    }else{
        estado = 0;
        pausa = 8;
    }

    LPC_SC -> EXTINT |= (1 << 1);

}