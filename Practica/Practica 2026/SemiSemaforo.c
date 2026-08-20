#include "LPC17xx.h"

#define LEDROJO (1 << 22)
#define LEDAZUL (1 << 26)
#define LEDVERDE (1 << 25)

#define ticks (1000000)

static uint32_t count = 0;

uint32_t marca_r = 0;
uint32_t marca_a = 0;
uint32_t marca_v = 0;

void config();
void SysTick_Handler();

enum estados{
    Rojo,
    Azul,
    Verde
};

enum estados cambio = Rojo;

int main(){

    config();
    SysTick_Config(ticks);

    marca_r = count;

    while(1){

        switch(cambio){

            case(Rojo):

                if(count - marca_r < 400){
                    cambio = Rojo;

                    // Prender led rojo
                    LPC_GPIO0 -> FIOCLR = LEDROJO;

                    // Apagar led's restantes
                    LPC_GPIO3 -> FIOSET = LEDAZUL;
                    LPC_GPIO3 -> FIOSET = LEDVERDE;

                }else{
                    cambio = Azul;

                    marca_a = count;
                }

                break;

            case(Azul):

                if(count - marca_a < 350){
                    cambio = Azul;

                    // Prender led rojo
                    LPC_GPIO0 -> FIOCLR = LEDAZUL;

                    // Apagar led's restantes
                    LPC_GPIO3 -> FIOSET = LEDROJO;
                    LPC_GPIO3 -> FIOSET = LEDVERDE;

                }else{
                    cambio = Verde;

                    marca_v = count;
                }

                break;

            case(Verde):

                if(count - marca_v < 300){
                    cambio = Verde;

                    // Prender led rojo
                    LPC_GPIO0 -> FIOCLR = LEDVERDE;

                    // Apagar led's restantes
                    LPC_GPIO3 -> FIOSET = LEDAZUL;
                    LPC_GPIO3 -> FIOSET = LEDROJO;

                }else{
                    cambio = Rojo;

                    marca_r = count;
                }

                break;

        }

    }

}

void config(){

    // Configuracion Pin 0.22
    LPC_PINCON -> PINSEL1 &= ~(3 << 12);
    LPC_PINCON -> PINMODE1 &= ~(3 << 12);
    LPC_PINCON -> PINMODE1 |= (1 << 13);
    LPC_GPIO0 -> FIODIR |= LEDROJO;
    LPC_GPIO0 -> FIOSET = LEDROJO;

    // Configuracion Pin 3.25
    LPC_PINCON -> PINSEL7 &= ~(3 << 18);
    LPC_PINCON -> PINMODE7 &= ~(3 << 18);
    LPC_PINCON -> PINMODE7 |= (1 << 19);
    LPC_GPIO3 -> FIODIR |= LEDVERDE;
    LPC_GPIO3 -> FIOSET = LEDVERDE;

    // Configuracion Pin 3.26
    LPC_PINCON -> PINSEL7 &= ~(3 << 20);
    LPC_PINCON -> PINMODE7 &= ~(3 << 20);
    LPC_PINCON -> PINMODE7 |= (1 << 21);
    LPC_GPIO3 -> FIODIR |= LEDAZUL;
    LPC_GPIO3 -> FIOSET = LEDAZUL;
}

void SysTick_Handler(){
    count++;
}
