#include "LPC17xx.h"

#define PINES (0x7F << 0)
#define BOTON (1 << 10)

void config();
void EINT0_IRQHandler();
void delay();

int secuencia[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int contador = 0;
int dirAde = 1;
int dirAtr = -1;

int direccionActual = 1;

enum estado { CORRIENDO, DETENIDO };

enum estado momento = CORRIENDO;


int main(){

    config();

    NVIC_EnableIRQ(EINT0_IRQn);

    while(1){

        if (momento == CORRIENDO){
            if(direccionActual == dirAde){
                if(contador >= 15){
                    contador = 0;
                    LPC_GPIO0 -> FIOCLR = PINES;
                    LPC_GPIO0 -> FIOSET = secuencia[contador];
                }else{
                    LPC_GPIO0 -> FIOSET = secuencia[contador];
                    delay();
                    LPC_GPIO0 -> FIOCLR = PINES;
                    contador = contador+direccionActual;
                }
            }else{
                if(contador <= 0){
                    contador = 15;
                    LPC_GPIO0 -> FIOCLR = PINES;
                    LPC_GPIO0 -> FIOSET = secuencia[contador];
                }else{
                    LPC_GPIO0 -> FIOSET = secuencia[contador];
                    delay();
                    LPC_GPIO0 -> FIOCLR = PINES;
                    contador = contador+direccionActual;
                }
            }
        }else{
            LPC_GPIO0 -> FIOSET = secuencia[contador];
        }

    }

}

void config(){

    // Configuracion display

    LPC_PINCON -> PINSEL0 &= ~(0x3FFF << 0);

    LPC_PINCON -> PINMODE0 &= ~(0x3FFF << 0);

    LPC_PINCON -> PINMODE0 |= (0x2AAA << 0);

    LPC_GPIO0 -> FIODIR |= PINES;

    LPC_GPIO0 -> FIOSET = secuencia[contador];

    // Configuracion boton

    LPC_PINCON -> PINSEL4 &= ~(3 << 20);

    LPC_PINCON -> PINSEL4 |= (1 << 20); // EINT0

    LPC_PINCON -> PINMODE4 &= ~(3 << 20);

    LPC_PINCON -> PINMODE4 |= (1 << 21);

    LPC_GPIO2 -> FIODIR |= BOTON;

    LPC_SC -> EXTMODE |= (1 << 0);

    LPC_SC -> EXTPOLAR |= (1 << 0);

}

void EINT0_IRQHandler(){

    if(LPC_SC -> EXTPOLAR & 1){
        momento = DETENIDO;
        LPC_SC -> EXTPOLAR &= ~(1 << 0);
    }else{
        momento = CORRIENDO;
        LPC_SC -> EXTPOLAR |= (1 << 0);

        if(direccionActual == dirAde){
            direccionActual = dirAtr;
        }else{
            direccionActual = dirAde;
        }

    }
    LPC_SC -> EXTINT = 1;
}

void delay(){

    for(int i=0; i<2500; i++){
        for(int j=0; j<2500; j++){}
    }

}
