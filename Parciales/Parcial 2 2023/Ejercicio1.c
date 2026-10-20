/*
Programar el microcontrolador LPC1769 en un código de lenguaje C para que, utilizando un timer y un pin de capture de esta placa sea posible 
demodular una señal PWM que ingresa por dicho pin (calcular el ciclo de trabajo y el periodo) y sacar una tensión continua proporcional al 
ciclo de trabajo a través del DAC de rango dinámico 0-2V con un rate de actualización de 0,5s del promedio de las últimos diez valores obtenidos en la captura.
*/

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpio.h"

void configTMR(void);
void configDAC(void);

static volatile uint32_t subida = 0;
static volatile uint32_t bajada = 0;
static volatile uint32_t contador = 0;

static volatile uint32_t reg[10];

int main(){

    configTMR();
    configDAC();

    while(1){
        __WFI();
    }

}

void configTMR(){

    TIM_TIMERCFG_T conf = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 1
    };

    TIM_InitTimer(LPC_TIM0, &conf);

    TIM_CAPTURECFG_T confC = {
        .channel = TIM_CAPTURE_0,
        .risingEn = ENABLE,
        .fallingEn = ENABLE,
        .intEn = ENABLE
    };

    TIM_ConfigCapture(LPC_TIM0, &confC);
    TIM_PinConfig(TIM_CAP0_0_P1_26);

    TIM_InitTimer(LPC_TIM1, &conf);

    TIM_MATCHCFG_T confM = {
        .channel = TIM_MATCH_0,
        .intEn = ENABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOp = TIM_NOTHING,
        .matchValue = 499999 // Para hacer los 500ms
    };

    TIM_ConfigMatch(LPC_TIM1, &confM);

    TIM_Enable(LPC_TIM0);
    TIM_Enable(LPC_TIM1);

    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);

}

void configDAC(){

    DAC_Init();
    DAC_SetBias(DAC_700uA);

}

void TIMER0_IRQHandler(){

    if(GPIO_ReadValue(PORT_0) & (1 << 26)){
        subida = TIM_GetCaptureValue(LPC_TIM0, TIM_CAPTURE_0);
    }else{
        bajada = TIM_GetCaptureValue(LPC_TIM0, TIM_CAPTURE_0);
    }

    reg[contador] = bajada - subida; // El tiempo en alto

    if(contador <= 8){
        contador++;
    }else{
        contador = 0;
    }

    TIM_ClearIntPending(LPC_TIM0, TIM_CR0_INT);

}

void TIMER1_IRQHandler(){

    volatile uint32_t dato = 0;

    for(int i = 0; i<10; i++){
        dato += reg[i];
    }

    DAC_UpdateValue(((dato/10)/1024) * 2); //El promedio de los tiempos dividido los bits de resolucion por el voltaje maximo que se pide

    TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);

}
