// Created by Tomas

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#define timing 10000

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


int main(void) {

    configuraciones();

    int leds[6] = {LED1, LED2, LED3, LED4, LED5, LED6};

    int btt[4] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};

    int estadoJuego = 0;

    while(1) {

        for(volatile int i=0; i<5; i++){
            LPC_GPIO1->FIOSET = leds[i];
            delay(300);
            LPC_GPIO1->FIOCLR = leds[i];
            delay(300);

            for(volatile int j=0; j<4; j++){
                if((LPC_GPIO0->FIOPIN & btt[j]) == 0){
                    for(volatile int k=0; k<6; k++){
                        if((LPC_GPIO1->FIOPIN & leds[k]) == 0){
                            LPC_GPIO1->FIOSET = leds[k];
                            delay(100);
                            if((LPC_GPIO0->FIOPIN & btt[k]) == 0){
                                LPC_GPIO1->FIOCLR = leds[k];
                                for(volatile int o=0; o<3; o++){
                                    LPC_GPIO1->FIOSET = LED5;
                                    delay(25);
                                    LPC_GPIO1->FIOCLR = LED5;
                                    delay(25);
                                }
                            }else{
                                LPC_GPIO1->FIOSET = LED6;
                                for(volatile int p=0; p<5; p++){
                                    LPC_GPIO1->FIOSET = leds[p];
                                    delay(25);
                                    LPC_GPIO1->FIOCLR = leds[p];
                                    delay(25);
                                }
                                LPC_GPIO1->FIOCLR = LED6;
                            }
                        }
                    }
                }
            }
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

    LPC_PINCON->PINSEL0 &= ~(0xF << 0);

    LPC_GPIO0->FIODIR &= ~(BUTTON1 | BUTTON2 | BUTTON3 | BUTTON4);
}
