/*
Programar el microcontrolador LPC1769 en un código de lenguaje C para que mediante su ADC digitalice  una señal analógica cuyo ancho de banda es de 16 khz. 
La señal analógica tiene una amplitud de pico máxima positiva de 3.3 voltios. Los datos deben ser guardados utilizando el Hardware GDMA en la primera mitad 
de la memoria SRAM ubicada en el bloque AHB SRAM - bank 0, de manera tal que permita almacenar todos los datos posibles que esta memoria nos permita. 
Los datos deben ser almacenados como un buffer circular conservando siempre las últimas muestras.

Por otro lado se tiene una forma de onda como se muestra en la imagen a continuación. 
Esta  señal debe ser generada por una función y  debe ser reproducida por el DAC desde la segunda mitad de AHB SRAM - bank 0 memoria utilizando 
DMA de tal forma que se logre un periodo de 614us logrando la máxima resolución y máximo rango de tensión.
Durante operación normal se debe generar por el DAC la forma de onda mencionada como wave_form. 

Se debe indicar cuál es el mínimo incremento de tensión de salida de esa forma de onda.
Cuando interrumpe una extint conectada a un pin, el ADC configurado debe completar el ciclo de conversión que estaba realizando, y ser detenido, 
a continuación se comienzan a sacar las muestras del ADC por el DAC utilizando DMA y desde las posiciones de memoria originales.
Cuando interrumpe nuevamente en el mismo pin, se vuelve a repetir la señal del DAC generada por la forma de onda de wave_form previamente almacenada 
y se arranca de nuevo la conversión de datos del adc. Se alterna así entre los dos estados del sistema con cada interrupción externa.
Suponer una frecuencia de core cclk de 80 Mhz. El código debe estar debidamente comentado. 
*/

#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_dma.h"
#include "lpc17xx_exti.h"

#define BANCO0_INF 0x2007C000
#define BANCO0_SUP 0x2007E000

void configEXTI(void);
void configADC(void);
void configDAC(void);
void configDMA(void);

enum{
    CASO1,
    CASO2
} estado = CASO1;

int main(){

    configEXTI();
    configADC();
    configDAC();
    configDMA();

    while(1){
        __WFI();
    }

}

void configEXTI(){

    EXTI_Init();

    EXTI_CFG_T conf = {
        .line = EXTI_EINT0,
        .mode = EXTI_EDGE_SENSITIVE,
        .polarity = EXTI_RISING_EDGE
    };

    EXTI_PinConfig(EXTI_EINT0, EXTI_PULLDOWN);
    EXTI_ConfigEnable(&conf);

}

void configADC(){

    ADC_Init(40000); // 40 kHz > 2 * 16 kHz
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_BurstEnable();
    ADC_PowerUp();
    ADC_StartCmd(ADC_START_CONTINUOUS);
    ADC_ChannelEnable(ADC_CHANNEL_0);
    ADC_IntEnable(ADC_CHANNEL_0);

}

void configDAC(){

    DAC_Init();
    DAC_SetBias(DAC_700uA);

}

void configDMA(){

    GPDMA_Init();

    switch(estado){
        case CASO1:

        GPDMA_Channel_CFG_T conf1 = {
            .channelNum = GPDMA_CH_0,
            .transferSize = 2048,
            .type = GPDMA_P2M,
            .srcMemAddr = ADC_CH0,
            .dstMemAddr = BANCO0_INF,
            .srcConn = GPDMA_ADC,
            .dstConn = 0,
            .src.width = GPDMA_WORD,
            .src.increment = DISABLE,
            .dst.width = GPDMA_WORD,
            .dst.increment = ENABLE,
            .intTC = DISABLE,
            .intErr = DISABLE,
            .linkedList.srcAddr = ADC_CH0,
            .linkedList.dstAddr = BANCO0_INF,
            .linkedList.dstAddr = BANCO0_INF,
            .linkedList.control = 2048 | // Todas las otras config;
        }

        GPDMA_SetupChannel(&conf1)
    }
}