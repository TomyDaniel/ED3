En la dirección de memoria 0x20080000 se tiene almacenado un valor de 32 bits que representa cuatro formas de onda binarias de 8 bits cada una. Utilizando los registros de configuración y el systick del microcontrolador LPC1769 generar por el pin P2.8 las formas de onda binarias en serie de 8 bits almacenadas en la dirección anteriormente mencionada, y generar por el puerto P2 el promedio de la forma de onda binaria seleccionada.

El pin asociado a EINTO presenta una resistencia de pull-down externa.

Configurar la interrupción de dicho pin con prioridad 3, para que, cada vez que interrumpa, termine la forma de onda actual y cambie a la siguiente (una vez que llega a la última, debe volver a comenzar la primera).

El periodo de la señal debe ser establecido mediante la interrupción (prioridad 2) del pin asociado a EINT1, el cual presenta una resistencia de pull-up. De manera que se pueda cambiar el periodo de la señal entre 80 [ms] (por defecto) y 160 [ms]. Considerando un cclk de 65 [MHz].