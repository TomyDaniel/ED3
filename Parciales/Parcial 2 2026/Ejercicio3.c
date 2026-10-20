/*

Diseñe un programa en C que implemente una transferencia de datos de memoria a memoria utilizando el controlador GPDMA, 
la cual deberá iniciarse únicamente al detectarse una interrupción externa proveniente de un pulsador conectado al pin P2.11. 
La operación debe configurarse para copiar datos en formato de 2 bytes (Halfword) desde la primera mitad del banco AHB SRAM1 hacia la segunda mitad del banco AHB SRAMO. 
Una vez concluida la totalidad del copiado, el sistema deberá encender un LED conectado al pin P0.22.

*/

#include "LPC17xx.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_dma.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define SRAM1 0x20080000
#define SRAM0 0x2007E000

void config(void);
void configEXTI(void);
void configDMA(void);

int main(){

    config();
    configEXTI();
    configDMA();

    while(1){}

}

void config(){

    PINSEL_CFG_T conf = {
        .port = PORT_0,
        .pin = PIN_22,
        .func = PINSEL_FUNC_00,
        .mode = PINSEL_TRISTATE
    };

    PINSEL_ConfigPin(&conf);

    GPIO_SetDir(PORT_0, (1 << 22), GPIO_OUTPUT);

}

void configEXTI(){

    EXTI_Init();

    EXTI_CFG_T conf = {
        .line = EXTI_EINT1,
        .mode = EXTI_EDGE_SENSITIVE,
        .polarity = EXTI_RISING_EDGE
    };

    EXTI_ConfigEnable(&conf);
    EXTI_PinConfig(EXTI_EINT1, EXTI_PULLDOWN);

}

void configDMA(){

    GPDMA_Init();

    GPDMA_Channel_CFG_T conf = {
        .channelNum = GPDMA_CH_7,
        .transferSize = 2048,
        .type = GPDMA_M2M,
        .srcMemAddr = SRAM1,
        .dstMemAddr = SRAM0,
        .srcConn = 0,
        .dstConn = 0,
        .src.width = GPDMA_HALFWORD,
        .src.increment = ENABLE,
        .dst.width = GPDMA_HALFWORD,
        .src.increment = ENABLE,
        .intTc = ENABLE
    };

    GPDMA_SetupChannel(&conf);

    NVIC_EnableIRQ(DMA_IRQn);

}

void EINT0_IRQHandler(){

    GPDMA_ChannelStart(GPDMA_CH_7);

    EXTI_ClearFlag(EXTI_EINT1);

}

void DMA_IRQHandler(){

    GPIO_ClearPins(PORT_0, (1 << 22));

    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_7);

}
