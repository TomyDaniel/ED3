#include "LPC17xx.h"

void config();
void config_TMR();

void TIMER0_IRQHandler();

int main(){

    config();
    config_TMR();

    while(1){

    }

}

void config(){

    LPC_PINCON -> PINSEL3 &= ~(3 << 24);
    LPC_PINCON -> PINSEL3 |= (3 << 24); // Pin en MAT0.0, lo controla el TMR0

    return;

}

void config_TMR(){

    LPC_SC -> PCONP |= (1 << 1); // Enciende el TMR0, por default despues de un reset encendido
    LPC_SC -> PCLKSEL0 &= ~(3 << 2); // Pongo a CCLK en CCLK/4

    LPC_TIM0 -> CTCR &= ~(3 << 0); // Ponemos modo Timer. Table 429

    LPC_TIM0 -> PR = 25000; // CCLK/4 = 40ns y si lo multiplico po 25k da 1ms

    LPC_TIM0 -> TCR &= ~(1 << 1); // Habilito el reset
    LPC_TIM0 -> IR |= (1 << 0); // Limpio la bandera IR del MR0

    LPC_TIM0 -> MR0 = 999; // Le paso para que * 1ms de 1s de interrupcion

    LPC_TIM0 -> MCR |= (1 << 0); // Habilito interrupcion en MR0
    LPC_TIM0 -> MCR |= (1 << 1); // Habilito reset en MR0
    LPC_TIM0 -> MCR &= ~(1 << 2); // Deshabilito el stop en MR0

    LPC_TIM0 -> EMR &= ~(3 << 4);   // limpio el campo EMC0
    LPC_TIM0 -> EMR |= (3 << 4);    // 11 = toggle en Match0

    LPC_TIM0 -> TCR |= (1 << 0); // Counter enable

    NVIC_EnableIRQ(TIMER0_IRQn);

    return;

}

void TIMER0_IRQHandler(){

    LPC_TIM0 -> IR |= (1 << 0);
    return;

}