#include "LPC17xx.h"

#define PIN (22)
#define TIME 2500

void config();
void delay();

int main(){

	config();

	while(1){
		LPC_GPIO0 -> FIOCLR = (1 << PIN);
		delay();
		LPC_GPIO0 -> FIOSET = (1 << PIN);
		delay();
	}

}

void config(){

	LPC_PINCON -> PINSEL1 &= ~(3 << 12);

	LPC_PINCON -> PINMODE1 &= ~(3 << 12);

	LPC_PINCON -> PINMODE1 |= (1 << 13);

	LPC_GPIO0 -> FIODIR |= (1 << PIN);

	LPC_GPIO0 -> FIOSET = (1 << PIN);
}

void delay(){
	for(int i=0; i<TIME; i++){
		for(int j=0; j<TIME; j++){

		}
	}
}
