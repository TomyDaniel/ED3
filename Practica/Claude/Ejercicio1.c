/*
Programar el LPC1769 en C para que, mediante su ADC, digitalice por el canal 1 una señal analógica cuyo ancho de banda es de 8 kHz y cuya excursión es de 0 a 3,3 V. 
La conversión debe dispararse por hardware mediante un match de timer; elegir y justificar cuál.
Las muestras deben guardarse con el hardware GPDMA en la segunda mitad del bloque AHB SRAM – bank 1, como buffer circular que conserve siempre las últimas muestras. 
Debe almacenar la mayor cantidad de datos que esa región permita.
Cada vez que el buffer se completa, recorrerlo y, si alguna muestra supera los 3 V, encender un LED en P0.22.
Indicar cuántas muestras entran en el buffer y cuánto tiempo de señal representan.
*/

#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_dma.h"
#include "lpc17xx_timer.h"

#define DIR_IN 0x20082000
#define DIR_FI 0x20083FFF
#define ADC_DIR 0x40034014

void config(void);
void configTMR(void);
void configDMA(void);
void configADC(void);

void ADC_IRQHandler();
void DMA_IRQHandler();

int main(){

    config();
    configTMR();
    configDMA();
    configADC();

    while(1){
        __WFI();
    }

}

void config(){

    // Configuracion LED

    PINSEL_CFG_T conf = {
        .port = 0,
        .pin = 22,
        .func = 0,
        .mode = 2
    };

    PINSEL_ConfigPin(&conf);
    GPIO_SetDir(PORT_0, (1 << 22), GPIO_OUTPUT);

}

void configADC(){

    ADC_Init(200000); // La frecuencia la fija el timer
    ADC_PinConfig(ADC_CHANNEL_1);
    ADC_PowerUp();
    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_ChannelEnable(ADC_CHANNEL_1);
    ADC_IntEnable(ADC_INT_CH1); // Habilito la interrupcion para que el DMA sepa cuando termino

}

void configTMR(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US, // Seteo en microsegundos
        .prescaleValue = 1 // Cargo 1 us. T = 1/(2*f), uso 60 us es > 16 kHz
    };

    TIM_InitTimer(LPC_TIM0, &conf); // Inicializo el timer

    TIM_MATCHCFG_T confM = {
        .channel = TIM_MATCH_1, // Match 1 para habilitar el ADC
        .intEn = DISABLE, // No necesito la interrupcion ya que la habilitacion es automatica
        .intStop = DISABLE,
        .resetEn = ENABLE,
        .extOpt = TIM_TOGGLE, // Por cada match togglea la senial
        .matchValue = 29 // Cargo 29 ya que el periodo es de 60 y T_match = (T/2) - 1.
    };

    TIM_ConfigMatch(LPC_TIM0, &confM);
    TIM_Enable(LPC_TIM0);

}

void configDMA(){

    GPDMA_Init();

    static GPDMA_Channel_CFG_T conf = {
        .channelNum = 0, // No especifica, tomo el 0
        .transferSize = 2048, // Es la resta de las direcciones que da 8191 bytes. Uso una palabra pq el tamanio de transfer es de 4095 y una palabra me da 2048
        .type = GPDMA_P2M,
        .srcMemAddr = ADC_DIR,
        .dstMemAddr = DIR_IN,
        .srcConn = GPDMA_ADC,
        .dstConn = 0, // Memoria
        .src.width = GPDMA_WORD,
        .src.burst = 0, // No uso burst
        .src.increment = DISABLE,
        .dst.width = GPDMA_WORD,
        .dst.burst = 0,
        .dst.increment = ENABLE,
        .intTC = ENABLE,
        .intErr = DISABLE,
        .linkedList.srcAddr = ADC_DIR,
        .linkedList.dstAddr = DIR_IN,
        .linkedList.nextLLI = (uint32_t)&conf.linkedList,
        .linkedList.control = 2048 | (0 << 12) | (0 << 15) | (2 << 18) | (2 << 21) | (0 << 26) | (1 << 27) | (1u << 31)
    };

    GPDMA_SetupChannel(&conf);
    GPDMA_ChannelStart(GPDMA_CHANNEL_0);

    NVIC_EnableIRQ(DMA_IRQn);
    
}

void ADC_IRQHandler(){} // No hago nada, es solo para decir termine

void DMA_IRQHandler(){

    volatile uint32_t *buf = (uint32_t *) DIR_IN;
    volatile uint8_t supera = 0;

    for(int i=0; i<2048; i++){

        if(((buf[i] >> 4) & 0xFFF) > 3723){ // 3 * 4096 / 3,3 = 3723
            supera = 1;
            break;
        }
    }

    if(supera == 1){
        GPIO_ClearPins(0, (1 << 22));
    }else{
        GPIO_SetPins(0, (1 << 22));
    }

    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_0);

}
