/*
Implementar en LPC1769, usando CMSIS y programación por registros, un sistema RGB con interrupciones.

Requisitos:
Pulsador en P2.10 como EINT0, disparado por flanco de subida.
Cada pulsación cambia el color en ciclo: rojo → azul → verde → rojo.
Switch en P0.8 con interrupción GPIO en ambos flancos.
El switch selecciona período de parpadeo del color activo: 10 ms o 100 ms.
El parpadeo debe hacerse con SysTick.
*/

#include "LPC17xx.h"

static volatile uint32_t current_color = 0U;
static uint32_t led_visible = 1U; // error, debe ser volatile

static void renderCurrentColorIfVisible(void);
/* falta llamar a las funciones */

int main(void) {
    conf_pcb();
    conf_sysTick();
    // falta llamar funciones

    LPC_GPIO0->FIOCLR = (1UL << 22);

    while (1) {
        __WFI();
    }

    return 0;
}

void conf_pcb(void) {
    
    LPC_GPIO0->FIODIR |= (1UL << 22);
    LPC_GPIO3->FIODIR |= (1UL << 25);
    LPC_GPIO3->FIODIR |= (1UL << 27); // incorrecto, es el 26

    LPC_PINCON->PINSEL1 &= ~(3UL << 12);
    LPC_PINCON->PINSEL7 &= ~(3UL << 18);
    LPC_PINCON->PINSEL7 &= ~(3UL << 20);

    LPC_PINCON->PINSEL0 &= ~(3UL << 16); 
    LPC_GPIO0->FIODIR |= (1UL << 8); // Error, salida y debe ser entrada

    LPC_PINCON->PINSEL4 &= ~(3UL << 20);
    LPC_PINCON->PINSEL4 |= (1UL << 22); //error, deberia ser 20
    LPC_GPIO2->FIODIR &= ~(1UL << 10);

    LPC_GPIO0->FIOSET = (1UL << 22); /* Error, acitvo por bajo */
    LPC_GPIO3->FIOSET = (1UL << 25);
    LPC_GPIO3->FIOSET = (1UL << 26);
}

void conf_sysTick(void) {
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U; 
    SysTick->VAL = 0U;
    SysTick->CTRL = (1UL << 2) |
                    (1UL << 0); // mal aca, el bit0 ok, pero el bit1 no esta configurado, y necesita un uno
}

void conf_eint(void) {
    LPC_SC->EXTMODE |= (1UL << 0);
    LPC_SC->EXTPOLAR &= ~(1UL << 0); // Flanco de bajada, error
    LPC_SC->EXTINT = (1UL << 0);

    LPC_GPIOINT->IO2IntClr = (1UL << 8); // Error, es IO0
    LPC_GPIOINT->IO2IntEnR |= (1UL << 8);
    LPC_GPIOINT->IO2IntEnF |= (1UL << 8);

    NVIC_ClearPendingIRQ(EINT0_IRQn);
    NVIC_ClearPendingIRQ(EINT3_IRQn);
    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);
}

void SysTick_Handler(void) {
    led_visible ^= 1U; 
    renderCurrentColorIfVisible();
}

void EINT3_IRQHandler(void) {
    if (LPC_GPIOINT->IO0IntStatR & (1UL << 8)) {
        LPC_GPIOINT->IO0IntClr = (1UL << 8);
    }

    if (LPC_GPIOINT->IO0IntStatF & (1UL << 8)) {
        LPC_GPIOINT->IO0IntClr = (1UL << 8);
    }

    if ((LPC_GPIO0->FIOPIN & (1UL << 8)) != 0U) {
        SysTick->LOAD = (SystemCoreClock / 100U) - 1U;
    } else {
        SysTick->LOAD = (SystemCoreClock / 10U) - 1U; // mal, deberia ser 1000
    }

    SysTick->VAL = 0U;
}

void EINT1_IRQHandler(void) { // Mal handler
    LPC_SC->EXTINT = (1UL << 0);

    current_color = (current_color + 1U) % 3U;
    renderCurrentColorIfVisible();
}

static void renderCurrentColorIfVisible(void) {
    LPC_GPIO0->FIOSET = (1UL << 22);
    LPC_GPIO3->FIOSET = (1UL << 25);
    LPC_GPIO3->FIOSET = (1UL << 26);

    if (!led_visible) {
        return;
    }

    if (current_color == 0U) {
        LPC_GPIO0->FIOCLR = (1UL << 22);
    } else if (current_color == 1U) {
        LPC_GPIO3->FIOCLR = (1UL << 26);
    } else {
        LPC_GPIO3->FIOCLR = (1UL << 25);
    }
}
