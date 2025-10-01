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

### ¿Cómo Funciona la "Limpieza" de Bits?

La línea `LPC_PINCON->PINSEL4 &= ~(3 << 20);` puede parecer compleja, pero es una técnica estándar y muy eficiente para modificar registros. Su objetivo es muy específico: **poner en `0` ciertos bits sin alterar el resto**.

Veámoslo paso a paso, usando el ejemplo de `P2.10` que modifica los bits 20 y 21 del registro `PINSEL4`.

#### Paso A: La Máscara Base (`3`)

Primero, elegimos el número `3`. ¿Por qué? Porque en binario, `3` es `11`. Este patrón de dos bits `11` es exactamente lo que necesitamos para "apuntar" a los dos bits que controlan la función de un pin.

`Valor decimal 3  =  ...0000 0000 0011 (en binario)`

#### Paso B: El Desplazamiento (`3 << 20`)

Ahora, necesitamos mover ese patrón `11` a la posición correcta dentro del registro de 32 bits. Para el pin `P2.10`, sabemos que sus bits de configuración empiezan en la posición `20`.

El operador `<<` (desplazamiento a la izquierda) toma los bits de `3` y los "empuja" 20 lugares hacia la izquierda, rellenando con ceros a la derecha.

-   **Antes del desplazamiento:** `...00000000 00000000 00000000 00000011`
-   **Después de `<< 20`:**   `...00000011 00000000 00000000 00000000`

Visualmente, el patrón `11` ahora está en las posiciones 21 y 20:

```
Bits: ... 22 21 20 19 ...
Valor: ... 0 1 1 0 ...
```


A esto lo llamamos una **máscara**. Es una plantilla que nos dice qué bits nos interesan.

#### Paso C: La Inversión (`~`)

Nuestro objetivo es poner los bits 20 y 21 en `00` y **dejar todos los demás como están**. Si usáramos la máscara `...001100...` directamente, afectaríamos solo a esos bits, pero no de la forma que queremos.

Aquí es donde entra el operador `~` (NOT bit a bit). Invierte cada uno de los 32 bits de la máscara: cada `0` se convierte en `1` y cada `1` se convierte en `0`.

-   **Máscara original:** `0000...001100...0000`
-   **Máscara invertida:** `1111...110011...1111`

Ahora tenemos una nueva máscara donde los bits que queremos modificar son `00` y todos los demás son `1`.

```
Bits: ... 22 21 20 19 ...
Valor: ... 1 0 0 1 ...
```

#### Paso D: La Operación AND (`&=`)

Finalmente, aplicamos esta máscara invertida al registro `PINSEL4` usando el operador `&=` (AND y asignación). La operación AND bit a bit tiene dos reglas muy útiles:

1.  `Cualquier_bit AND 1 = Cualquier_bit` (El bit original no cambia)
2.  `Cualquier_bit AND 0 = 0` (El bit se fuerza a ser `0`)

Esto es exactamente lo que necesitamos. Al aplicar nuestra máscara invertida, logramos el efecto deseado:

-   Los bits de `PINSEL4` que se multiplican por `1` (todos excepto el 20 y 21) **conservan su valor original**.
-   Los bits 20 y 21 de `PINSEL4` que se multiplican por `0` **se limpian, es decir, se fuerzan a ser `0`**.

**Ejemplo Gráfico Final:**

Supongamos que el valor inicial de `PINSEL4` era `...101110...` en la zona de interés.

```
   .... 1110 1011 0101 ....    (Valor original de PINSEL4)
 & .... 1111 0011 1111 ....    (Nuestra máscara invertida ~(3 << 20))
   ------------------------
 = .... 1110 0011 0101 ....    (Resultado final)
```

Como se puede ver, solo los bits 20 y 21 fueron modificados a `0`, y el resto del registro quedó intacto.

### ¿Por qué no usar `|=` (OR y asignación)?

El operador `|=` se utiliza para **encender** bits (ponerlos en `1`), no para apagarlos. La lógica de la operación OR es:

1.  `Cualquier_bit OR 0 = Cualquier_bit` (El bit original no cambia)
2.  `Cualquier_bit OR 1 = 1` (El bit se fuerza a ser `1`)

Si hiciéramos `LPC_PINCON->PINSEL4 |= (3 << 20);`, estaríamos forzando los bits 20 y 21 a `11`, configurando la tercera función alternativa del pin, ¡que no es lo que queremos!

> **Conclusión Clave:**
> -   Usa `Registro &= ~(máscara);` para **limpiar** bits (ponerlos a `0`).
> -   Usa `Registro |= máscara;` para **establecer** bits (ponerlos a `1`).

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

### Análisis Detallado: Configurando la Dirección del Pin

A diferencia de `PINSEL` donde se usan 2 bits por pin, en el registro `FIODIR` cada pin es controlado por **un solo bit**. Esto simplifica un poco la máscara que necesitamos crear. Analicemos los dos casos.

---

#### Caso 1: Configurar un Pin como SALIDA (Poner un bit en 1)

Usaremos el ejemplo `LPC_GPIO0->FIODIR |= (1 << 22);` para configurar el pin `P0.22` como salida.

##### Paso A: La Máscara Base (`1`)

Para afectar a un solo bit, nuestra máscara inicial más simple es el número `1`, que en binario es `...0001`.

##### Paso B: El Desplazamiento (`1 << 22`)

Necesitamos mover ese bit `1` hasta la posición 22, que es la que controla al pin `P0.22`. Usamos el operador de desplazamiento a la izquierda `<<`.

-   **Antes del desplazamiento:** `0000 0000 0000 0000 0000 0000 0000 0001`
-   **Después de `<< 22`:**   `0000 0000 0100 0000 0000 0000 0000 0000`

Ahora tenemos una máscara con un `1` únicamente en la posición 22 y `0` en todas las demás.

##### Paso C: La Operación OR (`|=`)

Para encender el bit 22 sin alterar el estado de los otros 31 bits, usamos el operador `|=` (OR y asignación). Recordemos cómo funciona la operación OR:

1.  `Cualquier_bit OR 0 = Cualquier_bit` (El bit original no cambia)
2.  `Cualquier_bit OR 1 = 1` (El bit se fuerza a ser `1`)

Nuestra máscara es perfecta para esto: pone un `1` donde queremos y `0` en todos los demás sitios.

**Ejemplo Gráfico:**

Supongamos que el valor inicial de `FIODIR` tenía otros pines configurados como salida.

```
.... 0000 1000 0001 0010 .... (Valor original de FIODIR0)
.... 0100 0000 0000 0000 .... (Nuestra máscara (1 << 22))
.... 0100 1000 0001 0010 .... (Resultado final)
```


Como resultado, el bit 22 se ha encendido (`1`), convirtiendo al pin `P0.22` en **salida**, y todos los demás bits han conservado su valor original.

---

#### Caso 2: Configurar un Pin como ENTRADA (Poner un bit en 0)

Ahora, veamos el ejemplo `LPC_GPIO2->FIODIR &= ~(1 << 10);` para configurar el pin `P2.10` como entrada. El proceso es idéntico al que vimos para `PINSEL`.

##### Paso A y B: Crear la Máscara (`1 << 10`)

Primero, creamos una máscara que "apunte" al bit 10.

-   `1 << 10` nos da una máscara con un `1` solo en la posición 10: `...0000 0100 0000 0000`

##### Paso C: La Inversión (`~`)

Para apagar un bit, necesitamos aplicar una operación AND con una máscara que tenga un `0` en esa posición y `1` en todas las demás. Para lograrlo, simplemente invertimos la máscara anterior con el operador `~` (NOT).

-   **Máscara original:** `0000...0000 0100 0000 0000`
-   **Máscara invertida:** `1111...1111 1011 1111 1111`

##### Paso D: La Operación AND (`&=`)

Finalmente, usamos el operador `&=` (AND y asignación). Recordemos sus reglas:

1.  `Cualquier_bit AND 1 = Cualquier_bit` (El bit original no cambia)
2.  `Cualquier_bit AND 0 = 0` (El bit se fuerza a ser `0`)

**Ejemplo Gráfico:**

Supongamos que el pin `P2.10` ya estaba configurado como salida (su bit era `1`).

```
.... 0000 0100 0001 0010 .... (Valor original de FIODIR2)
.... 1111 1011 1111 1111 .... (Nuestra máscara invertida ~(1 << 10))
.... 0000 0000 0001 0010 .... (Resultado final)
```


El bit 10 se ha apagado (`0`), convirtiendo al pin `P2.10` en **entrada**, y el resto de los bits del registro `FIODIR2` permanecen intactos.

> **Conclusión Clave para `FIODIR`:**
> -   **SALIDA (bit = 1):** Usa OR para "sumar" un `1` en la posición deseada.
>     `LPC_GPIOx->FIODIR |= (1 << numero_de_pin);`
> -   **ENTRADA (bit = 0):** Usa AND con una máscara invertida para "restar" un `1` (o forzar un `0`).
>     `LPC_GPIOx->FIODIR &= ~(1 << numero_de_pin);`

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

### Escribiendo y Leyendo el Estado de un Pin

#### 1. `FIOSET` y `FIOCLR`: Operaciones Atómicas

Una operación atómica es aquella que el hardware ejecuta en un solo paso, sin posibilidad de ser interrumpida.

-   **Método Inseguro (Read-Modify-Write):** Si quisiéramos encender el bit 22 escribiendo en `FIOPIN`, tendríamos que:
    1.  Leer el valor actual de `FIOPIN`.
    2.  Modificar solo el bit 22 (usando una operación OR).
    3.  Escribir el nuevo valor completo de vuelta en `FIOPIN`.
    
    Este proceso de tres pasos es lento y peligroso. Si una interrupción ocurriera entre el paso 1 y el 3 y modificara otro pin del mismo puerto, ¡ese cambio se perdería!

-   **Método Seguro y Rápido (`FIOSET`/`FIOCLR`):**
    `LPC_GPIO0->FIOSET = (1 << 22);`
    
    Con esta única instrucción, le decimos al hardware: "No me importa el estado actual de los otros pines, simplemente **enciende** el pin 22". El hardware lo hace en un solo ciclo de reloj, de forma indivisible. Lo mismo ocurre con `FIOCLR` para apagar un pin. Es la forma más eficiente y correcta de hacerlo.

> **Nota Clave:** Los registros `FIOSET` y `FIOCLR` solo prestan atención a los bits que les envías como `1`. Si en la máscara un bit es `0`, el pin correspondiente simplemente no se ve afectado.

#### 2. Desglosando la Lectura con `FIOPIN`

La línea `if (LPC_GPIO2->FIOPIN & (1 << 10))` es la forma estándar de consultar el estado de un solo bit dentro de un registro. Veamos cómo funciona.

##### Paso A: Obtener el Estado de Todos los Pines

El registro `LPC_GPIO2->FIOPIN` nos da un "retrato" de 32 bits del estado de todos los pines del Puerto 2. Podría ser algo como esto si los pines 0 y 4 estuvieran en alto:

`Valor FIOPIN: ...0000 0000 0001 0001`

Pero a nosotros solo nos interesa el bit 10.

##### Paso B: Crear la Máscara de Aislamiento

Como en los pasos anteriores, creamos una máscara que solo tenga un `1` en la posición que nos interesa.

`Máscara (1 << 10): ...0000 0100 0000 0000`

##### Paso C: Aplicar la Máscara con AND (`&`)

Usamos el operador `&` (AND bit a bit) para "filtrar" el valor de `FIOPIN`. La operación `&` solo da como resultado `1` si ambos bits que se comparan son `1`.

-   Todos los bits de `FIOPIN` (excepto el 10) se compararán con un `0` de la máscara. El resultado siempre será `0`.
-   El bit 10 de `FIOPIN` se comparará con el `1` de la máscara.
    -   Si el bit 10 de `FIOPIN` es `0`, el resultado será `0`.
    -   Si el bit 10 de `FIOPIN` es `1`, el resultado será `1`.

**Ejemplo Gráfico (si el pin 10 está en ALTO):**

```
.... 0100 0000 0001 0001 .... (Valor de FIOPIN con pin 10 en ALTO)
.... 0100 0000 0000 0000 .... (Nuestra máscara (1 << 10))
.... 0100 0000 0000 0000 .... (Resultado: un número distinto de cero)
```

**Ejemplo Gráfico (si el pin 10 está en BAJO):**

```
.... 0000 0000 0001 0001 .... (Valor de FIOPIN con pin 10 en BAJO)
.... 0100 0000 0000 0000 .... (Nuestra máscara (1 << 10))
.... 0000 0000 0000 0000 .... (Resultado: cero)
```

##### Paso D: La Evaluación del `if`

En C, una condición `if` evalúa lo siguiente:
-   Si el resultado es **cero (`0`)**, la condición es **FALSA**.
-   Si el resultado es **cualquier número distinto de cero**, la condición es **VERDADERA**.

Por lo tanto:
-   `if (LPC_GPIO2->FIOPIN & (1 << 10))` será **VERDADERO** solo si el pin 10 está en ALTO.
-   Y será **FALSO** si el pin 10 está en BAJO.

> **Conclusión Clave para la Operación:**
> -   **Escribir:** Usa `FIOSET` y `FIOCLR` con una máscara `(1 << pin)` para operaciones rápidas y seguras.
> -   **Leer:** Usa `FIOPIN` con el operador `&` y una máscara `(1 << pin)` para aislar y comprobar el estado de un pin específico.

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

## Ampliando el Uso del SysTick: El Método de Interrupción (No Bloqueante)

Ya vimos cómo crear una función `delay()` que detiene el procesador. Pero, ¿qué pasa si necesitamos que una tarea se ejecute periódicamente **sin congelar nuestro programa principal**?

Aquí es donde entra en juego el método más potente del SysTick: las **interrupciones**.

### La Diferencia Clave: Bloqueante vs. No Bloqueante

*   **Polling (función `delay()`):** Es **bloqueante**.
*   **Interrupción (`SysTick_Handler()`):** Es **no bloqueante**.

### ¿Cómo Funciona una Interrupción?

En lugar de que nuestro código principal pregunte constantemente "¿ya terminó?", el hardware del microcontrolador se encarga de todo.

1.  **SysTick:** El temporizador cuenta hasta cero como siempre.
2.  **La Novedad:** Al llegar a cero, en lugar de solo levantar una bandera, el SysTick envía una "señal de ayuda" o una "solicitud de interrupción" al **NVIC**.
3.  **NVIC:** Es el "gestor de eventos" del procesador. Recibe la señal del SysTick.
4.  **El Salto:** El NVIC detiene forzosamente la ejecución del código `main()`, guarda su estado actual y obliga al procesador a saltar y ejecutar una función especial: `SysTick_Handler()`.
5.  **El Retorno:** Una vez que el código dentro de `SysTick_Handler()` termina, el NVIC restaura el estado anterior y el programa `main()` continúa exactamente donde se quedó, como si nada hubiera pasado.

### Flujo de Programación para una Interrupción

El proceso es similar al de polling, pero con dos diferencias cruciales en la configuración.

#### Paso 1: Calcular y Cargar el Valor de Recarga (`LOAD`)
Este paso es idéntico. Calculamos los ticks necesarios para el intervalo deseado.

```c
// Para una interrupción cada 1ms con CPU a 100MHz
// (100,000,000 / 1000) - 1 = 99999
SysTick->LOAD = (SystemCoreClock / 1000) - 1; 
```

#### Paso 2: Resetear el Contador (`VAL`)
También es idéntico. Limpiamos el contador para un inicio limpio.

```c
SysTick->VAL = 0;
```

#### Paso 3: Habilitar el Timer y la Interrupción (`CTRL`)
Aquí está la primera gran diferencia. Además de habilitar el timer y seleccionar la fuente de reloj, debemos activar el bit que genera la solicitud de interrupción.
- `ENABLE` (Bit 0) = 1: Habilita el contador.
- `CLKSOURCE` (Bit 2) = 1: Usa el reloj del procesador (CCLK).
- `TICKINT` (Bit 1) = 1: ¡CLAVE! Habilita la solicitud de interrupción al llegar a cero.

```c
// Habilitar SysTick, reloj CPU Y la interrupción
SysTick->CTRL = (1 << 2) | (1 << 1) | (1 << 0);
```

#### Paso 4: Escribir el Código del "Handler"
Debemos crear la función que se ejecutará automáticamente. El nombre SysTick_Handler está predefinido por el sistema y no se puede cambiar.

```c
// Esta función será llamada por el hardware cada vez que el SysTick llegue a cero.
void SysTick_Handler(void) {
    // Aquí va el código que queremos ejecutar periódicamente.
    // Ejemplo: Invertir el estado de un LED, incrementar un contador, etc.
}
```

---

### Código de Ejemplo: Un Contador que se Incrementa "en Segundo Plano"
Este ejemplo configura el SysTick para que interrumpa cada milisegundo e incremente una variable global. El bucle main() queda libre para otras tareas.

```c
#include "LPC17xx.h"

// Usamos 'volatile' para asegurar que el compilador no optimice la variable,
// ya que es modificada por una interrupción y leída en el main.
volatile uint32_t ms_ticks = 0;

/**
 * @brief Esta función se ejecuta automáticamente cada 1ms gracias al SysTick.
 */
void SysTick_Handler(void) {
    ms_ticks++; // Incrementa el contador de milisegundos
}

int main(void) {
    // 1. Configurar el SysTick para que interrumpa cada 1ms.
    // Asume SystemCoreClock ya está configurado (ej: 100MHz).
    SysTick_Config(SystemCoreClock / 1000);

    // Bucle principal infinito.
    // El microprocesador ejecutará este código, y será interrumpido
    // cada 1ms para ejecutar SysTick_Handler().
    while(1) {
        // El programa principal puede hacer otras cosas aquí.
        // Por ejemplo, podríamos comprobar el valor de ms_ticks:
        if (ms_ticks > 5000) {
            // Han pasado 5 segundos... hacer algo.
            // ...
        }
    }
    return 0; // Nunca se llega aquí
}
```

### Resumen: Polling vs. Interrupción

| Característica | `delay()` (Polling) | `SysTick_Handler()` (Interrupción) |
| :--- | :--- | :--- |
| **¿CPU Ocupada?** | **Sí.** 100% ocupada esperando. | **No.** Libre para otras tareas. |
| **Naturaleza** | **Bloqueante** (detiene todo). | **No Bloqueante** (trabaja en segundo plano). |
| **Bit `TICKINT` (`CTRL[1]`)**| `0` (Desactivado). | `1` (Activado). |
| **Cuándo Usarlo** | Para pausas simples y cortas donde no importa detener el sistema. | Para tareas periódicas, bases de tiempo, o cuando el programa principal debe seguir funcionando. |

#### Análisis Detallado: Los Registros del SysTick en Acción

Para entender cómo funciona el SysTick, debemos analizar los bits específicos que manipulamos en sus registros. Cada operación de configuración es una instrucción precisa que le damos al hardware.

---

#### 1. El Registro de Control (`SysTick->CTRL`): El Panel de Mandos

Este registro de 32 bits es el centro de operaciones del SysTick. Aunque tiene muchos bits, solo nos interesan cuatro para empezar. Piénsalo como un panel con interruptores clave.

| Bit | Nombre      | Función (cuando el bit es 1)                                        |
|:---:|:------------|:--------------------------------------------------------------------|
|  0  | `ENABLE`    | **Enciende el contador.** Sin este bit, el SysTick está apagado.     |
|  1  | `TICKINT`   | **Habilita la interrupción.** Le dice al SysTick que "pida ayuda" (interrumpa al CPU) cuando llegue a cero. |
|  2  | `CLKSOURCE` | **Selecciona el reloj.** Si es 1, usa el reloj del procesador (`SystemCoreClock`). Si es 0, usa una fuente externa (menos común). |
| 16  | `COUNTFLAG` | **Bandera de finalización.** El hardware la pone en 1 automáticamente cuando el contador llega a cero. Es de solo lectura. |

Ahora, veamos cómo las líneas de código manipulan estos "interruptores":

##### Configuración para un `delay()` (Polling):

```c
// Habilitar SysTick con el reloj del CPU
SysTick->CTRL = (1 << 0) | (1 << 2);
```

- `(1 << 0)`: Crea una máscara `...0001`. Le estamos diciendo: "activa el bit 0" -> `ENABLE`.
- `(1 << 2)`: Crea una máscara `...0100`. Le estamos diciendo: "activa el bit 2" -> `CLKSOURCE`.
- `|` (OR): Combina ambas máscaras. El valor final que escribimos es `...0101`.
- **Traducción**: "SysTick, enciéndete (`ENABLE`) y usa la velocidad del CPU (`CLKSOURCE`) como referencia de tiempo". El bit `TICKINT` queda en 0, por lo que no generará interrupciones.

```c
// Esperar en un bucle hasta que la bandera se levante
while (!(SysTick->CTRL & (1 << 16)));
```

- `(1 << 16)`: Crea una máscara para aislar el bit `COUNTFLAG`.
- `SysTick->CTRL & ...`: La operación AND compara el estado actual del registro con nuestra máscara.
    - Si `COUNTFLAG` es 0, el resultado de la operación AND es 0.
    - Si `COUNTFLAG` es 1, el resultado es un número distinto de cero (0x10000).
- `!(...)`: El operador NOT lógico invierte el resultado.
    - Si el resultado del AND es `0`, `!(0)` es verdadero, y el bucle `while` continúa.
    - Si el resultado es distinto de cero, `!(distinto de cero)` es falso, y el bucle `while` termina.
- Traducción: "Quédate atrapado en este bucle mientras la bandera `COUNTFLAG` esté apagada. Sal en el instante en que se encienda".

```c
// Habilitar SysTick, reloj CPU Y la interrupción
SysTick->CTRL = (1 << 2) | (1 << 1) | (1 << 0);
```

- Esta línea es casi idéntica a la de polling, pero añade un "interruptor" más.
- `(1 << 1)`: Activa el bit 1 -> `TICKINT`.
- **Traducción**: "SysTick, enciéndete (`ENABLE`), usa la velocidad del CPU (`CLKSOURCE`) y, lo más importante, avísame con una interrupción (`TICKINT`) cada vez que termines la cuenta".

---

#### 2. El Valor de Cuenta: ¿Por Qué LOAD = N - 1?
El SysTick es un **contador descendente**. Esto significa que cuenta desde el valor que le cargamos en `LOAD` **hasta llegar a cero**.

Veamos un ejemplo simple. Si queremos esperar 3 ciclos de reloj:

- Si cargamos `LOAD = 3`, el contador hará la secuencia: `3 -> 2 -> 1 -> 0`. Esto son **4** estados o "ticks". ¡Un tick de más!
- Si cargamos `LOAD = 2` (es decir, 3 - 1), el contador hará la secuencia: `2 -> 1 -> 0`. Esto son **3** estados. ¡Correcto!

La transición de `1` a `0` es el momento en que se activa la bandera `COUNTFLAG`. Como la cuenta incluye el estado `0`, siempre debemos cargar un valor de `N-1` para obtener `N` ticks de retardo.

---

#### 3. El Reset: ¿Por Qué VAL = 0?
El registro `SysTick->VAL` contiene el valor actual del contador. Es de solo lectura, excepto por una característica especial definida por el hardware de ARM:

Escribir **cualquier valor** en `SysTick->VAL` lo fuerza a resetearse a **0** inmediatamente. Al hacerlo, también provoca que el contador se recargue con el valor que está en `SysTick->LOAD` en el siguiente ciclo de reloj.
   
Lo usamos para garantizar que nuestra cuenta comience desde un estado conocido y limpio, eliminando cualquier valor residual de una operación anterior.

---

#### 4. El Nombre Mágico: SysTick_Handler()
¿Por qué esa función tiene ese nombre exacto? ¿Y por qué no la llamamos desde `main()`?

La respuesta está en la **Tabla de Vectores de Interrupción**. Cuando el microcontrolador arranca, carga en una zona especial de la memoria una tabla que funciona como un "índice de emergencias". Cada tipo de interrupción (SysTick, Timer0, UART, etc.) tiene una entrada fija en esta tabla.

1. **El Evento**: El SysTick termina su cuenta y, como activamos el bit TICKINT, le envía una señal al controlador de interrupciones (NVIC).
2. **La Búsqueda**: El NVIC identifica que la señal viene del SysTick. Va a la Tabla de Vectores y busca la entrada correspondiente a "SysTick".
3. **El Salto**: En esa entrada encuentra la dirección de memoria de una función. El hardware fuerza al procesador a detener lo que estaba haciendo en main() y a saltar a ejecutar el código en esa dirección.
4. **El Nombre**: El nombre de la función que el compilador y el enlazador colocan en esa dirección es, por convención, `SysTick_Handler`.

El nombre está predefinido en los archivos de inicio del proyecto (ej: `startup_LPC17xx.c`). No podemos cambiarlo porque es el nombre que el hardware está programado para buscar. Nosotros simplemente le damos "cuerpo" a esa función para que haga algo útil.

**Conclusión**: No llamamos a `SysTick_Handler()`. Es el **hardware** el que la llama por nosotros cuando ocurre el evento de interrupción, usando la Tabla de Vectores como guía.

---

**Codigo de ejemplo:**

```c
#include "LPC17xx.h"

// 'volatile' evita que el compilador optimice la variable, ya que se
// modifica en una interrupción y se lee en el main.
volatile uint32_t ms_ticks = 0;

/**
 * @brief Esta función se ejecuta automáticamente cada 1ms gracias al SysTick.
 */
void SysTick_Handler(void) {
    ms_ticks++; // Incrementa el contador de milisegundos
}

int main(void) {
    // Configura el SysTick para interrumpir cada 1ms (1000 veces por segundo).
    // SysTick_Config es una función de conveniencia que hace los 3 pasos:
    // 1. Calcula y carga LOAD = (SystemCoreClock / 1000) - 1
    // 2. Resetea VAL
    // 3. Configura CTRL para habilitar SysTick, reloj y la INTERRUPCIÓN.
    SysTick_Config(SystemCoreClock / 1000);

    // Bucle principal infinito. El CPU ejecuta este código y es interrumpido
    // cada 1ms para ejecutar SysTick_Handler().
    while(1) {
        // El programa principal está libre para hacer otras cosas.
        // Por ejemplo, podríamos usar la base de tiempo creada:
        if ((ms_ticks % 500) == 0) {
            // Cada 500ms, hacer algo (ej: parpadear un LED).
        }
    }
    return 0; // Nunca se llega aquí
}
```

---

## El Convertidor Analógico-Digital (ADC)

### 1. ¿Qué es y por qué lo necesitamos?

El microcontrolador vive en un mundo binario de `1`s y `0`s (ALTO y BAJO). Pero el mundo real es **analógico**: la temperatura, la luz, el volumen o la posición de una perilla no son solo "encendido" o "apagado", sino que tienen un rango continuo de valores.

El **Convertidor Analógico-Digital (ADC)** es el traductor que mide una señal analógica (como un voltaje) y la convierte en un número digital que el procesador puede entender.

En el LPC1769, el ADC tiene una resolución de **12 bits**. Esto significa que puede representar un voltaje de entrada con 2¹² = **4096** niveles distintos (del 0 al 4095).

---

### 2. El Proceso de Conversión en 6 Pasos

Al igual que con el SysTick, interactuar con el ADC sigue una secuencia lógica y repetible. Para realizar una única lectura (método de "polling" o sondeo), seguiremos estos pasos:

1.  **Paso 1: Encender el Periférico (PCONP)**
    -   Registro: `LPC_SC->PCONP`
    -   Propósito: Darle energía al bloque de hardware del ADC. Sin esto, el periférico está "dormido" para ahorrar energía.
2.  **Paso 2: Configurar la Función del Pin (PINSEL)**
    -   Registro: `LPC_PINCON->PINSELx`
    -   Propósito: Indicarle al pin que su función no será GPIO, sino **entrada analógica**.
3.  **Paso 3: Configurar el ADC (AD0CR)**
    -   Registro: `LPC_ADC->AD0CR`
    -   Propósito: El "panel de control" del ADC. Aquí seleccionamos el canal a leer, la velocidad de conversión y lo activamos.
4.  **Paso 4: Iniciar la Conversión (AD0CR)**
    -   Registro: `LPC_ADC->AD0CR`
    -   Propósito: Dar la orden de "¡Mide ahora!".
5.  **Paso 5: Esperar a que Termine (AD0GDR)**
    -   Registro: `LPC_ADC->AD0GDR`
    -   Propósito: Monitorear una bandera que nos avisa cuando la conversión ha finalizado.
6.  **Paso 6: Leer el Resultado (AD0GDR)**
    -   Registro: `LPC_ADC->AD0GDR`
    -   Propósito: Extraer el número digital de 12 bits del registro de resultados.

---

### Análisis Detallado: Los Registros del ADC en Acción

#### Paso 1: Encender el Periférico (`PCONP`)

El registro `PCONP` (Power Control for Peripherals) es el interruptor general. Cada bit controla la energía de un periférico diferente. Para el ADC, necesitamos encender el bit 12.

```c
// Habilita la energía para el periférico ADC (PCADC = bit 12)
LPC_SC->PCONP |= (1 << 12);
```

#### Paso 2: Configurar el Pin (`PINSEL` y `PINMODE`)

Supongamos que queremos leer del canal 0 del ADC, que está en el pin **P0.23**.

1.  **Seleccionar la función:** Consultando el manual, vemos que la función ADC para P0.23 es la primera alternativa (`01`).
    -   Pin `P0.23` -> `y=23`, por lo que usamos `PINSEL1`.
    -   Posición de los bits: `(23 - 16) * 2 = 14`. Debemos modificar los bits **14** y **15**.
    -   Para poner `01`, necesitamos que el bit 14 sea `1` y el bit 15 sea `0`.

    ```c
    // Pone el bit 14 en 1 para seleccionar la función AD0.0
    LPC_PINCON->PINSEL1 |= (1 << 14);
    // Pone el bit 15 en 0
    LPC_PINCON->PINSEL1 &= ~(1 << 15);
    ```

2.  **Modo del pin (Opcional pero recomendado):** Para entradas analógicas, es crucial que no haya resistencias internas de pull-up o pull-down que puedan alterar la medición. Deshabilitamos ambas.
    -   El registro `PINMODE` usa 2 bits por pin, igual que `PINSEL`. `10` significa "sin pull-up ni pull-down".
    
    ```c
    // Configura P0.23 en modo "ni pull-up ni pull-down" (valor 10)
    // Bit 15=1, Bit 14=0
    LPC_PINCON->PINMODE1 |= (1 << 15);
    LPC_PINCON->PINMODE1 &= ~(1 << 14);
    ```

#### Paso 3: Configurar el Registro de Control (`AD0CR`)

Este es el registro más importante. Lo configuraremos para una lectura simple.

| Bits    | Nombre    | Descripción                                                                                                 |
|:--------|:----------|:--------------------------------------------------------------------------------------------------------------|
| `0-7`   | `SEL`     | **Selector de Canal.** Un bitmask. Para leer el canal 0, ponemos un `1` en el bit 0. `(1 << 0)`.                |
| `8-15`  | `CLKDIV`  | **Divisor de Reloj.** El ADC necesita un reloj < 13 MHz. Si CCLK=100MHz, PCLK=25MHz. `CLKDIV=1` divide por 2 (12.5MHz), lo cual es seguro. |
| `16`    | `BURST`   | `0` para conversiones controladas por software (una a la vez).                                                |
| `21`    | `PDN`     | **Power Down.** `1` para poner el ADC en modo operacional (¡Sí, `1` para encender!). `0` para apagarlo.       |
| `24-26` | `START`   | `000` por ahora. Usaremos este campo más tarde para iniciar la conversión.                                    |

```c
// 1. Configurar el divisor de reloj. (PCLK/ (CLKDIV+1))
//    Si CCLK=100MHz, PCLK_ADC=25MHz. 25MHz/(1+1) = 12.5MHz. Seguro.
uint32_t clkdiv = 1;

// 2. Escribir en el registro de control
//    PDN=1 (operacional), CLKDIV, SEL (lo pondremos en la función de lectura)
LPC_ADC->AD0CR = (1 << 21) | (clkdiv << 8);
```

#### Pasos 4, 5 y 6: El Ciclo de Lectura

Estos tres pasos se realizan juntos cada vez que queremos una nueva medición.

1.  **Seleccionar canal e Iniciar:**
    -   Modificamos `AD0CR` para seleccionar el canal (ej. `1 << 0` para el canal 0) y para iniciar la conversión (`1 << 24` para START NOW).

2.  **Esperar a que termine:**
    -   El registro `AD0GDR` (Global Data Register) contiene la bandera `DONE` en el bit 31. Hacemos un bucle hasta que este bit se ponga en `1`.

3.  **Leer el resultado:**
    -   El resultado de 12 bits está en los bits `15:4` del mismo registro `AD0GDR`. Debemos desplazarlo y enmascararlo.

---

### Código de Implementación: Funciones Reutilizables

Vamos a encapsular esta lógica en dos funciones: `ADC_Init()` y `ADC_Read()`.

```c
/**
 * @brief Inicializa el periférico ADC.
 */
void ADC_Init(void) {
    // 1. Encender la energía del ADC (PCONP, bit 12)
    LPC_SC->PCONP |= (1 << 12);

    // 2. Configurar el pin P0.23 (AD0.0) como entrada analógica
    //    Función 01 en PINSEL1[15:14]
    LPC_PINCON->PINSEL1 |= (1 << 14);
    LPC_PINCON->PINSEL1 &= ~(1 << 15);
    //    Deshabilitar pull-up/pull-down en PINMODE1[15:14] (modo 10)
    LPC_PINCON->PINMODE1 |= (1 << 15);
    LPC_PINCON->PINMODE1 &= ~(1 << 14);

    // 3. Configurar el registro de control del ADC
    //    PDN=1 (operacional), CLKDIV=1 (para 12.5MHz con PCLK=25MHz)
    LPC_ADC->AD0CR = (1 << 21) | (1 << 8);
}

/**
 * @brief Lee un valor de un canal específico del ADC.
 * @param channel: El número del canal a leer (0 a 7).
 * @return El valor digital de 12 bits (0-4095).
 */
uint16_t ADC_Read(uint8_t channel) {
    uint32_t reg_val;

    // 4. Iniciar la conversión
    //    Construye el comando: selecciona el canal Y pone START en 1
    reg_val = (1 << channel) | (1 << 24);
    //    Sobrescribe solo los bits de canal y START, manteniendo la configuración
    LPC_ADC->AD0CR &= ~((0xFF) | (7 << 24)); // Limpia bits SEL y START
    LPC_ADC->AD0CR |= reg_val;

    // 5. Esperar a que la conversión termine (DONE bit 31 en AD0GDR)
    while (!(LPC_ADC->AD0GDR & (1U << 31)));

    // 6. Leer el resultado
    reg_val = LPC_ADC->AD0GDR;
    
    // Extraer el valor de 12 bits (bits 15:4) y retornar
    return (reg_val >> 4) & 0xFFF;
}
```

### Ejemplo Completo: Controlar la Velocidad de un LED con un Potenciómetro

Este programa hace parpadear un LED a una velocidad que depende de la posición de un potenciómetro conectado al canal 0 del ADC.

```c
#include "LPC17xx.h"
// Incluir las funciones de Systick y GPIO que ya hemos definido
// ... void delay_ms_systick(uint32_t ms);
// ... [funciones de inicialización de GPIO para un LED]

// [Pegar las funciones ADC_Init() y ADC_Read() aquí arriba]

int main(void) {
    uint16_t adc_value;
    uint32_t delay_time;

    // Configurar los relojes del sistema (ej. a 100MHz)
    SystemInit();
    
    // Inicializar el GPIO para el LED (ej. P0.22 como salida)
    LED_Init(); 

    // Inicializar el ADC
    ADC_Init();

    while(1) {
        // Leer el valor del potenciómetro en el canal 0
        adc_value = ADC_Read(0);

        // Mapear el valor del ADC (0-4095) a un rango de retardo (ej. 10-1000ms)
        // Regla de tres simple: delay = min_delay + (adc_value/4095.0) * (max_delay - min_delay)
        delay_time = 10 + (uint32_t)(adc_value * (990.0 / 4095.0));
        
        // Hacer parpadear el LED
        LED_On();
        delay_ms_systick(delay_time);
        LED_Off();
        delay_ms_systick(delay_time);
    }
    return 0;
}
```

## El Convertidor Digital-Analógico (DAC)

### 1. ¿Qué es y por qué lo necesitamos?

Si el ADC son los "oídos" del microcontrolador, el **Convertidor Digital-Analógico (DAC)** es su "voz". Es el proceso inverso al ADC: toma un número digital que el procesador le da y lo convierte en un voltaje analógico preciso en un pin de salida.

¿Para qué sirve esto?
-   **Generar Señales de Audio:** Crear sonidos y música.
-   **Controlar Motores:** Ajustar la velocidad de un motor de forma suave y precisa.
-   **Generar Formas de Onda:** Crear señales sinusoidales, triangulares o de diente de sierra para pruebas o instrumentación.
-   **Controlar la Intensidad de un LED:** Dimerizar un LED sin usar PWM.

El LPC1769 cuenta con un DAC de **10 bits**. Esto le permite generar 2¹⁰ = **1024** niveles de voltaje distintos, desde 0V hasta el voltaje de referencia (`VREF`).

### 2. El Proceso de 3 Pasos para Usar el DAC

A diferencia del ADC, el DAC es un periférico mucho más simple de configurar y usar. El proceso se reduce a estos tres pasos:

1.  **Paso 1: Configurar la Función del Pin (PINSEL)**
    -   Registro: `LPC_PINCON->PINSELx`
    -   Propósito: Indicarle al pin de salida que su función será **AOUT (Analog Output)**.
2.  **Paso 2: Inicializar el DAC**
    -   Registro: `LPC_DAC->DACR`
    -   Propósito: Asegurarse de que la salida comience en un estado conocido, como 0V.
3.  **Paso 3: Operar el DAC (DACR)**
    -   Registro: `LPC_DAC->DACR`
    -   Propósito: Escribir el valor digital (0-1023) que se desea convertir a un voltaje.

> **¡Noticia Importante!** A diferencia de casi todos los demás periféricos, el DAC **no tiene un bit de encendido en el registro `PCONP`**. Su energía está ligada directamente al pin `VDDA`, y su reloj (`PCLK_DAC`) está habilitado por defecto. ¡Esto nos ahorra un paso!

---

### Análisis Detallado: Los Registros del DAC en Acción

#### Paso 1: Configurar el Pin (`PINSEL`)

La salida del DAC está disponible en un pin específico. Según el manual del usuario (Tabla 74), este es el pin **P0.26**.

1.  **Seleccionar la función:** Consultando la Tabla 81 del manual (`PINSEL1`), vemos que la función `AOUT` para el pin `P0.26` es la **segunda función alternativa** (valor `10`).
    -   Pin `P0.26` -> `y=26`, por lo tanto, usamos el registro `PINSEL1`.
    -   Posición de los bits: `(26 - 16) * 2 = 20`. Debemos configurar los bits **21** y **20**.
    -   Para escribir el valor `10`, el bit 21 debe ser `1` y el bit 20 debe ser `0`.

La forma más segura y profesional de configurar esto es:
1.  Limpiar ambos bits a `00`.
2.  Luego, establecer solo el bit 21 a `1`.

```c
// 1. Limpia los bits 21 y 20 de PINSEL1 para P0.26
LPC_PINCON->PINSEL1 &= ~(3 << 20);

// 2. Establece la función 10 (AOUT)
//    (2 << 20) crea una máscara ...001000... para poner el bit 21 en 1 y el 20 en 0.
LPC_PINCON->PINSEL1 |= (2 << 20);
```

#### Paso 2 y 3: Operar con el Registro del Convertidor DAC (`DACR`)

El registro `LPC_DAC->DACR` (DAC Register) es el corazón del DAC. Es donde escribimos el valor que queremos convertir. Su estructura es un poco particular.

| Bits    | Nombre | Descripción                                                                                               |
|:--------|:-------|:----------------------------------------------------------------------------------------------------------|
| `5:0`   | -      | Reservado.                                                                                                |
| `15:6`  | `VALUE`| **El valor digital de 10 bits.** Aquí es donde va nuestro número de 0 a 1023. ¡Observa que está desplazado!  |
| `16`    | `BIAS` | Controla el balance entre velocidad y consumo. `0` = alta velocidad/alto consumo. `1` = baja velocidad/bajo consumo. Por defecto es `0`. |

**¡La Clave del `DACR`! El Desplazamiento de Bits**

El hardware fue diseñado de tal manera que el valor de 10 bits **no empieza en el bit 0**, sino en el **bit 6**.

```
Registro DACR (32 bits)
... [ Bit 16: BIAS ] [ Bits 15 a 6: VALUE (10 bits) ] [ Bits 5 a 0: Reservado ] ...
```

Esto significa que para escribir un valor, primero debemos **desplazarlo 6 bits a la izquierda**.

**Fórmula de Escritura:** `LPC_DAC->DACR = (valor_10_bits << 6);`

**Fórmula de Voltaje de Salida:**

El voltaje que aparecerá en el pin P0.26 se calcula con esta fórmula:
`Vout = VREF * (VALUE / 1024)`
Donde `VREF` suele ser 3.3V y `VALUE` es nuestro número de 0 a 1023.

---

### Código de Implementación: Funciones Reutilizables

Encapsulamos la lógica en funciones `DAC_Init()` y `DAC_Write()`.

```c
/**
 * @brief Inicializa el periférico DAC y configura el pin P0.26.
 */
void DAC_Init(void) {
    // 1. Configurar P0.26 para la función AOUT (10)
    LPC_PINCON->PINSEL1 &= ~(3 << 20); // Limpiar bits 21:20
    LPC_PINCON->PINSEL1 |= (2 << 20);  // Establecer función 10

    // 2. Inicializar el DAC con una salida de 0V.
    LPC_DAC->DACR = 0;
}

/**
 * @brief Escribe un valor digital de 10 bits en el DAC para generar un voltaje.
 * @param value: El valor a convertir (0 a 1023).
 */
void DAC_Write(uint16_t value) {
    // Asegurarse de que el valor no exceda los 10 bits (máscara 0x3FF)
    // y desplazarlo 6 bits a la izquierda para alinearlo con el campo VALUE.
    LPC_DAC->DACR = ((value & 0x3FF) << 6);
}
```

---

## Apéndice: Glosario de Registros y Drivers (API) **PRIMER PARCIAL**

Esta sección sirve como una referencia rápida y un resumen de los registros de hardware y las funciones de alto nivel (drivers) para los periféricos más comunes del LPC1769.

### 1. Configuración de Pines (PINSEL)

#### 1.1. Registros de Hardware

-   **`LPC_PINCON->PINSELx`**: **Función del Pin.**
    -   Controla la función principal de cada pin (GPIO, ADC, UART, etc.).
    -   **2 bits por pin**.
        -   `00`: Generalmente GPIO.
        -   `01`, `10`, `11`: Funciones alternativas.
-   **`LPC_PINCON->PINMODEx`**: **Modo de Resistencia Interna.**
    -   Configura las resistencias de pull-up o pull-down.
    -   **2 bits por pin**.
        -   `00`: Pull-up.
        -   `01`: Repeater (mantiene el último estado).
        -   `10`: Sin resistencia (Tristate/Flotante).
        -   `11`: Pull-down.
-   **`LPC_PINCON->PINMODE_ODx`**: **Modo de Drenador Abierto (Open-Drain).**
    -   Permite que el pin solo pueda ponerse en estado BAJO, quedando en alta impedancia en estado ALTO (requiere pull-up externo).
    -   **1 bit por pin**.
        -   `0`: Modo Normal (Push-Pull).
        -   `1`: Modo Open-Drain.

#### 1.2. Estructura y Driver de Configuración

-   **Estructura `PINSEL_CFG_Type`**: Contenedor para configurar un pin de forma completa.
    -   `Portnum`: Puerto del pin (0-4).
    -   `Pinnum`: Número del pin (0-31).
    -   `Funcnum`: Número de función (0-3).
    -   `Pinmode`: Modo de resistencia (`PULLUP`, `TRISTATE`, `PULLDOWN`).
    -   `OpenDrain`: Modo de salida (`NORMAL`, `OPENDRAIN`).
-   **Función `PINSEL_ConfigPin(PinCfg)`**: Aplica la configuración definida en la estructura a un pin.

### 2. Entrada/Salida de Propósito General (GPIO)

#### 2.1. Registros de Hardware

-   **`LPC_GPIOx->FIODIR`**: **Dirección del Pin.**
    -   Define si un pin GPIO es entrada o salida.
    -   **1 bit por pin**. `0` = Entrada, `1` = Salida.
-   **`LPC_GPIOx->FIOMASK`**: **Máscara de Acceso.**
    -   Permite "proteger" pines para que no sean afectados por escrituras en `FIOSET`, `FIOCLR` o `FIOPIN`.
    -   **1 bit por pin**. `0` = Acceso habilitado, `1` = Acceso deshabilitado (enmascarado).
-   **`LPC_GPIOx->FIOPIN`**: **Valor del Pin.**
    -   **Lectura:** Devuelve el estado lógico actual de todos los pines del puerto.
    -   **Escritura:** Establece el estado de todos los pines no enmascarados simultáneamente.
-   **`LPC_GPIOx->FIOSET`**: **Poner en ALTO.**
    -   Pone en `1` los pines correspondientes a los bits que se escriben como `1`.
-   **`LPC_GPIOx->FIOCLR`**: **Poner en BAJO.**
    -   Pone en `0` los pines correspondientes a los bits que se escriben como `1`.

#### 2.2. Registros de Interrupción GPIO

-   **`LPC_GPIOINT->IOxIntEnR`**: Habilita la interrupción por **flanco de subida** en el puerto `x`.
-   **`LPC_GPIOINT->IOxIntEnF`**: Habilita la interrupción por **flanco de bajada** en el puerto `x`.
-   **`LPC_GPIOINT->IntStatus`**: Estado general de interrupciones. Indica si hay alguna interrupción pendiente en el Puerto 0 (`P0Int`) o Puerto 2 (`P2Int`).
-   **`LPC_GPIOINT->IOxIntStatR` / `IOxIntStatF`**: Estado detallado. Indica qué pin específico del puerto `x` generó una interrupción por flanco de subida/bajada.
-   **`LPC_GPIOINT->IOxIntClr`**: Limpia la bandera de interrupción de un pin específico.

#### 2.3. Funciones de Driver (API)

-   **`GPIO_SetDir(port, pins, dir)`**: Configura la dirección (Entrada/Salida) para un conjunto de `pins` en un `port`.
-   **`GPIO_SetValue(port, pins)`**: Pone en ALTO los `pins` especificados.
-   **`GPIO_ClearValue(port, pins)`**: Pone en BAJO los `pins` especificados.
-   **`GPIO_ReadValue(port)`**: Lee el estado de todos los pines de un `port`.

### 3. Interrupciones Externas (EINT)

#### 3.1. Registros de Hardware

-   **`LPC_SC->EXTINT`**: **Bandera de Interrupción.**
    -   Se pone en `1` cuando ocurre un evento de interrupción externa. Se limpia escribiendo un `1`.
-   **`LPC_SC->EXTMODE`**: **Modo de Detección.**
    -   Configura si la interrupción se dispara por **nivel** (`0`) o por **flanco** (`1`).
-   **`LPC_SC->EXTPOLAR`**: **Polaridad.**
    -   Si es modo nivel, define si es activo en BAJO (`0`) o ALTO (`1`).
    -   Si es modo flanco, define si es por bajada (`0`) o subida (`1`).

#### 3.2. Funciones de Driver (API)

-   **`EXTI_Init()`**: Inicializa las interrupciones externas a su estado por defecto.
-   **`EXTI_Config(EXTICfg)`**: Configura una línea de interrupción usando una estructura.
-   **`NVIC_EnableIRQ(EINTx_IRQn)`**: Habilita la interrupción en el controlador de interrupciones (NVIC).
-   **`EINTx_IRQHandler()`**: Nombre de la función handler que se ejecutará cuando ocurra la interrupción.

### 4. Timer SysTick

#### 4.1. Registros de Hardware (Accesibles directamente)

-   **`SysTick->CTRL`**: **Registro de Control y Estado.**
    -   `ENABLE` (bit 0): Habilita/deshabilita el contador.
    -   `TICKINT` (bit 1): Habilita/deshabilita la interrupción.
    -   `CLKSOURCE` (bit 2): Selecciona la fuente de reloj (interna o externa).
    -   `COUNTFLAG` (bit 16): Bandera que indica que el contador ha llegado a cero.
-   **`SysTick->LOAD`**: **Valor de Recarga.**
    -   Contiene el valor desde el cual el contador comenzará a descender.
-   **`SysTick->VAL`**: **Valor Actual.**
    -   Contiene el valor actual del contador. Escribir cualquier valor lo resetea.
-   **`SysTick->CALIB`**: **Calibración.**
    -   Contiene un valor de fábrica para generar un intervalo de 10ms si el CCLK es 100MHz.

#### 4.2. Funciones de Driver (API)

-   **`SysTick_Config(ticks)`**: Función principal que configura el SysTick para generar interrupciones periódicas con un valor de recarga de `ticks`.
-   **`SysTick_Handler()`**: Nombre de la función handler de la interrupción.
-   **`SYSTICK_Cmd(NewState)`**: Habilita o deshabilita el contador.
-   **`SYSTICK_IntCmd(NewState)`**: Habilita o deshabilita la generación de interrupciones.

### 5. Convertidor Analógico-Digital (ADC)

#### 5.1. Registros de Hardware

-   **`LPC_SC->PCONP`**: (Bit 12 `PCADC`) Habilita la energía del periférico.
-   **`LPC_SC->PCLKSEL0`**: (Bits 25:24 `PCLK_ADC`) Configura el reloj del periférico.
-   **`LPC_ADC->ADCR`**: **Registro de Control.**
    -   `SEL` (bits 7:0): Selecciona qué canal(es) convertir.
    -   `CLKDIV` (bits 15:8): Divide el `PCLK_ADC` para obtener el reloj de conversión (< 13MHz).
    -   `PDN` (bit 21): Pone el ADC en modo operacional (`1`) o power-down (`0`).
    -   `START` (bits 26:24): Inicia la conversión (ej. `001` = Iniciar ahora).
-   **`LPC_ADC->ADGDR`**: **Registro de Datos Global.**
    -   `RESULT` (bits 15:4): Contiene el resultado de 12 bits de la última conversión.
    -   `DONE` (bit 31): Bandera que indica que la conversión ha finalizado.
-   **`LPC_ADC->ADDRx`**: Registro de datos específico para el canal `x`.

### 6. Convertidor Digital-Analógico (DAC)

#### 6.1. Registros de Hardware

-   **`LPC_SC->PCLKSEL0`**: (Bits 23:22 `PCLK_DAC`) Configura el reloj (usado por el timer interno para DMA).
-   **`LPC_DAC->DACR`**: **Registro del Convertidor.**
    -   `VALUE` (bits 15:6): Valor de 10 bits a convertir. **¡Ojo al desplazamiento!**
    -   `BIAS` (bit 16): Configura el balance velocidad/consumo.
-   **`LPC_DAC->DACCTRL`**: **Registro de Control.**
    -   Controla las operaciones con DMA y el timer interno.
-   **`LPC_DAC->DACCNTVAL`**: Valor de recarga para el timer interno del DAC.

---

## Acceso Directo a Memoria (DMA)

### 1. ¿Qué es y por qué es tan importante?

Imagina que tu CPU es un gerente muy ocupado. Tareas como mover datos de un lugar a otro (por ejemplo, desde un buffer de memoria al registro de transmisión de un UART) son repetitivas y consumen mucho tiempo. El **Acceso Directo a Memoria (DMA)** es como contratar a un asistente ultra-eficiente para que se encargue de todas estas mudanzas de datos.

El controlador DMA es un periférico especializado que puede transferir datos entre la memoria y los periféricos **sin ninguna intervención de la CPU**.

**Beneficios Clave:**

1.  **Libera a la CPU:** Mientras el DMA mueve datos en segundo plano, la CPU queda libre para ejecutar lógica de programa más compleja.
2.  **Alta Velocidad:** El DMA está optimizado para transferencias de datos, haciéndolas mucho más rápidas y eficientes que si la CPU las hiciera instrucción por instrucción.
3.  **Ahorro de Energía:** La CPU puede entrar en modos de bajo consumo mientras el DMA trabaja, ya que no necesita estar activa.
4.  **Multitarea Real:** Permite que las operaciones de E/S (como recibir datos de un ADC o enviar por un puerto serie) ocurran simultáneamente con el procesamiento principal, creando un sistema mucho más responsivo.

El controlador del LPC1769 se llama **GPDMA** (General-Purpose DMA) y cuenta con **8 canales**, lo que significa que puede gestionar hasta 8 transferencias de datos independientes simultáneamente.

### 2. El Proceso de una Transferencia DMA en 4 Pasos

Configurar una transferencia DMA implica decirle al "asistente" exactamente qué mover, desde dónde, hacia dónde y cuándo.

1.  **Paso 1: Encender el Periférico (PCONP)**
    -   Registro: `LPC_SC->PCONP`
    -   Propósito: Suministrar energía y reloj al controlador GPDMA. Sin esto, todo el bloque está inactivo.
2.  **Paso 2: Habilitar y Configurar el Controlador (DMACConfig)**
    -   Registro: `LPC_GPDMA->DMACConfig`
    -   Propósito: Activar el motor DMA principal. Es el interruptor general del controlador.
3.  **Paso 3: Configurar un Canal DMA**
    -   Registros: `LPC_GPDMACHx->...` (donde x es el canal 0-7)
    -   Propósito: Aquí se define cada detalle de la transferencia:
        -   **Dirección de Origen:** `DMACCSrcAddr`
        -   **Dirección de Destino:** `DMACCDestAddr`
        -   **Información de Control:** `DMACCControl` (Tamaño de la transferencia, ancho de datos, si las direcciones se incrementan, etc.).
        -   **Próximo Enlace (Opcional):** `DMACCLLI` (Para transferencias encadenadas).
4.  **Paso 4: Habilitar el Canal (DMACCxConfig)**
    -   Registro: `LPC_GPDMACHx->DMACCConfig`
    -   Propósito: Dar la orden final para que el canal configurado comience a esperar su "disparador" (trigger) y comience la transferencia.

---

### Análisis Detallado: Los Registros del DMA en Acción

#### Paso 1: Encender el Periférico (`PCONP`)

Como con la mayoría de los periféricos, el primer paso es habilitar su reloj en el registro de control de energía. El bit para el GPDMA es el **bit 29**.

```c
// Habilita la energía para el periférico GPDMA (PCGPDMA = bit 29)
LPC_SC->PCONP |= (1 << 29);
```

#### Paso 2: Configurar el Controlador (`DMACConfig`)

Este registro es el interruptor maestro.
-   Bit 0 (`E`): **Enable**. Ponerlo en `1` enciende el controlador DMA.
-   Bit 1 (`M`): **Endianness Mode**. Controla el modo Endian para las transferencias. Por defecto es Little-Endian (`0`), que es lo que usaremos.

```c
// Habilita el controlador GPDMA
LPC_GPDMA->DMACConfig = 1;
```
También es importante limpiar cualquier interrupción pendiente que pudiera haber quedado de un estado anterior.
```c
// Limpia las banderas de interrupción de Terminal Count y Error
LPC_GPDMA->DMACIntTCClear = 0xFF;
LPC_GPDMA->DMACIntErrClear = 0xFF;
```

#### Paso 3: Configurar un Canal Específico

Aquí es donde reside la mayor parte del trabajo. Cada uno de los 8 canales tiene su propio conjunto de registros. Supongamos que configuramos el **Canal 0**.

##### a. Direcciones de Origen y Destino

Son registros de 32 bits que simplemente apuntan a las direcciones de memoria de inicio.

```c
// Origen: La dirección de un array en memoria
LPC_GPDMACH0->DMACCSrcAddr = (uint32_t)miArrayFuente;

// Destino: La dirección del registro de datos de un periférico (ej. UART)
LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&(LPC_UART0->THR);
```

##### b. Registro de Control (`DMACCxControl`)

Este registro define **cómo** se realizará la transferencia.

| Bits    | Nombre          | Descripción                                                                                                  |
|:--------|:----------------|:-------------------------------------------------------------------------------------------------------------|
| `11:0`  | `TransferSize`  | **Cuántos datos mover.** Número de transferencias (no bytes). Si el ancho es byte, es el número de bytes.      |
| `20:18` | `SWidth`        | **Ancho del Origen.** `000`=Byte, `001`=Halfword(16b), `010`=Word(32b).                                        |
| `23:21` | `DWidth`        | **Ancho del Destino.** Igual que `SWidth`. El hardware adapta los anchos si son diferentes.                   |
| `26`    | `SI`            | **Incremento de Origen.** `1`=La dirección de origen se incrementa después de cada transferencia (para arrays). `0`=No se incrementa (para periféricos). |
| `27`    | `DI`            | **Incremento de Destino.** `1`=La dirección de destino se incrementa. `0`=No se incrementa.                    |
| `31`    | `I`             | **Interrupción.** `1`=Genera una interrupción cuando la transferencia termina (`TransferSize` llega a 0).      |

**Ejemplo de configuración para enviar un array de 50 bytes a un UART:**
```c
uint32_t dma_control_valor = 0;
dma_control_valor |= (50 << 0);       // TransferSize = 50
dma_control_valor |= (0 << 18);       // SWidth = Byte
dma_control_valor |= (0 << 21);       // DWidth = Byte
dma_control_valor |= (1 << 26);       // SI = Incrementar origen (recorremos el array)
dma_control_valor |= (0 << 27);       // DI = No incrementar destino (UART THR es una dirección fija)
dma_control_valor |= (1 << 31);       // I = Habilitar interrupción al terminar

LPC_GPDMACH0->DMACCControl = dma_control_valor;
```

#### Paso 4: Habilitar y Vincular el Canal (`DMACCxConfig`)

Este es el último paso, donde vinculamos el canal a un periférico y lo activamos.

| Bits    | Nombre            | Descripción                                                                                                  |
|:--------|:------------------|:-------------------------------------------------------------------------------------------------------------|
| `0`     | `E`               | **Channel Enable.** `1` para activar el canal. El canal queda a la espera de un request.                        |
| `5:1`   | `SrcPeripheral`   | **ID del Periférico de Origen.** Si la transferencia es Periférico -> Memoria, aquí se indica qué periférico. (Ej: ADC es 4). |
| `10:6`  | `DestPeripheral`  | **ID del Periférico de Destino.** Si la transferencia es Memoria -> Periférico, aquí se indica qué periférico. (Ej: UART0 Tx es 2). |
| `13:11` | `TransferType`    | **Tipo de Transferencia.** `001`=Memoria a Periférico, `010`=Periférico a Memoria, `000`=Memoria a Memoria.    |
| `15`    | `IE`              | **Interrupt Error Mask.** `0` para enmascarar (ignorar) interrupciones de error.                             |
| `16`    | `ITC`             | **Terminal Count Interrupt Mask.** `0` para enmascarar (ignorar) la interrupción de fin de transferencia.      |

**Ejemplo de configuración para nuestro caso de UART:**
```c
uint32_t dma_config_valor = 0;
dma_config_valor |= (1 << 0);        // E = Habilitar canal
dma_config_valor |= (2 << 6);        // DestPeripheral = 2 (UART0 Tx)
dma_config_valor |= (1 << 11);       // TransferType = 1 (Memoria a Periférico)
// No enmascaramos interrupciones, así que dejamos IE e ITC en 0.

LPC_GPDMACH0->DMACCConfig = dma_config_valor;
```
¡Listo! Una vez que el registro `U0THR` del UART esté vacío, enviará una solicitud al DMA, y el Canal 0 automáticamente comenzará a transferir los 50 bytes desde el array, uno por uno, sin que la CPU tenga que hacer nada más.

---

### Apéndice: Glosario de Registros y Drivers (API)

### 7. Acceso Directo a Memoria (GPDMA)

#### 7.1. Registros Globales del Controlador

-   **`LPC_SC->PCONP`**: (Bit 29 `PCGPDMA`) Habilita la energía del periférico GPDMA.
-   **`LPC_GPDMA->DMACConfig`**: **Registro de Configuración Global.**
    -   `E` (bit 0): Habilita el controlador DMA.
-   **`LPC_GPDMA->DMACIntStat`**: **Estado de Interrupción.** Indica qué canales tienen una interrupción activa.
-   **`LPC_GPDMA->DMACIntTCStat`**: **Estado de Interrupción por Fin de Conteo (Terminal Count).** Indica qué canales han completado su transferencia.
-   **`LPC_GPDMA->DMACIntTCClear`**: **Limpieza de Interrupción TC.** Escribir un `1` en un bit limpia la bandera de interrupción TC para el canal correspondiente.
-   **`LPC_GPDMA->DMACIntErrStat` / `DMACIntErrClear`**: Igual que los anteriores, pero para condiciones de error.
-   **`LPC_GPDMA->DMACEnbldChns`**: **Canales Habilitados.** Registro de solo lectura que muestra qué canales están actualmente habilitados.

#### 7.2. Registros por Canal (`LPC_GPDMACHx->...`)

-   **`DMACCSrcAddr`**: **Dirección de Origen.** Puntero de 32 bits a la fuente de los datos.
-   **`DMACCDestAddr`**: **Dirección de Destino.** Puntero de 32 bits al destino de los datos.
-   **`DMACCLLI`**: **Elemento de Lista Enlazada (Linked List Item).** Puntero a la siguiente estructura de transferencia para operaciones de scatter-gather. Si es `0`, es la última transferencia.
-   **`DMACCControl`**: **Control de Transferencia.**
    -   `TransferSize` (bits 11:0): Número de transferencias a realizar.
    -   `SWidth`/`DWidth`: Ancho de los datos en origen y destino (Byte, Halfword, Word).
    -   `SI`/`DI`: Habilita (`1`) o deshabilita (`0`) el auto-incremento de las direcciones de origen/destino.
    -   `I` (bit 31): Habilita la interrupción por fin de conteo (TC).
-   **`DMACCConfig`**: **Configuración del Canal.**
    -   `E` (bit 0): Habilita el canal.
    -   `SrcPeripheral`/`DestPeripheral`: Asocia el canal con la línea de solicitud de un periférico.
    -   `TransferType`: Define el flujo de la transferencia (M2P, P2M, M2M).
    -   `IE`/`ITC`: Máscaras para las interrupciones de error y de fin de conteo.

---

## Usando la Abstracción: CMSIS y los Drivers de Periféricos

Hasta ahora, hemos interactuado directamente con el hardware a través de sus registros (`LPC_GPIO0->FIODIR = ...`). Esto es potente y educativo, pero puede ser propenso a errores, difícil de leer y nada portable. Para solucionar esto, existe el **CMSIS**.

### 1. ¿Qué es CMSIS?

**CMSIS (Cortex Microcontroller Software Interface Standard)** es un estándar creado por ARM para facilitar el desarrollo de software para todos los microcontroladores que usan un núcleo Cortex-M. Es como una "capa de traducción" entre tu código y el hardware.

Se divide en dos partes principales que nos interesan:

1.  **CMSIS-Core:** Es la parte **universal** del estándar. Proporciona acceso estandarizado a los registros del **núcleo del procesador**. Esto incluye:
    -   El **SysTick Timer**.
    -   El **NVIC** (Nested Vectored Interrupt Controller).
    -   Registros especiales del CPU.
    El código que usa CMSIS-Core para el SysTick en un LPC1769 funcionaría casi sin cambios en un microcontrolador STM32.

2.  **CMSIS-Driver (API de Periféricos):** Esta es la parte **específica del fabricante** (NXP en nuestro caso). NXP proporciona una librería de drivers (un conjunto de archivos `.c` y `.h`) que implementan una API (Application Programming Interface) fácil de usar para controlar **sus periféricos específicos**:
    -   GPIO
    -   ADC y DAC
    -   UART, I2C, SPI
    -   Timers, PWM, etc.

**La Gran Idea: Abstracción**

En lugar de recordar que el bit 29 de `PCONP` enciende el DMA, simplemente llamarás a una función como `GPDMA_Init()`. La función se encarga de la manipulación de bits por ti.

| Característica | Acceso a Registros Directos | Usando Drivers CMSIS |
| :--- | :--- | :--- |
| **Legibilidad** | Baja (`LPC_SC->PCONP |= (1<<12)`) | Alta (`ADC_Init(LPC_ADC, 200000)`) |
| **Mantenimiento** | Difícil. Un error en un bit es difícil de encontrar. | Fácil. La lógica está encapsulada en funciones. |
| **Portabilidad** | Nula. Los registros son específicos del chip. | Alta (dentro de la familia de chips del fabricante). |
| **Curva de Aprendizaje**| Requiere leer el manual de usuario a fondo. | Requiere leer la documentación de la API del driver. |

---

### 2. El Nuevo Flujo de Trabajo con Drivers

El proceso de desarrollo cambia ligeramente. Ahora, en lugar de manipular bits, seguirás un patrón de uso de la API:

1.  **Incluir el Header:** Añade el archivo de cabecera del driver que necesitas (ej. `#include "lpc17xx_gpio.h"`).
2.  **Configurar Estructuras (si es necesario):** Muchos drivers usan estructuras para agrupar toda la configuración de un periférico. Llenas la estructura con los valores deseados.
3.  **Llamar a la Función de Inicialización:** Pasas la estructura a una función `_Init()` (ej. `PINSEL_ConfigPin(&pin_config)`).
4.  **Usar Funciones de Operación:** Utilizas funciones de alto nivel para controlar el periférico (ej. `GPIO_SetValue()`, `ADC_GetData()`).

### 3. Guía Práctica: De Registros a Drivers

Veamos cómo se transforman las tareas que ya conoces.

#### Ejemplo 1: Configurar un Pin GPIO (LED en P0.22)

**El Modo Antiguo (Registros):**
```c
// Paso 1: Configurar P0.22 como GPIO (función 00 en PINSEL1[13:12])
LPC_PINCON->PINSEL1 &= ~(3 << 12);

// Paso 2: Configurar P0.22 como salida (bit 22 en FIODIR0)
LPC_GPIO0->FIODIR |= (1 << 22);

// Operación: Encender el LED
LPC_GPIO0->FIOSET = (1 << 22);
```

**El Nuevo Modo (Drivers CMSIS):**
```c
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

// Paso 1: Configurar el pin usando una estructura y una función
PINSEL_CFG_Type pin_config;

pin_config.Portnum   = PINSEL_PORT_0;
pin_config.Pinnum    = PINSEL_PIN_22;
pin_config.Funcnum   = PINSEL_FUNC_0; // 0 para GPIO
pin_config.Pinmode   = PINSEL_PINMODE_PULLUP; // Opcional
pin_config.OpenDrain = PINSEL_PINMODE_NORMAL; // Opcional

PINSEL_ConfigPin(&pin_config);

// Paso 2: Configurar la dirección
GPIO_SetDir(0, (1 << 22), 1); // Puerto 0, Pin 22, Dirección 1 (Salida)

// Operación: Encender el LED
GPIO_SetValue(0, (1 << 22));
```**Análisis:** El nuevo código es más largo, pero es auto-documentado. `PINSEL_FUNC_0` es mucho más claro que `~(3 << 12)`. `GPIO_SetDir` con el parámetro `1` es explícito sobre que es una salida.

---

#### Ejemplo 2: Usar el SysTick para un Delay

Aquí vemos el poder de **CMSIS-Core**.

**El Modo Antiguo (Registros):**```c
void delay_ms_systick(uint32_t milisegundos) {
    uint32_t ticks = (SystemCoreClock / 1000) * milisegundos;
    SysTick->LOAD = ticks - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = (1 << 2) | (1 << 0);
    while (!(SysTick->CTRL & (1 << 16)));
    SysTick->CTRL = 0;
}
```

**El Nuevo Modo (CMSIS-Core):**
```c
// Esta función ya viene definida en los archivos de CMSIS-Core!
#include "LPC17xx.h" // SystemCoreClock y SysTick_Config están aquí

void delay_ms_systick(uint32_t milisegundos) {
    // Configura Y HABILITA el SysTick para que interrumpa cada 1ms
    // Esta función hace los 3 pasos: LOAD, VAL y CTRL.
    SysTick_Config(SystemCoreClock / 1000);

    // Bucle de espera
    // (Este ejemplo es un poco más complejo, pero la inicialización es la clave)
    // Para un delay simple, la lógica de interrupción es más común con CMSIS.
    // Lo más importante: ¡La inicialización se reduce a UNA línea!
}

// El uso más común con CMSIS es el basado en interrupciones:
volatile uint32_t ms_ticks = 0;
void SysTick_Handler(void) {
    ms_ticks++;
}

int main(void) {
    // Configura el SysTick para que interrumpa cada 1ms
    SysTick_Config(SystemCoreClock / 1000); 

    while(1) { /* ... */ }
}
```
**Análisis:** La función `SysTick_Config()` encapsula toda la configuración del SysTick para interrupciones periódicas. Simplifica enormemente el inicio, haciendo el código más limpio y menos propenso a errores de configuración.

---

#### Ejemplo 3: Leer el ADC

Aquí es donde los drivers realmente brillan, al manejar periféricos más complejos.

**El Modo Antiguo (Registros):**
```c
// Inicialización...
LPC_SC->PCONP |= (1 << 12);
LPC_PINCON->PINSEL1 |= (1 << 14); // ... etc para PINSEL y PINMODE
LPC_ADC->AD0CR = (1 << 21) | (1 << 8);

// Lectura...
LPC_ADC->AD0CR |= (1 << 0) | (1 << 24); // Seleccionar canal 0 e iniciar
while (!(LPC_ADC->AD0GDR & (1U << 31))); // Esperar bandera DONE
uint16_t valor = (LPC_ADC->AD0GDR >> 4) & 0xFFF; // Extraer resultado
```

**El Nuevo Modo (Drivers CMSIS):**
```c
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"

// Inicialización...
// (Configurar el pin P0.23 con PINSEL_ConfigPin como antes, función 1)
ADC_Init(LPC_ADC, 200000); // Inicializa el ADC con una tasa de muestreo de 200KHz
ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, DISABLE); // Deshabilitar interrupciones
ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE); // Habilitar el canal 0

// Lectura...
ADC_StartCmd(LPC_ADC, ADC_START_NOW); // Iniciar conversión

// Esperar a que la conversión del canal 0 termine
while (ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE) == RESET);

uint16_t valor = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
```
**Análisis:** ¡La diferencia es abismal!
-   `ADC_Init` se encarga de `PCONP` y `ADCR`.
-   Las funciones `_Cmd` son claras para iniciar o habilitar partes del periférico.
-   `ADC_ChannelGetStatus` es una forma explícita de comprobar la bandera `DONE`.
-   `ADC_ChannelGetData` **hace el trabajo sucio por ti**: lee el registro, extrae el valor de 12 bits y te lo devuelve limpio. Ya no tienes que hacer desplazamientos ni aplicar máscaras.

---

## Guía de Programación con Drivers CMSIS para LPC1769

El cambio de manipular registros directamente a usar los drivers es un salto hacia un código más robusto, legible y mantenible. A continuación, se detalla cómo configurar y usar los periféricos esenciales con esta nueva metodología.

### 1. Gestión de Clocks: El Corazón del Sistema

Antes de inicializar cualquier periférico, debemos asegurarnos de que el sistema y los periféricos mismos estén funcionando a la velocidad de reloj que esperamos.

#### El Modo Antiguo (Registros)

Configurar el PLL0, los divisores CCLK y PCLK implicaba una secuencia compleja de escrituras en registros como `LPC_SC->CLKSRCSEL`, `LPC_SC->PLL0CFG`, `LPC_SC->CCLKCFG`, y `LPC_SC->PCLKSELx`. Era fácil cometer un error.

#### El Nuevo Modo (Drivers CMSIS)

El sistema de drivers simplifica esto enormemente.

1.  **`SystemInit()`**: Esta función es tu punto de partida. Se llama automáticamente al inicio de tu programa (antes de `main()`) desde el archivo `startup_LPC17xx.s`. Su propósito es configurar el microcontrolador a un estado operativo por defecto, incluyendo la configuración del reloj principal (CCLK) a una velocidad predeterminada (usualmente 100 MHz usando el PLL0).
2.  **`SystemCoreClockUpdate()`**: Después de `SystemInit()` o de cualquier cambio en la configuración del reloj, debes llamar a esta función. Lee los registros de configuración del reloj y actualiza la variable global `SystemCoreClock`, que es crucial para que otros drivers (como el SysTick) calculen los retardos correctamente.
3.  **`CLKPWR_SetPCLKDiv(peripheral, divisor)`**: Esta es la función clave para controlar la velocidad de cada periférico individualmente.
    -   `peripheral`: Una macro que identifica al periférico (ej. `CLKPWR_PCLKSEL_ADC`).
    -   `divisor`: El factor por el cual se dividirá el CCLK (`CLKPWR_PCLKDIV_1`, `_2`, `_4`, `_8`).

**Código de Ejemplo: Configuración de Clocks**
```c
#include "lpc17xx_clkpwr.h"

int main(void) {
    // 1. SystemInit() ya fue llamado automáticamente. 
    //    El CCLK está probablemente a 100 MHz.
    SystemCoreClockUpdate(); // Actualiza la variable global SystemCoreClock

    // Por defecto, muchos periféricos corren a CCLK/4.
    // Vamos a configurar el reloj del ADC para que corra a CCLK/8.
    // Si CCLK = 100MHz, PCLK_ADC será 12.5 MHz.
    CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_ADC, CLKPWR_PCLKDIV_8);

    // ... inicializar el resto de los periféricos ...

    while(1) { /* ... */ }
}
```
**Análisis:** En lugar de manipular múltiples registros con valores crípticos, ahora usamos funciones con nombres descriptivos y macros. `CLKPWR_PCLKDIV_8` es mucho más claro que escribir `0b11` en una posición de bit específica.

---

### 2. Configuración de Pines (PINSEL / GPIO)

#### El Modo Antiguo (Registros)
```c
LPC_PINCON->PINSEL1 &= ~(3 << 12); // P0.22 como GPIO
LPC_GPIO0->FIODIR |= (1 << 22);   // P0.22 como Salida
LPC_GPIO0->FIOSET = (1 << 22);    // Poner en ALTO
```

#### El Nuevo Modo (Drivers CMSIS)
```c
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

// 1. Configurar la función del pin
PINSEL_CFG_Type PinCfg;
PinCfg.Portnum = PINSEL_PORT_0;
PinCfg.Pinnum  = PINSEL_PIN_22;
PinCfg.Funcnum = PINSEL_FUNC_0; // 0 para GPIO
PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE; // Sin pull-up/down

PINSEL_ConfigPin(&PinCfg);

// 2. Configurar la dirección del pin
GPIO_SetDir(0, (1 << 22), 1); // Puerto 0, máscara para Pin 22, Dirección 1 (Salida)

// 3. Operar el pin
GPIO_SetValue(0, (1 << 22));   // Poner en ALTO
// GPIO_ClearValue(0, (1 << 22)); // Poner en BAJO
```
**Análisis:** La estructura `PINSEL_CFG_Type` agrupa toda la configuración de un pin en un solo lugar, haciendo el código más ordenado. Las funciones `GPIO_SetDir`, `GPIO_SetValue` y `GPIO_ClearValue` son explícitas sobre su propósito.

---

### 3. Timer SysTick

#### El Modo Antiguo (Registros)
```c
SysTick->LOAD = (SystemCoreClock / 1000) - 1;
SysTick->VAL = 0;
SysTick->CTRL = (1 << 2) | (1 << 1) | (1 << 0);
// ... y definir SysTick_Handler()
```

#### El Nuevo Modo (CMSIS-Core)
El SysTick es parte del núcleo Cortex-M, por lo que su control está estandarizado en CMSIS-Core.

```c
#include "LPC17xx.h" // Incluye todo lo necesario para CMSIS-Core

volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

int main(void) {
    SystemCoreClockUpdate();

    // Configura Y HABILITA el SysTick para que interrumpa cada 1ms.
    // Esta función hace todo el trabajo de LOAD, VAL y CTRL por ti.
    if (SysTick_Config(SystemCoreClock / 1000)) {
        // Hubo un error en la configuración
        while (1);
    }

    while(1) {
        if (ms_ticks > 1000) {
            // Ha pasado 1 segundo...
            ms_ticks = 0;
        }
    }
}
```
**Análisis:** La función `SysTick_Config()` es la joya de la corona. Reduce una configuración de 3 registros propensa a errores a una sola llamada de función segura (incluso devuelve un error si el valor de recarga es inválido).

---

### 4. Convertidor Analógico-Digital (ADC)

#### El Modo Antiguo (Registros)
Una secuencia compleja de escrituras en `PCONP`, `PINSEL`, `AD0CR` y `AD0GDR`, incluyendo desplazamientos de bits y máscaras para leer el resultado.

#### El Nuevo Modo (Drivers CMSIS)
```c
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"

void ADC_Configuration(void) {
    // Configurar P0.23 (AD0.0) con PINSEL_ConfigPin, Funcnum = 1

    // Inicializar el ADC. El driver se encarga de PCONP y la configuración base.
    // 200000 Hz es la tasa de muestreo máxima recomendada.
    ADC_Init(LPC_ADC, 200000);

    // Habilitar el canal 0 para la conversión
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
}

uint16_t ADC_ReadValue(void) {
    // Iniciar la conversión ahora, en modo software
    ADC_StartCmd(LPC_ADC, ADC_START_NOW);

    // Esperar hasta que la conversión en el canal 0 termine
    while (ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE) == RESET);

    // Leer el dato. La función se encarga de extraer y devolver el valor limpio.
    return ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
}
```
**Análisis:** El driver abstrae toda la complejidad. Ya no necesitas recordar en qué bits está el resultado (`15:4`) ni la bandera `DONE` (bit 31). Funciones como `ADC_ChannelGetData` hacen el código mucho más limpio y seguro.

---

### 5. Convertidor Digital-Analógico (DAC)

#### El Modo Antiguo (Registros)
```c
// Configurar P0.26 para la función 2 (10)
LPC_PINCON->PINSEL1 &= ~(3 << 20);
LPC_PINCON->PINSEL1 |= (2 << 20);
// Escribir valor, recordando el desplazamiento de 6 bits
LPC_DAC->DACR = (valor_10_bits << 6);
```

#### El Nuevo Modo (Drivers CMSIS)
```c
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"

void DAC_Configuration(void) {
    // Configurar P0.26 con PINSEL_ConfigPin, Funcnum = 2

    // Inicializar el DAC
    DAC_Init(LPC_DAC);
}

void DAC_SetValue(uint16_t valor) {
    // Actualizar el valor de salida. 
    // La función se encarga del desplazamiento de 6 bits por ti.
    DAC_UpdateValue(LPC_DAC, valor);
}
```
**Análisis:** El driver del DAC es simple pero muy útil. `DAC_UpdateValue` elimina la necesidad de recordar el desplazamiento de 6 bits, una fuente común de errores.

---

### 6. Acceso Directo a Memoria (DMA)

#### El Modo Antiguo (Registros)
Configuración muy detallada y propensa a errores de `PCONP`, `DMACConfig`, y los múltiples registros de cada canal (`DMACCSrcAddr`, `DMACCDestAddr`, `DMACCControl`, `DMACCConfig`).

#### El Nuevo Modo (Drivers CMSIS)
El driver del DMA es uno de los que más simplifica el proceso, usando una estructura para configurar cada canal.

```c
#include "lpc17xx_gpdma.h"

// Ejemplo: Transferir un array de memoria a un UART usando DMA
uint8_t mensaje_uart[] = "Hola DMA!";

void DMA_Configuration(void) {
    GPDMA_Channel_CFG_Type GPDMACfg;

    // Inicializar el GPDMA (habilita PCONP y el controlador)
    GPDMA_Init();

    // Configurar el Canal 0 del DMA
    GPDMACfg.ChannelNum = 0;
    // Origen: Memoria
    GPDMACfg.SrcMemAddr = (uint32_t)mensaje_uart;
    // Destino: Periférico UART0 Tx (ID = 2)
    GPDMACfg.DstConn = GPDMA_CONN_UART0_Tx;
    // Tamaño de la transferencia
    GPDMACfg.TransferSize = sizeof(mensaje_uart);
    // Ancho: Byte
    GPDMACfg.TransferWidth = GPDMA_WIDTH_BYTE;
    // Tipo: Memoria a Periférico
    GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
    // No encadenar transferencias
    GPDMACfg.DMALI = 0;

    // Aplicar la configuración al canal 0
    GPDMA_Setup(&GPDMACfg);
}

int main(void) {
    // ... inicializar UART ...
    DMA_Configuration();
    
    // Habilitar el canal 0 para que comience la transferencia cuando el UART lo pida
    GPDMA_ChannelCmd(0, ENABLE);

    while(1) { /* La transferencia ocurre en segundo plano */ }
}
```
**Análisis:** La estructura `GPDMA_Channel_CFG_Type` es la clave. Organiza toda la configuración del canal de una manera lógica y legible. Las macros como `GPDMA_CONN_UART0_Tx` y `GPDMA_TRANSFERTYPE_M2P` eliminan la necesidad de buscar "números mágicos" en el manual. El flujo "Configurar -> Setup -> Habilitar" es claro y consistente.

---

### 7. Manejo de Interrupciones de Periféricos con Drivers CMSIS

Hasta ahora, hemos usado el "polling" (sondeo) para esperar a que un periférico termine una tarea (ej. `while (flag == 0);`). Este método es simple, pero **muy ineficiente**, ya que la CPU se queda atrapada en un bucle sin hacer nada útil.

Las **interrupciones** resuelven este problema. Son un mecanismo de hardware que permite a un periférico "avisar" a la CPU cuando un evento ha ocurrido. La CPU detiene momentáneamente su tarea principal, atiende la solicitud del periférico en una función especial llamada **Handler**, y luego regresa a su trabajo exactamente donde lo dejó.

#### El Flujo de Trabajo para Configurar una Interrupción

Usando los drivers CMSIS, el proceso es estándar y consistente para casi todos los periféricos:

1.  **Paso 1: Configurar el Periférico:** Inicializa el periférico como lo harías normalmente (ej. `ADC_Init()`, `GPIO_SetDir()`).
2.  **Paso 2: Configurar la Lógica de Interrupción del Periférico:** Dile al periférico bajo qué condiciones debe generar una señal de interrupción.
3.  **Paso 3: Habilitar la Interrupción en el NVIC:** El NVIC (Nested Vectored Interrupt Controller) es el "centro de control de eventos" de la CPU. Debes decirle explícitamente que "escuche" las señales provenientes del periférico que has configurado.
4.  **Paso 4: Escribir la Rutina de Servicio de Interrupción (ISR / Handler):** Crea la función `NombrePeriferico_IRQHandler()` que se ejecutará automáticamente cuando ocurra la interrupción. Dentro de esta función, es crucial:
    a. Identificar la fuente de la interrupción (si es necesario).
    b. Realizar la tarea requerida (leer datos, establecer una bandera, etc.).
    c. **Limpiar la bandera de interrupción del periférico.** Si no lo haces, el periférico seguirá interrumpiendo y el programa se quedará atascado en el handler.

---

#### Ejemplo Práctico: Interrupción Externa (EINT) por Pulsador

Este es el ejemplo clásico: un programa que enciende un LED, pero solo después de que se presione un botón. La detección del botón se hará por interrupción, dejando el bucle `main()` libre. Usaremos el pin **P2.10**, que corresponde a la interrupción externa **EINT0**.

**El Modo Antiguo (Registros):**
```c
// Configurar P2.10 como entrada EINT0 (Función 1)
LPC_PINCON->PINSEL4 |= (1 << 20); 
LPC_PINCON->PINSEL4 &= ~(1 << 21);
// Configurar para interrupción por flanco
LPC_SC->EXTMODE |= (1 << 0); 
// Configurar para flanco de bajada
LPC_SC->EXTPOLAR &= ~(1 << 0); 
// Habilitar la interrupción EINT0 en el NVIC (bit 18)
LPC_NVIC->ISER[0] |= (1 << 18);

// En el Handler...
void EINT0_IRQHandler(void) {
    // Limpiar la bandera escribiendo un 1
    LPC_SC->EXTINT |= (1 << 0);
    // ...hacer algo...
}
```
**Análisis del Modo Antiguo:** Es fácil olvidar un paso, como habilitar la interrupción en el NVIC. Además, ¿quién recuerda que EINT0 es el bit 18 del registro `ISER[0]`? Es muy propenso a errores.

---

**El Nuevo Modo (Drivers CMSIS):**
```c
#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

// Usamos 'volatile' para que el compilador no optimice esta variable,
// ya que es modificada en una interrupción y leída en el bucle principal.
volatile bool button_pressed = false;

/**
 * @brief Handler para la interrupción externa EINT0 (pin P2.10)
 */
void EINT0_IRQHandler(void) {
    // PASO 4c: Limpiar la bandera de interrupción. ¡Siempre primero!
    EXTI_ClearIntPending(EXTI_EINT0);

    // PASO 4b: Realizar la tarea
    button_pressed = true;
}

int main(void) {
    // ... inicializar LED en P0.22 ...
    
    // PASO 1: Configurar el pin P2.10 para la función EINT0
    PINSEL_CFG_Type PinCfg;
    PinCfg.Portnum = PINSEL_PORT_2;
    PinCfg.Pinnum  = PINSEL_PIN_10;
    PinCfg.Funcnum = PINSEL_FUNC_1; // Función 1 para EINT0
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP; // Usar pull-up interno
    PINSEL_ConfigPin(&PinCfg);

    // PASO 2: Configurar la lógica de la interrupción externa
    EXTI_Init(); // Inicializa el periférico EXTI
    EXTI_SetEdge(EXTI_EINT0, EXTI_EDGE_FALLING); // Configurar para flanco de bajada

    // PASO 3: Habilitar la interrupción en el NVIC
    NVIC_EnableIRQ(EINT0_IRQn);

    while(1) {
        // El bucle principal está libre para otras tareas.
        // Aquí solo comprobamos la bandera que la interrupción activa.
        if (button_pressed) {
            // La interrupción ocurrió.
            // Invertir el estado del LED
            uint32_t led_state = GPIO_ReadValue(0);
            if (led_state & (1 << 22)) {
                GPIO_ClearValue(0, (1 << 22));
            } else {
                GPIO_SetValue(0, (1 << 22));
            }
            
            // Reiniciar la bandera para esperar la próxima pulsación
            button_pressed = false;
        }
        
        // ... aquí podría ir otro código que se ejecuta continuamente ...
    }
}
```

**Análisis del Nuevo Modo:**

*   **Claridad:** Funciones como `EXTI_SetEdge(EXTI_EINT0, EXTI_EDGE_FALLING)` son auto-documentadas. Sabes exactamente lo que estás configurando sin consultar el manual.
*   **Abstracción del NVIC:** `NVIC_EnableIRQ(EINT0_IRQn)` es una función estándar de **CMSIS-Core**. Funciona para cualquier interrupción. No necesitas saber el número de IRQ (18) ni en qué registro del NVIC (`ISER[0]`) se encuentra. La macro `EINT0_IRQn` se encarga de eso.
*   **Seguridad:** El driver proporciona la función `EXTI_ClearIntPending()` para limpiar la bandera. Es mucho menos propenso a errores que recordar el registro y el bit correctos.
*   **Eficiencia:** El bucle `main()` ya no está bloqueado esperando un evento. El programa es ahora **dirigido por eventos**, lo que lo hace mucho más potente y eficiente.


---

### El Periférico de Comunicación Serie (UART)

El **UART (Universal Asynchronous Receiver/Transmitter)** es el periférico más común para la comunicación serie asíncrona, vital para enviar datos a la PC (ej. terminal serial) o a otros dispositivos.

#### 1. Conceptos Clave

| Concepto | Descripción |
| :--- | :--- |
| **Baud Rate** | La velocidad de transmisión en bits por segundo (bps). (Ej: 9600, 115200). Es la frecuencia con la que se envían los bits. |
| **Frame** | El "paquete" de datos de la comunicación: 1 Bit de Inicio (Start), 5-8 Bits de Datos, 1 Bit de Paridad (Opcional), 1-2 Bits de Parada (Stop). |
| **FIFO** | **First-In, First-Out.** El UART del LPC1769 tiene buffers (colas) de hardware para 16 bytes. Esto permite al CPU enviar o recibir bloques de datos sin tener que atender bit a bit. |
| **Registros Divisores** | Se usan para dividir el reloj del periférico (`PCLK`) para generar el *Baud Rate* deseado. |

#### 2. Registros Importantes (A Nivel de Control)

| Registro | Propósito |
| :--- | :--- |
| `LPC_UARTx->LCR` | **Line Control Register.** Configura el formato del *Frame*: longitud de palabra (5-8 bits), paridad, bits de parada. |
| `LPC_UARTx->FCR` | **FIFO Control Register.** Habilita el uso del FIFO y los *reset* de las colas. |
| `LPC_UARTx->LSR` | **Line Status Register.** Muestra el estado actual, crucial para el *polling* (ej. `THRE` - Transmit Holding Register Empty, si está listo para enviar). |
| `LPC_UARTx->RBR` | **Receive Buffer Register.** ¡Registro de **lectura**! Contiene el byte recibido más reciente. |
| `LPC_UARTx->THR` | **Transmit Holding Register.** ¡Registro de **escritura**! Se escribe el byte que se quiere transmitir. |

#### 3. Flujo de Programación con Drivers CMSIS

El driver de UART es indispensable ya que el cálculo del *Baud Rate* es complejo.

##### **Función `UART_Init()` (Configuración)**

```c
// [Configuración del Clock PCLK del UART, usando CLKPWR_SetPCLKDiv()]

// 1. Configurar los pines P0.2 y P0.3 para UART0
//    Usar PINSEL_ConfigPin con Funcnum = 1 para P0.2 (Tx) y P0.3 (Rx).

// 2. Definir la configuración de la UART
UART_CFG_Type uart_config;
uart_config.Baud_rate = 115200; // Velocidad de comunicacion
uart_config.Databits = UART_DATABIT_8; // 8 bits de datos
uart_config.Parity = UART_PARITY_NONE; // Sin paridad
uart_config.Stopbits = UART_STOPBIT_1; // 1 bit de parada

// 3. Inicializar la UART (el driver calcula y configura LCR/FCR/DLL/DLM)
UART_Init(LPC_UART0, &uart_config);

// 4. Habilitar la transmisión y recepción
UART_Cmd(LPC_UART0, ENABLE);
```

##### **Función `UART_Send()` (Transmisión)**

Se usa *polling* para asegurar que el registro de transmisión esté libre antes de enviar el siguiente byte.

```c
/**
 * @brief Envía datos usando Polling (bloqueante).
 */
void UART_Send(uint8_t *tx_data, uint32_t len) {
    // La función del driver se encarga de:
    // 1. Monitorear el bit THRE en el registro LSR (Line Status Register).
    // 2. Escribir el byte en el registro THR.
    UART_Send(LPC_UART0, tx_data, len, BLOCKING);
}
```

##### **Función `UART_Receive()` (Recepción)**

Se usa *polling* para esperar que un dato llegue al buffer.

```c
/**
 * @brief Lee datos usando Polling (bloqueante).
 */
uint32_t UART_Receive(uint8_t *rx_data, uint32_t len) {
    // La función del driver se encarga de:
    // 1. Monitorear el bit DR (Data Ready) en el registro LSR.
    // 2. Leer el byte del registro RBR.
    return UART_Receive(LPC_UART0, rx_data, len, BLOCKING);
}
```

---

### Comunicación Serial Síncrona (SPI y I2C)

Para comunicaciones más avanzadas entre chips, se utilizan interfaces síncronas que usan una señal de reloj compartida.

#### El Periférico SSP/SPI (Serial Synchronous Port)

*   **Conceptos Clave:** **Maestro/Esclavo**, **Reloj (SCK)**, Líneas de Datos (**MOSI/MISO**), Chip Select (**CS**). Es ideal para alta velocidad.
*   **Flujo con CMSIS:** Se configura el *Baud Rate* y el modo (Maestro o Esclavo) usando `SSP_Init()`. La transferencia se realiza con `SSP_SendData()` y `SSP_ReceiveData()`, a menudo utilizando DMA para enviar grandes bloques de datos de forma muy eficiente.

#### El Periférico I2C (Inter-Integrated Circuit)

*   **Conceptos Clave:** **Maestro/Esclavo**, **Solo dos líneas** (SDA: Datos, SCL: Reloj), **Direccionamiento** de dispositivos. Es ideal para conectar múltiples chips en un mismo bus (ej. sensores, EEPROMs).
*   **Flujo con CMSIS:** La comunicación es compleja (Estados de START, STOP, ACK/NACK). El driver simplifica esto con funciones de alto nivel como `I2C_MasterTransferData()`, que se encargan de gestionar toda la secuencia de estados.

---

### Integración con DMA 

Un punto clave es cómo la comunicación se beneficia del DMA:

*   **Transferencia UART Tx con DMA:** La CPU configura el Canal DMA con el array a enviar y el registro `THR` del UART como destino. El DMA envía todos los bytes al UART sin que la CPU tenga que esperar la bandera `THRE` byte por byte.
*   **Transferencia ADC/UART con DMA:** El DMA puede configurarse para mover automáticamente un resultado de la conversión ADC a un buffer de memoria, y luego otro canal DMA puede mover ese buffer al UART para ser enviado a la PC. **¡La CPU no hace nada!**

---

## Apéndice: Glosario de Drivers API (LPC1769)

Esta sección resume la Interfaz de Programación de Aplicaciones (API) de los Drivers CMSIS (NXP) para el LPC1769, extraída directamente de la documentación de referencia, para una consulta rápida de estructuras y funciones.

### 1. Configuración de Pines (`PINSEL`)

| Elemento Clave | Tipo | Descripción |
| :--- | :--- | :--- |
| `PINSEL_PORT_OPT` | `enum` | Selección del puerto: `PINSEL_PORT_0` a `PINSEL_PORT_4`. |
| `PINSEL_PIN_OPT` | `enum` | Selección del pin: `PINSEL_PIN_0` a `PINSEL_PIN_31`. |
| `PINSEL_FUNC_OPT` | `enum` | Selección de la función: `PINSEL_FUNC_0` (GPIO) a `PINSEL_FUNC_3`. |
| `PINSEL_PINMODE_OPT` | `enum` | Resistencia interna: `PINSEL_PULLUP`, `PINSEL_REPEATER`, `PINSEL_TRISTATE`, `PINSEL_PULLDOWN`. |
| `PINSEL_OD_OPT` | `enum` | Modo de salida: `PINSEL_OD_NORMAL`, `PINSEL_OD_OPENDRAIN`. |
| `PINSEL_CFG_Type` | `struct` | Estructura de configuración: `portNum`, `pinNum`, `funcNum`, `pinMode`, `openDrain`. |

**Funciones de Control:**

```c
// Configures the pin according to the parameters in pinCfg.
void PINSEL_ConfigPin(const PINSEL_CFG_Type* pinCfg);

// Configures multiple pins according to the parameters in pinCfg and the pins mask.
void PINSEL_ConfigMultiplePins(const PINSEL_CFG_Type* pinCfg, uint32_t pins);
```

---

### 2. Entrada/Salida de Propósito General (`GPIO`)

| Elemento Clave | Tipo | Descripción |
| :--- | :--- | :--- |
| `GPIO_PORT_OPT` | `enum` | Selección del puerto GPIO (`GPIO_PORT_0` a `_4`). |
| `GPIO_DIR_OPT` | `enum` | Dirección del pin: `GPIO_INPUT` (0) o `GPIO_OUTPUT`. |
| `GPIO_INT_EDGE_OPT`| `enum` | Flanco de interrupción: `GPIO_INT_RISING` (0) o `GPIO_INT_FALLING`. |

**Funciones de Control:**

```c
// Sets the direction for the specified GPIO port pins.
void GPIO_SetDir(GPIO_PORT_OPT portNum, uint32_t pins, GPIO_DIR_OPT dir);

// Sets the specified output pins to high on a given GPIO port.
void GPIO_SetPins(GPIO_PORT_OPT portNum, uint32_t pins);

// Clears the specified output pins to low on a given GPIO port.
void GPIO_ClearPins(GPIO_PORT_OPT portNum, uint32_t pins);

// Reads the current state of all pins on the specified GPIO port.
uint32_t GPIO_ReadValue(GPIO_PORT_OPT portNum);

// Toggles the state of specified pins on the given GPIO port.
void GPIO_TogglePins(GPIO_PORT_OPT portNum, uint32_t pins);

// Sets the interrupt enable mask for GPIO pins on the given port.
void GPIO_IntCmd(GPIO_PORT_OPT portNum, uint32_t newValue, GPIO_INT_EDGE_OPT edgeState);

// Clears the interrupt status for selected GPIO pins.
void GPIO_ClearInt(GPIO_PORT_OPT portNum, uint32_t pins);
```

---

### 3. Interrupciones Externas (`EXTI`)

| Elemento Clave | Tipo | Descripción |
| :--- | :--- | :--- |
| `EXTI_LINE_OPT` | `enum` | Línea de interrupción: `EXTI_EINT0` (P2.10) a `EXTI_EINT3` (P2.13). |
| `EXTI_MODE_OPT` | `enum` | Sensibilidad: `EXTI_LEVEL_SENSITIVE` o `EXTI_EDGE_SENSITIVE`. |
| `EXTI_POLARITY_ENUM`| `enum` | Polaridad/Flanco: `EXTI_LOW_ACTIVE`, `EXTI_FALLING_EDGE`, `EXTI_HIGH_ACTIVE`, `EXTI_RISING_EDGE`. |
| `EXTI_CFG_Type` | `struct` | Estructura de configuración: `line`, `mode`, `polarity`. |

**Funciones de Control:**

```c
// Initializes the External Interrupt (EXTI) controller.
void EXTI_Init(void);

// Configures a specific External Interrupt (EXTI) line.
void EXTI_Config(const EXTI_CFG_Type* EXTICfg);

// Clears the external interrupt flag for the specified EXTI line.
void EXTI_ClearFlag(EXTI_LINE_OPT EXTILine);

// Clears the interrupt flag and enables the IRQ for the specified EXTI line.
void EXTI_EnableIRQ(EXTI_LINE_OPT EXTILine);
```

---

### 4. Timers/Contadores (`TIMERS`)

| Elemento Clave | Tipo | Descripción |
| :--- | :--- | :--- |
| `TIM_INT_TYPE` | `enum` | Tipo de interrupción: `TIM_MRx_INT` (Match) o `TIM_CRx_INT` (Capture). |
| `TIM_MODE_OPT` | `enum` | Modo de operación: `TIM_TIMER_MODE`, `TIM_COUNTER_RISING_MODE`, etc. |
| `TIM_TIMERCFG_Type` | `struct` | Configuración del *Prescale*: `prescaleOption` (`TIM_TICKVAL`/`TIM_USVAL`) y `prescaleValue`. |
| `TIM_MATCHCFG_Type` | `struct` | Configuración de la comparación (Match): `matchChannel`, `matchValue`, `intonMatch`, `stopOnMatch`, `resetOnMatch`, `extMatchOutputType`. |
| `TIM_CAPTURECFG_Type`| `struct` | Configuración de la captura: `captureChannel`, `risingEdge`, `fallingEdge`, `intOnCapture`. |

**Funciones de Control:**

```c
// Initializes the specified Timer/Counter peripheral.
void TIM_Init(LPC_TIM_TypeDef *TIMx, TIM_MODE_OPT timerCounterMode, void *TIM_ConfigStruct);

// Configures the match channel for the specified Timer/Counter peripheral.
void TIM_ConfigMatch(LPC_TIM_TypeDef *TIMx, TIM_MATCHCFG_Type *TIM_MatchConfigStruct);

// Updates the match value for the specified Timer/Counter channel.
void TIM_UpdateMatchValue(LPC_TIM_TypeDef *TIMx, TIM_MATCH_CHANNEL_OPT matchChannel, uint32_t matchValue);

// Sets the external match output type for a specific match channel.
void TIM_SetMatchExt(LPC_TIM_TypeDef *TIMx, TIM_MATCH_CHANNEL_OPT matchChannel, TIM_EXTMATCH_OPT extMatchOutputType);

// Enables or disables the specified Timer/Counter peripheral.
void TIM_Cmd(LPC_TIM_TypeDef *TIMx, FunctionalState newState);

// Clears the specified Timer/Counter interrupt pending flag.
void TIM_ClearIntPending(LPC_TIM_TypeDef *TIMx, TIM_INT_TYPE intFlag);
```

---

### 5. Convertidor Analógico-Digital (`ADC`)

| Elemento Clave | Tipo | Descripción |
| :--- | :--- | :--- |
| `ADC_CHANNEL_SELECTION`| `enum` | Selección del canal (`ADC_CHANNEL_0` a `_7`). |
| `ADC_START_OPT` | `enum` | Modo de inicio (`ADC_START_NOW`, `ADC_START_CONTINUOUS`, `ADC_START_ON_EINT0`, etc.). |
| `ADC_DATA_STATUS` | `enum` | Bandera de estado: `ADC_DATA_DONE` (conversión finalizada). |

**Funciones de Control:**

```c
// Initializes the ADC peripheral with the specified conversion rate.
void ADC_Init(uint32_t rate);

// Starts ADC conversion in the specified mode.
void ADC_StartCmd(ADC_START_OPT startMode);

// Enables or disables the specified ADC channel.
void ADC_ChannelCmd(ADC_CHANNEL_SELECTION channel, FunctionalState newState);

// Gets the conversion result for the specified ADC channel.
uint16_t ADC_ChannelGetData(ADC_CHANNEL_SELECTION channel);

// Gets the status flag for the specified ADC channel.
FlagStatus ADC_ChannelGetStatus(ADC_CHANNEL_SELECTION channel, ADC_DATA_STATUS statusType);```

---

### 6. Convertidor Digital-Analógico (`DAC`)

| Elemento Clave | Tipo | Descripción |
| :--- | :--- | :--- |
| `DAC_CURRENT_OPT` | `enum` | Configura el *Bias* (velocidad vs. consumo: 700uA o 350uA). |
| `DAC_CONVERTER_CFG_Type`| `struct` | Configuración avanzada: `doubleBufferEnable`, `counterEnable`, `dmaEnable`. |

**Funciones de Control:**

```c
// Initializes the DAC peripheral.
void DAC_Init(void);

// Updates the output value of the DAC.
void DAC_UpdateValue(uint32_t newValue);

// Configures the DAC converter control features.
void DAC_ConfigDAConverterControl(DAC_CONVERTER_CFG_Type *cfgStruct);
```

---

### 7. Acceso Directo a Memoria (`GPDMA`)

| Elemento Clave | Tipo | Descripción |
| :--- | :--- | :--- |
| `GPDMA_TRANSFER_TYPE` | `enum` | Flujo de transferencia: `GPDMA_M2M`, `GPDMA_M2P`, `GPDMA_P2M`, `GPDMA_P2P`. |
| `GPDMA_CONNECTION` | `enum` | Periférico Origen/Destino: `GPDMA_UARTO_Tx`, `GPDMA_ADC`, etc. |
| `GPDMA_TRANSFER_WIDTH` | `enum` | Ancho de datos: `GPDMA_BYTE`, `GPDMA_HALFWORD`, `GPDMA_WORD`. |
| `GPDMA_Channel_CFG_Type` | `struct` | Configuración completa: `transferSize`, `srcMemAddr`, `dstMemAddr`, `transferType`, `srcConn`, `dstConn`. |

**Funciones de Control:**

```c
// Initializes the GPDMA controller.
void GPDMA_Init(void);

// Configures and sets up a GPDMA channel according to the provided configuration.
Status GPDMA_Setup(const GPDMA_Channel_CFG_Type *cfg);

// Enables or disables the specified GPDMA channel.
void GPDMA_ChannelCmd(GPDMA_CHANNEL channelNum, FunctionalState NewState);

// Clears the pending interrupt flag for the specified GPDMA channel.
void GPDMA_ClearIntPending(GPDMA_CLEAR_INT type, GPDMA_CHANNEL channel);
```

---