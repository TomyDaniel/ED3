#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"

#define ticks (0.001 * 100000000)

void config();
void configDAC();
void SysTick_Handler();

uint32_t medicion = 0;

int main(){

	config();

    configDAC();

    SysTick_Config(ticks);

    while(1){}

}

void config(){

    // Pin 0.26

    PINSEL_CFG_T p = {0};

    p.port = 0;
    p.pin = 26;
    p.func = 2;
    p.mode = 2;

    PINSEL_ConfigPin(&p);
    GPIO_SetDir(0, (1 << 26), 1); // Puerto 0; Mascara; Salida

}

void configDAC(){

    DAC_Init();
    DAC_UpdateValue(medicion);
    DAC_SetBias(0);

}

void SysTick_Handler(){

    if(medicion <= 999){
        medicion++;
    }else{
        medicion = 0;
    }

    DAC_UpdateValue(medicion);

}
