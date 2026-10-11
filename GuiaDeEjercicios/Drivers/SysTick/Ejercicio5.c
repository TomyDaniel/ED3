#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"

#define ticks (0.1 * 100000000) // 100 ms
#define mascara (0x0F << 0)
#define LED (1 << 22)

uint8_t sec[] = {0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

void config(void);
void SysTick_Handler(void);

uint8_t contadorA = 5;
uint8_t contadorB = 2;

uint8_t valor = 0;

int main(){

    config();
    SysTick_Config(ticks);

    while(1){}

}

void config(){

    // LED's

    PINSEL_CFG_T leds;

    leds.port = 0;
    leds.pin = 0;
    leds.func = 0;
    leds.mode = 2;

    PINSEL_ConfigMultiplePins(&leds, mascara);
    GPIO_SetDir(0, mascara, 1);

    // LED

    leds = (PINSEL_CFG_T){0};

    leds.port = 0;
    leds.pin = 22;
    leds.func = 0;
    leds.mode = 2;

    PINSEL_ConfigPin(&leds);
    GPIO_SetDir(0, LED, 1);

}

void SysTick_Handler(){

    contadorA--;
    contadorB--;

    if(contadorA == 0){

        GPIO_TogglePins(0, LED);

        contadorA = 5;

    }
    if(contadorB == 0){

        GPIO_ClearPins(0, mascara);
        GPIO_SetPins(0, sec[valor]);

        if(valor <= 12){
            valor++;
        }else{
            valor = 0;
        }

        contadorB = 2;

    }

    return;

}
