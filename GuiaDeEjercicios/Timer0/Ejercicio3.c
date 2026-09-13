#include "LPC17xx.h"

#define BOTON (1 << 0) // GPIO Interrupt
#define salida (1 << 28) // Salida MAT0.0

void config();
void config_TMR();

void TIMER0_IRQHandler();
void EINT3_IRQHandler();

uint32_t contador = 0;
uint32_t PRC = 500;

int main(){

    config();
    config_TMR();

    NVIC_EnableIRQ(EINT3_IRQn);

    while (1){}
    
}

void config(){

    LPC_PINCON -> PINSEL0 &= ~(3 << 0);
    LPC_PINCON -> PINSEL3 &= ~(3 << 24);
    LPC_PINCON -> PINSEL3 |= (3 << 24);

    LPC_PINCON -> PINMODE0 &= ~(3 << 0);
    LPC_PINCON -> PINMODE0 |= (3 << 0);

    LPC_GPIO0 -> FIODIR &= ~BOTON;

    LPC_GPIOINT -> IO0IntEnR |= BOTON;

    return;

}

void config_TMR(){

    LPC_SC -> PCONP |= (1 << 1);
    LPC_SC -> PCLKSEL0 &= ~(3 << 2);

    LPC_TIM0 -> IR |= (1 << 0);

    LPC_TIM0 -> TCR &= ~(1 << 1);
    LPC_TIM0 -> CTCR &= ~(3 << 0);

    LPC_TIM0 -> PR = 25000; 
    
    LPC_TIM0 -> MR0 = PRC;
    LPC_TIM0 -> MCR |= (1 << 0);
    LPC_TIM0 -> MCR |= (1 << 1);
    LPC_TIM0 -> MCR &= ~(1 << 2);

    LPC_TIM0 -> EMR &= ~(3 << 4);
    LPC_TIM0 -> EMR |= (3 << 4);

    LPC_TIM0 -> TCR |= (1 << 0);

    NVIC_EnableIRQ(TIMER0_IRQn);
    return;

}

void TIMER0_IRQHandler(){

    LPC_TIM0 -> IR |= (1 << 0);
    return;

}

void EINT3_IRQHandler(){

    if(contador >= 10){
        contador = 0;
        PRC = 500;
        LPC_TIM0 -> MR0 = PRC;
    }else{
        contador++;
        PRC = PRC/2;
        LPC_TIM0 -> MR0 = PRC;
    }

    LPC_GPIOINT -> IO0IntClr |= BOTON;
    return;

}