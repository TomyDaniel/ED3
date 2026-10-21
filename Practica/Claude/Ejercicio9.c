/*
Se debe generar por el DAC una señal triangular de 0 a 3,3 V a partir de una tabla de 250 muestras, generada por una 
función y almacenada en la primera mitad del bloque AHB SRAM – bank 1. La reproducción se hace con DMA en forma continua y con el menor consumo del DAC.

El período de la señal debe recorrer cíclicamente los valores 1 ms → 2 ms → 4 ms → 1 ms → …, cambiando cada 5 s, medidos con el Timer1 por interrupción.

Indicar: el valor de DACCNTVAL para cada período, y verificar que el DAC puede alcanzar esas frecuencias de actualización con la configuración elegida.
*/

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_dma.h"

#define BANCO1_INF 0x20080000
#define DAC_ADDR 0x4008C000

void configTMR();
void configDAC();
void configDMA();
void senial();

static GPDMA_Channel_CFG_T conf;

enum{
    UNO,
    DOS,
    CUATRO
}estado = UNO;

int main(){

    configTMR();
    configDAC();
    configDMA();
    senial();

    while(1){
        __WFI();
    }

}

void configTMR(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 1000000 // 1 s
    };

    TIM_InitTimer(LPC_TIM1, &conf);

    TIM_MATCHCFG_T confM = {
        .channel = TIM_MATCH_0,
        .intEn = ENABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOp = TIM_NOTHING,
        .matchValue = 4
    };

    TIM_ConfigMatch(LPC_TIM1, &confM);
    TIM_Enable(LPC_TIM1);

    NVIC_EnableIRQ(TIMER1_IRQn);

}

void configDAC(){

    DAC_Init();
    DAC_SetBias(DAC_350uA);

    DAC_CONVERTER_CFG_T conf = {
        .doubleBuffer = ENABLE,
        .dmaCounter = ENABLE,
        .dmaRequest = ENABLE
    };

    DAC_ConfigDAConverterControl(&conf);

    DAC_SetDMATimeOut(100);

}

void configDMA(){

    GPDMA_Init();

    conf.channelNum = GPDMA_CH_0,
    conf.transferSize = 250, // Solo 250 valores
    conf.type = GPDMA_M2P,
    conf.srcMemAddr = BANCO1_INF,
    conf.dstMemAddr = DAC_ADDR,
    conf.srcConn = 0,
    conf.dstConn = GPDMA_DAC,
    conf.src.width = GPDMA_WORD,
    conf.src.increment = ENABLE,
    conf.dst.width = GPDMA_WORD,
    conf.dst.increment = DISABLE,
    conf.intTC = DISABLE,
    conf.intErr = DISABLE,
    conf.linkedList.srcAddr = BANCO1_INF,
    conf.linkedList.dstAddr = DAC_ADDR,
    conf.linkedList.nextLLI = &conf.linkedList,
    conf.linkedList.control = 250 | (2 << 18) | (2 << 21) | (1 << 26);

    GPDMA_SetupChannel(&conf);
    GPDMA_ChannelStart(GPDMA_CH_0);

}

void senial(){

    for(uint8_t i=0; i<250;i++){
        *(BANCO1_INF + i) = (i/1024) * 3.3;
    }

}

void TIMER1_IRQHandler(){

    switch (estado){

    case UNO:
        DAC_SetDMATimeOut(200);
        estado = DOS;
        break;

    case DOS:
        DAC_SetDMATimeOut(400);
        estado = CUATRO;
        break;

    case CUATRO:
        DAC_SetDMATimeOut(100);
        estado = UNO;
        break;
    
    }

    TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);

}