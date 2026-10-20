/*
Diseñe un sistema con dos modos de operación alternativos mediante un pulsador externo.

Modo 1: Se recibe una señal PWM de 30 kHz por un pin de Captura del Timer, cuyo ciclo de trabajo varía entre 0% y 50%. El sistema debe 
medir el ancho de pulso y generar una tensión continua en el DAC estrictamente proporcional, mapeando el 0% a OV y el 50% a 3.3V.

Modo 2: Se adquiere una señal analógica de 20 kHz por el ADC para ser reproducida inmediatamente por el DAC. 
El hardware del DAC debe estar configurado para garantizar el menor consumo de corriente posible durante la reconstrucción de esta señal

*/

/*
  ----
_|    |______ 3,3v ____________ 0v
*/

#include "LPC17xx.h"
#include "lpc17xx_dac"
#include "lpc17xx_adc"
#include "lpc17xx_timer"

void configDAC();
void configADC();
void configEINT();
void configTMR();

enum{
    IDLE,
    MODO1,
    MODO2
}estado = MODO1;

int main(){

    configEINT();
    configTMR();
    configADC();
    configDAC();
    
    while(1){}

}

void configEINT(){

    EXTI_Init();

    EXTI_CFG_T conf = {
        .line = EXTI_EINT0,
        .mode = EXTI_EDGE_SENSITIVE,
        .polarity = EXTI_RISING_EDGE
    };

    EXTI_ConfigEnable(&conf);
    EXTI_PinConfig(EXTI_EINT0, EXTI_PULLDOWN);

}

void configDAC(){

    DAC_Init(200000); // 200000 > 30 kHz * 2. Nyquist
    DAC_SetBias(DAC_350uA); // 400 kHz < 65000

}

void configADC(){

    ADC_Init(65000); // 65000 > 30 kHz * 2
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_ChannelEnable(ADC_CHANNEL_0);
    ADC_IntEnable(ADC_INT_CH0);

}

void configTMR(){

    TIM_CAPTURECFG_T conf = {
        .channel = TIM_CAPTURE_0,
        .risingEn = ENABLE,
        .fallingEn = ENABLE,
        .intEn = ENABLE
    };

    TIM_ConfigCapture(LPC_TIM0, &conf);
    TIM_PinConfig(TIM_CAP0_0_P1_26);

    NVIC_EnableIRQ(TIMER0_IRQn);

}

void EINT0_IRQHandler(){

    switch(estado){
        case IDLE:
            estado = MODO1;
            TIM_Enable(LPC_TIM0);
            ADC_PowerDown();
            break;

        case MODO1:
            estado = MODO2;
            TIM_Disable(LPC_TIM0);
            ADC_PowerUp();
            ADC_StartCmd(ADC_START_NOW);
            break;

        case MODO2:
            estado = MODO1;
            TIM_Enable(LPC_TIM0);
            ADC_PowerDown();
            break;  
    }

    EINT_ClearFlag(EXTI_EINT0);

}

void TIMER0_IRQHandler(){

    static volatile uint32_t dato = 0;

    dato = TIM_GetCaptureValue(LPC_TIM0, TIM_CAPTURE_0);

    if(dato > /* tal valor*/){
        DAC_UpdateValue(1023);
    }else{
        DAC_UpdateValue(0);
    }

    TIM_ClearIntPending(LPC_TIM0, TIM_CR0_INT);

}

void ADC_IRQHandler(){

    static volatile uint16_t dato = 0;

    dato = ADC_ChannelGetData(ADC_CHANNEL_0);

    DAC_UpdateValue(dato); // Estaria mal pq el adc tiene 12 bits, y el dac solo 10

}
