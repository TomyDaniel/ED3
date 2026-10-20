/*
Por un pin del ADC del microcontrolador LPC1769 ingresa una tensión de rango dinámico 0 a 3,3[v] proveniente de un sensor de temperatura. 
Debido a la baja tasa de variación de la señal, se pide tomar una muestra cada 30[s]. Pasados los 2[min] se debe promediar las últimas 
4 muestras y en función de este valor, tomar una decisión sobre una salida digital de la placa:

Si el valor es <1 [V] colocar la salida en 0 (0[V]).
Si el valor es >= 1[V] y <=2[V] sacar una señal intermitente de  F= ????
Si el valor es > 2[V] colocar la salida en 1 (3,3[V]).

*/

#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_tim.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

void config(void);
void configADC(void);
void configTMR(void);

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

    // P0.0

    PINSEL_CFG_T conf = {
        .port = PORT_0,
        .pin = PIN_0,
        .func = PINSEL_FUNC_00,
        .mode = PINSEL_TRISTATE
    };

    PINSEL_ConfigPin(&conf);
    GPIO_SetDir(PORT_0, (1 << 22), GPIO_OUTPUT);

}

void configTMR(){

    TIM_TIMERCFG_T conf0 = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 1000000 // 1 s
    };

    TIM_InitTimer(LPC_TIM0, &conf0);

    TIM_MATCHCFG_T conf0M = {
        .channel = TIM_MATCH_1,
        .intEn = DISABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOp = TIM_TOGGLE,
        .matchValue = 14
    };

    TIM_ConfigMatch(LPC_TIM0, &conf0M);

    TIM_TIMERCFG_T conf1 = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 1000000 // 1 s
    };

    TIM_InitTimer(LPC_TIM1, &conf0);

    TIM_MATCHCFG_T conf1M = {
        .channel = TIM_MATCH_0,
        .intEn = ENABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOp = TIM_NOTHING,
        .matchValue = 119 // 120 segundos = 2 min
    };

    TIM_ConfigMatch(LPC_TIM1, &conf1M);

    TIM_Enable(LPC_TIM0);
    TIM_Enable(LPC_TIM1);

    NVIC_EnableIRQ(TIMER1_IRQn);

}

void configADC(){

    ADC_Init(200000);
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_PowerUp();
    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_ChannelEnable(ADC_CHANNEL_0);
    ADC_EdgeStartConfig(ADC_START_ON_RISING);
    ADC_IntEnable(ADC_INT_CH0);

}

void ADC_IRQHandler(){

    reg[contador] = ADC_ChannelGetData(ADC_CHANNEL_0);

    if(contador <= 2){
        contador++;
    }else{
        contador = 0;
    }

}

void TIMER1_IRQHandler(){

    volatile uint32_t dato = 0;

    for(uint8_t i=0; i<4; i++){
        dato += reg[i];
    }

    dato = ((dato / 4)/4095) * 3.3;

    if(dato < 1){
        GPIO_ClearPins(PORT_0, (1 << 0));
    }else if(dato >= 1 && dato <= 2){
        //intermitente
    }else{
        GPIO_SetPins(PORT_0, (1 << 0));
    }

    TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
    
}