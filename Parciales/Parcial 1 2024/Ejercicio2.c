#include "LPC17XX.h"

#define Sensor (1 << 6)
#define DIPSwitch 0b1111
#define Codigo 0xA
#define Buzzer (1 << 11)

void Systick_Handler(void);
void EINT3_IRQHandler(void);
void configPIN(void);
void config_GPIO_Interrupt(void);

volatile uint8_t intentos = 0;
volatile uint8_t alarma = 0;
volatile uint8_t contador = 0;

int main(void)
{
    SystemInit();

    configPIN();

    config_GPIO_Interrupt();

    LPC_GPIO1->FIOCLR = Buzzer;
    SysTick->CTRL = 0;

    while(1){
        if(alarma == 1){
            LPC_GPIO1->FIOSET = Buzzer;
        }else{
            LPC_GPIO1->FIOCLR = Buzzer;
        }
    }

    return 0;

}

void SysTick_Handler(void)
{
    uint8_t codigo_ingresado;

    
    if (alarma == 1 || SysTick->CTRL == 0) return;

    contador--;

    codigo_ingresado = (uint8_t)(LPC_GPIO2->FIOPIN & DIPSwitch);
    if (codigo_ingresado == CODIGO_CORRECTO) {
        SysTick->CTRL = 0;
        LPC_GPIOINT->IO0IntEnR |= Sensor;
        return;
    }

    if (contador == 0) {
        intentos++;

        codigo_ingresado = (uint8_t)(LPC_GPIO2->FIOPIN & DIPSwitch);
        
        if (codigo_ingresado == CODIGO_CORRECTO) {
            SysTick->CTRL = 0;
            LPC_GPIOINT->IO0IntEnR |= Sensor;
        } else if (intentos < 2) {
            contador = 30;
        } else {
            alarma = 1;
            SysTick->CTRL = 0;
        }
    }
}

void EINT3_IRQHandler(void)
{
    // verifico si la interrupcion vino del puerto 0
    // uso la mascara del sensor
    if (LPC_GPIOINT->IO0IntStatR & (1 << 6)) {
        
        contador = 30;
        intentos = 0;
        alarma = 0;
        
        SysTick_Config(SystemCoreClock);
        
        LPC_GPIOINT->IO0IntEnR &= ~(1 << 6);

        LPC_GPIOINT->IO0IntClr = (1 << 6);
    }
}

void configPIN(void)
{
    // Configuracion del sensor
    LPC_PINCON->PINSEL0 &= ~(0b11 << 12);

    LPC_GPIO0->FIODIR &= ~Sensor; // entrada

    // Configuracion buffer
    LPC_PINCON->PINSEL2 &= ~(0b11 << 22);

    LPC_GPIO1->FIODIR |= Buzzer;

    // Configuracion DIP
    LPC_PINCON->PINSEL4 &= ~(0xFF << 0);

    LPC_GPIO2->FIODIR &= ~DIPSwitch;

    LPC_PINCON->PINMODE4 &= ~(0xFF << 0);
}

void config_GPIO_Interrupt(void) {

    LPC_GPIOINT->IO0IntEnR |= Sensor;


    NVIC_EnableIRQ(EINT3_IRQn);
}
