# El Desafío: Juego de Reacción "Reflejos de Luz"

### Objetivo
Crear un juego interactivo que ponga a prueba los reflejos del jugador. El sistema encenderá LEDs de forma aleatoria y el jugador deberá responder presionando el botón correcto en un tiempo límite que se acorta a medida que avanza el juego.

### Hardware Requerido
- **4 LEDs** de juego (ej. P1.18, P1.19, P1.20, P1.21)
- **4 Pulsadores**, uno por cada LED (ej. P0.0, P0.1, P0.2, P0.3)
- **2 LEDs** de estado (ej. P1.22 para "Éxito", P1.23 para "Fallo")

### La Lógica del Juego

1.  **Inicio del Juego:**
    *   Al encender, el sistema entra en modo "Esperando Inicio". Los 4 LEDs de juego parpadean lentamente.
    *   El juego comienza cuando el jugador presiona **cualquiera** de los 4 pulsadores.

2.  **Generación de la Secuencia:**
    *   El sistema debe encender **un** LED de juego de forma pseudo-aleatoria.
    *   El LED elegido se queda encendido, indicando al jugador qué botón debe presionar.

3.  **Input del Jugador:**
    *   El jugador tiene un tiempo límite para presionar el pulsador que corresponde al LED encendido.
    *   El tiempo límite inicial es de 1 segundo.
    *   El sistema debe detectar:
        *   **Acierto:** Si el jugador presiona el botón correcto dentro del tiempo.
        *   **Fallo (Botón Incorrecto):** Si el jugador presiona un botón que no corresponde al LED encendido.
        *   **Fallo (Tiempo Agotado):** Si el jugador no presiona ningún botón antes de que se acabe el tiempo.

4.  **Gestión de Resultados:**
    *   **Si acierta:**
        *   El LED de "Éxito" (P1.22) parpadea rápidamente 3 veces.
        *   La "ronda" termina y una nueva comienza inmediatamente.
        *   El tiempo límite para la siguiente ronda se reduce en 50ms (haciendo el juego progresivamente más difícil).
    *   **Si falla (por cualquier motivo):**
        *   El LED de "Fallo" (P1.23) se enciende de forma fija.
        *   Todos los LEDs de juego parpadean rápidamente para indicar "Game Over".
        *   El juego se detiene y vuelve al estado de "Esperando Inicio".

### ¿Por qué es mucho más difícil?

*   **Gestión de Múltiples Entradas:** Debes leer 4 botones distintos y saber cuál se presionó.
*   **Timeouts:** El núcleo del desafío. Debes crear un bucle que haga dos cosas a la vez: esperar una entrada (`FIOPIN`) y contar el tiempo con un `delay()` para ver si se agota. Esto no se puede hacer con un simple `delay()` bloqueante. *Pista: Necesitarás un bucle `for` o `while` que itere muchas veces (simulando el paso del tiempo) y que en cada iteración revise el estado de los botones.*
*   **Estado y Puntuación:** Debes usar variables para recordar el nivel actual, que se traduce en el `delay` del timeout.
*   **Pseudo-Aleatoriedad:** Deberás inventar un método para que la elección del siguiente LED no sea predecible.