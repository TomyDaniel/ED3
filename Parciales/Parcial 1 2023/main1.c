#include "LPC17xx.h"

#define PINS (0x7F << 0)
#define PEINT0 (1 << 10)
#define PEINT1 (1 << 11)
#define PEINT2 (1 << 12)

#define ticks (0.001 * 16000000 - 1)

static uint32_t contador = 0;
static uint32_t tiempo = 1000;
int i = 0;
int estado = 0;

void config();
void SysTick_Handler();
void EINT0_IRQHandler();
void EINT1_IRQHandler();
void EINT2_IRQHandler();

int numeros[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};

int main(){

    config();

    SysTick_Config(ticks);

    contador = tiempo;

    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT1_IRQn);
    NVIC_EnableIRQ(EINT2_IRQn);

    while(1){

    }

}

void config(){

    // Configuracion Pines
    LPC_PINCON -> PINSEL0 &= ~(0x3FFF << 0);
    LPC_PINCON -> PINMODE0 &= ~(0x3FFF << 0);
    LPC_PINCON -> PINMODE0 |= (0x2AAA << 0);

    LPC_GPIO0 -> FIODIR |= PINS;
    LPC_GPIO0 -> FIOSET = numeros[0];

    // Configuracion P2.10
    LPC_PINCON -> PINSEL4 &= ~(3 << 20);
    LPC_PINCON -> PINSEL4 |= (1 << 20);

    LPC_PINCON -> PINMODE4 &= ~(3 << 20);
    LPC_PINCON -> PINMODE4 |= (3 << 20);

    LPC_SC -> EXTMODE |= (1 << 0);
    LPC_SC -> EXTPOLAR |= (1 << 0);

    // Configuracion P2.11
    LPC_PINCON -> PINSEL4 &= ~(3 << 22);
    LPC_PINCON -> PINSEL4 |= (1 << 22);

    LPC_PINCON -> PINMODE4 &= ~(3 << 22);
    LPC_PINCON -> PINMODE4 |= (3 << 22);

    LPC_SC -> EXTMODE |= (1 << 1);
    LPC_SC -> EXTPOLAR |= (1 << 1);

    // Configuracion P2.12
    LPC_PINCON -> PINSEL4 &= ~(3 << 24);
    LPC_PINCON -> PINSEL4 |= (1 << 24);

    LPC_PINCON -> PINMODE4 &= ~(3 << 24);
    LPC_PINCON -> PINMODE4 |= (3 << 24);

    LPC_SC -> EXTMODE |= (1 << 2);
    LPC_SC -> EXTPOLAR |= (1 << 2);

}

void SysTick_Handler(){

    contador--;
    if(contador == 0){
        LPC_GPIO0 -> FIOCLR = PINS;
        if(i <= 9){
            LPC_GPIO0 -> FIOSET = numeros[i];
            i++;
        }else{
            i=0;
            LPC_GPIO0 -> FIOSET = numeros[i];
        }
        contador = tiempo;
    }

}

void EINT0_IRQHandler(){

    i = 0;
    LPC_GPIO0 -> FIOCLR = PINS;
    LPC_GPIO0 -> FIOSET = numeros[i];

    LPC_SC -> EXTINT |= (1 << 0);

}

void EINT1_IRQHandler(){

    if(estado == 0){
        estado = 1;
        SysTick -> CTRL &= ~(1 << 0);
    }else{
        estado = 0;
        SysTick -> CTRL |= (1 << 0);
    }

    LPC_SC -> EXTINT |= (1 << 1);

}

void EINT2_IRQHandler(){

    if(tiempo == 1000){
        tiempo = 1;
    }else{
        tiempo = 1000;
    }
    
    LPC_SC -> EXTINT |= (1 << 2);

}