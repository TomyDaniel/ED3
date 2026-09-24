#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_adc.h"

void config();
void config_TMR0();
void config_ADC();
void config_DAC();
void ADC_IRQHandler();
void TIMER0_IRQHandler();

uint16_t medicion = 0;

int main(){

    config();
    config_ADC();
    config_DAC();
    config_TMR0();

    while(1){}

}

void config(){

    /*
        Configuracion de Pines
        Led: Rojo P0.22; Azul P3.26; Verde P3.25
    */

    PINSEL_CFG_T PIN_CFG_1; // Led ROJO
    PINSEL_CFG_T PIN_CFG_2; // Led AZUL y VERDE

    // Led Rojo
    PIN_CFG_1.port = 0;
    PIN_CFG_1.pin = 22;
    PIN_CFG_1.func = 0;
    PIN_CFG_1.mode = 2;

    // Led Azul y Verde
    PIN_CFG_2.port = 3;
    PIN_CFG_2.pin = 25;
    PIN_CFG_2.func = 0;
    PIN_CFG_2.mode = 2;

    uint32_t mask = (3 << 25);

    PINSEL_ConfigPin(&PIN_CFG_1);
    PINSEL_ConfigMultiplePins(&PIN_CFG_2, mask);

    GPIO_SetDir(0, (1 << 22), 1); // Configuracion 0.22
    GPIO_SetDir(3, (3 << 25), 1); // Configuracion 3.25 y 3.26

    GPIO_SetPins(0, (1 << 22));
    GPIO_SetPins(3, (3 << 25));

}

void config_TMR0(){

    TIM_TIMERCFG_T TIM_CONFIG;

    TIM_CONFIG.prescaleOpt = 1;
    TIM_CONFIG.prescaleValue = 10000; // 10 ms

    TIM_InitTimer(LPC_TIM0, &TIM_CONFIG);

    TIM_MATCHCFG_T MATCH_CFG;

    MATCH_CFG.channel = 0; // Habilito MR0
    MATCH_CFG.intEn = ENABLE; // Habilito la Interrupcion
    MATCH_CFG.stopEn = DISABLE; // Deshabilito el stop
    MATCH_CFG.resetEn = ENABLE; // Habilito el reset
    MATCH_CFG.matchValue = 10; // Le paso 10 para los 100 ms

    TIM_ConfigMatch(LPC_TIM0, &MATCH_CFG);

    TIM_Enable(LPC_TIM0);

    NVIC_EnableIRQ(TIMER0_IRQn);

}

void config_ADC(){

	ADC_Init(100000);
    ADC_PinConfig(0); // Configuracion del pin fisico
    ADC_ChannelEnable(0); // Habilitacion del canal
    //ADC_StartCmd(1); // Configuracion del modo de inicio
    ADC_PowerUp(); // Inicia la conversion
    ADC_BurstDisable(); // Deshabilitamos el burst
    ADC_IntEnable(0); // Habilito la interrupcion

    NVIC_EnableIRQ(ADC_IRQn);

}

void config_DAC(){

    DAC_Init();
    DAC_UpdateValue(medicion);
    DAC_SetBias(0);

}

void TIMER0_IRQHandler(){

    TIM_ClearIntPending(LPC_TIM0, 0); // Limpiamos flag para el match 0

    ADC_StartCmd(1);

}

void ADC_IRQHandler(){

    medicion = ADC_ChannelGetData(0) >> 2;

    DAC_UpdateValue(medicion);

    if(medicion < 310){
        GPIO_ClearPins(3, (1 << 25));
        GPIO_SetPins(3, (1 << 26));
        GPIO_SetPins(0, (1 << 22));
    }else if(medicion >= 310 && medicion < 620){
        GPIO_SetPins(3, (1 << 25));
        GPIO_ClearPins(3, (1 << 26));
        GPIO_SetPins(0, (1 << 22));
    }else if(medicion >= 620 && medicion < 930){
        GPIO_SetPins(3, (1 << 25));
        GPIO_ClearPins(3, (1 << 26));
        GPIO_ClearPins(0, (1 << 22));
    }else{
        GPIO_SetPins(3, (1 << 25));
        GPIO_SetPins(3, (1 << 26));
        GPIO_ClearPins(0, (1 << 22));
    }

}
