#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"

#define LEDS (0x0F << 1) // Desde el pin 1 del puerto 0 conecto los 4 Leds

void config(void);
void EINT1_IRQHandler(void);
void EINT3_IRQHandler(void);
void delay(void);

uint8_t secuenciaA[] = {0xF, 0xA, 0x2, 0x7};
uint8_t secuenciaB[] = {0xC, 0x1, 0x5, 0xD};

uint8_t contadorA = 0;
uint8_t contadorB = 0;

enum estado{
    SEC_A,
    SEC_B
};

enum estado secuencia = SEC_A;

int main(){

    config();

    while(1){

        switch(secuencia){
            case SEC_A:

                GPIO_ClearPins(0, LEDS);
                GPIO_SetPins(0, (secuenciaA[contadorA] << 1)); // Desplazo para posicionarme sobre el pin 1 en adelante
                delay();

                if(contadorA <= 2){
                    contadorA++;
                }else{
                    contadorA = 0;
                }

                break;

            case SEC_B:

                GPIO_ClearPins(0, LEDS);
                GPIO_SetPins(0, (secuenciaB[contadorB] << 1)); // idem
                delay();

                if(contadorB <= 2){
                    contadorB++;
                }else{
                    contadorB = 0;
                }

                break;

        }
    }
}

void config(){

    // LEDS

    PINSEL_CFG_T leds;

    leds.port = 0;
    leds.pin = 1;
    leds.func = 0;
    leds.mode = 2;

    PINSEL_ConfigMultiplePins(&leds, LEDS);

    GPIO_SetDir(0, LEDS, 1);

    // EINT3

    PINSEL_CFG_T botton_p0;

    botton_p0.port = 0;
    botton_p0.pin = 0;
    botton_p0.func = 0;
    botton_p0.mode = 3;

    PINSEL_ConfigPin(&botton_p0);

    GPIO_SetDir(0, (1 << 0), 0);
    GPIO_IntConfigPin(0, 0, 1, ENABLE); // Puerto 0. Pin 0. Flanco de Subida. Hab. de Int.

    // EINT1

    EXTI_CFG_T config;

    config.line = 1;
    config.mode = 1;
    config.polarity = 1; // Por alto. Flanco subida

    EXTI_Config(&config);
    EXTI_PinConfig(1, 1); // EINT1, Pull-down
    EXTI_EnableIRQ(1);
    EXTI_EnableIRQ(3);

}

void EINT1_IRQHandler(){

    if(secuencia == SEC_B){
        secuencia = SEC_A;
    }else{
        secuencia = SEC_A; // Redundante pero me
    }

    EXTI_ClearFlag(1);

}

void EINT3_IRQHandler(){

    if(secuencia == SEC_A){
        secuencia = SEC_B;
    }else{
        secuencia = SEC_B;
    }

    GPIO_ClearInt(0, (1 << 0));

}

void delay(){

    for(int i=0; i<2500; i++){
        for(int j=0; j<2500; j++){}
    }

}