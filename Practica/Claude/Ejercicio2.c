/*
Dos formas de onda, una diente de sierra y una triangular simétrica, deben ser generadas por una función y almacenadas en la primera mitad del bloque AHB SRAM – bank 0. 
Se deben reproducir por el DAC usando DMA con las siguientes condiciones:

Período de señal de 1 ms.
Máxima excursión de tensión posible.
Menor consumo de energía posible del DAC.
La mayor cantidad de muestras por período compatible con lo anterior.

Cada 10 s, medidos con el Timer0 por interrupción, se alterna entre una forma de onda y la otra.

Un pulsador conectado a EINT0 detiene la generación y la salida conserva el último valor. Una segunda pulsación la reanuda.

Indicar cuál es el mínimo incremento de tensión obtenido en cada forma de onda.

cclk = 100 MHz. El código debe estar debidamente comentado.
*/

#include "LPC17xx.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_dma.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_timer.h"

#define DIR_SIE 0x2007C000
#define DIR_TRI 0x2007C640 // Son las 400 muestras por 4 bytes (32 bits, configuracion dma)
#define DAC_ADDR 0x4008C000

void configExti(void);
void configDAC(void);
void configTimer(void);
void configDMA(void);
void dienteSierra(void);
void triangular(void);

void EINT0_IRQHandler(void);

enum estado{
    SIERRA,
    TRIANGULAR
};

enum estado senial = SIERRA;
static GPDMA_Channel_CFG_T conf;
static volatile uint8_t flag = 0;

int main(){

    configExti();
    dienteSierra();
    triangular();
    configDAC();
    configTimer();
    configDMA();

    while(1){}

}

void configExti(){

    EXTI_Init();
    
    EXTI_PinConfig(EXTI_EINT0, EXTI_PULLDOWN);

    EXTI_CFG_T conf = {
        .line = EXTI_EINT0,
        .mode = EXTI_EDGE_SENSITIVE,
        .polarity = EXTI_RISING_EDGE
    };

    EXTI_Config(&conf);
    EXTI_ConfigEnable(&conf);
    EXTI_EnableIRQ(EXTI_EINT0);

}

void configDAC(){
    
    DAC_Init();

    LPC_SC -> PCLKSEL0 &= ~(3 << 22);
    LPC_SC -> PCLKSEL0 |= (2 << 22); // Divido por 2 para que ahora si me de un numero entero

    DAC_UpdateValue(0);
    DAC_SetBias(DAC_350uA);

    DAC_SetDMATimeOut(125); // Como esta a 25 MHz, PCLK / 4, esto sobre los 400 kHz me da un flotante. Tengo que hacer PCLK/2 para que este en 50 MHz

    DAC_CONVERTER_CFG_T conf = {
        .doubleBuffer = ENABLE,
        .dmaCounter = ENABLE,
        .dmaRequest = ENABLE
    };

    DAC_ConfigDAConverterControl(&conf);

}

void configTimer(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 1000000 // 1 Segundo
    };

    TIM_InitTimer(LPC_TIM0, &conf);

    TIM_MATCHCFG_T confM = {
        .channel = 0,
        .intEn = ENABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOpt = TIM_NOTHING, // No me interesa que haga nada, solo que habilite la interrupcion
        .matchValue = 9 // La mitad del periodo para llegar a los 10 segundos
    };

    TIM_ConfigMatch(LPC_TIM0, &confM);
    NVIC_EnableIRQ(TIMER0_IRQn);

    TIM_Enable(LPC_TIM0);

}

void configDMA(){

    GPDMA_Init();


    conf.channelNum = GPDMA_CH_0;
    conf.transferSize = 400; // 400 kHz * 1 ms = . El 400 kHz viene dado de usar los 350 uA del DAC
    conf.type = GPDMA_M2P;
    conf.srcMemAddr = DIR_SIE;
    conf.dstMemAddr = DAC_ADDR;
    conf.srcConn = 0; // Memoria
    conf.dstConn = GPDMA_DAC;
    conf.src.width = GPDMA_WORD;
    conf.src.burst = 0;
    conf.src.increment = ENABLE;
    conf.dst.width = GPDMA_WORD;
    conf.dst.burst = 0;
    conf.dst.increment = DISABLE;
    conf.intTC = DISABLE;
    conf.intErr = DISABLE;
    conf.linkedList.srcAddr = DIR_SIE;
    conf.linkedList.dstAddr = DAC_ADDR;
    conf.linkedList.nextLLI = (uint32_t)&conf.linkedList;
    conf.linkedList.control = 400 | (0 << 12) | (0 << 15) | (2 << 18) | (2 << 21) | (1 << 26) | (0 << 27) | (0 << 31);
    

    GPDMA_SetupChannel(&conf);
    GPDMA_ChannelStart(GPDMA_CH_0);
    
}

void dienteSierra(){

    static volatile uint32_t *sierra = (uint32_t *) DIR_SIE;

    for (uint32_t i = 0; i < 400; i++) {
        sierra[i] = ((i * 1023 / 399) << 6) | (1 << 16);
    }

}

void triangular(){

    static volatile uint32_t *tria = (uint32_t *) DIR_TRI;

    for (uint32_t i = 0; i < 200; i++) {
        tria[i] = ((i * 1023 / 199) << 6) | (1 << 16);
    }

    for (uint32_t i = 200; i < 400; i++) {
        tria[i] = (((399 - i) * 1023 / 199) << 6) | (1 << 16);
    }

}

void EINT0_IRQHandler(){

    if(flag == 0){
        GPDMA_ChannelPause(GPDMA_CH_0);
        flag = 1;
    }else{
        GPDMA_ChannelResume(GPDMA_CH_0);
        flag = 0;
    }

    EXTI_ClearFlag(EXTI_EINT0);

}

void TIMER0_IRQHandler(){

    if(senial == SIERRA){
        conf.linkedList.srcAddr = DIR_TRI;
        senial = TRIANGULAR;
    }else{
        conf.linkedList.srcAddr = DIR_SIE;
        senial = SIERRA;
    }

    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);

}
