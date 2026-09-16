#include "LPC17xx.h"
#define LED (1 << 18)          
#define CCLK 100000000UL      

volatile uint32_t t = 0;       
volatile uint32_t t_prev = 0;  
volatile uint32_t periodo = 500;

void SysTick_Handler(void){
    t++;   
}

void EINT1_IRQHandler(void){
    if(LPC_SC->EXTINT & (1<<1)){

        uint32_t nuevoPeriodo = t - t_prev;  // ms transcurridos desde el pulso anterior
        t_prev = t;                          // guardo la referencia para la próxima medición

        
        if(nuevoPeriodo < 100){
            nuevoPeriodo = 100;
        }
        if(nuevoPeriodo > 1000){
            nuevoPeriodo = 1000;
        }

        periodo = nuevoPeriodo;

        LPC_SC->EXTINT = (1<<1); // limpio flag
    }
}

void systick_config(void){
    SysTick->LOAD = (CCLK / 1000) - 1; // Recargo cada 1ms
    SysTick->VAL = 0;
    SysTick->CTRL = (1<<0) | (1<<1) | (1<<2); 
}

void eint1_config_reg(void){
    LPC_PINCON->PINSEL4 &= ~(3<<22);
    LPC_PINCON->PINSEL4 |= (1<<22);      
    LPC_SC->EXTMODE |= (1<<1);           
    LPC_SC->EXTPOLAR &= ~(1<<1);         

    NVIC_EnableIRQ(EINT1_IRQn); 
}

void config(){
    LPC_PINCON -> PINSEL3 &= ~(3 << 4);
    LPC_PINCON -> PINMODE3 &= ~(3 << 4);  
    LPC_PINCON -> PINMODE3 |= (1 << 5);
    LPC_GPIO1 -> FIODIR |= LED;
    LPC_GPIO1 -> FIOCLR = LED;
}

int main(void){

    config();
    systick_config();
    eint1_config_reg();

    uint32_t ultimoToggle = 0;

    while(1){


        if((t - ultimoToggle) >= (periodo / 2)){
            ultimoToggle = t;

            if(LPC_GPIO1 -> FIOPIN & LED){
                LPC_GPIO1 -> FIOCLR = LED;
            }else{
                LPC_GPIO1 -> FIOSET = LED;
            }
        }

    }

}
