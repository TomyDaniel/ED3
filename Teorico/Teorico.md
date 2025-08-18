# Electrónica Digital 3

## Guía de Programación GPIO para LPC1769

### Principios Básicos

La interacción con los periféricos en el LPC1769 se realiza a través de **registros mapeados en memoria**. Esto significa que para controlar un pin, simplemente leemos o escribimos valores en direcciones de memoria específicas que el hardware interpreta como comandos.

El flujo de trabajo siempre sigue dos fases:

1.  **Configuración:** Se realiza una sola vez al inicio del programa. Aquí se define el comportamiento del hardware.
2.  **Operación:** Se realiza continuamente dentro de un bucle principal. Aquí se ejecutan las acciones.

### El Proceso de 3 Pasos para Controlar un Pin GPIO

Para controlar cualquier pin como Entrada/Salida de Propósito General (GPIO), se deben seguir estos tres pasos en orden:

1.  **Paso 1: Configurar la Función del Pin**
    -   Registro: `LPC_PINCON->PINSELx`
    -   Propósito: Decirle al pin si será GPIO, ADC, UART, etc.
2.  **Paso 2: Configurar la Dirección del Pin**
    -   Registro: `LPC_GPIOx->FIODIR`
    -   Propósito: Si es GPIO, especificar si será Entrada o Salida.
3.  **Paso 3: Operar el Pin**
    -   Registros: `FIOSET`, `FIOCLR`, `FIOPIN`
    -   Propósito: Escribir un estado si es salida, o leer el estado si es entrada.

---

### Paso 1: Configurar la Función del Pin (`PINSEL`)

Cada pin del microcontrolador puede tener hasta 4 funciones distintas. Debemos seleccionar cuál queremos usar. Para LEDs y botones, siempre usaremos la función **GPIO**.

La función de cada pin se configura mediante 2 bits en los registros `PINSEL`.

-   `00`: Función principal (generalmente **GPIO**).
-   `01`: Primera función alternativa.
-   `10`: Segunda función alternativa.
-   `11`: Tercera función alternativa.

> **Importante:** Para usar un pin como GPIO, debemos asegurarnos de que sus 2 bits correspondientes en `PINSEL` estén en `00`. La forma más segura de hacerlo es "limpiando" esos bits.

#### ¿Qué registro `PINSEL` debo usar?

El LPC1769 agrupa los registros `PINSEL` de la siguiente manera: cada registro controla 16 pines (32 bits, 2 por pin).

| Puerto | Pines 0-15  | Pines 16-31  |
| :----: | :-------------------------: | :--------------------------: |
|  **P0**  |         `PINSEL0`         |          `PINSEL1`           |
|  **P1**  |         `PINSEL2`         |          `PINSEL3`           |
|  **P2**  |         `PINSEL4`         |          `PINSEL5`           |
|  **P3**  |         `PINSEL6`         |          `PINSEL7`           |
|  **P4**  |         `PINSEL8`         |          `PINSEL9`           |

#### ¿Cómo Calcular la Posición de los Bits?

Para un pin `Px.y`, donde `x` es el puerto y `y` es el número de pin:

1.  **Si `y` está entre 0 y 15:**
    -   El registro es `PINSEL` par.
    -   **Fórmula:** `posicion_bit_inicio = y * 2`
    -   *Ejemplo para `P2.10` (`y=10`):*
        -   Registro: `PINSEL4`
        -   Posición: `10 * 2 = 20`. Se deben modificar los bits **20** y **21**.

2.  **Si `y` está entre 16 y 31:**
    -   El registro es `PINSEL` impar.
    -   **Fórmula:** `posicion_bit_inicio = (y - 16) * 2`
    -   *Ejemplo para `P0.22` (`y=22`):*
        -   Registro: `PINSEL1`
        -   Posición: `(22 - 16) * 2 = 12`. Se deben modificar los bits **12** y **13**.

**Código para limpiar los bits:**

```c
// Para P2.10: Limpia los bits 20 y 21 de PINSEL4
// El 3 en binario es 11. (3 << 20) crea una máscara ...001100...
// ~(3 << 20) la invierte a ...110011... para limpiar solo esos bits.
LPC_PINCON->PINSEL4 &= ~(3 << 20);

// Para P0.22: Limpia los bits 12 y 13 de PINSEL1
LPC_PINCON->PINSEL1 &= ~(3 << 12);
```

---

### Paso 2: Configurar la Dirección del Pin (`FIODIR`)

Una vez que el pin está configurado como GPIO, le decimos si será una entrada o una salida a través del registro `FIODIR` de su puerto correspondiente.

- **Salida (Output)**: El bit correspondiente al pin debe ser 1.
- **Entrada (Input)**: El bit correspondiente al pin debe ser 0.

**Ejemplos de código:**

```c
// Configura el pin P0.22 como SALIDA (enciende el bit 22 en FIODIR del puerto 0)
LPC_GPIO0->FIODIR |= (1 << 22);

// Configura el pin P2.10 como ENTRADA (apaga el bit 10 en FIODIR del puerto 2)
LPC_GPIO2->FIODIR &= ~(1 << 10);
```

**Nota sobre operadores:**

- Usamos |= (OR) para encender un bit sin afectar a los demás.
- Usamos &= ~ (AND con NOT) para apagar un bit sin afectar a los demás.

---

### Paso 3: Operar el Pin

Con el pin ya configurado, podemos interactuar con él.

**Para Escribir en un Pin de Salida**

- `LPC_GPIOx->FIOSET`: **Pone en ALTO (1)** el pin correspondiente. Escribir un 0 no tiene efecto.
- `LPC_GPIOx->FIOCLR`: **Pone en BAJO (0)** el pin correspondiente. Escribir un 0 no tiene efecto.

**Ejemplc de código:**

```c
// Pone en ALTO (enciende LED) el pin P0.22
LPC_GPIO0->FIOSET = (1 << 22);

// Pone en BAJO (apaga LED) el pin P0.22
LPC_GPIO0->FIOCLR = (1 << 22);
```

**Para Leer un Pin de Entrada**

- `LPC_GPIOx->FIOPIN`: Contiene el estado actual de **todos los pines** del puerto. Se debe usar una máscara para leer el estado de un pin específico.

```c
// Lee el estado del pin P2.10 (un botón, por ejemplo)
if (LPC_GPIO2->FIOPIN & (1 << 10)) {
    // El pin P2.10 está en ALTO
} else {
    // El pin P2.10 está en BAJO
}
```

---

---

## Manejando el Tiempo: Clocks y el Timer SysTick

### 1. El Corazón del Sistema: La Gestión de Clocks 

Antes de hacer cualquier cosa, el microcontrolador necesita un "latido". Este latido es la señal de reloj, que sincroniza todas las operaciones.

#### ¿Por Qué es Tan Importante?

1.  **Velocidad de Ejecución:** La frecuencia del clock determina qué tan rápido "piensa" la CPU. Más MHz = programa más rápido.
2.  **Funcionamiento de Periféricos:** Cada componente necesita su propia señal de reloj para funcionar.
3.  **Gestión de Energía:** Apagar el reloj de los periféricos que no usas es la forma más eficaz de ahorrar batería.

> **Idea Central:** La configuración del clock es el **Paso Cero** de cualquier proyecto. Define la velocidad de todo el sistema, y esa velocidad es la que usaremos como referencia para crear retardos precisos con el SysTick.

---

### 2. La Herramienta de Precisión: El Timer SysTick 

Una vez que nuestro sistema tiene un "latido" estable, necesitamos una forma de contar esos latidos para medir el tiempo. La herramienta más simple y eficiente para esto es el **SysTick**.

#### La Clave: ¿Es del Núcleo (ARM) o del Fabricante (NXP)?

Esta es la distinción más importante que debes entender:

*    **El Núcleo (ARM):** La empresa ARM diseña el "cerebro" del microcontrolador. Esto es estándar para todos los chips Cortex-M.
*    **Los Periféricos (NXP):** Empresas como NXP toman ese "cerebro" y le añaden "brazos y piernas" (GPIO, Timers 0/1/2, UARTs, ADC). Estos son específicos de cada fabricante.

Por eso el SysTick es tan especial:
*   **Simplicidad:** Al ser parte del núcleo, no necesita configuraciones complejas de reloj en registros de NXP como `PCONP`. Ya está "conectado" directamente al procesador.
*   **Portabilidad:** El mismo código para el SysTick funcionará en un microcontrolador de ST, Texas Instruments o cualquier otro que use un núcleo Cortex-M.

#### SysTick vs. Timers Periféricos (Timer0, etc.)

| Característica | SysTick  | Timers Periféricos  |
| :--- | :--- | :--- |
| **Uso Ideal** | Delays sencillos, base de tiempo para RTOS. | Tareas complejas: Generar PWM, medir pulsos, etc. |
| **Complejidad** | **Muy baja.** Acceso directo. | **Media.** Requiere habilitar su clock en `PCONP`. |
| **Portabilidad** | **Alta** (Estándar de ARM). | **Nula** (Específico de NXP). |
| **Cantidad** | Solo hay **uno**. | Hay **varios** disponibles (Timer0, 1, 2, 3). |

---

#### Los 3 Registros Clave del SysTick

Todo el control se realiza a través de 3 registros accesibles directamente.

| Registro | Nombre Completo | Propósito Principal |
| :--- | :--- | :--- |
| `SysTick->CTRL` | Control and Status Register | **El interruptor principal.** Habilita/deshabilita el timer, selecciona la fuente de reloj y contiene la bandera que indica que el tiempo ha finalizado. |
| `SysTick->LOAD` | Reload Value Register | **El valor de cuenta.** Aquí se carga el número de "ticks" que queremos que el contador espere. Es un contador descendente, así que cuenta desde este valor hasta cero. |
| `SysTick->VAL` | Current Value Register | **El contador actual.** Muestra el valor actual del conteo. Escribir cualquier valor en él lo resetea a 0 de inmediato. |

---

#### Flujo de Programación para un Retardo

Para crear una función de retardo `delay`, el proceso siempre sigue estos 5 pasos:

##### Paso 1: Calcular los Ticks Necesarios
Determina cuántos ciclos de reloj del CPU se necesitan para el retardo deseado. La variable global `SystemCoreClock` (definida en los archivos de sistema de NXP) contiene la frecuencia del CPU en Hz.

*   **Ticks por segundo** = `SystemCoreClock`
*   **Ticks por milisegundo** = `SystemCoreClock / 1000`
*   **Ticks por microsegundo** = `SystemCoreClock / 1000000`

##### Paso 2: Cargar el Valor de Recarga (`LOAD`)
Carga el número de ticks calculados (menos 1, ya que la cuenta incluye el 0) en el registro `LOAD`. El valor máximo es de 24 bits (`0x00FFFFFF`).

```c
// Para un retardo de 'N' ticks
SysTick->LOAD = N - 1;
```

##### Paso 3: Resetear el Contador (VAL)
Para asegurar que la cuenta empiece desde el valor cargado, se debe limpiar el contador actual escribiendo cualquier valor en VAL.

```c
SysTick->VAL = 0;
```

##### Paso 4: Habilitar el Timer (CTRL)
Configura y enciende el SysTick escribiendo en el registro `CTRL`. La configuración típica es:
- Bit 0 (`ENABLE`) = 1: Habilita el contador.
- Bit 2 (`CLKSOURCE`) = 1: Usa el reloj del procesador como fuente.

```c
// Habilitar SysTick con el reloj del CPU
SysTick->CTRL = (1 << 0) | (1 << 2);
```

##### Paso 5: Esperar la Bandera (COUNTFLAG)
Monitorea el bit 16 (COUNTFLAG) del registro CTRL. Este bit se pondrá en 1 automáticamente cuando el contador llegue a cero. El bucle se detiene cuando esto sucede.

```c
// Esperar en un bucle hasta que la bandera se levante
while (!(SysTick->CTRL & (1 << 16)));
```

**Nota:** Una vez que la bandera se lee, se limpia automáticamente por hardware.

##### Paso 6 (Opcional): Detener el Timer
Para ahorrar energía, deshabilita el SysTick una vez que el retardo ha finalizado.

```c
SysTick->CTRL = 0;
```

##### Código de ejemplo:

```c
/**
 * @brief Genera un retardo en milisegundos usando el SysTick.
 * @param milisegundos: Tiempo de espera en ms.
 */
void delay_ms_systick(uint32_t milisegundos) {
    // 1. Calcular los ticks necesarios.
    // (SystemCoreClock / 1000) nos da los ticks por milisegundo.
    uint32_t ticks = (SystemCoreClock / 1000) * milisegundos;

    // 2. Cargar el valor de recarga (N-1). El máximo es 0xFFFFFF.
    SysTick->LOAD = ticks - 1;

    // 3. Resetear el contador actual.
    SysTick->VAL = 0;

    // 4. Habilitar el SysTick (usando el reloj del CPU).
    // Bit 2: CLKSOURCE=1 (CPU clock), Bit 0: ENABLE=1 (Habilitar)
    SysTick->CTRL = (1 << 2) | (1 << 0);

    // 5. Esperar hasta que la bandera COUNTFLAG (bit 16) se ponga en 1.
    while (!(SysTick->CTRL & (1 << 16)));

    // 6. Deshabilitar el SysTick.
    SysTick->CTRL = 0;
}
```
