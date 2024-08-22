/* *very* simple pwm demo */

int main(void) {

    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB2ENR |= ( RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN );

    // for debug
    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; 
    GPIOC->CRH |= GPIO_CRH_MODE13_1;
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;

    // A6 as a alternate function push pull
    GPIOA->CRL &= ~GPIO_CRL_MODE6_Msk;
    GPIOA->CRL |= GPIO_CRL_MODE6_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF6_Msk;
    GPIOA->CRL |= GPIO_CRL_CNF6_1;

    // tim 3 channel 1 corresponds to A6

    // auto reload-preload
    TIM3->ARR = 1000;
    
    // output compare mode as pwm mode 1
    TIM3->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
    
    // output channel 1
    TIM3->CCER |= TIM_CCER_CC1E;

    // aaaaand start the timer
    TIM3->CR1 |= TIM_CR1_CEN;

    uint32_t duty_cycle = 0;
    int32_t step = 1;

    while(1) {
        duty_cycle += step;
        
        if (duty_cycle == 0 || duty_cycle == 1000) {
            step = -step; // just reverse
            GPIOC->ODR ^= GPIO_ODR_ODR13; 
        }

        TIM3->CCR1 = duty_cycle;

        // i do not like this delay
        for (int i = 0; i < 1000; ++i) __asm__("nop");
    };
}
