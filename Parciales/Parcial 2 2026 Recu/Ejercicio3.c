/*
Considerando que se tiene un bloque de datos comprendidos entre las posiciones de memorias, dirección inicial = 0x10000800 a la dirección final = 0x10001000 
ambas inclusive y se desea trasladar este bloque de datos a una nueva zona de memoria comprendida entre la dirección inicial = 0x10002800 y 
la dirección Final = 0x10003000 (en el mismo orden). Teniendo en cuenta además que los datos contenidos dentro de la zona de memoria son de 16 bits 
(AHB Master endianness configuration - por defecto) y que estos deben moverse de a uno (1) en cada evento de DMA, se sincronizará la transmisión con 
evento de match0 del timer1.

Se pide que Enumere y explique los puntos a tener en cuenta para configurar correctamente el controlador DMA.
*/

#include "LPC17xx.h"
#include "lpc17xx_dma.h"

#define DIR_INI 0x10000800
#define DIR_FIN 0x10002800

void configDMA(void);

int main(){

    configDMA();

    while(1){}

}

void configDMA(){

    GPDMA_Init();

    GPDMA_Channel_CFG_T conf = {
        .channel = GPDMA_CH_0,
        .transferSize = 1025,
        .type = GPDMA_P2M,
        .srcMemAddr = DIR_INI,
        .dstMemAddr = DIR_FIN,
        .srcConn = GPDMA_MAT1_0,
        .dstConn = 0,
        .src.width = GPDMA_HALFWORD,
        .src.burst = GPDMA_BSIZE_1, // Para que mueva de a uno
        .src.increment = ENABLE,
        .dst.width = GPDMA_HALFWORD,
        .dst.increment = ENABLE,
        .intTC = ENABLE,
        .intErr = DISABLE,
        .linkedList = 0
    };

    GPDMA_SetupChannel(&conf);
    GPDMA_ChannelStart(GPDMA_CH_0);

    NVIC_EnableIRQ(DMA_IRQn);

}
