# Consigna del Ejercicio: "El Contador Binario de 4 Bits"

**Objetivo General**

Construir un contador de 4 bits que muestre números del 0 al 15 en formato binario utilizando 4 LEDs. Este proyecto enseña a manejar un puerto o un subconjunto de sus pines como una única entidad para representar datos.

**Requisitos de Hardware**

- **Componentes:** 4 LEDs.
- **Pines de Conexión:** Para facilitar la programación, los 4 LEDs estarán conectados a los 4 pines menos significativos (los primeros 4) de un solo puerto.
    - LED 0 (Bit 0 - LSB): **P2.0**
    - LED 1 (Bit 1): **P2.1**
    - LED 2 (Bit 2): **P2.2**
    - LED 3 (Bit 3 - MSB): **P2.3**

**Requisitos Funcionales**

1. **Configuración:** Los 4 pines (P2.0, P2.1, P2.2, P2.3) deben ser configurados como salidas GPIO.
2. **Contador:** El programa debe mantener una variable de conteo que se incremente en cada ciclo.
3. **Secuencia:** El contador debe ciclar desde 0 (0b0000) hasta 15 (0b1111). Al llegar a 15, debe volver a 0 en el siguiente ciclo.
4. **Visualización:** El valor actual del contador debe mostrarse en los 4 LEDs. Por ejemplo:
    - Si el contador es 5 (0b0101), deben encenderse los LEDs conectados a P2.0 y P2.2.
    - Si el contador es 12 (0b1100), deben encenderse los LEDs conectados a P2.2 y P2.3.
5. **Ritmo:** Debe haber una pausa visible (aprox. 500 ms) entre cada incremento del contador para que la secuencia sea fácil de seguir.

**Pasos de Implementación**
1. Configuración Eficiente (configurarContador):
    - Todos los pines están en el Puerto 2, mitad inferior (0-15), por lo que todos usarán PINSEL4.
    - Reto de eficiencia: En lugar de configurar cada pin individualmente, ¿puedes hacerlo todo en una o dos líneas?
        - Para configurar los 4 pines como GPIO, necesitas limpiar los primeros 8 bits de PINSEL4. ¿Cuál sería la máscara de bits para eso?
        - Para configurar los 4 pines como SALIDA, necesitas poner en 1 los primeros 4 bits de FIODIR2. ¿Cuál es la máscara para eso?
2. Lógica del main:
    - Declara una variable contador (por ejemplo, uint8_t contador = 0;).
    - Dentro del bucle while(1):
        - Limpieza: Antes de mostrar el nuevo número, es una buena práctica apagar todos los LEDs del contador para evitar estados intermedios.
        - Escritura: Escribe el valor de la variable contador directamente en los pines. ¡Descubrirás que FIOSET es perfecto para esto!
        - Pausa: Llama a tu función delay.
        - Incremento: Incrementa la variable contador.
        - Control de Desborde: Comprueba si el contador ha superado 15. Si es así, reinícialo a 0. Una forma elegante de hacer esto es con el operador módulo (%).