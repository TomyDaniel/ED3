# Ejercicio Práctico: Configuración de Clocks para Periféricos
**Objetivo**

Comprender la necesidad de habilitar los relojes de los periféricos (PCONP) antes de su configuración y uso. Reforzar el proceso de configuración de un pin GPIO.

**Escenario**

Vamos a hacer parpadear un LED conectado al pin **P1.29**. Para controlar el tiempo de parpadeo, utilizaremos el Timer0 del microcontrolador. Sin embargo, antes de poder configurar tanto el pin **P1.29** como el Timer0, debemos asegurarnos de que sus respectivos bloques tengan "energía".

**Tu Tarea**

Deberás escribir el código necesario para:
- Habilitar el reloj para el periférico Timer0.
- Configurar la función del pin **P1.29** para que opere como GPIO.
- Configurar la dirección del pin **P1.29** como salida.