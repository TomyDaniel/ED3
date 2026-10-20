/*
Mediante la utilización del microcontrolador LPC1769, implemente el software necesario  para cumplir con los siguientes requerimientos:
1- Usar el hardware GPDMA para transferir datos en palabras de 2 bytes desde la primera mitad de la SRAM del banco 1 hacia la segunda mitad de la SRAM del banco 0.

2- Una vez finalizada la transferencia, mediante la interrupción por cuenta terminal del GPDMA, se debe encender un LED indicador conectado a P0.22.

3- Determine usted Cuántos bytes fueron transferidos

Fueron transferidos 8192 bytes
*/

#include "LPC17xx.h"
#include "lpc17xx_dma.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define BANCO1 0x20080000
#define BANCO0 0x2007E000

void config(void);
void configDMA(void);

int main(){

    config();
    configDMA();

    while(1){
        __WFI();
    }

}

void config(){

    // P0.22

    PINSEL_CFG_T conf = {
        .port = PORT_22,
        .pin = PIN_0,
        .func = PINSEL_FUNC_00,
        .mode = PINSEL_TRISTATE
    };

    PINSEL_ConfigPin(&conf);
    GPIO_SetDir(PORT_0, (1 << 22), GPIO_OUTPUT);

}

void configDMA(){

    GPDMA_Init();

    GPDMA_Channel_CFG_T conf = {
        .channelNum = GPDMA_CH_7,
        .transferSize = 4095,
        .type = GPDMA_M2M,
        .srcMemAddr = BANCO0,
        .dstMemAddr = BANCO1,
        .srcConn = 0,
        .dstConn = 0,
        .src.width = GPDMA_HALFWORD,
        .src.increment = ENABLE,
        .dst.width = GPDMA_HALFWORD,
        .dst.increment = ENABLE,
        .intTC = ENABLE,
        .intErr = DISABLE,
        .linkedList = 0
    };

    GPDMA_SetupChannel(&conf);

    GPDMA_ChannelStart(GPDMA_CH_7);

}

void DMA_IRQHandler(){

    GPIO_ClearPins(PORT_0, (1 << 22));

    GPDMA_ClearIntPending(GPDMA_INT_INTTC, GPDMA_CH_7);

}
