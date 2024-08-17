/* changing led on interrupt & button click */

#include "stm32f103x6.h"
#include <stdint.h>

#define NO_OF_LEDS 4
#define MS 1000

typedef enum LED {
    RED,
    GREEN,
    BLUE,
    NONE
} LED;

volatile LED curr_led = RED;
volatile uint32_t core_clock_hz;

void start_timer(TIM_TypeDef *TIMx, uint16_t ms);
void stop_timer(TIM_TypeDef *TIMx);
void switch_led();

int main() {
    
    /* i wont explain in detail below code, i have done it already in different programs */

    /* setting up led pins and a button pin */

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; 
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk; 

    GPIOB->CRH &= ~GPIO_CRH_MODE12_Msk & ~GPIO_CRH_MODE13_Msk & ~GPIO_CRH_MODE14_Msk;
    GPIOB->CRH |= GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 | GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1 | GPIO_CRH_MODE14_0 | GPIO_CRH_MODE14_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF12_Msk & ~GPIO_CRH_CNF13_Msk & ~GPIO_CRH_CNF14_Msk;
    
    GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk;
    GPIOA->CRL |= GPIO_CRL_CNF0_1;
    GPIOA->ODR |= GPIO_ODR_ODR0;

    /* setting up flash */

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    /* setting up clock */

    RCC->CFGR |= RCC_CFGR_PLLMULL16;
    RCC->CR |= RCC_CR_PLLON;
    while ( !(RCC->CR & RCC_CR_PLLRDY) ) {};

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ) {};

    core_clock_hz = 64000000;

    /* setting up timer */

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* setting up button cont. */

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->RTSR &= ~EXTI_RTSR_TR0; // maybe i should use it?
    EXTI->FTSR |= EXTI_FTSR_TR0;

    /* setting up interrupts */

    NVIC_SetPriority(EXTI0_IRQn, 3);
    NVIC_EnableIRQ(EXTI0_IRQn);

    NVIC_SetPriority(TIM2_IRQn, 3);
    NVIC_EnableIRQ(TIM2_IRQn);

    start_timer(TIM2, MS);

    while (1) {};
}

void start_timer(TIM_TypeDef *TIMx, uint16_t ms) {
    TIMx->CR1 &= ~TIM_CR1_CEN;

    if ( TIMx == TIM2 ) {
        RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
        RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
    }

    TIMx->PSC |= core_clock_hz / 1000 - 1;
    TIMx->ARR = ms;
    TIMx->EGR |= TIM_EGR_UG;
    TIMx->DIER |= TIM_DIER_UIE;
    TIMx->CR1 |= TIM_CR1_CEN;
}

void stop_timer(TIM_TypeDef *TIMx) {
    TIMx->CR1 &= ~TIM_CR1_CEN;
    TIMx->SR &= ~TIM_SR_UIF;
}

void switch_led() {
    switch (curr_led) {
        case RED: {
            GPIOB->BSRR |= GPIO_BSRR_BS14;
            GPIOB->BSRR |= GPIO_BSRR_BR13;
            GPIOB->BSRR |= GPIO_BSRR_BR12;
            break; 
        } 
        case GREEN: { 
            GPIOB->BSRR |= GPIO_BSRR_BR14; 
            GPIOB->BSRR |= GPIO_BSRR_BS13; 
            GPIOB->BSRR |= GPIO_BSRR_BR12; 
            break; 
        } 
        case BLUE: { 
            GPIOB->BSRR |= GPIO_BSRR_BR14; 
            GPIOB->BSRR |= GPIO_BSRR_BR13;
            GPIOB->BSRR |= GPIO_BSRR_BS12; 
            break; 
        } 
        default: { 
            GPIOB->BSRR |= GPIO_BSRR_BR14; 
            GPIOB->BSRR |= GPIO_BSRR_BR13;
            GPIOB->BSRR |= GPIO_BSRR_BR12;
            break;
        }
    }
}

/* handlers */

void TIM2_IRQHandler(void) {
    if ( TIM2->SR & TIM_SR_UIF ) {
        TIM2->SR &= ~TIM_SR_UIF;

        curr_led = ( curr_led + 1 ) % NO_OF_LEDS;
        
        GPIOC->ODR ^= GPIO_ODR_ODR13;
        
        switch_led();
    }
}

void EXTI0_IRQHandler(void) {
    if ( EXTI->PR & EXTI_PR_PR0 ) {
        EXTI->PR |= EXTI_PR_PR0;

        // GPIOC->ODR ^= GPIO_ODR_ODR13;
      
        stop_timer(TIM2);
        
        if ( !(GPIOA->IDR & GPIO_IDR_IDR0) ) {
        
            GPIOB->BSRR |= GPIO_BSRR_BS14; 
            GPIOB->BSRR |= GPIO_BSRR_BS13;
            GPIOB->BSRR |= GPIO_BSRR_BS12;

            while ( !(GPIOA->IDR & GPIO_IDR_IDR0) ) {};
        }

        switch_led();
        start_timer(TIM2, MS);
    }
}
