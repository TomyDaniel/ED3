#include "LPC17xx.h"

#define ticks (1000000)

volatile int contador = 0;
volatile bool bandera = false;
volatile int j = 0;

void SysTick_Handler();

enum estado{
    E1 = 0,
    E2 = 1;
};

int main(){

    SysTick_Config(ticks);

    while(1){

        switch(estado){
            case(E1):
                if(bandera == false){
                    estado = E1;
                }else{
                    estado = E2;
                }
                break;
            case(E2):
                bandera = false;
                j++;
                estado = E1;
                break;
        }
        
    }

}

void SysTick_Handler(){

    contador++;

    if(contador == 100){
        bandera = true;
        contador = 0;
    }

}