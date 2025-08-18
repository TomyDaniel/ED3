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

- Si se utiliza GPIO indicar si es salida o entrada, el "0" se utiliza para decir es mi entrada y el "1" para decir es mi salida.
    ```
    LPC_GPIO0->FIODIR |= (0 << 5) **Entrada**

    LPC_GPIO0->FIODIR |= (1 << 5) **Salida**
    ```

### Operar el PIN:

- Comandos para setar, limpir y saber que esta haciendo el PIN (estado):
    ```
    FIOSET
    FIOCLR
    FIOPIN
    ```

La LPC 1769 posee 4 puertos, cada puerto tiene 15 pines y son controlados por 32 bits de la siguiente manera:

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

Con la siguiente representación:

- La **x** representa el *puerto*

- La **y** representa el *pin*

Sabiendo esto y para facilitar el calculo de los bits, disponemos de las siguientes formulas:

- Mitad inferior:

$ (2 \cdot y) $     y      $(2 \cdot y + 1)$

- Mitad superior:

$ [(y - 16) \cdot 2] $     y      $ [(y - 16) \cdot 2 + 1] $

