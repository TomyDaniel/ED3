# Ejercicio 2 (35%)

En una fábrica, hay un sistema de alarma utilizando una LPC1769 Rev. D trabajando a una frecuencia de CCLK a 100 [MHz], conectado a un sensor de puerta que se activa cuando la puerta se abre. El sensor está conectado al pin P0[6], el cual genera una interrupción externa (EINT) cuando se detecta una apertura (cambio de estado). Al detectar que la puerta se ha abierto, el sistema debe iniciar un temporizador utilizando el Systick para contar un período de 30 segundos.
Durante estos 30 segundos, el usuario deberá introducir un código de desactivación mediante un DIP switch de 4 entradas conectado a los pines P2[0] - P2[3]. El código correcto es 0xAA (1010 en binario). El usuario tiene dos intentos para introducir el código correcto. Si después de dos intentos el código ingresado es incorrecto, la alarma se activará, encendiendo un buzzer conectado al pin P1[11].

**Se pide:**

- Realice el diseño de la solución a implementar mediante un diagrama de flujo o actividad.
- Implemente el firmware debidamente comentado en C, usando CMSIS o indirecta por medio de drivers.