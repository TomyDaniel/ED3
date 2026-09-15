static uint32_t contador = 0;
static uint32_t paso = 0;

int main(){
    configPWM();

    while(1){}

}

void configPWM(){
    contador = perfil[1][0] * 1000; // Pongo el 300s en el contador
    conf_timer0_pwm(perfil[0][0]);
}

void EINT0_IRQHandler(){
    LPC_TIM0 -> MR1 = 0;
    LPC_TIM0 -> EMR &= ~(1 << 1);
    LPC_SC -> PCONP &= ~(1 << 1); // Apago timer
    LPC_GPIO0 -> FIOSET = (1 << 0); // hago de cuenta que hay un buffer
    LPC_SC -> EXTINT |= (1 << 0); // Bajo la flag;
}

void TIMER0_IRQHandler(){

    if(contador <= 0){
        if(paso < 10){
            paso++;
            LPC_TIM0 -> MR1 = (25000* perfil[0][paso]) / 100;
            contador = perfil[1][paso] * 1000;
        }else{
            paso = 0;
        }
    }else{
        contador--;
    }

    LPC_TIM0 -> IR |= (3 << 0);

}
