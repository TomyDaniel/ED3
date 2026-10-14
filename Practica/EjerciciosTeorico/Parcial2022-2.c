/*
    Programar la LPC1769 para que mediante su ADC digitalice dos seniales analogicas cuyos anchos de bandas son de 10 kHz cada una.
    Los canales utilizados deben ser el 2 y el 4 y los datos deben ser guardados en dos regiones de memoria distintas que permitan contar
    los ultimos 20 datos de cada canal. Suponer una frecuencia de cclk de 100 MHz
*/  

#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dma.h"

void configADC();

static volatile uint32_t *mem1 = 0x1000; // Canal 1;
static volatile uint32_t *mem2 = 0x2000; // Canal 2;

static volatile uint32_t cont1 = 0;
static volatile uint32_t cont2 = 0;

int main(){

    configADC();

    while(1){

    }

}

void configADC(){

    ADC_Init(40000);

    ADC_PinConfig(ADC_CHANNEL_2);
    ADC_PinConfig(ADC_CHANNEL_4);

    ADC_IntEnable(ADC_INT_CH2);
    ADC_IntEnable(ADC_INT_CH4);

    NVIC_EnableIRQ(ADC_IRQn);

    ADC_ChannelEnable(ADC_CHANNEL_2);
    ADC_StartCmd(ADC_START_NOW);

}

void ADC_IRQHandler(){

    if(ADC_ChannelGetStatus(ADC_CHANNEL_2, ADC_DATA_DONE)){
        *mem1 = ADC_ChannelGetData(ADC_CHANNEL_2);
        
        if(cont1 <= 19){
            mem1++;
            cont1++;
        }else{
            cont1 = 0;
            mem1 = 0x1000;
        }

    }else{
        *mem2 = ADC_ChannelGetData(ADC_CHANNEL_4);
        
        if(cont2 <= 19){
            mem2++;
            cont2++;
        }else{
            cont2 = 0;
            mem2 = 0x2000;
        } 
    }
}
