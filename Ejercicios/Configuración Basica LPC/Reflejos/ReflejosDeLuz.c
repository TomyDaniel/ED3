// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 10000

unsigned int semilla = 1234;

#define LED1 (1 << 18)
#define LED2 (1 << 19)
#define LED3 (1 << 20)
#define LED4 (1 << 21)
#define LED5 (1 << 22)
#define LED6 (1 << 23)

#define BUTTON1 (1 << 0)
#define BUTTON2 (1 << 1)
#define BUTTON3 (1 << 2)
#define BUTTON4 (1 << 3)


void delay(int cantidad);

void configuraciones(void);

unsigned int mi_rand(void);

int main(void) {

    configuraciones();

    int leds[6] = {LED1, LED2, LED3, LED4};

    int btt[4] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};

    int estadoJuego = 1;

    int indice = 0;

    int tiempoDef = 1000;

    while(1) {

        switch(estadoJuego){
            case 1: // Esperando inicio
                for(volatile int i=0; i<4; i++){
                    LPC_GPIO1->FIOSET = leds[i];
                    delay(300);
                    LPC_GPIO1->FIOCLR = leds[i];
                    delay(300);
                }
                for(volatile int i=0; i<4; i++){
                    if((LPC_GPIO0->FIOPIN & btt[i]) == 0){
                        estadoJuego = 2;
                    }
                }
                break;
            case 2: // Encencido LED
                indice = mi_rand() % 4;
                estadoJuego = 3;
                break;
            case 3: // Input

                int tiempoTrans = 0;
                int acierto = 0;

                LPC_GPIO1->FIOSET = leds[indice];
                
                while(tiempoTrans < tiempoDef){
                    if ((LPC_GPIO0->FIOPIN & btt[indice]) == 0) {
                        acierto = 1;
                        break;      
                    }

                    for (int i = 0; i < 4; i++) {
                        if (i != indice) {
                            if ((LPC_GPIO0->FIOPIN & btt[i]) == 0) {
                                acierto = 2;
                                break;
                            }
                        }
                    }
                    if (acierto == 2) {
                        break;
                    }

                    delay(1);
                    tiempoTrans++;
                }

                LPC_GPIO1->FIOCLR = leds[indice];

                if (acierto == 1) {
                    
                    for(volatile int i=0; i<3; i++){
                        LPC_GPIO1->FIOSET = LED5;
                        delay(tiempoDef);
                        LPC_GPIO1->FIOCLR = LED5;
                        delay(tiempoDef);
                    }

                    tiempoDef -= 50;
                    estadoJuego = 2;

                } else { 
                    
                    LPC_GPIO1->FIOCLR = leds[indice];
                    LPC_GPIO1->FIOSET = LED6;

                    for(volatile int i=0; i<4; i++){
                        LPC_GPIO1->FIOSET = leds[i];
                        delay(50);
                        LPC_GPIO1->FIOCLR = leds[i];
                        delay(50);
                    }

                    LPC_GPIO1->FIOCLR = LED6;
                    
                    estadoJuego = 1;
                }
            break;

        }
    }
    return 0 ;
}

void delay(int cantidad){
	for(volatile int i=0; i<timing * cantidad; i++){}
}


void configuraciones(void){

    // config. LED´s

    LPC_PINCON->PINSEL3 &= ~(0xFFF << 4);

    LPC_GPIO1->FIODIR |= (0x3F << 18);

    // config. botones

    LPC_PINCON->PINSEL0 &= ~(0xFF << 0);

    LPC_GPIO0->FIODIR &= ~(BUTTON1 | BUTTON2 | BUTTON3 | BUTTON4);
}

unsigned int mi_rand(void) {
    semilla = semilla * 1103515245 + 12345;  // cambia la semilla
    return (semilla >> 16) & 0x7FFF;         // devuelve un número pseudoaleatorio
}