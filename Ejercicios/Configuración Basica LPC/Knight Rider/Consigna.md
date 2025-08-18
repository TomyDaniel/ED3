# El Desafío: "El Auto Fantástico" (Efecto K.I.T.T.)

**Objetivo:** Crear el famoso efecto de la luz que se mueve de un lado a otro, como en el auto de la serie "Knight Rider" (El Auto Fantástico).

**Hardware Asumido:**

- Vamos a usar 6 LEDs para crear la barra de luz.

- Para que practiques con un bloque de pines, los conectaremos todos al mismo puerto y de forma consecutiva:

    - LED 1: **P1.18**
    - LED 2: **P1.19**
    - LED 3: **P1.20**
    - LED 4: **P1.21**
    - LED 5: **P1.22**
    - LED 6: **P1.23**

**La Secuencia Lógica:**

1. Se enciende el LED 1, los demás apagados. Se espera un momento.
2. Se enciende el LED 2, los demás apagados. Se espera un momento.
3. ...y así sucesivamente hasta encender el LED 6.
4. Una vez que llega al final (LED 6), la secuencia se invierte.
5. Se enciende el LED 5, los demás apagados. Se espera un momento.
6. Se enciende el LED 4, los demás apagados. Se espera un momento.
7. ...y así hasta volver al LED 1.
8. El ciclo completo se repite indefinidamente.