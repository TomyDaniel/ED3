#include "LPC17xx.h"

#define LED (1 << 22)
#define BOTONEINT (1 << 10)
#define BOTONGPIO (1 << 0)

void config();
void configTMR0();
void EINT0_IRQHandler();
void EINT3_IRQHandler();

int main(){

    config();
    configTMR0();

    while(1){}

}

void config(){

    // Configuracion LED

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);
    LPC_PINCON -> PINMODE1 &= ~(3 << 12);
    LPC_PINCON -> PINMODE1 |= (1 << 13);
    LPC_GPIO0 -> FIODIR |= LED;
    LPC_GPIO0 -> FIOSET = LED;

    // Configuracion Boton EINT

    LPC_PINCON -> PINSEL4 &= ~(3 << 20);
    LPC_PINCON -> PINSEL4 |= (1 << 21);
    LPC_PINCON -> PINMODE1 &= (3 << 20);
    LPC_PINCON -> PINMODE1 |= (3 << 20);
    LPC_GPIO2 -> FIODIR |= BOTONEINT;
    LPC_SC -> EXTMODE |= (1 << 0);
    LPC_SC -> EXTPOLAR |= (1 << 0);

    // Configuracion Boton GPIO

    LPC_PINCON -> PINSEL0 &= ~(3 << 0);
    LPC_PINCON -> PINMODE0 &= ~(3 << 0);
    LPC_PINCON -> PINMODE0 |= (3 << 0);
    LPC_GPIO0 -> FIODIR |= BOTONGPIO;
    LPC_GPIOINT -> IO0IntEnR |= BOTONGPIO;

    return;

}

void configTMR0(){

    LPC_SC -> PCONP |= (1 << 1); // Encendido TMR0
    LPC_SC -> PCLKSEL0 &= ~(0x03 << 2); // PCLK_TIMER0 = CCLK / 4
    LPC_TIM0 -> CTCR &= ~(3 << 0); // Modo de timer
    LPC_TIM0 -> PR = 25000; // Valor maximo para PC
    LPC_TIM0 -> TCR |= (1 << 1);
    //Deshabilitación Reset del TIMER0: (Counter Reset=0)
    LPC_TIM0->TCR &= ~(1<<1);
    //Limpia Bandera de Interrupción por MATCH0
    LPC_TIM0->IR |= (1<<0);

    //Valor Máximo para TC MRO0
    LPC_TIM0->MR0 = 9;
    //Habilitación de Interrupción por MATCH: (MR0I=1)
    LPC_TIM0->MCR |= (1<<0);
    //Habilitación de Reset por MATCH: (MR0R=1)
    LPC_TIM0->MCR |= (1<<1);
    //Deshabilitación de Stop por MATCH: (MR0S=0)
    LPC_TIM0->MCR &= ~(1<<2);

    //Valor maximo para TC MR1
    LPC_TIM0 -> MR1 = 9;

    LPC_TIM0 -> MCR |= (1 << 3);

    LPC_TIM0 -> MCR |= (1 << 4);

    LPC_TIM0 -> MCR &= ~(1 << 5);

    //Habilitación del TIMER0: (Counter Enable=1)
    LPC_TIM0->TCR |= (1<<0);

    //Habilitación de Interrupción por TIMER0
    NVIC_EnableIRQ(TIMER0_IRQn);

    return;
}

void TIMER0_IRQHandler(){

    if(LPC_TIM0 -> IR & (1 << 0)){
        if(LPC_GPIO0 -> FIOPIN & LED){
            LPC_GPIO0 -> FIOCLR = LED;
        }else{
            LPC_GPIO0 -> FIOSET = LED;
        }
    }

    if(LPC_TIM0 -> IR & (1 << 1)){
        if(LPC_GPIO0 -> FIOPIN & LED){
            LPC_GPIO0 -> FIOCLR = LED;
        }else{
            LPC_GPIO0 -> FIOSET = LED;
        }
    }

}

void EINT0_IRQHandler(){

    if(LPC_TIM0 -> MR0 == 9){
        LPC_TIM0 -> MR0 = 9;
    }else{
        LPC_TIM0 -> MR0++;
    }

    LPC_SC -> EXTINT = (1 << 0);

}

void EINT3_IRQHandler(){

    if(LPC_TIM0 -> MR0 == 0){
        LPC_TIM0 -> MR0 = 0;
    }else{
        LPC_TIM0 -> MR0--;
    }

    LPC_GPIOINT -> IO0IntClr = BOTONGPIO;

}
