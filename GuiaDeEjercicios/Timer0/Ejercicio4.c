#include "LPC17xx.h"

#define BOTON (1 << 0)
#define EXIT (1 << 28)

void config();
void config_TMR0();

void TIMER0_IRQHandler();
void EINT3_IRQHandler();

uint32_t contador = 5;
uint32_t periodoCompleto = 5000; // Va a ser 5s arriba y 5s abajo
uint32_t periodoVariable = 2500;

int main(){

    config();
    config_TMR0();

    NVIC_EnableIRQ(EINT3_IRQn);

    while(1){

    }

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

void config_TMR0(){

    LPC_SC -> PCONP |= (1 << 1);
    LPC_SC -> PCLKSEL0 &= ~(3 << 2);

    LPC_TIM0 -> PR = 25000;

    LPC_TIM0 -> IR |= (1 << 0);
    LPC_TIM0 -> TCR &= ~(1 << 1);
    LPC_TIM0 -> CTCR &= ~(3 << 0);

    LPC_TIM0 -> MCR |= (1 << 0);
    LPC_TIM0 -> MCR |= (1 << 1);
    LPC_TIM0 -> MCR &= ~(1 << 2);

    LPC_TIM0 -> MCR &= ~(1 << 3);
    LPC_TIM0 -> MCR &= ~(1 << 4); // deshabilito reset en MR1
    LPC_TIM0 -> MCR &= ~(1 << 5);

    LPC_TIM0 -> EMR &= ~(3 << 4);
    LPC_TIM0 -> EMR |= (1 << 5); // Cuando MR0 interrumpe, pin en alto

    LPC_TIM0 -> EMR &= ~(3 << 6);
    LPC_TIM0 -> EMR |= (1 << 6); // Cunado MR1 interrumpe, pin en bajo

    LPC_TIM0 -> MR0 = periodoCompleto;
    LPC_TIM0 -> MR1 = periodoVariable;

    LPC_TIM0 -> TCR |= (1 << 0);

    NVIC_EnableIRQ(TIMER0_IRQn);
    return;

}

void TIMER0_IRQHandler(){

    LPC_TIM0 -> IR |= (1 << 0);

}

void EINT3_IRQHandler(){

    if(contador >= 10){
        contador = 0;
        periodoVariable = 0;
        LPC_TIM0 -> MR1 = periodoVariable;
    }else{
        contador++;
        periodoVariable = periodoVariable + 500;
        LPC_TIM0 -> MR1 = periodoVariable;
    }

    LPC_GPIOINT -> IO0IntClr |= BOTON;
    return;

}
