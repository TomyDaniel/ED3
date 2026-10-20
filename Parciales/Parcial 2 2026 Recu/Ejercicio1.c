/*
Una alarma contra incendio tiene un sensor de temperatura cuya función de transferencia es la siguiente.

El sensor puede usarse tanto en industria como en residencias.
En industria la temperatura máxima de disparo de alarma es 80 °C, en residencias 57 °C.

Se debe medir y almacenar los últimos 100 datos de temperatura y disparar una alarma si la temperatura sube por encima del límite establecido (sea residencial o industrial).
Adicionalmente, debe dispararse la alarma si la tasa de aumento es de más de 7 °C/min, incluso si no se ha alcanzado la temperatura de disparo fija.
Realice mediciones cada 5 segundos.
Mediante interrupción externa modifique el modo de funcionamiento de industrial a domiciliario, indicando en un par de leds el modo de operación.
cclock 100 MHz
*/

#include "LPC17xx.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

#define BANCO 0x20080000

uint16_t contadorDatos = 0;

void config();
void configTMR();
void configADC();
void configEXTI();

enum{
    RESIDENCIA,
    INDUSTRIAL
}estado = RESIDENCIA;

int main(){

    config();
    configTMR();
    configADC();
    configEXTI();

    while(1){}

}

void config(){

    // P0.22 Led Rojo

    PINSEL_CFG_T conf = {
        .port = PORT_0,
        .pin = PIN_22,
        .mode = PINSEL_TRISTATE,
        .func = PINSEL_FUNC_00
    };

    PINSEL_ConfigPin(&conf);
    GPIO_SetDir(PORT_0, (1 << 22), GPIO_OUTPUT);

    // P3.25 Led ...

    conf = {0};

    conf = {
        .port = PORT_3,
        .pin = PIN_25,
        .func = PINSEL_FUNC_00,
        .mode = PINSEL_TRISTATE
    };

    PINSEL_ConfigPin(&conf);
    GPIO_SetDir(PORT_3, (1 << 25), GPIO_OUTPUT);

    // P0.0

    conf = {0};

    conf = {
        .port = PORT_0,
        .pin = PIN_0,
        .func = PINSEL_FUNC_00,
        .mode = PINSEL_TRISTATE
    };

    PINSEL_ConfigPin(&conf);
    GPIO_SetDir(PORT_0, (1 << 0), GPIO_OUTPUT); // Sirena
}

void configEXTI(){

    EXTI_Init();

    EXTI_CFG_T conf = {
        .line = EXTI_EINT0,
        .mode = EXTI_EDGE_SENSITIVE,
        .polarity = EXTI_RISING_EDGE
    };

    EXTI_EnableConfig(&conf);
    EXTI_PinConfig(EXTI_EINT0, EXTI_PULLDOWN);

}

void configTMR(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 100000 // 100ms
    };

    TIM_InitTimer(LPC_TIM0, &conf);

    TIM_TIMERCFG_T confM = {
        .channel = TIM_MATCH_1,
        .intEn = DISABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOpt = TIM_TOGGLE,
        .matchValue = 24 // 5 segundos
    };

    TIM_ConfigMatch(LPC_TIM0, &confM);
    TIM_Enable(LPC_TIM0);

}

void configADC(){

    ADC_Init(200000); // No me importa la frecuencia, me lo define el timer
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_PowerUp();
    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_ChannelEnable(ADC_CHANNEL_0);
    ADC_EdgeStartConfig(ADC_START_ON_RISING);
    ADC_IntEnable(ADC_INT_CH0);

    NVIC_EnableIRQ(ADC_IRQn);

}

void EINT0_IRQHandler(){

    if(estado == RESIDENCIA){
        estado = INDUSTRIAL;
        GPIO_SetPins(PORT_0, (1 << 22));
        GPIO_ClearPins(PORT_3, (1 << 25));
    }else{
        estado = RESIDENCIA;
        GPIO_ClearPins(PORT_0, (1 << 22));
        GPIO_SetPins(PORT_3, (1 << 25));
    }

    EXTI_ClearFlags(EXTI_EINT0);

}

void ADC_IRQHandler(){

    if(contadorDatos <= 99){
        static volatile dato = ADC_GetDataChannel(ADC_CHANNEL_0);
        *(BANCO + contadorDatos) = dato;
        contadorDatos++;
    }else{
        TIM_Disable(LPC_TIM0); // No se activaria mas el ADC por match

        for(uint32_t i=0; i<100; i++){
            static volatile uint32_t temperatura = ((*BANCO + i)*150)/4095; // 4095 son los 12 bits y 150 la temp maxima
            if(estado == RESIDENCIA && temperatura > 57){
                GPIO_SetPins(PORT_0, (1 << 0)); // Sirena
                break;
            }

            if(estado == INDUSTRIAL && temperatura > 80){
                GPIO_SetPins(PORT_0, (1 << 0)); // Sirena
                break;
            }
        }
    }
}
