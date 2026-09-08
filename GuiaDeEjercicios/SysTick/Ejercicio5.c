#include "LPC17xx.h"

#define ticks (0.1 * 100000000 - 1)

#define LED (1 << 22)
#define LEDs (0xF << 0)

void config();
void SysTick_Handler();

int contA = 5;
int contB = 2;

int secuencia = 0;

int main(){

    config();

    SysTick_Config(ticks);

    while(1){}

}

void config(){

    // Configuracion LED P0.22

    LPC_PINCON -> PINSEL1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 |= (1 << 13);

    LPC_GPIO0 -> FIODIR |= LED;

    LPC_GPIO0 -> FIOSET = LED;

    // Configuracion 4 LED

    LPC_PINCON -> PINSEL0 &= ~(0xFF << 0);

    LPC_PINCON -> PINMODE0 &= ~(0xFF << 0);

    LPC_PINCON -> PINMODE0 |= (0xAA << 0);

    LPC_GPIO0 -> FIODIR |= LEDs;

    LPC_GPIO0 -> FIOCLR = LEDs;

}

void SysTick_Handler(){

    contA--;
    contB--;

    if(contA == 0){
        if(LPC_GPIO0 -> FIOPIN & LED){
            LPC_GPIO0 -> FIOCLR = LED;
        }else{
            LPC_GPIO0 -> FIOSET = LED;
        }
        contA = 5;
    }

    if(contB == 0){
        if(secuencia < 15){
            secuencia++;
            LPC_GPIO0 -> FIOCLR = LEDs;
            LPC_GPIO0 -> FIOSET = secuencia;
        }else{
            secuencia = 0;
            LPC_GPIO0 -> FIOCLR = LEDs;
            LPC_GPIO0 -> FIOSET = secuencia;
        }
        contB = 2;
    }

}
