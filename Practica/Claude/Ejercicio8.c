/*
Por el canal AD0.3 ingresa una señal analógica de 0 a 3,3 V con un ancho de banda de 5 kHz. Se pide digitalizarla con el ADC, 
disparando cada conversión por hardware con un match del Timer0. Elegir el match y justificar.

Las muestras deben guardarse con GPDMA en la segunda mitad del bloque AHB SRAM – bank 0, como buffer circular que conserve siempre 
las últimas muestras, y usando la mayor cantidad posible.

Cada vez que se presiona un pulsador conectado a EINT2, se debe buscar el valor máximo del buffer y sacarlo por el DAC como tensión 
continua (actualizado por software), con el menor consumo posible del DAC.

Indicar: la fs elegida, el valor de MR, la cantidad de muestras y el tiempo de señal que representan.
*/

#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dma.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_timer.h"

#define BANCO0_SUP 0x2007E000
#define ADC_CH_3 0x4003401C

void configEXTI();
void configDAC();
void configADC();
void configTMR();
void configDMA();

static GPDMA_Channel_CFG_T conf;
static volatile uint32_t dato = 0;

int main(){

    configEXTI();
    configDAC();
    configADC();
    configTMR();
    configDMA();

    while(1){
        __WFI();
    }

}

void configEXTI(){

    // EINT2

    EXTI_Init();

    EXTI_CFG_T conf = {
        .line = EXTI_EINT2,
        .mode = EXTI_EDGE_SENSITIVE,
        .polarity = EXTI_RISING_EDGE
    };

    EXTI_PinConfig(EXTI_EINT2, EXTI_PULLDOWN);
    EXTI_ConfigEnable(&conf);

}

void configDAC(){

    DAC_Init();
    DAC_SetBias(DAC_350uA);

}

void configADC(){

    ADC_Init(200000); // 20 kHz > 2 * 5kHz
    ADC_PinConfig(ADC_CHANNEL_3);
    ADC_PowerUp();
    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_ChannelEnable(ADC_CHANNEL_3);
    ADC_EdgeStartConfig(ADC_START_ON_RISING);
    ADC_IntEnable(ADC_INT_CH3);

}

void configTMR(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 1
    };

    TIM_InitTimer(LPC_TIM0, &conf);

    TIM_MATCHCFG_T confM = {
        .channel = TIM_MATCH_1,
        .intEn = DISABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOp = TIM_TOGGLE,
        .matchValue = 24 // Para 50 us
    };

    TIM_ConfigMatch(LPC_TIM0, &confM);
    TIM_Enable(LPC_TIM0);

}

void configDMA(){

    GPDMA_Init();

    conf.channelNum = GPDMA_CH_0;
    conf.transferSize = 2048;
    conf.type = GPDMA_P2M;
    conf.srcMemAddr = ADC_CH_3;
    conf.dstMemAddr = BANCO0_SUP;
    conf.srcConn = GPDMA_ADC;
    conf.dstConn = 0;
    conf.src.width = GPDMA_WORD;
    conf.src.increment = DISABLE;
    conf.dst.width = GPDMA_WORD;
    conf.dst.increment = ENABLE;
    conf.intTC = DISABLE;
    conf.intErr = DISABLE;
    conf.linkedList.srcAddr = ADC_CH_3;
    conf.linkedList.dstAddr = BANCO0_SUP;
    conf.linkedList.nextLLI = &conf.linkedList;
    conf.linkedList.control = 2048 | (2 << 18) | (2 << 21) | (1 << 27);

    GPDMA_SetupChannel(&conf);
    GPDMA_ChannelStart(GPDMA_CH_0);

}

void ADC_IRQHandler(){}

void EINT2_IRQHandler(){

    volatile uint32_t valor = 0;
    for(uint32_t i=0; i<2048; i++){
        dato = *(BANCO0_SUP + i);
        
        if(dato >= valor){
            valor = dato;
        }

    }

    dato = dato / 2048; // Divido sobre 2048, pq cada dato ocupa 4 bytes

    DAC_UpdateValue((dato/1024)*3.3);

}