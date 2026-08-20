#include "LPC17xx.h"


#define LED (22)

void config();
void delay();

int main(){

    config();

    while(1){

        if((LPC_GPIO2 -> FIOPIN & (1 << 10)) == 0){
            LPC_GPIO0 -> FIOCLR = (1 << LED);
            delay();
            LPC_GPIO0 -> FIOSET = (1 << LED);
            delay();

        }else{
            LPC_GPIO0 -> FIOSET = (1 << LED);
        }
    }

}

void config(){
    /*
        Configuracion del LED P0.22
    */
    LPC_PINCON -> PINSEL1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 &= ~(3 << 12);

    LPC_PINCON -> PINMODE1 |= (1 << 13);

    LPC_GPIO0 -> FIODIR |= (1 << LED);

    LPC_GPIO0 -> FIOSET = (1 << LED);

    /*
        Configuracion Boton P2.10
    */

    LPC_PINCON -> PINSEL4 &= ~(3 << 10);

    LPC_PINCON -> PINMODE4 &= ~(3 << 10);

    LPC_GPIO2 -> FIODIR &= ~(1 << 10);

}

void delay(){
	for(int i=0; i<2500; i++){
		for(int j=0; j<2500; j++){

		}
	}
}
