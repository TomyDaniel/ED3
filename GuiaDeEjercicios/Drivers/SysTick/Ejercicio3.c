#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

#define ticks (0.1 * 100000000) // 100 ms

void config(void);
void SysTick_Handler(void);

uint8_t contador = 4;

int main(){

    config();
    SysTick_Config(ticks);

    while(1){}

}

void config(){

    // LED

    PINSEL_CFG_T led;

    led.port = 0;
    led.pin = 22;
    led.func = 0;
    led.mode = 2;

    PINSEL_ConfigPin(&led);

    GPIO_SetDir(0, (1 << 22), 1);

}

void SysTick_Handler(){

    if(contador == 0){
        if(GPIO_ReadValue(0) & (1 << 22)){
            GPIO_ClearPins(0, (1 << 22));
            contador = 4;
        }else{
            GPIO_SetPins(0, (1 << 22));
            contador = 4;
        }
    }else{
        contador--;
    }
}
