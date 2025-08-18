# Ejercicio Práctico 2: El Impacto del Reloj del Periférico (PCLK)
**Objetivo**

Comprender cómo la configuración del reloj de un periférico (PCLK) afecta su comportamiento y por qué los cálculos (como el Prescaler del Timer) dependen directamente de esta configuración.

**Escenario**

Partimos de tu código funcional que hace parpadear un LED cada 500ms. Tu función delay_ms funciona perfectamente porque el Prescaler del Timer0 (PR = 24) fue calculado asumiendo que el reloj del Timer0 (PCLK_TIM0) es de 25 MHz.

Por defecto, el LPC1769 configura el reloj de la mayoría de los periféricos para que sea la cuarta parte de la velocidad del reloj principal (CCLK). Si asumimos que CCLK es 100 MHz, entonces PCLK = 100 MHz / 4 = 25 MHz.

**¿Pero qué pasa si cambiamos esa velocidad?**

**El Reto**

Tu tarea es modificar la velocidad del reloj **exclusivamente para el Timer0** y observar el efecto que tiene sobre el parpadeo del LED.

1. **Investigar**: El registro que controla la velocidad de los periféricos es `LPC_SC->PCLKSEL0` y `LPC_SC->PCLKSEL1`. Específicamente, el reloj del Timer0 se controla con los bits 2 y 3 del registro `LPC_SC->PCLKSEL0`.  
Los valores posibles para esos dos bits son:

    - `00: PCLK_TIM0 = CCLK / 4` (El valor por defecto, 25 MHz)
    - `01: PCLK_TIM0 = CCLK / 1` (100 MHz)
    - `10: PCLK_TIM0 = CCLK / 2` (50 MHz)
    - `11: PCLK_TIM0 = CCLK / 8` (12.5 MHz)

2. **Modificar**: Ve a tu función configuraciones() y añade una línea de código para cambiar el reloj del Timer0 a CCLK / 1 (100 MHz).

3. **Predecir**: Antes de compilar y grabar el código, responde a esta pregunta: Si el reloj que alimenta al Timer0 ahora es 4 veces más rápido, pero el Prescaler y la lógica de delay_ms siguen exactamente iguales, ¿qué le pasará a la velocidad del parpadeo del LED? ¿Será más rápido, más lento o igual?

4. **Verificar**: Compila y ejecuta el código en tu placa. ¿Tu predicción fue correcta?