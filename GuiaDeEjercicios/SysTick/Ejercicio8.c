#include "LPC17xx.h"

#define ticks (0.1 * 100000000 - 1)

#define LEDROJOA (1 << 16)
#define LEDAMARILLOA (1 << 17)
#define LEDVERDEA (1 << 18)

#define LEDROJOB (1 << 19)
#define LEDAMARILLOB (1 << 20)
#define LEDVERDEB (1 << 21)

#define BOTON (1 << 0)

int countL = 250;
int countC = 0;

enum sem{
    ROJO_VERDE,
    ROJO_AMARILLO,
    VERDE_ROJO,
    AMARILLO_ROJO
};

enum sem semaforos = ROJO_VERDE;

void config();
void EINT3_IRQHandler();
void SysTick_Handler();

int main(){

    config();

    SysTick_Config(ticks);

    NVIC_EnableIRQ(EINT3_IRQn);

    while(1){}

}

void config(){

    // Configuracion LEDs

    LPC_PINCON -> PINSEL1 &= ~(0xFFF << 0);

    LPC_PINCON -> PINMODE1 &= ~(0xFFF << 0);

    LPC_PINCON -> PINMODE1 |= (0xAAA << 0);

    LPC_GPIO0 -> FIODIR |= (0x3F << 16);

    // Configuracion Boton

    LPC_PINCON -> PINSEL0 &= ~(3 << 0);

    LPC_PINCON -> PINMODE0 &= ~(3 << 0);

    LPC_PINCON -> PINMODE0 |= (3 << 0);

    LPC_GPIO0 -> FIODIR &= ~BOTON;

    LPC_GPIOINT -> IO0IntEnR |= BOTON;

}

void SysTick_Handler(){

    switch(semaforos){

        case ROJO_VERDE:

            if(countL <= 0){
                semaforos = ROJO_AMARILLO;
                countC = 50;
            }else{
                countL--;
            }

            break;
        
        case ROJO_AMARILLO:
            
            if(countC <= 0){
                semaforos = VERDE_ROJO;
                countL = 250;
            }else{
                countC--;
            }

            break;

        case AMARILLO_ROJO:

            if(countC <= 0){
                semaforos = ROJO_VERDE;
                countL = 250;
            }else{
                countC--;
            }

            break;

        case VERDE_ROJO:

            if(countL <= 0){
                semaforos = AMARILLO_ROJO;
                countC = 50;
            }else{
                countL--;
            }

            break;
    }

    switch(semaforos){
        case ROJO_VERDE:
            LPC_GPIO0 -> FIOCLR = (0x3F << 16); // Limpio todos los leds

            LPC_GPIO0 -> FIOSET = LEDROJOA;
            LPC_GPIO0 -> FIOSET = LEDVERDEB;

            break;

        case ROJO_AMARILLO:
            LPC_GPIO0 -> FIOCLR = (0x3F << 16); // Limpio todos los leds

            LPC_GPIO0 -> FIOSET = LEDROJOA;
            LPC_GPIO0 -> FIOSET = LEDAMARILLOB;

            break;

        case VERDE_ROJO:
            LPC_GPIO0 -> FIOCLR = (0x3F << 16); // Limpio todos los leds

            LPC_GPIO0 -> FIOSET = LEDVERDEA;
            LPC_GPIO0 -> FIOSET = LEDROJOB;

            break;

        case AMARILLO_ROJO:
            LPC_GPIO0 -> FIOCLR = (0x3F << 16); // Limpio todos los leds

            LPC_GPIO0 -> FIOSET = LEDAMARILLOA;
            LPC_GPIO0 -> FIOSET = LEDROJOB;

            break;
    } 

}

void EINT3_IRQHandler(){

    if(semaforos == ROJO_VERDE){
        semaforos = ROJO_AMARILLO;
        countC = 50;
    }
    if(semaforos == VERDE_ROJO){
        semaforos = AMARILLO_ROJO;
        countC = 50;
    }

    LPC_GPIOINT -> IO0IntClr = BOTON;

}
