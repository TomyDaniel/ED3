uint32_t nTicks[2] = {0x927BE, /*completar = 0x124F7F*/}, cTicks[2] = {0x00, 0x07}; // 011 = 0x03
uint8_t seq[15] = {0,0,0,0,1,0,1,0,1,0,1,0,0,0,0};

// 0.001 * 120M - 1 = 
// 0.01 * 120M - 1 = 1199999 = 0x124F7F

void EINT3_IRQHandler(void){ 
    static uint8_t index = 1;

    if(LPC_GPIOINT->IntStatus & (1<<2)){ // Mal, es 1 << 0, ahi veo puerto 2
        if(LPC_GPIOINT->IO2IntStatF & (1<<15)){ // Mal, debe ser ascendente, IO0IntStatF
            cfgSysTick(nTicks[index], cTicks[0]); // Apago timer, deberia ser 1
            index=(index+1)%1; // Deberia ser %2
        }
    }
}


void EINT1_IRQHandler(void){
    cfgSysTick(nTicks[0], cTicks[0]); // ok, deshabilita 
    LPC_GPIO2->FIOSET |= (1<<15); // Mal puerto y mal FIO
    LPC_GPIOINT->IO0IntClr |= (1<<15); // LPC_SC -> EXTINT | = (1 << 1);
}


void SysTick_Handler(void){
    static uint8_t index = 0;

    if(seq[index]) LPC_GPIO0->FIOCLR = (1 << 15); // FIOSET

    if(!seq[index]) LPC_GPIO0->FIOSET = (1 << 15); // FIOCLR

        // como esta era 111101110111

    index=(index+1)%15;
}


void cfgSysTick(uint32_t nTicks, uint8_t cTicks){
    SysTick->LOAD = nTicks;
    SysTick->VAL = 0x01; // Deberia ser 0
    SysTick->CTRL = cTicks;
}

/* 
    1) 
    Correcion systick
    FIOSET por fioclr y viceversa

    2) 
    Cambiar el (1 << 0)
    Cambiar IO0IntStatR
    Cambiar cTicks y poner 0x03 pq sino apago timer

    3)
    Cambiar puerto

*/
