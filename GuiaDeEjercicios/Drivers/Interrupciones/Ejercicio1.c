#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"

void config();
void EINT0_IRQHandler();

int main(){

    config();

    while(1){

    }

}

void config(){

    EXTI_CFG_T pinIn;

    pinIn.line = EXTI_EINT0;
    pinIn.mode = 1;
    pinIn.polarity = 0;

    EXTI_Config(&pinIn);
    EXTI_PinConfig(0, 0); // EINT0 y Pull-Up
    EXTI_EnableIRQ(0);

    PINSEL_CFG_T pinOut;

    pinOut.port = 0;
    pinOut.pin = 22;
    pinOut.func = 0;
    pinOut.mode = 2;

    PINSEL_ConfigPin(&pinOut);

    GPIO_SetDir(0, (1<<22), 1);

}

void EINT0_IRQHandler(){

    if(GPIO_ReadValue(0) & (1 << 22)){
        GPIO_ClearPins(0, (1 << 22));
    }else{
        GPIO_SetPins(0, (1 << 22));
    }

    EXTI_ClearFlag(0);

}
