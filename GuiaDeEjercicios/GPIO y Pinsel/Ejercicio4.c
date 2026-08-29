#include "LPC17xx.h"

int seg = 2500;

void config();
void delay();

int numeros[16] = {
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F,
    0x77, 
    0x7C,
    0x39,
    0x5E,
    0x79,
    0x71
};

int main(){

    config();

    int i = 0;

    while(1){

        LPC_GPIO0 -> FIOSET = numeros[i];
        delay();
        LPC_GPIO0 -> FIOCLR = numeros[i];
        delay();

        i++;

        if(i >= 16){
            i = 0;
        }

    }

}

void config(){

    LPC_PINCON -> PINSEL0 &= ~(0x3FFF << 0);

    LPC_GPIO0 -> FIODIR |= (0x7F << 0);

    LPC_GPIO0 -> FIOCLR = (0x7F << 0);
    
}

void delay(){

    for(int i=0; i < seg; i++){
        for(int j=0; j < seg; j++){}
    }
}
