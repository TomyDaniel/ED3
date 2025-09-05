# **Ejercicio 1 (45%)**

Un estacionamiento automatizado utiliza una barrera que se abre y cierra en función de la validación de un ticket de acceso utilizando una LPC1769 Rev. D trabajando a una frecuencia de CCLK a 70 [MHz].

Cuando el sistema detecta que un automóvil se ha posicionado frente a la barrera, se debe activar un sensor conectado al pin **P2[4]** mediante una interrupción externa (EINT). Una vez validado el ticket, el sistema activa un motor que abre la barrera usando el pin **P0[15]**. El motor debe estar activado por **X segundos** y luego apagarse, utilizando el temporizador Systick para contar el tiempo. Si el ticket es inválido, se encenderá un LED rojo conectado al pin **P1[5]**.

Para gestionar el tiempo de apertura de la barrera, existe un switch conectado al pin **P3[4]** que dispone de una **ventana de configuración de 3 segundos** gestionada por el temporizador Systick.

Durante dicha ventana, se debe contar cuantas veces se presiona el switch y en función de dicha cantidad, establecer el tiempo de la barrera.

| Cantidad de veces que se presiona el switch | Tiempo de barrera |
| :----------------------------------------- | :---------------- |
| 0x00                                       | 5 segundos        |
| 0x01                                       | 10 segundos       |
| 0x02                                       | 20 segundos       |
| 0x03                                       | 40 segundos       |
| 0x04                                       | 5 segundos        |