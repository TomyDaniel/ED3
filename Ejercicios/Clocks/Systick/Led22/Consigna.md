### **Consigna del Ejercicio: "Parpadeo"**

**Objetivo Principal:**
Escribir un programa desde cero para el microcontrolador LPC1769 que haga parpadear el LED rojo (conectado al pin **P0.22**) de forma continua. El ciclo de parpadeo debe ser de **1 segundo en total**: medio segundo (500 ms) encendido y medio segundo (500 ms) apagado.

**Requisitos Técnicos Obligatorios:**

1.  **Uso del SysTick:** Debes utilizar el temporizador SysTick para controlar el tiempo. La configuración del SysTick debe generar una interrupción periódica a una frecuencia alta (se recomienda **1000 Hz**, es decir, una interrupción cada **1 milisegundo**).

2.  **Lógica No Bloqueante:** El bucle principal (`while(1)` en la función `main`) **NO debe contener retardos por software** (bucles `for` o `while` vacíos que consuman tiempo). El programa principal debe estar siempre "libre" y la decisión de cambiar el estado del LED se debe tomar comparando una variable de tiempo.

3.  **Manejo de Interrupción (Handler):** Debes implementar la función `SysTick_Handler()`. La responsabilidad de esta función debe ser mínima: únicamente actualizar una variable global que sirva como contador de tiempo (por ejemplo, un contador de milisegundos).

4.  **Estructura del Código:**
    *   **Variable de tiempo:** Debes usar una variable global declarada como `volatile` para llevar la cuenta del tiempo transcurrido.
    *   **Función `main`:**
        *   Debe contener la configuración inicial del hardware (GPIO y SysTick).
        *   Debe contener el bucle infinito (`while(1)`).
        *   Dentro del bucle, debe estar la lógica que comprueba si han pasado los 500 ms y, en caso afirmativo, invierte el estado del LED.

**Hardware a Utilizar:**
*   Placa de desarrollo con microcontrolador LPC1769.
*   El LED rojo integrado en la placa, que está conectado al pin P0.22.

**Resultado Esperado:**
Al compilar y cargar el programa en la placa, el LED rojo debe empezar a parpadear de forma regular y precisa, con un ciclo visible de un segundo.