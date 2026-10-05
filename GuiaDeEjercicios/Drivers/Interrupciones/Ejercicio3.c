#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"

#define mascara (0x7F << 0)

void config(void);
void EINT0_IRQHandler(void);
void delay(void);

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

uint8_t contador = 0;

enum tipo{
    ADELANTE,
    ATRAS
};

enum tipo caso = ADELANTE;

int main(){

    config();

    caso = ADELANTE;

    while (1){

        GPIO_ClearPins(0, mascara);
        GPIO_SetPins(0, numeros[contador]);

        switch(caso){
            case ADELANTE:

                if(contador <= 8){
                    contador++;
                }else{
                    contador = 0;
                }

                break;
            
            case ATRAS:

                if(contador > 0){
                    contador--;
                }else{
                    contador = 9;
                }

                break;
    
        }

        delay();

    }
    
}

void config(){

    PINSEL_CFG_T display;

    display.port = 0;
    display.pin = 0;
    display.func = 0;
    display.mode = 2;
    
    PINSEL_ConfigMultiplePins(&display, mascara);

    GPIO_SetDir(0, mascara, 1);

    GPIO_SetPins(0, numeros[contador]);

    EXTI_CFG_T conf;

    conf.line = 0;
    conf.mode = 1;
    conf.polarity = 1;

    EXTI_Config(&conf);
    EXTI_PinConfig(0, 1);
    EXTI_EnableIRQ(0);

}

void EINT0_IRQHandler(){

    if(caso == ADELANTE){
        caso = ATRAS;
    }else{
        caso = ADELANTE;
    }

    EXTI_ClearFlag(0);

}

void delay(void){
    for(int i=0; i<2500; i++){
        for(int j=0; j<2500; j++){}
    }
}
