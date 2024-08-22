/* using the button to cycle through a series of blink rates */
/* its almost the same as one of a previous programs, but this time i actually understand what i am doing and how to determine values of the compare/prescaler registers */

/* i want to get rid of hardcoded pins in the future */

#include "stm32f103x6.h"

void configure_gpios(void);
void configure_clock(void);
void start_timer(uint16_t prescaler, uint16_t compare);
void stop_timer(void);

typedef struct blinkType {
    char const* name;
    uint16_t prescaler;
    uint16_t compare;
} blinkType;

// so now, i have a clock at 64Mhz; i want my LED to blink at 10 Hz -> that means interrupting it at 20 Hz, to turn it on, and then of; 
// 64 000 000 / 20 = 3 200 000
// TIM3 is 16bit, means it can count up to 65 535, so i need to use the prescaler
    
// interruptFrequency = clockIn / ( prescaler * compare )
// i know the clockIn, and the interruptFrequency, and that prescaler * compare = 3 200 000
// 3 200 * 1000

// i also want a 5Hz and 1Hz blinks
// for 5Hz, 64 000 000 / 10 = 6 400 000, which is equal to 6 400 * 1000
// for 1Hz, 64 000 000 / 2 = 32 000 000, which is equal to 32 000 * 1000

const blinkType modes[] = {
    { "10 Hz", 3200, 1000 },
    { "5 Hz", 6400, 1000 },
    { "1 Hz", 32000, 1000 }
};

volatile uint8_t curr_mode = 0;
volatile const uint8_t no_of_modes = 3;

int main(void) {

    configure_gpios();
    configure_clock();
  
    // interrupts

    NVIC_SetPriority(TIM3_IRQn, 3);
    NVIC_EnableIRQ(TIM3_IRQn);

    NVIC_SetPriority(EXTI0_IRQn, 3);
    NVIC_EnableIRQ(EXTI0_IRQn);

    while (1) {};
}

void TIM3_IRQHandler(void) {
    
    // update is pending

    if ( TIM3->SR & TIM_SR_UIF ) {

        // "This bit is set by hardware on an update event. It is cleared by software."

        TIM3->SR &= ~TIM_SR_UIF;
        GPIOA->ODR ^= GPIO_ODR_ODR6;
    }
}

void EXTI0_IRQHandler(void) {

    if ( EXTI->PR & EXTI_PR_PR0 ) {
        
        EXTI->PR |= EXTI_PR_PR0;

        stop_timer();

        if ( !(GPIOC->IDR & GPIO_IDR_IDR0) ) {
            
            GPIOC->BSRR |= GPIO_BSRR_BR13;

            curr_mode = (curr_mode + 1) % no_of_modes;

            start_timer(modes[curr_mode].prescaler, modes[curr_mode].compare);

            while ( !(GPIOA->IDR & GPIO_IDR_IDR0) ) {};
        }

        GPIOC->BSRR |= GPIO_BSRR_BS13;

        // once again, i made debouncer with a capacitor
    }
}
void start_timer(uint16_t prescaler, uint16_t compare) {

     // disable the counter
    
    TIM3->CR1 &= ~TIM_CR1_CEN;

    // reset the timer

    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;

    // prescaler

    TIM3->PSC = prescaler - 1; // prescaler starts from 0

    // output compare register

    TIM3->CCR1 = compare;

    // auto reload register

    TIM3->ARR = compare - 1;

    // event generation register, update generation

    TIM3->EGR |= TIM_EGR_UG;

    // enable interrupt

    TIM3->DIER |= TIM_DIER_UIE;

    // enable timer

    TIM3->CR1 |= TIM_CR1_CEN;
}

void stop_timer(void) {
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->SR &= ~TIM_SR_UIF;
}

void configure_gpios(void) {

    // PC13 for debug, as always, and PA6 as an output, and PA0 as an input
    
    RCC->APB2ENR |= ( RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN );

    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;
    GPIOC->CRH |= GPIO_CRH_MODE13_1;
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk; 

    GPIOA->CRL &=  ~GPIO_CRL_MODE6_Msk;
    GPIOA->CRL |= GPIO_CRL_MODE6_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF6_Msk; 
    GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk;
    GPIOA->CRL |= GPIO_CRL_CNF0_1;

    GPIOA->ODR |= GPIO_ODR_ODR0;

    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->RTSR &= ~EXTI_RTSR_TR0; 
    EXTI->FTSR |= EXTI_FTSR_TR0;
}

void configure_clock(void) {

    // im going to use PLL, to achieve 64Mhz (can i ever achieve 72Mhz?? probably only with HSE)
    // via manual: "When the HSI is used as a PLL clock input, the maximum system clock frequency that can be achieved is 64 MHz."
    
    // setting up flash, access control register -> prefetch buffer, latency 

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2; // two wait states for 48Mhz < SYSCLK < 72MHz
    
    // 64Mhz = ( 8Mhz / 2 ) *  16 

    // multiply
    
    RCC->CFGR |= RCC_CFGR_PLLMULL16;

    // turn on, wait a bit

    RCC->CR |= RCC_CR_PLLON;
    while ( !(RCC->CR & RCC_CR_PLLRDY) ) {};

    // set PLL as a system clock source, wait a bit

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ) {};

    // im going to use TIM3CH1

    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}
