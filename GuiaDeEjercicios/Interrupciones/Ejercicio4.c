#include "LPC17xx.h"

#define BOTONA (1 << 0)
#define BOTONB (1 << 11)

#define DISPLAY (0x7F << 16)

void config();
void EINT1_IRQHandler();
void EINT3_IRQHandler();
void delay();

int secuenciaA[4] = {0x06, 0x4F, 0x7D, 0x77};
int secuenciaB[4] = {0x3F, 0x5B, 0x07, 0x39};

enum quien{
    PUERTO0,
    PUERTO2
};

enum quien estado = PUERTO0;

int main(){

    config();

    NVIC_EnableIRQ(EINT1_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);

    NVIC_SetPriority(EINT1_IRQn, 0); 
    NVIC_SetPriority(EINT3_IRQn, 1);

    while(1){
        if(estado == PUERTO0){
            for(int i=0; i<=3; i++){
                LPC_GPIO0 -> FIOSET = (secuenciaA[i] << 16);
                delay();
                LPC_GPIO0 -> FIOCLR = DISPLAY;
            }
        }

        if(estado == PUERTO2){
            for(int i=0; i<=3; i++){
                LPC_GPIO0 -> FIOSET = (secuenciaB[i] << 16);
                delay();
                LPC_GPIO0 -> FIOCLR = DISPLAY;
            }
        }
    }

}

void config(){

    // Configuracion display

    LPC_PINCON -> PINSEL1 &= ~(0x3FFF << 0);

    LPC_PINCON -> PINMODE1 &= ~(0x3FFF << 0);

    LPC_PINCON -> PINMODE1 |= (0x2AAA << 0);

    LPC_GPIO0 -> FIODIR |= DISPLAY;

    // Config boton A

    LPC_PINCON -> PINSEL0 &= ~(3 << 0);

    LPC_PINCON -> PINMODE0 &= ~(3 << 0);

    LPC_PINCON -> PINMODE0 |= (3 << 0);

    LPC_GPIO0 -> FIODIR &= ~BOTONA;

    LPC_GPIOINT -> IO0IntEnR |= BOTONA;

    // Config boton B

    LPC_PINCON -> PINSEL4 &= ~(3 << 22);

    LPC_PINCON -> PINSEL4 |= (1 << 22);

    LPC_PINCON -> PINMODE4 &= ~(3 << 22);

    LPC_GPIO2 -> FIODIR &= ~BOTONB;

    LPC_SC -> EXTMODE |= (1 << 1);

    LPC_SC -> EXTPOLAR &= ~(1 << 1);
}

void EINT1_IRQHandler(){

    estado = PUERTO2;

    LPC_SC -> EXTINT = 1;
}

void EINT3_IRQHandler(){

    estado = PUERTO0;

    LPC_GPIOINT -> IO0IntClr = BOTONA;
}

void delay(){
    for(int i=0; i<2500; i++){
        for(int j=0; j<2500; j++){}
    }
}
