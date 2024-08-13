/* toggling leds using spdt on-on switch */

#include "main.h"

volatile unsigned char which_led = 0;
volatile unsigned char should_toggle = 0;
volatile uint32_t input_a = 0;
volatile uint32_t input_b = 0;
volatile unsigned char toggled = 0;

int main() {
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // C13 LED for debbuging
    
    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; 
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;

    GPIOA->CRL &= ~GPIO_CRL_MODE0_Msk;
    GPIOA->CRL |= GPIO_CRL_MODE0_0;
    GPIOA->CRL |= GPIO_CRL_MODE0_1; // 0011
    GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk;

    GPIOB->CRL &= ~GPIO_CRL_MODE0_Msk;
    GPIOB->CRL |= GPIO_CRL_MODE0_0;
    GPIOB->CRL |= GPIO_CRL_MODE0_1; // 0011
    GPIOB->CRL &= ~GPIO_CRL_CNF0_Msk;

    GPIOB->CRH &= ~GPIO_CRH_MODE13_Msk & ~GPIO_CRH_MODE12_Msk;
    GPIOB->CRH &= ~GPIO_CRH_CNF13_Msk & ~GPIO_CRH_CNF12_Msk;
    GPIOB->CRH |= GPIO_CRH_CNF13_1 | GPIO_CRH_CNF12_1;
    GPIOB->ODR |= GPIO_ODR_ODR13 | GPIO_ODR_ODR12;

    while (1) {
        /* i really dont want to make it ""toggle"" unnecessary */
        input_a = GPIOB->IDR & GPIO_IDR_IDR12;
        input_b = GPIOB->IDR & GPIO_IDR_IDR13;

        should_toggle = ( ( ( input_a && !which_led ) || ( input_b && which_led ) ) ? 0 : 1 );

        if ( should_toggle )  {
            if ( input_a ) {
                GPIOB->BSRR = GPIO_BSRR_BR0;
                GPIOA->BSRR = GPIO_BSRR_BS0;
                which_led = 0;
            } else if ( input_b ) {
                GPIOB->BSRR = GPIO_BSRR_BS0;
                GPIOA->BSRR = GPIO_BSRR_BR0;
                which_led = 1;
                GPIOC->ODR ^= GPIO_ODR_ODR13;
            }
        } 


     /*   if ( !input_a ) should_toggle = 1;

        if (!input_a && toggled ) should_toggle = 0;
        if ( !input_b ) toggled = 0;

        if ( should_toggle ) {
            GPIOA->ODR ^= GPIO_ODR_ODR0;
            GPIOB->ODR ^= GPIO_ODR_ODR0; 
            GPIOC->ODR ^= GPIO_ODR_ODR13;
            toggled = 1;
        }


      should_toggle = 1;

        if ( input_a && !which_led ) should_toggle = 0;
        if ( input_b && which_led ) should_toggle = 0;

        //should_toggle = ( (  ) ? 1 : 0 );

        if ( should_toggle )  {
            if ( input_a ) {
                GPIOB->ODR &= ~GPIO_ODR_ODR0;
                GPIOA->ODR |= GPIO_ODR_ODR0;
                which_led = 0;
            } else if ( input_b ) {
                GPIOA->ODR &= ~GPIO_ODR_ODR0;
                GPIOB->ODR |= GPIO_ODR_ODR0;
                which_led = 1;
                GPIOC->ODR ^= GPIO_ODR_ODR13;
            }
        } 

               for (int i = 0; i < 100000; ++i) __asm__("nop");*/
    }
}
