#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"

#define DIR1 0x2007E000 // Mitad del banco 0
#define DIR2 0x20080000
#define ADC_RATE 160000
#define ticks (0.1 * 80000000)
#define METRICA 8192

static uint32_t *direccion1 = DIR1;
static uint32_t *direccion2 = DIR2;

void config(void);
void configADC(void);
void configDMA(void);

void ADC_IRQHandler(void);
void EINT0_IRQHandler(void);
void SysTick_Handler(void);
void GPDMA_IRQHandler(void);

enum estado{
    IDLE,
    ADC,
    DMA
};

enum estado tipo = IDLE;
uint16_t contador = 10; // Contador para llegar al segundo
uint32_t idx = 0;

int main(){

    config();
    configADC();
    configDMA();

    SysTick_Config(ticks); // Configuro el timer en 100 ms

    while(1){

    }

}

void config(){

    /* Configuracion P2.10
    PINSEL_CFG_T conf = {
        .port = 2;
        .pin = 10;
        .func = PINSEL_FUNC_01;
        .mode = PINSEL_PULLDOWN;
    }

    PINSEL_ConfigPin(&conf);

    GPIO_SetDir(2, (1 << 10), GPIO_INPUT);

    */ // Creo que no es necesario

    EXTI_Init();

    EXTI_PinConfig(EXTI_EINT0, EXTI_PULLDOWN);

    EXTI_CFG_T conf = {
        .line = EXTI_EINT0;
        .mode = EXTI_EDGE_SENSITIVE;
        .polarity = EXTI_RISING_EDGE;
    }

    EXTI_Config(&conf);
    EXTI_ConfigEnable(&conf);

    EXTI_EnableIRQ(EXTI_EINT0);

}

void configADC(){

    // 160 Khz de Inicio del ADC. Doble del ancho de banda de la senial
    ADC_Init(ADC_RATE);
    // No dice canal pero uso el 0
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_PowerUp();
    ADC_StartCmd(ADC_START_ON_EINT0);
    ADC_ChannelEnable(ADC_CHANNEL_0);

}

void configDMA(){

    GPDMA_Init();

    GPDMA_Channel_CFG_T config = {
        .channelNum = GPDMA_CH_0;
        .transferSize = 0; // Nose, lo que recolecte el ADC en 1 segundo?
        .type = GPDMA_P2M; // ADC a Mem
        .srcMemAddr = DIR1;
        .dstMemAddr = DIR2;
        .srcConn = GPDMA_ADC;
        .dstConn = 0; // Memoria
        .src.width = GPDMA_HALFWORD; // Supongo que la cantidad de bits del ADC. 16?
        .src.burst = 0; // No uso burst
        .src.increment = DISABLE;
        .dst.width = GPDMA_WORD;
        .src.burst = 0;
        .src.increment = ENABLE; // La idea es que se vaya incrementando la memoria
        .intTC = DISABLE;
        .intErr = ENABLE;
    }

    GPDMA_SetupChannel(&config);
    GPDMA_ChannelStart(GPDMA_CH_0);

}

void EINT0_IRQHandler(){

    switch (tipo){
        case IDLE:
            tipo = ADC;
            // Ya se inicializo el ADC por la configuracion previa en configADC
            break;
        case ADC:
            tipo = DMA
            break;
        
        default:
            tipo = ADC
            break;
    }
}

void SysTick_Handler(){

    contador--;

    if(contador <= 0){
        switch (tipo){
            case ADC:
                direccion1 = ADC_ChannelGetData(ADC_CHANNEL_0);
                
                if(idx < METRICA){
                    idx++;
                    &direccion1++;
                }else{
                    idx = 0;
                    direccion1 = DIR1;
                }

                ADC_StartCmd(ADC_START_NOW);
                break;
            
            case DMA:
                
                break;
        }
    }
}

void GPDMA_IRQHandler(){

}
