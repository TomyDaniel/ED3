#include "LPC 17xx.h"
#include <stdio.h>

#define LED1 (1 << 0)
#define LED2 (1 << 1)
#define LED3 (1 << 2)
#define LED4 (1 << 3)

#define BT1 (1 << 0)
#define BT2 (1 << 1)
#define BT3 (1 << 2)
#define BT4 (1 << 3)

#define ticks 1000000

typedef enum{
    ESTADO_IDLE,
    ESTADO_INICIO,
    ESTADO_GENERAR_PASO,
    ESTADO_MOSTRAR_SECUENCIA,
    ESTADO_ESPERAR_JUGADOR,
    ESTADO_VALIDAR_JUGADA,
    ESTADO_RONDA_SUPERADA,
    ESTADO_GAME_OVER,
    ESTADO_VICTORIA
}FSM;

typedef enum{
    SUB_ENCENDER_LED,
    SUB_ESPERAR_LED_ON,
    SUB_APAGAR_LED,
    SUB_ESPERAR_LED_OFF
}FSM_SUB;

void config_botones(void);
void leds_init(void);
void leds_apagar_todos(void);
void led_enceder(uint8_t indice);
void led_apagar(uint8_t indice);
void led_mostrar(uint8_t indice);

uint8_t tiempo_cumplido(uint32_t referencia, uint32_t demora_ms);
void SysTick_Handler(void);

uint8_t boton_sigue_presionado(uint8_t boton);
void EINT3_IRQHandler(void);

FSM estado_actual = ESTADO_IDLE;
