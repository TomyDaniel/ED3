# Electronica Digital 3

## LPC 1769

### Programación GPIO

- Hardware mapeado en memorias: Escribo y leo en direcciones de memoria especificas que estan conectadas a los perifericos.

- Configurar vs. operar: Primero se configura el hardware y luego se opera mediante un bucle

**¿Que debo hacer primero?**

#### Configurar la función:

- Primero le digo al PIN que hacer (GPIO, UART, ADC, ...)    *(Para LED´s y botones siempre utilizo GPIO)*
    ```
    **LPC_PINCON->PINSELx**

    (La x depende del puerto y los bits seleccionados)
    ```

#### Configurar la dirección:

- Si se utiliza GPIO, indicar si es salida o entrada.
    - **Salida (Output):** El bit correspondiente debe ser `1`.
    - **Entrada (Input):** El bit correspondiente debe ser `0`.

```c
// Configura el pin 5 como SALIDA (enciende el bit 5)
LPC_GPIO0->FIODIR |= (1 << 5);

// Configura el pin 5 como ENTRADA (apaga el bit 5)
LPC_GPIO0->FIODIR &= ~(1 << 5);
```

### Operar el PIN:

- Comandos para setar, limpir y saber que esta haciendo el PIN (estado):
    ```
    FIOSET
    FIOCLR
    FIOPIN
    ```

La LPC 1769 posee 5 puertos, cada puerto tiene 15 pines y son controlados por 32 bits de la siguiente manera:

- **Puerto 0**:
    - PINSEL 0: Va desde 0 a 15 bits
    - PINSEL 1: Va desde 16 a 31 bits

Esto se "repite" hasta el puerto 4:

- **Puerto 4**:
    - PINSEL 8: Va desde 0 a 15 bits
    - PINSEL 9: Va desde 16 a 31 bits

La distribución de los bits dentro de un PINSEL es:

*bits* 0 y 1 ----> Pin 0 del puerto

*bits* 2 y 3 ----> Pin 1 del puerto

.

.

.

*bits* 30 y 31 ----> Pin 15 del puerto

Entonces podemos deducir la siguiente ecuación:

$ posicion_{bit} = nro_{pin} * 2 $

Ademas sabemos que nuestra ecuación general es:

$$ P_{x} \cdot _{y} $$

### Cómo Calcular la Posición de Bits en `PINSEL`

Para cualquier pin **P<sub>x.y</sub>**, sigue estos pasos para encontrar los bits que lo controlan:

#### **1. Si el pin `y` está en la Mitad Inferior (0 a 15):**

-   **Fórmula:**
    $bit_{inicio} = 2 \cdot y$
-   **Ejemplo para P2.10 (`y=10`):**
    $bit_{inicio} = 2 \cdot 10 = 20$.
    *Los bits a controlar son el **20** y el **21**.*
-   **Código de ejemplo para limpiar los bits:**
    ```c
    // Limpia los bits 20 y 21
    LPC_PINCON->PINSEL4 &= ~(3 << 20);
    ```

#### **2. Si el pin `y` está en la Mitad Superior (16 a 31):**

-   **Fórmula:**
    $bit_{inicio} = (y - 16) \cdot 2$
-   **Ejemplo para P0.22 (`y=22`):**
    $bit_{inicio} = (22 - 16) \cdot 2 = 6 \cdot 2 = 12$.
    *Los bits a controlar son el **12** y el **13**.*
-   **Código de ejemplo para limpiar los bits:**
    ```c
    // Limpia los bits 12 y 13
    LPC_PINCON->PINSEL1 &= ~(3 << 12);
    ```


