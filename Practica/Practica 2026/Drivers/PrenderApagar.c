#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

#define ticks (999999)


void config(){
	PINSEL_CFG_T PIN_CFG;

	PIN_CFG.port = 0;
	PIN_CFG.pin = 22;
	PIN_CFG.mode = 2;
	PIN_CFG.func = 0;

	PINSEL_ConfigPin(&PIN_CFG);
	GPIO_SetDir(0, (1 << 22), 1);
	GPIO_SetPins(0, (1 << 22));

}

void SysTick_Handler();

uint32_t contador = 0;

int main(){

	contador = 100;

	config();

    SysTick_Config(ticks);

    while(1){}

}

void SysTick_Handler(){
	contador--;
	if(contador <= 0){
		if(GPIO_ReadValue(0) & (1 << 22)){
			GPIO_ClearPins(0, (1 << 22));
			contador = 100;
		}else{
			GPIO_SetPins(0, (1 << 22));
			contador = 100;
		}
	}
}
