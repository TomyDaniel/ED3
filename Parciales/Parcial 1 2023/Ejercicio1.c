#include "lpc17xx.h"

#define Botton1 (1 << 11)
#define Botton2 (1 << 12)
#define Reset (1 << 10)
#define diplay (0x7F << 0)

volatile uint8_t pausa = 0;
volatile uint8_t contador = 0;
volatile uint8_t velocidad = 0;
const uint8_t tabla_7seg[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x67  // 9
};

void SysTick_Handler(void);
void configSysTick(void);
void configPin(void);
void configEINT(void);
void EINT0_Handler(void);
void EINT1_Handler(void);
void EINT2_Handler(void);

int main(void){

    SystemInit();

    configPIN();

    configSysTick();

    configEINT();

    while(1){
        __WFI();
    }

}

void configPIN(void){

    // Botones
    LPC_PINCON->PINSEL4 &= ~(0x3F << 20); // 0001 0101 Para EINT

    LPC_PINCON->PINSEL4 |= (0x15 << 20);

    LPC_GPIO2->FIODIR &= ~(0b111 << 10);

    LPC_PINCON->PINMODE4 &= ~(0x3F << 20);

    // Display
    LPC_PINCON->PINSEL0 &= ~(0x3FFF << 0); // 0000 0000 0000 0000 -> 0011 1111 1111 1111 0x3FFF

    LPC_GPIO0->FIODIR |= (0x7F << 0);
}

void configEINT(void){
    
    LPC_SC->EXTINT |= (1 << 0) | (1 << 1) | (1 << 2);

    LPC_SC->EXTMODE |= (1 << 0) | (1 << 1) | (1 << 2);

    LPC_SC->EXTPOLAR &= ~((0 << 0) | (0 << 1) | (0 << 2));

}

void EINT0_Handler(void){
    
    pausa = 1;
    contador = 0;

    LPC_SC->EXTINT |= (1 << 0); // limpio la flag

    LPC_GPIO0->FIOCLR = 0x7F; // reseteo el 7 segmentos
    LPC_GPIO0->FIOSET = tabla_7seg[contador]; // coloco el valor 0

}

void EINT1_Handler(void){

    if(pausa == 0){
        pausa = 1;
    }else{
        pausa = 0;
    }

    LPC_SC->EXTINT |= (1 << 1);

}

void EINT2_Handler(void){

    if(velocidad == 0){
        velocidad = 1;
    }else{
        velocidad = 0;
    }

    LPC_SC->EXTINT |= (1 << 2);

}

void SysTick_Handler(void){

    static uint16_t count = 0;

    if(pausa == 1){
        return;
    }else{
        if(velocidad == 1){
            contador++;
        }else{
            count++;
            if(count >= 1000){
                count = 0;
                contador++;
            }
        }
    }

    if(contador == 10){
        contador = 0;
    }

    LPC_GPIO0->FIOCLR = 0x7F;

    LPC_GPIO0->FIOSET = tabla_7seg[contador];

}

void configSysTick(void){
    SysTick_Config(16000);
}