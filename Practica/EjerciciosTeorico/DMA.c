#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_dma.h"

#define SRC 0xAAAA
#define DST 0xBBBB

// Transferir un buffer de 64 datos de 16 bits de la posicion 0xAAAA a la posicion 0xBBBB
// La transferencia se dispara con la EINT2
// Al finalizar la transferencia encendemos el LED rojo

void config(void);
void configDMA(void);
void EINT2_IRQHandler(void);
void DMA_IRQHandler(void);

int main(){

    config();
    configDMA();

    while(1){}

}

void config(){

    // EINT2

    EXTI_CFG_T eint;

    eint.line = 2;
    eint.mode = 1;
    eint.polarity = 1;

    EXTI_Config(&eint);

    EXTI_PinConfig(2, 1); // EINT2 y Pull-down

    EXTI_Init();

    // LED

    PINSEL_CFG_T led;

    led.port = 0;
    led.pin = 22;
    led.func = 0;
    led.mode = 3;

    PINSEL_ConfigPin(&led);

    GPIO_SetDir(0, (1 << 22), 1);

}

void configDMA(){

    GPDMA_Init();

    GPDMA_Endpoint_T entrada = {0};

    entrada.width = 1; // Halfword
    entrada.burst = 3; // 16 bits
    entrada.increment = ENABLE;

    GPDMA_Endpoint_T salida = {0};

    salida.width = 1;
    salida.burst = 3;
    salida.increment = ENABLE;

    GPDMA_Channel_CFG_T canal = {0};

    canal.channelNum = 0; // Canal 0
    canal.transferSize = 64; // 64 bits de transferencia
    canal.type = 0; // Memoria a memoria
    canal.srcMemAddr = SRC;
    canal.dstMemAddr = DST;
    canal.srcConn = 0;
    canal.dstConn = 0;
    canal.src = &entrada;
    canal.dst = &salida;
    canal.intTC = ENABLE; // Interrupcion de conteo
    canal.intErr = DISABLE; // Interrupcion de error
    canal.linkedList = 0;

    GPDMA_SetupChannel(&canal);

}

void EINT2_IRQHandler(){

    GPDMA_ChannelStart(0);

    EXTI_ClearFlag(2);

}

void DMA_IRQHandler(){

    GPIO_ClearPins(0, (1 << 22)); // Prendo led

    GPDMA_ChannelGracefulStop(0); // Paro el DMA

    GPDMA_ClearIntPending(GPDMA_CLR_INTERR, 0); // Limpiar interrupcion del canal 0

}
