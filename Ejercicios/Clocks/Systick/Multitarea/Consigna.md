### **Consigna del Ejercicio: "El Sistema Multitarea Básico"**

**Objetivo Principal:**
Crear un sistema que controle dos LEDs con patrones de parpadeo independientes y que, además, reaccione a la entrada de un pulsador para modificar el comportamiento de uno de los LEDs.

**Hardware a Utilizar:**
1.  **LED Lento (LED 1):** El LED rojo que ya usaste, en el pin **P0.22**.
2.  **LED Rápido (LED 2):** Un segundo LED de otro color, conectado a un pin de propósito general como **P0.0**. (Necesitarás su resistencia limitadora, ~330Ω).
3.  **Pulsador de Control:** Un pulsador conectado al pin **P2.10**. Se conectará de la siguiente forma: una pata del pulsador al pin P2.10 y la otra pata a tierra (GND). Usaremos la resistencia de pull-up interna del microcontrolador.

**Comportamiento Detallado del Sistema:**

1.  **LED Lento (P0.22):**
    *   Debe parpadear de forma **constante e ininterrumpida**, sin importar lo que ocurra con el pulsador o el otro LED.
    *   Su ciclo será de 2 segundos: **1 segundo encendido, 1 segundo apagado**.

2.  **LED Rápido (P0.0):**
    *   Debe parpadear a un ritmo mucho más rápido.
    *   Su ciclo será de 500 ms: **250 milisegundos encendido, 250 milisegundos apagado**.

3.  **Lógica del Pulsador (P2.10):**
    *   Mientras el pulsador **NO** esté presionado, el **LED Rápido** debe parpadear normalmente.
    *   Si el pulsador **se mantiene presionado**, el **LED Rápido** debe detener su parpadeo y quedarse **completamente apagado**.
    *   Al soltar el pulsador, el LED Rápido debe reanudar su parpadeo inmediatamente.
    *   **Importante:** La acción del pulsador solo afecta al LED Rápido. El LED Lento debe seguir su parpadeo sin alterarse.

**Requisitos Técnicos Obligatorios:**

1.  **Único SysTick:** Debes seguir usando una única configuración de SysTick que interrumpa cada **1 milisegundo**. Toda la lógica de tiempo del programa debe derivarse de este único contador.
2.  **Cero Bloqueos:** El bucle `while(1)` en `main` debe seguir siendo no bloqueante. No se permite ningún tipo de retardo por software.
3.  **Estructura en `main`:** El bucle `while(1)` debe ser capaz de gestionar las tres tareas (chequear tiempo del LED 1, chequear tiempo del LED 2, y leer el estado del pulsador) en cada pasada.

**Pistas y Consejos:**

*   **Múltiples Tiempos:** Si para una tarea necesitaste una variable `tiempo_anterior`, para dos tareas con tiempos independientes... ¡necesitarás **dos variables `tiempo_anterior`**! Por ejemplo, `tiempo_anterior_led1` y `tiempo_anterior_led2`.

*   **Configurar el Pulsador:**
    *   El pin P2.10 debe ser configurado como GPIO (`PINSEL4`) y como entrada (`FIODIR2`).
    *   Para usar la resistencia de pull-up interna (muy recomendable), debes configurar el registro `PINMODE4`. El modo por defecto es pull-up, pero es bueno ser explícito. Poner los bits correspondientes a P2.10 en `00` activa el pull-up.
    *   `LPC_PINCON->PINMODE4 &= ~(0x3 << 20); // Pull-up para P2.10`

*   **Leer el Pulsador:**
    *   Cuando el pulsador **no está presionado**, la resistencia de pull-up interna mantiene el pin en estado alto (un '1' lógico).
    *   Cuando **presionas** el pulsador, conectas el pin a GND, forzando un estado bajo (un '0' lógico). Esto se conoce como lógica "activa baja".
    *   Para leerlo en un `if`, la condición para "pulsador presionado" sería: `if (!(LPC_GPIO2->FIOPIN & (1 << 10)))`. ¡Fíjate en el `!` al principio que niega la condición!