#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_dma.h"

void configDMA(void);
void configADC(void);
void configDAC(void);
void configTMR(void);

void ADC_IRQHandler(void);
void DMA_IRQHandler(void);

int main(){

    while(1){}

}
