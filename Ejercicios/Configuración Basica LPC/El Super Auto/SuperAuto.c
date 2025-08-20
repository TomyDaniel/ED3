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

#define BUTTON_MODE (1 << 0)
#define BUTTON_SPEED (1 << 1)

void delay(int cantidad);

void configuraciones(void);


int main(void) {

    configuraciones();

    int leds [6] = {LED1, LED2, LED3, LED4, LED5, LED6};
    
    int modo = 1;

    int modoSpeed = 1;

    int velocidad = 150;

    while(1) {

        if((LPC_GPIO0->FIOPIN & BUTTON_MODE) == 0){
            modo++;
            if(modo > 3){
                modo = 1;
            }
            delay(100); // funciona como antirebote
        }

        if((LPC_GPIO0->FIOPIN & BUTTON_SPEED) == 0){
            modoSpeed++;
            if(modoSpeed > 3){
                modoSpeed = 1;
            }
            delay(100);
        }

        if(modoSpeed == 1){
            velocidad = 300;
        }else if(modoSpeed == 2){
            velocidad = 150;
        }else{
            velocidad = 50;
        }

        if(modo == 1){
            for(volatile int i=0; i<6; i++){
                LPC_GPIO1->FIOSET = leds[i];
                delay(velocidad);
                LPC_GPIO1->FIOCLR = leds[i];
                delay(velocidad);
            }

            for(volatile int i=4; i>0; i--){
                LPC_GPIO1->FIOSET = leds[i];
                delay(velocidad);
                LPC_GPIO1->FIOCLR = leds[i];
                delay(velocidad);
            }
        }else if(modo == 2){
            for(volatile int i=0; i<6; i++){
                LPC_GPIO1->FIOSET = leds[i];
                delay(velocidad);
            }

            for(volatile int i=0; i<6; i++){
                LPC_GPIO1->FIOCLR = leds[i];
                delay(velocidad);
            }
        }else{
            for(volatile int i=0; i<3; i++){
                LPC_GPIO1->FIOSET = leds[i] | leds[5 - i];;
                delay(velocidad);
                LPC_GPIO1->FIOCLR = leds[i] | leds[5 - i];;
                delay(velocidad);
            }

            for(volatile int i=1; i>=0; i--){
                LPC_GPIO1->FIOSET = leds[i] | leds[5 - i];;
                delay(velocidad);
                LPC_GPIO1->FIOCLR = leds[i] | leds[5 - i];;
                delay(velocidad);
            }
        }

    }
    return 0 ;
}

void delay(int cantidad){
	for(volatile int i=0; i<timing * cantidad; i++){}
}


void configuraciones(void){

    // configuración de los leds

	LPC_PINCON->PINSEL3 &= ~(0xFFF << 4);

	LPC_GPIO1->FIODIR |= (0x3F << 18);

    // configuración de los botones

    LPC_PINCON->PINSEL0 &= ~(0xF << 0);

    LPC_GPIO0->FIODIR &= ~(BUTTON_MODE | BUTTON_SPEED);

}
