// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 1000000

#define LED1 (1 << 19)

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    while(1) {

        LPC_GPIO1->FIOSET = LED1;
        delay(500); //ms
        LPC_GPIO1->FIOCLR = LED1;
        delay(500);

    }

    return 0 ;
}

void delay(int cantidad){
	LPC_TIM0->MR0 = cantidad * 1000;

    // 2. Configurar el Match Control Register (MCR) para que el timer se detenga
    // y genere una bandera de interrupción cuando TC == MR0.
    // Bit 0 (Interrupt on MR0): Genera la bandera.
    // Bit 2 (Stop on MR0): Detiene el conteo.
    LPC_TIM0->MCR = (1 << 0) | (1 << 2);
    
    // 3. Resetear el contador y habilitar el timer.
    LPC_TIM0->TCR = (1 << 1); // Reset
    LPC_TIM0->TCR = (1 << 0); // Habilitar

    // 4. Esperar hasta que la bandera de interrupción del Match 0 se active.
    // El bit 0 del registro TIR (Timer Interrupt Register) se pondrá a 1.
    while (!(LPC_TIM0->IR & (1 << 0)));

    // 5. Limpiar la bandera de interrupción para el próximo uso.
    LPC_TIM0->IR |= (1 << 0);
}


void configuraciones(void){

    LPC_SC->PCONP |= (1 << 1);
    LPC_SC->PCONP |= (1 << 15);

    LPC_SC->PCLKSEL0 &= ~(3 << 2);
    LPC_SC->PCLKSEL0 |= (1 << 2);

    LPC_TIM0->TCR = 0;
    LPC_TIM0->PR = 24;
    LPC_TIM0->TCR |= (1 << 1);
    LPC_TIM0->TCR &= ~(1 << 1);

    LPC_PINCON->PINSEL3 &= ~(3 << 6);
    LPC_GPIO1->FIODIR |= LED1;
    LPC_GPIO1->FIOCLR = LED1;
}
