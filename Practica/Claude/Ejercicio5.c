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

void configTMR(void);
void configDMA(void);

int main(){

    configTMR();
    configDMA();

    while(1){}

}

void configTMR(){

    
}