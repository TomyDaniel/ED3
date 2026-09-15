Complete y corrija las funciones mostradas a continuación, asumiendo que los prototipos de funciones, funciones de 
configuraciones de GPIO e Interrupciones, y función principal (main) ya se encuentran completos. El siguiente código era
 C para un LPC1769 (CCLK=120MHz), debe realizar:

1) Emitir la secuencia binaria "seq" por el pin P0.15 usando SysTick.

2) Ante cada interrupción por flanco ascendente en P0.15, debe alternar la duración de los elementos de la secuencia entre 5 ms 
y 10 ms. Considerar 5 ms como valor inicial.

3) Ante una interrupción EINT1, por flanco de bajada, debe detener la secuencia deshabilitando SysTick, y dejar 
el pin P0.15 en estado lógico bajo.