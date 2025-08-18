# Consigna del Ejercicio: "El Interruptor de Palanca (Toggle Switch)"

**Objetivo General**

Crear un programa que utilice un botón como un interruptor de palanca. Cada vez que se presiona el botón, un LED debe cambiar su estado: si estaba apagado, se enciende; si estaba encendido, se apaga.

**Requisitos de Hardware**
- LED (Salida): Conectado al pin **P0.4**.
- Botón (Entrada): Conectado al pin **P2.12**.

**Requisitos Funcionales**

- Configuración:
    - El pin del LED (P0.4) debe ser configurado como salida GPIO.
    - El pin del botón (P2.12) debe ser configurado como entrada GPIO.
- Lógica del Interruptor:
    - El programa debe detectar una sola pulsación del botón. Es decir, si mantienes el botón presionado, el LED debe cambiar de estado solo una vez, no parpadear locamente.
    - Al detectar una pulsación, el estado del LED debe invertirse.
- Estado Inicial: El LED debe comenzar apagado.

**El Gran Desafío: El "Debounce" (Antirrebote)**

Los botones mecánicos no son perfectos. Cuando los presionas, el contacto metálico interno "rebota" varias veces a nivel microscópico, generando múltiples señales de "presionado/no presionado" en milisegundos. Si no manejas esto, tu programa pensará que presionaste el botón 50 veces muy rápido, y el LED podría terminar en el estado incorrecto.

**Tu reto es implementar una solución simple para este problema. La más fácil es:**
1. Detectar que el botón ha sido presionado.
2. Si lo está, cambiar el estado del LED.
3. Inmediatamente después, esperar en un bucle hasta que el botón sea soltado. Esto asegura que el código solo continuará una vez que el dedo se haya levantado del botón.