# Ejercicio Práctico Básico: LED Interactivo

**Objetivo**

Poner en práctica los conceptos de GPIO (entrada y salida) y la creación de retardos precisos con el timer SysTick. Crearás un programa donde un LED parpadea, pero su comportamiento cambia al presionar un botón.

**Materiales**

- Placa de desarrollo con LPC1769.
- 1 LED (cualquier color).
- 1 Resistencia de ~220Ω o 330Ω.
- 1 Pulsador (push-button).
- Protoboard y cables de conexión.

**Pines a Utilizar**

Para mantener la consistencia con el apunte teórico, usaremos:

- LED (Salida): Conectado al pin P0.22.
- Pulsador (Entrada): Conectado al pin P2.10.

**Descripción del Funcionamiento**

El programa debe comportarse de la siguiente manera:

- Al encender el microcontrolador, el LED debe comenzar a parpadear continuamente: 500 ms encendido, 500 ms apagado.
- Si el usuario presiona y mantiene presionado el pulsador, el parpadeo debe detenerse y el LED debe quedar encendido de forma sólida.
- Cuando el usuario suelta el pulsador, el LED debe apagarse y reanudar el parpadeo normal (500 ms encendido, 500 ms apagado).