#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

#define ticks (0.01 * 100000000) // 10 ms 

void config(void);
void SysTick_Handler();

int main(){

    config();

    SysTick_Config(ticks);

    while(1){

    }

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

    if(GPIO_ReadValue(0) & (1 << 22)){
        GPIO_ClearPins(0, (1 << 22));
    }else{
        GPIO_SetPins(0, (1 << 22));
    }

}
