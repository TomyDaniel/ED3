# Consigna del Ejercicio: "Hola, Mundo del Hardware: Blinky Básico"

**Objetivo General**

El objetivo de este ejercicio es escribir, compilar y entender un programa fundamental para un microcontrolador LPC1769: hacer que un LED parpadee. Este proyecto sirve como el "Hola, Mundo" del desarrollo de sistemas embebidos, verificando que el entorno de desarrollo está correctamente configurado y demostrando el control básico de un pin de hardware.

**Requisitos de Hardware**

- *Placa de Desarrollo:* LPCXpresso1769.
- *Componente:* El LED azul integrado en la placa.
- *Pin de Conexión:* El LED azul está conectado al pin **P0.22** del microcontrolador.

**Requisitos Funcionales**

1. **Configuración del Pin:** El pin P0.22 debe ser configurado como una salida de propósito general (GPIO).
2. **Secuencia de Parpadeo:** El programa debe entrar en un bucle infinito donde el LED se enciende, permanece encendido durante un período de tiempo visible (aprox. 500 ms), luego se apaga y permanece apagado por el mismo período.
3. **Ciclo Continuo:** Esta secuencia de encendido y apagado debe repetirse indefinidamente mientras el microcontrolador esté alimentado.

**Pasos de Implementación**

1. **Creación del Proyecto:**
    - Crear un nuevo proyecto en MCUXpresso IDE para el microcontrolador LPC17xx.

2. Configuración del Hardware (configurar_LED):

    - **Selección de Función (PINSEL):**
        - Identificar el registro PINSEL que controla el pin P0.22 (Puerto 0, mitad superior).
        - Calcular la posición de los bits de control para el pin 22.
        - Escribir en el registro para configurar la función del pin como GPIO (valor 00).
    - **Selección de Dirección (FIODIR):**
        - Identificar el registro FIODIR para el Puerto 0.
        - Escribir en el registro para configurar el pin P0.22 como Salida (valor 1).

3. Implementación de la Lógica (main):

    - Llamar a la función de configuración una sola vez al inicio.
    - Crear un bucle infinito (while(1)).
    - Dentro del bucle:
        - Utilizar el registro FIOSET del Puerto 0 para encender el LED.
        - Llamar a una función de retardo (delay).
        - Utilizar el registro FIOCLR del Puerto 0 para apagar el LED.
        - Llamar a la misma función de retardo.

4. Función de Retardo (delay):

    - Implementar una función que genere una pausa mediante un bucle for. El contador del bucle debe ser declarado como volatile para evitar que el compilador lo optimice y lo elimine.