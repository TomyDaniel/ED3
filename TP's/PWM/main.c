/*
    Usar SysTick para controlar un led y EINT0.
    Usar PWM por software para controlar la intensidad luminosa del led
    Mediante EINT gestionar el duty cycle e interrupciones por GPIO para aumentar o disminuir el brillo del LED
*/

#include "LPC17xx.h"

#define ticks (0.001 * 100000000 - 1) // 1ms 
#define LED (1 << 22)
#define BOTONEINT (1 << 10)
#define BOTONGPIO (1 << 0)

static uint32_t periodoPWM = 10; // total 10ms
static uint32_t contadorPWM = 0;
static uint32_t tiempoEnAlto = 0;
static uint32_t contador = 0;

void config();
void SysTick_Handler();
void EINT0_IRQHandler();
void EINT3_IRQHandler();

int main(){

    config();

    SysTick_Config(ticks);

    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);

    while(1){}

}

void config(){

    // Configuracion LED

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);
    LPC_PINCON -> PINMODE1 &= ~(3 << 12);
    LPC_PINCON -> PINMODE1 |= (1 << 13);
    LPC_GPIO0 -> FIODIR |= LED;
    LPC_GPIO0 -> FIOSET = LED;

    // Configuracion boton EINT

    LPC_PINCON -> PINSEL4 &= ~(3 << 20);
    LPC_PINCON -> PINSEL4 |= (1 << 20);
    LPC_PINCON -> PINMODE4 &= ~(3 << 20);
    LPC_PINCON -> PINMODE4 |= (3 << 20);
    LPC_GPIO2 -> FIODIR &= ~BOTONEINT;
    LPC_SC -> EXTMODE |= (1 << 0);
    LPC_SC -> EXTPOLAR |= (1 << 0);

    // Configuracion boton GPIO

    LPC_PINCON -> PINSEL0 &= ~(3 << 0);
    LPC_PINCON -> PINMODE0 &= ~(3 << 0);
    LPC_PINCON -> PINMODE0 |= (3 << 0);
    LPC_GPIO0 -> FIODIR &= ~BOTONGPIO;
    LPC_GPIOINT -> IO0IntEnR |= BOTONGPIO;

}

void SysTick_Handler(){

    contadorPWM++;
    if(contadorPWM <= tiempoEnAlto){
        LPC_GPIO0 -> FIOCLR = LED;
    }
    else if (contadorPWM > tiempoEnAlto && contadorPWM <= periodoPWM){
        LPC_GPIO0 -> FIOSET = LED;
    }
    else {
        contadorPWM = 0;
    }

}

void EINT0_IRQHandler(){

    if(tiempoEnAlto > periodoPWM){
        tiempoEnAlto = 10;
    }else{
        tiempoEnAlto++;
    }

    LPC_SC -> EXTINT = (1 << 0);

}

void EINT3_IRQHandler(){
    
    if(tiempoEnAlto < 0){
        tiempoEnAlto = 0;
    }else{
        tiempoEnAlto--;
    }

    LPC_GPIOINT -> IO0IntClr = BOTONGPIO;

}
