#include "LPC17xx.h"
#include "lpc17xx_gpio.h"

void config();
void delay(int num);

int main(){

    config();

    while(1){
        GPIO_ClearPins(0, (1<<22));
        GPIO_SetPins(3, (3<<25));
        delay(2500);
        GPIO_SetPins(0, (1<<22));
        GPIO_SetPins(3, (1<<26));
        GPIO_ClearPins(3, (1<<25));
        delay(2500);
        GPIO_ClearPins(0, (1<<22));
        GPIO_SetPins(3, (1<<26));
        GPIO_ClearPins(3, (1<<25));
        delay(5000);
        GPIO_SetPins(0, (1<<22));
        GPIO_SetPins(3, (1<<25));
        GPIO_ClearPins(3, (1<<26));
        delay(2500);
        GPIO_ClearPins(0, (1<<22));
        GPIO_SetPins(3, (1<<25));
        GPIO_ClearPins(3, (1<<26));
        delay(2500);
        GPIO_ClearPins(0, (1<<22));
        GPIO_ClearPins(3, (1<<26));
        GPIO_ClearPins(3, (1<<25));
        delay(5000);
    }
}

void config(){
    PINSEL_CFG_T pin22;

    pin22.port = 0;
    pin22.pin = 22;
    pin22.func = 0;
    pin22.mode = 2;

    PINSEL_ConfigPin(&pin22);

    PINSEL_CFG_T pinRest;

    pinRest.port = 3;
    pinRest.pin = 25;
    pinRest.func = 0;
    pinRest.mode = 2;

    PINSEL_ConfigMultiplePins(&pinRest, (3 << 25));

    GPIO_SetDir(0, (1<<22), 1);
    GPIO_SetDir(3, (3<<25), 1);

}

void delay(){
    for(int i=0; i<2500; i++){
        for(int j=0; j<2500; j++){}
    }
}
