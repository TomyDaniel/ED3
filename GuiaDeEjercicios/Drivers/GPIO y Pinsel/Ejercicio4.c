#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define mascara (0x7F << 0)

void config();
void delay();

uint8_t contador = 0;

uint8_t numeros[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

int main(){

    config();

    while(1){
        GPIO_SetPins(0, numeros[contador]);
        delay();
        GPIO_ClearPins(0, mascara);
        if(contador <= 8){
            contador++;
        }else{
            contador = 0;
        }
    }

}

void config(){
    PINSEL_CFG_T pinsConfig;

    pinsConfig.port = 0;
    pinsConfig.pin = 0;
    pinsConfig.func = 0;
    pinsConfig.mode = 2;

    PINSEL_ConfigMultiplePins(&pinsConfig, mascara);

    GPIO_SetDir(0, mascara, 1);

}

void delay(){
    for(int i=0; i<2500; i++){
        for(int j=0; j<2500; j++){}
    }
}
