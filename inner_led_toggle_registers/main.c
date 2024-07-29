#include "main.h"

int main(void) {

    /* enable clock for ports: A and C */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
 
    // mind its in output mode
    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; // clear mode bits
    GPIOC->CRH |= GPIO_CRH_MODE13_0;  // Set MODE13[0]-bit -> now is 01, in output mode
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk; // clear both CNF13[1:0] bits -> make it 00 push-pull
    
    /* A0 pin -> mind CRL! */
    //  pull-up  pull-down //
    GPIOA->CRL &= ~GPIO_CRL_MODE0_Msk;// zeroing (unnecesary), to make it input 00
    GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk; // same as up, zeroing
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // as the pin is in input mode, 10 means "input with pull-up/pull-down"
    GPIOA->ODR |= GPIO_ODR_ODR0; // just resetting output data register


    while(1) {
        uint32_t gpioa_input = GPIOA->IDR & GPIO_IDR_IDR0; // idr is read-only
        // checking the bit with AND (something AND 1)
        
       
        /* is pressed? */
        if (!gpioa_input) {
            uint32_t count = 10000;

            /* silly debouncer */
            while (count--) {
                __asm__("nop");
            }

            if(!(GPIOA->IDR  & GPIO_IDR_IDR0 )) {
                GPIOC->ODR ^= GPIO_ODR_ODR13; // finally, toggle
                while (!(GPIOA->IDR  & GPIO_IDR_IDR0));
            }
        }
     }
}
