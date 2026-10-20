/*
Por el pin CAP0.0 ingresa una señal PWM de frecuencia desconocida, entre 1 kHz y 5 kHz, y ciclo de trabajo variable.

Se pide:

Usando el Timer0 en modo captura, medir el período y el tiempo en alto de la señal en cada ciclo, y calcular el ciclo de trabajo en %.
Guardar los últimos 8 ciclos de trabajo medidos en un buffer circular en RAM.
Cada 200 ms, medidos con el Timer1 por interrupción, calcular el promedio de esos 8 valores y sacar por el DAC una tensión continua proporcional:
0 % → 0 V
100 % → 2 V
El DAC se actualiza por software, sin DMA, con el menor consumo posible.

Responder:

¿Con qué resolución (tick del Timer0) medís, y cuántas cuentas tenés en el período más corto y en el más largo?
¿Cómo hacés para capturar los dos flancos y saber cuál de los dos llegó?
¿Qué valor de DAC corresponde a 2 V? ¿Cuál es el mínimo incremento de tensión de la salida?
*/

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"

#define BUFFER 0x20080000

void configTMR(void);
void configDAC(void);

void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);

static volatile uint32_t periodo = 0;
static volatile uint32_t subida = 0;
static volatile uint32_t bajada = 0;
static volatile uint8_t contador = 0;
static volatile uint8_t ready = 0;

int main(){

    configTMR();
    configDAC();

    while(1){}

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

    TIM_Enable(LPC_TIM0);

    TIM_TIMERCFG_T confTM = {
        .prescaleOpt = TIM_US,
        .prescaleValue = 100000 // 100 ms
    };

    TIM_InitTimer(LPC_TIM1, &confTM);

    TIM_MATCHCFG_T confM = {
        .channel = TIM_MATCH_0,
        .intEn = ENABLE,
        .stopEn = DISABLE,
        .resetEn = ENABLE,
        .extOpt = TIM_NOTHING,
        .matchValue = 1
    };

    TIM_ConfigMatch(LPC_TIM1, &confM);

    TIM_Enable(LPC_TIM1);

    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);

}

void configDAC(){
    
    DAC_Init();
    DAC_SetBias(DAC_350uA);

}

void TIMER1_IRQHandler(){

    volatile uint32_t dato = 0;

    for(uint8_t i=0; i<8; i++){
        dato += *(BUFFER + i); // Extraigo el dato
    }

    dato = (dato * 620) / 100;

    DAC_UpdateValue(dato);

    TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);

}

void TIMER0_IRQHandler(){

    static volatile uint32_t dato = 0;
    dato = TIM_GetCaptureValue(LPC_TIM0, TIM_CAPTURE_CH0);

    if((GPIO_ReadValue(PORT_1) & (1 << 26))){ // Si esta en alto cargo los valores
        periodo = dato - subida;
        subida = dato;
    }else{
        bajada = dato;
        ready = 1;
    }

    if(ready == 1){
        *(BUFFER + contador) = ((bajada-subida)*100)/(periodo);
        ready = 0;

        if(contador <= 6){  
            contador++;
        }else{
            contador = 0;
        }
    }

    TIM_ClearIntPending(LPC_TIM0, TIM_CR0_INT);

}