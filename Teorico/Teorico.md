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

## Introducción a la Gestión de Clocks

Antes de que el microcontrolador pueda ejecutar una sola instrucción o mover un solo bit en un pin GPIO, su sistema de reloj debe estar configurado. El clock del microcontrolador; sincroniza todas las operaciones internas.

### ¿Por Qué son Importantes los Clocks?

1.  **Velocidad de Ejecución:** La frecuencia del clock principal determina cuántas instrucciones por segundo puede ejecutar la CPU. Un clock más rápido significa un programa más rápido.
2.  **Funcionamiento de Periféricos:** Cada periférico (UART, Timers, ADC, etc.) necesita una señal de reloj para funcionar. La frecuencia de este reloj determina sus velocidades de operación.
3.  **Gestión de Energía:** Una de las tareas más importantes es **habilitar el reloj solo para los periféricos que vamos a usar**. Si no usas el ADC, apagas su reloj. Esto reduce drásticamente el consumo de energía del chip.

### Programación del timer SysTick

El SysTick es un temporizador estándar de 24 bits integrado en el núcleo de todos los procesadores ARM Cortex-M, incluido el del LPC1769. Es la herramienta preferida para crear retardos sencillos y para servir como base de tiempo ("tick") en Sistemas Operativos de Tiempo Real (RTOS).

*¿Por Qué Usar SysTick?*

- **Simplicidad**: Se configura con solo 3 registros. No requiere habilitar clocks en PCONP como los periféricos de NXP.
- **Portabilidad**: El código que usa SysTick es directamente portable a cualquier otro microcontrolador con núcleo Cortex-M.
- **Eficiencia**: Está diseñado para ser una base de tiempo de baja sobrecarga para el sistema.

#### SysTick vs. Timers Periféricos (Timer0, Timer1...)

| Característica | SysTick (Recomendado para Delays) | Timers Periféricos (Timer0, etc.) |
| :--- | :--- | :--- |
| **Uso Ideal** | Delays sencillos, base de tiempo para RTOS. | Tareas complejas: PWM, medir pulsos, etc. |
| **Complejidad** | Muy baja. | Media. Requiere configuración de `PCONP`. |
| **Portabilidad** | Alta (Estándar de ARM). | Nula (Específico de NXP). |
| **Cantidad** | Solo hay uno. | Hay varios disponibles (Timer0, 1, 2, 3). |

---

#### Los 3 Registros Clave del SysTick

Todo el control se realiza a través de 3 registros accesibles directamente.

| Registro | Nombre Completo | Propósito Principal |
| :--- | :--- | :--- |
| `SysTick->CTRL` | Control and Status Register | **El interruptor principal.** Habilita/deshabilita el timer, selecciona la fuente de reloj y contiene la bandera que indica que el tiempo ha finalizado. |
| `SysTick->LOAD` | Reload Value Register | **El valor de cuenta.** Aquí se carga el número de "ticks" que queremos que el contador espere. Es un contador descendente, así que cuenta desde este valor hasta cero. |
| `SysTick->VAL` | Current Value Register | **El contador actual.** Muestra el valor actual del conteo. Escribir cualquier valor en él lo resetea a 0 de inmediato. |

---

#### Flujo de Programación para un Retardo (Polling)

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

--- 

### El Sistema de Clocks del LPC1769

El proceso para generar el clock principal del sistema sigue una cadena:

`Fuente de Reloj` -> `PLL0 (Multiplicador)` -> `Divisor de CPU` -> `Reloj final (CCLK)`

1.  **Fuente de Reloj:** El "marcapasos" inicial. Puede ser:
    *   **Oscilador RC Interno (IRC):** Viene dentro del chip (~4 MHz). Es impreciso pero funciona sin componentes externos.
    *   **Oscilador Principal (Externo):** Se conecta un cristal de cuarzo externo (típicamente de 12 MHz). Es muy preciso y es la opción recomendada.

2.  **PLL:** Es un circuito que toma una frecuencia de entrada baja y la multiplica para generar una frecuencia muy alta. Esto permite que el núcleo del procesador funcione a velocidades elevadas como 100 MHz.

3.  **Divisor de CPU (CCLKSEL):** Toma la salida del PLL y la divide para obtener la frecuencia final del procesador.

### Pasos Prácticos para Configurar los Clocks

La configuración de los relojes se realiza manipulando registros dentro del **System Control Block** (`LPC_SC`).

#### Paso 0: Habilitar el Clock del Periférico que Vas a Usar (`PCONP`)

Esta es la regla de oro: **antes de configurar o usar CUALQUIER periférico, debes darle energía habilitando su reloj.** Esto se hace en el registro `PCONP`.

Por defecto, tras un reset, solo el clock para GPIO está habilitado.

```c
// Para poder usar los registros PINSEL (parte del bloque PINCONNECT)
// debemos habilitar su reloj. PCPIN es el bit 15.
LPC_SC->PCONP |= (1 << 15);

// Habilitar el reloj para el periférico GPIO (ya está por defecto, pero es bueno saberlo)
// PCGPIO es el bit 15, pero ¡ojo!, el bloque PINCONNECT y GPIO son cosas distintas
// aunque a menudo se usan juntos. ¡El manual de usuario es clave aquí!
// Para GPIO, no se necesita PCONP, pero sí para los otros periféricos como UART, Timers, etc.

// Ejemplo: Habilitar reloj para el Timer0 (bit 1) y UART0 (bit 3)
LPC_SC->PCONP |= (1 << 1) | (1 << 3);
```

**¡Importante!** Un error muy común es intentar escribir en un registro de un periférico (ej: LPC_UART0->LCR) sin haber habilitado su clock en PCONP primero. La escritura simplemente no tendrá efecto.

### Configuración del Clock Principal

1. Seleccionar la fuente de reloj:

```c
// Selecciona el Oscilador Principal (cristal externo) como fuente para el PLL0
LPC_SC->CLKSRCSEL = (1 << 0);
```

2. Configurar el PLL0 para multiplicar la frecuencia:
La meta es llegar a un CCLK de 100 MHz desde un cristal de 12 MHz.

- Se usan un multiplicador (M) y un divisor (N).
- Se calcula una frecuencia intermedia F_CCO = (2 * M * F_in) / N. F_CCO debe estar entre 275 y 550 MHz.
- Para 100 MHz, una configuración común es: M=50, N=3.
- F_CCO = (2 * 50 * 12 MHz) / 3 = 400 MHz.

```c
// Configurar multiplicador y divisor (M-1 y N-1)
// M = 50 -> M-1 = 49. N = 3 -> N-1 = 2
LPC_SC->PLL0CFG = (49 << 0) | (2 << 16); 
LPC_SC->PLL0CON = (1 << 0); // Habilitar PLL0
LPC_SC->PLL0FEED = 0xAA;    // Secuencia de alimentación obligatoria
LPC_SC->PLL0FEED = 0x55;

// Esperar a que el PLL se "enganche" (se estabilice)
while (!(LPC_SC->PLL0STAT & (1 << 26)));

LPC_SC->PLL0CON = (1 << 0) | (1 << 1); // Conectar PLL0 como fuente de reloj
LPC_SC->PLL0FEED = 0xAA;
LPC_SC->PLL0FEED = 0x55;
```

3. Configurar el divisor de la CPU para obtener 100 MHz:

Tenemos F_CCO a 400 MHz. Queremos CCLK a 100 MHz. Necesitamos dividir por 4.

- El divisor es el valor del registro + 1. Para dividir por 4, escribimos un 3.

```c
// CCLKCFG = 3, significa que el divisor es 4
LPC_SC->CCLKCFG = 3;
```

**Resumen del Flujo de Clocks**

Siempre que se use un nuevo periférico, primero ve al registro PCONP y enciende su bit correspondiente.
La configuración del reloj principal es compleja, pero generalmente se hace una sola vez al inicio del sistema y no se vuelve a tocar. Es bueno entenderla para saber a qué velocidad está corriendo tu microcontrolador.

---

## Programación de Timers (Para Delays Precisos)

Aunque un bucle `for` puede crear un retardo, es una mala práctica porque es **bloqueante** e **impreciso**. La forma profesional de manejar el tiempo es usando un periférico de hardware: el **Timer**.

### ¿Cómo Funciona un Timer?

Imagina un contador de números que avanza a un ritmo perfectamente constante. Nosotros podemos:
1.  Establecer la **velocidad** a la que cuenta.
2.  Decirle que nos **avise** cuando llegue a un número específico.
3.  Iniciar, detener y resetear el conteo.

### Registros Clave del Timer0

| Registro         | Propósito                                                                                                | Ejemplo de Uso                                     |
| ---------------- | -------------------------------------------------------------------------------------------------------- | -------------------------------------------------- |
| `LPC_TIM0->TCR`  | **Timer Control Register:** El interruptor principal. Habilita (`1`), deshabilita (`0`) y resetea el contador. | `LPC_TIM0->TCR = (1 << 0);` // Habilita el timer |
| `LPC_TIM0->PR`   | **Prescale Register:** Define la velocidad del contador. `Velocidad = PCLK / (PR + 1)`.                     | `LPC_TIM0->PR = 24;` // Hace que cuente cada 1us si PCLK=25MHz |
| `LPC_TIM0->MR0`  | **Match Register 0:** El número objetivo. Cuando el contador `TC` llega a este valor, ocurre un "match".     | `LPC_TIM0->MR0 = 1000;` // El objetivo es 1000 |
| `LPC_TIM0->MCR`  | **Match Control Register:** ¿Qué hacer en un "match"? ¿Generar una bandera? ¿Detener el timer? ¿Resetear? | `LPC_TIM0->MCR = 3;` // Genera bandera y resetea en match |
| `LPC_TIM0->IR`   | **Interrupt Register:** Contiene las "banderas" que se levantan cuando ocurre un evento (como un match).  | `while (!(LPC_TIM0->IR & 1));` // Espera a la bandera |

### Ejemplo: Función `delay_ms` con Timer0

Esta función implementa un retardo preciso esperando a que el timer complete su cuenta.

```c
/**
 * @brief Genera un retardo preciso usando el Timer0.
 * @param milisegundos: Cantidad de milisegundos a esperar.
 * @note Asume que el Timer0 ya ha sido configurado con un Prescaler
 *       para contar en microsegundos (PR=24 para PCLK=25MHz).
 */
void delay_ms(uint32_t milisegundos){
    // 1. Cargar el valor final en el Match Register.
    // Si el timer cuenta en us, para ms multiplicamos por 1000.
    LPC_TIM0->MR0 = milisegundos * 1000;

    // 2. Configurar el MCR para que en el match:
    //    - Genere una bandera de interrupción (bit 0).
    //    - Detenga el timer (bit 2).
    LPC_TIM0->MCR = (1 << 0) | (1 << 2);
    
    // 3. Resetear y habilitar el timer.
    LPC_TIM0->TCR = (1 << 1); // Reset
    LPC_TIM0->TCR = (1 << 0); // Habilitar

    // 4. Esperar (polling) hasta que se levante la bandera de match en MR0.
    while (!(LPC_TIM0->IR & (1 << 0)));

    // 5. Limpiar la bandera para el próximo uso.
    LPC_TIM0->IR |= (1 << 0);
}