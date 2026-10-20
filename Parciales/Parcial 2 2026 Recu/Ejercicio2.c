/*
Dada la siguiente función:

1- Escriba las líneas de código que configuren de forma adecuada un pin que pueda ser utilizado con el código mostrado.
2- ¿Cuál es el intervalo máximo e intervalo mínimo de tiempo que se va a poder medir con esta configuración?
3- Explique con sus propias palabras para qué se utiliza este modo de funcionamiento del timer.
*/

void config_timer_x()
{
    TIM_TIMERCFG_Type   struct_config;
    TIM_CAPTURECFG_Type struct_capture;

    struct_config.PrescaleOption = TIM_PRESCALE_USVAL;
    struct_config.PrescaleValue = 1;

    struct_capture.CaptureChannel = 0;
    struct_capture.RisingEdge = ENABLE;
    struct_capture.FallingEdge = ENABLE;
    struct_capture.IntOnCaption = ENABLE;

    TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &struct_config);
    TIM_ConfigCapture(LPC_TIM1, &struct_capture);
    TIM_Cmd(LPC_TIM1, ENABLE);
    NVIC_EnableIRQ(TIMER1_IRQn);
}

// Con los drivers nuevos es hacer un pin config del CAP0. Es el P1.26
