/*
El siguiente código debería generar por AOUT una señal de 1 kHz a partir de una tabla de 100 muestras, en forma continua,
mediante el canal 0 del GPDMA y con el menor consumo del DAC. PCLK_DAC = cclk/4 (cclk = 100 MHz).

a) Identificar todos los errores, justificar cada uno y corregirlos.
b) Una vez corregido, explicar cómo queda configurado el canal: tipo de transferencia, origen, destino, cantidad de datos y qué define el ritmo de cada transferencia.
*/

#include "LPC17xx.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_dma.h"

#define N 100
uint16_t tabla[N]; // Guardo una word, no una halfword

void config(void){

    GPDMA_Channel_CFG_T conf; // Deberia ser global para re apuntarse

    for(uint32_t i = 0; i < N; i++)
        tabla[i] = (i * 1023) / N; // Seria N - 1 pq nunca se llega a 10. Falta el desplazamiento y el bit del bias (1 << 16)

    DAC_Init();
    DAC_SetBias(DAC_700uA); // Mal. Deberia ser 350uA
    DAC_SetDMATimeOut(2500); // timeout = 25MHz / ( N * 1kHz) = 250. Incorrecto. F_actualizacion = N * 1kHz
 
    DAC_CONVERTER_CFG_T dacCfg = {
        .doubleBuffer = ENABLE,
        .dmaCounter   = ENABLE,
        .dmaRequest   = DISABLE // No pide datos, mal
    };
    DAC_ConfigDAConverterControl(&dacCfg);

    GPDMA_Init();

    conf.channelNum    = GPDMA_CH_0;
    conf.transferSize  = N;
    conf.type          = GPDMA_P2M; // Mal, deberia ser memoria a periferico
    conf.srcMemAddr    = (uint32_t)tabla;
    conf.dstMemAddr    = (uint32_t)&LPC_DAC->DACR;
    conf.srcConn       = 0;
    conf.dstConn       = GPDMA_DAC;
    conf.src.width     = GPDMA_WORD;
    conf.src.burst     = 0;
    conf.src.increment = ENABLE;
    conf.dst.width     = GPDMA_WORD;
    conf.dst.burst     = 0;
    conf.dst.increment = DISABLE;
    conf.intTC         = DISABLE;
    conf.intErr        = DISABLE;

    conf.linkedList.srcAddr = (uint32_t)tabla;
    conf.linkedList.dstAddr = (uint32_t)&LPC_DAC->DACR;
    conf.linkedList.nextLLI = 0; // Deberia reapuntarse para un buffer circular
    conf.linkedList.control = N | (2 << 18) | (2 << 21) | (1 << 27); // El destino no debe incrementarse. Si la fuente

    GPDMA_SetupChannel(&conf);
    GPDMA_ChannelStart(GPDMA_CH_0);
}

/*
b- El tipo de transferencia es de memoria a periferico. Donde el origen es una region de memoria y el destino es el DAC. Se transmiten 100 muestras en un periodo de la senial
de 1ms. El origen se incrementa, el destino es fijo. Es continuo ya que la LLI se apunta a si misma y el DAC marca el ritmo de la senial pidiendo datos al DMA
*/
