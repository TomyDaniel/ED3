# El Desafío: El Súper Auto Fantástico (K.I.T.T. 2.0)

### Objetivo
Mejorar el efecto de "El Auto Fantástico" para que sea interactivo. Utilizaremos dos pulsadores para controlar el comportamiento de las luces, permitiendo al usuario cambiar el patrón y la velocidad de la secuencia en tiempo real.

### Hardware Requerido

- **6 LEDs** para la barra de luz.
- **2 Pulsadores** para el control de usuario.

#### Conexiones de LEDs
Para practicar con un bloque de pines, los conectaremos todos al mismo puerto y de forma consecutiva:
- **LED 1**: `P1.18`
- **LED 2**: `P1.19`
- **LED 3**: `P1.20`
- **LED 4**: `P1.21`
- **LED 5**: `P1.22`
- **LED 6**: `P1.23`

#### Conexiones de Pulsadores
Añadiremos 2 pulsadores para controlar la lógica:
- **BOTON_MODO**: Conectado al pin `P0.0`
- **BOTON_VELOCIDAD**: Conectado al pin `P0.1`

*(**Nota**: Asumiremos que los pulsadores están configurados en modo pull-up, lo que significa que al presionarlos, el pin lee un `0` lógico, y un `1` cuando están sin presionar).*

---

### La Nueva Lógica

El sistema deberá tener **dos variables de estado**: una para el `modo` de la secuencia y otra para la `velocidad`.

#### 1. Control de Modos (`BOTON_MODO` - `P0.0`)

Cada vez que se presione el `BOTON_MODO`, el sistema cambiará al siguiente patrón de luces. Al llegar al último modo, volverá al primero.

*   **Modo 1: Clásico (Auto Fantástico)**
    *   La secuencia original: un solo LED se mueve de izquierda a derecha y viceversa.
    *   `100000` -> `010000` -> ... -> `000001` -> `000010` -> ... -> `100000`.

*   **Modo 2: Relleno y Vaciado**
    *   Los LEDs se encienden secuencialmente de izquierda a derecha hasta que todos están encendidos. Luego, se apagan en el mismo orden.
    *   `100000` -> `110000` -> `111000` -> ... -> `111111`
    *   Luego: `011111` -> `001111` -> `000111` -> ... -> `000000`.

*   **Modo 3: Convergencia y Divergencia**
    *   Dos luces se encienden en los extremos y se mueven hacia el centro. Al encontrarse, invierten su camino y vuelven a los extremos.
    *   `100001` -> `010010` -> `001100` (convergencia)
    *   Luego: `010010` -> `100001` (divergencia).

#### 2. Control de Velocidad (`BOTON_VELOCIDAD` - `P0.1`)

Cada vez que se presione el `BOTON_VELOCIDAD`, la pausa entre cada paso de la secuencia cambiará. Habrá 3 niveles de velocidad. Al llegar a la última velocidad, volverá a la primera.

*   **Velocidad 1: Lento** (ej. Pausa de 300ms)
*   **Velocidad 2: Normal** (ej. Pausa de 150ms)
*   **Velocidad 3: Rápido** (ej. Pausa de 50ms)

---

### Requisitos de Implementación (usando solo funciones básicas)

1.  **Configuración (`PINSEL` y `FIODIR`):**
    *   Configura los 6 pines de los LEDs (`P1.18` a `P1.23`) como GPIO y como **salidas**.
    *   Configura los 2 pines de los pulsadores (`P0.0` y `P0.1`) como GPIO y como **entradas**.

2.  **Bucle Principal (`while(1)`):**
    *   Dentro del bucle infinito, el programa debe hacer dos cosas constantemente:
        *   **Revisar los pulsadores:** Usar `FIOPIN` para leer el estado de `P0.0` y `P0.1`. Si se detecta una presión (un `0`), se debe actualizar la variable de estado correspondiente (`modo` o `velocidad`).
            *   **Anti-rebote simple:** Para evitar que una sola presión se registre múltiples veces, puedes añadir una pequeña pausa justo después de detectar el `0`.
        *   **Ejecutar la secuencia:** Basándose en los valores actuales de las variables `modo` y `velocidad`, ejecutar el paso correspondiente de la secuencia de luces. Puedes usar una estructura `switch` o `if-else if` para decidir qué patrón ejecutar.

3.  **Control de LEDs (`FIOSET`, `FIOCLR`):**
    *   Usa estas funciones para encender y apagar los LEDs según el patrón y el paso actual de la secuencia. Recuerda que para cada paso, generalmente querrás limpiar el estado anterior antes de establecer el nuevo (`FIOCLR` antes de `FIOSET`).

---

### Entregable

-   El código fuente completo en un archivo `.c`.
-   **(Opcional pero recomendado)** Un video corto mostrando el circuito funcionando y cómo responde a las presiones de ambos botones, cambiando entre los modos y las velocidades.

---

### ¿Por qué es más difícil?

*   **Gestión de Estado:** Ya no es una única secuencia lineal. Debes usar variables para "recordar" en qué modo y velocidad se encuentra el sistema.
*   **Entrada de Usuario:** Introduce la necesidad de leer pines de entrada (`FIOPIN`) y reaccionar a ellos.
*   **Lógica Condicional Compleja:** El bucle principal debe manejar múltiples condiciones (qué modo está activo, qué velocidad usar, si se ha presionado un botón).
*   **Manejo de "Anti-Rebote" (Debouncing):** Aunque se sugiere una solución simple (una pausa), introduce al estudiante al problema real de que las entradas mecánicas no son limpias y pueden generar múltiples lecturas en una sola pulsación.