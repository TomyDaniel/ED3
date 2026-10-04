#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

void config();

int main(){

    config();

    while(1){
        if(GPIO_ReadValue(0) & (1 << 0)){
            GPIO_ClearPins(0, (1<<22));
        }else{
            GPIO_SetPins(0, (1<<22));
        }
    }
}

void config(){

    PINSEL_CFG_T configIn;

    configIn.port = 0;
    configIn.pin = 0;
    configIn.func = 0;
    configIn.mode = 3;

    PINSEL_ConfigPin(&configIn);

    GPIO_SetDir(0, (1 << 0), 0);

    PINSEL_CFG_T configOut;

    configOut.port = 0;
    configOut.pin = 22;
    configOut.func = 0;
    configOut.mode = 2;

    PINSEL_ConfigPin(&configOut);

    GPIO_SetDir(0, (1 << 22), 1);

}
