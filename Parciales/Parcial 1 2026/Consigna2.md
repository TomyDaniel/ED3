**Objetivo**: Se implementará un control de potencia a lazo abierto en la resistencia de un horno industrial mediante un control pwm generado a través del TIMER0, el periodo (MR0) del PWM será 1 ms, el ciclo de trabajo (MR1) se define en la primer fila de la matriz “perfil”.   

El tiempo que la resistencia debe mantener ese valor de pwm se encuentra en la segunda fila de la misma matriz expresado en segundos.  

Si alguien presiona un pulsador conectado a la EINT0 debe cortarse la alimentación en la carga y encender una salida que se conectará a una señal sonora para informar una falla grave.  

Perfil Térmico: El proceso debe seguir el pwm [%], indicado en la primera fila durante el tiempo [segundos] indicado en la segunda fila.

```
uint16_t perfil[2][10] = {
{10, 30, 60, 85, 100, 100, 70, 40, 20, 0},//ciclo de trabajo
{300, 450, 600, 600, 1200, 900, 600, 450, 300, 0}//Tiempo
};
```

1) Implemente únicamente  
- a) la función int main(void)  
- i) Configuración de pwm y Esperar tiempo de perfil térmico  
- b) la función void EINT0_IRQHandler(void)de manera que detenga todo el proceso y dispare la señal de falla  
- c) la función void TIMER0_IRQHandler(void)de manera que controle el PWM  
2) Según la consigna solicitada, es correcta la siguiente función de configuración de TIMER0?  
corríjala para que funcione correctamente.

```
void conf_timer0_pwm(uint8_t duty) {
    LPC_SC->PCONP |= (1 << 1);
    // Falta el PCLKSEL0 &= ~(0b11 << 2); 
    LPC_TIM0->TCR = 0x02;
    LPC_TIM0->PR = 0; // 40ns
    LPC_TIM0->MR0 = 25000; 
    LPC_TIM0->MR1 = (23000 * duty) / 100; // Deberia ser 25k
    LPC_TIM0->MCR = (1 << 0) | (0 << 1) | (1 << 3); // deberia ser (1 << 1)
    LPC_TIM0->TCR &= 0x01; // mal
}
```