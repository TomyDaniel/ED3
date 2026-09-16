# EJERCICIO N°2: (40 %)

El siguiente código pretende realizar el parpadeo de un LED ubicado en el pin P1.18 a partir de la medición del período de una señal cuadrada que ingresa por el pin asociado a la interrupción EINT1. Se utiliza SysTick como base de tiempo, para medir el período de la señal cuadrada de entrada. Luego, se obtiene una frecuencia proporcional a dicho período que limita el rango de parpadeo del LED en torno a los 100–1000 ms (visible al ojo humano).

Por tanto:

Si el período de la señal cuadrada que ingresa es menor a 100 ms, el LED debe parpadear con un período de 100 ms.  
Si el período de la señal cuadrada que ingresa se encuentra en el rango de 100 a 1000 ms, en el LED debe parpadear con un período lineal a la señal cuadrada (es una función lineal).  
Si el período de la señal cuadrada que ingresa es mayor a 1000 ms, entonces el LED debe parpadear con un período de 1000 ms.

Se pide:

Complete el código.  
Configure la prioridad relativa de SysTick y EINT1 según considere adecuado.  
Justifique la elección de la base de tiempo.  
Indique el modo de interrupción de EINT1.  

```
#include "LPC17xx.h"
#define LED (1<<18)

volatile uint32_t t, t_prev, periodo;

void SysTick_Handler(void){t=1;}

void EINT1_IRQHandler(void){
    if(LPC_SC->EXTINT & (1<<1)){
        uint32_t now=SysTick->VAL;
        periodo=now-t_prev;
        t_prev=now;
        LPC_SC->EXTINT=(1<<1);
    }
}

void systick_config(void){
    SysTick->LOAD=110000;
    SysTick->VAL=0;
    SysTick->CTRL=(1<<0)&(1<<1)&(1<<2);
}

void eint1_config_reg(void){
    LPC_PINCON->PINSEL4&=~(3<<22);
    LPC_PINCON->PINSEL4|=(1<<22);
    LPC_GPIOINT->IO2IntEnR|=(1<<11);
    LPC_GPIOINT->IO2IntEnF&=~(1<<11);
    NVIC_EnableIRQ(EINT3_IRQn);
}

int main(void){
    systick_config();
    eint1_config_reg();
    while(1){
        // TODO: parpadear LED con periodo medido (100-1000
ms)
    }
}
```