#include "LPC17xx.h"

#define cambio (1 << 0) // Pin P2.0
#define exit (1 << 1) // Pin P2.1
#define PINE0 (1 << 10) // Pin P2.10
#define PINE1 (1 << 11) // Pin P2.11

uint32_t ticks = (0.01 * 100000000) - 1; // 10ms

uint8_t secuencia[15]={0,0,0,0,1,0,1,0,1,0,1,0,0,0,0};

void SysTick_Handler();
void EINT0_IRQHandler();
void EINT1_IRQHandler();
void EINT3_IRQHandler();

void config_tim0();
void config_sys();
void config();

enum tiempo{
    APAGADO,
    SYSTICK,
    TIMER0
};

enum sec{
    TIEMPO0,
    TIEMPO1
};

enum tiempo CLK = APAGADO;
enum sec sel = TIEMPO0;

uint32_t i = 0;

int main(){

    config();
    config_tim0(10);
    config_sys();

    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT1_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);

    while(1){}

}

void config(){

    // Configuracion P2.0
    LPC_PINCON -> PINSEL4 &= ~(3 << 0);
    LPC_PINCON -> PINMODE4 &= ~(3 << 0);
    LPC_PINCON -> PINMODE4 |= (3 << 0); // Pull-down
    LPC_GPIO2 -> FIODIR &= ~cambio;
    LPC_GPIOINT -> IO2IntEnR |= cambio;

    // Configuracion P2.1
    LPC_PINCON -> PINSEL4 &= ~(3 << 2);
    LPC_PINCON -> PINMODE4 &= ~(3 << 2);
    LPC_PINCON -> PINMODE4 |= (1 << 3);
    LPC_GPIO2 -> FIODIR |= exit;

    // Configuracion P2.10
    LPC_PINCON -> PINSEL4 &= ~(3 << 20);
    LPC_PINCON -> PINMODE4 &= ~(3 << 20);
    LPC_PINCON -> PINMODE4 |= (3 << 20); // Pull-down
    LPC_SC -> EXTPOLAR |= (1 << 0);
    LPC_SC -> EXTMODE |= (1 << 0);
    LPC_SC -> EXTINT |= (1 << 0);

    // Configuracion P2.11
    LPC_PINCON -> PINSEL4 &= ~(3 << 22);
    LPC_PINCON -> PINMODE4 &= ~(3 << 22);
    LPC_PINCON -> PINMODE4 |= (3 << 22); // Pull-down
    LPC_SC -> EXTPOLAR |= (1 << 1);
    LPC_SC -> EXTMODE |= (1 << 1);
    LPC_SC -> EXTINT |= (1 << 1);

}

void config_sys(){

    SysTick -> LOAD = ticks; // 10ms
    SysTick -> VAL = 0;
    SysTick -> CTRL = (1 << 1) | (1 << 2); // No lo enciendo aun

}

void config_tim0(uint32_t periodo){

    LPC_SC -> PCONP |= (1 << 1); // Enciendo el timer
    LPC_SC -> PCLKSEL0 &= ~(3 << 2); // Eligo el CCLK, CCLK/4

    LPC_TIM0 -> CTCR &= ~(3 << 0);
    LPC_TIM0 -> PR = 25000; // 1ms

    LPC_TIM0 -> MCR |= (1 << 0) | (1 << 1);
    LPC_TIM0 -> MR0 = periodo;

    LPC_TIM0 -> IR |= (1 << 0); // Limpio flag

}

void SysTick_Handler(){
    
    if(i <= 14){
        if(secuencia[i] == 0){
            LPC_GPIO2 -> FIOCLR = exit;
            i++;
        }else{
            LPC_GPIO2 -> FIOSET = exit;
            i++;
        }
    }else{
        LPC_GPIO2 -> FIOCLR = exit; // Forzo el apagado
        i = 0;
        SysTick -> CTRL &= ~(1 << 0); // Apago el Systick
        CLK = APAGADO;
        sel = TIEMPO0;
    }

}

void TIMER0_IRQHandler(){

    if(i <= 14){
        if(secuencia[i] == 0){
            LPC_GPIO2 -> FIOCLR = exit;
            i++;
        }else{
            LPC_GPIO2 -> FIOSET = exit;
            i++;
        }
    }else{
        LPC_GPIO2 -> FIOCLR = exit; // Forzo el apagado
        i = 0;
        LPC_TIM0 -> TCR &= ~(1 << 0); // Apago el TIMER
        CLK = APAGADO;
        sel = TIEMPO0;
    }

    LPC_TIM0 -> IR |= (1 << 0);

}

void EINT0_IRQHandler(){

    
    sel = TIEMPO0;
    if(CLK == SYSTICK){
        SysTick -> LOAD = ticks;
    }else{
        config_tim0(10);
    }

    LPC_SC -> EXTINT |= (1 << 0);

}

void EINT1_IRQHandler(){

    sel = TIEMPO1;
    if(CLK == SYSTICK){
        SysTick -> LOAD = (0.005 * 100000000) - 1; // 5ms
    }else{
        config_tim0(5);
    }

    LPC_SC -> EXTINT |= (1 << 1);

}

void EINT3_IRQHandler(){

    i = 0;

    if(CLK == APAGADO){
        CLK = SYSTICK;
        LPC_TIM0 -> TCR &= ~(1 << 0); // Deshabilito tim0
        SysTick -> CTRL |= (1 << 0); // Habilitamos SysTick
    }else{
        CLK = TIMER0;
        SysTick -> CTRL &= ~(1 << 0); // Deshabilito SysTick
        LPC_TIM0 -> TCR |= (1 << 0); // Habilito tim0
    }

    LPC_GPIOINT -> IO2IntClr = cambio;

}
