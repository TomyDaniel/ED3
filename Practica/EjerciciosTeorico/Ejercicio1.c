#include "LPC17xx.h"

#define LED_ROJO (1 << 22)
#define LED_AZUL (1 << 25)
#define LED_VERDE (1 << 26)

#define BUTTON (1 << 8)
#define Boton (1 << 10)

#define ticks10ms (1000000) // 0.01 * 100000000 - 1 = 999999
#define ticks100ms (10000000) // 0.1 * 100000000 - 1 = 9999999

void config();
void EINT0_IRQHandler();
void EINT3_IRQHandler();
void SysTick_Handler();

enum Estado{
    LedRojo,
    LedVerde,
    LedAzul
};

enum Estado color = LedRojo;

int main(){

    SysTick_Config(ticks10ms);

    config();

    NVIC_EnableIRQ(EINT3_IRQn);
    NVIC_EnableIRQ(EINT0_IRQn);

    while(1){

    }

}

void config(){

    /*
        LEDS
    */
    
    LPC_PINCON -> PINSEL1 &= ~(3 << 12);
    LPC_PINCON -> PINSEL6 &= ~(3 << 18);
    LPC_PINCON -> PINSEL6 &= ~(3 << 20);

    LPC_PINCON -> PINMODE1 &= ~(3 << 12);
    LPC_PINCON -> PINMODE1 |= (1 << 13);

    LPC_PINCON -> PINMODE6 &= ~(15 << 18);
    LPC_PINCON -> PINMODE6 |= (1 << 19);
    LPC_PINCON -> PINMODE6 |= (1 << 21);

    LPC_GPIO0 -> FIODIR |= LED_ROJO;
    LPC_GPIO3 -> FIODIR |= LED_AZUL;
    LPC_GPIO3 -> FIODIR |= LED_VERDE;

    LPC_GPIO0 -> FIOSET = LED_ROJO;
    LPC_GPIO3 -> FIOSET = LED_AZUL;
    LPC_GPIO3 -> FIOSET = LED_VERDE;

    /*
        Switch
    */
    LPC_PINCON -> PINSEL0 &= ~(3 << 8);

    LPC_PINCON -> PINMODE0 &= ~(3 << 8);
   
    LPC_GPIO0 -> FIODIR &= ~BUTTON;

    LPC_GPIOINT -> IO0IntEnR |= BUTTON;

    LPC_GPIOINT -> IO0IntEnF |= BUTTON;

    /*
        Boton 2.10 EINT0
    */

    LPC_PINCON -> PINSEL4 &= ~(3 << 20);

    LPC_PINCON -> PINSEL4 |= (1 << 20);

    LPC_PINCON -> PINMODE4 &= ~(3 << 20);

    LPC_GPIO2 -> FIODIR &= ~Boton;

    LPC_SC -> EXTMODE = 0;

    LPC_SC -> EXTPOLAR = 0; // Deberia activarse por bajo, el pin esta siempre en alto

}

void SysTick_Handler(){

    switch(color){
        case(LedRojo):
            if(LPC_GPIO0 -> FIOPIN & LED_ROJO){
                LPC_GPIO0 -> FIOCLR = LED_ROJO;
            }else{
                LPC_GPIO0 -> FIOSET = LED_ROJO;
            }
            break;
        case(LedVerde):
            if(LPC_GPIO3 -> FIOPIN & LED_VERDE){
                LPC_GPIO3 -> FIOCLR = LED_VERDE;
            }else{
                LPC_GPIO3 -> FIOSET = LED_VERDE;
            }
            break;
        case(LedAzul):
            if(LPC_GPIO3 -> FIOPIN & LED_AZUL){
                LPC_GPIO3 -> FIOCLR = LED_AZUL;
            }else{
                LPC_GPIO3 -> FIOSET = LED_AZUL;
            }
            break;
    }
    
}

void EINT3_IRQHandler(){

    if(LPC_GPIOINT -> IO0IntStatR & BUTTON){
        SysTick_Config(ticks100ms);

        LPC_GPIOINT -> IO0IntClr = BUTTON;

        return;
    }

    if(LPC_GPIOINT -> IO0IntStatF & BUTTON){
        SysTick_Config(ticks10ms);

        LPC_GPIOINT -> IO0IntClr = BUTTON;

        return;
    }

}

void EINT0_IRQHandler(){

    switch (color)
    {

    case LedRojo:
        LPC_GPIO0 -> FIOSET = LED_ROJO;
        color = LedAzul;
        break;
    
    case LedAzul:
        LPC_GPIO3 -> FIOSET = LED_AZUL;
        color = LedVerde;
        break;
    
    case LedVerde:
        LPC_GPIO3 -> FIOSET = LED_VERDE;
        color = LedRojo;
        break;

    }

    LPC_SC -> EXTINT = (1 << 0);

}
