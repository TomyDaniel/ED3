#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"

#define DIR1 0x2007C000 // Inicio del banco 0
#define DIR2 0x2007E000 // Mitad del banco 0
#define DIR3 0x2007E001 // Mitad del banco 0
#define DIR4 0x20080000 // Final del banco 0

#define ADC_RATE 160000
#define METRICA 1000

static uint32_t *direccion1 = DIR1;
static uint32_t *direccion2 = DIR2;

volatile uint8_t flagDMA;

void config(void);
void configADC(void);
void configDMA(void);
void configTMR(void);
void configDAC(void);

void ADC_IRQHandler(void);
void EINT0_IRQHandler(void);
void SysTick_Handler(void);
void GPDMA_IRQHandler(void);

enum estado{
    ADC_POINTER,
    ADC_DMA,
    MEM_DMA,
    DAC_DATA,
    DAC_WAVE
};

enum estado tipo = ADC_POINTER;

int main(){

    config();
    configADC();
    configTMR();
    configDAC();
    configDMA();

    while(1){

    }

}

void config(){

    /* Configuracion P2.10
    PINSEL_CFG_T conf = {
        .port = 2;
        .pin = 10;
        .func = PINSEL_FUNC_01;
        .mode = PINSEL_PULLDOWN;
    }

    PINSEL_ConfigPin(&conf);

    GPIO_SetDir(2, (1 << 10), GPIO_INPUT);

    */ // Creo que no es necesario

    EXTI_Init();

    EXTI_PinConfig(EXTI_EINT0, EXTI_PULLDOWN);

    EXTI_CFG_T conf = {
        .line = EXTI_EINT0;
        .mode = EXTI_EDGE_SENSITIVE;
        .polarity = EXTI_RISING_EDGE;
    }

    EXTI_Config(&conf);
    EXTI_ConfigEnable(&conf);

    EXTI_EnableIRQ(EXTI_EINT0);

}

void configADC(){

    // 160 Khz de Inicio del ADC. Doble del ancho de banda de la senial
    ADC_Init(ADC_RATE);
    // No dice canal pero uso el 0
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_PowerUp();
    ADC_StartCmd(ADC_START_ON_EINT0);
    ADC_ChannelEnable(ADC_CHANNEL_0);

}

void configDMA(){

    GPDMA_Init();

    GPDMA_Channel_CFG_T config = {
        .channelNum = GPDMA_CH_0;
        .transferSize = 0; // Nose, lo que recolecte el ADC en 1 segundo?
        .type = GPDMA_P2M; // ADC a Mem
        .srcMemAddr = 0x40034010; //ADR0
        .dstMemAddr = DIR1;
        .srcConn = GPDMA_ADC;
        .dstConn = 0; // Memoria
        .src.width = GPDMA_HALFWORD; // Supongo que la cantidad de bits del ADC. 16?
        .src.burst = 0; // No uso burst
        .src.increment = DISABLE;
        .dst.width = GPDMA_WORD;
        .src.burst = 0;
        .src.increment = ENABLE; // La idea es que se vaya incrementando la memoria
        .intTC = ENABLE;
        .intErr = DISABLE;
    }

    GPDMA_SetupChannel(&config);
    GPDMA_ChannelStart(GPDMA_CH_0);

    GPDMA_Channel_CFG_T configM2M = {
        .channelNum = GPDMA_CH_7;
        .transferSize = 8192; // Cantidad de datos a transferir. Uno solo
        .type = GPDMA_P2M; // ADC a Mem
        .srcMemAddr = DIR1;
        .dstMemAddr = DIR3;
        .srcConn = 0;
        .dstConn = 0; // Memoria
        .src.width = GPDMA_HALFWORD; // Supongo que la cantidad de bits del ADC. 16?
        .src.burst = 0; // No uso burst
        .src.increment = ENABLE;
        .dst.width = GPDMA_HALFWORD;
        .src.burst = 0;
        .src.increment = ENABLE; // La idea es que se vaya incrementando la memoria
        .intTC = DISABLE;
        .intErr = ENABLE;
    }

    GPDMA_SetupChannel(&configM2M);
    GPDMA_ChannelStart(GPDMA_CH_7);

}

void configTMR(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US;
        .prescaleValue = 1;
    };

    TIM_InitTimer(LPC_TIM0, &conf);
    TIM_Enable(LPC_TIM0);

    TIM_MATCHCFG_T confM = {
        .channel = 1;
        .intEn = DISABLE;
        .stopEn = DISABLE;
        .resetEn = ENABLE;
        .extOpt = TIM_HIGH;
        .matchValue = 14; // 1 us + 14 us = 15 us para que inicie la conversion
    };

    TIM_ConfigMatch(LPC_TIM0, &confM);
    
}

void EINT0_IRQHandler(){

    switch (tipo){
        case ADC_POINTER:
            tipo = ADC_DMA;
            // Ya se inicializo el ADC por la configuracion previa en configADC
            break;
        case ADC_DMA:
            tipo = MEM_DMA;
            break;
        
        case DAC_DATA:
            tipo = DAC_WAVE;
            break;

        case DAC_WAVE:
            tipo = ADC_POINTER;
            break;
    }

}

void GPDMA_IRQHandler(){

    GPDMA_ClearIntPending(GPDMA_INTTC, GPDMA_CHANNEL_0);

}
