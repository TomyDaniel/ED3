#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"

#define ticks (0.1 * 100000000) // 100 ms
#define mascara (0x7F << 0)

void config(void);
void SysTick_Handler(void);

uint8_t Display[] = {
    0x7E, // 0
    0x30, // 1
    0x6D, // 2
    0x79, // 3
    0x33, // 4
    0x5B, // 5
    0x5F, // 6
    0x70, // 7
    0x7F, // 8
    0x7B  // 9
};

uint8_t contador = 10;
uint8_t valor = 0;

int main(){

    config();

    SysTick_Config(ticks);

    while(1){}

}

void config(){

    // DISPLAY

    PINSEL_CFG_T display;

    display.port = 0;
    display.pin = 0;
    display.func = 0; // Funcion 0
    display.mode = 2; // No pull-down, no pull-up

    PINSEL_ConfigMultiplePins(&display, mascara);

    GPIO_SetDir(0, mascara, 1); // Puerto 0, mascara de bits, salida
    GPIO_SetPins(0, Display[valor]);

}

void SysTick_Handler(){

    contador--;
    if(contador == 0){

        if(valor <= 8){
            valor++;  
        }else{
            valor = 0;
        }

        GPIO_ClearPins(0, mascara);

        GPIO_SetPins(0, Display[valor]);
        
        contador = 10;
    }

    return;

}
