/*
Por el canal AD0.2 ingresa una tensión de 0 a 3,3 V proveniente de un sensor.

Tomar una muestra cada 500 ms, con la conversión disparada por hardware mediante un match del Timer0.
Leer el resultado en la interrupción del ADC y guardar las últimas 4 muestras.
Cada vez que llega una muestra nueva, calcular el promedio de las 4 últimas:
Si el promedio es mayor a 2 V, encender un LED en P0.22.
Si no, apagarlo.

Responder:

¿Qué match usás y qué valor de MR cargás? Justificar.
¿Cuánto vale el umbral de 2 V en cuentas del ADC? //
*/

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"

void config(void);
void configTMR(void);
void configADC(void); 

void ADC_IRQHandler(void);

static volatile uint8_t contador = 0;

static volatile uint32_t reg[4];

int main(){

    config();
    configTMR();
    configADC();

    while(1){
        __WFI();
    }

}

void config(){

    // P0.22
    PINSEL_CFG_T conf = {
        .port = PORT_0,
        .pin = PIN_22,
        .func = PINSEL_FUNC_00,
        .mode = PINSEL_TRISTATE
    };

    PINSEL_ConfigPin(&conf);
    GPIO_SetDir(PORT_0, (1<<22), GPIO_OUTPUT);

}

void configTMR(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 50000 // 50 ms
    };

    TIM_InitTimer(LPC_TIM0, &conf);

    TIM_MATCHCFG_T confM = {
        .channel = TIM_MATCH_1,
        .intEn = DISABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOpt = TIM_TOGGLE,
        .matchValue = 4
    };

    TIM_ConfigMatch(LPC_TIM0, &confM);

    TIM_Enable(LPC_TIM0);

}

void configADC(){

    ADC_Init(100000); // La controla el timer
    ADC_PinConfig(ADC_CHANNEL_2);
    ADC_PowerUp();
    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_ChannelEnable(ADC_CHANNEL_2);
    ADC_EdgeStartConfig(ADC_START_ON_RISING);
    ADC_IntEnable(ADC_INT_CH2);

    NVIC_EnableIRQ(ADC_IRQn);

}

void ADC_IRQHandler(){

    volatile uint32_t dato = 0;
    volatile uint32_t prom = 0;

    dato = ADC_ChannelGetData(ADC_CHANNEL_2);

    reg[contador] = dato;

    if(contador <= 2){
        contador++;
    }else{
        contador = 0;
    }

    for(uint16_t i = 0; i<4; i++){
        prom += reg[i]; // Divido sobre la cantidad de bits para tener el valor del voltaje
    }

    if((prom / 4) > 2482){ // 2V/4095 * 3.3V = 2482 bits
        GPIO_ClearPins(PORT_0, (1 << 22));
    }else{
        GPIO_SetPins(PORT_0, (1 << 22));
    }

}
