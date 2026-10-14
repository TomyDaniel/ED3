/*
    Utilizando el timer0, un DAC, interrupciones y el driver de LPC1769, escribir un codigo que permita generar una senial triangular
    periodica simetrica, que tenga el minimo periodo posible, la maxima excursion de voltaje pico a pico posible y el minimo incremento
    de senial posible por el DAC. Suponer una frecuencia de cclk de 100MHz.
    El codigo debe estar debidamente comentado.
*/

#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_adc.h"

void configTMR();
void configDAC();

void TIM0_IRQHandler();

uint32_t valor = 0;

enum estado{
    SUMA,
    RESTA
};

enum estado mom = SUMA;

int main(){

    configTMR();
    configDAC();

    while(1){}

}

void config(){

}

void configDAC(){

    DAC_Init();
    DAC_SetBias(0); // 700 uA

}

void configTMR(){

    TIM_TIMERCFG_T conf = {0}; // TIM_TIMERCFG_T conf = {TIM_US, 1};

    conf.prescaleOpt = TIM_US;
    conf.prescaleValue = 1; // 1 us

    TIM_InitTimer(LPC_TIM0, conf); // Timer0 y prescaler

    TIM_MATCHCFG_T mach = {0};

    mach.channel = TIM_MATCH_0;
    mach.intEn = ENABLE;
    mach.stopEn = DISABLE;
    mach.resetEn = ENABLE;
    mach.extOpt = TIM_NOTHING;
    mach.matchValue = 0; // 0 s

    TIM_ConfigMatch(LPC_TIM0, mach);

    NVIC_EnableIRQ(TIM0_IRQn);

    TIM_Enable(LPC_TIM0);

}

void TIM0_IRQHandler(){

    switch(mom){
        case SUMA:
            if(valor <= 1022){
                valor++;
            }else{
                mom = RESTA;
            }
            break;
        case RESTA:
            if(valor <= 1){
                valor--;
            }else{
                mom = SUMA;
            }
            break;
    }

    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);

}
