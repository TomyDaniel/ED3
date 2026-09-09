#include "LPC17xx.h"

/*
Enunciado del ejercicio

Implementar en LPC1769, usando CMSIS y programacion por registros:
1. Configurar PINSEL y PINMODE de todos los pines usados.
2. Configurar GPIO para un semaforo de autos (rojo, amarillo, verde) y un LED peatonal.
3. Configurar SysTick como base de tiempo (tick fijo de 10 ms).
4. Configurar un pulsador en P2.11 como EINT1 por flanco de subida.
5. Configurar un switch en P0.9 con interrupcion GPIO por ambos flancos.
6. Estado normal: autos en verde y peaton en rojo.
7. Al pulsar EINT1: ejecutar secuencia peatonal:
   - amarillo autos intermitente
   - rojo autos + verde peaton
   - volver a estado normal
8. El switch P0.9 selecciona perfil de tiempos:
   - nivel alto: perfil rapido
   - nivel bajo: perfil lento
9. No usar delays bloqueantes; todo por SysTick + ISR.

Nota: LEDs activos por bajo (0 enciende, 1 apaga).
*/

static uint32_t crossing_request = 0U; // volatile
static volatile uint32_t timing_fast = 1U;
static volatile uint32_t phase = 0U;
static volatile uint32_t phase_ticks = 0U;
static uint32_t blink_ticks = 0U; //volatile
static volatile uint32_t yellow_on = 0U;

void conf_pcb(void);
void conf_sysTick(void);
void conf_eint(void);

static void applyTimingFromSwitch(void);
static void showNormalState(void);
static void showYellowBlinkState(void);
static void showPedestrianCrossState(void);

// Falta systick y el handler del EINT

int main(void) {
    conf_pcb();
    conf_sysTick();

    applyTimingFromSwitch();
    showNormalState();

    while (1) {
        __WFI();
    }

    return 0;
}

void conf_pcb(void) {
    LPC_GPIO0->FIODIR |= (1UL << 22);
    LPC_GPIO3->FIODIR |= (1UL << 25);
    LPC_GPIO3->FIODIR |= (1UL << 27); // 26
    LPC_GPIO2->FIODIR |= (1UL << 0);

    LPC_PINCON->PINSEL1 &= ~(3UL << 12);
    LPC_PINCON->PINSEL7 &= ~(3UL << 18);
    LPC_PINCON->PINSEL7 &= ~(3UL << 20);
    LPC_PINCON->PINSEL4 &= ~(3UL << 0); //mal

    LPC_PINCON->PINSEL0 &= ~(3UL << 18);
    LPC_GPIO0->FIODIR |= (1UL << 9); // mal esot, deberia se entrada

    LPC_PINCON->PINSEL4 &= ~(3UL << 22);
    LPC_PINCON->PINSEL4 |= (2UL << 22); // mal

    LPC_PINCON->PINMODE0 &= ~(3UL << 18);
    LPC_PINCON->PINMODE4 &= ~(3UL << 22); // Flanco de subida, aca esta para flanco de bajada
    LPC_PINCON->PINMODE1 &= ~(3UL << 12); // deberia ser flotante
    LPC_PINCON->PINMODE7 &= ~(3UL << 18); // flotante
    LPC_PINCON->PINMODE7 &= ~(3UL << 20); // flotante
    LPC_PINCON->PINMODE4 &= ~(3UL << 0);

    LPC_GPIO2->FIODIR |= (1UL << 11);

    LPC_GPIO0->FIOSET = (1UL << 22);
    LPC_GPIO3->FIOSET = (1UL << 25);
    LPC_GPIO3->FIOSET = (1UL << 26);
    LPC_GPIO2->FIOSET = (1UL << 0);
}

void conf_sysTick(void) {
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL = 0U;
    SysTick->CTRL = (1UL << 2) |
                    (1UL << 0); // Falta poner en uno un registro
}

void conf_eint(void) {
    LPC_SC->EXTMODE |= (1UL << 1); 
    LPC_SC->EXTPOLAR &= ~(1UL << 1); // deberia ser flanco de subida
    LPC_SC->EXTINT = (1UL << 1);

    LPC_GPIOINT->IO0IntClr = (1UL << 9);
    LPC_GPIOINT->IO0IntEnR |= (1UL << 9);
    LPC_GPIOINT->IO0IntEnF |= (1UL << 9);

    NVIC_ClearPendingIRQ(EINT1_IRQn);
    NVIC_ClearPendingIRQ(EINT3_IRQn);
    NVIC_EnableIRQ(EINT2_IRQn); // es eint1
    NVIC_EnableIRQ(EINT3_IRQn);
}

void SysTick_Handler(void) {
    uint32_t yellow_total_ticks;
    uint32_t yellow_toggle_ticks;
    uint32_t ped_cross_ticks;

    if (timing_fast) {
        yellow_total_ticks = 120U;
        yellow_toggle_ticks = 10U;
        ped_cross_ticks = 180U;
    } else {
        yellow_total_ticks = 300U;
        yellow_toggle_ticks = 25U;
        ped_cross_ticks = 500U;
    }

    if (phase == 0U) {
        if (crossing_request) {
            crossing_request = 0U;
            phase = 1U;
            phase_ticks = 0U;
            blink_ticks = 0U;
            yellow_on = 0U;
            showYellowBlinkState();
        }
        return;
    }

    if (phase == 1U) {
        phase_ticks++;
        blink_ticks++;

        if (blink_ticks >= yellow_toggle_ticks) {
            blink_ticks = 0U;
            yellow_on ^= 1U;
            showYellowBlinkState();
        }

        if (phase_ticks >= yellow_total_ticks) {
            phase = 2U;
            phase_ticks = 0U;
            showPedestrianCrossState();
        }
        return;
    }

    if (phase == 2U) {
        phase_ticks++;

        if (phase_ticks >= ped_cross_ticks) {
            phase = 0U;
            phase_ticks = 0U;
            showNormalState();
        }
    }
}

void EINT3_IRQHandler(void) {
    if (LPC_GPIOINT->IO0IntStatR & (1UL << 9)) {
        LPC_GPIOINT->IO0IntClr = (1UL << 9);
    }

    if (LPC_GPIOINT->IO0IntStatF & (1UL << 9)) {
        LPC_GPIOINT->IO0IntClr = (1UL << 9);
    }

    applyTimingFromSwitch();
}

void EINT1_IRQHandler(void) {
    LPC_SC->EXTINT = (1UL << 1);
    crossing_request = 1U;
}

static void applyTimingFromSwitch(void) {
    if ((LPC_GPIO0->FIOPIN & (1UL << 8)) != 0U) {
        timing_fast = 1U;
    } else {
        timing_fast = 0U;
    }
}

static void showNormalState(void) {
    LPC_GPIO0->FIOSET = (1UL << 22);
    LPC_GPIO3->FIOSET = (1UL << 25);
    LPC_GPIO3->FIOCLR = (1UL << 26);
    LPC_GPIO2->FIOSET = (1UL << 0);
}

static void showYellowBlinkState(void) {
    LPC_GPIO0->FIOSET = (1UL << 22);
    LPC_GPIO3->FIOSET = (1UL << 26);
    LPC_GPIO2->FIOSET = (1UL << 0);

    if (yellow_on) {
        LPC_GPIO3->FIOCLR = (1UL << 25);
    } else {
        LPC_GPIO3->FIOSET = (1UL << 25);
    }
}

static void showPedestrianCrossState(void) {
    LPC_GPIO0->FIOCLR = (1UL << 22);
    LPC_GPIO3->FIOSET = (1UL << 25);
    LPC_GPIO3->FIOSET = (1UL << 26);
    LPC_GPIO2->FIOCLR = (1UL << 0);
}
