/* pwm stuff to ensure that i understand what i am doing
*
*  to make sure everything works, i will at first make LED toggle every 10Hz
*  speed is set to 64Mhz -> 64 000 000 / 10 = 6 400 000
*  interruptFrequency = clockIn / ( prescaler * compare )
*
*  6 400 * 1000
*
*  10Hz is definitely to slow for a dimming
*  lets try 100Hz
*
*  64 000 000 / 100 = 640 000
*  640 * 1000
*
*  and 200 for a speaker test
*  
*  64 000 000 / 200 = 320 000
*  320 * 1000
*
*  now, lets make a duty cycle of x%
*  this means two compare registers
*  that means, firstly LED is turned on, until timer == compare register B
*  then, it is turned off, and when timer == compare register A, LED turns on, and timer resets
*/

#include "stm32f103x6.h"

void configure_gpios(void);
void configure_clock(void);
void start_timer(uint16_t prescaler, uint16_t compare, uint16_t duty_cycle);
void stop_timer(void);

typedef struct blinkType {
    char const* name;
    uint16_t prescaler;
    uint16_t compare;
    uint16_t duty_cycle;
} blinkType;

const blinkType modes[] = {
    { "80%", 320, 1000, 80 },
    { "50%", 320, 1000, 50 },
    { "25%", 320, 1000, 25 },
    { "10%", 320, 1000, 10 }
};

volatile uint8_t curr_mode = 0;
volatile const uint8_t no_of_modes = 4;

int main(void) {
    
    configure_clock();
    configure_gpios();

    NVIC_SetPriority(EXTI0_IRQn, 3);
    NVIC_EnableIRQ(EXTI0_IRQn);
        
    while(1) {};
}


void EXTI0_IRQHandler(void) {

    if ( EXTI->PR & EXTI_PR_PR0 ) {
        
        EXTI->PR |= EXTI_PR_PR0;

        stop_timer();

        if ( !(GPIOC->IDR & GPIO_IDR_IDR0) ) {

            GPIOC->BSRR |= GPIO_BSRR_BR13;

            curr_mode = (curr_mode + 1) % no_of_modes;

            start_timer(modes[curr_mode].prescaler, modes[curr_mode].compare, modes[curr_mode].duty_cycle);

            while ( !(GPIOA->IDR & GPIO_IDR_IDR0) ) {};
        }

        GPIOC->BSRR |= GPIO_BSRR_BS13;
    }
}

void stop_timer(void) {
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->SR &= ~TIM_SR_UIF;
}

void start_timer(uint16_t prescaler, uint16_t compare, uint16_t duty_cycle) {
    // made with an official stm32 generel-purpose timer cookbook 
    
    TIM3->CR1 &= ~TIM_CR1_CEN;

    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;

    TIM3->PSC = prescaler;

    TIM3->ARR = compare - 1;

    // The TIM3 timer channel 1 after reset is configured as output 
    // TIM3->CC1S reset value is 0 
    
    // output compare mode as pwm mode 1
    TIM3->CCMR1 |= ( TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 );

    // capture-compare register (to set the duty cycle)
    TIM3->CCR1 = ( TIM3->ARR * duty_cycle ) / 100;

    // Enable the TIM3 channel1 and keep the default configuration 
    // (state after reset) for channel polarity 
    TIM3->CCER |= TIM_CCER_CC1E;

    // aaaaand start the timer
    TIM3->CR1 |= TIM_CR1_CEN;

}

void configure_gpios(void) {
    RCC->APB2ENR |= ( RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN );

    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; 
    GPIOC->CRH |= GPIO_CRH_MODE13_1;
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;

    GPIOA->CRL &= ~GPIO_CRL_MODE6_Msk;
    GPIOA->CRL |= GPIO_CRL_MODE6_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF6_Msk;
    GPIOA->CRL |= GPIO_CRL_CNF6_1;

    GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk;
    GPIOA->CRL |= GPIO_CRL_CNF0_1;
    GPIOA->ODR |= GPIO_ODR_ODR0;

    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->RTSR &= ~EXTI_RTSR_TR0;
    EXTI->FTSR |= EXTI_FTSR_TR0;
}

void configure_clock(void) {
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;  

    RCC->CFGR |= RCC_CFGR_PLLMULL16;

    RCC->CR |= RCC_CR_PLLON;
    while ( !(RCC->CR & RCC_CR_PLLRDY) ) {};

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ) {};

    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}
