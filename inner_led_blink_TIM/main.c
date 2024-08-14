/* blinking internal led & hardware clock interrupt, made with a big help of a vivonomicon tutorial */
#include "main.h"

volatile uint32_t core_clock_hz;

void start_timer(TIM_TypeDef *TIMx, uint16_t ms);
void stop_timer(TIM_TypeDef *TIMx);

int main() {

    /* just for pc13 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; 
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk; 

    /* "When the HSI is used as a PLL clock input, the maximum system clock frequency that can be      *  achieved is 64 MHz."*/

    /* setting up flash memory */
    
    /* flash access control register */
    /* enabling prefetch buffer, and a latency */

    /* "These options should be used in accordance with the Flash memory access time. The wait
     * states represent the ratio of the SYSCLK (system clock) period to the Flash memory access time:
     * zero wait state, if 0 < SYSCLK ≤ 24 MHz
     * one wait state, if 24 MHz < SYSCLK ≤ 48 MHz
     * two wait states, if 48 MHz < SYSCLK ≤ 72 MHz" */

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    /* The HSI clock signal is generated from an internal 8 MHz RC Oscillator and can be used
     * directly as a system clock or divided by 2 to be used as PLL input. */

    /* 64 Mhz = ( 8Mhz / 2 ) * 16 */

    /* changes in clock control register
     * by default, HSI is enabled, and trimmed to 8 Mhz, because HSTRIM is set to 10000 by default */

    RCC->CFGR |= RCC_CFGR_PLLMULL16; // multiply by 16
    
    /* turning on PLL and waiting for it to be ready */

    RCC->CR |= RCC_CR_PLLON;
    while ( !(RCC->CR & RCC_CR_PLLRDY) )  {};

    /* make a PLL a system clock source by setting system clock switch, and wait for it */

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ) {};

    core_clock_hz = 64000000;

    /* im gonna use general-purpose timer tim2 
     * TIM2 is connected via APB1 */

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* and interrupts, as in previous program */

    NVIC_SetPriority(TIM2_IRQn, 3);
    NVIC_EnableIRQ(TIM2_IRQn);

    start_timer(TIM2, 1000);

    while (1) {};
}

void start_timer(TIM_TypeDef *TIMx, uint16_t ms) {
    
    /* disabling counter */
    TIMx->CR1 &= ~TIM_CR1_CEN;

    /* reseting the timer */
    if ( TIMx == TIM2 ) {
        RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
        RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
    }

    /* prescaler */
    TIMx->PSC |= core_clock_hz / 1000 - 1; // the counter clock frequency CK_CNT is equal to fCK_OSC / (OSC[15:0] + 1)

    /* auto reload register */
    TIMx->ARR = ms;

    /* event generation register, update generation */
    TIMx->EGR |= TIM_EGR_UG;

    /* enable interrupt */
    TIMx->DIER |= TIM_DIER_UIE; // update interrupt enable

    /* finally, enable timer */
    TIMx->CR1 |= TIM_CR1_CEN; // counter enable 
}

void TIM2_IRQHandler(void) {
    // using status register and update interrupt flag
    if ( TIM2->SR & TIM_SR_UIF ) {
        TIM2->SR &= ~TIM_SR_UIF;
        GPIOC->ODR ^= GPIO_ODR_ODR13;
    }
}

void stop_timer(TIM_TypeDef *TIMx) {
    TIMx->CR1 &= ~TIM_CR1_CEN;
    TIMx->SR &= ~TIM_SR_UIF;
}
