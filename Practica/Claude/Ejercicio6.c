/*
Usando el GPDMA del LPC1769:

Transferir 1024 datos de 16 bits (halfwords), desde el inicio del AHB SRAM – bank 0 hacia la segunda mitad del AHB SRAM – bank 1.
Antes de la transferencia, llenar el bloque de origen con una función que cargue los valores 0, 1, 2, … 1023.
Cuando termina la transferencia, mediante la interrupción por TC del DMA, encender un LED en P0.22.
Responder:
¿Cuántos bytes se transfieren? Son 2 bytes de datos * 1024, es decir 2048
¿Cuáles son las direcciones de inicio y fin del origen y del destino?
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_dma.h"

#define DIR_INI 0x2007C000 // Final en el 0x2007C7FF. 2048 a hexa es 800 (32 bits). DIR_INI + 0x800 =
#define DIR_FIN 0x20082000 // Final en el 0x200827FF. Cada registro es de un byte. Yo cargo 2 bytes, por ende 2 registros por dato

void config(void); // Para el LED
void llenar(void);
void configDMA(void);

void DMA_IRQHandler(void);

int main(){

    config();
    llenar();
    configDMA();

    while(1){
        __WFI();
    }

}

void config(){
    // P0.22 LED ROJO

    PINSEL_CFG_T conf = {
        .port = PORT_0,
        .pin = PIN_22,
        .func = PINSEL_FUNC_00,
        .mode = PINSEL_TRISTATE
    };

    PINSEL_ConfigPin(&conf);
    GPIO_SetDir(PORT_0, (1 << 22), GPIO_OUTPUT);

}

void llenar(){

    uint16_t *src = (uint16_t *) DIR_INI;
    for (uint16_t i = 0; i < 1024; i++)
        src[i] = i;

}

void configDMA(){

    GPDMA_Init();

    GPDMA_Channel_CFG_T conf = {
        .channelNum = GPDMA_CH_7, // Como es M2M, uso el de menos prioridad
        .transferSize = 1024, // Son 1024 datos, uno en cada lugar de memoria
        .type = GPDMA_M2M,
        .srcMemAddr = DIR_INI,
        .dstMemAddr = DIR_FIN,
        .srcConn = 0,
        .dstConn = 0,
        .src.width = GPDMA_HALFWORD,
        .src.increment = ENABLE,
        .dst.width = GPDMA_HALFWORD,
        .dst.increment = ENABLE,
        .intTC = ENABLE,
    }; // Lo que no agrego, supongo que inician en 0 o desactivadas

    GPDMA_SetupChannel(&conf);

    GPDMA_ChannelStart(GPDMA_CH_7);

    NVIC_EnableIRQ(DMA_IRQn);

}

void DMA_IRQHandler(){

    if(GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CH_7)){
        GPDMA_ChannelStop(GPDMA_CH_7); // Apago el canal ya que es una sola transmision
        GPIO_ClearPins(PORT_0, (1 << 22));
    }

    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_7);

}
