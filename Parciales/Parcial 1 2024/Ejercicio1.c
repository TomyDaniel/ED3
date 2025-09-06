#include "LPC17xx.h"

#define Sensor (1 << 4)
#define Motor (1 << 15)
#define LED (1 << 5)
#define Switch (1 << 12) // Cambio el puerto y el pin

volatile uint32_t tiempo_barrera_seg = 5;
volatile uint32_t tiempo_restante = 0;
volatile uint8_t pulsaciones_config = 0;
volatile uint8_t modo_config_activo = 1;

void SysTick_Handler(void);
void EINT3_IRQHandler(void);
void config_GPIO_Interrupt(void);
void configPIN(void);
void validar_ticket(void);

int main(void){
    SystemInit();

    configPIN();

    config_GPIO_Interrupt();

    modo_config_activo = 1;
    tiempo_restante = 3;
    SysTick_Config(SystemCoreClock);

    while(modo_config_activo == 1){
        _WFI();
    };

    SysTick->CTRL = 0;

    while(1){
        _WFI();
    }
}

void SysTick_Handler(void){
    if (modo_config_activo == 1) {
        if (tiempo_restante > 0) {
            tiempo_restante--;
        } else {
            modo_config_activo = 0;

            if (pulsaciones_config >= 4) {
                pulsaciones_config = 0;
            }

            switch(pulsaciones_config) {
                case 0: tiempo_barrera_seg = 5; break;
                case 1: tiempo_barrera_seg = 10; break;
                case 2: tiempo_barrera_seg = 20; break;
                case 3: tiempo_barrera_seg = 40; break;
                default: tiempo_barrera_seg = 5; break;
            }
        }
    } else {
        if (tiempo_restante > 0) {
            tiempo_restante--;
        } else {
            LPC_GPIO0->FIOCLR = Motor;
            LPC_GPIOINT->IO2IntEnR |= Sensor;
            SysTick->CTRL = 0;
        }
    }
}

int validar_ticket(void) {
    return 1;
}

void EINT3_IRQHandler(void){

    if (LPC_GPIOINT->IO2IntStatR & Sensor) {
        
       
        LPC_GPIOINT->IO2IntClr = Sensor;
        
        
        if (modo_config_activo == 0 && (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) == 0) {
            if (validar_ticket()) {
                LPC_GPIO0->FIOSET = Motor;          
                tiempo_restante = tiempo_barrera_seg;  
                SysTick_Config(SystemCoreClock);      
                LPC_GPIOINT->IO2IntEnR &= ~Sensor;
            } else {
                LPC_GPIO1->FIOSET = LED;        
            }
        }
    }

    if (LPC_GPIOINT->IO2IntStatR & Switch) {
        LPC_GPIOINT->IO2IntClr = Switch;
        if (modo_config_activo == 1) {
            pulsaciones_config++;
        }
    }

}

void config_GPIO_Interrupt(void){
    LPC_GPIOINT->IO2IntEnR |= Sensor | Switch;

    NVIC_EnableIRQ(EINT3_IRQn);
}

void configPIN(void){
    // Configuracion Sensor
    LPC_PINCON->PINSEL4 &= ~(0b11 << 8);

    LPC_GPIO2->FIODIR &= ~Sensor; // entrada

    // Configuracion Motor
    LPC_PINCON->PINSEL0 &= ~(0b11 << 30);

    LPC_GPIO0->FIODIR |= Motor;

    // Configuracion LED
    LPC_PINCON->PINSEL2 &= ~(0b11 << 10);

    LPC_GPIO1->FIODIR |= LED;

    // Configuracion Switch
    LPC_PINCON->PINSEL4 &= ~(0b11 << 24);

    LPC_GPIO3->FIODIR &= ~Switch;

    LPC_PINCON->PINMODE4 &= ~(0b11 << 24);
}