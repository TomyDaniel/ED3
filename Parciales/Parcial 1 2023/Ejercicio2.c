#include "lpc17xx.h"

#define Entrada1 (1 << 0)
#define Entrada2 (1 << 1)
#define Entrada3 (1 << 2)
#define Entrada4 (1 << 3)
#define MAX_PRIORITY 0
#define MIN_PRIORITY 31

uint8_t numeros[10];
volatile uint8_t contador = 0;

void configPIN(void);
void EINT3_IRQHandler(void);
void config_GPIO_Interrupt(void);

int main(void){
    
    SystemInit();

    configPIN();

    config_GPIO_Interrupt();

    NVIC_SetPriority(EINT3_IRQn, MAX_PRIORITY);

    while(1){
        __WFI();
    }
}

void configPIN(void){
    // Entradas
    LPC_PINCON->PINSEL4 &= ~(0xFF << 0); // ... 1111 1111 1111 1111 0000 0000

    LPC_GPIO2->FIODIR &= ~0xF;

    LPC_PINCON->PINMODE4 = (LPC_PINCON->PINMODE4 & ~0xFF) | 0xAA; // ... 1111 1111 1111 1111 1010 1010
}

void EINT3_IRQHandler(void){

    LPC_GPIOINT->IO2IntClr |= 0xF;

    uint8_t numeroLeido = (LPC_GPIO2->FIOPIN & 0x0F);

    for(int i=8; i>=0; i--){

        numeros[i + 1] = numeros[i];

    }

    numeros[0] = numeroLeido;

    contador++;

    if(contador >= 200){
        contador = 0;

        uint32_t prioridadActual = NVIC_GetPriority(EINT3_IRQn);
        
        if (prioridadActual >= MIN_PRIORITY) {
            LPC_GPIOINT->IO2IntEnF &= ~0xF;
            NVIC_DisableIRQ(EINT3_IRQn);
        } else {
            uint32_t nuevaPrioridad = prioridadActual + 1;
            NVIC_SetPriority(EINT3_IRQn, nuevaPrioridad);
        }
    }

}

void config_GPIO_Interrupt(void){

    LPC_GPIOINT->IO2IntEnF |= 0xF; // Interrupciones para el puerto 2. Flanco de bajada

    NVIC_EnableIRQ(EINT3_IRQn);
}