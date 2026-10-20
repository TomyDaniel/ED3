/*
Por el canal 0 del ADC ingresa una señal de 0 a 3,3 V con un ancho de banda de 4 kHz.

Grabación: al presionar un pulsador conectado a EINT1, se debe grabar la señal en la primera mitad del bloque AHB SRAM – bank 1, 
usando la mayor cantidad de muestras que esa región permita. Condiciones:

Muestreo disparado por hardware con un match del Timer1. Elegir el match y justificar.
Transferencia con GPDMA, en una sola pasada: el buffer se llena una vez y se detiene.
Mientras se graba, el LED en P0.22 permanece encendido.

Reproducción: cuando termina la grabación (interrupción por TC del DMA):

Apagar el LED.
Convertir las muestras del buffer al formato que necesita el DAC, en el mismo lugar de memoria.
Reproducirlas por el DAC con DMA, en forma continua, a la misma frecuencia con la que se grabaron y con el menor consumo posible del DAC.

Una nueva pulsación de EINT1 detiene la reproducción y vuelve a grabar.

Responder:

¿Qué fs elegiste y por qué? ¿Qué valor de MR cargás?
¿Cuántas muestras entran y cuánto tiempo de señal representan?
¿Qué valor de timeout (DACCNTVAL) usás? ¿Hace falta cambiar PCLK_DAC?
¿Cómo convertís una muestra del ADC (12 bits, en los bits 15:4) al formato del DAC (10 bits, en los bits 15:6, más BIAS)?
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dma.h"
#include "lpc17xx_dac.h"

#define GRABO 0x20080000
#define FIN 0x20081FFF
#define ADC_ADDR 0x40034010
#define DAC_ADDR 0x4008C000

void config(void);
void configExti(void);
void configADC(void);
void configDAC(void);
void configTMR(void);
void configDMA1(void);
void configDMA2(void);

void TIMER0_IRQHandler(void);
void EINT1_IRQHandler(void);

static GPDMA_Channel_CFG_T conf;

enum {
    ESPERANDO,
    GRABANDO,
    REPRODUCIENDO
}estados = ESPERANDO;

int main(){

    config();
    configExti();
    configADC();
    configDAC();
    configTMR();

    NVIC_EnableIRQ(DMA_IRQn);

    while(1){}

}

void config(){

    PINSEL_CFG_T conf = {
        .pin = PIN_22,
        .port = PORT_0,
        .func = PINSEL_FUNC_00,
        .mode = PINSEL_TRISTATE
    };

    PINSEL_ConfigPin(&conf);

    GPIO_SetDir(PORT_0, (1 << 22), GPIO_OUTPUT);

}

void configExti(){

    EXTI_Init();

    EXTI_PinConfig(EXTI_EINT1, EXTI_PULLDOWN);

    EXTI_CFG_T conf = {
        .line = EXTI_EINT1,
        .mode = EXTI_EDGE_SENSITIVE,
        .polarity = EXTI_RISING_EDGE
    };

    EXTI_Config(&conf);
    EXTI_ConfigEnable(&conf);

    EXTI_EnableIRQ(EXTI_EINT1);

}

void configADC(){

    // Canal 0 del ADC. Senial de 0 a 3,3 V. Ancho de banda de 4 kHz.
    // Inicio del ADC = 10000 kHz > 4 kHz * 2. Por Nyquist
    // Periodo = 1/10 kHz = 100 us

    ADC_Init(200000); // Me sirve despues para calcular el timeout
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_StartCmd(ADC_START_ON_MAT01);
    ADC_ChannelEnable(ADC_CHANNEL_0);
    ADC_IntEnable(ADC_CHANNEL_0); // Para que avise al DMA que termino
    
}

void configDAC(){
    
    // Menor consumo posible. Misma frecuencia del ADC(8500). 

    DAC_Init();
    DAC_SetBias(DAC_350uA);

    DAC_CONVERTER_CFG_T conf = {
        .doubleBuffer = ENABLE,
        .dmaCounter = ENABLE,
        .dmaRequest = ENABLE
    };

    DAC_SetDMATimeOut(2500); // CLK en 25 MHz. Frecuencia 10kHz. 2500
    DAC_ConfigDAConverterControl(&conf);

}

void configTMR(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 1 // Cargo 1 us
    };

    TIM_InitTimer(LPC_TIM0, &conf);

    TIM_MATCHCFG_T confM = {
        .channel = LPC_MATCH1,
        .intEn = DISABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOp = TIM_TOGGLE,
        .matchValue = 49 // Para que llegue a 100us
    };

    TIM_ConfigMatch(LPC_TIM0, &confM);

}

void configDMA1(){

    GPDMA_Init();

    conf.channelNum = GPDMA_CH_0;
    conf.transferSize = 2048; // Cantidad de muestras posible en 8192 bytes con una palabra
    conf.type = GPDMA_P2M; // ADC a memoria
    conf.srcMemAddr = ADC_ADDR;
    conf.dstMemAddr = GRABO;
    conf.srcConn = GPDMA_ADC;
    conf.dstConn = 0; // Memoria
    conf.src.width = GPDMA_WORD;
    conf.src.increment = DISABLE;
    conf.dst.width = GPDMA_WORD;
    conf.dst.increment = ENABLE;
    conf.intTC = ENABLE;
    conf.linkedList.nextLLI = 0;

    GPDMA_SetupChannel(&conf);

}

void configDMA2(){

    GPDMA_Init();

    conf.channelNum = GPDMA_CH_0;
    conf.transferSize = 2048; // Cantidad de muestras posible en 8192 bytes con una palabra
    conf.type = GPDMA_M2P; // Memoria a DAC
    conf.srcMemAddr = GRABO;
    conf.dstMemAddr = DAC_ADDR;
    conf.srcConn = 0; // Memoria
    conf.dstConn = GPDMA_DAC;
    conf.src.width = GPDMA_WORD;
    conf.src.increment = ENABLE;
    conf.dst.width = GPDMA_WORD;
    conf.dst.increment = DISABLE;
    conf.intTC = DISABLE;
    conf.linkedList.src = GRABO;
    conf.linkedList.dst = DAC_ADDR;
    conf.linkedList.nextLLI = (uint32_t)&conf.linkedList;
    conf.linkedList.control = 2048 | (2 << 18) | (2 << 21) | (1 << 26);

    GPDMA_SetupChannel(&conf);

}

void EINT1_IRQHandler(){

    if(estados != GRABANDO){

        GPDMA_ChannelStop(GPDMA_CH_0); // Lo apago de una porque no quiero que siga
        configDMA1();
        GPDMA_ChannelStart(GPDMA_CH_0);

        GPIO_ClearPins(0, (1 << 22));
        TIM_Enable(LPC_TIM0);
        estados = GRABANDO;

    }

    EXTI_ClearIntPending(EXTI_EINT1);

}

void DMA_IRQHandler(){

    /*
        Apagar el LED.
        Convertir las muestras del buffer al formato que necesita el DAC, en el mismo lugar de memoria.
        Reproducirlas por el DAC con DMA, en forma continua, a la misma frecuencia con la que se grabaron y con el menor consumo posible del DAC.
    */

    static volatile uint32_t* dato = (uint32_t *) GRABO; // Datos

    TIM_Disable(LPC_TIM0);
    GPIO_SetPins(0, (1 << 22));

    for(uint32_t i = 0; i < 2048; i++){
        dato[i] = (dato[i] & 0xFFC0) | (1 << 16);
    }

    configDMA2();
    GPDMA_ChannelStart(GPDMA_CH_0);
    estados = REPRODUCIENDO;

    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_0);

}