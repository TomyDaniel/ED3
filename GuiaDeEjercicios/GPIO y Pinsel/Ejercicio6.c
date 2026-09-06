#include <LPC17xx.h>

#define entradas (0xFFFFFFFF << 0)

#define LEDs (0x1F << 0)

void config();
void delay();

int contador = 0;

int main(){

    config();

    while(1){

        for(int i=0; i<32; i++){
            if(LPC_GPIO0 -> FIOPIN & (1 << i)){
                contador++;
            }
        }

        LPC_GPIO2 -> FIOSET = (contador & LEDs);
        delay();
        LPC_GPIO2 -> FIOCLR = LEDs;
        contador = 0;
        delay();

    }

}

void config(){

    // Configuracion puertos 0 entrada

    LPC_PINCON -> PINSEL0 &= ~(0xFFFFFFFF << 0); // Primeros 16 pines

    LPC_PINCON -> PINSEL1 &= ~(0xFFFFFFFF << 16); // Segundo 16 pines

    LPC_PINCON -> PINMODE0 &= ~(0xFFFFFFFF << 0); // Colocamos una res de pull down para hacer logica positiva

    LPC_PINCON -> PINMODE0 |= (0xFFFFFFFF << 0);

    LPC_PINCON -> PINMODE1 &= ~(0xFFFFFFFF << 16);

    LPC_PINCON -> PINMODE1 |= (0xFFFFFFFF << 16);

    LPC_GPIO0 -> FIODIR &= ~entradas;

    // Configuracion LEDs
 
    LPC_PINCON -> PINSEL4 &= ~(0x3FF << 0);

    LPC_PINCON -> PINMODE4 &= ~(0x3FF << 0);

    LPC_PINCON -> PINMODE4 |= (0x2AA << 0); // Salida flotante

    LPC_GPIO2 -> FIODIR = LEDs;

    LPC_GPIO2 -> FIOCLR = LEDs; // Empiezo con los leds apagados

}

void delay(){

    for(int i=0; i<2500; i++){
        for(int j=0; j<2500; j++){}
    }
}
